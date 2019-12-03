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
* @file      iqmatlabpimpl.h
*
* @brief     This is the header file of class IqMatlab::Impl.
*
* @details   This class contains the private implementation corresponding to class IqMatlab.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include "common.h"

#include "iarrayselector.h"
#include "dataimportexportbase.h"
#include "iqmatlab.h"
#include "iqmatlab_reader.h"
#include "iqmatlab_writer.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /** 
      * @brief Private implementation corresponding to class IqMatlab.
      */
      class IqMatlab::Impl final : public DataImportExportBase, IAnalyzeContent, IArraySelector, ITempDir
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
          @brief Sets the matlab file version used to write the file.
          There is no need to set the matlab file version in order to read
          a file. In this case, the file version is detected automatically.
          @param [in]  version The matlab file version to be used. File 
          version 4 and 7.3 is supported.
          @returns Returns ErrorCodes::WriterAlreadyInitialized if the matlab writer 
          has already been initialized and the file version cannot be changed anymore.
          Otherwise ErrorCodes::Success is returned.
        */int setMatlabVersion(const MatlabVersion version);

        /**
          @returns Returns the matlab file version that is used to write a file.
        */MatlabVersion getMatlabVersion() const;

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

        int matchArrayDimensions(size_t minCols, size_t minRows, bool exactColMatch, std::vector<std::string>& arrayNames);
        int64_t getNofRows(const std::string& arrayName);
        int64_t getNofCols(const std::string& arrayName);
        int readRawArray(const std::string& arrayName, size_t column, size_t nofValues, std::vector<float>& values, size_t offset = 0);
        int readRawArray(const std::string& arrayName, size_t column, size_t nofValues, float* values, size_t offset = 0);
        int readRawArray(const std::string& arrayName, size_t column, size_t nofValues, std::vector<double>& values, size_t offset = 0);
        int readRawArray(const std::string& arrayName, size_t column, size_t nofValues, double* values, size_t offset = 0);

        int setTempDir(const std::string& path);
        std::string getTempDir() const;

      private:
        /** @brief Private default constructor. */
        Impl();

        /** @brief Private copy constructor. */
        Impl(const Impl&);

        /** @brief Private assignment operator.*/
        Impl& operator=(const Impl&);

        void updateChannels(const std::string& channelName, double clock, double frequency, size_t samples);
        void updateMetadata(const std::string& key, const std::string& value);
        void updateTimestamp(const time_t timestamp);

        /** @brief Matlab reader instance. */
        IqMatlabReader* reader_;

        /** @brief Matlab writer instance */
        IqMatlabWriter* writer_;

        /** @brief Path to the directory used to save temporary files. */
        std::string tempPath_;

        /** @brief Matlab version used to create .mat file. */
        MatlabVersion matVersion_;
      };
    }
  }
}