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

#include "dataimportexportbase.h"

#include "common.h"

#include <set>

#include "daiexception.h"
#include "errorcodes.h"

using namespace std;

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      DataImportExportBase::DataImportExportBase(const std::string& filename) : 
        filename_(filename),
        timestamp_(time(nullptr))
      {
      }

      bool DataImportExportBase::verifyUniqueChannelNames(const std::vector<ChannelInfo>& channelInfos)
      {
        auto hash = set<string>();
        for (auto channel : channelInfos)
        {
          // allow empty channel names -> will be replaced by default channel name when 
          // file is saved.
          if (false == channel.getChannelName().empty())
          {
            if (false == hash.insert(channel.getChannelName()).second)
            {
              return false;
            }
          }
        }

        return true;
      }

      int DataImportExportBase::readOpen(std::vector<std::string>& arrayNames)
      {
        arrayNames.clear();

        this->clearInternalState();

        // check that file exists and is not opened by other handle
        if (false == Platform::isFileAccessible(this->filename_))
        {
          return ErrorCodes::FileNotFound;
        }

        return ErrorCodes::Success;
      }

      int DataImportExportBase::writeOpen(
        IqDataFormat /*format*/,
        size_t /*nofArrays*/,
        const std::string& /*applicationName*/,
        const std::string& /*comment*/,
        const std::vector<ChannelInfo>& /*channelInfos*/,
        const std::map<std::string, std::string>* /*metadata*/)
      {
        this->clearInternalState();
        if (!Platform::isFileWriteable(this->filename_.c_str()))
        {
           return ErrorCodes::FileOpenError;
        }

        // if file does already exists, try to delete it
        try
        {
          if (Platform::isFileAccessible(this->filename_.c_str()))
          {
            // stop if file could not be deleted
            if (0 != remove(this->filename_.c_str()))
            {
              return ErrorCodes::FileOpenError;
            }
          }
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      void DataImportExportBase::clearInternalState()
      {
        this->metadata_.clear();
        this->channelInfos_.clear();
        this->timestamp_ = time(0);
      }

      void DataImportExportBase::addMetadata(const std::string& key, const std::string& value)
      {
        this->metadata_.insert(make_pair(key, value));
      }

      int DataImportExportBase::getMetadata(std::vector<ChannelInfo>& channelInfos, std::map<std::string, std::string>& metadata) const
      {
        if (0 == this->getChannelCount())
        {
          return ErrorCodes::OpenFileHasNotBeenCalled;
        }

        channelInfos = this->channelInfos_;
        metadata = this->metadata_;

        return ErrorCodes::Success;
      }

      void DataImportExportBase::addChannel(const std::string& channelName, double clockRate, double centerFreq, size_t samples)
      {
        this->channelInfos_.push_back(ChannelInfo(channelName, clockRate, centerFreq, samples));
      }

      const std::vector<ChannelInfo>& DataImportExportBase::getChannelInfos() const
      {
        return this->channelInfos_;
      }

      size_t DataImportExportBase::getChannelCount() const
      {
        return this->channelInfos_.size();
      }

      time_t DataImportExportBase::getTimestamp() const
      {
        return this->timestamp_;
      }

      void DataImportExportBase::setTimestamp(const time_t timestamp)
      {
        this->timestamp_ = timestamp;
      }

      bool DataImportExportBase::validateInputData(const std::vector<ChannelInfo>& channelInfos, const std::vector<size_t>& sizes, IqDataFormat format)
      {
        auto nofValueArrays = sizes.size();
        if (format != IqDataFormat::Real)
        {
          if (nofValueArrays % 2 != 0)
          {
            return false;
          }

          for (size_t i = 0; i < nofValueArrays; ++i)
          {
            if (sizes[i] != sizes[i + 1])
            {
              return false;
            }

            i++;
          }
        }

        if (format == IqDataFormat::Real)
        {
          return channelInfos.size() == nofValueArrays;
        }
        else
        {
          return channelInfos.size() == (nofValueArrays / 2);
        }
      }

      size_t DataImportExportBase::getWordWidth(IqDataType datatype)
      {
        switch (datatype)
        {
          //case IqDataType::Int8:
          //	return 1;
          //case IqDataType::Int16:
          //	return 2;
          //case IqDataType::Int32:
        case IqDataType::Float32:
          return 4;
        case IqDataType::Float64:
          return 8;
        default:
          return 0;
        }
      }

      size_t DataImportExportBase::getValuesPerSample(IqDataFormat format)
      {
        if (format == IqDataFormat::Real)
        {
          return 1;
        }
        else
        {
          return 2;
        }
      }
    }
  }
}