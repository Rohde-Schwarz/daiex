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

#include "iqtarpimpl.h"

#include <vector>

#include "platform.h"
#include "errorcodes.h"
#include "daiexception.h"
#include "iqtar_reader.h"

using namespace std;

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
 	    IqTar::Impl::Impl(const std::string& filename) : DataImportExportBase(filename),
        reader_(nullptr),
        writer_(nullptr),
        dataType_(IqDataType::Float32),
        tempPath_(Platform::getTmpDir()),
        expectedIqDataFileSize_(0),
        enablePreview_(true)
      {
      }

      IqTar::Impl::~Impl()
      {
        this->close();
      }

      int IqTar::Impl::setPreviewEnabled(bool enable)
      {
        if (this->writer_ != nullptr)
        {
          return ErrorCodes::WriterAlreadyInitialized;
        }

        this->enablePreview_ = enable;
        return ErrorCodes::Success;
      }

      bool IqTar::Impl::getPreviewEnabled() const
      {
        return this->enablePreview_;
      }

      int IqTar::Impl::readOpen(std::vector<std::string>& arrayNames)
      {
        int ret = DataImportExportBase::readOpen(arrayNames);
        if (ErrorCodes::Success != ret)
        {
          return ret;
        }

        if (this->reader_ != nullptr)
        {
          return ErrorCodes::ReaderAlreadyInitialized;
        }

        if (this->writer_ != nullptr)
        {
          return ErrorCodes::WriterAlreadyInitialized;
        }

        if (this->filename_.empty())
        {
          return ErrorCodes::FileNotFound;
        }
        
        // open reader and analyze tar/xml content
        try
        {
          this->reader_ = new IqTarReader(this->filename_, *this);
          this->reader_->analyzeContent();

          if (this->getChannelCount() == 0)
          {
            return ErrorCodes::EmptyChannelInfo;
          }

          // get array names
          this->reader_->getArrayNames(arrayNames);
        }
        catch (DaiException &e)
        {
          this->close();
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int IqTar::Impl::writeOpen(
        IqDataFormat format,
        size_t nofArrays,
        const std::string& applicationName,
        const std::string& comment,
        const std::vector<ChannelInfo>& channelInfos,
        const std::map<std::string, std::string>* metadata,
        const std::string* deprecatedInfoXml)
      {
        int ret = DataImportExportBase::writeOpen(format, nofArrays, applicationName, comment, channelInfos, metadata);
        if (ErrorCodes::Success != ret)
        {
          return ret;
        }

        if (this->reader_ != nullptr)
        {
          return ErrorCodes::ReaderAlreadyInitialized;
        }

        if (this->writer_ != nullptr)
        {
          return ErrorCodes::WriterAlreadyInitialized;
        }

        // check if the specified deprecated information is a valid XML string.
        if (deprecatedInfoXml != nullptr && false == Common::isValidXml(*deprecatedInfoXml))
        {
          return ErrorCodes::InvalidXmlString;
        }
        
        try
        {
          this->writer_ = new IqTarWriter(
            this->filename_,
            format,
            this->dataType_,
            nofArrays,
            applicationName,
            comment,
            this->timestamp_,
            channelInfos,
            this->enablePreview_,
            this->tempPath_,
            metadata,
            deprecatedInfoXml,
            this->expectedIqDataFileSize_);

          this->writer_->open();
        }
        catch (DaiException &e)
        {
          this->close();
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int IqTar::Impl::close()
      {
        try
        {
          if (this->reader_ != nullptr)
          {
            this->reader_->close();
            delete this->reader_;
            this->reader_ = nullptr;
          }

          if (this->writer_ != nullptr)
          {
            this->writer_->close();
            delete this->writer_; 
            this->writer_ = nullptr;
          }
        }
        catch (DaiException &e)
        {
          return e.code();
        }
        catch (...)
        {
          return ErrorCodes::InternalError;
        }

        return ErrorCodes::Success;
      }

      int IqTar::Impl::setTempDir(const std::string& path)
      {
        if (this->writer_ != nullptr)
        {
          return ErrorCodes::WriterAlreadyInitialized;
        }

        string pathCopy = path;
        if (Common::strEndsWithIgnoreCase(pathCopy, "\\") || Common::strEndsWithIgnoreCase(pathCopy, "/"))
        {
          pathCopy = pathCopy.substr(0, pathCopy.length() -1);
        }

        if (false == Common::directoryExists(pathCopy))
        {
          return ErrorCodes::FolderNotFound;
        }

        this->tempPath_ = pathCopy;

        return ErrorCodes::Success;
      }

      std::string IqTar::Impl::getTempDir() const
      {
        return this->tempPath_;
      }

      int IqTar::Impl::disableTempFile(const uint64_t nofIqValues, const size_t nofChannels, const IqDataFormat format, const IqDataType dataType)
      {
        if (this->writer_ != nullptr)
        {
          return ErrorCodes::WriterAlreadyInitialized;
        }

        // iq.tar format requires all channels to have the same number of samples
        auto nofArrays = nofChannels;
        if (format == IqDataFormat::Complex)
        {
          nofArrays *= 2;
        }

        if (dataType == IqDataType::Float32)
        {
          this->expectedIqDataFileSize_ = nofIqValues * nofArrays * sizeof(float);
        }
        else if (dataType == IqDataType::Float64)
        {
          this->expectedIqDataFileSize_ = nofIqValues * nofArrays * sizeof(double);
        }
        else
        {
          return ErrorCodes::WrongDataType;
        }

        this->dataType_ = dataType;

        return ErrorCodes::Success;
      }

      int IqTar::Impl::getDeprecatedInfo(std::string& xmlString) const
      {
        if (0 == this->getChannelCount())
        {
          return ErrorCodes::OpenFileHasNotBeenCalled;
        }

        xmlString = this->deprecatedInfo_;

        return ErrorCodes::Success;
      }

      int64_t IqTar::Impl::getArraySize(const std::string& arrayName) const
      {
        if (this->reader_ == nullptr)
        {
          return -1;
        }

        if (arrayName.empty() || false == this->reader_->containsArray(arrayName))
        {
          return -1;
        }

        return this->reader_->getNofSamples();
      }

      int IqTar::Impl::readArray(const std::string& arrayName, std::vector<float>& values, size_t nofValues, size_t offset)
      {
        values.resize(nofValues);
        return this->readArray(arrayName, values.data(), nofValues, offset);
      }

      int IqTar::Impl::readArray(const std::string& arrayName, float* values, size_t nofValues, size_t offset)
      {
        if (this->reader_ == nullptr)
        {
          return ErrorCodes::OpenFileHasNotBeenCalled;
        }

        try
        {
          this->reader_->readArray(arrayName, values, nofValues, offset);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int IqTar::Impl::readArray(const std::string& arrayName, std::vector<double>& values, size_t nofValues, size_t offset)
      {
        values.resize(nofValues);
        return this->readArray(arrayName, values.data(), nofValues, offset);
      }

      int IqTar::Impl::readArray(const std::string& arrayName, double* values, size_t nofValues, size_t offset)
      {
        if (this->reader_ == nullptr)
        {
          return ErrorCodes::OpenFileHasNotBeenCalled;
        }

        try
        {
          this->reader_->readArray(arrayName, values, nofValues, offset);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int IqTar::Impl::readChannel(const std::string& channelName, std::vector<float>& values, size_t nofValues, size_t offset)
      {
        values.resize(nofValues);
        return this->readChannel(channelName, values.data(), nofValues, offset);
      }

      int IqTar::Impl::readChannel(const std::string& channelName, float* values, size_t nofValues, size_t offset)
      {
        if (this->reader_ == nullptr)
        {
          return ErrorCodes::OpenFileHasNotBeenCalled;
        }

        try
        {
          this->reader_->readChannel(channelName, values, nofValues, offset);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int IqTar::Impl::readChannel(const std::string& channelName, std::vector<double>& values, size_t nofValues, size_t offset)
      {
        values.resize(nofValues);
        return this->readChannel(channelName, values.data(), nofValues, offset);
      }

      int IqTar::Impl::readChannel(const std::string& channelName, double* values, size_t nofValues, size_t offset)
      {
        if (this->reader_ == nullptr)
        {
          return ErrorCodes::OpenFileHasNotBeenCalled;
        }

        try
        {
          this->reader_->readChannel(channelName, values, nofValues, offset);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int IqTar::Impl::appendArrays(const std::vector<std::vector<float>>& iqdata)
      {
        vector<float*> dataPtr;
        vector<size_t> sizes;
        Common::getVectorOfPtrAndSize(iqdata, dataPtr, sizes);

        return this->appendArrays(dataPtr, sizes);
      }

      int IqTar::Impl::appendArrays(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
      {
        if (this->writer_ == nullptr)
        {
          return ErrorCodes::FileWriterUninitialized;
        }

        try
        {
          this->writer_->appendArray(iqdata, sizes);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int IqTar::Impl::appendArrays(const std::vector<std::vector<double>>& iqdata)
      {
        vector<double*> dataPtr;
        vector<size_t> sizes;
        Common::getVectorOfPtrAndSize(iqdata, dataPtr, sizes);

        return this->appendArrays(dataPtr, sizes);
      }

      int IqTar::Impl::appendArrays(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
      {
        if (this->writer_ == nullptr)
        {
          return ErrorCodes::FileWriterUninitialized;
        }

        try
        {
          this->writer_->appendArray(iqdata, sizes);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int IqTar::Impl::appendChannels(const std::vector<std::vector<float>>& iqdata)
      {
        vector<float*> dataPtr;
        vector<size_t> sizes;
        Common::getVectorOfPtrAndSize(iqdata, dataPtr, sizes);

        return this->appendChannels(dataPtr, sizes);
      }

      int IqTar::Impl::appendChannels(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
      {
        if (this->writer_ == nullptr)
        {
          return ErrorCodes::FileWriterUninitialized;
        }

        try
        {
          this->writer_->appendChannel(iqdata, sizes);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int IqTar::Impl::appendChannels(const std::vector<std::vector<double>>& iqdata)
      {
        vector<double*> dataPtr;
        vector<size_t> sizes;
        Common::getVectorOfPtrAndSize(iqdata, dataPtr, sizes);

        return this->appendChannels(dataPtr, sizes);
      }

      int IqTar::Impl::appendChannels(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
      {
        if (this->writer_ == nullptr)
        {
          return ErrorCodes::FileWriterUninitialized;
        }

        try
        {
          this->writer_->appendChannel(iqdata, sizes);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      /** IAnalyzeContentIqTar start */
      void IqTar::Impl::updateChannels(const std::string& channelName, double clock, double frequency, size_t samples)
      {
        this->addChannel(channelName, clock, frequency, samples);
      }

      void IqTar::Impl::updateMetadata(const std::string& key, const std::string& value)
      {
        this->addMetadata(key, value);
      }

      void IqTar::Impl::updateTimestamp(const time_t timestamp)
      {
        this->setTimestamp(timestamp);
      }

      void IqTar::Impl::updateDeprecatedInfo(const std::string& xmlString)
      {
        this->deprecatedInfo_ = xmlString;
      }

      /** IAnalyzeContentIqTar end */
    }
  }
}
