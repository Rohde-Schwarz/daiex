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
* @file      iqtar_reader.h
*
* @brief     This is the header file of class IqTarReader.
*
* @details   This class contains the implementation of IqTarReader.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include "common.h"

#include "pugixml.hpp"
#include "memory_mapped_file.hpp"
#include "archive.h"
#include "archive_entry.h"

#include "ianalyzecontentiqtar.h"
#include "daiexception.h"
#include "errorcodes.h"
#include "enums.h"
#include "settings.h"
#include "platform.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /** 
      * Struct combining memory mapped file reader with information about current 
      * read offset and buffer size used to read data.
      */struct mmfReadMemoryData
      {
        /**
          @brief Constructor. 
        */mmfReadMemoryData() : offset(0), copyBufSize(Settings::getBufferSize()){}

        /**
          @brief Closes the file handle.
        */void close()
        {
          this->mmf.close();
          this->offset = 0;
        }

        /** @brief Provides read-only access to file via memory mapping. */
        memory_mapped_file::read_only_mmf mmf;

        /** @brief Current position in file stream. */
        size_t offset;

        /** @brief Number of bytes copied at once, when calling archiveReadCallback(). */
        size_t copyBufSize;
      };

      /**
      * @brief This class contains functionality needed to tread the contents of the RS iq-tar 
      * file format.
      */
      class IqTarReader
      {
      public:
        /**
          @brief Constructor. Initializes a new instance with the specified filename.
          Be aware that the filename cannot be changed after construction.
          @param [in]  filename Name of the file to be read or to be written.
          @param [in]  updateContent Provides callback methods to process meta data found in file.
        */IqTarReader(const std::string& filename, IAnalyzeContentIqTar& updateContent);

        /* @copydoc IqTar::~IqTar() */
        ~IqTarReader();

        /**
          @brief Closes the file and releases the file handle.
        */inline void close()
        {
          this->initialized_ = false;
          if (this->archive_ != nullptr)
          {
            Common::archiveAssert(archive_read_close(this->archive_));
            Common::archiveAssert(archive_read_free(this->archive_));

            this->archive_ = nullptr;
          }
        }

        /**
          @brief Parses the file content and verifies that the file contains all information required
          to be a valid I/Q data file. I.e. all mandatory data and optional meta data is parsed and
          promoted to the callback methods implemented by "updateContent".
        */void analyzeContent();
        
        /**
          @brief Returns the I/Q data array names found in this file. 
          @pre analyzeContent() must be called to beforehand, to parse the file.
          @param [out]  arrayNames Names of I/Q data arrays.
        */void getArrayNames(std::vector<std::string>& arrayNames) const;

        /**
          @brief Checks if the specified I/Q data array exists in the file read.
          @pre analyzeContent() must be called to beforehand, to parse the file.
          @param [in]  arrayName Name of the data array which is assumed in the file.
          @returns Returns TRUE if the I/Q data array was found in the file, otherwise
          FALSE is returned.
        */bool containsArray(const std::string& arrayName) const;

        /** @returns the number of I/Q samples */
        size_t getNofSamples() const;
        
        /**
          @returns Returns TRUE if this class has been initialized, i.e. 
          analyseContent() was called and the file content has been parsed
          successfully. Otherwise FALSE is returned.
        */bool isInitialized() const;

        /**
          @brief Reads the values of the specified array (e.g. 'Data_I' or 'Data_Q') and returns them with single or
          double precision, as defined by the template parameter.
          @tparam Template type that specifies the precision of the destination array used to store all i/q values.
          @param [in]  arrayName The name of the array to read.
          @param [out]  values The values read. Data read from file will be converted to the precision defined 
          by the template parameter, independent of the original data type.
          @param [in]  nofValues Number of values to read.
          @param [in]  offset Defines the start position in the I/Q data record at which the read operation is started.
        */template<typename T>
        void readArray(const std::string& arrayName, T* values, size_t nofValues, size_t offset)
        {
          size_t readOffset = 0;
          size_t ignoreNofChannelValues = 0;
          this->readPrepare(arrayName, nofValues, offset, readOffset, ignoreNofChannelValues);

          bool readIValues = false;
          if (Common::strEndsWithIgnoreCase(arrayName, "_I") || this->dataFormat_ == IqDataFormat::Real)
          {
            readIValues = true;
          }

          if (this->dataType_ == IqDataType::Float32)
          {
            this->readData<float>(nofValues, readOffset, ignoreNofChannelValues, readIValues, values);
          }
          else if (this->dataType_ == IqDataType::Float64)
          {
            this->readData<double>(nofValues, readOffset, ignoreNofChannelValues, readIValues, values);
          }
          else
          {
            throw DaiException(ErrorCodes::WrongDataType);
          }
        }

        /**
          @brief Reads the values of the specified channel (e.g. 'Data') and returns them as single or double 
          precision vector, as defined by the template parameter.
          @tparam Template type that specifies the precision of the destination array used to store all i/q values.
          @param [in]  channelName The name of the channel to read.
          @param [out]  values The values read. Data read from file will be converted to the precision defined 
          by the template parameter, independent of the original data type.
          @param [in]  nofValues Number of values to read.
          @param [in]  offset Defines the number of I/Q pairs to be skipped before the read operation is started.
        */template<typename T>
        void readChannel(const std::string& channelName, T* values, size_t nofValues, size_t offset)
        {
          // get array name
          std::string arrayName;
          if (this->dataFormat_ == IqDataFormat::Real)
          {
            arrayName = channelName;
          }
          else
          {
            arrayName = channelName + "_I";
          }

          // calculate number of samples to read
          size_t samplesToRead = 0;
          if (this->dataFormat_ == IqDataFormat::Real)
          {
            samplesToRead = nofValues;
          }
          else
          {
            if (nofValues % 2 != 0)
            {
              throw DaiException(ErrorCodes::InvalidArraySize);
            }

            samplesToRead = nofValues / 2;
          }

          // calculate offsets
          size_t readOffset = 0;
          size_t ignoreNofChannelValues = 0;
          this->readPrepare(arrayName, samplesToRead, offset, readOffset, ignoreNofChannelValues);

          // read actual data
          if (this->dataFormat_ == IqDataFormat::Real)
          {
            if (this->dataType_ == IqDataType::Float32)
            {
              this->readData<float>(nofValues, readOffset, ignoreNofChannelValues, true, values);
            }
            else if (this->dataType_ == IqDataType::Float64)
            {
              this->readData<double>(nofValues, readOffset, ignoreNofChannelValues, true, values);
            }
            else
            {
              throw DaiException(ErrorCodes::WrongDataType);
            }
          }
          else
          {
            if (this->dataType_ == IqDataType::Float32)
            {
              this->readDataInterleaved<float>(samplesToRead, readOffset, ignoreNofChannelValues, values);
            }
            else if (this->dataType_ == IqDataType::Float64)
            {
              this->readDataInterleaved<double>(samplesToRead, readOffset, ignoreNofChannelValues, values);
            }
            else
            {
              throw DaiException(ErrorCodes::WrongDataType);
            }
          }
        }

      private:
        /** @brief Private default constructor. */
        IqTarReader();

        /** @brief Private copy constructor. */
        IqTarReader(const IqTarReader&);

        /** @brief Private assignment operator.*/
        IqTarReader& operator=(const IqTarReader&);

        /**
          @brief Searches the specified tar archive for a tar element.
          @param [in]  archive The opened tar archive.
          @param [in]  tarElementName The required tar element.
          @returns Returns a pointer to found tar archive element.
          @throws InvalidTarArchive if the element was not found in the tar archive.
        */static archive_entry* findTarElement(archive* archive, const std::string& tarElementName);
        
        /**
          @brief Read callback used by tar archive reader when reading data from file. Uses
          a memory mapped file for fast data access.
          @param [in]  a The tar archive to be accessed.
          @param [in]  clientData User data to be passed to reader callback, i.e. mmfReadMemoryData to handle
          file access.
          @param [in]  buffer Destination buffer for read data.
          @returns Returns the number of bytes read.
        */static __LA_SSIZE_T	archiveReadCallback(struct archive* a, void* clientData, const void** buffer);
        
        /**
          @brief Close file callback used by tar archive. Closes the memory mapped file handle.
          @param [in]  a The tar archive to be closed.
          @param [in]  clientData User data to be passed to reader callback, i.e. mmfReadMemoryData to handle
          file access.
          @returns Returns ARCHIVE_OK if file was closed successfully, otherwise ARCHIVE_FATAL is returned.
        */static int archiveCloseCallback(struct archive* a, void* clientData);
      
        /**
          @brief Callback to speed up seek operations in tar archive. Underlying data access uses
          memory mapped file.
          @param [in]  a The tar archive to be closed.
          @param [in]  clientData User data to be passed to reader callback, i.e. mmfReadMemoryData to handle
          file access.
          @param [in]  request Number of bytes to be seek forward in tar stream.
          @returns Returns the number of skipped bytes.
        */static __LA_INT64_T	archiveReadSkipCallback(struct archive* a, void* clientData, __LA_INT64_T request);

        /**
          @brief Reads I/Q data from file according to the data format of the IqTar file. A scaling factor
          is applied to the data, if saved with the meta data. The precision of the I/Q data will be converted
          to T2, independent of the formated used in the file.
          @tparam T Template parameter of the source I/Q data precision, as saved to file, i.e. float or double.
          @tparam T2 Template parameter of the destination I/Q data precision, as returned by this method., i.e. float or double.
          @param [in]  nofValues Number of values to read.
          @param [in]  readOffset Read offset in bytes, w.r.t to start position of the data file in the tar stream.
          @param [in]  ignoreNofChannelValues Number of values to be ignored, e.g. if data is saved interleaved as I1I2I3Q1Q2Q3
          and the first channel is read, values I2 and I3 need to be skipped, thus ignoreNofChannelValues = 2.
          @param [in]  readIValues If TRUE, I-values are read, otherwise Q-values are read from file.
          @param [in]  values The destination buffer for the I/Q data.
          @throws DaiException(InternalError) if an error occurred while accessing the file.
        */template<typename T, typename T2>
        void readData(size_t nofValues, size_t readOffset, size_t ignoreNofChannelValues, bool readIValues, T2* values)
        {
          try
          {
            memory_mapped_file::read_only_mmf mmf;
            Platform::mmfOpen(mmf, this->filename_, false);

            if (this->dataFormat_ == IqDataFormat::Real)
            {
              // calculate number of bytes to read
              size_t readSize = (nofValues + nofValues * ignoreNofChannelValues) * sizeof(T);

              // align memory
              mmf.map(readOffset, readSize);
              Common::mmfDataAssert(mmf);

              // copy data from file to values vector: strideCopy copies one value and then skips 
              // n-values (ignoreNofChannelValues) before reading the next value again
              Common::strideCopy(reinterpret_cast<const T*>(mmf.data()), values, nofValues, ignoreNofChannelValues);
            }
            else // IqDataFormat::Complex || IqDataFormat::Polar
            {
              // calculate number of bytes to read
              size_t readSize = (2 * nofValues + nofValues * ignoreNofChannelValues) * sizeof(T);

              // align memory
              mmf.map(readOffset, readSize);
              Common::mmfDataAssert(mmf);

              // calculate the position of the requested value in the data stream
              // calculate the number of values to be skipped ahead of reading the desired value and
              // after reading the desired value
              int preSkip = 0;
              int postSkip = 0;
              if (readIValues)
              {
                postSkip = 1 + ignoreNofChannelValues;
              }
              else
              {
                preSkip = 1;
                postSkip = ignoreNofChannelValues;
              }

              // copy data from file to values vector: strideCopy copies one value and then skips 
              // n-values (ignoreNofChannelValues) before reading the next value again
              // skip preSkip values at the beginning and then skip preSkip + postSkip values before reading the next value.
              Common::strideCopy(reinterpret_cast<const T*>(mmf.data()) + preSkip, values, nofValues, preSkip + postSkip);
            }

            mmf.close();

            // apply scaling if required
            if (false == std::isnan(this->scalingFactor_))
            {
              std::transform(values, values + nofValues, values, std::bind1st(std::multiplies<T>(), this->scalingFactor_));
            }
          }
          catch (const std::exception &e)
          {
            throw DaiException(ErrorCodes::InternalError, e.what());
          }
          catch (...)
          {
            throw DaiException(ErrorCodes::InternalError, "unknown");
          }
        }

        /**
          @brief Reads I/Q data from file in an interleaved manner, hence data must be available in file as I1Q1I2Q2, etc.
          The precision of the I/Q data will be converted to T2, independent of the formated used in the file.
          @tparam T Template parameter of the source I/Q data precision, as saved to file, i.e. float or double.
          @tparam T2 Template parameter of the destination I/Q data precision, as returned by this method., i.e. float or double.
          @param [in]  samplesToRead Number of samples to read.
          @param [in]  readOffset Read offset in bytes, w.r.t to start position of the data file in the tar stream.
          @param [in]  ignoreNofChannelValues Number of values to be ignored.
          @param [in]  values The destination buffer for the I/Q data.
          @throws DaiException(InternalError) if an error occurred while accessing the file.
        */template<typename T, typename T2>
        void readDataInterleaved(size_t samplesToRead, size_t readOffset, size_t ignoreNofChannelValues, T2* values)
        {
          try
          {
            // calculate number of bytes to read. Double number of values to read as samplesToRead is number of I/Q pairs
            size_t nofValues = 2 * samplesToRead;
            size_t readSize = (nofValues + samplesToRead * ignoreNofChannelValues) * sizeof(T);

            memory_mapped_file::read_only_mmf mmf;
            Platform::mmfOpen(mmf, this->filename_, false);

            // align memory
            mmf.map(readOffset, readSize);
            Common::mmfDataAssert(mmf);

            // copy 1 I/Q pair (2 values) and skip ignoreNofChannelValues values before reading the next pair
            Common::strideCopyIqPairs(reinterpret_cast<const T*>(mmf.data()), values, nofValues, ignoreNofChannelValues);

            mmf.close();

            // apply scaling if required
            if (0 == std::isnan(this->scalingFactor_))
            {
              std::transform(values, values + nofValues, values, std::bind1st(std::multiplies<T>(), this->scalingFactor_));
            }
          }
          catch (const std::exception &e)
          {
            throw DaiException(ErrorCodes::InternalError, e.what());
          }
          catch (...)
          {
            throw DaiException(ErrorCodes::InternalError, "unknown");
          }
        }

        /**
          @brief Opens the file for reading. If already opened, the file is closed and re-opened again.
        */void open();

        /** 
          @returns Returns the number of values per samples, based on
          the data format, i.e. 1 for IqDataFormat::Real.
        */size_t getValuesPerSample() const;

        /**
          @returns Returns the number of channels found in this file.
        */size_t getNofChannels() const;

        /** 
          prepare read
        */void readPrepare(const std::string& arrayName, size_t nofReadValues, size_t offset, size_t& readOffset, size_t& ignoreNofChannelValues);

        /**
          @brief Extracts the names of all tar elements found in this file.
          @param [out]  tarElementNames Vector containing all tar element names found.
        */void readArchiveContent(std::vector<std::string>& tarElementNames);
        
        /**
          @brief Reads the data of the specified tar element.
          @param [in]  tarElementName Name of the tar element to read.
          @param [out]  data Byte data of tar element as vector of chars.
        */void readElementContent(const std::string& tarElementName, std::vector<char>& data);

        /**
          @brief Iterates all specified tar elements and parses the first element that
          represents a xml file. The xml file content is further processed in parseXmlMetadata().
          @param [in]  tarElementNames Tar element names found in iq.tar file.
        */void loadXmlContentFromIqTar(const std::vector<std::string>& tarElementNames);

        /**
          @brief Interprets the specified vector of bytes as XML data and extracts
          mandatory and optional user data, such as application name, comment, iq data format,
          and channel information.
          @param [in]  data Raw XML data.
        */void parseXmlMetadata(const std::vector<char>& data);

        /**
          @brief Parses the specified xml document and extracts the channel information.
          @param [in]  doc Xml tree containing channel information.
        */void createChannelInformation(pugi::xml_document& doc);

        /**
          @brief Reads the optional center frequency tag from xml.
          @param [in]  doc Xml tree containing the Constants::XmlUserData tree including
          center frequency tag. NaN will be returned, if no center frequency was found.
          @returns The found center frequency of NaN.
        */double readCenterFrequency(pugi::xml_document& doc);

        /**
          @brief Parses the specified Xml and extracts optional user-defined metadata.
          @param [in]  doc Xml tree with Constants::XmlUserData root.
        */void readOptionalMetadata(pugi::xml_document& doc);

        /**
          @brief Parses the specified Xml and extracts optional 'deprecated information', as 
          required by 1ES1. Deprecated information can be an arbitrary xml-tree, located at 
          &lt;RS_IQ_TAR_FileFormat&gt;&lt;userdata&gt;&lt;rohdeschwarz&gt; level.
          @param [in]  doc Xml tree with Constants::XmlUserData root.
        */void readDeprecatedData(pugi::xml_document& doc);

        /** @brief Callback functions that process meta data found in file. 
          Called by analyzeContent(). 
        */IAnalyzeContentIqTar* updateContent_;

        /** @brief Set true, if analyseContent() was called and the file was parsed successfully. */
        bool initialized_;

        /** @brief Filename of the .iq.tar file */
        const std::string filename_;

        /** @brief Handle to the tar archive */
        struct archive* archive_;

        /** @brief Memory mapped reader used to read tar file */
        struct mmfReadMemoryData mmfReader_;

        /** @brief I/Q data type of data to be written. */
        IqDataType dataType_;

        /** @brief Data format of this file. */
        IqDataFormat dataFormat_;

        /** @brief Scaling factor that is to be applied to I/Q data after data
          has been read from file. 
        */double scalingFactor_;

        /** @brief Number of I/Q samples contained in file. */
        size_t nofSamples_;

        /** @brief Filename of binary data file contained in iq.tar file. */
        std::string iqDataFilename_;

        /** @brief Mapping between an array name and the corresponding channel. */
        std::map<std::string, size_t> arrayNameToChannelNo_;
      };
    }
  }
}
