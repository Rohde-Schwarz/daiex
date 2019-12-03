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

#include "iqw.h"

#include "iqwpimpl.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      Iqw::Iqw(const std::string& filename)
      {
        this->pimpl = new Iqw::Impl(filename);
      }

      Iqw::~Iqw()
      {
        delete this->pimpl;
      }

      int Iqw::readOpen(std::vector<std::string>& arrayNames)
      {
        return this->pimpl->readOpen(arrayNames);
      }

      int Iqw::writeOpen(
        IqDataFormat format,
        size_t nofArrays,
        const std::string& applicationName,
        const std::string& comment,
        const std::vector<ChannelInfo>& channelInfos,
        const std::map<std::string, std::string>* metadata)
      {
        return this->pimpl->writeOpen(
          format,
          nofArrays,
          applicationName,
          comment,
          channelInfos,
          metadata);
      }

      int Iqw::close()
      {
        return this->pimpl->close();
      }

      int64_t Iqw::getArraySize(const std::string& arrayName) const
      {
        return this->pimpl->getArraySize(arrayName);
      }

      time_t Iqw::getTimestamp() const
      {
        return this->pimpl->getTimestamp();
      }

      void Iqw::setTimestamp(const time_t timestamp)
      {
        this->pimpl->setTimestamp(timestamp);
      }

      int Iqw::getMetadata(std::vector<ChannelInfo>& channelInfos, std::map<std::string, std::string>& metadata) const
      {
        return this->pimpl->getMetadata(channelInfos, metadata);
      }

      IqDataOrder Iqw::getDataOrder() const
      {
        return this->pimpl->getDataOrder();
      }

      int Iqw::setDataOrder(IqDataOrder dataOrder)
      {
        return this->pimpl->setDataOrder(dataOrder);
      }

      int Iqw::readArray(const std::string& arrayName, std::vector<float>& values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readArray(arrayName, values, nofValues, offset);
      }

      int Iqw::readArray(const std::string& arrayName, float* values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readArray(arrayName, values, nofValues, offset);
      }

      int Iqw::readArray(const std::string& arrayName, std::vector<double>& values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readArray(arrayName, values, nofValues, offset);
      }

      int Iqw::readArray(const std::string& arrayName, double* values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readArray(arrayName, values, nofValues, offset);
      }

      int Iqw::readChannel(const std::string& channelName, std::vector<float>& values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readChannel(channelName, values, nofValues, offset);
      }

      int Iqw::readChannel(const std::string& channelName, float* values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readChannel(channelName, values, nofValues, offset);
      }

      int Iqw::readChannel(const std::string& channelName, std::vector<double>& values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readChannel(channelName, values, nofValues, offset);
      }

      int Iqw::readChannel(const std::string& channelName, double* values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readChannel(channelName, values, nofValues, offset);
      }

      int Iqw::appendArrays(const std::vector<std::vector<float>>& iqdata)
      {
        return this->pimpl->appendArrays(iqdata);
      }

      int Iqw::appendArrays(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
      {
        return this->pimpl->appendArrays(iqdata, sizes);
      }

      int Iqw::appendArrays(const std::vector<std::vector<double>>& iqdata)
      {
        return this->pimpl->appendArrays(iqdata);
      }

      int Iqw::appendArrays(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
      {
        return this->pimpl->appendArrays(iqdata, sizes);
      }

      int Iqw::appendChannels(const std::vector<std::vector<float>>& iqdata)
      {
        return this->pimpl->appendChannels(iqdata);
      }

      int Iqw::appendChannels(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
      {
        return this->pimpl->appendChannels(iqdata, sizes);
      }

      int Iqw::appendChannels(const std::vector<std::vector<double>>& iqdata)
      {
        return this->pimpl->appendChannels(iqdata);
      }

      int Iqw::appendChannels(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
      {
        return this->pimpl->appendChannels(iqdata, sizes);
      }

      int Iqw::setTempDir(const std::string& path)
      {
        return this->pimpl->setTempDir(path);
      }

      std::string Iqw::getTempDir() const
      {
        return this->pimpl->getTempDir();
      }
    }
  }
}