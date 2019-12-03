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


#include "iqmatlab_reader.h"

#include "platform.h"
#include "constants.h"

using namespace std;

#include <iostream>

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      IqMatlabReader::IqMatlabReader(const std::string& filename, IAnalyzeContent& updateContent) :
        updateContent_(&updateContent),
        matfp_(nullptr),
        initialized_(false),
        filename_(filename)
      {
      }

      IqMatlabReader::~IqMatlabReader()
      {
        this->close();
      }

      bool IqMatlabReader::isInitialized() const
      {
        return this->initialized_;
      }

      void IqMatlabReader::analyzeContent()
      {
        if (this->initialized_)
        {
          return;
        }

        // open file
        this->open();

        // read meta data
        try
        {
          this->readMandatoryMetadata();
          this->readOptionalMetadata();
        }
        catch (DaiException)
        {
          throw DaiException(ErrorCodes::InvalidFormatOfIqMatlabContent);
        }

        this->initialized_ = true;
      }

      void IqMatlabReader::readMandatoryMetadata()
      {
        string appName = IqMatlabReader::readCharArray(this->matfp_, Constants::XmlApplicationName);
        this->updateContent_->updateMetadata(Constants::XmlApplicationName, appName);

        string comment = IqMatlabReader::readCharArray(this->matfp_, Constants::XmlComment);
        this->updateContent_->updateMetadata(Constants::XmlComment, comment);

        string formatStr = IqMatlabReader::readCharArray(this->matfp_, Constants::XmlFormat);
        this->dataFormat_ = Common::getDataFormatFromString(formatStr);
        this->updateContent_->updateMetadata(Constants::XmlFormat, formatStr);

        string datatypeStr = IqMatlabReader::readCharArray(this->matfp_, Constants::XmlDataType);
        this->dataType_ = Common::getDataTypeFromString(datatypeStr);
        this->updateContent_->updateMetadata(Constants::XmlDataType, datatypeStr);

        string timeStr = IqMatlabReader::readCharArray(this->matfp_, Constants::XmlDateTime);
        time_t time = Platform::getTime(timeStr);
        this->updateContent_->updateTimestamp(time);

        this->createChannelInformation();
      }

      void IqMatlabReader::readOptionalMetadata()
      {
        try
        {
          string nofOptDataFieldName = Constants::XmlUserData + "_Count";
          size_t count = static_cast<size_t>(IqMatlabReader::readDoubleValue(this->matfp_, nofOptDataFieldName));

          for (size_t i = 0; i < count; ++i)
          {
            string varName = Constants::XmlUserData + to_string(i);
            auto kv = IqMatlabReader::readKeyValue(this->matfp_, varName);
            this->updateContent_->updateMetadata(kv.first, kv.second);
          }
        }
        catch (...)
        {
          // do nothing, data is optional
        }
      }

      void IqMatlabReader::createChannelInformation()
      {
        this->arrayNameToChannelNo_.clear();
        this->arrayNameToSamples_.clear();

        size_t nofChannels = static_cast<size_t>(IqMatlabReader::readDoubleValue(this->matfp_, Constants::XmlChannels));
        for (size_t i = 1; i <= nofChannels; ++i)
        {
          string channelBase = "Ch" + to_string(i) + "_";

          // channel name
          string channelKeyName = channelBase + "ChannelName";
          string channelName = IqMatlabReader::readCharArray(this->matfp_, channelKeyName);

          // clock rate
          string channelKeyClock = channelBase + "Clock_Hz";
          double clockRate = IqMatlabReader::readDoubleValue(this->matfp_, channelKeyClock);

          // center freq
          string channelKeyFreq = channelBase + "CFrequency_Hz";
          double freq = IqMatlabReader::readDoubleValue(this->matfp_, channelKeyFreq);

          // nof samples
          string channelKeySamples = channelBase + "Samples";
          double samples = IqMatlabReader::readDoubleValue(this->matfp_, channelKeySamples);

          this->updateContent_->updateChannels(channelName, clockRate, freq, static_cast<size_t>(samples));

          // store array length 
          // store mapping between channel name and channel number
          size_t nofSamples = static_cast<size_t>(samples);
          if (this->dataFormat_ == IqDataFormat::Real)
          {
            this->arrayNameToSamples_.insert(make_pair(channelName, nofSamples));
            this->arrayNameToChannelNo_.insert(make_pair(channelName, i));
          }
          else
          {
            this->arrayNameToSamples_.insert(make_pair(channelName + "_I", nofSamples));
            this->arrayNameToSamples_.insert(make_pair(channelName + "_Q", nofSamples));
            this->arrayNameToChannelNo_.insert(make_pair(channelName + "_I", i));
            this->arrayNameToChannelNo_.insert(make_pair(channelName + "_Q", i));
          }
        }
      }

      std::string IqMatlabReader::readCharArray(mat_t* const matfp, const std::string& arrayName)
      {
        matvar_t* matvar = Mat_VarRead(matfp, arrayName.c_str());
        if (matvar == nullptr)
        {
          throw DaiException(ErrorCodes::InvalidMatlabArrayName);
        }

        if (matvar->class_type != MAT_C_CHAR)
        {
          Mat_VarFree(matvar);
          throw DaiException(ErrorCodes::InvalidMatlabArrayType);
        }

        if (matvar->dims[0] != 1)
        {
          Mat_VarFree(matvar);
          throw DaiException(ErrorCodes::InvalidMatlabArraySize);
        }

        if (matvar->dims[1] == 0)
        {
          Mat_VarFree(matvar);
          return "";
        }

        string value(static_cast<char*>(matvar->data), matvar->dims[1]);

        // remove possible null-termination
        value = string(value.c_str());

        Mat_VarFree(matvar);

        return value;
      }

      double IqMatlabReader::readDoubleValue(mat_t* const matfp, const std::string& arrayName)
      {
        matvar_t* matvar = Mat_VarRead(matfp, arrayName.c_str());
        if (matvar == nullptr)
        {
          throw DaiException(ErrorCodes::InvalidMatlabArrayName);
        }

        if (matvar->class_type != MAT_C_DOUBLE)
        {
          Mat_VarFree(matvar);
          throw DaiException(ErrorCodes::InvalidMatlabArrayType);
        }

        if (matvar->dims[0] != 1 || matvar->dims[1] != 1)
        {
          Mat_VarFree(matvar);
          throw DaiException(ErrorCodes::InvalidMatlabArraySize);
        }

        double value = *static_cast<double*>(matvar->data);

        Mat_VarFree(matvar);

        return value;
      }

      std::pair<std::string, std::string> IqMatlabReader::readKeyValue(mat_t* const matfp, const std::string& arrayName)
      {
        matvar_t* matvar = Mat_VarRead(matfp, arrayName.c_str());
        if (matvar == nullptr)
        {
          throw DaiException(ErrorCodes::InvalidMatlabArrayName);
        }

        if (matvar->class_type != MAT_C_CHAR)
        {
          Mat_VarFree(matvar);
          throw DaiException(ErrorCodes::InvalidMatlabArrayType);
        }

        if (matvar->dims[0] != 2)
        {
          Mat_VarFree(matvar);
          throw DaiException(ErrorCodes::InvalidMatlabArraySize);
        }

        size_t totalLength = 2 * matvar->dims[1];
        char * data = static_cast<char*>(matvar->data);

        string key(stride_iterator<char*>(data, 2), stride_iterator<char*>(data + totalLength, 2));
        string value(stride_iterator<char*>(data + 1, 2), stride_iterator<char*>(data + totalLength - 1, 2));
        value.append(data + totalLength - 1, data + totalLength);

        // remove possible null-termination
        key = string(key.c_str());
        value = string(value.c_str());

        Mat_VarFree(matvar);

        return make_pair(key, value);
      }

      void IqMatlabReader::getArrayNames(std::vector<string>& arrayNames) const
      {
        arrayNames.clear();
        
        arrayNames.reserve(this->arrayNameToChannelNo_.size());
        for (auto imap: this->arrayNameToChannelNo_)
        {
          arrayNames.push_back(imap.first);
        }
      }

      void IqMatlabReader::open()
      {
        this->close();

        if (this->filename_.empty())
        {
          throw DaiException(ErrorCodes::FileNotFound);
        }

#if defined(_WIN32)
        // get windows short file path, as matio does not support wstring utf-16 encoding.
        this->matfp_ = Mat_Open(Common::getShortFilePath(this->filename_).c_str(), MAT_ACC_RDONLY);
#else
        this->matfp_ = Mat_Open(this->filename_.c_str(), MAT_ACC_RDONLY);
#endif
        if (this->matfp_ == nullptr)
        {
          throw DaiException(ErrorCodes::FileOpenError);
        }
      }

      void IqMatlabReader::close()
      {
        if (this->matfp_ == nullptr)
        {
          return;
        }

        Mat_Close(this->matfp_);
        this->matfp_ = nullptr;
      }

      bool IqMatlabReader::containsArray(const std::string& arrayName) const
      {
        return this->arrayNameToChannelNo_.count(arrayName) == 0 ? false : true;
      }

      long IqMatlabReader::getNofSamples(const std::string& arrayName) const
      {
        if (this->arrayNameToSamples_.count(arrayName) == 1)
        {
          return this->arrayNameToSamples_.at(arrayName);
        }

        return -1;
      }

      void IqMatlabReader::matchArrayDimensions(size_t minCols, size_t minRows, bool exactColMatch, std::vector<std::string>& arrayNames)
      {
        arrayNames.clear();

        if (this->matfp_ == nullptr)
        {
          this->open();
        }

        matvar_t* matvar;
        while ((matvar = Mat_VarReadNextInfo(this->matfp_)) != nullptr)
        {
          if (matvar->class_type != MAT_C_DOUBLE && matvar->class_type != MAT_C_SINGLE)
          {
            Mat_VarFree(matvar);
            continue;
          }

          auto rows = matvar->dims[0];
          auto cols = matvar->dims[1];
          string arrayName(matvar->name);
          Mat_VarFree(matvar);

          if (rows < minRows)
          {
            continue;
          }

          if (exactColMatch && cols != minCols)
          {
            continue;
          }

          if (cols < minCols)
          {
            break;
          }

          arrayNames.push_back(arrayName);
        }
      }

      int IqMatlabReader::getNofRows(const std::string& arrayName)
      {
        if (this->matfp_ == nullptr)
        {
          this->open();
        }

        matvar_t* matvar = Mat_VarReadInfo(this->matfp_, arrayName.c_str());
        if (matvar == nullptr)
        {
          Mat_VarFree(matvar);
          return -1;
        }

        auto tmp = static_cast<int>(matvar->dims[0]);
        Mat_VarFree(matvar);
        return tmp;
      }

      int IqMatlabReader::getNofCols(const std::string& arrayName)
      {
        if (this->matfp_ == nullptr)
        {
          this->open();
        }

        matvar_t* matvar = Mat_VarReadInfo(this->matfp_, arrayName.c_str());
        if (matvar == nullptr)
        {
          Mat_VarFree(matvar);
          return -1;
        }

        auto tmp = static_cast<int>(matvar->dims[1]);
        Mat_VarFree(matvar);
        return tmp;
      }
    }
  }
}
