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

#include "iqmatlabpimpl.h"

#include "errorcodes.h"
#include "daiexception.h"
#include "platform.h"

using namespace std;

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      IqMatlab::Impl::Impl(const std::string& filename) : DataImportExportBase(filename),
        reader_(nullptr),
        writer_(nullptr),
        tempPath_(Platform::getTmpDir()),
        matVersion_(MatlabVersion::Mat73)
      {
      }

      IqMatlab::Impl::~Impl()
      {
        this->close();
      }

      int IqMatlab::Impl::setMatlabVersion(MatlabVersion version)
      {
        if (this->writer_ != nullptr)
        {
          return ErrorCodes::WriterAlreadyInitialized;
        }

        this->matVersion_ = version;
        return ErrorCodes::Success;
      }

      MatlabVersion IqMatlab::Impl::getMatlabVersion() const
      {
        return this->matVersion_;
      }

      int IqMatlab::Impl::readOpen(std::vector<std::string>& arrayNames)
      {
        int ret = DataImportExportBase::readOpen(arrayNames);
        if (ErrorCodes::Success != ret)
        {
          return ret;
        }
        
        if (this->writer_ != nullptr)
        {
          return ErrorCodes::WriterAlreadyInitialized;
        }

        if (this->reader_ != nullptr)
        {
          return ErrorCodes::ReaderAlreadyInitialized;
        }

        if (this->filename_.empty())
        {
          return ErrorCodes::FileNotFound;
        }
        
        // open reader and analyze content
        try
        {
          this->reader_ = new IqMatlabReader(this->filename_, *this);
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
          // if this was not a valid IqMatlab file, keep file opened, as user
          // might query mat-arrays manually.
          if (e.code() != ErrorCodes::InvalidFormatOfIqMatlabContent)
          {
            this->close();
          }
          
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int IqMatlab::Impl::writeOpen(
        IqDataFormat format,
        size_t nofArrays,
        const std::string& applicationName,
        const std::string& comment,
        const std::vector<ChannelInfo>& channelInfos,
        const std::map<std::string, std::string>* metadata)
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

        try
        {
          this->writer_ = new IqMatlabWriter(
            this->filename_,
            this->matVersion_,
            format, 
            nofArrays,
            applicationName,
            comment,
            this->timestamp_,
            channelInfos,
            this->tempPath_,
            metadata);  

          this->writer_->open();
        }
        catch (DaiException &e)
        {
          this->close();
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int IqMatlab::Impl::close()
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

        return ErrorCodes::Success;
      }

      int64_t IqMatlab::Impl::getArraySize(const std::string& arrayName) const
      {
        if (this->reader_ == nullptr)
        {
          return -1;
        }

        return this->reader_->getNofSamples(arrayName);
      }

      int IqMatlab::Impl::readArray(const std::string& arrayName, std::vector<float>& values, size_t nofValues, size_t offset)
      {
        values.resize(nofValues);
        return this->readArray(arrayName, values.data(), nofValues, offset);
      }

      int IqMatlab::Impl::readArray(const std::string& arrayName, float* values, size_t nofValues, size_t offset)
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

      int IqMatlab::Impl::readArray(const std::string& arrayName, std::vector<double>& values, size_t nofValues, size_t offset)
      {
        values.resize(nofValues);
        return this->readArray(arrayName, values.data(), nofValues, offset);
      }

      int IqMatlab::Impl::readArray(const std::string& arrayName, double* values, size_t nofValues, size_t offset)
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

      int IqMatlab::Impl::readChannel(const std::string& channelName, std::vector<float>& values, size_t nofValues, size_t offset)
      {
        values.resize(nofValues);
        return this->readChannel(channelName, values.data(), nofValues, offset);
      }

      int IqMatlab::Impl::readChannel(const std::string& channelName, float* values, size_t nofValues, size_t offset)
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

      int IqMatlab::Impl::readChannel(const std::string& channelName, std::vector<double>& values, size_t nofValues, size_t offset)
      {
        values.resize(nofValues);
        return this->readChannel(channelName, values.data(), nofValues, offset);
      }

      int IqMatlab::Impl::readChannel(const std::string& channelName, double* values, size_t nofValues, size_t offset)
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

      int IqMatlab::Impl::appendArrays(const std::vector<std::vector<float>>& iqdata)
      {
        vector<float*> dataPtr;
        vector<size_t> sizes;
        Common::getVectorOfPtrAndSize(iqdata, dataPtr, sizes);

        return this->appendArrays(dataPtr, sizes);
      }

      int IqMatlab::Impl::appendArrays(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
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

      int IqMatlab::Impl::appendArrays(const std::vector<std::vector<double>>& iqdata)
      {
        vector<double*> dataPtr;
        vector<size_t> sizes;
        Common::getVectorOfPtrAndSize(iqdata, dataPtr, sizes);

        return this->appendArrays(dataPtr, sizes);
      }

      int IqMatlab::Impl::appendArrays(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
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

      int IqMatlab::Impl::appendChannels(const std::vector<std::vector<float>>& iqdata)
      {
        vector<float*> dataPtr;
        vector<size_t> sizes;
        Common::getVectorOfPtrAndSize(iqdata, dataPtr, sizes);

        return this->appendChannels(dataPtr, sizes);
      }

      int IqMatlab::Impl::appendChannels(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
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

      int IqMatlab::Impl::appendChannels(const std::vector<std::vector<double>>& iqdata)
      {
        vector<double*> dataPtr;
        vector<size_t> sizes;
        Common::getVectorOfPtrAndSize(iqdata, dataPtr, sizes);

        return this->appendChannels(dataPtr, sizes);
      }

      int IqMatlab::Impl::appendChannels(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
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

      /** IAnalyzeContent start */
      void IqMatlab::Impl::updateChannels(const std::string& channelName, double clock, double frequency, size_t samples)
      {
        this->addChannel(channelName, clock, frequency, samples);
      }

      void IqMatlab::Impl::updateMetadata(const std::string& key, const std::string& value)
      {
        this->addMetadata(key, value);
      }

      void IqMatlab::Impl::updateTimestamp(const time_t timestamp)
      {
        this->setTimestamp(timestamp);
      }
      /** IAnalyzeContent end */

      /** IArraySelector start */

      int IqMatlab::Impl::matchArrayDimensions(size_t minCols, size_t minRows, bool exactColMatch, std::vector<std::string>& arrayNames)
      {
        if (this->filename_.empty())
        {
          return ErrorCodes::FileNotFound;
        }

        try
        {
          IqMatlabReader reader(this->filename_.c_str(), *this);
          reader.open();
          reader.matchArrayDimensions(minCols, minRows, exactColMatch, arrayNames);
          reader.close();
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int64_t IqMatlab::Impl::getNofRows(const std::string& arrayName)
      {
        if (this->filename_.empty())
        {
          return ErrorCodes::FileNotFound;
        }

        int64_t tmp = -1;
        try
        {
          IqMatlabReader reader(this->filename_, *this);
          tmp = reader.getNofRows(arrayName);
          reader.close();
        }
        catch (...)
        {
        }

        return tmp;
      }

      int64_t IqMatlab::Impl::getNofCols(const std::string& arrayName)
      {
        if (this->filename_.empty())
        {
          return ErrorCodes::FileNotFound;
        }

        int64_t tmp = -1;
        try
        {
          IqMatlabReader reader(this->filename_, *this);
          tmp = reader.getNofCols(arrayName);
          reader.close();
        }
        catch (...)
        {
        }
        
        return tmp;
      }

      int IqMatlab::Impl::readRawArray(const std::string& arrayName, size_t column, size_t nofValues, std::vector<float>& values, size_t offset)
      {
        values.resize(nofValues);
        return this->readRawArray(arrayName, column, nofValues, values.data(), offset);
      }

      int IqMatlab::Impl::readRawArray(const std::string& arrayName, size_t column, size_t nofValues, float* values, size_t offset)
      {
        if (this->filename_.empty())
        {
          return ErrorCodes::FileNotFound;
        }

        try
        {
          IqMatlabReader reader(this->filename_, *this);
          reader.readRawArray(arrayName, column, nofValues, values, offset);
          reader.close();
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int IqMatlab::Impl::readRawArray(const std::string& arrayName, size_t column, size_t nofValues, std::vector<double>& values, size_t offset)
      {
        values.resize(nofValues);
        return this->readRawArray(arrayName, column, nofValues, values.data(), offset);
      }

      int IqMatlab::Impl::readRawArray(const std::string& arrayName, size_t column, size_t nofValues, double* values, size_t offset)
      {
        if (this->filename_.empty())
        {
          return ErrorCodes::FileNotFound;
        }

        try
        {
          IqMatlabReader reader(this->filename_, *this);
          reader.readRawArray(arrayName, column, nofValues, values, offset);
          reader.close();
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      /** IArraySelector end */

      /** ITempDir start */

      int IqMatlab::Impl::setTempDir(const std::string& path)
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

      std::string IqMatlab::Impl::getTempDir() const
      {
        return this->tempPath_;
      }

      /** ITempDir end */
    }
  }
}