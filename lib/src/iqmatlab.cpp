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

#include "iqmatlab.h"

#include "iqmatlabpimpl.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      IqMatlab::IqMatlab(const std::string& filename)
      {
        this->pimpl = new IqMatlab::Impl(filename);
      }

      IqMatlab::~IqMatlab()
      {
        delete this->pimpl;
      }

      int IqMatlab::readOpen(std::vector<std::string>& arrayNames)
      {
        return this->pimpl->readOpen(arrayNames);
      }

      int IqMatlab::writeOpen(
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

      int IqMatlab::close()
      {
        return this->pimpl->close();
      }

      int64_t IqMatlab::getArraySize(const std::string& arrayName) const
      {
        return this->pimpl->getArraySize(arrayName);
      }

      time_t IqMatlab::getTimestamp() const
      {
        return this->pimpl->getTimestamp();
      }

      void IqMatlab::setTimestamp(const time_t timestamp)
      {
        this->pimpl->setTimestamp(timestamp);
      }

      int IqMatlab::setMatlabVersion(const MatlabVersion version)
      {
        return this->pimpl->setMatlabVersion(version);
      }

      MatlabVersion IqMatlab::getMatlabVersion() const
      {
        return this->pimpl->getMatlabVersion();
      }

      int IqMatlab::getMetadata(std::vector<ChannelInfo>& channelInfos, std::map<std::string, std::string>& metadata) const
      {
        return this->pimpl->getMetadata(channelInfos, metadata);
      }

      int IqMatlab::readArray(const std::string& arrayName, std::vector<float>& values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readArray(arrayName, values, nofValues, offset);
      }

      int IqMatlab::readArray(const std::string& arrayName, float* values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readArray(arrayName, values, nofValues, offset);
      }

      int IqMatlab::readArray(const std::string& arrayName, std::vector<double>& values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readArray(arrayName, values, nofValues, offset);
      }

      int IqMatlab::readArray(const std::string& arrayName, double* values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readArray(arrayName, values, nofValues, offset);
      }

      int IqMatlab::readChannel(const std::string& channelName, std::vector<float>& values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readChannel(channelName, values, nofValues, offset);
      }

      int IqMatlab::readChannel(const std::string& channelName, float* values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readChannel(channelName, values, nofValues, offset);
      }

      int IqMatlab::readChannel(const std::string& channelName, std::vector<double>& values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readChannel(channelName, values, nofValues, offset);
      }

      int IqMatlab::readChannel(const std::string& channelName, double* values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readChannel(channelName, values, nofValues, offset);
      }

      int IqMatlab::appendArrays(const std::vector<std::vector<float>>& iqdata)
      {
        return this->pimpl->appendArrays(iqdata);
      }

      int IqMatlab::appendArrays(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
      {
        return this->pimpl->appendArrays(iqdata, sizes);
      }

      int IqMatlab::appendArrays(const std::vector<std::vector<double>>& iqdata)
      {
        return this->pimpl->appendArrays(iqdata);
      }

      int IqMatlab::appendArrays(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
      {
        return this->pimpl->appendArrays(iqdata, sizes);
      }

      int IqMatlab::appendChannels(const std::vector<std::vector<float>>& iqdata)
      {
        return this->pimpl->appendChannels(iqdata);
      }

      int IqMatlab::appendChannels(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
      {
        return this->pimpl->appendChannels(iqdata, sizes);
      }

      int IqMatlab::appendChannels(const std::vector<std::vector<double>>& iqdata)
      {
        return this->pimpl->appendChannels(iqdata);
      }

      int IqMatlab::appendChannels(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
      {
        return this->pimpl->appendChannels(iqdata, sizes);
      }

      int IqMatlab::matchArrayDimensions(size_t minCols, size_t minRows, bool exactColMatch, std::vector<std::string>& arrayNames)
      {
        return this->pimpl->matchArrayDimensions(minCols, minRows, exactColMatch, arrayNames);
      }

      int64_t IqMatlab::getNofRows(const std::string& arrayName)
      {
        return this->pimpl->getNofRows(arrayName);
      }

      int64_t IqMatlab::getNofCols(const std::string& arrayName)
      {
        return this->pimpl->getNofCols(arrayName);
      }

      int IqMatlab::readRawArray(const std::string& arrayName, size_t column, size_t nofValues, std::vector<float>& values, size_t offset)
      {
        return this->pimpl->readRawArray(arrayName, column, nofValues, values, offset);
      }

      int IqMatlab::readRawArray(const std::string& arrayName, size_t column, size_t nofValues, float* values, size_t offset)
      {
        return this->pimpl->readRawArray(arrayName, column, nofValues, values, offset);
      }

      int IqMatlab::readRawArray(const std::string& arrayName, size_t column, size_t nofValues, std::vector<double>& values, size_t offset)
      {
        return this->pimpl->readRawArray(arrayName, column, nofValues, values, offset);
      }

      int IqMatlab::readRawArray(const std::string& arrayName, size_t column, size_t nofValues, double* values, size_t offset)
      {
        return this->pimpl->readRawArray(arrayName, column, nofValues, values, offset);
      }

      int IqMatlab::setTempDir(const std::string& path)
      {
        return this->pimpl->setTempDir(path);
      }

      std::string IqMatlab::getTempDir() const
      {
        return this->pimpl->getTempDir();
      }
    }
  }
}