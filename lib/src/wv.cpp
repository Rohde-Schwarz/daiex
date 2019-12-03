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

#include "wv.h"
#include "wvpimpl.h"

namespace rohdeschwarz
{
	namespace mosaik
	{
		namespace dataimportexport
		{

			Wv::Wv(const std::string& filename)
				: m_pimpl(new Wv::Impl(filename))
			{}

			Wv::Wv(Wv&& other)
				: m_pimpl(other.m_pimpl)
			{
				other.m_pimpl = nullptr;
			}

			Wv::~Wv()
			{
				if (m_pimpl)
				{
					delete m_pimpl;
					m_pimpl = nullptr;
				}
			}

			Wv& Wv::operator=(Wv&& other)
			{
				if (m_pimpl)
				{
					delete m_pimpl;
				}
				m_pimpl = other.m_pimpl;
				other.m_pimpl = nullptr;
				return *this;
			}

			int Wv::readOpen(std::vector<std::string>& arrayNames)
			{
				return m_pimpl->readOpen(arrayNames);
			}

			int Wv::writeOpen(
				IqDataFormat format,
				size_t nofArrays,
				const std::string& applicationName,
				const std::string& comment,
				const std::vector<ChannelInfo>& channelInfos,
				const std::map<std::string, std::string>* metadata)
			{
				return m_pimpl->writeOpen(format, nofArrays, applicationName, comment, channelInfos, metadata);
			}

			int Wv::close()
			{
				return m_pimpl->close();
			}

			int Wv::getMetadata(std::vector<ChannelInfo>& channelInfos, std::map<std::string, std::string>& metadata) const
			{
				return m_pimpl->getMetadata(channelInfos, metadata);
			}

			int64_t Wv::getArraySize(const std::string& arrayName) const
			{
				return m_pimpl->getArraySize(arrayName);
			}

			time_t Wv::getTimestamp() const
			{
				return m_pimpl->getTimestamp();
			}

			void Wv::setTimestamp(const time_t timestamp)
			{
				return m_pimpl->setTimestamp(timestamp);
			}

			int Wv::readArray(const std::string& arrayName, std::vector<float>& values, size_t nofValues, size_t offset)
			{
				return m_pimpl->readArray(arrayName, values, nofValues, offset);
			}

			int Wv::readArray(const std::string& arrayName, float* values, size_t nofValues, size_t offset)
			{
				return m_pimpl->readArray(arrayName, values, nofValues, offset);
			}

			int Wv::readArray(const std::string& arrayName, std::vector<double>& values, size_t nofValues, size_t offset)
			{
				return m_pimpl->readArray(arrayName, values, nofValues, offset);
			}

			int Wv::readArray(const std::string& arrayName, double* values, size_t nofValues, size_t offset)
			{
				return m_pimpl->readArray(arrayName, values, nofValues, offset);
			}

			int Wv::readChannel(const std::string& channelName, std::vector<float>& values, size_t nofValues, size_t offset)
			{
				return m_pimpl->readChannel(channelName, values, nofValues, offset);
			}

			int Wv::readChannel(const std::string& channelName, float* values, size_t nofValues, size_t offset)
			{
				return m_pimpl->readChannel(channelName, values, nofValues, offset);
			}

			int Wv::readChannel(const std::string& channelName, std::vector<double>& values, size_t nofValues, size_t offset)
			{
				return m_pimpl->readChannel(channelName, values, nofValues, offset);
			}

			int Wv::readChannel(const std::string& channelName, double* values, size_t nofValues, size_t offset)
			{
				return m_pimpl->readChannel(channelName, values, nofValues, offset);
			}

			int Wv::appendArrays(const std::vector<std::vector<float> >& iqdata)
			{
				return m_pimpl->appendArrays(iqdata);
			}

			int Wv::appendArrays(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
			{
				return m_pimpl->appendArrays(iqdata, sizes);
			}

			int Wv::appendArrays(const std::vector<std::vector<double> >& iqdata)
			{
				return m_pimpl->appendArrays(iqdata);
			}

			int Wv::appendArrays(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
			{
				return m_pimpl->appendArrays(iqdata, sizes);
			}

			int Wv::appendChannels(const std::vector<std::vector<float> >& iqdata)
			{
				return m_pimpl->appendChannels(iqdata);
			}

			int Wv::appendChannels(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
			{
				return m_pimpl->appendChannels(iqdata, sizes);
			}

			int Wv::appendChannels(const std::vector<std::vector<double> >& iqdata)
			{
				return m_pimpl->appendChannels(iqdata);
			}

			int Wv::appendChannels(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
			{
				return m_pimpl->appendChannels(iqdata, sizes);
			}

			void Wv::setScrambler(WvScramblerBase * scrambler)
			{
				m_pimpl->setScrambler(scrambler);
			}

		}
	}
}
