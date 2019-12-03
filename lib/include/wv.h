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
* @file      wv.h
*
* @brief     This is the header file of class Wv.
*
* @details   Class to read or write I/Q data from or to an wv file.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include "idataimportexport.h"

class WvScramblerBase;

namespace rohdeschwarz
{
	namespace mosaik
	{
		namespace dataimportexport
		{
			class WvImpl;
			/**
			* @brief Class to read or write I/Q data from or to an WV file.
			* Format description details:
			* - IQDataFormat.Complex
			* - IQDataType.Int16 will be converted to float or double.
			* - Intel byte order
			* - The data order is always IQIQIQ.
			*/

			class MOSAIK_MODULE Wv : public IDataImportExport
			{
			public:

				/**
				@brief Constructor. Initializes a new instance with the specified filename.
				Filename must be UTF-8 encoded. Be aware that the filename cannot be changed after construction.
				@param [in]  filename Name of the file to be read or to be written.
				*/  Wv(const std::string& filename);

				/** @brief Destructor. Calls close()
				*/ virtual ~Wv();

#if _MSC_VER > 1700 // from VS2013
				/** @brief Copy constructor. not implemented
				*/ Wv(const Wv&) = delete;
#endif
				/** @brief Copy constructor reference.
				*/ Wv(Wv&& other);

#if _MSC_VER > 1700 // from VS2013
				/** @brief Assign operator. not implemented
				*/ Wv& operator=(const Wv&) = delete;
#endif

				/** @brief Assign operator reference.
				*/ Wv& operator=(Wv&& other);


				/** @brief Open the Wv file and read meta data and array and channel information.
				*/ int readOpen(std::vector<std::string>& arrayNames) override;

				/** @brief Create and open the Wv file. Writing of meta data and array and channel information
				* has is done, when closing the file, because the meta data is stored at the end of the file.
				* Only interleaved IQIQ format is allowed.
				*/ int writeOpen(
	IqDataFormat format,
	size_t nofArrays,
	const std::string& applicationName,
	const std::string& comment,
	const std::vector<ChannelInfo>& channelInfos,
	const std::map<std::string, std::string>* metadata = 0) override;

				  /** @brief Close the file and write meta data.
				  */ int close() override;

				  /** @brief Return channel info and other meta data
				  */ int getMetadata(std::vector<ChannelInfo>& channelInfos, std::map<std::string, std::string>& metadata) const override;

				  int64_t getArraySize(const std::string& arrayName) const override;

				  time_t getTimestamp() const override;

				  void setTimestamp(const time_t timestamp) override;

				  int readArray(const std::string& arrayName, std::vector<float>& values, size_t nofValues, size_t offset = 0) override;

				  int readArray(const std::string& arrayName, float* values, size_t nofValues, size_t offset = 0) override;

				  int readArray(const std::string& arrayName, std::vector<double>& values, size_t nofValues, size_t offset = 0) override;

				  int readArray(const std::string& arrayName, double* values, size_t nofValues, size_t offset = 0) override;

				  int readChannel(const std::string& channelName, std::vector<float>& values, size_t nofValues, size_t offset = 0) override;

				  int readChannel(const std::string& channelName, float* values, size_t nofValues, size_t offset = 0) override;

				  int readChannel(const std::string& channelName, std::vector<double>& values, size_t nofValues, size_t offset = 0) override;

				  int readChannel(const std::string& channelName, double* values, size_t nofValues, size_t offset = 0) override;

				  /// @brief append arrays
				  int appendArrays(const std::vector<std::vector<float> >& iqdata) override;
				  /// @brief append arrays
				  int appendArrays(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes) override;
				  /// @brief append arrays
				  int appendArrays(const std::vector<std::vector<double> >& iqdata) override;

				  /// @brief append arrays
				  int appendArrays(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes) override;

				  /// @brief append channels
				  int appendChannels(const std::vector<std::vector<float> >& iqdata) override;

				  /// @brief append channels
				  int appendChannels(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes) override;

				  /// @brief append channels
				  int appendChannels(const std::vector<std::vector<double> >& iqdata) override;

				  /// @brief append channels
				  int appendChannels(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes) override;

				  void setScrambler(WvScramblerBase * scrambler);

			private:
				/** @brief Private implementation */
				class Impl;

				/** @brief Private implementation */
				Impl* m_pimpl;
			};

		}
	}
}
