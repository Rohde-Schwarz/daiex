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
* @details   Class to group all information needed about the data of one channel.
*/

#pragma once

#include <string>
#include <vector>

#include "exportdecl.h"
#include "enums.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief Class to group all information needed about the data of one channel.
      * Information related to a channel is typically the channel name, the channel's clock
      * frequency and the center frequency used when creating the I/Q data. 
      */
      class ChannelInfo
      {
      public:
        /**
          @brief Constructor.
          @param [in]  name Name of the channel.
          @param [in]  clock Clock frequency in [Hz], i.e. the sampling rate of the I/Q data of this channel.
          @param [in]  centerFreq Center frequency used when creating the I/Q data of this channel.
        */MOSAIK_MODULE ChannelInfo(const std::string &name, double clock, double centerFreq);

        /**
          @brief Constructor.
          @param [in]  name Name of the channel.
          @param [in]  clock Clock frequency in [Hz], i.e. the sampling rate of the I/Q data of this channel.
          @param [in]  centerFreq Center frequency used when creating the I/Q data of this channel.
          @param [in]  samples Number of I/Q samples contained by the channel.
        */MOSAIK_MODULE ChannelInfo(const std::string &name, double clock, double centerFreq, size_t samples);

        /**
          @brief Creates a default name for a channel with the given channel number. The default channel name
          is used by this library, if no name was specified explicitly by the user.
          @param [in]  channelCount Index of the channel in the channel list.
          @returns Returns the default name of a channel with respect to the specified channel count,
          i.e. 'Channel1' is returned if channelCount equals zero. 
        */MOSAIK_MODULE static std::string getDefaultChannelName(int channelCount);

        /**
          @brief Creates a default name for an array with respect to the specified channel number. The default
          array name is used by this library, if no name was specified explicitly by the user.
          @param [in]  channelCount Index of the channel in the channel list.
          @param [in]  representI True if array represents I values.
          @returns The default name for an I or Q array in the given channel, e.g. 'Channel1_I' is returned
          if channelCount equals zero and the channel represents I values.
        */MOSAIK_MODULE static std::string getDefaultArrayName(int channelCount, bool representI);

        /**
          @brief Gets the name for this channel. Will be used to identify the data within a file.
          @returns The name of the channel.
        */MOSAIK_MODULE const std::string& getChannelName() const;

        /**
          @brief Gets the clock frequency in [Hz], i.e. the sampling rate of the I/Q data. A
          signal generator typically outputs the I/Q data at a rate that equals the clock
          frequency. If the I/Q data was captured with a signal analyzer, the signal
          analyzer used the clock frequency as sampling rate.
          @returns The clock frequency of the channel's data.
        */MOSAIK_MODULE double getClockRate() const;

        /**
          @brief Gets the center frequency of the modulated waveform in [Hz].
          @returns The center frequency of the channel's data.
        */MOSAIK_MODULE double getFrequency() const;

        /**
          @brief Gets the number of samples of this channel. For instance a single sample can be
          - a complex number represented as a pair of I and Q values, or
          - a complex number represented as a pair of magnitude and phase values, or
          - a real number represented as a single real value.
          @returns The number of samples of this channel.
        */MOSAIK_MODULE size_t getSamples() const;

      private:
        /** @brief Prevents execution of the default constructor. */
        ChannelInfo();

        /** @brief Name of the channel. */
        std::string name_;

        /** @brief Clock rate of the channel. */
        double clock_;

        /** @brief Center frequency of the channel. */
        double freq_;

        /** @brief Number of samples contained by channel. */
        size_t samples_;
      };
    }
  }
}