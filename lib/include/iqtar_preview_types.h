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

/******************************************************************************/
/**
@copyright     (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich
*
@language      C++
*
@description   The declaration of all XML types used by the block "IqTar Calc Preview"
@description   generated from iqtar_blocks.xml
*******************************************************************************/

#ifdef _MSC_VER
__pragma(warning(push))
__pragma(warning(disable: 4244))
__pragma(warning(disable: 4996))
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic warning "-w"
#endif

#include <string>

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /* STRUCTURE ******************************************************************/
      /** Min / Max Preview
      *
      * Minimum and maximum preview trace data for a single I/Q channel.
      *
      *******************************************************************************/
      struct SMinMaxPreview
      {
        /** Min - Minimum-values trace. Should have the same length as the max-values trace. */
        std::vector<float> vfMin;

        /** Max - Maximum-values trace. Should have the same length as the min-values trace. */
        std::vector<float> vfMax;

        /** Default constructor */
        SMinMaxPreview()
          :
          vfMin(),
          vfMax()
        {
        }

        /** Constructor with initialisation */
        SMinMaxPreview(
          const std::vector<float> &vfMinInit,
          const std::vector<float> &vfMaxInit
          )
          :
        vfMin(vfMinInit),
          vfMax(vfMaxInit)
        {
        }
      };

      /* STRUCTURE ******************************************************************/
      /** I/Q Preview
      *
      * I/Q preview for a single I/Q channel.
      *
      *******************************************************************************/
      struct SIqPreview
      {
        /** Histogram - Vector with characters 0-9. 0 means no I/Q sample in that bin, 9 means many I/Q samples in that bin.  Length of vector = width * height. Each character corresponds to one bin (pixel). The order is: first line from left to rigth, then second line from left to right, ... */
        std::string sHistogram;

        /** Width - Width, i.e. number of pixels in I direction. */
        unsigned int uiWidth;

        /** Height - Height, i.e. number of pixels in Q direction. */
        unsigned int uiHeight;

        /** Default constructor */
        SIqPreview()
          :
          sHistogram(""),
          uiWidth(0),
          uiHeight(0)
        {
        }

        /** Constructor with initialisation */
        SIqPreview(
          const std::string &sHistogramInit,
          const unsigned int &uiWidthInit,
          const unsigned int &uiHeightInit
          )
          :
        sHistogram(sHistogramInit),
          uiWidth(uiWidthInit),
          uiHeight(uiHeightInit)
        {
        }
      };

      /* STRUCTURE ******************************************************************/
      /** Channel Preview
      *
      * PvT and Spectrum preview trace data for a single I/Q channel.
      *
      *******************************************************************************/
      struct SChannelPreview
      {
        /** Power vs Time - Power vs Time: Data representing the power of the data of this channel. This data can be used to generate a small diagram of the signal. The data is calculated as 20*log10(abs(I/Q)). */
        SMinMaxPreview tPowerVsTime;

        /** Spectrum - Spectrum: Data representing the spectrum of the data of this channel. This data can be used to generate a small diagram of the signal. The data is calculated as 20*log10(abs(fftshift(fft(I/Q)))). */
        SMinMaxPreview tSpectrum;

        /** I/Q - I/Q: Histogram representation of the I/Q samples in the complex cartesian I/Q plane. */
        SIqPreview tIQ;

        /** Default constructor */
        SChannelPreview()
          :
          tPowerVsTime(),
          tSpectrum(),
          tIQ()
        {
        }

        /** Constructor with initialisation */
        SChannelPreview(
          const SMinMaxPreview &tPowerVsTimeInit,
          const SMinMaxPreview &tSpectrumInit,
          const SIqPreview &tIQInit
          )
          :
        tPowerVsTime(tPowerVsTimeInit),
          tSpectrum(tSpectrumInit),
          tIQ(tIQInit)
        {
        }
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