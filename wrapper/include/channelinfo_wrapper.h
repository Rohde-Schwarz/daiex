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

#include "channelinfo.h"

namespace RohdeSchwarz
{
  namespace Mosaik
  {
    namespace DataImportExport
    {
      /// <summary>
      /// Wraps up the native ChannelInfo object defined in channelinfo.h.
      /// </summary>
      public ref class ChannelInfo
      {
      public:
        /// <summary>Initializes a new instance of the <see cref="ChannelInfo"/> class.</summary>
        /// <param name="name">Name of the channel.</param>
        /// <param name="clock">Clock frequency in [Hz], i.e. the sampling rate of the I/Q data of this channel.</param>
        /// <param name="centerFreq">Center frequency used when creating the I/Q data of this channel.</param>
        ChannelInfo(System::String ^name, double clock, double centerFreq);

        /// <summary>Finalizes an instance of the <see cref="ChannelInfo"/> class.</summary>
        ~ChannelInfo();

        /// <summary>Gets the name for this channel. Will be used to identify the data within a file.</summary>
        property System::String^ ChannelName
        {
          System::String^ get();
        }

        /// <summary>
        /// Gets the clock frequency in Hz, i.e. the sampling rate of the I/Q data. A
        /// signal generator typically outputs the I/Q data at a rate that equals the clock
        /// frequency. If the I/Q data was captured with a signal analyzer, the signal
        ///	analyzer used the clock frequency as sampling rate.
        ///	</summary>
        property double ClockRate
        {
          double get();
        }

        /// <summary>
        /// Gets the center frequency of the modulated waveform in [Hz].
        /// </summary>
        property double Frequency
        {
          double get();
        }

        /// <summary>
        /// Gets the number of samples of this channel. For instance a single sample can be
        /// - a complex number represented as a pair of I and Q values, or
        /// - a complex number represented as a pair of magnitude and phase values, or
        /// - a real number represented as a single real value.
        /// </summary>
        property size_t Samples
        {
          size_t get();
        }

      protected:
        /// <summary>Finalizer, cleans up native resources.</summary>
        !ChannelInfo();

      private:
        /// <summary>Handle to the native object that is wrapped up by this class.</summary>
        rohdeschwarz::mosaik::dataimportexport::ChannelInfo *nativeImpl_;
      };
    }
  }
}