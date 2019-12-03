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

#include <sys/stat.h>
#include <sys/types.h>

#include <numeric>

#include "aidpimpl.h"
#include "ZFFileReader.h"
#include "ZFFileWriter.h"
#include "ArrayComplex.h"
#include <codecvt>
#include <locale>
namespace rohdeschwarz
{
	namespace mosaik
	{
		namespace dataimportexport
		{

			using namespace std;

AidImpl::AidImpl(const std::string& filename) : 
  DataImportExportBase(filename),
	m_filename(filename),
	m_write(false),
	m_samples(0),
  m_timeStamp(time(nullptr))
{
}

AidImpl::~AidImpl()
{
}

int AidImpl::readOpen(std::vector<std::string>& arrayNames)
{
  resetData();
  arrayNames.clear();

  // open file
  m_reader.setFilename(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(m_filename));
  if (m_reader.open())
  {
    return 1;
  }
  m_timeStamp = m_reader.getFirstTimestamp();

  // and now ... build meta data and channel infos:
  m_samples = m_reader.getNumberOfSamples();
  m_channelInfo.push_back(ChannelInfo("Channel1", m_reader.getSampleRate(), (double)m_reader.getCenterFrequency(), m_samples));
  arrayNames.push_back("Channel1");
  if (m_reader.getBandwidth() != 0)
  {
    m_metaData.emplace("Ch1_MeasBandwidth[Hz]", std::to_string(m_reader.getBandwidth()));
  }
  if (m_reader.getAntennaVoltageRef() != 0)
  {
    m_metaData.emplace("Ch1_AntennaVoltageRef", std::to_string(m_reader.getAntennaVoltageRef()));
  }
  if (m_reader.getDuration() != 0)
  {
    m_metaData.emplace("Ch1_Duration", std::to_string(m_reader.getDuration()));
  }

  m_metaData.emplace("Ch1_SampleSize", std::to_string(m_reader.getSampleSize()));
  m_write = false;
  return 0;
}

int AidImpl::getMetadata(std::vector<ChannelInfo>& channelInfos, map<string, string>& metadata) const
{
  if ((m_channelInfo.empty() || (m_metaData.empty())))
  {
    return 1;
  }
  metadata = m_metaData;
  channelInfos = m_channelInfo;
  return 0;
}

void AidImpl::resetData()
{
  m_nofArrays = 0;
  m_applicationName = "";
  m_comment = "";
  m_channelInfo.clear();
  m_metaData.clear();
  m_samples = 0;
  m_reader.reset();
}

int AidImpl::writeOpen(IqDataFormat format, size_t nofArrays, const std::string& applicationName, const std::string& comment,
  const std::vector<ChannelInfo>& channelInfos, const std::map<std::string, std::string>* metadata)
{
  resetData();
  int status = DataImportExportBase::writeOpen(format, nofArrays, applicationName, comment, channelInfos, metadata);
  if (status != 0)
  {
    return status;
  }
  m_write = true;
  m_writer.setTimestamp(m_timeStamp);
  m_writer.setCenterFrequency((uint64_t)channelInfos[0].getFrequency());
  m_writer.setSampleRate((uint32_t)channelInfos[0].getClockRate());
  m_writer.setDatablockSettings(100000u, 1u);
  //m_writer.setFrameType(ekFRH_DATASTREAM__IFDATA_32RE_32IM_FLOAT_RESCALED);

  m_writer.setFilename(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(m_filename));
  status = m_writer.open();
  return status;
}

int AidImpl::close()
{
  // close file

  if (m_writer.isOpen())
  {
    m_writer.close();
  }
  if (m_reader.isOpen())
  {
    m_reader.close();
  }
  return 0;
}

time_t AidImpl::getTimestamp() const
{
  return m_timeStamp;
}

int64_t  AidImpl::getArraySize(const std::string& /*arrayName*/) const
{
  return m_samples;
}

void AidImpl::setTimestamp(const time_t timestamp)
{
  m_timeStamp = timestamp;
}

int AidImpl::readAid(size_t nofValues, size_t offset)
{
  int status = 0;
  m_cArr.resize((uint32_t)nofValues);
  status = m_reader.setReadMarker(offset, offset + nofValues);
  if (status != 0)
  {
    return 1;
  }
  status = m_reader.read(m_cArr);
  return status;
}

int AidImpl::readArray(const std::string& arrayName, std::vector<float>& values, size_t nofValues, size_t offset)
{
  int status = readAid(nofValues, offset);
  if (status != 0)
  {
    return 1;
  }
  values.reserve(nofValues);
  bool isI = arrayName.find("_I", arrayName.size() - 2) != string::npos;
  if (isI)
  {

    for (uint32_t i = 0; i< nofValues; i++)
    {
      values.push_back(m_cArr[i].re);
    }
  }
  else
  {
    for (uint32_t i = 0; i< nofValues; i++)
    {
      values.push_back(m_cArr[i].im);
    }
  }
  return 0;
}

int AidImpl::readArray(const std::string& arrayName, float* values, size_t nofValues, size_t offset)
{
  int status = readAid(nofValues, offset);
  if (status != 0)
  {
    return 1;
  }
  bool isI = arrayName.find("_I", arrayName.size() - 2) != string::npos;
  if (isI)
  {

    for (uint32_t i = 0; i< nofValues; i++)
    {
      values[i] = m_cArr[i].re;
    }
  }
  else
  {
    for (uint32_t i = 0; i< nofValues; i++)
    {
      values[i] = m_cArr[i].im;
    }
  }
  return 0;
}

int AidImpl::readArray(const std::string& arrayName, std::vector<double>& values, size_t nofValues, size_t offset)
{
  int status = readAid(nofValues, offset);
  if (status != 0)
  {
    return 1;
  }
  values.reserve(nofValues);
  bool isI = arrayName.find("_I", arrayName.size() - 2) != string::npos;
  if (isI)
  {

    for (uint32_t i = 0; i< nofValues; i++)
    {
      values.push_back(m_cArr[i].re);
    }
  }
  else
  {
    for (uint32_t i = 0; i< nofValues; i++)
    {
      values.push_back(m_cArr[i].im);
    }
  }
  return 0;
}

int AidImpl::readArray(const std::string& arrayName, double* values, size_t nofValues, size_t offset)
{
  int status = readAid(nofValues, offset);
  if (status != 0)
  {
    return 1;
  }
  bool isI = arrayName.find("_I", arrayName.size() - 2) != string::npos;
  if (isI)
  {

    for (uint32_t i = 0; i< nofValues; i++)
    {
      values[i] = m_cArr[i].re;
    }
  }
  else
  {
    for (uint32_t i = 0; i< nofValues; i++)
    {
      values[i] = m_cArr[i].im;
    }
  }
  return 0;
}

int AidImpl::readChannel(const std::string& /*channelName*/, std::vector<float>& values, size_t nofValues, size_t offset)
{
  int status = readAid(nofValues, offset);
  if (status != 0)
  {
    return 1;
  }
  values.reserve(nofValues);
  for (uint32_t i = 0; i< nofValues; i++)
  {
    values.push_back(m_cArr[i].re);
    values.push_back(m_cArr[i].im);
  }
  return 0;
}

int AidImpl::readChannel(const std::string& /*channelName*/, float* values, size_t nofValues, size_t offset)
{
  int status = readAid(nofValues, offset);
  if (status != 0)
  {
    return 1;
  }
  for (uint32_t i = 0; i< nofValues; i++)
  {
    values[i * 2] = m_cArr[i].re;
    values[i * 2 + 1] = m_cArr[i].im;
  }
  return 0;
}

int AidImpl::readChannel(const std::string& /*channelName*/, std::vector<double>& values, size_t nofValues, size_t offset)
{
  int status = readAid(nofValues, offset);
  if (status != 0)
  {
    return 1;
  }
  values.reserve(nofValues);
  for (uint32_t i = 0; i< nofValues; i++)
  {
    values.push_back(m_cArr[i].re);
    values.push_back(m_cArr[i].im);
  }
  return 0;
}

int AidImpl::readChannel(const std::string& /*channelName*/, double* values, size_t nofValues, size_t offset)
{
  int status = readAid(nofValues, offset);
  if (status != 0)
  {
    return 1;
  }
  for (uint32_t i = 0; i< nofValues; i++)
  {
    values[i * 2] = m_cArr[i].re;
    values[i * 2 + 1] = m_cArr[i].im;
  }
  return 0;
}

int AidImpl::appendArrays(const std::vector<std::vector<float> >& iqdata)
{
  // aid supports only one channel, what is the same as two arrays
  if (iqdata.size() != 2)
  {
    return ErrorCodes::InternalError;
  }

  for (uint32_t i = 0; i < iqdata.size() / 2; i++)
  {
    if (iqdata[i * 2].size() != iqdata[i * 2 + 1].size())
    {
      return ErrorCodes::InternalError;
    }
  }
  m_cArr.resize((uint32_t)iqdata[0].size());
  for (size_t i = 0; i< iqdata[0].size(); i++)
  {
    m_cArr[(uint32)i].re = iqdata[0][i];
    m_cArr[(uint32)i].im = iqdata[1][i];
  }
  m_cArr.setSize((uint32)iqdata[0].size());
  m_writer.write(m_cArr);
  return 0;
}

int AidImpl::appendArrays(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
{
  if (sizes.size() != 2)
  {
    return ErrorCodes::InternalError;
  }
  for (size_t i = 0; i < sizes.size() / 2; i++)
  {
    if (sizes[i * 2] != sizes[i * 2 + 1])
    {
      return ErrorCodes::InternalError;
    }
  }

  m_cArr.realloc((uint32)sizes[0]);
  for (size_t i = 0; i< sizes[0]; i++)
  {
    m_cArr[(uint32)i].re = iqdata[0][i];
    m_cArr[(uint32)i].im = iqdata[1][i];
    if (m_cArr[(uint32)i].re != iqdata[0][i])
    {
      throw 1;
    }
  }
  m_cArr.setSize((uint32)sizes[0]);
  m_writer.write(m_cArr);
  return 0;
}

int AidImpl::appendArrays(const std::vector<std::vector<double> >& iqdata)
{
  if (iqdata.size() != 2)
  {
    return ErrorCodes::InternalError;
  }

  for (size_t i = 0; i < iqdata.size() / 2; i++)
  {
    if (iqdata[i * 2].size() != iqdata[i * 2 + 1].size())
    {
      return ErrorCodes::InternalError;
    }
  }
  m_cArr.realloc((uint32)iqdata[0].size());
  for (size_t i = 0; i< iqdata[0].size(); i++)
  {
    m_cArr[(uint32)i].re = (float)iqdata[0][i];
    m_cArr[(uint32)i].im = (float)iqdata[1][i];
  }
  m_cArr.setSize((uint32)iqdata[0].size());
  m_writer.write(m_cArr);
  return 0;
}

int AidImpl::appendArrays(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
{
  if (sizes.size() != 2)
  {
    return ErrorCodes::InternalError;
  }
  for (size_t i = 0; i < sizes.size() / 2; i++)
  {
    if (sizes[i * 2] != sizes[i * 2 + 1])
    {
      return ErrorCodes::InternalError;
    }
  }

  m_cArr.realloc((uint32)sizes[0]);
  for (size_t i = 0; i< sizes[0]; i++)
  {
    m_cArr[(uint32)i].re = (float)iqdata[0][i];
    m_cArr[(uint32)i].im = (float)iqdata[1][i];
  }
  m_cArr.setSize((uint32)sizes[0]);
  m_writer.write(m_cArr);
  return 0;
}

int AidImpl::appendChannels(const std::vector<std::vector<float> >& iqdata)
{
  // aid supports only one channel
  if (iqdata.size() > 1)
  {
    return 1;
  }
  m_cArr.realloc((uint32)(iqdata[0].size() / 2));
  for (size_t i = 0; i< iqdata[0].size() / 2; i++)
  {
    m_cArr[(uint32)i].re = iqdata[0][i*2];
    m_cArr[(uint32)i].im = iqdata[0][i*2+1];
  }
  m_cArr.setSize((uint32)(iqdata[0].size() / 2));
  m_writer.write(m_cArr);
  return 0;
}

int AidImpl::appendChannels(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
{
  // aid supports only one channel
  if (iqdata.size() > 1)
  {
    return 1;
  }
  m_cArr.realloc((uint32)(sizes[0] / 2));
  for (size_t i = 0; i< sizes[0] / 2; i++)
  {
    m_cArr[(uint32)i].re = iqdata[0][i * 2];
    m_cArr[(uint32)i].im = iqdata[0][i * 2 + 1];
  }
  m_cArr.setSize((uint32)sizes[0] / 2);
  m_writer.write(m_cArr);
  return 0;
}

int AidImpl::appendChannels(const std::vector<std::vector<double> >& iqdata)
{
  // aid supports only one channel
  if (iqdata.size() > 1)
  {
    return 1;
  }
  m_cArr.realloc((uint32)(iqdata[0].size() / 2));
  for (size_t i = 0; i< iqdata[0].size() / 2; i++)
  {
    m_cArr[(uint32)i].re = (float)iqdata[0][i * 2];
    m_cArr[(uint32)i].im = (float)iqdata[0][i * 2 + 1];
  }
  m_cArr.setSize((uint32)(iqdata[0].size() / 2));
  m_writer.write(m_cArr);
  return 0;
}

int AidImpl::appendChannels(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
{
  // aid supports only one channel
  if (iqdata.size() > 1)
  {
    return 1;
  }
  m_cArr.resize((uint32)(sizes[0] / 2));
  for (size_t i = 0; i< sizes[0] / 2; i++)
  {
    m_cArr[(uint32)i].re = (float)iqdata[0][i * 2];
    m_cArr[(uint32)i].im = (float)iqdata[0][i * 2 + 1];
  }
  m_cArr.setSize((uint32)(sizes[0] / 2));
  m_writer.write(m_cArr);
  return 0;
}

		} // namespace
	} // namespace
} // namespace
