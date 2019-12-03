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
* @file      dataimportexportbase.h
*
* @brief     This is the header file of class DataImportExportBase.
*
* @details   Base implementation of IDataImportExport providing basic functionality.
*						 Derive all file format implementations from this class.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include <string>
#include <vector>
#include <map>

#include "idataimportexport.h"
#include "daiexception.h"
#include "errorcodes.h"
#include "channelinfo.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief Base implementation of IDataImportExport providing basic functionality.
      * Derive all file format implementations from this class.
      */class DataImportExportBase : IDataImportExport
      {
      public:
        /**
          @brief Constructor. Initializes the class with the specified file.
          @param [in]  filename Fully qualified file path.
        */DataImportExportBase(const std::string& filename);

        /**
          @brief Destructor. closes() the file.
        */virtual ~DataImportExportBase() {}

        virtual int getMetadata(std::vector<ChannelInfo>& channelInfos, std::map<std::string, std::string>& metadata) const;

        /**
          @returns Returns a timestamp that is saved with the header as meta data, if
          the file format supports meta data. Otherwise 0 is returned.
        */virtual time_t getTimestamp() const;

        /**
          @brief Sets a timestamp that is saved with the meta data to file, if the 
          file format supports meta data.
          @param [in]  timestamp The time to be saved to file.
        */virtual void setTimestamp(const time_t timestamp);

        /**
          @brief Guarantees that all channels of the vector have unique names.
          @param [in]  channelInfos Vector of ChannelInfo() objects.
          @returns Returns TRUE if no channel name appears twice in the vector, otherwise FALSE
          is returned.
        */static bool verifyUniqueChannelNames(const std::vector<ChannelInfo>& channelInfos);

        /**
          @brief Verifies the input data to be consistent:
          - Check if number and length of data arrays matches information specified by channelInfoList.size() and the specified I/Q data format,
          - I and Q data arrays of one channel have equal length,
          - Check that all channel names are unambiguous.
          @param [in]  channelInfos ChannelInfo objects that should match the data given. Channel names need to be unique.
          @param [in]  sizes Array containing the lengths of the individual I/Q data arrays, given in the following order: "Channel1_I,
          Channel1_Q, Channel2_I, Channel2_Q, ..." for complex data. Equal length of all channels is NOT necessary, but I and Q array of one channel
          must be of equal length.
          @param [in]  format Format of the I/Q data (real, complex or polar).
          @returns TRUE if all input parameters are consistent. FALSE otherwise.
        */static bool validateInputData(const std::vector<ChannelInfo>& channelInfos, const std::vector<size_t>& sizes, IqDataFormat format);

        /** 
          @brief Returns the size of the specified data type, i.e. IqDataType::Float32 is of 
          size 4 and IqDataType::Float64 of size 8. If the specified type is unknown, 0 is returned.
          @param [in]  datatype Data type used by the I/Q data file.
          @returns The size of the specified data type.
        */static size_t getWordWidth(IqDataType datatype);

        /**
          @returns Returns the number of values per samples, based on
          the data format, i.e. 1 for IqDataFormat::Real.
          @param [in]  format Data format used by the I/Q data file.
        */static size_t getValuesPerSample(IqDataFormat format);

        /**
          @brief Initializes the data type used to save I/Q data to file. The data type is 
          set during the first call of appendArrays() or appendChannels(), where the data type is
          specified by the I/Q data passed and is identified via the respective template argument.
          All subsequent calls of appendArrays() or appendChannels() have to be of the same data type, 
          i.e. mixing float and double arrays is prevented.
          @tparam Data type of I/Q data to be written.
          @param [in] dataType  Data type to be used to write data.
          @param [in] lock  TRUE if data type has already been set in a previous write action.
          If the type is locked, the value of parameter dataType must match the template type,
          i.e. template type is float and dataType is IqDataType::FLoat32.
          @throws Throws DaiException(ErrorCodes::WrongDataType) if the data type used to write I/Q data has already been set and
          the specified type does not the initialized type.
        */template<typename T>
        void static initializeDataType(IqDataType& dataType, bool& lock)
        {
          IqDataType writeType;

#ifdef _MSC_VER
          __pragma(warning(push))
          __pragma(warning(disable:4127))
#elif __GNUC__
          #pragma GCC diagnostic push
          #pragma GCC diagnostic warning "-w"
#endif

          if (std::is_same<T, float>::value)
          {
            writeType = IqDataType::Float32;
          }
          else if (std::is_same<T, double>::value)
          {
            writeType = IqDataType::Float64;
          }
          else
          {
            throw DaiException(ErrorCodes::WrongDataType);
          }

#ifdef _MSC_VER
          __pragma(warning(pop))
#elif __GNUC__
          #pragma GCC diagnostic pop
#endif
          

          if (lock && writeType != dataType)
          {
            throw DaiException(ErrorCodes::WrongDataType);
          }

          dataType = writeType;
          lock = true;
        }

      protected:
        virtual int readOpen(std::vector<std::string>& arrayNames);
        virtual int writeOpen(
          IqDataFormat format,
          size_t nofArrays,
          const std::string& applicationName,
          const std::string& comment,
          const std::vector<ChannelInfo>& channelInfos,
          const std::map<std::string, std::string>* metadata);

        /**
          @brief Adds the specified metadata to the internal dictionary.
          @param [in]  key The unique key of the metadata.
          @param [in]  value The actual value of the metadata.
        */virtual void addMetadata(const std::string& key, const std::string& value);

        /**
          @brief Adds a new channel to this file.
          @param [in]  channelName Name of the channel.
          @param [in]  clockRate Clock frequency in [Hz], i.e. the sampling rate of the I/Q data.
          @param [in]  centerFreq Center frequency of the modulated waveform in [Hz].
          @param [in]  samples Number of samples hold by this channel.
        */virtual void addChannel(const std::string& channelName, double clockRate, double centerFreq, size_t samples);

        /**
          @returns Returns a vector containing information for each channel in the file.
        */virtual const std::vector<ChannelInfo>& getChannelInfos() const;

        /**
          @returns Returns the number of channels in the data file.
        */virtual size_t getChannelCount() const;

        /** @brief Path to the corresponding I/Q data file. **/
        std::string filename_;

        /** @brief If supported by the file format, meta data can be provided as key-value pair. **/
        std::map<std::string, std::string> metadata_;

        /** @brief Information for each channel in the data file. **/
        std::vector<ChannelInfo> channelInfos_;

        /** @brief Timestamp saved with metadata. **/
        time_t timestamp_;

      private:
        /** @brief Private default constructor. */
        DataImportExportBase();

        /** @brief Private copy constructor. */
        DataImportExportBase(const DataImportExportBase&);

        /** @brief Private assignment operator.*/
        DataImportExportBase& operator=(const DataImportExportBase&);

        /** @brief Clears metadata, channelInfo and timestamp. */
        void clearInternalState();
      };
    }
  }
}