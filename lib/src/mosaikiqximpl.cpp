/*
	* Copyright (c) Rohde & Schwarz
	*
	* Licensed under the Apache License, Version 2.0 (the "License");
	* you may not use this file except in compliance with the License.
	* You may obtain a copy of the License at
	*
	*     http://www.apache.org/licenses/LICENSE-2.0
	*
	* Unless required by applicable law or agreed to in writing, software
	* distributed under the License is distributed on an "AS IS" BASIS,
	* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	* See the License for the specific language governing permissions and
	* limitations under the License.
*/

#include <fcntl.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#include <string.h>
#endif
#include "common.h"

#include <sys/stat.h>
#include <sys/types.h>

#include <numeric>

#include "mosaikiqximpl.h"
#include <iqxformat/iqxfile.h>
#include "../iqxformat/src/iqbitconverter.h"

namespace rohdeschwarz
{
namespace mosaik
{
namespace dataimportexport
{

using namespace std;
using namespace IQW;

#ifdef _WIN32
/// seek for large files > 1TB
#define portable_lseek _lseeki64
#define portable_read _read
#else
#ifdef __APPLE__
#define portable_lseek lseek
#define portable_read read
#else
/// seek for large files > 1TB
#define portable_lseek lseek64
#define portable_read read
#endif
#endif


MosaikIqxImpl::MosaikIqxImpl(const std::string& filename)
  : m_filename(filename)
{
}

MosaikIqxImpl::~MosaikIqxImpl()
{
}

int MosaikIqxImpl::readOpen(std::vector<std::string>& arrayNames)
{
  if (m_piqx)
  {
    return 0;
  }
  try
  {
    arrayNames.clear();
    m_piqx = unique_ptr<IqxFile>(new IqxFile(m_filename));
    arrayNames = m_piqx->getStreamSources();
    // remove GPS if exists, because mosaik does only support IQ channels
    for (int i = arrayNames.size() - 1; i >= 0; i--)
    {
      //if (m_piqx->getStreamType(m_piqx->getStreamNo(arrayNames[i])) == IQX_STREAM_TYPE_GEOLOC)
      if (m_piqx->getStreamType(i) == IQX_STREAM_TYPE_GEOLOC)
      {
        arrayNames.erase(arrayNames.begin() + i);
      }
    }
    m_nofArrays = arrayNames.size();

    assembleIqxMetaData();
  }
  catch (const iqxformat_error&)
  {
    return 1;
  }
  catch (const exception&)
  {
    return ErrorCodes::InternalError;
  }
  catch (...)
  {
    return ErrorCodes::InternalError;
  }
  return 0;
}

int MosaikIqxImpl::getMetadata(std::vector<ChannelInfo>& channelInfos, map<string, string>& metadata) const
{
  if ((m_channelInfo.empty() || (m_metaData.empty())))
  {
    return ErrorCodes::InternalError;
  }
  metadata = m_metaData;
  channelInfos = m_channelInfo;

  return 0;
}

void MosaikIqxImpl::assembleIqxMetaData()
{
  const vector<string>& tags = m_piqx->getTags();
  const string& comments = m_piqx->getComment();
  const vector<string>& sources = m_piqx->getStreamSources();
  m_applicationName = m_piqx->getDescriptionName();

  //m_metaData.emplace("duration", to_string(m_piqx->getDuration()));
  string tagsstr = "";
  for (auto &t : tags)
  {
    if (tagsstr == "") tagsstr = t;
    else tagsstr = tagsstr + "," + t;
  }
  m_metaData.emplace("tags", tagsstr);
  m_metaData.emplace("comments", comments);
  string sourcesstr = "";
  for (auto &s : sources)
  {
    if (sourcesstr == "") sourcesstr = s;
    else sourcesstr = sourcesstr + "," + s;
  }
  m_metaData.emplace("sources", sourcesstr);
  m_metaData.emplace("ApplicationName", m_applicationName);

  size_t nStreams = m_nofArrays; // (GPS possible) m_piqx->getNumberOfStreams();
  for (size_t i = 0; i < nStreams; ++i)
  {
	  auto iqprops = m_piqx->getIqStreamParameters(m_piqx->getStreamSource(i));
	  //double clock, double centerFreq, size_t samples
      m_channelInfo.emplace_back(ChannelInfo(m_piqx->getStreamSource(i), iqprops.samplerate, iqprops.center_frequency, (size_t)m_piqx->getStreamNoOfSamples((uint64_t)i)));
	  if (iqprops.reflevel_valid) 
	  {
	    //m_metaData.emplace(m_piqx->getStreamSource(i) + " reflevel", to_string(iqprops.reflevel));
      m_metaData.emplace("Ch" + to_string(i + 1) + "_RefLevel[dBm]", to_string(iqprops.reflevel));
      float fullScale = (10.0 * log10(1.0 / 50.0) + 30.0);
      m_scaleFactor = pow(10, (iqprops.reflevel - fullScale) / 20.0);
      m_multiplicator = m_scaleFactor / INT16_MAX;
    }
	  if (iqprops.bandwidth_valid)
	  {
		  //m_metaData.emplace(m_piqx->getStreamSource(i) + " bandwith", to_string(iqprops.bandwith*1E6));
      m_metaData.emplace("Ch" + to_string(i+1) + "_MeasBandwidth[Hz]", to_string(iqprops.bandwidth));
	  }
	  if (iqprops.resolution_valid)
	  {
		  //m_metaData.emplace(m_piqx->getStreamSource(i) + " resolution", to_string(iqprops.resolution));
      m_metaData.emplace("Ch" + to_string(i + 1) + "_Resolution", to_string(iqprops.resolution));
	  }
    // assemble trigger info from iqx file
#if 0 // Syntax 
  key: "Ch1_MarkerInfo[XML]"
    value : “<ArrayOfEvents length = "2">
    <Event>
    <Type> IF Power Trigger < / Type>
    <Timestamp unit = "s"> 1530187710 < / Timestamp>
    <Index unit = "sample">0< / Index>
    < / Event>
    <Event>
    <Type> IF Power Trigger < / Type>
    <Timestamp unit = "s"> 1530187710 < / Timestamp>
    <Index unit = "sample">1000< / Index>
    < / Event>
    < / ArrayOfEvents>“

    <Type>: Damit ist der Marker type gemeint.Dieser entspricht dem ausgeschriebenen Typ :

       0x02 : TDT1(Time Domain Trigger 1)
         0x03 : HBT(Heartbeat Trigger)
         0x04 : EXT0(External Trigger 0)
         0x05 : SWT(Software Trigger)
         0x06 : FMT(Frequency Mask Trigger)
         0x07 : PMT(Probability Mask Trigger)
         0x08 : CFT(Correlation Trigger)
         0x09 : RFPT(RF Power Trigger)
         0x0A : IFPT(IF Power Trigger)
         0x0B : IQPT(IQ Power Trigger)
         0x0C : MODT(Modulation Trigger)
         0x0D : EXT1(External Trigger 1)
         0x0E : EXT2(External Trigger 2)
         0x0F : EXT3(External Trigger 3)
#endif

    vector<IqxTriggerEntry> triggers = m_piqx->getTriggers(i);
    if (triggers.size() > 0)
    {
      const size_t MaxTrig = 3600;
      size_t maxTrig = 0;
      string key = "Ch" + to_string(i + 1) + "_MarkerInfo[XML]";
      string value = "<ArrayOfEvents length = \"" + std::to_string(min(triggers.size(), MaxTrig)) + "\">\n";
      for (const auto &trigger : triggers)
      {
        maxTrig++;
        if (maxTrig > MaxTrig) break;
        value = value + "<Event>\n";
        string t = "";
        switch (trigger.type)
        {
        default: 
          t = "unknown";
          break;
        case 0x02:
          t = "Time Domain Trigger 1";
          break;
        case 0x03:
          t = "Heartbeat Trigger";
          break;
        case 0x04:
          t = "External Trigger 0";
          break;
        case 0x05:
          t = "Software Trigger";
          break;
        case 0x06:
          t = "Frequency Mask Trigger";
          break;
        case 0x07:
          t = "Probability Mask Trigger";
          break;
        case 0x08:
          t = "Correlation Trigger";
          break;
        case 0x09:
          t = "RF Power Trigger";
          break;
        case 0x0a:
          t = "IF Power Trigger";
          break;
        case 0x0b:
          t = "IQ Power Trigger";
          break;
        case 0x0c:
          t = "Modulation Trigger";
          break;
        case 0x0d:
          t = "External Trigger 1";
          break;
        case 0x0e:
          t = "External Trigger 2";
          break;
        case 0x0f:
          t = "External Trigger 3";
          break;

        }
        value = value + " <Type>" + t + "</Type>\n";
        value = value + " <Timestamp unit = \"s\">" + std::to_string(trigger.digiq_timestamp) + "</Timestamp>\n";

        value = value + " <Index unit = \"sample\">" + std::to_string(m_piqx->getSampleFromTimestamp(i, trigger.timestamp)) + "</Index>\n";
        value = value + "</Event>\n";

      }
      value = value + "</ArrayOfEvents>";
      m_metaData.emplace(key, value);
    }

  }

}

int MosaikIqxImpl::writeOpen(IqDataFormat format, size_t nofArrays, const string& applicationName, const string& comment,
                             const vector<ChannelInfo>& channelInfos, const map<string, std::string>* metadata)
{
  // other close, when writing 
  m_write = true; 
  m_format = format;
  m_nofArrays = nofArrays;
  m_applicationName = applicationName;
  m_comment = comment;
  m_channelInfo = channelInfos;
  m_metaData = *metadata;

  vector<pair<string, IqxStreamDescDataIQ>> descriptions;

  for (const auto &info : channelInfos)
  {
    // translate from DaiLib to IQX: generate stream descriptions from channel infos
    IqxStreamDescDataIQ description;
    string source = info.getChannelName();
    //sources.push_back(source);

	description.reflevel = 0;
	description.reflevel_valid = false;
	description.reflevel_variable = false;
	description.samplerate = info.getClockRate();
	description.samplerate_valid = true;
	description.samplerate_variable = false;
	description.bandwidth = 0;
	description.bandwidth_valid = false;
	description.bandwidth_variable = false;
	description.center_frequency = info.getFrequency();
	description.centfreq_valid = true;
	description.resolution = 16;
	description.resolution_valid = false;
   descriptions.push_back(make_pair(source,description));

    try
    {
      m_piqx = unique_ptr<IqxFile>(new IqxFile(m_filename, applicationName, comment, descriptions));
    }
    catch (const exception&)
    {
      return ErrorCodes::FileOpenError;
    }
    catch (...)
    {
      return ErrorCodes::FileOpenError;
    }

    return 0;
  }

  return 0;
}

int MosaikIqxImpl::close()
{
  m_piqx.reset();
  return 0;
}

time_t MosaikIqxImpl::getTimestamp() const
{
  iqx_timespec time;

  time = m_piqx->getStartTime();
  return time.tv_sec;
}

int64_t MosaikIqxImpl::getArraySize(const std::string& arrayName) const
{
  int64_t streamNo = m_piqx->getStreamNo(arrayName);
  if (streamNo >= 0)
  {
      return m_piqx->getStreamNoOfSamples(streamNo);
  }
  else
  {
      return 0;
  }
}

void MosaikIqxImpl::setTimestamp(const time_t /*timestamp*/)
{}

int MosaikIqxImpl::readArray(const std::string& arrayName, std::vector<float>& values, size_t nofValues, size_t offset)
{
  std::vector<double> dv(0);
  return readArrayAll(arrayName, values, dv, (float*)nullptr, (double*)nullptr, nofValues, offset, rFloatVector);
}

int MosaikIqxImpl::readArray(const std::string& arrayName, float* values, size_t nofValues, size_t offset)
{
    std::vector<float> fv;
    std::vector<double> dv;
    return readArrayAll(arrayName, fv, dv, values, (double*)nullptr, nofValues, offset, rFloatPointer);
}

int MosaikIqxImpl::readArray(const std::string& arrayName, std::vector<double>& values, size_t nofValues, size_t offset)
{
    std::vector<float> fv;
    return readArrayAll(arrayName, fv, values, (float*)nullptr, (double*)nullptr, nofValues, offset, rDoubleVector);
}

int MosaikIqxImpl::readArray(const std::string& arrayName, double* values, size_t nofValues, size_t offset)
{
    std::vector<float> fv;
    std::vector<double> dv;
    return readArrayAll(arrayName, fv, dv, (float*)nullptr, values, nofValues, offset, rDoublePointer);
}

int MosaikIqxImpl::readChannel(const std::string& channelName, std::vector<float>& values, size_t nofValues, size_t offset)
{
  std::vector<double> dv;
  return readChannelAll(channelName, values, dv, (float*)nullptr, (double*)nullptr , nofValues, offset, rFloatVector);
}

int MosaikIqxImpl::readChannel(const std::string& channelName, float* values, size_t nofValues, size_t offset)
{
    std::vector<float> fv;
    std::vector<double> dv;
    return readChannelAll(channelName, fv, dv, values, (double*)nullptr , nofValues, offset, rFloatPointer);
}

int MosaikIqxImpl::readChannel(const std::string& channelName, std::vector<double>& values, size_t nofValues, size_t offset)
{
    std::vector<float> fv;
    return readChannelAll(channelName, fv, values, (float*)nullptr, (double*)nullptr , nofValues, offset, rDoubleVector);
}

int MosaikIqxImpl::readChannel(const std::string& channelName, double* values, size_t nofValues, size_t offset)
{
    std::vector<float> fv;
    std::vector<double> dv;
    return readChannelAll(channelName, fv, dv, (float*)nullptr, values , nofValues, offset, rDoublePointer);
}

int MosaikIqxImpl::appendArrays(const std::vector<std::vector<float> >& iqdata)
{
  if (iqdata.size() % 2 != 0)
  {
    return ErrorCodes::InternalError;
  }

  for (size_t i = 0; i < iqdata.size() / 2; i++)
  {
    if (iqdata[i * 2].size() != iqdata[i * 2 + 1].size())
    {
      return ErrorCodes::InternalError;
    }
    writeIqFramesFromArrays(i, (float *)&(iqdata[i * 2][0]), (float *)&(iqdata[i * 2 + 1][0]), NULL, NULL, iqdata[i * 2].size(), wFloat);
  }
  return 0;
}

int MosaikIqxImpl::appendArrays(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
{
  if (sizes.size() % 2 != 0)
  {
    return ErrorCodes::InternalError;
  }
  for (size_t i = 0; i < sizes.size() / 2; i++)
  {
    if (sizes[i * 2] != sizes[i * 2 + 1])
    {
      return ErrorCodes::InternalError;
    }
    writeIqFramesFromArrays(i, (float *)(iqdata[i * 2]), (float *)(iqdata[i * 2 + 1]), NULL, NULL, sizes[i * 2], wFloat);
  }
  return 0;
}

int MosaikIqxImpl::appendArrays(const std::vector<std::vector<double> >& iqdata)
{
  if (iqdata.size() % 2 != 0)
  {
    return ErrorCodes::InternalError;
  }

  for (size_t i = 0; i < iqdata.size() / 2; i++)
  {
    if (iqdata[i * 2].size() != iqdata[i * 2 + 1].size())
    {
      return ErrorCodes::InternalError;
    }
    writeIqFramesFromArrays(i, NULL, NULL, (double *)&(iqdata[i * 2][0]), (double *)&(iqdata[i * 2 + 1][0]), iqdata[i * 2].size(), wDouble);
  }
  return 0;
}

int MosaikIqxImpl::appendArrays(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
{
  if (sizes.size() % 2 != 0)
  {
    return ErrorCodes::InternalError;
  }
  for (size_t i = 0; i < sizes.size() / 2; i++)
  {
    if (sizes[i * 2] != sizes[i * 2 + 1])
    {
      return ErrorCodes::InternalError;
    }
    writeIqFramesFromArrays(i, NULL, NULL, (double *)(iqdata[i * 2]), (double *)(iqdata[i * 2 + 1]), sizes[i * 2], wDouble);
  }
  return 0;
}

int MosaikIqxImpl::appendChannels(const std::vector<std::vector<float> >& iqdata)
{
  for (size_t i = 0; i < iqdata.size(); i++)
  {
    writeIqFramesFromChannel(i, (float *) iqdata[i].data(), NULL, iqdata[i].size()/2, wFloat);
  }
  return 0;
}

int MosaikIqxImpl::appendChannels(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
{
  for (size_t i = 0; i < iqdata.size(); i++)
  {
    writeIqFramesFromChannel(i, (float *)(iqdata[i]), NULL, sizes[i]/2, wFloat);
  }
  return 0;
}

int MosaikIqxImpl::appendChannels(const std::vector<std::vector<double> >& iqdata)
{
  for (size_t i = 0; i < iqdata.size(); i++)
  {
    writeIqFramesFromChannel(i, NULL, (double *)iqdata[i].data(), iqdata[i].size()/2, wDouble);
  }
  return 0;
}

int MosaikIqxImpl::appendChannels(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
{
  for (size_t i = 0; i < iqdata.size(); i++)
  {
    writeIqFramesFromChannel(i, NULL, (double *)(iqdata[i]), sizes[i]/2, wDouble);
  }
  return 0;
}

/*
* IQX:       IQIQIQIQ    IQIQIQIQ    IQIQIQIQ
*                           \ \ \    / /
* Target:                     IIIIIIII     or    QQQQQQQ
*/
int MosaikIqxImpl::readArrayAll(const std::string& arrayName, std::vector<float>& vfValues, std::vector<double>& vdValues, float* fValues, double* dValues, size_t nofValues, size_t offset, rType rw)
{
	float *fPtr = fValues;
	double *dPtr = dValues;
	switch (rw)
	{
	case rFloatVector:
		vfValues.clear();
    vfValues.reserve(nofValues);
		break;
	case rDoubleVector:
		vdValues.clear();
    vdValues.reserve(nofValues);
		break;
        default:
            break;
	}
	auto streamNo = m_piqx->getStreamNo(arrayName);
	if (m_piqx->getStreamNoOfSamples(streamNo) < (offset + nofValues))
	{
		return ErrorCodes::InternalError;
	}
   map<size_t, IqxStreamType> streamtypes = m_piqx->getStreamTypes();


  bool isI = arrayName.find("_I", arrayName.size() - 2) != string::npos;

  int64_t actPair = offset;
  // nofValues in samples, we copy only I or Q
  int64_t pairCount = nofValues;
  while (pairCount > 0)
  {
    // use cue entries to calculate the frame(s) to read
    auto cue = m_piqx->getCueEntry(streamNo, m_piqx->getTimestampFromSample(streamNo, actPair));
    // read the preamble
    portable_lseek(*m_piqx, cue.offset, SEEK_SET);
    IqxPreamble preamble;
    // read the preamble
    if (portable_read(*m_piqx, &preamble, sizeof(preamble)) < static_cast<ssize_t>(sizeof(preamble)))
    {
      return ErrorCodes::InternalError;
    }
    if (memcmp(&preamble.sync, iqxsync, sizeof(iqxsync)))
    {
      return ErrorCodes::InternalError;
    }

	 //============================================================================
	 // 12 Bit:
	 // from iqbitconverter we need
	 // - conv12to16_dstsize
	 // - conv12to16
	 /*
    12 Bit Frame
       --32 Byte- DIGIQ_WORD_SIZE ----------------- 256 Bit ----------- - ==> 10 IQ Samples ist kleinste Einheit
       -- 16 Byte-------------------- 16 Byte--------------------------

       -- 5 * 2 * 12 Bit + 8 Bit reserved | 5 * 2 * 12 Bit + 8 Bit reserved---- -
       = > IQIQIQIQIQ R                   | IQIQIQIQIQ R
       = > 120 Bit + 8                    | 120 Bit + 8

	 */

    // calculate the first pair in the frame
    int64_t firstPairInFrame = m_piqx->getSampleFromTimestamp(streamNo, cue.timestamp);

    // skip header
    portable_lseek(*m_piqx, preamble.headsize, SEEK_CUR);

    //                                 ==================== 16 Bit IQ ========================================================      
    //                                 Frame iqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiq
    //                                       |                     values  ---  preamble.datasize / 2  ---
    //                                       |                     samples ---  preamble.datasize / 4  ---
    //                                       firstPairInFrame                                                          
    //                                                      actPair  
    // read pairs if you need more
    // first pair to read
    // two values are one pair
    uint32_t resolution = (streamtypes.at(preamble.streamnum) == IQX_STREAM_TYPE_IQDATA16) ? 16 : 12;

    int64_t PairsInFrame = preamble.datasize / 4;
    if (resolution == 12) PairsInFrame = IqBitConverter::conv12to16_dstsize(preamble.datasize) / 4;
    int64_t lastPairInFrame = firstPairInFrame + PairsInFrame - 1;
    int64_t firstUseablePairInFrame = actPair;
    int64_t offsetOfFirstUseablePairInFrame = actPair - firstPairInFrame;
    int64_t lastUseablePairInFrame = min(firstUseablePairInFrame + pairCount - 1, lastPairInFrame);
    int64_t useablePairsInFrame = lastUseablePairInFrame - actPair + 1;
    //size_t useableValuesInFrame = useablePairsInFrame * 2;

    // leading and trailing pairs for 12 Bit data (aligned to 10 Samples (32 Bit))
    size_t leadingPairs = 0;
    size_t trailingPairs = 0;
    if (resolution == 12)
    {
       leadingPairs = offsetOfFirstUseablePairInFrame % 10;
       size_t modTrailingPairs = (offsetOfFirstUseablePairInFrame + pairCount) % 10;
       if (modTrailingPairs != 0) 
       { 
          trailingPairs = 10 - modTrailingPairs;
       }
    }
    // calculate the new actPair for the next while iteration 
    actPair += useablePairsInFrame;


    const size_t size = (useablePairsInFrame + leadingPairs + trailingPairs) * 2;
    const size_t leadingValues = leadingPairs * 2;

    // buffer for 16 Bit data
    auto data = unique_ptr<int16_t[]>(new int16_t[size]);

    if (resolution == 12)
    {
       // auf DIGIQ_WORD_SIZE aufgerundet lesen und dann in data konvertieren
       const size_t size12 = (useablePairsInFrame + leadingPairs + trailingPairs) / 10 * DIGIQ_WORD_SIZE;
       // offset 
       portable_lseek(*m_piqx, (offsetOfFirstUseablePairInFrame - leadingPairs) / 10 * DIGIQ_WORD_SIZE, SEEK_CUR);
       auto data12 = unique_ptr<uint8_t[]>(new uint8_t[size12]);
       if (portable_read(*m_piqx, data12.get(), static_cast<unsigned int>(size12)) < static_cast<int>(size12))
       {
          return ErrorCodes::InternalError;
       }
       if (IqBitConverter::conv12to16(data12.get(), data.get(), size12) == -1)
       {
          return ErrorCodes::InternalError;
       }
    }
    else
    {
       // seek to First Pair which is 2 * int16 so mult by 4
       portable_lseek(*m_piqx, offsetOfFirstUseablePairInFrame * 4, SEEK_CUR);
       // read only as much data of the data part of the stream as we need
       if (portable_read(*m_piqx, data.get(), static_cast<unsigned int>(size * 2)) < static_cast<int>(size * 2))
       {
          return ErrorCodes::InternalError;
       }
    }

    int64_t firstValue = isI ? leadingValues : leadingValues + 1;
    int64_t lastValue = firstValue + useablePairsInFrame * 2;
    for (int64_t value = firstValue; value <= lastValue; value = value + 2)
    {
      // convert and copy
      switch (rw)
      {
      case rFloatVector:
      {
        float f = data[value];
        f = f * m_multiplicator;
        vfValues.push_back(f);
        break;
      }
      case rDoubleVector:
      {
        double d = data[value];
        d = d * m_multiplicator;
        vdValues.push_back(d);
        break;
      }
      case rFloatPointer:
      {
        float f = data[value];
        f = f  * m_multiplicator;
        *fPtr = f;
        fPtr++;
        break;
      }
      case rDoublePointer:
      {
        double d = data[value];
        d = d  * m_multiplicator;
        *dPtr = d;
        dPtr++;
        break;
      }
      }
      // convert and copy end
      pairCount--;
      if (pairCount == 0)
      {
        return 0;
      }
    }
  }
	return 0;
}

int MosaikIqxImpl::readChannelAll(const std::string& channelName, std::vector<float>& vfValues, std::vector<double>& vdValues, float* fValues, double* dValues, size_t nofValues, size_t offset, rType rw)
{
  float *fPtr = fValues;
  double *dPtr = dValues;
  auto streamNo = m_piqx->getStreamNo(channelName);
  if (m_piqx->getStreamNoOfSamples(streamNo) < (offset + nofValues))
  {
    return ErrorCodes::InternalError;
  }
  if (rw == rFloatVector)
  {
    vfValues.clear();
    if (vfValues.max_size() < nofValues * 2)
    {
      return ErrorCodes::InternalError;
    }
    vfValues.reserve(nofValues * 2);
  }
  if (rw == rDoubleVector)
  {
    vdValues.clear();
    if (vdValues.max_size() < nofValues * 2)
    {
      return ErrorCodes::InternalError;
    }
    vdValues.reserve(nofValues * 2);
  }

  map<size_t, IqxStreamType> streamtypes = m_piqx->getStreamTypes();

  int64_t actPair = offset;
  // nofValues in samples, each sample has I and Q, so mult by 2 
  int64_t pairCount = nofValues;
  while (pairCount > 0)
  {
     // use cue entries to calculate the frame(s) to read
     auto cue = m_piqx->getCueEntry(streamNo, m_piqx->getTimestampFromSample(streamNo, actPair));
     // read the preamble
     portable_lseek(*m_piqx, cue.offset, SEEK_SET);
     IqxPreamble preamble;
     // read the preamble
     if (portable_read(*m_piqx, &preamble, sizeof(preamble)) < static_cast<ssize_t>(sizeof(preamble)))
     {
        return ErrorCodes::InternalError;
     }
     if (memcmp(&preamble.sync, iqxsync, sizeof(iqxsync)))
     {
        return ErrorCodes::InternalError;
     }

     // calculate the first pair in the frame
     int64_t firstPairInFrame = m_piqx->getSampleFromTimestamp(streamNo, cue.timestamp);
     // skip header
     portable_lseek(*m_piqx, preamble.headsize, SEEK_CUR);
     // read pairs if you need more
     // first pair to read
     // two values are one pair
     uint32_t resolution = (streamtypes.at(preamble.streamnum) == IQX_STREAM_TYPE_IQDATA16) ? 16 : 12;

     int64_t PairsInFrame = preamble.datasize / 4;
     if (resolution == 12) PairsInFrame = IqBitConverter::conv12to16_dstsize(preamble.datasize) / 4;
     int64_t lastPairInFrame = firstPairInFrame + PairsInFrame - 1;
     int64_t firstUseablePairInFrame = actPair;
     int64_t offsetOfFirstUseablePairInFrame = actPair - firstPairInFrame;
     int64_t lastUseablePairInFrame = min(firstUseablePairInFrame + pairCount - 1, lastPairInFrame);
     int64_t useablePairsInFrame = lastUseablePairInFrame - actPair + 1;
     size_t useableValuesInFrame = useablePairsInFrame * 2;

     // leading and trailing pairs for 12 Bit data (aligned to 10 Samples (32 Bit))
     size_t leadingPairs = 0;
     size_t trailingPairs = 0;
     if (resolution == 12)
     {
        leadingPairs = offsetOfFirstUseablePairInFrame % 10;
        size_t modTrailingPairs = (offsetOfFirstUseablePairInFrame + pairCount) % 10;
        if (modTrailingPairs != 0)
        {
           trailingPairs = 10 - modTrailingPairs;
        }
     }
     // calculate the new actPair for the next while iteration 
     actPair += useablePairsInFrame;


     const size_t size = (useablePairsInFrame + leadingPairs + trailingPairs) * 2;
     const size_t leadingValues = leadingPairs * 2;

     // buffer for 16 Bit data
     auto data = unique_ptr<int16_t[]>(new int16_t[size]);

     if (resolution == 12)
     {
        // auf DIGIQ_WORD_SIZE aufgerundet lesen und dann in data konvertieren
        const size_t size12 = (useablePairsInFrame + leadingPairs + trailingPairs) / 10 * DIGIQ_WORD_SIZE;
        // offset 
        portable_lseek(*m_piqx, (offsetOfFirstUseablePairInFrame - leadingPairs) / 10 * DIGIQ_WORD_SIZE, SEEK_CUR);
        auto data12 = unique_ptr<uint8_t[]>(new uint8_t[size12]);
        if (portable_read(*m_piqx, data12.get(), static_cast<unsigned int>(size12)) < static_cast<int>(size12))
        {
           return ErrorCodes::InternalError;
        }
        if (IqBitConverter::conv12to16(data12.get(), data.get(), size12) == -1)
        {
           return ErrorCodes::InternalError;
        }
     }
     else
     {
        // seek to First Pair which is 2 * int16 so mult by 4
        portable_lseek(*m_piqx, offsetOfFirstUseablePairInFrame * 4, SEEK_CUR);
        // read only as much data of the data part of the stream as we need
        if (portable_read(*m_piqx, data.get(), static_cast<unsigned int>(size * 2)) < static_cast<int>(size * 2))
        {
           return ErrorCodes::InternalError;
        }
     }

     int64_t firstValue = leadingValues;
     int64_t lastValue = firstValue + useableValuesInFrame -1;
     for (int64_t value = firstValue; value <= lastValue; value++)
     {
        // convert and copy
        switch (rw)
        {
        case rFloatVector:
        {
           float f = data[value];
           f = f  * m_multiplicator;
           vfValues.push_back(f);
           break;
        }
        case rDoubleVector:
        {
           double d = data[value];
           d = d  * m_multiplicator;
           vdValues.push_back(d);
           break;
        }
        case rFloatPointer:
        {
           float f = data[value];
           f = f  * m_multiplicator;
           *fPtr = f;
           fPtr++;
           break;
        }
        case rDoublePointer:
        {
           double d = data[value];
           d = d  * m_multiplicator;
           *dPtr = d;
           dPtr++;
           break;
        }
        } // end switch
        // convert and copy end
     } // end for
     pairCount -= useablePairsInFrame;
     if (pairCount == 0)
     {
        return 0;
     }
  }
  return 0;
}

int MosaikIqxImpl::writeIqFramesFromArrays(int64_t streamno, float * fArrayI, float * fArrayQ, double * dArrayI, double * dArrayQ, int64_t samples, wType w)
{
	// each frame has up to 4096*4096*4 samples                       for tests with more than one frame 
	const int64_t MaxSamples = static_cast<int64_t>(4096 * 4096 * 4 /* 512*4 */);
	int64_t samplesLeft = samples;
	int64_t samplesProcessed = 0;
	while (samplesLeft > 0)
	{
    vector<int16_t> iqData;
    int64_t samplesToDo = samplesLeft;
		if (samplesToDo > MaxSamples)
		{
			samplesToDo = MaxSamples;
		}
    iqData.reserve(samplesToDo * 2);
		if (w == wFloat)
		{
			for (int64_t i = samplesProcessed; i < samplesProcessed + samplesToDo; i++)
			{
				iqData.push_back((int16_t)(*(fArrayI + i) * INT16_MAX));
				iqData.push_back((int16_t)(*(fArrayQ + i) * INT16_MAX));
			}
		}
		else
		{
			for (int64_t i = samplesProcessed; i < samplesProcessed + samplesToDo; i++)
			{
				iqData.push_back((int16_t)(*(dArrayI + i) * INT16_MAX));
				iqData.push_back((int16_t)(*(dArrayQ + i) * INT16_MAX));
			}
		}
		int seq = m_piqx->getSequenceNo(streamno);
		m_piqx->writeDataFrame(streamno, seq, iqData);
		m_piqx->setSequenceNo(streamno, seq + 1);

		samplesLeft -= samplesToDo;
		samplesProcessed += samplesToDo;
	}
	return 0;
}

int MosaikIqxImpl::writeIqFramesFromChannel(int64_t streamno, float * fChannel, double * dChannel, int64_t samples, wType w)
{
	// each frame has up to 4096*4096 symbols
	const int64_t MaxSamples = static_cast<int64_t>(4096 * 4096 * 4);
	int64_t samplesLeft = samples;
	int64_t samplesProcessed = 0;
	while (samplesLeft > 0)
	{
    vector<int16_t> iqData;
    int64_t samplesToDo = samplesLeft;
		if (samplesToDo > MaxSamples)
		{
			samplesToDo = MaxSamples;
		}
		iqData.reserve(samplesToDo * 2);
		if (w == wFloat)
		{
			for (int64_t i = samplesProcessed; i < samplesProcessed + samplesToDo; i++)
			{
				iqData.push_back((int16_t)(*fChannel++ * INT16_MAX));
				iqData.push_back((int16_t)(*fChannel++ * INT16_MAX));
			}
		}
		else
		{
			for (int64_t i = samplesProcessed; i < samplesProcessed + samplesToDo; i++)
			{
				iqData.push_back((int16_t)(*dChannel++ * INT16_MAX));
				iqData.push_back((int16_t)(*dChannel++ * INT16_MAX));
			}
		}

		int seq = m_piqx->getSequenceNo(streamno);
		m_piqx->writeDataFrame(streamno, seq, iqData);
		m_piqx->setSequenceNo(streamno, seq + 1);

		// iqData has a size of 2 * samples (I+Q)
		samplesLeft -= samplesToDo;
		samplesProcessed += samplesToDo;
	}
	return 0;
}


} // namespace
} // namespace
} // namespace
