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
* @file      iqtarpimpl.h
*
* @brief     This is the header file of class IqTar::Impl.
*
* @details   This class contains the implementation of IqTar.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include "ianalyzecontent.h"
#include "iqtar.h"
#include "dataimportexportbase.h"
#include "iqtar_reader.h"
#include "iqtar_writer.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
       * @brief Private implementation of class IqTar.
      */
      class IqTar::Impl final : public DataImportExportBase, IAnalyzeContentIqTar, ITempDir
      {
      public:
        /**
          @brief Constructor. Initializes a new instance with the specified filename.
          Filename must be UTF-8 encoded. Be aware that the filename cannot be changed after construction.
          @param [in]  filename Name of the file to be read or to be written.
        */Impl(const std::string& filename);
        
        /** @brief Destructor. Calls close()
        */~Impl();

        /** 
          @brief Enables or disables the calculation of the I/Q preview saved in the XML meta data
          file. Has to be set before writeOpen is called! The default value is TRUE.
          @param [in]  enable TRUE if a preview of the I/Q data shall be saved to the XML
          meta data file, otherwise FALSE.
          @returns ErrorCodes::Success if the preview was successfully enabled, otherwise 
          ErrorCodes::WriterAlreadyInitialized is returned.
        */int setPreviewEnabled(bool enable);
        
        /** 
          @returns Returns TRUE if an I/Q preview is calculated and saved to XML, otherwise
          FALSE is returned.
        */bool getPreviewEnabled() const;

        int readOpen(std::vector<std::string>& arrayNames);

        /**
          @brief Opens a file in write-only mode.
          @post Call close() to finalize the file.
          @param [in]  format Specifies how I/Q values are saved, i.e. complex, real, or polar.
          @param [in]  nofArrays The number of data arrays to be written. E.g. one complex I/Q channel contains 2 arrays.
          @param [in]  applicationName Application or instrument name exporting this I/Q data.
          @param [in]  comment Text that further describes the file contents.
          @param [in]  channelInfos Channel information with name, clock rate and center frequency. One object is required per channel.
          @param [in]  metadata Non-standardized meta data; provided as key-value pairs.
          @param [in]  deprecatedInfoXml Information that can be added to the XML file at hierarchy level &lt;UserData&gt;&lt;RohdeSchwarz&gt;DEPRECATED_INFO_STRING,
          as required by FSW. Make sure to pass a valid XML string. 
          @returns If the file was successfully opened, ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */int writeOpen(
          IqDataFormat format,
          size_t nofArrays,
          const std::string& applicationName,
          const std::string& comment,
          const std::vector<ChannelInfo>& channelInfos,
          const std::map<std::string, std::string>* metadata = 0,
          const std::string* deprecatedInfoXml = 0);
      
        int close();

        /**
          @brief Returns deprecated information from xml-file at hierarchy level &lt;UserData&gt;&lt;RohdeSchwarz&gt;DEPRECATED_INFO_STRING.
          Required by FSW.
          @param [out]  xmlString Information as xml string, as saved in xml-file of iq.tar. If no
          deprecated information was found in the xml-file, an empty string is returned.
          @returns Returns ErrorCodes::OpenFileHasNotBeenCalled if file has not been opened so far
          and xml could not be parsed. Otherwise, ErrorCodes::Success is returned.
        */int getDeprecatedInfo(std::string& xmlString) const;

        int64_t getArraySize(const std::string& arrayName) const;

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

        /** @brief If the number of i/q values to be written is well-known in advance, buffering data
        * to temporary files can be disabled. Internally, the expected file size will be calculated. If that
        * this is exceeded, an exception will be raised.
        * @param [in]  nofIqValues Number of i/q values per channel. Iq.tar file format requires 
        * all channels to have the same length.
        * @param [in]  nofChannels Number of channels to be written.
        * @param [in]  format  I/q file format, i.e. complex or real.
        * @param [in]  dataType Type of the i/q values, i.e. float32 or float64.
        * @returns If the expected file size has been calculated ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */int disableTempFile(const uint64_t nofIqValues, const size_t nofChannels, const IqDataFormat format, const IqDataType dataType);

      private:
        /** @brief Private default constructor. */
        Impl();

        /** @brief Private copy constructor. */
        Impl(const Impl&);

        /** @brief Private assignment operator.*/
        Impl& operator=(const Impl&);

        // IAnalyzeContentIqTar
        void updateChannels(const std::string& channelName, double clock, double frequency, size_t samples);
        void updateMetadata(const std::string& key, const std::string& value);
        void updateTimestamp(const time_t timestamp);
        void updateDeprecatedInfo(const std::string& xmlString);

        /** @brief I/Q tar reader */
        IqTarReader* reader_;
        
        /** @brief I/Q tar writer */
        IqTarWriter* writer_;

        /** @brief I/Q data type. Used when writing iq.tar files without temporary files. 
        To generate the filename if the data file used in the tar archive, the dataType must be known.
        If iq.tar is written using temporary file buffering, the type of the data will be determined the
        first time appendArrays or appendChannels is called.
        */IqDataType dataType_;

        /** @brief Path to the directory used to save temporary files. */
        std::string tempPath_;

        /** @brief Expected i/q data file size in bytes. If &gt; 0, no temp file will be
        * generated, but the actual tar file is written directly. 
        */uint64_t expectedIqDataFileSize_;

        /** @brief String that is added to the XML file at hierarchy level &lt;UserData&gt;&lt;RohdeSchwarz&gt;DEPRECATED_INFO_STRING,
        as required by FSW.
        */std::string deprecatedInfo_;

        /** 
          @brief If set TRUE, the IqTarPreview will be calculated every time new I/Q data is added.
          Otherwise no preview will be available in the XML meta data file .
        */bool enablePreview_;
      };
    }
  }
}
