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

#include "iqcsv.h"

#include "iqcsvpimpl.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      IqCsv::IqCsv(const std::string& filename)
      {
        this->pimpl = new IqCsv::Impl(filename);
      }

      IqCsv::~IqCsv()
      {
        delete this->pimpl;
      }

      int IqCsv::readOpen(std::vector<std::string>& arrayNames)
      {
        return this->pimpl->readOpen(arrayNames);
      }

      int IqCsv::writeOpen(
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

      int IqCsv::close()
      {
        return this->pimpl->close();
      }

      int IqCsv::setSeparatorConfiguration(const CsvSeparatorConfig config)
      {
        return this->pimpl->setSeparatorConfiguration(config);
      }

      CsvSeparatorConfig IqCsv::getSeparatorConfiguration() const
      {
        return this->pimpl->getSeparatorConfiguration();
      }

      char IqCsv::getValueSeparator() const
      {
        return this->pimpl->getValueSeparator();
      }

      char IqCsv::getDecimalSeparator() const
      {
        return this->pimpl->getDecimalSeparator();
      }

      int IqCsv::setFormatSpecifier(const std::string& format)
      {
        return this->pimpl->setFormatSpecifier(format);
      }

      const std::string& IqCsv::getFormatSpecifier() const
      {
        return this->pimpl->getFormatSpecifier();
      }

      int IqCsv::setFormatSpecifierChannelInfo(const std::string& format)
      {
        return this->pimpl->setFormatSpecifierChannelInfo(format);
      }

      const std::string& IqCsv::getFormatSpecifierChannelInfo() const
      {
        return this->pimpl->getFormatSpecifierChannelInfo();
      }

      int64_t IqCsv::getArraySize(const std::string& arrayName) const
      {
        return this->pimpl->getArraySize(arrayName);
      }

      time_t IqCsv::getTimestamp() const
      {
        return this->pimpl->getTimestamp();
      }

      void IqCsv::setTimestamp(const time_t timestamp)
      {
        this->pimpl->setTimestamp(timestamp);
      }

      int IqCsv::getMetadata(std::vector<ChannelInfo>& channelInfos, std::map<std::string, std::string>& metadata) const
      {
        return this->pimpl->getMetadata(channelInfos, metadata);
      }

      int IqCsv::readArray(const std::string& arrayName, std::vector<float>& values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readArray(arrayName, values, nofValues, offset);
      }

      int IqCsv::readArray(const std::string& arrayName, float* values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readArray(arrayName, values, nofValues, offset);
      }

      int IqCsv::readArray(const std::string& arrayName, std::vector<double>& values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readArray(arrayName, values, nofValues, offset);
      }

      int IqCsv::readArray(const std::string& arrayName, double* values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readArray(arrayName, values, nofValues, offset);
      }

      int IqCsv::readChannel(const std::string& channelName, std::vector<float>& values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readChannel(channelName, values, nofValues, offset);
      }

      int IqCsv::readChannel(const std::string& channelName, float* values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readChannel(channelName, values, nofValues, offset);
      }

      int IqCsv::readChannel(const std::string& channelName, std::vector<double>& values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readChannel(channelName, values, nofValues, offset);
      }

      int IqCsv::readChannel(const std::string& channelName, double* values, size_t nofValues, size_t offset)
      {
        return this->pimpl->readChannel(channelName, values, nofValues, offset);
      }

      int IqCsv::appendArrays(const std::vector<std::vector<float>>& iqdata)
      {
        return this->pimpl->appendArrays(iqdata);
      }

      int IqCsv::appendArrays(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
      {
        return this->pimpl->appendArrays(iqdata, sizes);
      }

      int IqCsv::appendArrays(const std::vector<std::vector<double>>& iqdata)
      {
        return this->pimpl->appendArrays(iqdata);
      }

      int IqCsv::appendArrays(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
      {
        return this->pimpl->appendArrays(iqdata, sizes);
      }

      int IqCsv::appendChannels(const std::vector<std::vector<float>>& iqdata)
      {
        return this->pimpl->appendChannels(iqdata);
      }

      int IqCsv::appendChannels(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
      {
        return this->pimpl->appendChannels(iqdata, sizes);
      }

      int IqCsv::appendChannels(const std::vector<std::vector<double>>& iqdata)
      {
        return this->pimpl->appendChannels(iqdata);
      }

      int IqCsv::appendChannels(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
      {
        return this->pimpl->appendChannels(iqdata, sizes);
      }

      int64_t IqCsv::getNofRows(size_t column)
      {
        return this->pimpl->getNofRows(column);
      }

      int64_t IqCsv::getNofCols()
      {
        return this->pimpl->getNofCols();
      }

      int IqCsv::readRawArray(size_t column, size_t nofValues, std::vector<float>& values, size_t offset)
      {
        return this->pimpl->readRawArray(column, nofValues, values, offset);
      }

      int IqCsv::readRawArray(size_t column, size_t nofValues, float* values, size_t offset)
      {
        return this->pimpl->readRawArray(column, nofValues, values, offset);
      }

      int IqCsv::readRawArray(size_t column, size_t nofValues, std::vector<double>& values, size_t offset)
      {
        return this->pimpl->readRawArray(column, nofValues, values, offset);
      }

      int IqCsv::readRawArray(size_t column, size_t nofValues, double* values, size_t offset)
      {
        return this->pimpl->readRawArray(column, nofValues, values, offset);
      }
    }
  }
}