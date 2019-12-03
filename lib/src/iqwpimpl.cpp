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

#include "iqwpimpl.h"

#include <limits>

#include "platform.h"
#include "settings.h"
#include "constants.h"

using namespace std;
using namespace memory_mapped_file;

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      const std::string Iqw::Impl::DefaultArrayNameI_ = ChannelInfo::getDefaultArrayName(0, true);
      const std::string Iqw::Impl::DefaultArrayNameQ_ = ChannelInfo::getDefaultArrayName(0, false);

      Iqw::Impl::Impl(const std::string& filename) : DataImportExportBase(filename),
        dataOrder_(IqDataOrder::IIIQQQ),
        tempPath_(Platform::getTmpDir()),
        readerInitialized_(false),
        writerInitialized_(false)
      {
      }

      Iqw::Impl::~Impl()
      {
        this->close();
      }

      bool Iqw::Impl::isArrayNameValid(const std::string& arrayName, bool& readI)
      {
        if (arrayName.empty())
        {
          return false;
        }

        if (0 == arrayName.compare(Iqw::Impl::DefaultArrayNameI_))
        {
          readI = true;
        }
        else if (0 == arrayName.compare(Iqw::Impl::DefaultArrayNameQ_))
        {
          readI = false;
        }
        else
        {
          return false;
        }

        return true;
      }

      int Iqw::Impl::readOpen(std::vector<std::string>& arrayNames)
      {
        int ret = DataImportExportBase::readOpen(arrayNames);
        if (ErrorCodes::Success != ret)
        {
          return ret;
        }

        if (this->filename_.empty())
        {
          return ErrorCodes::FileNotFound;
        }

        // Einen Channel intern anlegen - denn IQW enthaelt immer genau einen Channel ( I/Q complex ),
        // damit bei GetMetadata diese Daten abgefragt werden koennen.
        // CenterFrequency und ClockRate sind unbekannt!

        size_t fileSize = Common::getFileSize(this->filename_);
        size_t pairs;
        size_t offsetI;
        size_t offsetQ;

        try
        {
          this->getReadParameters(fileSize, false, 0, 0, pairs, offsetI, offsetQ);
        }
        catch (DaiException &e)
        {
          arrayNames.clear();
          return e.code();
        }

        this->addChannel(
          ChannelInfo::getDefaultChannelName(0),
          numeric_limits<double>::quiet_NaN(),
          numeric_limits<double>::quiet_NaN(),
          pairs);

        // add metadata
        this->addMetadata(Constants::XmlDataType, "float32");
        this->addMetadata(Constants::XmlFormat, "complex");

        arrayNames.push_back(Iqw::Impl::DefaultArrayNameI_);
        arrayNames.push_back(Iqw::Impl::DefaultArrayNameQ_);

        this->readerInitialized_ = true;

        return ErrorCodes::Success;
      }

      int Iqw::Impl::writeOpen(
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

        if (format != IqDataFormat::Complex)
        {
          return ErrorCodes::InvalidDataFormat;
        }

        if (1 != channelInfos.size())
        {
          return ErrorCodes::InconsistentInputData;
        }

        // init writers
        if (this->dataOrder_ == IqDataOrder::IQIQIQ)
        {
          Platform::mmfOpen(this->mmfWriterOne_, this->filename_, if_exists_truncate, if_doesnt_exist_create);
          if (false == this->mmfWriterOne_.is_open())
          {
            return ErrorCodes::FileOpenError;
          }
        }
        else
        {
          // IqDataOrder::IIIQQQ
          this->tempFileI_ = Platform::getTmpFilename(this->tempPath_);
          this->tempFileQ_ = Platform::getTmpFilename(this->tempPath_);
          Platform::mmfOpen(this->mmfWriterOne_, this->tempFileI_, if_exists_truncate, if_doesnt_exist_create);
          Platform::mmfOpen(this->mmfWriterTwo_, this->tempFileQ_, if_exists_truncate, if_doesnt_exist_create);
          if (false == mmfWriterOne_.is_open() || false == mmfWriterTwo_.is_open())
          {
            return ErrorCodes::FileOpenError;
          }
        }

        this->writerInitialized_ = true;

        return ErrorCodes::Success;
      }

      int Iqw::Impl::close()
      {
        // nothing to do in read-only mode
        if (false == this->writerInitialized_)
        {
          this->readerInitialized_ = false;
          return ErrorCodes::Success;
        }

        // interleaved format -> merge temp files
        if (this->dataOrder_ == IqDataOrder::IIIQQQ)
        {
          try
          {
            this->finalizeTemporarySequence();
          }
          catch (DaiException &e)
          {
            return e.code();
          }
        }

        // close writers after finalizing tmp seq.
        try
        {
          if (this->mmfWriterOne_.is_open())
          {
            this->mmfWriterOne_.close();
          }

          if (this->mmfWriterTwo_.is_open())
          {
            this->mmfWriterTwo_.close();
          }
        }
        catch (...)
        {
          return ErrorCodes::InternalError;
        }

        this->deleteTempFiles();

        this->writerInitialized_ = false;
        return ErrorCodes::Success;
      }

      void Iqw::Impl::deleteTempFiles()
      {
        remove(this->tempFileI_.c_str());
        remove(this->tempFileQ_.c_str());
      }

      int64_t Iqw::Impl::getArraySize(const std::string& arrayName) const
      {
        if (false == readerInitialized_)
        {
          return -1;
        }

        bool readI;
        if (false == Iqw::Impl::isArrayNameValid(arrayName, readI) || 0 == this->channelInfos_.size())
        {
          return -1;
        }

        return this->channelInfos_[0].getSamples();
      }

      IqDataOrder Iqw::Impl::getDataOrder() const
      {
        return this->dataOrder_;
      }

      int Iqw::Impl::setDataOrder(IqDataOrder dataOrder)
      {
        if (this->readerInitialized_)
        {
          return ErrorCodes::ReaderAlreadyInitialized;
        }

        if (this->writerInitialized_)
        {
          return ErrorCodes::WriterAlreadyInitialized;
        }

        this->dataOrder_ = dataOrder;
        return ErrorCodes::Success;
      }

      int Iqw::Impl::readArray(const std::string& arrayName, std::vector<float>& values, size_t nofValues, size_t offset)
      {
        values.resize(nofValues);
        return this->readArray(arrayName, values.data(), nofValues, offset);
      }

      int Iqw::Impl::readArray(const std::string& arrayName, float* values, size_t nofValues, size_t offset)
      {
        try
        {
          this->readArrayInternal(arrayName, values, nofValues, offset);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int Iqw::Impl::readArray(const std::string& arrayName, std::vector<double>& values, size_t nofValues, size_t offset)
      {
        values.resize(nofValues);
        return this->readArray(arrayName, values.data(), nofValues, offset);
      }

      int Iqw::Impl::readArray(const std::string& arrayName, double* values, size_t nofValues, size_t offset)
      {
        try
        {
          this->readArrayInternal(arrayName, values, nofValues, offset);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int Iqw::Impl::readChannel(const std::string& channelName, std::vector<float>& values, size_t nofValues, size_t offset)
      {
        values.resize(nofValues);
        return this->readChannel(channelName, values.data(), nofValues, offset);
      }

      int Iqw::Impl::readChannel(const std::string& channelName, float* values, size_t nofValues, size_t offset)
      {
        try
        {
          this->readChannelInternal(channelName, values, nofValues, offset);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int Iqw::Impl::readChannel(const std::string& channelName, std::vector<double>& values, size_t nofValues, size_t offset)
      {
        values.resize(nofValues);
        return this-> readChannel(channelName, values.data(), nofValues, offset);
      }

      int Iqw::Impl::readChannel(const std::string& channelName, double* values, size_t nofValues, size_t offset)
      {
        try
        {
          this->readChannelInternal(channelName, values, nofValues, offset);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int Iqw::Impl::appendArrays(const std::vector<std::vector<float>>& iqdata)
      {
        vector<float*> dataPtrs;
        vector<size_t> sizes;
        Common::getVectorOfPtrAndSize(iqdata, dataPtrs, sizes);
        
        return this->appendArrays(dataPtrs, sizes);
      }

      int Iqw::Impl::appendArrays(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
      {
        try
        {
          this->writeArrayInternal(iqdata, sizes);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int Iqw::Impl::appendArrays(const std::vector<std::vector<double>>& iqdata)
      {
        vector<double*> dataPtrs;
        vector<size_t> sizes;
        Common::getVectorOfPtrAndSize(iqdata, dataPtrs, sizes);

        return this->appendArrays(dataPtrs, sizes);
      }

      int Iqw::Impl::appendArrays(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
      {
        try
        {
          this->writeArrayInternal(iqdata, sizes);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int Iqw::Impl::appendChannels(const std::vector<std::vector<float>>& iqdata)
      {
        vector<float*> dataPtrs;
        vector<size_t> sizes;
        Common::getVectorOfPtrAndSize(iqdata, dataPtrs, sizes);

        return this->appendChannels(dataPtrs, sizes);
      }

      int Iqw::Impl::appendChannels(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
      {
        try
        {
          this->writeChannelInternal(iqdata, sizes);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int Iqw::Impl::appendChannels(const std::vector<std::vector<double>>& iqdata)
      {
        vector<double*> dataPtrs;
        vector<size_t> sizes;
        Common::getVectorOfPtrAndSize(iqdata, dataPtrs, sizes);

        return this->appendChannels(dataPtrs, sizes);
      }

      int Iqw::Impl::appendChannels(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
      {
        try
        {
          this->writeChannelInternal(iqdata, sizes);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int Iqw::Impl::setTempDir(const std::string& path)
      {
        if (writerInitialized_)
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

      std::string Iqw::Impl::getTempDir() const
      {
        return this->tempPath_;
      }

      void Iqw::Impl::getReadParameters(
        size_t fileSize,
        bool readChannelInterleaved,
        size_t offset,
        size_t readNofValues,
        size_t& arraySize,
        size_t& readOffsetI,
        size_t& readOffsetQ)
      {
        // Calculate sequence length and seek start
        
        const int bytesPerVal = sizeof(float); // number of bytes per value (4 as IQW is always float32 data)
        const int valPerGroup = 2; // 1 I value and 1 Q value

        arraySize = fileSize / (valPerGroup * bytesPerVal);
        if (arraySize == 0)
        {
          throw DaiException(ErrorCodes::NoDataFoundInFile);
        }

        // allocate space for I + Q data
        if (readChannelInterleaved)
        {
          arraySize *= 2;
        }

        if (offset >= 0)
        {
          if (arraySize > offset)
          {
            arraySize = arraySize - offset;
            if (this->dataOrder_ == IqDataOrder::IQIQIQ)
            {
              readOffsetI = offset * bytesPerVal * valPerGroup;
              readOffsetQ = readOffsetI + (sizeof(float));
            }
            else
            {
              readOffsetI = offset * bytesPerVal;
              readOffsetQ = (fileSize / 2) + readOffsetI; 
            }
          }
          else
          {
            throw DaiException(ErrorCodes::StartIndexOutOfRange);
          }
        }

        if (readNofValues > 0 && arraySize != readNofValues)
        {
          if (arraySize > readNofValues)
          {
            arraySize = readNofValues;
          }
          else
          {
            throw DaiException(ErrorCodes::InvalidDataInterval);
          }
        }
      }

      void Iqw::Impl::finalizeTemporarySequence()
      {
        try
        {
          this->mmfWriterOne_.flush();
          this->mmfWriterOne_.close();
          this->mmfWriterTwo_.flush();
          this->mmfWriterTwo_.close();

          // TODO: copy using memory mapped file
          ofstream of_one;
          ifstream if_two;
          Platform::streamOpen(of_one, this->tempFileI_, ios_base::binary | ios_base::app);
          Platform::streamOpen(if_two, this->tempFileQ_, ios_base::binary);
          of_one.seekp(0, ios_base::end);
          of_one << if_two.rdbuf();

          of_one.close();
          if_two.close();

#if defined(_WIN32)
          // use utf-16 on windows
          _wrename(Common::utf8toUtf16(this->tempFileI_).c_str(), Common::utf8toUtf16(this->filename_).c_str());
#else
          rename(this->tempFileI_.c_str(), this->filename_.c_str());
#endif
        }
        catch (exception e)
        {
          throw DaiException(e.what());
        }
      }
    }
  }
}