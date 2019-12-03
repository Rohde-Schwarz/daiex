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
* @file      iqmatlab.h
*
* @brief     This is the header file of class IqMatlab.
*
* @details   This class contains the implementation of the Matlab I/Q file format, which
*            saves raw I/Q data alongside with additional meta information in the MathWorks Matlab(R)
*            file format MAT. Files can be written in matlab file format 4.0 or 7.3 respectively. The 
*            class can read matlab files of format 4.0, 5.0, and 7.3.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include "idataimportexport.h"
#include "iarrayselector.h"
#include "itempdir.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief This class contains the implementation of the Matlab I/Q file format, which
      * saves raw I/Q data alongside with additional meta information in the MathWorks Matlab(R)
      * file format MAT. Files can be written in matlab file format 4.0 or 7.3 respectively. The 
      * class can read matlab files of format 4.0, 6.0, and 7.3.
      * The mat file must contain the following fields with correct dimensions and value types:
      * array name        | value type        | size (rows x columns)
      * ----------------- | ----------------- | ---------------------
      * Comment					  | char							| 1 x N
      * DataType				  | char							| 1 x N
      * Format					  | char							| 1 x N
      * DateTime				  | char							| 1 x N
      * NumberOfChannels  | double						| 1 x N
      * Channel information is stored in variables with leading "ChX_" as variable name, whereas X
      * represents the channel count, starting from 1.
      * array name        | value type        | size (rows x columns)
      * ----------------- | ----------------- | ---------------------
      * ChX_ChannelName	  | char							| 1 x N
      * ChX_CFrequency_Hz | double						| 1 x 1
      * ChX_Clock_Hz		  | double						| 1 x 1
      * ChX_Samples			  | double						| 1 x 1
      * ChX_Data          | double						| 2 x N for complex data, 1 x N for real data.
      * Optional user data can be stored in variables starting with prefix "UserDataX", where
      * X counts the number of written user data fields, starting from 1. The total number
      * of user data written is stored in array UserData_Count.
      * array name        | value type        | size (rows x columns)
      * ----------------- | ----------------- | -----------------
      * User_DataX		    | char							| 1 x N
      * User_DataCount	  | double						| 1 x 1
      * Several size limitations apply for the different matlab file formats with respect to
      * the maximum number of elements per array, the maximum size of an array in bytes and the 
      * total file size. For further information see
      * http://de.mathworks.com/help/matlab/import_export/mat-file-versions.html and 
      * https://de.mathworks.com/matlabcentral/answers/91711-what-is-the-maximum-matrix-size-for-each-platform
      */
      class MOSAIK_MODULE IqMatlab : public IDataImportExport, IArraySelector, ITempDir
      {
      public:
        /**
          @brief Constructor. Initializes a new instance with the specified filename.
          Filename must be UTF-8 encoded. Be aware that the filename cannot be changed after construction.
          @param [in]  filename Name of the file to be read or to be written.
        */IqMatlab(const std::string& filename);
        
        /** @brief Destructor. Calls close()
        */~IqMatlab();

        /**
          @brief Sets the matlab file version used to write the file.
          There is no need to set the matlab file version in order to read
          a file. In this case, the file version is detected automatically.
          @param [in]  version The matlab file version to be used. File 
          versions 4 and 7.3 are supported.
          @returns Returns ErrorCodes::WriterAlreadyInitialized if the matlab writer 
          has already been initialized and the file version cannot be changed anymore.
          Otherwise ErrorCodes::Success is returned.
        */int setMatlabVersion(const MatlabVersion version);

        /**
          @returns Returns the matlab file version that is used to write a file.
        */MatlabVersion getMatlabVersion() const;

        time_t getTimestamp() const;
        void setTimestamp(const time_t timestamp);

        int readOpen(std::vector<std::string>& arrayNames);
        int writeOpen(
          IqDataFormat format,
          size_t nofArrays,
          const std::string& applicationName,
          const std::string& comment,
          const std::vector<ChannelInfo>& channelInfos,
          const std::map<std::string, std::string>* metadata = 0);

         /**
          @brief Closes a file that has previously been opened. The method is also called by the destructor.
          @remarks This is be a long-running operation, since temporary files need to be merged to the final .mat file. 
          be aware that all i/q data written is loaded to RAM before written to the matlab file, since it is currently not
          possible to append data to a matlab variable.
          @returns If the final I/Q file has successfully been written, ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */int close();

        int getMetadata(std::vector<ChannelInfo>& channelInfos, std::map<std::string, std::string>& metadata) const;
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
        IqMatlab();

        /** @brief Private copy constructor. */
        IqMatlab(const IqMatlab&);

        /** @brief Private assignment operator.*/
        IqMatlab& operator=(const IqMatlab&);

        /** @brief Private implementation */
        class Impl;

        /** @brief Private implementation */
        Impl* pimpl;
      };
    }
  }
}