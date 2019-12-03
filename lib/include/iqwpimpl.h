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
* @file      iqwpimpl.h
*
* @brief     This is the header file of class Iqw::Impl.
*
* @details   Private implementation of class Iqw.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include "itempdir.h"

#include "common.h"

#include "memory_mapped_file.hpp"

#include "iqw.h"
#include "dataimportexportbase.h"
#include "enums.h"
#include "errorcodes.h"
#include "daiexception.h"
#include "channelinfo.h"
#include "stride_iterator.h"
#include "platform.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief Private implementation of class Iqw.
      */
      class Iqw::Impl final : public DataImportExportBase, ITempDir
      {
      public:
        /**
          @brief Constructor. Initializes a new instance with the specified filename.
          Filename must be UTF-8 encoded. Be aware that the filename cannot be changed after construction.
          @param [in]  filename Name of the file to be read or to be written.
        */Impl(const std::string& filename);

        /** @brief Destructor. Calls close() 
        */~Impl();

        int readOpen(std::vector<std::string>& arrayNames);
        int writeOpen(
          IqDataFormat format,
          size_t nofArrays,
          const std::string& applicationName,
          const std::string& comment,
          const std::vector<ChannelInfo>& channelInfos,
          const std::map<std::string, std::string>* metadata = 0);
        int close();

        int64_t getArraySize(const std::string& arrayName) const;

        /**
          @returns Gets the order of the I/Q data as written to file.
        */IqDataOrder getDataOrder() const;

       /**
          @brief Sets the order of the I/Q data. Must be set before \ref readOpen() or \ref writeOpen() 
          is called. The default value is IqDataOrder::IIIQQQ. If data is written in non-interleaved order, 
          temporary files must be written, which impairs the write performance. When reading data make sure to 
          match the data order used by the file.
          @param [in]  dataOrder The order of the I/Q samples.
          @returns Returns ErrorCode::ReaderAlreadyInitialized or ErrorCode::WriterAlreadyInitialized
          if the file reader or the file writer have already been initialized respectively.
          In this case, the data order cannot be changed anymore.
        */int setDataOrder(IqDataOrder dataOrder);

        int readArray(const std::string& arrayName, std::vector<float>& values, size_t nofValues, size_t offset = 0);
        int readArray(const std::string& arrayName, float* values, size_t nofValues, size_t offset = 0);
        int readArray(const std::string& arrayName, std::vector<double>& values, size_t nofValues, size_t offset = 0);
        int readArray(const std::string& arrayName, double* values, size_t nofValues, size_t offset = 0);

        int readChannel(const std::string& channelName, std::vector<float>& values, size_t nofValues, size_t offset = 0);
        int readChannel(const std::string& channelName, float* values, size_t nofValues, size_t offset = 0);
        int readChannel(const std::string& channelName, std::vector<double>& values, size_t nofValues, size_t offset = 0);
        int readChannel(const std::string& channelName, double* values, size_t nofValues, size_t offset = 0);

        int appendArrays(const std::vector<std::vector<float>>& iqdata);
        int appendArrays(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes);
        int appendArrays(const std::vector<std::vector<double>>& iqdata);
        int appendArrays(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes);

        int appendChannels(const std::vector<std::vector<float>>& iqdata);
        int appendChannels(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes);
        int appendChannels(const std::vector<std::vector<double>>& iqdata);
        int appendChannels(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes);

        int setTempDir(const std::string& path);
        std::string getTempDir() const;

      private:
        /** @brief Private default constructor. */
        Impl(const Impl&);

        /** @brief Private assignment operator.*/
        Impl& operator=(const Impl&);

        /**
          @brief Validates whether or not the specified array name is valid.
          Valid names are e.g. "Channel1_I", and "Channel1_Q". The channel count always starts from 1.
          @param [in]  arrayName Name of the array name to check.
          @param [out]  readI TRUE if the array hold I values, FALSE if the array holds Q values.
          @returns Returns TRUE if the array name is valid, FALSE otherwise.
        */static bool isArrayNameValid(const std::string& arrayName, bool& readI);

        /**
          @brief Helper function to get key reading parameters for accessing a binary IQW file.
          @param [in]  fileSize Size of the used file in bytes.
          @param [in]  readInterleaved TRUE if the complete channel shall be read at once in interleaved mode, not
          only I or Q part.
          @param [in]  offset Defines the start position in the I/Q data record at which the read operation is started.
          @param [out]  readNofValues Number of values to read.
          @param [out]  arraySize Size required to read data to an array, based on the specified offset, number of values to read, and interleaved setting.
          @param [out]  readOffsetI File offset before first I value can be read in [byte]. Calculation based on the specified offset value.
          @param [out]  readOffsetQ File offset before first Q value can be read in [byte].Calculation based on the specified offset value.
        */void getReadParameters(
          size_t fileSize,
          bool readInterleaved,
          size_t offset,
          size_t readNofValues,
          size_t& arraySize,
          size_t& readOffsetI,
          size_t& readOffsetQ);

        /**
          @brief Creates the final IQW file if data was written in non-interleaved mode (IIIQQQ).
          In this case, the two temporary I and Q files are merged to one file. This might be
          a long-running operation.
        */void finalizeTemporarySequence();

        /** @brief Deletes temporary files created while writing IQW file with data order IIIQQQ. 
        */void deleteTempFiles();

        /**
          @brief Reads I/Q data from the specified array name. This methods prepares the actual
          read operation by calculating the necessary parameters, followed by a call of readData().
          @tparam Template parameter of the destination I/Q data precision, i.e. float or double.
          @param [in]  arrayName The name of the array to read.
          @param [out]  values The values read. Data read from file will be converted to single precision,
          independent of the original data type.
          @param [in]  nofValues Number of values to read. Make sure to provide sufficient memory to store the values.
          @param [in]  offset Defines the start position in the I/Q data record at which the read operation is started.
          @throws DaiException(OpenFileHasNotBeenCalled) If the file has not been opened. Call readOpen() first.
          @throws DaiException(InvalidArrayName) If the specified array name was not found in the file.
          @throws DaiException(NoDataFoundInFile) If the tar file does not contain a binary I/Q data file.
          @throws DaiException(StartIndexOutOfRange) If the offset position exceeds the number of I/Q values stored in the data file.
          @throws DaiException(InvalidDataInterval) If the combination of offset and number of values to read exceeds the available 
          number of I/Q values stored in the file.
          @throws DaiException(InternalError) If an error occurred while accessing the file.
        */template<typename T>
        void readArrayInternal(const std::string& arrayName, T* values, size_t nofValues, size_t offset)
        {
          // is file ready to read?
          if (0 == this->getChannelInfos().size())
          {
            throw DaiException(ErrorCodes::OpenFileHasNotBeenCalled);
          }

          // check if specified array name is valid
          bool readI = true;
          if (false == this->isArrayNameValid(arrayName, readI))
          {
            throw DaiException(ErrorCodes::InvalidArrayName);
          }

          // get read parameters
          size_t readOffsetI = 0;
          size_t readOffsetQ = 0;
          size_t pairs = 0; 
          size_t fileSize = Common::getFileSize(this->filename_);
          this->getReadParameters(fileSize, false, offset, nofValues, pairs, readOffsetI, readOffsetQ);

          // read actual data
          this->readData(nofValues, readOffsetI, readOffsetQ, readI, values);
        }

        /**
          @brief Reads I/Q data from the specified channel. This methods prepares the actual 
          read operation by calculating the necessary parameters, followed by a call of readDataInterleaved().
          @tparam Template parameter of the destination I/Q data precision, i.e. float or double.
          @param [in]  channelName The name of the channel to read.
          @param [out]  values The values read. Data read from file will be converted to single precision,
          independent of the original data type.
          @param [in]  nofValues Number of values to read. Make sure to provide sufficient memory to store the values.
          @param [in]  offset Defines the start position in the I/Q data record at which the read operation is started.
          @throws DaiException(OpenFileHasNotBeenCalled) If the file has not been opened. Call readOpen() first.
          @throws DaiException(InvalidArrayName) If the specified array name was not found in the file.
          @throws DaiException(NoDataFoundInFile) If the tar file does not contain a binary I/Q data file.
          @throws DaiException(StartIndexOutOfRange) If the offset position exceeds the number of I/Q values stored in the data file.
          @throws DaiException(InvalidDataInterval) If the combination of offset and number of values to read exceeds the available 
          number of I/Q values stored in the file.
          @throws DaiException(InternalError) If an error occurred while accessing the file.
        */template<typename T>
        void readChannelInternal(const std::string& channelName, T* values, size_t nofValues, size_t offset)
        {
          // is file ready to read?
          if (0 == this->getChannelInfos().size())
          {
            throw DaiException(ErrorCodes::OpenFileHasNotBeenCalled);
          }

          // check if specified array name is valid
          std::string fullChannelName = channelName + "_I";
          if (0 != fullChannelName.compare(Iqw::Impl::DefaultArrayNameI_))
          {
            throw DaiException(ErrorCodes::InvalidArrayName);
          }

          // get read parameters
          size_t readOffsetI = 0;
          size_t readOffsetQ = 0;
          size_t pairs = 0;
          size_t fileSize = Common::getFileSize(this->filename_);
          this->getReadParameters(fileSize, true, offset, nofValues, pairs, readOffsetI, readOffsetQ);

          // read actual data
          this->readDataInterleaved(nofValues, readOffsetI, readOffsetQ, values);
        }

        /**
          @brief Reads I/Q data from the binary IQW file utilizing a memory mapped file.
          @tparam Template parameter of the destination I/Q data precision, i.e. float or double.
          @param [in]  nofValues Number of values to read. Make sure to provide sufficient memory to store the values.
          @param [in]  readOffset Read offset for I values, given in bytes.
          @param [in]  readOffsetQ Read offset for Q values, given in bytes.
          @param [in]  readIValues If TRUE, I-values are read, otherwise Q-values.
          @param [out]  values The values read. Data read from file will be converted to single precision,
          independent of the original data type.
          @throws DaiException(InternalError) If an error occurred while accessing the file.
        */template<typename T>
        void readData(size_t nofValues, size_t readOffset, size_t readOffsetQ, bool readIValues, T& values)
        {
          try
          {
            // open mmf
            memory_mapped_file::read_only_mmf mmf;
            Platform::mmfOpen(mmf, this->filename_, false);

            // interleaved data order
            if (this->dataOrder_ == IqDataOrder::IQIQIQ)
            {
              // calculate readSize and readOffset in bytes depended on I or Q values; IQW data is always single-precision.
              size_t setReadOffset = readIValues ? readOffset : readOffsetQ;
              size_t readSize = 2 * nofValues * sizeof(float);

              // align memory
              mmf.map(setReadOffset, readSize);
              Common::mmfDataAssert(mmf);

              // stride iterator returns only every second value. Therewith we read either I or Q, dependent
              // on 'setReadOffset'
              std::copy(
                stride_iterator<const float*>(reinterpret_cast<const float*>(mmf.data()), 2),
                stride_iterator<const float*>(reinterpret_cast<const float*>(mmf.data()) + 2 * nofValues, 2),
                values);
            }
            else // IIIQQQ
            {
              // calculate readSize and readOffset in bytes
              size_t setReadOffset = readIValues ? readOffset : readOffsetQ;
              size_t readSize = nofValues * sizeof(float);

              // align memory
              mmf.map(setReadOffset, readSize);
              Common::mmfDataAssert(mmf);

              // copy values from file to values-vector
              std::copy(reinterpret_cast<const float*>(mmf.data()), reinterpret_cast<const float*>(mmf.data()) + nofValues, values);
            }

            mmf.close();
          }
          catch (const std::exception &e)
          {
            throw DaiException(ErrorCodes::InternalError, e.what());
          }
        }

        /**
          @brief Reads I/Q data from file utilizing a memory mapped file and returns the data in interleaved format (IQIQIQ) .
          @param [in]  nofValues Number of values to read. Make sure to provide sufficient memory to store the values.
          @param [in]  readOffset Read offset for I values, given in bytes.
          @param [in]  readOffsetQ Read offset for Q values, given in bytes.
          @param [out]  values The values read. Data read from file will be converted to single precision,
          independent of the original data type.
          @throws DaiException(InternalError) If an error occurred while accessing the file.
        */template<typename T>
        void readDataInterleaved(size_t nofValues, size_t readOffset, size_t readOffsetQ, T* values)
        {
          try
          {
            // open mmf
            memory_mapped_file::read_only_mmf mmf;
            Platform::mmfOpen(mmf, this->filename_, false);

            // interleaved data order
            if (this->dataOrder_ == IqDataOrder::IQIQIQ)
            {
              // calculate number of bytes to read; IQW is always single precision
              size_t readSize = nofValues * sizeof(float);

              // align memory
              mmf.map(readOffset, readSize);
              Common::mmfDataAssert(mmf);

              // copy values from file to value-vector
              std::copy(reinterpret_cast<const float*>(mmf.data()), reinterpret_cast<const float*>(mmf.data()) + nofValues, values);
            }
            else  // IqDataOrder.IIIQQQ
            {
              // read data in two steps, first I values followed by Q values -> only read nofValues/2; 
              // calculate number of bytes to read
              size_t nofPairs = nofValues / 2;
              size_t readSize = nofPairs * sizeof(float);

              // align memory to read I values
              mmf.map(readOffset, readSize);
              Common::mmfDataAssert(mmf);

              // copy I values from file to values-vector
              auto cdata = reinterpret_cast<const float*>(mmf.data());
              for (size_t i = 0, pos = 0; i < nofPairs; i++, pos += 2)
              {
                values[pos] = cdata[i];
              }

              // align memory to read Q values
              mmf.map(readOffsetQ, readSize);
              Common::mmfDataAssert(mmf);

              // copy Q values from file to values-vector
              cdata = reinterpret_cast<const float*>(mmf.data());
              for (size_t i = 0, pos = 1; i < nofPairs; i++, pos += 2)
              {
                values[pos] = cdata[i];
              }
            }

            mmf.close();
          }
          catch (const std::exception &e)
          {
            throw DaiException(ErrorCodes::InternalError, e.what());
          }
        }

        /**
          @brief Writes I/Q array data to file. Dependent on the data order, data is written to either  
          temporary files or immediately to the final IQW file.
          @tparam Template parameter of the I/Q data precision, i.e. float or double.
          @param [in]  iqdata Vector containing pointers to the I/Q data to be saved.
          @param [in]  sizes Vector containing the lengths of the iqdata arrays.
          @throws DaiException(FileWriterUninitialized) Thrown if this class was not initialized. Call writeOpen() first.
          @throws DaiException(InconsistentInputData) If input data does not match file format criteria.
          @throws DaiException(InternalError) If an error occurred while accessing the file.
        */template<typename T>
        void writeArrayInternal(const std::vector<T*>& iqdata, const std::vector<size_t>& sizes)
        {
          if (false == this->writerInitialized_)
          {
            throw DaiException(ErrorCodes::FileWriterUninitialized);
          }

          if (iqdata.size() != 2 || iqdata.size() != sizes.size() || sizes[0] != sizes[1])
          {
            throw DaiException(ErrorCodes::InconsistentInputData);
          }

          if (this->dataOrder_ == IqDataOrder::IIIQQQ)
          {
            this->writeTemporaryArraySequence(iqdata, sizes);
          }
          else
          {
            this->writeArray(iqdata, sizes);
          }
        }

        /**
          @brief Writes non-interleaved I/Q array data to temporary files.
          @tparam Template parameter of the I/Q data precision, i.e. float or double.
          @param [in]  iqdata Vector containing pointers to the I/Q data to be saved.
          @param [in]  sizes Vector containing the lengths of the iqdata arrays.
          @throws DaiException(InternalError) If an error occurred while accessing the file.
        */template<typename T>
        void writeTemporaryArraySequence(const std::vector<T*>& iqdata, const std::vector<size_t>& sizes)
        {
          try
          {
            // calculate number of bytes to write for I or Q data vector.
            size_t writeSize = sizes[0] * sizeof(float);

            // get current position of memory mapping
            size_t writeOffsetI = this->mmfWriterOne_.file_size();
            size_t writeOffsetQ = this->mmfWriterTwo_.file_size();

            // align memory for I and Q writer
            this->mmfWriterOne_.map(writeOffsetI, writeSize);
            Common::mmfDataAssert(this->mmfWriterOne_);
            this->mmfWriterTwo_.map(writeOffsetQ, writeSize);
            Common::mmfDataAssert(this->mmfWriterTwo_);
            
            // copy I and Q data to file; IQW only supports single-precision
            std::copy(iqdata[0], iqdata[0] + sizes[0], reinterpret_cast<float*>(this->mmfWriterOne_.data()));
            std::copy(iqdata[1], iqdata[1] + sizes[1], reinterpret_cast<float*>(this->mmfWriterTwo_.data()));

            // flush writers
            this->mmfWriterOne_.flush();
            this->mmfWriterTwo_.flush();
            this->mmfWriterOne_.unmap();
            this->mmfWriterTwo_.unmap();
          }
          catch (const std::exception &e)
          {
            throw DaiException(ErrorCodes::InternalError, e.what());
          }
        }

        /**
          @brief Writes interleaved I/Q array data to the IQW file.
          @tparam Template parameter of the I/Q data precision, i.e. float or double.
          @param [in]  iqdata Vector containing pointers to the I/Q data to be saved.
          @param [in]  sizes Vector containing the lengths of the iqdata arrays.
          @throws DaiException(InternalError) If an error occurred while accessing the file.
        */template<typename T>
        void writeArray(const std::vector<T*>& iqdata, const std::vector<size_t>& sizes)
        {
          try
          {
            // calculate number of bytes to written for I and Q data
            size_t writeSize = 2 * sizes[0] * sizeof(float);

            // get end of file and align memory
            size_t writeOffset = this->mmfWriterOne_.file_size();
            this->mmfWriterOne_.map(writeOffset, writeSize);

            // mix data of separated I and Q values from the corresponding vectors
            // to interleaved format IQIQIQ and copy to file.
            Common::mergeInterleaved(
              iqdata[0], iqdata[0] + sizes[0],
              iqdata[1], 
              reinterpret_cast<float*>(this->mmfWriterOne_.data()));

            this->mmfWriterOne_.flush();
            this->mmfWriterOne_.unmap();
          }
          catch (const std::exception &e)
          {
            throw DaiException(ErrorCodes::InternalError, e.what());
          }
        }

        /**
          @brief Writes I/Q channels to file. Dependent on the data order, data is written to either  
          temporary files or immediately to the final IQW file.
          @tparam Template parameter of the I/Q data precision, i.e. float or double.
          @param [in]  iqdata Vector containing pointers to the I/Q data to be saved.
          @param [in]  sizes Vector containing the lengths of the iqdata arrays.
          @throws DaiException(FileWriterUninitialized) Thrown if this class was not initialized. Call writeOpen() first.
          @throws DaiException(InconsistentInputData) If input data does not match file format criteria.
          @throws DaiException(InternalError) If an error occurred while accessing the file.
        */template<typename T>
        void writeChannelInternal(const std::vector<T*>& iqdata, const std::vector<size_t>& sizes)
        {
          if (false == this->writerInitialized_)
          {
            throw DaiException(ErrorCodes::FileWriterUninitialized);
          }

          if (iqdata.size() != 1 || iqdata.size() != sizes.size() || sizes[0] % 2 != 0)
          {
            throw DaiException(ErrorCodes::InconsistentInputData);
          }

          if (this->dataOrder_ == IqDataOrder::IIIQQQ)
          {
            this->writeTemporaryChannelSequence(iqdata[0], sizes[0]);
          }
          else
          {
            this->writeChannel(iqdata[0], sizes[0]);
          }
        }

        /**
          @brief Writes non-interleaved I/Q channel data to temporary files.
          @tparam Template parameter of the I/Q data precision, i.e. float or double.
          @param [in]  iqdata Vector containing pointers to the I/Q data to be saved.
          @param [in]  size The lengths of the iqdata array.
          @throws DaiException(InternalError) If an error occurred while accessing the file.
        */template<typename T>
        void writeTemporaryChannelSequence(T* iqdata, size_t size)
        {
          try
          {
            // calculated number of bytes to be written for each file -> nofValues / 2
            size_t writeSize = (size * sizeof(float)) / 2;
            size_t writeOffsetI = this->mmfWriterOne_.file_size();
            size_t writeOffsetQ = this->mmfWriterTwo_.file_size();

            // align amemory
            this->mmfWriterOne_.map(writeOffsetI, writeSize);
            Common::mmfDataAssert(this->mmfWriterOne_);
            this->mmfWriterTwo_.map(writeOffsetQ, writeSize);
            Common::mmfDataAssert(this->mmfWriterTwo_);

            // split the interleaved data of iqdata array to I and Q data and 
            // write to dedicated I and Q temp files
            Common::split(
              iqdata,
              reinterpret_cast<float*>(this->mmfWriterOne_.data()),
              reinterpret_cast<float*>(this->mmfWriterTwo_.data()),
              size);

            this->mmfWriterOne_.flush();
            this->mmfWriterTwo_.flush();
            this->mmfWriterOne_.unmap();
            this->mmfWriterTwo_.unmap();
          }
          catch (const std::exception &e)
          {
            throw DaiException(ErrorCodes::InternalError, e.what());
          }
        }

        /**
          @brief Writes interleaved I/Q channels to the IQW file.
          @tparam Template parameter of the I/Q data precision, i.e. float or double.
          @param [in]  iqdata Vector containing pointers to the I/Q data to be saved.
          @param [in]  size The lengths of the iqdata array.
          @throws DaiException(InternalError) If an error occurred while accessing the file.
        */template<typename T>
        void writeChannel(T* iqdata, size_t size)
        {
          try
          {
            // calculate number of bytes to be written
            size_t writeSize = size * sizeof(float);
            size_t writeOffset = this->mmfWriterOne_.file_size();

            // align memory
            this->mmfWriterOne_.map(writeOffset, writeSize);

            // copy interleaved I/Q values to file
            std::copy(iqdata, iqdata + size, reinterpret_cast<float*>(this->mmfWriterOne_.data()));

            this->mmfWriterOne_.flush();
            this->mmfWriterOne_.unmap();
          }
          catch (const std::exception &e)
          {
            throw DaiException(ErrorCodes::InternalError, e.what());
          }
        }

      private:
        /** @brief Default array name containing I-values */
        static const std::string DefaultArrayNameI_;

        /** @brief Default array name containing Q-values */
        static const std::string DefaultArrayNameQ_;

        /** @brief Specifies whether or not data is stored in interleaved format. */
        IqDataOrder dataOrder_;

        /** @brief Path used to write temporary I data. */
        std::string tempFileI_;

        /** @brief Path used to write temporary Q data. */
        std::string tempFileQ_;
        
        /** @brief Path to the directory used to save temporary files. */
        std::string tempPath_;

        /** @brief MMF writer used to write temp. I data */
        memory_mapped_file::writable_mmf mmfWriterOne_;

        /** @brief MMF writer used to write temp. Q data */
        memory_mapped_file::writable_mmf mmfWriterTwo_;

        /** @brief Indicates whether or not the file has been initialized for reading. If
        * initialized in read-mode, file cannot save new data. 
        */bool readerInitialized_;

        /** @brief Indicates whether or not the file has been initialized for writing. If
        * initialized in write-mode, file cannot read data. 
        */bool writerInitialized_;
      };
    }
  }
}