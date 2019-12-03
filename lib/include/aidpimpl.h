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

#pragma once

#include <time.h>

#include <memory>
#include <string>
#include <vector>
#include "aid.h"
#include "dataimportexportbase.h"
#include "ZFFileReader.h"
#include "ZFFileWriter.h"

namespace rohdeschwarz
{
	namespace mosaik
	{
		namespace dataimportexport
		{
      using namespace AmlabFiles;
      using namespace AmlabCommon;

			/// @brief Private implementation corresponding to class Aid.
			class AidImpl : public DataImportExportBase
			{
			public:
				/**
				@brief Constructor. Initializes a new instance with the specified filename.
				Filename must be UTF-8 encoded. Be aware that the filename cannot be changed after construction.
				@param [in]  filename Name of the file to be read or to be written.
				*/ AidImpl(const std::string& filename);
				/** @brief Destructor. Calls close()
				*/virtual ~AidImpl();

				/// @brief Opens the iqx file for reading and returns the names of existing arrays
				  int readOpen(std::vector<std::string>& arrayNames);

				  /// @brief Opens the iqx file for writing
				  int writeOpen(IqDataFormat format, size_t nofArrays, const std::string& applicationName, const std::string& comment,
					  const std::vector<ChannelInfo>& channelInfos, const std::map<std::string, std::string>* metadata);
				  /// @brief Closes the file and writes meta data
				  int close();
				  /// @brief Returns contained meta data and channel infos
				  int getMetadata(std::vector<ChannelInfo>& channelInfos, std::map<std::string, std::string>& metadata) const;
				  /// @brief Gets the time stamp of the file
				  time_t getTimestamp() const;
				  /// @brief Gets the size of a particular array
				  int64_t getArraySize(const std::string& arrayName) const;
				  /// @brief Sets the time stamp of the file
				  void setTimestamp(const time_t timestamp);
				  /// @brief read array into float vector
				  int readArray(const std::string& arrayName, std::vector<float>& values, size_t nofValues, size_t offset);
				  /// @brief read array into preallocated float array
				  int readArray(const std::string& arrayName, float* values, size_t nofValues, size_t offset);
				  /// @brief read array into double vector
				  int readArray(const std::string& arrayName, std::vector<double>& values, size_t nofValues, size_t offset);
				  /// @brief read array into preallocated double array
				  int readArray(const std::string& arrayName, double* values, size_t nofValues, size_t offset);
				  /// @brief read channel into a float vector
				  int readChannel(const std::string& channelName, std::vector<float>& values, size_t nofValues, size_t offset);
				  /// @brief read channel into a preallocated float array
				  int readChannel(const std::string& channelName, float* values, size_t nofValues, size_t offset);
				  /// @brief read channel into double vector
				  int readChannel(const std::string& channelName, std::vector<double>& values, size_t nofValues, size_t offset);
				  /// @brief read channel into double array
				  int readChannel(const std::string& channelName, double* values, size_t nofValues, size_t offset);
				  /// @brief append arrays at float vector
				  int appendArrays(const std::vector<std::vector<float> >& iqdata);
				  /// @brief append arrays
				  int appendArrays(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes);
				  /// @brief append arrays
				  int appendArrays(const std::vector<std::vector<double> >& iqdata);
				  /// @brief append arrays
				  int appendArrays(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes);
				  /// @brief append arrays
				  int appendChannels(const std::vector<std::vector<float> >& iqdata);
				  /// @brief append channels
				  int appendChannels(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes);
				  /// @brief append channels
				  int appendChannels(const std::vector<std::vector<double> >& iqdata);
				  /// @brief append channels
				  int appendChannels(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes);

			private:

				/// @brief for dataimport export api: reading
				enum rType {
					rFloatVector,
					rDoubleVector,
					rFloatPointer,
					rDoublePointer
				};

				/// @brief for dataimport export api: writing
				enum wType {
					wFloat,
					wDouble
				};


				/// reset data when open is called
				void resetData();

        /// read data from aid file
        int readAid(size_t nofValues, size_t offset);


				/// file name
				const std::string m_filename;
				/// ignored, we store only IQIQIQ
				IqDataFormat m_format;
				/// no of arrays from writeOpen request
				size_t m_nofArrays;
				/// application name from writeOpen request
				std::string m_applicationName;
				/// comment from writeOpen request
				std::string m_comment;
				/// channel info
				std::vector<ChannelInfo> m_channelInfo;
				/// meta data
				std::map<std::string, std::string> m_metaData;
				/// indicates if file is opened for read or write
				bool m_write;
				uint64 m_samples;
				time_t m_timeStamp;

        CZFFileReader m_reader;
        CZFFileWriter m_writer;
        CArrayComplex m_cArr;
      };
		}
	}
} // namespace
