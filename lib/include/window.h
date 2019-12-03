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

/*******************************************************************************/
/**
@file
@copyright     (c) Rohde & Schwarz GmbH & Co. KG, Munich
@version       $Workfile: window.h $
*
*
@language      ANSI C++
*
@description   Calculates different types of windows and applies them to input signal
*
@see
*
@history
*
*******************************************************************************/

#pragma once

#ifdef _MSC_VER
__pragma(warning(push))
__pragma(warning(disable: 4244))
__pragma(warning(disable: 4996))
__pragma(warning(disable: 4018))
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic warning "-w"
#endif

#define _USE_MATH_DEFINES

#include <complex>
#include <cmath>
#include <math.h>

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      enum t_eWINDOW_TYPE
      {
        eWINDOW_TYPE_FLATTOP,          // --> 1ES1 Special Flattop
        eWINDOW_NUMBEROFWINDOWS
      };

      enum t_eWINDOW_NORMALIZATION
      {
        eWINDOW_NORMALIZATION_NONE,  // no normalization
        eWINDOW_NORMALIZATION_RMS   // normalize to window RMS
      };

      /**
      Calculates different types of windows and applies them to an input signal.
      The window can optionally be normalized such that the energy of the windowed
      signal equals the energy of a rectangular windowed signal.
      *
      @version       $Workfile: window.h $.
      *
      *******************************************************************************/
      template <typename T_PRECISION> class CWindow
      {
      public:

        /// Default constructor.
        CWindow();

        /// Destructor
        ~CWindow();

        /* METHOD *********************************************************************/
        /**
        Configure the Window to a specific type and length.
        *
        @param eWindowType: Type of window
        @param iWindowLength: Length of window
        @param bNormalize: Flag if window shall be normalized
        *
        *******************************************************************************/
        void Configure(const t_eWINDOW_TYPE eWindowType, const int iWindowLength,
          const bool bNormalize, const double fWinParam = 0);

        /* METHOD *********************************************************************/
        /**
        Copy window taps to a given memory array.
        *
        @param pfWindowTaps: Pointer to memory where window will be copied to
        *
        *******************************************************************************/
        void CopyWindowTaps(T_PRECISION* pfWindowTaps);

        /* METHOD *********************************************************************/
        /**
        Inplace application of window on a complex data vector
        *
        @param pfcSignal: Source data vector (complex)
        *
        *******************************************************************************/
        void ApplyWindow(std::complex<T_PRECISION>* pfcSignal);

        /* METHOD *********************************************************************/
        /**
        Get window length.
        *
        @param return: window length
        *
        *******************************************************************************/
        inline const int GetWindowLength() const
        {
          return m_iWindowLength;
        }

      protected:

        /// The window coefficients
        std::vector<T_PRECISION> m_afWindowCoefs;

        /// The complex window coefficients
        std::vector<std::complex<T_PRECISION>> m_afcWindowCoefs;

        /// Type of window
        t_eWINDOW_TYPE m_eWindowType;

        /// Length of window
        int m_iWindowLength;

        /// Normalization
        t_eWINDOW_NORMALIZATION m_eNormalization;

        /// optional window parameter
        double m_fWinParam;

        /* METHOD *********************************************************************/
        /**
        Set the window taps vector according to the current window parameters
        *
        *******************************************************************************/
        void SetTaps();

        /* METHOD *********************************************************************/
        /**
        Calculate coefficients of a flattop window
        *
        *******************************************************************************/
        void CalcFlatTopWin();

        /* METHOD *********************************************************************/
        /**
        Calculate a cosine sum window with given coefficients
        *
        @param pfa: Cosine coefficients
        @param iOrder: Window order
        *
        *******************************************************************************/
        void CalcCosSumWin(const T_PRECISION* pfa, const int iOrder);

      private:

        /// ACOSH for complex arguments
        inline std::complex<double> acosh(const std::complex<double>& z) const;
        /// ACOSH for real and positive arguments
        inline double acosh(double zpos) const;
      };

      /* INLINE METHODS *************************************************************/

      /* METHOD *********************************************************************/
      /**
      Acosh for complex numbers.
      *
      @param z: Complex input
      @return Complex output
      *
      *******************************************************************************/
      //template <typename T_CPLX> void CCplxMixer<T_CPLX>::Mix(
      template <typename T_PRECISION>  inline std::complex<double> CWindow<T_PRECISION>::acosh(const std::complex<double>& z) const
      {
        return( log( z + sqrt(z-1.0) * sqrt(z+1.0) ) );
      }

      /* METHOD *********************************************************************/
      /**
      Acosh for real and positive numbers.
      *
      @param zpos: Real and positive input
      @return Real output
      *
      *******************************************************************************/
      template <typename T_PRECISION> inline double CWindow<T_PRECISION>::acosh(double zpos) const
      {
        return( log( zpos + sqrt(zpos-1.0) * sqrt(zpos+1.0) ) );
      }

      template <typename T_PRECISION> CWindow<T_PRECISION>::CWindow() :
      m_eWindowType(eWINDOW_TYPE_FLATTOP),
        m_iWindowLength(0),
        m_eNormalization(eWINDOW_NORMALIZATION_NONE),
        m_fWinParam(0)
      {  }

      template <typename T_PRECISION> CWindow<T_PRECISION>::~CWindow()
      {
      }

      /*******************************************************************************/

      template <typename T_PRECISION> void CWindow<T_PRECISION>::
        CopyWindowTaps(T_PRECISION* pfWindowTaps)
      {
        if (m_iWindowLength > 0) {
          std::copy(pfWindowTaps, pfWindowTaps + m_iWindowLength, m_afWindowCoefs.begin());
        }
      }

      /*******************************************************************************/

      template <typename T_PRECISION> void CWindow<T_PRECISION>::Configure(const t_eWINDOW_TYPE eWindowType,const int iWindowLength,const bool bNormalize, const double fWinParam)
      {
        m_eWindowType = eWindowType;
        m_iWindowLength = iWindowLength;
        m_eNormalization = bNormalize ? eWINDOW_NORMALIZATION_RMS : eWINDOW_NORMALIZATION_NONE;
        m_fWinParam = fWinParam;
        SetTaps();
      }

      /*******************************************************************************/

      template <typename T_PRECISION>
      void CWindow<T_PRECISION>::
        ApplyWindow(std::complex<T_PRECISION>* pfcSignal)
      {
        if (m_iWindowLength > 0) {
          std::transform(m_afcWindowCoefs.begin(), m_afcWindowCoefs.end(), pfcSignal, pfcSignal, std::multiplies<std::complex<T_PRECISION>>());
        }
      }

      /*******************************************************************************/

      template <typename T_PRECISION> void CWindow<T_PRECISION>::SetTaps()
      {
        m_afWindowCoefs.resize(m_iWindowLength);

        switch (m_eWindowType)
        {
        case eWINDOW_TYPE_FLATTOP :
          CalcFlatTopWin();
          break;
        default:
          throw std::out_of_range("CWindow::SetTaps() INVALID WINDOW_TYPE\n");
          break;
        }

        m_afcWindowCoefs.resize(m_iWindowLength);

        if ( m_iWindowLength > 0 )
        {
          // Normalize
          switch ( m_eNormalization )
          {
          case eWINDOW_NORMALIZATION_RMS:
            {
              T_PRECISION fNorm = 0;
              T_PRECISION sqrsum = std::accumulate(m_afWindowCoefs.begin(), m_afWindowCoefs.end(), 0.0, [](T_PRECISION x, T_PRECISION y) { return x + y*y; });
              fNorm = std::sqrt(sqrsum);
              fNorm /= std::sqrt(static_cast<T_PRECISION>(m_iWindowLength));

              if (fNorm > std::numeric_limits<T_PRECISION>::epsilon())
              {
                auto factor = 1.0 / fNorm;
                std::transform(m_afWindowCoefs.begin(), m_afWindowCoefs.end(), m_afWindowCoefs.begin(), std::bind1st(std::multiplies<T_PRECISION>(), factor));
              }

              break;
            }
          case eWINDOW_NORMALIZATION_NONE:
            // Nothing to do
            break;
          default:
            throw std::out_of_range("CWindow::SetTaps() INVALID NORMALIZATION METHOD\n");
            break;
          }

          //Calculate complex window
          std::transform(m_afWindowCoefs.begin(), m_afWindowCoefs.end(), m_afcWindowCoefs.begin(), [](T_PRECISION re) { return std::complex<T_PRECISION>(re, 0.0); });
        }
      }

      /*******************************************************************************/

      template <typename T_PRECISION> void CWindow<T_PRECISION>::CalcCosSumWin(const T_PRECISION* pfa, const int iOrder)
      {
        if (m_iWindowLength == 0) return;

        std::vector<T_PRECISION> afTone;
        afTone.resize(m_iWindowLength);

        // w(t) = a0
        std::fill(m_afWindowCoefs.begin(), m_afWindowCoefs.end(), pfa[0]);

        T_PRECISION fBaseFreq = static_cast<T_PRECISION>(1.0)/static_cast<T_PRECISION>(m_iWindowLength);
        T_PRECISION fPhase = 0;
        T_PRECISION fMag = 0;

        for (int k = 1; k <= iOrder; ++k)
        {
          if (pfa[k] < 0)
          {
            fMag = -pfa[k];
            fPhase = static_cast<T_PRECISION>(M_PI);
          }
          else
          {
            fMag = pfa[k];
            fPhase = static_cast<T_PRECISION>(0);
          }

          for (auto x = 0; x < afTone.size(); ++x)
          {
            afTone[x] = fMag * std::cos(static_cast<T_PRECISION>(x) * 2.0 * M_PI * static_cast<T_PRECISION>(k) * fBaseFreq + fPhase);
          }

          std::transform(afTone.begin(), afTone.end(), m_afWindowCoefs.begin(), m_afWindowCoefs.begin(), std::plus<T_PRECISION>());
        }
      }

      /*******************************************************************************/

      template <typename T_PRECISION> void CWindow<T_PRECISION>::CalcFlatTopWin()
      {
        if (m_iWindowLength == 0) return;

        // Coefficients for Flattop Window (R&S style)
        //  This window has a long tradition in R&S spectrum analyzer, e.g.
        //  See "Flat top window (R&S style)" 
        //
        //  The correct term is "Minimum sidelobe 5-term flat-top" which can be found here:
        //   Prof. Luigi Salvatore and Dr. Amerigo Trotta
        //   Flat-top windows for PWM waveform processing via DFT
        //   Electric Power Applications, IEE Proceedings B  (Vol. 135, Issue 6), Nov. 1988
        //   Page 348, "Minimum sidelobe 5-term flat-top"
        //
        //  There is also a valueable discussion here:
        //   G. Heinzel, A. Ruediger and R. Schilling
        //   Spectrum and spectral density estimation by the Discrete Fourier transform (DFT),
        //   including a comprehensive list of window functions and some new at-top windows.
        //   http://www.rssd.esa.int/SP/LISAPATHFINDER/docs/Data_Analysis/GH_FFT.pdf
        //   Page 40, Table 5 "SFT5M", Feb. 2002
        //
        // Note: R&S uses 0.009104 instead of 0.0091036 (see literature) for the
        //       last coefficient as in the literture. This is corrected here.
        int iOrder = 4;
        T_PRECISION afa[] = {
          static_cast<T_PRECISION>( 0.209671),
          static_cast<T_PRECISION>(-0.407331),
          static_cast<T_PRECISION>( 0.281225),
          static_cast<T_PRECISION>(-0.092669),
          static_cast<T_PRECISION>( 0.0091036)
        };
        CalcCosSumWin(&afa[0], iOrder);
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