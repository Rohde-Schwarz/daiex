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
* @file      iqtar.h
*
* @brief     This is the header file of class IqTar.
*
* @details	 This class contains the implementation of the file format iq-tar.
*						 An iq-tar file contains I/Q data in binary format alongside with meta 
*						 information that describes the nature and the source of the data, e.g.
*					   sampling rate. The objective of the iq-tar file format is to separate I/Q
*						 data from the meta information while still combining both information in 
*						 one file. In addition, the file format provides a preview of the contained
*						 I/Q data as well as user-specific data that can be visualized using a web browser.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include "idataimportexport.h"
#include "itempdir.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
        @brief This class contains the implementation of the file format iq-tar.
        An iq-tar file contains I/Q data in binary format alongside with meta 
        information that describes the nature and the source of the data, e.g.
        sampling rate. The objective of the iq-tar file format is to separate I/Q
        data from the meta information while still combining both information in 
        one file. In addition, the file format provides a preview of the contained
        I/Q data as well as user-specific data that can be visualized using a web browser.

        An iq-tar file must contain the following files:
        - one single I/Q parameter XML file, i.e. 'File.xml', which contains meta information about the
        I/Q data (e.g. sampling rate, etc.).
        - one single I/Q data binary file, e.g. File.complex.float32, which contains binary I/Q data of 
        all channels. The I/Q data is saved in binary format according to the format and data type specified
        in the XML file (see Format element and DataType element). To allow reading and writing of streamed I/Q
        data, all data is interleaved, i.e. complex values are interleaved pairs of I and Q values and multi-channel 
        signals contain interleaved (complex) samples for channel_1, channel_2 channel_3 etc.
        
        An iq-tar file can optionally contain the following file:
        - I/Q preview XSLT file, e.g. open_IqTar_xml_file_in_web_browser.xslt, a stylesheet to display the 
        I/Q parameter XML file and a preview of the I/Q data in a web browser.
        @image html iq-tar.png
        - The preview can be disabled by setting IqTar::setPreviewEnabled(false). If disabled, neither the preview data
        will be added to the XML file, nor the xslt file will be added to the tar archive.

        To check the content of an iq-tar file on a Windows PC:
        - Use an archive tool (e.g. WinZip(R) or PowerArchiver(R)) to unpack the iq-tar file into a folder
        - Locate the folder using Windows Explorer
        - Open your web browser
        - Drag the I/Q parameter XML file, e.g. "File.xml", into your web browser
        @image html iq-tar_BrowserPreview.png 

        By default, temporary files are used to buffer the raw i/q data. The final iq.tar file is then 
        created during the close() operation. Therefore, this might be a long-running operation. The 
        directory used to store temporary files can be changed using setTempDir(). If the size of the 
        actual i/q data is known in advance, it is not necessary to buffer data to temp files. In this 
        case, set the expected data file size using disableTempFile().
      */

      class IqTarReader;
      class IqTarWriter;
      class MOSAIK_MODULE IqTar : public IDataImportExport, ITempDir
      {
      // access for libarchive lock
      // TODO remove lock with libarchive 4.0 -> setlocale should then not be used anymore
      friend class IqTarReader;
      friend class IqTarWriter;

      public:
        /**
          @brief Constructor. Initializes a new instance with the specified filename.
          Filename must be UTF-8 encoded. Be aware that the filename cannot be changed after construction.
          @param [in]  filename Name of the file to be read or to be written.
        */IqTar(const std::string& filename);

        /** @brief Destructor. Calls close()
        */~IqTar();

        time_t getTimestamp() const;
        void setTimestamp(const time_t timestamp);

        /** 
          @brief Enables or disables the calculation of the I/Q preview saved in the XML meta data
          file. Has to be set before writeOpen is called. The preview is enabled by default.
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
          @param [in]  metadata Non-standardized meta data; provided as key-value pairs. If the value-string represents a valid
          XML, the value is written unmodified to file. If the value-string does not represent a valid-string, special characters
          are escaped to match the XML convention.
          @returns If the file was successfully opened, ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */int writeOpen(
          IqDataFormat format,
          size_t nofArrays,
          const std::string& applicationName,
          const std::string& comment,
          const std::vector<ChannelInfo>& channelInfos,
          const std::map<std::string, std::string>* metadata = 0);

        /**
          @brief Opens a file in write-only mode.
          @post Call close() to finalize the file.
          @param [in]  format Specifies how I/Q values are saved, i.e. complex, real, or polar.
          @param [in]  nofArrays The number of data arrays to be written. E.g. one complex I/Q channel contains 2 arrays.
          @param [in]  applicationName Application or instrument name exporting this I/Q data.
          @param [in]  comment Text that further describes the file contents.
          @param [in]  channelInfo Channel information with name, clock rate and center frequency. One object is required per channel.
          @param [in]  metadata Non-standardized meta data; provided as key-value pairs. If the value-string represents a valid
          XML, the value is written unmodified to file. If the value-string does not represent a valid-string, special characters
          are escaped to match the XML convention.
          @param [in]  deprecatedInfoXml Information that can be added to the XML file at hierarchy level &lt;UserData&gt;&lt;RohdeSchwarz&gt;DEPRECATED_INFO_STRING,
          as required by FSW. Make sure to pass a valid XML string. 
          @returns If the file was successfully opened, ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */int writeOpen(
          IqDataFormat format,
          size_t nofArrays,
          const std::string& applicationName,
          const std::string& comment,
          const std::vector<ChannelInfo>& channelInfo,
          const std::map<std::string, std::string>& metadata,
          const std::string& deprecatedInfoXml);

        int close();

        int getMetadata(std::vector<ChannelInfo>& channelInfos, std::map<std::string, std::string>& metadata) const;

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
        IqTar();

        /** @brief Private copy constructor. */
        IqTar(const IqTar&);

        /** @brief Private assignment operator.*/
        IqTar& operator=(const IqTar&);

        /** @brief Private implementation */
        class Impl;

        /** @brief Private implementation */
        Impl* pimpl;
      };
    }
  }
}
