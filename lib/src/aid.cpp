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

#include "aid.h"

#include "aidpimpl.h"

namespace rohdeschwarz
{
namespace mosaik
{
namespace dataimportexport
{

Aid::Aid(const std::string& filename)
: m_pimpl(new AidImpl(filename))
{}

Aid::Aid(Aid&& other)
: m_pimpl(other.m_pimpl)
{
	other.m_pimpl = nullptr;
}

Aid::~Aid()
{
  if (m_pimpl)
  {	
    delete m_pimpl;
	m_pimpl = nullptr;
  }
}

Aid& Aid::operator=(Aid&& other)
{
  if (m_pimpl)
  {
    delete m_pimpl;
  }
  m_pimpl = other.m_pimpl;
  other.m_pimpl = nullptr;
  return *this;
}

int Aid::readOpen(std::vector<std::string>& arrayNames)
{
  return m_pimpl->readOpen(arrayNames);
}

int Aid::writeOpen(
  IqDataFormat format,
  size_t nofArrays,
  const std::string& applicationName,
  const std::string& comment,
  const std::vector<ChannelInfo>& channelInfos,
  const std::map<std::string, std::string>* metadata)
{
  return m_pimpl->writeOpen(format, nofArrays, applicationName, comment, channelInfos, metadata);
}

int Aid::close()
{
  return m_pimpl->close();
}

int Aid::getMetadata(std::vector<ChannelInfo>& channelInfos, std::map<std::string, std::string>& metadata) const
{
  return m_pimpl->getMetadata(channelInfos, metadata);
}

int64_t Aid::getArraySize(const std::string& arrayName) const
{
  return m_pimpl->getArraySize(arrayName);
}

time_t Aid::getTimestamp() const
{
  return m_pimpl->getTimestamp();
}

void Aid::setTimestamp(const time_t timestamp)
{
  return m_pimpl->setTimestamp(timestamp);
}

int Aid::readArray(const std::string& arrayName, std::vector<float>& values, size_t nofValues, size_t offset)
{
  return m_pimpl->readArray(arrayName, values, nofValues, offset);
}

int Aid::readArray(const std::string& arrayName, float* values, size_t nofValues, size_t offset)
{
  return m_pimpl->readArray(arrayName, values, nofValues, offset);
}

int Aid::readArray(const std::string& arrayName, std::vector<double>& values, size_t nofValues, size_t offset)
{
  return m_pimpl->readArray(arrayName, values, nofValues, offset);
}

int Aid::readArray(const std::string& arrayName, double* values, size_t nofValues, size_t offset)
{
  return m_pimpl->readArray(arrayName, values, nofValues, offset);
}

int Aid::readChannel(const std::string& channelName, std::vector<float>& values, size_t nofValues, size_t offset)
{
  return m_pimpl->readChannel(channelName, values, nofValues, offset);
}

int Aid::readChannel(const std::string& channelName, float* values, size_t nofValues, size_t offset)
{
  return m_pimpl->readChannel(channelName, values, nofValues, offset);
}

int Aid::readChannel(const std::string& channelName, std::vector<double>& values, size_t nofValues, size_t offset)
{
  return m_pimpl->readChannel(channelName, values, nofValues, offset);
}

int Aid::readChannel(const std::string& channelName, double* values, size_t nofValues, size_t offset)
{
  return m_pimpl->readChannel(channelName, values, nofValues, offset);
}

int Aid::appendArrays(const std::vector<std::vector<float> >& iqdata)
{
  return m_pimpl->appendArrays(iqdata);
}

int Aid::appendArrays(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
{
  return m_pimpl->appendArrays(iqdata, sizes);
}

int Aid::appendArrays(const std::vector<std::vector<double> >& iqdata)
{
  return m_pimpl->appendArrays(iqdata);
}

int Aid::appendArrays(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
{
  return m_pimpl->appendArrays(iqdata, sizes);
}

int Aid::appendChannels(const std::vector<std::vector<float> >& iqdata)
{
  return m_pimpl->appendChannels(iqdata);
}

int Aid::appendChannels(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
{
  return m_pimpl->appendChannels(iqdata, sizes);
}

int Aid::appendChannels(const std::vector<std::vector<double> >& iqdata)
{
  return m_pimpl->appendChannels(iqdata);
}

int Aid::appendChannels(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
{
  return m_pimpl->appendChannels(iqdata, sizes);
}

}
}
}
