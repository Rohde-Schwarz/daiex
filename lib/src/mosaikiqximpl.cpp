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
  m_metaData.emplace("tags", accumulate(tags.begin(), tags.end(), string("")));
  m_metaData.emplace("comments", comments);
  m_metaData.emplace("sources", accumulate(sources.begin(), sources.end(), string("")));
  m_metaData.emplace("ApplicationName", m_applicationName);

  size_t nStreams = m_piqx->getNumberOfStreams();
  for (size_t i = 0; i < nStreams; ++i)
  {
	  auto iqprops = m_piqx->getIqStreamParameters(m_piqx->getStreamSource(i));
	  //double clock, double centerFreq, size_t samples
      m_channelInfo.emplace_back(ChannelInfo(m_piqx->getStreamSource(i), iqprops.samplerate, iqprops.center_frequency, (size_t)m_piqx->getStreamNoOfSamples((uint64_t)i)));
	  if (iqprops.reflevel_valid) 
	  {
	    //m_metaData.emplace(m_piqx->getStreamSource(i) + " reflevel", to_string(iqprops.reflevel));
      m_metaData.emplace("Ch" + to_string(i + 1) + "_RefLevel[dBm]", to_string(iqprops.reflevel));
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
      string key = "Ch" + to_string(i + 1) + "_MarkerInfo[XML]";
      string value = "<ArrayOfEvents length = \"" + std::to_string(triggers.size()) + "\">\n";
      for (const auto &trigger : triggers)
      {
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

  vector<pair<string, IqxStreamDescDataIQ16>> descriptions;

  for (const auto &info : channelInfos)
  {
    // translate from DaiLib to IQX: generate stream descriptions from channel infos
    IqxStreamDescDataIQ16 description;
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
	description.resolution = 0;
	description.resolution_valid = false;
    descriptions.push_back(make_pair(source,description));

    try
    {
      m_piqx = unique_ptr<IqxFile>(new IqxFile(m_filename, applicationName, comment, descriptions));
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

  bool isI = arrayName.find("_I", arrayName.size() - 2) != string::npos;

#if 1 // new implementation with cue frame
  int64_t actSample = offset;
  // nofValues in samples, we copy only I or Q
  size_t count = nofValues;
  while (count > 0)
  {
    // use cue entries to calculate the frame(s) to read
    auto cue = m_piqx->getCueEntry(streamNo, m_piqx->getTimestampFromSample(streamNo, actSample));
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

    // calculate the first sample in the frame
    int64_t frameSample = m_piqx->getSampleFromTimestamp(streamNo, cue.timestamp);

    // skip header
    portable_lseek(*m_piqx, preamble.headsize, SEEK_CUR);

    //                                 Frame iqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiqiq
    //                                       |                     values  ---  preamble.datasize / 2  ---
    //                                       |                     samples ---  preamble.datasize / 4  ---
    //                                       frameSample                                                          
    //                                                      actSample  
    // read samples if you need more
    // first sample to read
    // two values are one sample
    int64_t firstValue = (actSample - frameSample) * 2;
    int64_t lastValue = min(firstValue + count * 2, preamble.datasize / 2) - 1;
    // calculate the new actSample for the next while iteration 
    actSample += (lastValue - firstValue + 1) / 2;

    // seek to FirstValue which is int16 so mult by 2                   
    portable_lseek(*m_piqx, firstValue * 2, SEEK_CUR);
    const size_t size = lastValue - firstValue + 1;
    // read data
    auto data = unique_ptr<int16_t[]>(new int16_t[size]);
    // read only as much data of the data part of the stream as we need
    if (portable_read(*m_piqx, data.get(), static_cast<unsigned int>(size*2)) < static_cast<int>(size*2))
    {
      return ErrorCodes::InternalError;
    }

    for (int64_t value = isI?0:1  /*firstValue*/; value < size  /*lastValue*/; value = value + 2)
    {
      // convert and copy
      switch (rw)
      {
      case rFloatVector:
      {
        float f = data[value];
        f = f / INT16_MAX;
        vfValues.push_back(f);
        break;
      }
      case rDoubleVector:
      {
        double d = data[value];
        d = d / INT16_MAX;
        vdValues.push_back(d);
        break;
      }
      case rFloatPointer:
      {
        float f = data[value];
        f = f / INT16_MAX;
        *fPtr = f;
        fPtr++;
        break;
      }
      case rDoublePointer:
      {
        double d = data[value];
        d = d / INT16_MAX;
        *dPtr = d;
        dPtr++;
        break;
      }
      }
      // convert and copy end
      count--;
      if (count == 0)
      {
        return 0;
      }
    }
  }
#else // old implementation
	// readBegin8 is the data byte which is to read first:
	// if I have an offset of 1000 symbols, then there are 1000 IQ symbols to be skipped.
	// readBegin is the 0 + 1000 * 4. If the QArray has to be read, then if have to add 2 bytes to readBegin
	size_t readBegin8 = offset * 4;
	if (!isI)
	{
		readBegin8 += 2;
	}
	//                                            IQ = 4 Byte
	size_t readEnd8 = readBegin8 + (nofValues - 1) * 4;
	size_t frameDataBegin8 = 0;
	size_t frameDataEnd8 = 0;
	bool ready = false;
	bool first = true;
	portable_lseek(*m_piqx, m_piqx->getPayloadOffset(), SEEK_SET);
	while (!ready)
	{
		IqxPreamble preamble;
		if (portable_read(*m_piqx, &preamble, sizeof(preamble)) < static_cast<ssize_t>(sizeof(preamble)))
		{
			return ErrorCodes::InternalError;
		}

		if (memcmp(&preamble.sync, iqxsync, sizeof(iqxsync)))
		{
			return ErrorCodes::InternalError;
		}

		switch (preamble.frametype)
		{
		case IQX_FRAME_TYPE_IQDATA:
		{
			// calculate frames/stream symbols/stream
			if (static_cast<size_t>(preamble.streamnum) == streamNo)
			{
				if (first)
				{
					first = false;
				}
				else
				{
					frameDataBegin8 = frameDataEnd8 + 1;
				}
				frameDataEnd8 = frameDataBegin8 + preamble.datasize - 1;
				if (readBegin8 >= frameDataBegin8 && readBegin8 <= frameDataEnd8)
				{
					// skip header
					portable_lseek(*m_piqx, preamble.headsize, SEEK_CUR);
					// read data
					auto data = unique_ptr<int16_t[]>(new int16_t[preamble.datasize / 2]);
					if (portable_read(*m_piqx, data.get(), static_cast<unsigned int>(preamble.datasize)) < static_cast<int>(preamble.datasize))
					{
						return ErrorCodes::InternalError;
					}

					switch (rw)
					{
					case rFloatVector:
					{
						// now copy every 2nd int16 and convert it to float32
						while (readBegin8 <= frameDataEnd8 && readBegin8 <= readEnd8)
						{
							float f = data[(readBegin8 - frameDataBegin8) / 2];
							f = f / INT16_MAX;
							vfValues.push_back(f);
							readBegin8 += 4;
						}
						break;
					}
					case rDoubleVector:
					{
						// now copy every 2nd int16 and convert it to float32
						while (readBegin8 <= frameDataEnd8 && readBegin8 <= readEnd8)
						{
							double d = data[(readBegin8 - frameDataBegin8) / 2];
							d = d / INT16_MAX;
							vdValues.push_back(d);
							readBegin8 += 4;
						}
						break;
					}
					case rFloatPointer:
					{
						while (readBegin8 <= frameDataEnd8 && readBegin8 <= readEnd8)
						{
							float f = data[(readBegin8 - frameDataBegin8) / 2];
							f = f / INT16_MAX;
							*fPtr = f;
							fPtr++;
							readBegin8 += 4;
						}
						break;
					}
					case rDoublePointer:
					{
						while (readBegin8 <= frameDataEnd8 && readBegin8 <= readEnd8)
						{
							double d = data[(readBegin8 - frameDataBegin8) / 2];
							d = d / INT16_MAX;
							*dPtr = d;
							dPtr++;
							readBegin8 += 4;
						}
						break;
					}
					}
					if (readBegin8 > readEnd8)
					{
						return 0;
					}
					else
					{
						// skip rest of frame: skip the tail
						size_t tailSize = preamble.framesize - sizeof(preamble) - preamble.headsize - preamble.datasize;
						portable_lseek(*m_piqx, tailSize, SEEK_CUR);
					}
				}
				else
				{
					// skip rest of frame
					portable_lseek(*m_piqx, preamble.framesize - sizeof(preamble), SEEK_CUR);
				}
			}
			else
			{
				// skip rest of frame
				portable_lseek(*m_piqx, preamble.framesize - sizeof(preamble), SEEK_CUR);
			}

			break;
		}
		case IQX_FRAME_TYPE_PAYLOADEND:
		{
			// some data is missing
			return ErrorCodes::InternalError;
		}
		default:
		{
			// skip rest of frame
			portable_lseek(*m_piqx, preamble.framesize - sizeof(preamble), SEEK_CUR);
		}
		break;
		}
	}
#endif
	return 0;
}

#if 1 // new implementation
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

  int64_t actSample = offset;
  // nofValues in samples, each sample has I and Q, so mult by 2 
  size_t count = nofValues*2;
  while (count > 0)
  {
    // use cue entries to calculate the frame(s) to read
    auto cue = m_piqx->getCueEntry(streamNo, m_piqx->getTimestampFromSample(streamNo, actSample));
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

    // calculate the first sample in the frame
    int64_t frameSample = m_piqx->getSampleFromTimestamp(streamNo, cue.timestamp);

    // skip header
    portable_lseek(*m_piqx, preamble.headsize, SEEK_CUR);

    // read samples if you need more
    // first sample to read
    int64_t firstValue = (actSample - frameSample) * 2;
    int64_t lastValue = min(firstValue + count * 2, preamble.datasize / 2) - 1;
    // calculate the new actSample for the next while iteration 
    actSample += (lastValue - firstValue + 1) / 2;
    
    // seek to FirstValue which is int16 so mult by 2                   
    portable_lseek(*m_piqx, firstValue * 2, SEEK_CUR);
    const size_t size = lastValue - firstValue + 1;
    // read data
    auto data = unique_ptr<int16_t[]>(new int16_t[size]);
    // read only as much data of the data part of the stream as we need
    if (portable_read(*m_piqx, data.get(), static_cast<unsigned int>(size*2)) < static_cast<int>(size*2))
    {
      return ErrorCodes::InternalError;
    }
    
    for (int64_t value = 0; value < size; value++)
    {
      // convert and copy
      switch (rw)
      {
        case rFloatVector:
        {
          float f = data[value];
          f = f / INT16_MAX;
          vfValues.push_back(f);
          break;
        }
        case rDoubleVector:
        {
          double d = data[value];
          d = d / INT16_MAX;
          vdValues.push_back(d);
          break;
        }
        case rFloatPointer:
        {
          float f = data[value];
          f = f / INT16_MAX;
          *fPtr = f;
          fPtr++;
          break;
        }
        case rDoublePointer:
        {
          double d = data[value];
          d = d / INT16_MAX;
          *dPtr = d;
          dPtr++;
          break;
        }
      }
      // convert and copy end
      count--;
      if (count == 0)
      {
        return 0;
      }
    }
  }
  return 0;
}
#else // old implementation
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

	size_t readBegin8 = offset * 4;

	//                             Q              IQ = 4 Byte
	size_t readEnd8 = readBegin8 + 2 + (nofValues - 1) * 4;
	size_t frameDataBegin8 = 0;
	size_t frameDataEnd8 = 0;
	bool ready = false;
	bool first = true;
	portable_lseek(*m_piqx, m_piqx->getPayloadOffset(), SEEK_SET);
	while (!ready)
	{
		IqxPreamble preamble;
		if (portable_read(*m_piqx, &preamble, sizeof(preamble)) < static_cast<ssize_t>(sizeof(preamble)))
		{
			return ErrorCodes::InternalError;
		}

		if (memcmp(&preamble.sync, iqxsync, sizeof(iqxsync)))
		{
			return ErrorCodes::InternalError;
		}

		switch (preamble.frametype)
		{
		case IQX_FRAME_TYPE_IQDATA:
		{
			// calculate frames/stream samples/stream
			if (static_cast<size_t>(preamble.streamnum) == streamNo)
			{
				if (first)
				{
					first = false;
				}
				else
				{
					frameDataBegin8 = frameDataEnd8 + 1;
				}
				frameDataEnd8 = frameDataBegin8 + preamble.datasize - 1;
				if (readBegin8 >= frameDataBegin8 && readBegin8 <= frameDataEnd8)
				{
					// skip header
					portable_lseek(*m_piqx, preamble.headsize, SEEK_CUR);
					// read data
					auto data = unique_ptr<int16_t[]>(new int16_t[preamble.datasize / 2]);
					if (portable_read(*m_piqx, data.get(), static_cast<unsigned int>(preamble.datasize)) < static_cast<int>(preamble.datasize))
					{
						return ErrorCodes::InternalError;
					}

					switch (rw)
					{
					case rFloatVector:
					{
						// now copy every int16 and convert it to float32
						while (readBegin8 <= frameDataEnd8 && readBegin8 <= readEnd8)
						{
							float f = data[(readBegin8 - frameDataBegin8) / 2];
							f = f / INT16_MAX;
							vfValues.push_back(f);
							readBegin8 += 2;
						}
						break;
					}
					case rDoubleVector:
					{
						// now copy every int16 and convert it to double
						while (readBegin8 <= frameDataEnd8 && readBegin8 <= readEnd8)
						{
							double d = data[(readBegin8 - frameDataBegin8) / 2];
							d = d / INT16_MAX;
							vdValues.push_back(d);
							readBegin8 += 2;
						}
						break;
					}
					case rFloatPointer:
					{
						while (readBegin8 <= frameDataEnd8 && readBegin8 <= readEnd8)
						{
							float f = data[(readBegin8 - frameDataBegin8) / 2];
							f = f / INT16_MAX;
							*fPtr = f;
							fPtr++;
							readBegin8 += 2;
						}
						break;
					}
					case rDoublePointer:
					{
						// now copy every int16 and convert it to double
						while (readBegin8 <= frameDataEnd8 && readBegin8 <= readEnd8)
						{
							double d = data[(readBegin8 - frameDataBegin8) / 2];
							d = d / INT16_MAX;
							*dPtr = d;
							dPtr++;
							readBegin8 += 2;
						}
						break;
					}
					}

					if (readBegin8 > readEnd8)
					{
						return 0;
					}
					else
					{
						// skip rest of frame: skip the tail
						size_t tailSize = preamble.framesize - sizeof(preamble) - preamble.headsize - preamble.datasize;
						portable_lseek(*m_piqx, tailSize, SEEK_CUR);
					}
				}
				else
				{
					// skip rest of frame
					portable_lseek(*m_piqx, preamble.framesize - sizeof(preamble), SEEK_CUR);
				}
			}
			else
			{
				// skip rest of frame
				portable_lseek(*m_piqx, preamble.framesize - sizeof(preamble), SEEK_CUR);
			}

			break;
		}
		case IQX_FRAME_TYPE_PAYLOADEND:
		{
			return ErrorCodes::InternalError;
		}
		default:
		{
			// skip rest of frame
			portable_lseek(*m_piqx, preamble.framesize - sizeof(preamble), SEEK_CUR);
		}
		break;
		}
	}
	return 0;
}
#endif

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
