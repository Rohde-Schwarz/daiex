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
* @file      iqtar_writer.h
*
* @brief     This is the header file of class IqTarWriter.
*
* @details   This class contains the implementation of IqMatlabWriter.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include "itempdir.h"

#include "common.h"

#include <map>
#include <complex>

#include "archive.h"
#include "memory_mapped_file.hpp"

#include "dataimportexportbase.h"
#include "iqtar_preview.h"
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
        @brief This class contains the implementation of IqMatlabWriter. 
      */
      class IqTarWriter
      {
      public:
        /**
          @brief Constructor. Initializes a new instance with the specified filename and parameters.
          Parameters cannot be changed after construction.
          @param [in]  filename Name of the file to be written.
          @param [in]  dataFormat Iq data format of the data to be written
          @param [in]  dataType Iq data type of the data to be written. Data type will automatically be resolved when data is
          written the first time via appendArrays or appendChannels. However, if temp-files are disabled, the type of the data
          to be written must be known in advance to generate the corresponding data file name (generateIqDataFilename) for the tar.
          @param [in]  nofArrays The number of arrays passed in appendArray() or appendChannel().
          @param [in]  applicationName Name of the application or instrument exporting its data.
          @param [in]  comment Text that further describes the contents of the file.
          @param [in]  timestamp Timestamp indicating when meta data was written to file.
          @param [in]  channelInfos Name, clock rate and center frequency for each channel to be saved.
          @param [in]  enablePreview If set TRUE, a preview of the I/Q data will be calculated and saved as meta data to the XML-file
          every time new data is added.
          @param [in]  tempPath Path used to write temporary files to.
          @param [in]  metadata Additional non-standardized meta data as key - value pairs to be saved.
          @param [in]  deprecatedInfoXml Information that can be added to the XML file at hierarchy level 
          &lt;UserData&gt;&lt;RohdeSchwarz&gt;DEPRECATED_INFO_STRING, as required by FSW. Make sure to pass a valid XML string.
          @param [in]  expectedNofIqBytes Expected file size of the i/q data file in bytes. If &gt; 0, no temp file will be
          generated, but the actual tar file is written directly. The value must then match the actual number of bytes written.
        */IqTarWriter(
          const std::string& filename,
          IqDataFormat dataFormat,
          IqDataType dataType, 
          size_t nofArrays,
          const std::string& applicationName,
          const std::string& comment,
          const time_t timestamp,
          const std::vector<ChannelInfo>& channelInfos,
          bool enablePreview,
          const std::string& tempPath,
          const std::map<std::string, std::string>* metadata = 0,
          const std::string* deprecatedInfoXml = 0,
          const uint64_t expectedNofIqBytes = 0);

        /** @brief Destructor. Calls close()
        */~IqTarWriter();

        /**
          @brief Prepares the IqTarWriter to write a file:
          - Checks if the provided channel information is valid,
          - Opens a temporary writer that buffers i/q data to disk until
          the final iq.tar file is written.
          - Initializes the preview of i/q data that is written to the xml file.
          @throws FileNotFound if the provided filename is not valid.
          @throws InconsistentInputData if the provided channel information is invalid.
          @throws FileOpenError if the temporary file writer could not be initialized.
        */void open();

        /**
          @brief Closes the temporary file writer and creates the final iq.tar file.
          Finally, the temporary files are deleted.
          @throws InternalError In case of any error.
        */void close();

        /**
          @brief Adds the specified I/Q data to the existing data record. Data is first written to 
          temporary files and added to the actual .iq.tar file when close() is called. 
          The number of the I/Q data arrays passed to this method as well as the lengths of the arrays
          are validated w.r.t the specified channel information. In case of a mismatch between the channel
          information and passed data, an exception is thrown. Further, a preview is generated from the added data,
          which is saved as additional metadata.
          @tparam Precision of the value array - single or double.
          @param [in]  iqdata Vector containing I/Q data arrays.
          @param [in]  sizes The length of the specified data arrays.
          @throws DaiException(FileWriterUninitialized) Thrown if this class was not initialized. Call open() first.
          @throws DaiException(InconsistentInputData) Thrown if channel information and I/Q data arrays sizes are not consistent.
          @throws DaiException(InternalError) Thrown if data could not be saved to file.
        */template<typename T>
        void appendArray(const std::vector<T*>& iqdata, const std::vector<size_t>& sizes)
        {
          if (false == this->initialized_)
          {
            throw DaiException(ErrorCodes::FileWriterUninitialized);
          }

          DataImportExportBase::initializeDataType<T>(this->dataType_, this->lockDataType_);

          if (false == this->validateInputDataArray(iqdata.size(), sizes))
          {
            throw DaiException(ErrorCodes::InconsistentInputData);
          }

          // write to tmp file
          if (this->expectedNofIqBytes_ == 0)
          {
            if (this->dataType_ == IqDataType::Float32)
            {
              this->writeTemporaryArraySequence<float>(iqdata, sizes);
            }
            else
            {
              this->writeTemporaryArraySequence<double>(iqdata, sizes);
            }
          }
          else // write directly to tar
          {
            if (this->dataType_ == IqDataType::Float32)
            {
              this->writeArraySequence<float>(iqdata, sizes);
            }
            else
            {
              this->writeArraySequence<double>(iqdata, sizes);
            }
          }

          this->nofSamplesWritten_ += sizes[0];

          // add data to I/Q tar preview
          if (this->enablePreview_)
          {
            this->tarPreview_.addArrayData(iqdata, sizes[0], this->dataFormat_);
          }
        }

        /**
          @brief Add the specified I/Q channel to the existing data record. Data is first written to 
          temporary files and added to the actual .mat file when close() is called. 
          The number of the I/Q data arrays passed to this method as well as the lengths of the arrays
          are validated w.r.t the specified channel information. In case of a mismatch between the channel
          information and passed data, an exception is thrown. Further, a preview is generated from the added data,
          which is saved as additional metadata.
          @tparam Precision of the value array - single or double.
          @param [in]  iqdata Vector containing I/Q data in interleaved format.
          @param [in]  sizes The length of the specified data arrays.
          @throws DaiException(FileWriterUninitialized) Thrown if this class was not initialized. Call open() first.
          @throws DaiException(InconsistentInputData) Thrown if channel information and I/Q data arrays sizes are not consistent.
          @throws DaiException(InternalError) Thrown if data could not be saved to file.
        */template<typename T>
        void appendChannel(const std::vector<T*>& iqdata, const std::vector<size_t>& sizes)
        {
          // real data
          if (this->dataFormat_ == IqDataFormat::Real)
          {
            this->appendArray(iqdata, sizes);
            return;
          }

          // complex and polar data
          if (false == this->initialized_)
          {
            throw DaiException(ErrorCodes::FileWriterUninitialized);
          }

          DataImportExportBase::initializeDataType<T>(this->dataType_, this->lockDataType_);

          if (false == this->validateInputDataChannel(iqdata.size(), sizes))
          {
            throw DaiException(ErrorCodes::InconsistentInputData);
          }

          // write to tmp file
          if (this->expectedNofIqBytes_ == 0)
          {
            if (this->dataType_ == IqDataType::Float32)
            {
              this->writeTemporaryChannelSequence<float>(iqdata, sizes);
            }
            else
            {
              this->writeTemporaryChannelSequence<double>(iqdata, sizes);
            }
          }
          else // write directly to tar file
          {
            if (this->dataType_ == IqDataType::Float32)
            {
              this->writeChannelSequence<float>(iqdata, sizes);
            }
            else
            {
              this->writeChannelSequence<double>(iqdata, sizes);
            }
          }

          size_t nofSamples = sizes[0] / 2;
          this->nofSamplesWritten_ += nofSamples;

          // add data to I/Q tar preview
          if (this->enablePreview_)
          {
            this->tarPreview_.addChannelData(iqdata, sizes[0], this->dataFormat_);
          }
        }

      private:
        /** @brief Private default constructor. */
        IqTarWriter();

        /** @brief Private copy constructor. */
        IqTarWriter(const IqTarWriter&);

        /** @brief Private assignment operator.*/
        IqTarWriter& operator=(const IqTarWriter&);

        /**
          @brief Creates an appropriate i/q data file name that can be used within the iq.tar file.
          @param [in]  tarFilename Name of the iq.tar file.
          @param [in]  nofChannels Number of i/q data channels.
          @param [in]  dataFormat The format of the i/q data.
          @param [in]  dataType The data type used to write i/q data.
          @param [in]  timestamp Timestamp used in generated filename.
          @returns Returns a filename that can be used within the iq.tar file.
        */static std::string generateIqDataFilename(const std::string& tarFilename, int nofChannels, IqDataFormat dataFormat, IqDataType dataType, time_t timestamp);

        /**
          @brief Build the XML content of the iq-tar file that describes the content of the file.
          @param [in]  iqDataFilename Name of the actual I/Q data file used within the iq.tar. Use
          generateIqDataFilename() to create an appropriate file name.
          @throws DaiException(InternalError) If the generated XML document was invalid or could
          not be formatted correctly.
          @returns Content of the iq-tar XML file as string.
        */std::string generateXml(const std::string& iqDataFilename);

        /**
          @brief Writes non-interleaved I/Q array data to temporary files.
          @tparam Template parameter of the I/Q data precision, i.e. float or double.
          @param [in]  iqdata Vector containing pointers to the I/Q data to be saved.
          @param [in]  sizes Vector containing the lengths of the iqdata arrays.
          @throws DaiException(InternalError) If an error occurred while accessing the file.
        */template<typename T, typename T2>
        void writeTemporaryArraySequence(const std::vector<T2*>& iqdata, const std::vector<size_t>& sizes)
        {
          size_t usedChannels = this->channelInfos_.size();
          size_t nofValues = sizes[0];
          size_t valuesPerSample = this->dataFormat_ == IqDataFormat::Real ? 1 : 2;
          size_t writeOffset = this->mmfWriter_.file_size();
          size_t writeSize = usedChannels * nofValues * valuesPerSample * sizeof(T); // all channels have equal length in iq-tar

          this->mmfWriter_.map(writeOffset, writeSize);
          Common::mmfDataAssert(this->mmfWriter_);

          if (this->dataFormat_ == IqDataFormat::Real) // write real samples
          {
            for (size_t channelIdx = 0; channelIdx < usedChannels; ++channelIdx)
            {
              auto writeBuf = reinterpret_cast<T*>(this->mmfWriter_.data());
              long i = -1;
              for (size_t valIdx = 0; valIdx < nofValues; ++valIdx)
              {
                for (size_t channelIdx = 0; channelIdx < usedChannels; ++channelIdx)
                {
                  writeBuf[++i] = static_cast<T>(iqdata[channelIdx][valIdx]);
                }
              }
            }
          }
          else // write complex and polar samples
          {
            auto writeBuf = reinterpret_cast<T*>(this->mmfWriter_.data());
            for (size_t valIdx = 0, i = 0; valIdx < nofValues; ++valIdx)
            {
              for (size_t arrayIdx = 0; arrayIdx < iqdata.size(); ++arrayIdx)
              {
                writeBuf[i++] = static_cast<T>(iqdata[arrayIdx][valIdx]);
              }
            }
          }

          this->mmfWriter_.flush();
          this->mmfWriter_.unmap();
        }

        /**
          @brief Writes non-interleaved I/Q array data to the tar archive.
          @tparam Template parameter of the I/Q data precision, i.e. float or double.
          @param [in]  iqdata Vector containing pointers to the I/Q data to be saved.
          @param [in]  sizes Vector containing the lengths of the iqdata arrays.
          @throws DaiException(DataOverflow) If the number of bytes exceeds the file size specified in expectedIqDataFileSize_.
          @throws DaiException(InternalError) If an error occurred while accessing the file.
        */template<typename T, typename T2>
        void writeArraySequence(const std::vector<T2*>& iqdata, const std::vector<size_t>& sizes)
        {
          size_t usedChannels = this->channelInfos_.size();
          size_t nofValues = sizes[0];
          size_t valuesPerSample = this->dataFormat_ == IqDataFormat::Real ? 1 : 2;
          size_t writeSize = usedChannels * nofValues * valuesPerSample * sizeof(T); // all channels have equal length in iq-tar

          if (this->writtenIqBytes_ + writeSize > this->expectedNofIqBytes_)
          {
            throw DaiException(ErrorCodes::DataOverflow);
          }

          if (this->dataFormat_ == IqDataFormat::Real) // write real samples
          {
            for (size_t channelIdx = 0; channelIdx < usedChannels; ++channelIdx)
            {
              long i = -1;
              for (size_t valIdx = 0; valIdx < nofValues; ++valIdx)
              {
                for (size_t channelIdx = 0; channelIdx < usedChannels; ++channelIdx)
                {
                  T val = static_cast<T>(iqdata[channelIdx][valIdx]);
                  // might be pretty inefficient to write to tar byte-wise
                  if (-1 == archive_write_data(this->archive_, &val, sizeof(val)))
                  {
                    throw DaiException(ErrorCodes::InternalError);
                  }
                }
              }
            }
          }
          else // write complex and polar samples
          {
            for (size_t valIdx = 0, i = 0; valIdx < nofValues; ++valIdx)
            {
              for (size_t arrayIdx = 0; arrayIdx < iqdata.size(); ++arrayIdx)
              {
                T val = static_cast<T>(iqdata[arrayIdx][valIdx]);
                // might be pretty inefficient to write to tar byte-wise
                if (-1 == archive_write_data(this->archive_, &val, sizeof(val)))
                {
                  throw DaiException(ErrorCodes::InternalError);
                }
              }
            }
          }

          this->writtenIqBytes_ += writeSize;
        }

        /**
          @brief Writes non-interleaved I/Q channel data to temporary files.
          @tparam Template parameter of the I/Q data precision, i.e. float or double.
          @param [in]  iqdata Vector containing pointers to the I/Q data to be saved.
          @param [in]  sizes The lengths of the iqdata array.
          @throws DaiException(InternalError) If an error occurred while accessing the file.
        */template<typename T, typename T2>
        void writeTemporaryChannelSequence(const std::vector<T2*> iqdata, const std::vector<size_t>& sizes)
        {
          size_t writeOffset = this->mmfWriter_.file_size();
          size_t nofChannels = iqdata.size();
          size_t nofValues = sizes[0];
          size_t writeSize = nofChannels * nofValues * sizeof(T);

          this->mmfWriter_.map(writeOffset, writeSize);
          Common::mmfDataAssert(this->mmfWriter_);

          auto writeBuf = reinterpret_cast<T*>(this->mmfWriter_.data());
          long i = -1;
          for (size_t valIdx = 0; valIdx < nofValues; valIdx += 2)
          {
            for (size_t channelIdx = 0; channelIdx < nofChannels; ++channelIdx)
            {
              writeBuf[++i] = static_cast<T>(iqdata[channelIdx][valIdx]);
              writeBuf[++i] = static_cast<T>(iqdata[channelIdx][valIdx +1]);
            }
          }

          this->mmfWriter_.flush();
          this->mmfWriter_.unmap();
        }

         /**
          @brief Writes non-interleaved I/Q channel data to the tar archive.
          @tparam Template parameter of the I/Q data precision, i.e. float or double.
          @param [in]  iqdata Vector containing pointers to the I/Q data to be saved.
          @param [in]  sizes The lengths of the iqdata array.
          @throws DaiException(DataOverflow) If the number of bytes exceeds the file size specified in expectedIqDataFileSize_.
          @throws DaiException(InternalError) If an error occurred while accessing the file.
        */template<typename T, typename T2>
        void writeChannelSequence(const std::vector<T2*> iqdata, const std::vector<size_t>& sizes)
        {
          size_t nofChannels = iqdata.size();
          size_t nofValues = sizes[0];
          size_t writeSize = nofChannels * nofValues * sizeof(T);

          if (this->writtenIqBytes_ + writeSize > this->expectedNofIqBytes_)
          {
            throw DaiException(ErrorCodes::DataOverflow);
          }

          this->mmfWriter_.map(this->mmfWriter_.file_size(), writeSize);

          long i = -1;
          for (size_t valIdx = 0; valIdx < nofValues; valIdx += 2)
          {
            for (size_t channelIdx = 0; channelIdx < nofChannels; ++channelIdx)
            {
              T val = static_cast<T>(iqdata[channelIdx][valIdx]);
              // might be pretty inefficient to write to tar byte-wise
              if (-1 == archive_write_data(this->archive_, &val, sizeof(val)))
              {
                throw DaiException(ErrorCodes::InternalError);
              }
              
              val = static_cast<T>(iqdata[channelIdx][valIdx +1]);
              // might be pretty inefficient to write to tar byte-wise
              if (-1 == archive_write_data(this->archive_, &val, sizeof(val)))
              {
                throw DaiException(ErrorCodes::InternalError);
              }
            }
          }

          this->writtenIqBytes_ += writeSize;
        }

        /**
          @brief Validates if the input data is consistent, e.g.:
          - Check if number of data arrays matches information given by channelInfoList.samples and format
          - Check if ClockRate and CenterFrequency of each channel are "equal"
          @returns TRUE if all input parameters are consistent. FALSE otherwise.
        */bool validateChannelInformation();

        /**
          @brief Validates if the input data is consistent, i.e. the lengths of the specified 
          data arrays math the data format specifications. Use this method when writing I/Q arrays,
          for channels use validateInputDataChannel().
          @param [in]  nofArrays Number of arrays to be written to file.
          @param [in]  sizeOfArrays The lengths of the individual arrays.
          @returns TRUE if all input parameters are consistent. FALSE otherwise.
        */bool validateInputDataArray(size_t nofArrays, const std::vector<size_t>& sizeOfArrays);

        /**
          @brief Validates if the input data is consistent, i.e. the lengths of the specified 
          data arrays math the data format specifications. Use this method when writing channels,
          for I/Q arrays use validateInputDataArray().
          @param [in]  nofArrays Number of arrays to be written to file.
          @param [in]  sizeOfArrays The lengths of the individual arrays.
          @returns TRUE if all input parameters are consistent. FALSE otherwise.
        */bool validateInputDataChannel(size_t nofArrays, const std::vector<size_t>& sizeOfArrays);

        /**
          @brief Validates if all arrays have the same length.
          @param [in]  sizeOfArrays The lengths of the individual data arrays.
          @returns TRUE if all arrays are of the same length.
        */bool validateEqualArrayLength(const std::vector<size_t>& sizeOfArrays);

        /**
          @brief Creates the final iq.tar file:
          - Copies i/q data from temp. file to tar
          - Creates and adds xml file to tar, containing meta data and i/q preview
          - Adds xslt file to tar
        */void finalizeTemporarySequence();

        /** 
          @brief Finalizes the i/q data file and adds xml/xslt metadata files to the
          archive.
        */void finalizeTarArchive();

        /** 
          @brief Adds the xml metadata file to the archive. If the preview functionality is enabled,
          the xslt scheme will also be added.
          @param [in]  a Tar archive to which the files shall be added. Archive must be opened and ready to used.
        */void addMetadataFilesToArchiv(struct archive* a);

        /**
          @brief Deletes all temporary files that might be created to 
          buffer I/Q data. Should be called after finalizeTemporarySequence().
        */void deleteTempFiles();

        /** @brief TRUE if IqTarWriter was initialized successfully and is 
        ready to take I/Q data.*/
        bool initialized_;

        /** @brief Path to the iq.tar file to be written. */
        const std::string filename_;

        /** @brief Path to the directory used to save temporary files. */
        std::string tempPath_;

        /** @brief Path to temporary binary I/Q data file. I/Q Data is added
        to this file until finalizeTemporarySequence() is called. */
        std::string tempFile_;

        /** @brief Memory mapped file writer used to write I/Q data to tempFile_. */
        memory_mapped_file::writable_mmf mmfWriter_;

        /** @brief Counts the number of samples written to file. */
        size_t nofSamplesWritten_;

        /** @brief Name of the application or instrument exporting its data. */
        std::string applicationName_;

        /** @brief Text that further describes the contents of the file. */
        std::string comment_;

        /** @brief Format of i/q data. */
        IqDataFormat dataFormat_;

        /** @brief Precision used to write data to file. */
        IqDataType dataType_;

        /** @brief If set TRUE, dataType_ cannot be changed. Value is set at first call
        of appendArray() or appendChannel(). */
        bool lockDataType_;

        /** @brief Number of data arrays. */
        size_t nofArrays_;

        /** @brief Timestamp saved with metadata. **/
        time_t timestamp_;

        /** @brief Information for each channel in the data file. **/
        std::vector<ChannelInfo> channelInfos_;

        /** @brief If supported by the file format, meta data can be provided as key-value pair. **/
        std::map<std::string, std::string> metadata_;

        /** @brief XMl-string that is added to the XML file at hierarchy level &lt;UserData&gt;&lt;RohdeSchwarz&gt;DEPRECATED_INFO_STRING,
        as required by FSW.
        */std::string deprecatedInfoXml_;

        /** @brief Expected i/q data file size in bytes. If &gt; 0, no temp file will be
        * generated, but the actual tar file is written directly. 
        */uint64_t expectedNofIqBytes_;

        /** @brief Actual number of i/q data bytes written. Only used if expectedNofIqBytes_ is set
        * and no temp file is written.
        */uint64_t writtenIqBytes_;

        /** @brief Handle to tar archive. Only used if expectedNofIqBytes_ is set
        * and no temp file is written.
        */struct archive* archive_;

        /** @brief Handle to the i/q data entry of the tar archive. Only used if expectedNofIqBytes_ is set
        * and no temp file is written.
        */struct archive_entry* archiveEntry_;

        /** @brief If set TRUE, the IqTarPreview will be calculated every time new I/Q data is added.
        * Otherwise no preview will be available in the XML meta data file .
        */bool enablePreview_;

        /** @brief Generates xml-preview data of I/Q data. */
        IqTarPreview tarPreview_;
      };
    }
  }
}
