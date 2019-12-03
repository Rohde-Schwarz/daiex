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
* @file      iqw.h
*
* @brief     This is the header file of class Iqw.
*
* @details   Class to read or write I/Q data from or to an IQW file.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma  once

#include "idataimportexport.h"
#include "itempdir.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief Class to read or write I/Q data from or to an IQW file.
      * IQW is a binary file format containing one channel of complex I/Q data. 
      * Format description details:
      * - IQDataFormat.Complex
      * - IQDataType.Float32
      * - Intel byte order
      * - The data order is either IIIQQQ ( default ) or IQIQIQ.
      */
      class MOSAIK_MODULE Iqw : public IDataImportExport, ITempDir
      {
      public:
        
        /**
          @brief Constructor. Initializes a new instance with the specified filename.
          Filename must be UTF-8 encoded. Be aware that the filename cannot be changed after construction.
          @param [in]  filename Name of the file to be read or to be written.
        */Iqw(const std::string& filename);

        /** @brief Destructor. Calls close()
        */~Iqw();

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

        /**
          @brief Since this file format does not support meta data, no timestamp will be set.
          @returns Returns the set timestamp; Initially returns time_t(0).
        */time_t getTimestamp() const;

        /**
          @brief Since this file format does not support meta data, setting a timestamp
          will have no effect.
          @param [in]  timestamp Value will be ignored.
        */void setTimestamp(const time_t timestamp);

        int getMetadata(std::vector<ChannelInfo>& channelInfos, std::map<std::string, std::string>& metadata) const;

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
        Iqw();

        /** @brief Private copy constructor. */
        Iqw(const Iqw&);

        /** @brief Private assignment operator.*/
        Iqw& operator=(const Iqw&);

        /** @brief Private implementation */
        class Impl;

        /** @brief Private implementation */
        Impl* pimpl;
      };
    }
  }
}