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
* @file      idataimportexport.h
*
* @brief     This is the header file of interface IDataImportExport.
*
* @details   Interface class for basic read and write operations of I/Q data.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include <stdint.h>
#include <string>
#include <ctime>
#include <vector>
#include <map>

#include "exportdecl.h"
#include "enums.h"
#include "channelinfo.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief Interface class for basic read and write operations of I/Q data.
      */
      class MOSAIK_MODULE IDataImportExport
      {
      public:
        /**
          @brief Destructor. Calls close().
        */virtual ~IDataImportExport() {};

        /**
          @brief Opens a file in read-only mode and reads all meta data contained as well as the names
          of the I/Q value arrays found in the file. The meta data can be queried using \ref getMetadata(). No I/Q data
          will be read during this operation. Use \ref readArray() or \ref readChannel() to read the data itself.
          @post Call \ref close() when you're done.
          @param [out]  arrayNames String identifiers of the data arrays found in this file. Use the identifiers
          to read the corresponding I/Q data.
          @returns If the file was successfully opened, ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int readOpen(std::vector<std::string>& arrayNames) = 0;

        /**
          @brief Opens a file in write-only mode. If supported by the file format, the specified meta data will also be stored in the file.
          If a file with the specified filename does already exists, the file is deleted. In case the file cannot be deleted, a FileOpenError
          will be returned.
          @post Call \ref close() to finalize the file.
          @param [in]  format Specifies how I/Q values are saved, i.e. complex, real, or polar.
          @param [in]  nofArrays The number of data arrays to be written. E.g. one complex I/Q channel contains 2 arrays.
          @param [in]  applicationName Application or instrument name exporting this I/Q data.
          @param [in]  comment Text that further describes the file contents.
          @param [in]  channelInfos Channel information with name, clock rate and center frequency. One object is required per channel.
          @param [in]  metadata Additional, non-standardized meta data; provided as key-value pairs.
          @returns If the file was successfully opened, ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int writeOpen(
          IqDataFormat format,
          size_t nofArrays,
          const std::string& applicationName,
          const std::string& comment,
          const std::vector<ChannelInfo>& channelInfos,
          const std::map<std::string, std::string>* metadata = 0) = 0;

        /**
          @brief Closes a file that has previously been opened. The method is also called by the destructor.
          @remarks Dependent on the file format, this might be a long-running operation, since
          temporary files need to be merged to the final I/Q data file. 
          @returns If the final I/Q file has successfully been written, ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int close() = 0;

        /**
          @brief Returns the corresponding meta data of the I/Q record.
          @pre Make sure to call \ref readOpen() to initialize an I/Q record before reading its meta data.
          @param [out]  channelInfos Channel info, i.e. name, clock rate and center frequency of each channel
          contained in the file.
          @param [out]  metadata Additional meta data, represented as key-value-pairs. The vector contains standardized information as well
          as optional user-defined information.
          @returns Returns ErrorCodes.Success (=0) if all data could be accessed.
        */virtual int getMetadata(std::vector<ChannelInfo>& channelInfos, std::map<std::string, std::string>& metadata) const = 0;

        /**
          @brief Returns the length (i.e. the number of samples) of the specified data array. Use the length of the array
          to preallocate the required space of the data vector used for reading I/Q data with the methods \ref readArray()
          and \ref readChannel().
          @param [in]  arrayName Name of the data array contained in the file.
          @returns Returns the length of the queried array, or -1 if the specified array name is invalid.
        */virtual int64_t getArraySize(const std::string& arrayName) const = 0;

        /**
          @brief Returns the timestamp stored with the meta data of an I/Q record.
          @pre Make sure to call \ref readOpen() to open the I/Q file before reading.
          @returns If the file format does not support meta data or not timestamp was found, time_t(0) is returned.
        */virtual time_t getTimestamp() const = 0;

        /**
          @brief Changes the timestamp of the stored meta data. Must be set before \ref writeOpen() is called.
          @remarks If the file format does not support meta data or a timestamp, the value is ignored.
          @param [in]  timestamp The new timestamp.
        */virtual void setTimestamp(const time_t timestamp) = 0;

        /**
          @brief Reads the values of the specified array (e.g. 'Data_I' or 'Data_Q') and returns them as single precision vector.
          @pre Make sure to call \ref readOpen() to open the I/Q file before reading.
          @remarks Reserve the size of the values-vector with the number of values to read in order
          to avoid memory reallocation within the method.
          @param [in]  arrayName The name of the array to read.
          @param [out]  values The values read. Data read from file will be converted to single precision,
          independent of the original data type.
          @param [in]  nofValues Number of values to read.
          @param [in]  offset  Defines the number of values (real data) or I/Q pairs (for complex/polar data) respectively
          to be ignored at the beginning the data arrays. To read the complete array, set offset value 0. Otherwise this parameter specifies the
          position in the array where the read operation starts.
          @returns If data was read successfully ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int readArray(const std::string& arrayName, std::vector<float>& values, size_t nofValues, size_t offset = 0) = 0;

        /**
          @brief Reads the values of the specified array (e.g. 'Data_I' or 'Data_Q') and returns them as single precision array.
          @pre Make sure to call \ref readOpen() to open the I/Q file before reading.
          @remarks Reserve the size of the values-array with the number of values to read.
          @param [in]  arrayName The name of the array to read.
          @param [out]  values The values read. Data read from file will be converted to single precision,
          independent of the original data type.
          @param [in]  nofValues Number of values to read. Must match the size of the value array.
          @param [in]  offset  Defines the number of values (real data) or I/Q pairs (for complex/polar data) respectively
          to be ignored at the beginning the data arrays. To read the complete array, set offset value 0. Otherwise this parameter specifies the
          position in the array where the read operation starts.
          @returns If data was read successfully ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int readArray(const std::string& arrayName, float* values, size_t nofValues, size_t offset = 0) = 0;

        /**
          @brief Reads the values of the specified array (e.g. 'Data_I' or 'Data_Q') and returns them as double precision vector.
          @pre Make sure to call \ref readOpen() to open the I/Q file before reading.
          @remarks Reserve the size of the values-vector with the number of values to read in order
          to avoid memory reallocation within the method.
          @param [in]  arrayName The name of the array to read.
          @param [out]  values The values read. Data read from file will be converted to double precision,
          independent of the original data type.
          @param [in]  nofValues Number of values to read.
          @param [in]  offset  Defines the number of values (real data) or I/Q pairs (for complex/polar data) respectively
          to be ignored at the beginning the data arrays. To read the complete array, set offset value 0. Otherwise this parameter specifies the
          position in the array where the read operation starts.
          @returns If data was read successfully ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int readArray(const std::string& arrayName, std::vector<double>& values, size_t nofValues, size_t offset = 0) = 0;

        /**
          @brief Reads the values of the specified array (e.g. 'Data_I' or 'Data_Q') and returns them as single precision array.
          @pre Make sure to call \ref readOpen() to open the I/Q file before reading.
          @remarks Reserve the size of the values-array with the number of values to read.
          @param [in]  arrayName The name of the array to read.
          @param [out]  values The values read. Data read from file will be converted to double precision,
          independent of the original data type.
          @param [in]  nofValues Number of values to read. Must match the size of the value array.
          @param [in]  offset  Defines the number of values (real data) or I/Q pairs (for complex/polar data) respectively
          to be ignored at the beginning the data arrays. To read the complete array, set offset value 0. Otherwise this parameter specifies the
          position in the array where the read operation starts.
          @returns If data was read successfully ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int readArray(const std::string& arrayName, double* values, size_t nofValues, size_t offset = 0) = 0;

        /**
          @brief Reads the values of the specified channel (e.g. 'Data') and returns them as single precision vector.
          @pre Make sure to call \ref readOpen() to open the I/Q file before reading.
          @remarks Reserve the size of the values-vector with the number of values to read in order
          to avoid memory reallocation within the method.
          @param [in]  channelName The name of the channel to read.
          @param [out]  values The values read. Data read from file will be converted to single precision,
          independent of the original data type.
          @param [in]  nofValues Number of values to read.
          @param [in]  offset  Defines the number of values (real data) or I/Q pairs (for complex/polar data) respectively
          to be ignored at the beginning the data arrays. To read the complete array, set offset value 0. Otherwise this parameter specifies the
          position in the array where the read operation starts.
          @returns If data was read successfully ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int readChannel(const std::string& channelName, std::vector<float>& values, size_t nofValues, size_t offset = 0) = 0;

        /**
          @brief Reads the values of the specified channel (e.g. 'Data') and returns them as single precision array.
          @pre Make sure to call \ref readOpen() to open the I/Q file before reading.
          @remarks Reserve the size of the values-array with the number of values to read.
          @param [in]  channelName The name of the channel to read.
          @param [out]  values The values read. Data read from file will be converted to single precision,
          independent of the original data type.
          @param [in]  nofValues Number of values to read. Must match the size of the value array.
          @param [in]  offset  Defines the number of values (real data) or I/Q pairs (for complex/polar data) respectively
          to be ignored at the beginning the data arrays. To read the complete array, set offset value 0. Otherwise this parameter specifies the
          position in the array where the read operation starts.
          @returns If data was read successfully ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int readChannel(const std::string& channelName, float* values, size_t nofValues, size_t offset = 0) = 0;

        /**
          @brief Reads the values of the specified channel (e.g. 'Data') and returns them as double precision vector.
          @pre Make sure to call \ref readOpen() to open the I/Q file before reading.
          @remarks Reserve the size of the values-vector with the number of values to read in order
          to avoid memory reallocation within the method.
          @param [in]  channelName The name of the channel to read.
          @param [out]  values The values read. Data read from file will be converted to double precision,
          independent of the original data type.
          @param [in]  nofValues Number of values to read.
          @param [in]  offset  Defines the number of values (real data) or I/Q pairs (for complex/polar data) respectively
          to be ignored at the beginning the data arrays. To read the complete array, set offset value 0. Otherwise this parameter specifies the
          position in the array where the read operation starts.
          @returns If data was read successfully ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int readChannel(const std::string& channelName, std::vector<double>& values, size_t nofValues, size_t offset = 0) = 0;

        /**
          @brief Reads the values of the specified channel (e.g. 'Data') and returns them as double precision array.
          @pre Make sure to call \ref readOpen() to open the I/Q file before reading.
          @remarks Reserve the size of the values-array with the number of values to read.
          @param [in]  channelName The name of the channel to read.
          @param [out]  values The values read. Data read from file will be converted to single precision,
          independent of the original data type.
          @param [in]  nofValues Number of values to read. Must match the size of the value array.
          @param [in]  offset  Defines the number of values (real data) or I/Q pairs (for complex/polar data) respectively
          to be ignored at the beginning the data arrays. To read the complete array, set offset value 0. Otherwise this parameter specifies the
          position in the array where the read operation starts.
          @returns If data was read successfully ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int readChannel(const std::string& channelName, double* values, size_t nofValues, size_t offset = 0) = 0;

        /**
          @brief Adds new I/Q data with single precision to the file.
          @pre Make sure to call \ref writeOpen() to enter write-mode.
          @param [in]  iqdata I and Q data vectors in single precision that have to be stored.
          The vectors have to be ordered as iqdata&lt;Channel1_I, Channel1_Q, Channel2_I, Channel2_Q, ...&gt;
          for complex data. The number of I/Q data vectors must match the specified value nofArrays in \ref writeOpen().
          @returns If data was successfully added, ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int appendArrays(const std::vector<std::vector<float>>& iqdata) = 0;

        /**
          @brief Adds new I/Q data with single precision to the file.
          @pre Make sure to call \ref writeOpen() to enter write-mode.
          @param [in]  iqdata I and Q data arrays in single precision that have to be stored.
          The vector has to be ordered as iqdata&lt;Channel1_I-array, Channel1_Q-array, Channel2_I-array, Channel2_Q-array, ...&gt;
          for complex data. The number of I/Q data arrays must match the specified value nofArrays in \ref writeOpen().
          @param [in] sizes Vector containing the length of each I/Q data-array.
          @returns If data was successfully added, ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int appendArrays(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes) = 0;

        /**
          @brief Adds new I/Q data with double precision to the file.
          @pre Make sure to call \ref writeOpen() to enter write-mode.
          @param [in]  iqdata I and Q data vectors in double precision that have to be stored.
          The vectors have to be ordered as iqdata&lt;Channel1_I, Channel1_Q, Channel2_I, Channel2_Q, ...&gt;
          for complex data. The number of I/Q data vectors must match the specified value nofArrays in \ref writeOpen().
          @returns If data was successfully added, ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int appendArrays(const std::vector<std::vector<double>>& iqdata) = 0;

        /**
          @brief Adds new I/Q data with double precision to the file.
          @pre Make sure to call \ref writeOpen() to enter write-mode.
          @param [in]  iqdata I and Q data arrays in single precision that have to be stored.
          The vector has to be ordered as iqdata&lt;Channel1_I-array, Channel1_Q-array, Channel2_I-array, Channel2_Q-array, ...&gt;
          for complex data. The number of I/Q data arrays must match the specified value nofArrays in \ref writeOpen().
          @param [in] sizes Vector containing the length of each I/Q data-array.
          @returns If data was successfully added, ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int appendArrays(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes) = 0;

        /**
          @brief Adds new I/Q channel data with single precision to the file.
          @pre Make sure to call \ref writeOpen() to enter write-mode.
          @param [in]  iqdata Vector of channels, where each vector contains the I/Q values in interleaved format (I_1,Q_1,I_2,Q_2,...)
          for complex data.
          @returns If data was successfully added, ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int appendChannels(const std::vector<std::vector<float>>& iqdata) = 0;

        /**
          @brief Adds new I/Q channel data with single precision to the file.
          @pre Make sure to call \ref writeOpen() to enter write-mode.
          @param [in]  iqdata Vector of channels, where each vector contains the I/Q values in interleaved format (I_1,Q_1,I_2,Q_2,...)
          for complex data.
          @param [in] sizes Vector containing the length of each I/Q data-array.
          @returns If data was successfully added, ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int appendChannels(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes) = 0;

        /**
          @brief Adds new I/Q channel data with double precision to the file.
          @pre Make sure to call \ref writeOpen() to enter write-mode.
          @param [in]  iqdata Vector of channels, where each vector contains the I/Q values in interleaved format (I_1,Q_1,I_2,Q_2,...)
          for complex data.
          @returns If data was successfully added, ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int appendChannels(const std::vector<std::vector<double>>& iqdata) = 0;

        /**
          @brief Adds new I/Q channel data with double precision to the file.
          @pre Make sure to call \ref writeOpen() to enter write-mode.
          @param [in]  iqdata Vector of channels, where each vector contains the I/Q values in interleaved format (I_1,Q_1,I_2,Q_2,...)
          for complex data.
          @param [in] sizes Vector containing the length of each I/Q data-array.
          @returns If data was successfully added, ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int appendChannels(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes) = 0;
      };
    }
  }
}