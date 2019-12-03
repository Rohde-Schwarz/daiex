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

#include "iqtar.h"

#include "iqtarpimpl.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      IqTar::IqTar(const std::string& filename)
      {
        this->pimpl = new IqTar::Impl(filename);
      }

      IqTar::~IqTar()
      {
        delete this->pimpl;
      }

      int IqTar::readOpen(std::vector<std::string>& arrayNames)
      {
        return this->pimpl->readOpen(arrayNames);
      }

      int IqTar::writeOpen(
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
          metadata,
          nullptr);
      }

      int IqTar::writeOpen(
        IqDataFormat format,
        size_t nofArrays,
        const std::string& applicationName,
        const std::string& comment,
        const std::vector<ChannelInfo>& channelInfo,
        const std::map<std::string, std::string>& metadata,
        const std::string& deprecatedInfoXml)
      {
        return this->pimpl->writeOpen(
          format,
          nofArrays,
          applicationName,
          comment,
          channelInfo,
          &metadata,
          &deprecatedInfoXml);
      }

      int IqTar::close()
      {
        return this->pimpl->close();
      }

      int IqTar::getMetadata(std::vector<ChannelInfo>& channelInfos, std::map<std::string, std::string>& metadata) const
      {
        return this->pimpl->getMetadata(channelInfos, metadata);
      }

      int IqTar::getDeprecatedInfo(std::string& xmlString) const
      {
        return this->pimpl->getDeprecatedInfo(xmlString);
      }

      int64_t IqTar::getArraySize(const std::string& arrayName) const
      {
        return this->pimpl->getArraySize(arrayName);
      }

      time_t IqTar::getTimestamp() const
      {
        return this->pimpl->getTimestamp();
      }

      void IqTar::setTimestamp(const time_t timestamp)
      {
        this->pimpl->setTimestamp(timestamp);
      }

      int IqTar::setPreviewEnabled(bool enable)
      {
        return this->pimpl->setPreviewEnabled(enable);
      }

      bool IqTar::getPreviewEnabled() const
      {
        return this->pimpl->getPreviewEnabled();
      }

      int IqTar::readArray(const std::string& arrayName, std::vector<float>& values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readArray(arrayName, values, nofValues, offset);
      }

      int IqTar::readArray(const std::string& arrayName, float* values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readArray(arrayName, values, nofValues, offset);
      }

      int IqTar::readArray(const std::string& arrayName, std::vector<double>& values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readArray(arrayName, values, nofValues, offset);
      }

      int IqTar::readArray(const std::string& arrayName, double* values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readArray(arrayName, values, nofValues, offset);
      }

      int IqTar::readChannel(const std::string& channelName, std::vector<float>& values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readChannel(channelName, values, nofValues, offset);
      }

      int IqTar::readChannel(const std::string& channelName, float* values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readChannel(channelName, values, nofValues, offset);
      }

      int IqTar::readChannel(const std::string& channelName, std::vector<double>& values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readChannel(channelName, values, nofValues, offset);
      }

      int IqTar::readChannel(const std::string& channelName, double* values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readChannel(channelName, values, nofValues, offset);
      }

      int IqTar::appendArrays(const std::vector<std::vector<float>>& iqdata)
      {
        return this->pimpl->appendArrays(iqdata);
      }

      int IqTar::appendArrays(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
      {
        return this->pimpl->appendArrays(iqdata, sizes);
      }

      int IqTar::appendArrays(const std::vector<std::vector<double>>& iqdata)
      {
        return this->pimpl->appendArrays(iqdata);
      }

      int IqTar::appendArrays(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
      {
        return this->pimpl->appendArrays(iqdata, sizes);
      }

      int IqTar::appendChannels(const std::vector<std::vector<float>>& iqdata)
      {
        return this->pimpl->appendChannels(iqdata);
      }

      int IqTar::appendChannels(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
      {
        return this->pimpl->appendChannels(iqdata, sizes);
      }

      int IqTar::appendChannels(const std::vector<std::vector<double>>& iqdata)
      {
        return this->pimpl->appendChannels(iqdata);
      }

      int IqTar::appendChannels(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
      {
        return this->pimpl->appendChannels(iqdata, sizes);
      }

      int IqTar::setTempDir(const std::string& path)
      {
        return this->pimpl->setTempDir(path);
      }

      std::string IqTar::getTempDir() const
      {
        return this->pimpl->getTempDir();
      }

      int IqTar::disableTempFile(const uint64_t nofIqValues, const size_t nofChannels, const IqDataFormat format, const IqDataType dataType)
      {
        return this->pimpl->disableTempFile(nofIqValues, nofChannels, format, dataType);
      }
    }
  }
}
