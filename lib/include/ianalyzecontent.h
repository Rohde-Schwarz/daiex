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
* @file      ianalyzecontent.h
*
* @brief     This is the header file of class IAnalyzeContent.
*
* @details   This class contains the interface definition IAnalyzeContent. Declares methods called
*            after parsing an I/Q file to update the internal object state w.r.t. channel information
*            and meta data that was found in the file.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include <string>
#include <ctime>

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief Declares methods that are called after parsing an I/Q file in order to update the internal object
      * state w.r.t. channel information and meta data that was found in the file. 
      */
      class IAnalyzeContent
      {
      public:
        /**
          @brief Called whenever a new channel was found while parsing a file. 
          @param [in]  channelName String identifying the channel. Will be used to build the array names, 
          which are necessary to retrieve the according data.
          @param [in]  clock Clock frequency in Hz, i.e. the sample rate of the I/Q data.
          @param [in]  frequency Center frequency of the modulated waveform in [Hz].
          @param [in]  samples Number of samples of the channel.
        */virtual void updateChannels(const std::string& channelName, double clock, double frequency, size_t samples) = 0;

        /**
          @brief Called whenever user-specific meta data was found in a file.
          @param [in]  key Identifying the name of a meta information.
          @param [in]  value The value (as string  of the meta information named 'key'.
        */virtual void updateMetadata(const std::string& key, const std::string& value) = 0;

        /**
          @brief Called whenever a timestamp was found in the meta data of the file.
          @param [in]  timestamp Timestamp of the I/Q file.
        */virtual void updateTimestamp(const time_t timestamp) = 0;
      };
    }
  }
}