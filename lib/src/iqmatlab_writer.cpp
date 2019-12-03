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

#include "iqmatlab_writer.h"

#include "dataimportexportbase.h"
#include "constants.h"
#include "daiexception.h"
#include "errorcodes.h"
#include "settings.h"
#include "platform.h"

using namespace std;
using namespace memory_mapped_file;

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      IqMatlabWriter::IqMatlabWriter(const std::string& filename,
        MatlabVersion matlabVersion,
        IqDataFormat dataFormat,
        size_t nofArrays,
        const std::string& applicationName,
        const std::string& comment,
        const time_t timestamp,
        const std::vector<ChannelInfo>& channelInfos,
        const std::string& tempPath,
        const std::map<std::string, std::string>* metadata) :
      initialized_(false),
      filename_(filename),
      tempPath_(tempPath),
      matVersion_(matlabVersion),
      nofArrays_(nofArrays),
      dataFormat_(dataFormat),
      dataType_(IqDataType::Float32),
      lockDataType_(false),
      applicationName_(applicationName),
      comment_(comment),
      timestamp_(timestamp),
      channelInfos_(channelInfos),
      metadata_()
      {
        if (metadata != nullptr)
        {
          this->metadata_ = map<string, string>(metadata->begin(), metadata->end());
        }
      }

      IqMatlabWriter::~IqMatlabWriter()
      {
        this->close();
      }

      void IqMatlabWriter::open()
      {
        if (this->initialized_)
        {
          return;
        }
        
        if (this->filename_.empty())
        {
          throw DaiException(ErrorCodes::FileNotFound);
        }

        // verify that all channel names are unambiguous
        if (false == DataImportExportBase::verifyUniqueChannelNames(this->channelInfos_))
        {
          throw DaiException(ErrorCodes::InconsistentInputData);
        }

        // also split appendChannel() into sperate I and Q file
        // create required number of file writers
        size_t nofTmpFiles = (this->dataFormat_ == IqDataFormat::Real) ? this->nofArrays_ : 2 * this->channelInfos_.size();
        this->tempFiles_.resize(nofTmpFiles);
        this->mmfWriters_.resize(nofTmpFiles);
        for (size_t i = 0; i < nofTmpFiles; ++i)
        {
          this->tempFiles_[i] = Platform::getTmpFilename(this->tempPath_);
          Platform::mmfOpen(this->mmfWriters_[i], this->tempFiles_[i], if_exists_truncate, if_doesnt_exist_create);
        }

        this->initialized_ = true;
      }

      void IqMatlabWriter::close()
      {
        if (this->initialized_ == false)
        {
          return;
        }

        // writer is closed when being destroyed
        this->mmfWriters_.clear();

        // merge temp data to file and add meta data and remove temp overhead
        this->finalizeTemporarySequence();
        this->deleteTempFiles();

        this->initialized_ = false;
      }

      void IqMatlabWriter::deleteTempFiles()
      {
        for (auto file : this->tempFiles_)
        {
          remove(file.c_str());
        }
      }

      void IqMatlabWriter::finalizeTemporarySequence()
      {
        // we need to check if file already exists -> matio will try to open
        // existing files on Mat_CreateVer.
        // If file exists -> try to delete it, otherwise throw exception
        if (Platform::isFileAccessible(this->filename_.c_str()))
        {
          if (0 != remove(this->filename_.c_str()))
          {
            throw DaiException(ErrorCodes::FileOpenError);
          }
        }

        // create final matlab file
        auto version = (this->matVersion_ == MatlabVersion::Mat4) ? MAT_FT_MAT4 : MAT_FT_MAT73;

        mat_t* matfp = nullptr;
#if defined(_WIN32)
        // since matio does not support utf-16 encoding for windows, we
        // first write data to a tmp file and then rename the tmp file to
        // actual filename using utf-16.
        string path = Common::getPath(this->filename_);
        string tmpFile = Platform::getTmpFilename(path);
        matfp = Mat_CreateVer(tmpFile.c_str(), nullptr, version);
#else
        matfp = Mat_CreateVer(filename_.c_str(), nullptr, version);
#endif
        if (matfp == nullptr)
        {
          throw DaiException(ErrorCodes::FileOpenError);
        }

        try
        {
          // write meta data
          this->writeMetadata(matfp);
          IqMatlabWriter::writeCharArray(matfp, Constants::XmlDataType, IqDataTypeNames[this->dataType_]);
        
          // add actual I/Q data from temporary files
          if (this->dataFormat_ == IqDataFormat::Real)
          {
            this->writeRealData(matfp);
          }
          else
          {
            this->writeData(matfp);
          }
        }
        catch (DaiException)
        {
          Mat_Close(matfp);
          throw;
        }

        Mat_Close(matfp);

#if defined (_WIN32)
        // rename tmp file to actual filename.
        int res =_wrename(Common::utf8toUtf16(tmpFile).c_str(), Common::utf8toUtf16(this->filename_).c_str());
        if (res != 0)
        {
          throw DaiException(ErrorCodes::InternalError);
        }
#endif
      }

      void IqMatlabWriter::writeRealData(mat_t* const mat)
      {
        for (size_t ch = 0; ch < this->channelInfos_.size(); ++ch)
        {
          string arrayBaseName = "Ch" + to_string(ch + 1) + "_";
          string arrayDataName = arrayBaseName + "Data";
          string nofSamplesName = arrayBaseName + Constants::XmlSamples;

          // TODO task 232601: do not map all, copy step-wise -> Mat_VarWriteData() ?
          read_only_mmf mmf;
          Platform::mmfOpen(mmf, this->tempFiles_[ch], true);
          
          // temp file written with double precision
          size_t nofSamples = mmf.file_size() / sizeof(double); 
          if (nofSamples == 0)
          {
            mmf.close();
            return;
          }

          IqMatlabWriter::writeDoubleValue(mat, nofSamplesName, static_cast<double>(nofSamples)); 

          size_t dims[2] = { nofSamples, 1 };
          matvar_t* matvar = Mat_VarCreate(arrayDataName.c_str(), MAT_C_DOUBLE, MAT_T_DOUBLE, 2, dims, (double*)mmf.data(), MAT_F_DONT_COPY_DATA);
          if (matvar == nullptr)
          {
            mmf.close();
            throw DaiException(ErrorCodes::InternalError);
          }

          auto res = Mat_VarWrite(mat, matvar, MAT_COMPRESSION_NONE);
          if (res != 0)
          {
            mmf.close();
            Mat_VarFree(matvar);
            throw DaiException(ErrorCodes::InternalError);
          }

          Mat_VarFree(matvar);
          mmf.close();
        }
      }

      void IqMatlabWriter::writeData(mat_t* const mat)
      {
        for (size_t ch = 0; ch < this->channelInfos_.size(); ++ch)
        {
          string arrayBaseName = "Ch" + to_string(ch + 1) + "_";
          string arrayDataname = arrayBaseName + "Data";
          string nofSamplesName = arrayBaseName + Constants::XmlSamples;

          // TODO task 232601: do not map all, copy step-wise
          // currently not supported by matio, vars must be written at once
          read_only_mmf mmfI;
          read_only_mmf mmfQ;
          Platform::mmfOpen(mmfI, this->tempFiles_[2*ch], true);
          Platform::mmfOpen(mmfQ, this->tempFiles_[2*ch + 1], true);

          // temp file written with double precision
          size_t nofValues = mmfI.file_size() / sizeof(double);
          if (nofValues == 0)
          {
            mmfI.close();
            mmfQ.close();
            return;
          }

          IqMatlabWriter::writeDoubleValue(mat, nofSamplesName, static_cast<double>(nofValues));

          // copy data to IIIIQQQQ
          vector<double> data;
          data.reserve(2 * nofValues);
          
          data.insert(data.begin(), reinterpret_cast<const double*>(mmfI.data()), reinterpret_cast<const double*>(mmfI.data()) + nofValues);
          mmfI.close();

          data.insert(data.begin() + nofValues, reinterpret_cast<const double*>(mmfQ.data()), reinterpret_cast<const double*>(mmfQ.data()) + nofValues);
          mmfQ.close();

          size_t dims[2] = { nofValues, 2 };
          matvar_t* matvar = Mat_VarCreate(arrayDataname.c_str(), MAT_C_DOUBLE, MAT_T_DOUBLE, 2, dims, &data[0], MAT_F_DONT_COPY_DATA);
          if (matvar == nullptr)
          {
            throw DaiException(ErrorCodes::InternalError);
          }

          auto res = Mat_VarWrite(mat, matvar, MAT_COMPRESSION_NONE);
          if (res != 0)
          {
            Mat_VarFree(matvar);
            throw DaiException(ErrorCodes::InternalError);
          }

          Mat_VarFree(matvar);
        }
      }

      void IqMatlabWriter::writeMetadata(mat_t* const mat)
      {
        IqMatlabWriter::writeCharArray(mat, Constants::XmlApplicationName, this->applicationName_);
        IqMatlabWriter::writeCharArray(mat, Constants::XmlComment, this->comment_);
        IqMatlabWriter::writeCharArray(mat, Constants::XmlDateTime, Common::toString(this->timestamp_));
        IqMatlabWriter::writeCharArray(mat, Constants::XmlFormat, IqDataFormatNames[this->dataFormat_]);
        IqMatlabWriter::writeDoubleValue(mat, Constants::XmlChannels, static_cast<double>(this->channelInfos_.size()));

        // channel infos
        int ch = 1;
        for (auto channel : this->channelInfos_)
        {
          string chBase = "Ch" + ::to_string(ch) + "_";

          // channel name
          string chName = chBase + "ChannelName";
          IqMatlabWriter::writeCharArray(mat, chName, channel.getChannelName());

          // freq
          string chFreq = chBase + "CFrequency_Hz";
          IqMatlabWriter::writeDoubleValue(mat, chFreq, channel.getFrequency());

          // clock
          string chClock = chBase + "Clock_Hz";
          IqMatlabWriter::writeDoubleValue(mat, chClock, channel.getClockRate());

          ch++;
        }

        // metadata
        size_t nofMetadata = 0;
        if (this->metadata_.size() > 0)
        {
          nofMetadata = this->metadata_.size();

          int dataCount = 0;
          for (const auto& pair : this->metadata_)
          {
            string arrayName = Constants::XmlUserData + to_string(dataCount++);
            IqMatlabWriter::writeMetadata(mat, arrayName, pair.first, pair.second);
          }
        }

        IqMatlabWriter::writeDoubleValue(mat, Constants::XmlUserData + "_Count", static_cast<double>(nofMetadata));
      }

      void IqMatlabWriter::writeCharArray(mat_t* const mat, const std::string& arrayName, const std::string& value)
      {
        size_t dims[2] = { 1, value.size() };

        // MAT_T_UTF8 not defined in Mat_VarWrite4 -> use UINT8 instead
        matvar_t* matvar = Mat_VarCreate(arrayName.c_str(), MAT_C_CHAR, MAT_T_UINT8, 2, dims, (char*)value.c_str(), MAT_F_DONT_COPY_DATA);
        if (matvar == nullptr)
        {
          throw DaiException(ErrorCodes::InternalError);
        }

        auto res = Mat_VarWrite(mat, matvar, MAT_COMPRESSION_NONE);
        if (res != 0)
        {
          Mat_VarFree(matvar);
          throw DaiException(ErrorCodes::InternalError);
        }

        Mat_VarFree(matvar);
      }

      void IqMatlabWriter::writeMetadata(mat_t* const mat, const std::string& arrayname, const std::string& key, const std::string& value)
      {
        auto maxLen = max(key.size(), value.size());
        size_t dims[2] = { 2, maxLen };

        // pad to maxLen
        string writeKey = key;
        writeKey.resize(maxLen);
        string writeValue = value;
        writeValue.resize(maxLen);
      
        string writeData;
        writeData.resize(2 * maxLen);
        for (size_t i = 0; i < maxLen; ++i)
        {
          writeData[2*i] = writeKey[i];
          writeData[2*i + 1] = writeValue[i];
        }

        // MAT_T_UTF8 not defined in Mat_VarWrite4 -> use UINT8 instead
        matvar_t* matvar = Mat_VarCreate(arrayname.c_str(), MAT_C_CHAR, MAT_T_UINT8, 2, dims, (char*)writeData.c_str(), MAT_F_DONT_COPY_DATA);
        if (matvar == nullptr)
        {
          throw DaiException(ErrorCodes::InternalError);
        }

        auto res = Mat_VarWrite(mat, matvar, MAT_COMPRESSION_NONE);
        if (res != 0)
        {
          Mat_VarFree(matvar);
          throw DaiException(ErrorCodes::InternalError);
        }

        Mat_VarFree(matvar);
      }

      void IqMatlabWriter::writeDoubleValue(mat_t* const mat, const std::string& arrayName, double value)
      {
        size_t dims[2] = { 1, 1 };
        matvar_t* matvar = Mat_VarCreate(arrayName.c_str(), MAT_C_DOUBLE, MAT_T_DOUBLE, 2, dims, &value, MAT_F_DONT_COPY_DATA);
        if (matvar == nullptr)
        {
          throw DaiException(ErrorCodes::InternalError);
        }

        auto res = Mat_VarWrite(mat, matvar, MAT_COMPRESSION_NONE);
        if (res != 0)
        {
          Mat_VarFree(matvar);
          throw DaiException(ErrorCodes::InternalError);
        }

        Mat_VarFree(matvar);
      }
    }
  }
}
