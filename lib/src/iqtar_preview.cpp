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

#ifdef _MSC_VER
__pragma(warning(push))
__pragma(warning(disable: 4244))
__pragma(warning(disable: 4996))
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#endif

#include "iqtar_preview.h"

#include <sstream>

using namespace std;

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      bool IqTarPreview::initialize(
        int iqTarPvTPreviewLength,
        int iqTarSpectrumPreviewOrder,
        int iqTarIqPreviewNofPositiveBins,
        int nofChannels)
      {
        if (this->m_initialized)
        {
          return false;
        }

        if (iqTarPvTPreviewLength == 0 ||  iqTarSpectrumPreviewOrder == 0 || iqTarIqPreviewNofPositiveBins == 0 || nofChannels == 0)
        {
          return false;
        }

        this->m_uiIqTarPvTPreviewLength = iqTarPvTPreviewLength;
        this->m_uiIqTarSpectrumPreviewOrder = iqTarSpectrumPreviewOrder;
        this->m_uiIqTarIqPreviewNofPositiveBins = iqTarIqPreviewNofPositiveBins;
        this->m_iNofChannels = nofChannels;
        
        //=============================================================================
        // Initialization of preview classes
        //=============================================================================

        m_uiTotalNofSamplesInPvtPreview = 0;
        m_uiTotalNofSamplesInSpectrumPreview = 0;
        m_uiTotalNofSamplesInIqPreview = 0;

        //-----------------------------------------------------------------------------
        // Initialization of Power vs Time preview
        //-----------------------------------------------------------------------------

        try
        {
          // Update number of channels
          m_tPvtPreviewMin.SetNofChannels(m_iNofChannels);
          m_tPvtPreviewMax.SetNofChannels(m_iNofChannels);

          // Update preview lengths
          m_tPvtPreviewMin.SetPreviewLength(m_uiIqTarPvTPreviewLength);
          m_tPvtPreviewMax.SetPreviewLength(m_uiIqTarPvTPreviewLength);

          //-----------------------------------------------------------------------------
          // Initialization of Spectrum preview
          //-----------------------------------------------------------------------------

          // Yes, init spectrum preview
          // Anmerkung: "pwelch.h" kommt nicht klar mit iNffT = 0. Deshalb hier abfangen!
        
          // Prepare common values for spectrum previews
          const int iNfft = 1 << m_uiIqTarSpectrumPreviewOrder;
          const int iWindowLength = iNfft; // level accuracy does not matter
          static const double fOverlapRatio = 0.5; // small value -> faster
          static const t_eTAIL_HANDLING tTailHandling = eTAIL_HANDLING_COMPUTE; // include also last (tail) samples into spectrum calculation
          const int iRingBufferSize = 4*iNfft ; // arbitrarily chosen
          static const t_eWINDOW_TYPE tWindowType = eWINDOW_TYPE_FLATTOP;
          static const double fWindowParam = 0; // No meaning for flattop window
         
          // Update number of channels
          m_vtSpectrumPreviewMin.resize(m_iNofChannels);
          m_vtSpectrumPreviewMax.resize(m_iNofChannels);
        
          for (auto ch=0; ch<m_iNofChannels; ch++ ) 
          {
            // Initialization of  minimum spectrum preview class of channel 'ch'
            CPWelch<float> &oMin = m_vtSpectrumPreviewMin[ch];
            oMin.Init( iWindowLength,fOverlapRatio,iNfft );
            oMin.SetTailHandling( tTailHandling );
            oMin.SetAveragingMethod( eAVERAGING_METHOD_MIN ); // Minimum detector
            oMin.SetRingBufferSize( iRingBufferSize );
            oMin.SetWindowType( tWindowType, fWindowParam );
        
            // Initialization of  maximum spectrum preview class of channel 'ch'
            CPWelch<float> &oMax = m_vtSpectrumPreviewMax[ch];
            oMax.Init( iWindowLength,fOverlapRatio,iNfft );
            oMax.SetTailHandling( tTailHandling );
            oMax.SetAveragingMethod( eAVERAGING_METHOD_MAX ); // Maximum detector
            oMax.SetRingBufferSize( iRingBufferSize );
            oMax.SetWindowType( tWindowType, fWindowParam );
          } 

          //-----------------------------------------------------------------------------
          // Initialization of I/Q preview
          //-----------------------------------------------------------------------------

          // Update number of channels
          m_tIqPreview.SetNofChannels(m_iNofChannels);

          // Update preview size (number of positive bins)
          m_tIqPreview.SetNofPositiveBins(m_uiIqTarIqPreviewNofPositiveBins);
        }
        catch (...)
        {
          return false;
        }

        this->m_initialized = true;
        return true;
      }

      void IqTarPreview::add(const std::vector<std::complex<float>> &vfcIqDataFloat32)
      {
        if (false == this->m_initialized)
        {
          throw DaiException(ErrorCodes::InternalError);
        }

        try
        {
          //=============================================================================
          // Calculate Power vs Time previews. 
          //=============================================================================
        
          // Add ||^2 of new block data to previews
          std::vector<float> vSquareMagnitude;
          vSquareMagnitude.resize(vfcIqDataFloat32.size());
          std::transform(vfcIqDataFloat32.begin(), vfcIqDataFloat32.end(), vSquareMagnitude.begin(), [](complex<float> val) { return MagSqr(val); });

          m_tPvtPreviewMin.Feed(vSquareMagnitude);
          m_tPvtPreviewMax.Feed(vSquareMagnitude);

          // Update sample counters
          m_uiTotalNofSamplesInPvtPreview += static_cast<unsigned long long>(vfcIqDataFloat32.size() / m_iNofChannels);


          //=============================================================================
          // Calculate Spectrum preview
          //=============================================================================

          // Anzahl Samples eines Channels (gleich f�r alle Channels)
          // Anmerkung: Es wird derzeit nicht �berpr�ft, ob L==n*NofChannels. Funktioniert auch so, d.h. NofSamplesOfChannel=floor(L/NofChannels).
          
          const int iNofSamplesOfChannel = (int)vfcIqDataFloat32.size() / m_iNofChannels;
        
          // Nur weitermachen, falls Samples da sind (mindestens ein Sample pro Kanal)
          if ( iNofSamplesOfChannel > 0 ) 
          {
            // Buffer fuer I/Q-Block eines einzelnen Kanals anlegen
            // Anmerkung: Buffer kann fuer alle Kanaele verwendet werden (alle gleich lang)
            vector<complex<float>> vfcIQ(iNofSamplesOfChannel);
        
            // Schleife ueber alle Kanaele
            for (auto ch=0; ch<m_iNofChannels; ch++ ) 
            {
              // Source-Pointer fuer Kopier-Aktion auf gewuenschten Kanal setzen.
              complex<float> const *pfcSrc = &vfcIqDataFloat32[ch];
        
              // Destination-Pointer fuer Kopier-Aktion
              complex<float> *pfcDst = &vfcIQ[0];
        
              // Kopier-Aktion: Schleife ueber alle Samples in Kanal 'ch' des neuen I/Q Blocks
              for ( int i=0; i<iNofSamplesOfChannel; i++, pfcSrc+=m_iNofChannels, pfcDst++ ) 
              {
                *pfcDst = *pfcSrc;
              }

              // I/Q Daten "block-weise" in die Minimum Spektrum-Preview-Klasse fuettern
              int iPos = 0;
              while ( iPos < iNofSamplesOfChannel )
              {
                int iFeedLength = m_vtSpectrumPreviewMin[ch].GetMaxFeedLength();
                if ( iFeedLength > iNofSamplesOfChannel - iPos )
                {
                  iFeedLength = iNofSamplesOfChannel - iPos;
                }

                m_vtSpectrumPreviewMin[ch].Feed(&vfcIQ[iPos],iFeedLength);
                iPos+=iFeedLength;
              }
        
              // I/Q Daten "block-weise" in die Maximum Spektrum-Preview-Klasse fuettern
              iPos = 0;
              while ( iPos < iNofSamplesOfChannel )
              {
                int iFeedLength = m_vtSpectrumPreviewMax[ch].GetMaxFeedLength();
                if ( iFeedLength > iNofSamplesOfChannel - iPos )
                {
                  iFeedLength = iNofSamplesOfChannel - iPos;
                }

                m_vtSpectrumPreviewMax[ch].Feed(&vfcIQ[iPos],iFeedLength);
                iPos+=iFeedLength;
              }
            } 
        
            // Update sample counters
            m_uiTotalNofSamplesInSpectrumPreview += static_cast<unsigned long long>(vfcIqDataFloat32.size() / m_iNofChannels);
          } 
        
          //=============================================================================
          // Calculate I/Q previews
          //=============================================================================

          // Add new block of I/Q data to previews
          m_tIqPreview.Feed(vfcIqDataFloat32);

          // Update sample counters
          m_uiTotalNofSamplesInIqPreview += static_cast<unsigned long long>(vfcIqDataFloat32.size() / m_iNofChannels);
        }
        catch (DaiException)
        {
          throw;
        }
        catch (exception e)
        {
          throw DaiException(ErrorCodes::InternalError, e.what());
        }
      }

      void IqTarPreview::getPreviews(std::vector<SChannelPreview>& previews)
      {
        previews.resize(m_iNofChannels);

        try
        {
          //-----------------------------------------------------------------
          // Copy PvT previews to output structure
          //-----------------------------------------------------------------

          // Copy Power vs Time preview traces of all channels to output structure
          for (auto ch=0; ch<m_iNofChannels; ch++ ) 
          {
            // Minimum preview trace in V^2 ------------------------------
            previews[ch].tPowerVsTime.vfMin = m_tPvtPreviewMin.GetPreviewTrace(ch);

            // Maximum preview trace in V^2 ------------------------------
            previews[ch].tPowerVsTime.vfMax = m_tPvtPreviewMax.GetPreviewTrace(ch);
          } 

          //-----------------------------------------------------------------
          // Copy Spectrum preview to output structure
          //-----------------------------------------------------------------

          for (auto ch=0; ch<m_iNofChannels; ch++ ) 
          {
            // Copy minimum spectrum in V^2 of channel 'ch'
            int iN = m_vtSpectrumPreviewMin[ch].GetFFTLength();
            if (iN > 0) 
            {
              previews[ch].tSpectrum.vfMin.resize(iN);
              m_vtSpectrumPreviewMin[ch].GetSpectrum( &previews[ch].tSpectrum.vfMin[0] );
            }

            // Copy maximum spectrum in V^2 of channel 'ch'
            iN = m_vtSpectrumPreviewMax[ch].GetFFTLength();
            if (iN > 0) 
            {
              previews[ch].tSpectrum.vfMax.resize(iN);
              m_vtSpectrumPreviewMax[ch].GetSpectrum( &previews[ch].tSpectrum.vfMax[0] );
            }
          }

          //-----------------------------------------------------------------
          // Copy I/Q previews to output structure
          //-----------------------------------------------------------------

          // Width and height = 2*NofPositiveBins (identical for all channels)
          const int iHistoWidth = m_tIqPreview.GetNofPositiveBins() + m_tIqPreview.GetNofPositiveBins();

          // Copy I/Q preview histograms of all channels to output structure
          for (auto ch=0; ch<m_iNofChannels; ch++ ) 
          {
            // Kopiere Histogramm von Kanal 'ch'
            previews[ch].tIQ.uiHeight = iHistoWidth;
            previews[ch].tIQ.uiWidth  = iHistoWidth;
            previews[ch].tIQ.sHistogram = m_tIqPreview.GetPreviewHistogramAsString(ch);
          } 
        }
        catch (DaiException)
        {
          throw;
        }
        catch (exception e)
        {
          throw DaiException(ErrorCodes::InternalError, e.what());
        }
      }
    }
  }
}

#ifdef _MSC_VER
#pragma warning (pop)
#elif __GNUC__
#pragma GCC diagnostic pop
#endif

/*** @endcond ***/