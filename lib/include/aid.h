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

#include "idataimportexport.h"

namespace rohdeschwarz
{
namespace mosaik
{
namespace dataimportexport
{

class AidImpl;

/**
* @brief Class to read or write I/Q data from or to an AID file.
* AID is the binary file format used by AMMOS.
* Format description details:
* - IQDataFormat.Complex
* - Several data types
* - Little and Big Endian byte order
* - The data order is always IQIQIQ.
*/

class MOSAIK_MODULE Aid : public IDataImportExport
{
public:

    /**
      @brief Constructor. Initializes a new instance with the specified filename.
      Filename must be UTF-8 encoded. Be aware that the filename cannot be changed after construction.
      @param [in]  filename Name of the file to be read or to be written.
    */  Aid(const std::string& filename);

    /** @brief Destructor. Calls close()
    */ virtual ~Aid();

#if _MSC_VER > 1700 // from VS2013
    /** @brief Copy constructor. not implemented
    */ Aid(const Aid&) = delete;
#endif
    /** @brief Copy constructor reference.
    */ Aid(Aid&& other);

#if _MSC_VER > 1700 // from VS2013
    /** @brief Assign operator. not implemented
    */ Aid& operator=(const Aid&) = delete;
#endif

    /** @brief Assign operator reference.
    */ Aid& operator=(Aid&& other);


  /** @brief Open the Aid file and read meta data and array and channel information.
  */ int readOpen(std::vector<std::string>& arrayNames) override;

  /** @brief Create and open the Aid file. 
   * Only interleaved IQIQ format is allowed.
  */ int writeOpen(
    IqDataFormat format,
    size_t nofArrays,
    const std::string& applicationName,
    const std::string& comment,
    const std::vector<ChannelInfo>& channelInfos,
    const std::map<std::string, std::string>* metadata = 0) override;

  /** @brief Close the file.
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

private:
  /// pointer to implementation class
  AidImpl* m_pimpl;
};

}
}
}
