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

/*!
* @file      iqmatlab_writer.h
*
* @brief     This is the header file of class IqMatlabWriter.
*
* @details   This class contains the implementation of IqMatlabWriter.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include "itempdir.h"

#include "common.h"

#include "matio.h"
#include "memory_mapped_file.hpp"

#include "dataimportexportbase.h"
#include "channelinfo.h"
#include "daiexception.h"
#include "errorcodes.h"
#include "enums.h"


namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief This class contains functionality needed to write I/Q matlab files with 
      * matlab file format 4 and 7.4.
      */
      class IqMatlabWriter
      {
      public:
        /**
          @brief Constructor. Initializes a new instance with the specified filename and parameters.
          Parameters cannot be changed after construction.
          @param [in]  filename Name of the file to be written.
          @param [in]  matlabVersion Matlab file version of the file to be written.
          @param [in]  dataFormat Iq data format of the data to be written
          @param [in]  nofArrays The number of arrays passed in appendArray() or appendChannel().
          @param [in]  applicationName Name of the application or instrument exporting its data.
          @param [in]  comment Text that further describes the contents of the file.
          @param [in]  timestamp Timestamp indicating when meta data was written to file.
          @param [in]  channelInfos Name, clock rate and center frequency for each channel to be saved.
          @param [in]  tempPath Path used to write temporary files to.
          @param [in]  metadata Additional non-standardized meta data as key - value pairs to be saved.
        */IqMatlabWriter(const std::string& filename,
            MatlabVersion matlabVersion,
            IqDataFormat dataFormat,
            size_t nofArrays,
            const std::string& applicationName,
            const std::string& comment,
            const time_t timestamp,
            const std::vector<ChannelInfo>& channelInfos,
            const std::string& tempPath,
            const std::map<std::string, std::string>* metadata = 0);

        /** @brief Destructor. Calls close()
        */~IqMatlabWriter();

        /**
          @brief Opens the file for writing and verifies that the names of the specified channels
          are unique.
        */void open();

        /**
          @brief Closes the file and releases the file handle. Depending on the file format, the 
          final file is written at this point, thus this can be a long running operation.
        */void close();

        /**
          @brief Adds the specified I/Q data to the existing data record. Data is first written to 
          temporary files and added to the actual .mat file when close() is called. 
          The number of the I/Q data arrays passed to this method as well as the lengths of the arrays
          are validated w.r.t the specified channel information. In case of a mismatch between the channel
          information and passed data, an exception is thrown. Iq-data will always be written to 
          matlab arrays of type MAT_C_DOUBLE.
          @tparam Precision of the value array - single or double.
          @param [in]  iqdata Vector containing I/Q data arrays.
          @param [in]  sizes The length of the specified data arrays.
          @throws DaiException(FileWriterUninitialized) Thrown if this class was not initialized. Call open() first.
          @throws DaiException(InconsistentInputData) Thrown if channel information and I/Q data arrays sizes are not consistent.
          @throws DaiException(InternalError) Thrown if data could not be saved to file.
        */template<typename T>
        void appendArray(const std::vector<T*>& iqdata, const std::vector<size_t>& sizes)
        {
          // check if file writer is initialized
          if (false == this->initialized_)
          {
            throw DaiException(ErrorCodes::FileWriterUninitialized);
          }

          // init data type to float or double. 
          DataImportExportBase::initializeDataType<T>(this->dataType_, this->lockDataType_);

          // validate input data consistence
          if (false == DataImportExportBase::validateInputData(this->channelInfos_, sizes, this->dataFormat_))
          {
            throw DaiException(ErrorCodes::InconsistentInputData);
          }

          try
          {
            for (size_t i = 0; i < iqdata.size(); i++)
            {
              // we always write MAT_T_DOUBLE, so already cast when writing to tmp file.
              size_t writeSize = sizes[i] * sizeof(double);
              size_t writeOffset = this->mmfWriters_[i].file_size();

              // get memory to write data to
              this->mmfWriters_[i].map(writeOffset, writeSize);
              Common::mmfDataAssert(this->mmfWriters_[i]);

              // copy data to memory mapped file with double-precision
              std::copy(iqdata[i], iqdata[i] + sizes[i], reinterpret_cast<double*>(this->mmfWriters_[i].data()));

              // unmap mmf
              this->mmfWriters_[i].flush();
              this->mmfWriters_[i].unmap();
            }
          }
          catch (const std::exception &e)
          {
            throw DaiException(ErrorCodes::InternalError, e.what());
          }
        }

        /**
          @brief Add the specified I/Q channel to the existing data record. Data is first written to 
          temporary files and added to the actual .mat file when close() is called. 
          The number of the I/Q data arrays passed to this method as well as the lengths of the arrays
          are validated w.r.t the specified channel information. In case of a mismatch between the channel
          information and passed data, an exception is thrown. Iq-data will always be written to 
          matlab arrays of type MAT_C_DOUBLE.
          @tparam Precision of the value array - single or double.
          @param [in]  iqdata Vector containing I/Q data in interleaved format.
          @param [in]  sizes The length of the specified data arrays.
          @throws DaiException(FileWriterUninitialized) Thrown if this class was not initialized. Call open() first.
          @throws DaiException(InconsistentInputData) Thrown if channel information and I/Q data arrays sizes are not consistent.
          @throws DaiException(InternalError) Thrown if data could not be saved to file.
        */template<typename T>
        void appendChannel(const std::vector<T*>& iqdata, const std::vector<size_t>& sizes)
        {
          // write real data
          if (this->dataFormat_ == IqDataFormat::Real)
          {
            this->appendArray(iqdata, sizes);
            return;
          }
          
          // do not move up, appendArray() does check initialization itself.
          if (false == this->initialized_)
          {
            throw DaiException(ErrorCodes::FileWriterUninitialized);
          }

          if (this->channelInfos_.size() != iqdata.size() || this->channelInfos_.size() != sizes.size())
          {
            throw DaiException(ErrorCodes::InconsistentInputData);
          }

          DataImportExportBase::initializeDataType<T>(this->dataType_, this->lockDataType_);

          try
          {
            for (size_t i = 0; i < iqdata.size(); i++)
            {
              // we always write MAT_T_DOUBLE, so already cast when writing to tmp file.
              size_t writeSize = sizes[i] / 2 * sizeof(double);
              size_t writeOffsetI = this->mmfWriters_[2*i].file_size();
              size_t writeOffsetQ = this->mmfWriters_[2*i + 1].file_size();

              // get file writers for I and Q file and create memory mapping for number of samples to be written
              this->mmfWriters_[2*i].map(writeOffsetI, writeSize);
              this->mmfWriters_[2*i + 1].map(writeOffsetQ, writeSize);
              Common::mmfDataAssert(this->mmfWriters_[2*i]);
              Common::mmfDataAssert(this->mmfWriters_[2*i + 1]);

              // write I data with double-precision
              // stride iterator will return every second element from vector -> only I data
              std::copy(
                stride_iterator<T*>(iqdata[i], 2),
                stride_iterator<T*>(iqdata[i] + sizes[i], 2),
                reinterpret_cast<double*>(this->mmfWriters_[2*i].data()));

              // write Q data with double-precision
              // stride iterator will return every second element from vector -> only Q data
              std::copy(
                stride_iterator<T*>(iqdata[i] + 1, 2),
                stride_iterator<T*>(iqdata[i] + sizes[i] - 1, 2),
                reinterpret_cast<double*>(this->mmfWriters_[2*i + 1].data()));
              *(reinterpret_cast<double*>(this->mmfWriters_[2*i + 1].data()) + sizes[i] / 2 - 1) = *(iqdata[i] + sizes[i] - 1);

              this->mmfWriters_[2*i].flush();
              this->mmfWriters_[2*i + 1].flush();
              this->mmfWriters_[2*i].unmap();
              this->mmfWriters_[2*i + 1].unmap();
            }
          }
          catch (const std::exception &e)
          {
            throw DaiException(ErrorCodes::InternalError, e.what());
          }
        }

      private:
        /** @brief Private default constructor. */
        IqMatlabWriter();

        /** @brief Private copy constructor. */
        IqMatlabWriter(const IqMatlabWriter&);

        /** @brief Private assignment operator.*/
        IqMatlabWriter& operator=(const IqMatlabWriter&);

        /**
          @brief Writes a string to a matlab char array.
          @param [in]  mat The matlab file handle used to write the data.
          @param [in]  arrayName Name of the matlab array.
          @param [in]  value The string value to be written. 
        */static void writeCharArray(mat_t* const mat, const std::string& arrayName, const std::string& value);

        /**
          @brief Writes a double value to a matlab array of type double with dimensions (1x1).
          @param [in]  mat The matlab file handle used to write the data.
          @param [in]  arrayName Name of the matlab array.
          @param [in]  value The double value to be written.
        */static void writeDoubleValue(mat_t* const mat, const std::string& arrayName, double value);

        /**
          @brief Writes meta data as matlab character array with two rows. The first row will
          contain the meta data "key" and the second row will contain the "value" of the meta data.
          The number of columns of the matlab array will be max(key.size(), value.size()).
          @param [in]  mat The matlab file handle used to write the data.
          @param [in]  arrayName Name of the matlab array.
          @param [in]  key Meta data key to be written
          @param [in]  value Meta data value to be written.
        */static void writeMetadata(mat_t* const mat, const std::string& arrayName, const std::string& key, const std::string& value);

        /**
          @brief Writes all meta data defined by this class to the specified matlab file. 
          More detailed, the data written is: application name, comment, timestamp, I/Q data format,
          channel information for each channel, user-defined meta data.
          @param [in]  mat The matlab file handle used to write the data.
        */void writeMetadata(mat_t* const mat);

        /** 
          @brief Used to transfer I/Q data from temporary files to the
          final .mat file when iq format type is IqDataFormat::Real.
        */void writeRealData(mat_t* const mat);

        /**
          @brief Used to transfer I/Q data from temporary files to the
          final .mat file when iq format type is IqDataFormat::Complex.
        */void writeData(mat_t* const mat);

        /**
         @brief Closes the temporary file writers and transfers the written
         I/Q data to the matlab file.
        */void finalizeTemporarySequence();

        /**
          @brief Deletes the temporary files created by this class. 
          File handle must be released before calling this method. 
        */void deleteTempFiles();

        /** @brief TRUE if file was sucessfully initialized, i.e. open() was called. */
        bool initialized_;

        /** @brief Filename of the matlab file */
        const std::string filename_;

        /** @brief Vector containing the paths of all temporary files used.
        *	The number of temporary files depends on the I/Q data format as well as 
        *	the number of channels. For instance, complex data requires 2 temp files per
        *	channel. 
        */std::vector<std::string> tempFiles_;

        /** @brief Path to the directory used to save temporary files. */
        std::string tempPath_;

        /** @brief Matlab file version used to write file. */
        MatlabVersion matVersion_;

        /** @brief Number of data arrays. */
        size_t nofArrays_;

        /** @brief Temporary file writers. */
        std::vector<memory_mapped_file::writable_mmf> mmfWriters_;

        /** @brief Format of iq data. */
        IqDataFormat dataFormat_;

        /** @brief Precision used to write data to file. */
        IqDataType dataType_;

        /** @brief If set TRUE, \ref dataType_ cannot be changed. Value is set at first call
        * of \ref appendArray() or \ref appendChannel(). 
        */bool lockDataType_;

        /** @brief Name of the application or instrument exporting its data. */
        std::string applicationName_;

        /** @brief Text that further describes the contents of the file. */
        std::string comment_;

        /** @brief Timestamp indicating when meta data was written to file. */
        time_t timestamp_; 

        /** @brief Name, clock rate and center frequency for each channel to be saved. */
        std::vector<ChannelInfo> channelInfos_; 

        /** @brief Additional non-standardized meta data as key - value pairs to be saved. 
        */std::map<std::string, std::string> metadata_; 
      };
    }
  }
}