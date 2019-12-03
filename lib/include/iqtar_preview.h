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

/* @cond HIDDEN_SYMBOLS */

#pragma once

#ifdef _MSC_VER
__pragma(warning(push))
__pragma(warning(disable: 4244))
__pragma(warning(disable: 4996))
__pragma(warning(disable: 4389))
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
#endif

#include <vector>
#include <complex>
#include <limits>
#include <cmath>

#include "iqtar_preview_types.h"
#include "iqtar_iq_preview.h"
#include "iqtar_trace_preview.h"
#include "pwelch.h"

#include "daiexception.h"
#include "errorcodes.h"
#include "enums.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      class IqTarPreview final
      {
      public:
        IqTarPreview() : m_initialized(false) {};

        /**
          @brief Initializes the I/Q-Tar-Preview.
          @param [in]  iqTarPvTPreviewLength IqTar PvT Preview Length. The number of preview samples to calculate. Must be > 0.
          @param [in]  iqTarSpectrumPreviewOrder IqTar Spectrum Preview Order. The order of the FFT to be used for the Spectrum preview calculation (length = 2^order). Must be > 0.
          @param [in]  iqTarIqPreviewNofPositiveBins IqTar I/Q Preview Number of positive bins - The number of bins of the positive I or Q axis (2^n recommended). Must be > 0.
          @param [in]  nofChannels
        @returns Returns TRUE if initialization was successful.
        */bool initialize(
          int iqTarPvTPreviewLength,
          int iqTarSpectrumPreviewOrder,
          int iqTarIqPreviewNofPositiveBins,
          int nofChannels);

        template<typename T>
        void addChannelData(const std::vector<T*>& iqdata, size_t nofValues, IqDataFormat dataFormat)
        {
          std::vector<std::complex<float>> interleavedData;

          if (dataFormat == IqDataFormat::Real)
          {
            interleavedData.reserve(nofValues * m_iNofChannels);

            for (auto i = 0; i < nofValues; ++i)
            {
              for (auto ch = 0; ch < iqdata.size(); ++ch)
              {
                interleavedData.push_back(std::complex<float>(iqdata[ch][i], 0.0));
              }
            }
          }
          else
          {
            if (nofValues % 2 != 0)
            {
              throw DaiException(ErrorCodes::InternalError);
            }

            interleavedData.reserve((nofValues / 2) * m_iNofChannels);

            for (auto i = 0; i < nofValues / 2; ++i)
            {
              for (auto ch = 0; ch < iqdata.size(); ++ch)
              {
                interleavedData.push_back(std::complex<float>(iqdata[ch][i*2], iqdata[ch][i*2 + 1]));
              }
            }
          }

          this->add(interleavedData);
        }

        template<typename T>
        void addArrayData(const std::vector<T*>& iqdata, size_t nofValues, IqDataFormat dataFormat)
        {
          std::vector<std::complex<float>> interleavedData;

          if (dataFormat == IqDataFormat::Real)
          {
            interleavedData.reserve(nofValues * m_iNofChannels);

            for (auto i = 0; i < nofValues; ++i)
            {
              for (auto ch = 0; ch < iqdata.size(); ++ch)
              {
                interleavedData.push_back(std::complex<float>(iqdata[ch][i], 0.0));
              }
            }
          }
          else
          {
            if (iqdata.size() % 2 != 0)
            {
              throw DaiException(ErrorCodes::InternalError);
            }

            interleavedData.reserve((nofValues / 2) * m_iNofChannels);

            for (auto i = 0; i < nofValues; ++i)
            {
              for (auto arr = 0; arr < iqdata.size(); arr += 2)
              {
                interleavedData.push_back(std::complex<float>(iqdata[arr][i], iqdata[arr + 1][i]));
              }
            }
          }

          this->add(interleavedData);
        }

        void getPreviews(std::vector<SChannelPreview>& previews);

        /**
        @returns Returns TRUE if initialize() has been called.
        */bool isInitialized() const
        {
          return this->m_initialized;
        }

        template<typename T>
        static void vVtoDb(std::vector<T>& min_trace, std::vector<T>& max_trace)
        {
          // Determine total minimum / maximum from minimum / maximum trace and exclude values <= 0 V^2.
          T my_max = std::numeric_limits<T>::lowest();
          T my_min = std::numeric_limits<T>::max();

          if (min_trace.size() != max_trace.size())
          {
            throw DaiException(ErrorCodes::InconsistentInputData, "tracesToDb(): min_trace() and min_trace() have different lengths.");
          }

          for (auto idx = 0; idx < min_trace.size(); idx++)
          {
            //Determine minimum
            if (min_trace[idx] > 0 && min_trace[idx] < my_min)
            {
              my_min = min_trace[idx];
            }

            //Determine maximum
            if (max_trace[idx] > 0 && max_trace[idx] > my_max)
            {
              my_max = max_trace[idx];
            }
          }

          if (my_min == std::numeric_limits<T>::max())
          {
            //my_min = no value found
            if (my_max == std::numeric_limits<T>::lowest())
            {
              //my_max = no value found
              //Incorrect traces or all-zero signal => return traces with -200 dB (=1e-20 V^2)
              my_min = 1.0E-20;
              my_max = 1.0E-20;
            }
            else
            {
              //my_max > 0 (value found)
              my_min = my_max;
            }
          }
          else
          {
            //my_min > 0 (value found)
            if (my_max == std::numeric_limits<T>::lowest())
            {
              //my_max = no value found
              my_max = my_min;
            }
            //else
            //{
            //my_max > 0 (value found)
            //Both values okay (ignore if the min and max are switched)
            //}
          }

          if (my_min == my_max)
          {
            //Set my_min 100 dB below my_max
            my_min = 1.0E-10F * my_max;
          }

          //Replace values smaller or equal to zero by my_min and my_max
          for (auto idx = 0; idx < min_trace.size(); idx++)
          {
            if (min_trace[idx] <= 0)
            {
              min_trace[idx] = my_min;
            }

            if (max_trace[idx] <= 0)
            {
              //Use my_min - 10dB for min_trace (my_max was only required to calculate my_min)
              max_trace[idx] = 0.1F * my_min;
            }
          }

          //Calculate dB values and round (now save) (inplace operation)
          for (auto idx = 0; idx < min_trace.size(); idx++)
          {
            //Nicht so wichtig hier, wie gerundet wird (nur Preview...)
            min_trace[idx] = std::floor(10.0 * std::log10(min_trace[idx]));
            max_trace[idx] = std::ceil(10.0 * std::log10(max_trace[idx]));
          }
        }

      private:
        /**
        @brief Adds a new I/Q data block to the preview.
        @param [in]  vfcIqDataFloat32 I/Q Data Float32 The I/Q data in interleaved float32 format.  Multiple channels are also interleaved per I/Q sample.
        @throws Throws a DaiException in any error case.
        */void add(const std::vector<std::complex<float>> &vfcIqDataFloat32);

        /**  True if preview class has been initialized **/
        bool m_initialized;

        /** IqTar PvT Preview Length - The number of preview samples to calculate.  If this value is zero no preview will be calculated. */
        int m_uiIqTarPvTPreviewLength;

        /** IqTar Spectrum Preview Order - The order of the FFT to be used for the Spectrum preview calculation (length = 2^order).  If this value is zero no preview will be calculated. */
        int m_uiIqTarSpectrumPreviewOrder;

        /** IqTar I/Q Preview Number of positive bins - The number of bins of the positive I or Q axis (2^n recommended).  If this value is zero no preview will be calculated. */
        int m_uiIqTarIqPreviewNofPositiveBins;

        /** Number of channels - Number of channels of multi-channel signals. */
        int m_iNofChannels;

        /** Total number of samples in PvT preview - Total number of samples taken into account by the PvT previews. */
        unsigned long long m_uiTotalNofSamplesInPvtPreview;

        /** Total number of samples in Spectrum preview - Total number of samples taken into account by the Spectrum previews. */
        unsigned long long m_uiTotalNofSamplesInSpectrumPreview;

        /** Total number of samples in I/Q preview - Total number of samples taken into account by the I/Q preview. */
        unsigned long long m_uiTotalNofSamplesInIqPreview;

        /** PvT Preview Min - Class instance for the calculation of the Power vs Time minimum preview trace. */
        CTracePreview< CTPDetectorMinimum > m_tPvtPreviewMin;

        ///** PvT Preview Max - Class instance for the calculation of the Power vs Time maximum preview trace. */
        CTracePreview< CTPDetectorMaximum > m_tPvtPreviewMax;

        /** Spectrum Preview Min Array - Class instance for the calculation of the Spectrum minimum preview trace for every channel. */
        std::vector<CPWelch<float>> m_vtSpectrumPreviewMin;

        /** Spectrum Preview Max Array - Class instance for the calculation of the Spectrum maximum preview trace for every channel. */
        std::vector<CPWelch<float>> m_vtSpectrumPreviewMax;

        /** I/Q Preview - Class instance for the calculation of the I/Q preview histogram for every channel. */
        CIqPreview<float> m_tIqPreview;
      };
    }
  }
}

#ifdef _MSC_VER
#pragma warning (pop)
#elif __GNUC__
#pragma GCC diagnostic pop
#endif

/*** @endcond ***/