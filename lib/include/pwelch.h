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
@version       $Workfile: pwelch.h $
*
*
@language      ANSI C++
*
@description   Uses Welch algorithm to estimate spectrum of given signal
*
@see
*
@history
*
*******************************************************************************/

#pragma once

#ifdef _MSC_VER
//#pragma warning (push, 0)
__pragma(warning(push))
__pragma(warning(disable: 4244))
__pragma(warning(disable: 4996))
__pragma(warning(disable: 4018))
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreorder"
#endif

#include <complex>
#include <functional>

#include "window.h"
#include "ringbuffer.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /* LOCAL TYPES DECLARATION ****************************************************/
      //Different tail handling methods
      enum t_eTAIL_HANDLING
      {
        eTAIL_HANDLING_DELETE,  // Discard tail
        eTAIL_HANDLING_COMPUTE, // Compute tail with smaller shift
      };
      //Different spectrum averaging methods
      enum t_eAVERAGING_METHOD
      {
        eAVERAGING_METHOD_ACC,    //Accumulate and divide by number of additions
        eAVERAGING_METHOD_IIR,    //Weight already accumulated spectrum by a factor and add new spectrum with a weight (1-factor)
        eAVERAGING_METHOD_ACCIIR, //For the first NumWaitFFT additions, it's like eAVERAGING_METHOD_ACC and afterwards like eAVERAGING_METHOD_IIR
        eAVERAGING_METHOD_MAX,    // The maximum value is retained with each new spectrum
        eAVERAGING_METHOD_MIN,    // The minimum value is retained with each new spectrum
      };
      //Different spectrum domains used for averaging and final result calculation
      enum t_eSPECTRUM_DOMAIN
      {
        eSPECTRUM_DOMAIN_MAGNITUDE, // Use of magnitude spectra.
        eSPECTRUM_DOMAIN_POWER,     // [DEFAULT]  Use of power spectra.
      };
      //Different window normalisations
      enum t_eWINDOW_NORM
      {
        eWINDOW_NORM_LEVEL_TRUE, // Normalisation such that spectral peaks corresponds to the correct level (i.e. the window has unit magnitude at zero frequency). Useful for harmonic signal analysis.
        eWINDOW_NORM_POWER_TRUE, // [DEFAULT] Normalisation such that the mean over frequency corresponds to the signal power (i.e. the window has unit energy).  Useful for noise/wideband analysis.
      };

      template<typename T>
      inline T MagSqr(const std::complex<T>& c)
      {
        return static_cast<T>(c.real()*c.real() + c.imag()*c.imag());
      }

      template<typename T>
      inline T Mag(const std::complex<T>& c)
      {
        return std::sqrt(MagSqr(c));
      }

      /* FORWARD DECLARATIONS *******************************************************/

      /* CONSTANT DECLARATIONS ******************************************************/

      /* CLASS DECLARATIONS *********************************************************/

      /* CLASS DECLARATION **********************************************************/
      /**
      Uses Welch algorithm to estimate spectrum of given signal
      *
      @version       $Workfile: pwelch.h $.
      *
      *******************************************************************************/
      template <typename T> class CPWelch
      {
      public:

        /// Default constructor.
        CPWelch();
        /// parameterized constructor - See Init
        /*CPWelch(const int a_iWindowLength, const double a_fOverlapRatio, const int a_iNfft);*/

        /// Destructor
        ~CPWelch();

        /* METHOD *********************************************************************/
        /**
        Initializes the class
        *
        @param a_iWindowLength: Window Length
        @param a_fOverlapRatio: Overlap ratio of sliding window, value between 0 and 1
        @param a_iNfft:         Length of output spectrum
        *
        *******************************************************************************/
        void Init(const int a_iWindowLength, const double a_fOverlapRatio, const int a_iNfft);

        /* METHOD *********************************************************************/
        /**
        Passes data to be computed
        *
        @param a_pFeedData:       Pointer to data to be feed
        @param a_iLengthFeedData: Length of data to be feed
        *
        *******************************************************************************/
        void Feed(const std::complex<T>* a_pFeedData,const int a_iLengthFeedData);

        /* METHOD *********************************************************************/
        /**
        Returns calculated spectrum
        *
        @param afAccSpectrumNormalized:   Pointer to already allocated memory where spectrum will be stored
        *
        *******************************************************************************/
        void GetSpectrum(T* afAccSpectrumNormalized);

        /* METHOD *********************************************************************/
        /**
        Treates the remaining samples in input buffer according to m_eTailHandling and clears buffer
        *
        *******************************************************************************/
        void Flush();

        /* METHOD *********************************************************************/
        /**
        Clears input buffer and resets calculated spectrum
        *
        *******************************************************************************/
        void Reset();

        /* METHOD *********************************************************************/
        /**
        Returns maximum length of data that can be feed at once
        *
        @return maximum length of data that can be feed at once
        *
        *******************************************************************************/
        int GetMaxFeedLength(){return cRingBuffer.GetMaxFeedLength();};

        /* METHOD *********************************************************************/
        /**
        Returns window length that has been calculated using the given normalized
        RBW and the RBW of the selected window
        *
        @return window length
        *
        *******************************************************************************/
        int GetWindowLength(){return m_iWindowLength;};

        /* METHOD *********************************************************************/
        /**
        Returns fft length
        *
        @return fft length
        *
        *******************************************************************************/
        int GetFFTLength(){return m_iNfft;};

        /* METHOD *********************************************************************/
        /**
        Sets the spectrum averaging methods
        *
        @param a_eAVERAGING_METHOD    name of the method
        @param a_fAveragingFactor     Averaging factor for eAVERAGING_METHOD_IIR and eAVERAGING_METHOD_ACCIIR
        @param a_iAveragingNumWaitFFT Wait NumWaitFFT additions until switching from ACC to IIR, only eAVERAGING_METHOD_ACCIIR
        *
        *******************************************************************************/
        void SetAveragingMethod(const t_eAVERAGING_METHOD a_eAVERAGING_METHOD);
        void SetAveragingMethod(const t_eAVERAGING_METHOD a_eAVERAGING_METHOD,const double a_fAveragingFactor);
        void SetAveragingMethod(const t_eAVERAGING_METHOD a_eAVERAGING_METHOD,const double a_fAveragingFactor,const int a_iAveragingNumWaitFFT);

        /* METHOD *********************************************************************/
        /**
        Sets the spectrum result domain
        *
        @param a_eRESULT_DOMAIN       type of the result domain
        *
        *******************************************************************************/
        void SetResultDomain(const t_eSPECTRUM_DOMAIN a_eRESULT_DOMAIN);

        /* METHOD *********************************************************************/
        /**
        Sets the spectrum averaging domain
        *
        @param a_eAVERAGING_DOMAIN    type of the averaging domain
        *
        *******************************************************************************/
        void SetAveragingDomain(const t_eSPECTRUM_DOMAIN a_eAVERAGING_DOMAIN);

        /* METHOD *********************************************************************/
        /**
        Sets the window normalisation
        *
        @param a_eWINDOW_NORM    type of the window normalisation
        *
        *******************************************************************************/
        void SetWindowNormalisation(const t_eWINDOW_NORM a_eWINDOW_NORM);

        /* METHOD *********************************************************************/
        /**
        Get tail handling method
        *
        @return tail handling method
        *
        *******************************************************************************/
        t_eTAIL_HANDLING GetTailHandling(){return m_eTailHandling;};

        /* METHOD *********************************************************************/
        /**
        Set tail handling method
        *
        @param a_eTailHandling: Tail handling method
        *
        *******************************************************************************/
        void SetTailHandling(const t_eTAIL_HANDLING a_eTailHandling){m_eTailHandling=a_eTailHandling;};

        /* METHOD *********************************************************************/
        /**
        Set the ring buffer size
        *
        @param a_iRingBufferSize ring buffer size
        *
        *******************************************************************************/
        void SetRingBufferSize(const int a_iRingBufferSize);

        /* METHOD *********************************************************************/
        /**
        Get the ring buffer size
        *
        @return ring buffer size
        *
        *******************************************************************************/
        int GetRingBufferSize();

        /* METHOD *********************************************************************/
        /**
        Set the window type
        *
        @param a_eWindowType: Window type
        @param a_fWindowParam: Optional window parameter
        *
        *******************************************************************************/
        void SetWindowType(const t_eWINDOW_TYPE a_eWindowType, const double a_fWindowParam = 0);

      protected:

        ///Accumulated spectrum
        std::vector<T> m_afAccSpectrum;

        ///Window
        CWindow<T> m_cWindow;

        ///Input ring buffer
        CRingBuffer<std::complex<T>> cRingBuffer;

        ///Delete Tail or compute tail with smaller shift
        t_eTAIL_HANDLING m_eTailHandling;

        ///FFT-Size, size of output vector
        int m_iNfft;

        ///Calculated window length
        int m_iWindowLength;

        ///Shift for sliding window
        int m_iWindowShift;

        ///Size of Ringbuffer
        int m_iRingBufferSize;

        ///Number of accumulated FFTs
        int m_iNumAccFFT;

        ///Spectrum averaging method
        t_eAVERAGING_METHOD m_eAVERAGING_METHOD;

        ///Spectrum averaging domain
        t_eSPECTRUM_DOMAIN m_eAVERAGING_DOMAIN;

        ///Spectrum result domain
        t_eSPECTRUM_DOMAIN m_eRESULT_DOMAIN;

        ///Window normalization
        t_eWINDOW_NORM m_eWINDOW_NORM;

        ///Depending on spectrum averaging method - weight factor for IIR and ACCIIR
        double m_fAveragingFactor;

        ///ACCIIR-spectrum averaging method - when switch between ACC and IIR
        int m_iAveragingNumWaitFFT;

        /* METHOD *********************************************************************/
        /**
        Processes windows and padded segment stored in m_afSegmentWindowLength
        *
        *******************************************************************************/
        void ProcessSegment();

        /* METHOD *********************************************************************/
        /**
        Calculate FFT for complex input signal
        @param afcSignal  Pointer to complex input signal
        @param afcSpectrum  Pointer to allocated memory where spectrum will be stored
        *
        *******************************************************************************/
        void DoFFTwithComplexSignal(const std::complex<T>* afcSignal, std::complex<T>* afcSpectrum);

      private:

        ///Temporary vectors
        std::vector<std::complex<T>> m_afSegmentWindowLength;
        std::vector<std::complex<T>> m_afSegmentFFTLength;
        std::vector<std::complex<T>> m_afcTempSpectrumFFTLength;
        std::vector<T> m_afTempAccSpectrum;
      };

      /* INLINE METHODS *************************************************************/

      /* TEMPLATE METHODS ***********************************************************/
      /* PUBLIC *********************************************************************/

      /* PUBLIC *********************************************************************/
      /// Default constructor.
      template <typename T> CPWelch<T>::
        CPWelch():m_eTailHandling(eTAIL_HANDLING_DELETE),m_iNfft(0),m_iNumAccFFT(0),
        m_iWindowLength(0),m_iWindowShift(0),m_iRingBufferSize(10000)
      {
      }

      /// Destructor
      template <typename T> CPWelch<T>::~CPWelch()
      {
      }

      /********************************************************************************/

      template <typename T> void CPWelch<T>::
        Init(const int a_iWindowLength, const double a_fOverlapRatio, const int a_iNfft)
      {
        if (a_fOverlapRatio<0 || a_fOverlapRatio>=1)
          throw std::out_of_range("CPWelch::Init() Overlap out of range, must be 0<Overlap<=1\n");

        if (a_iNfft < a_iWindowLength)
          throw std::out_of_range("CPWelch::Init() Window length exceeds FFT length\n");

        m_iNfft = a_iNfft;

        m_iWindowLength = a_iWindowLength;
        m_iWindowShift=static_cast<int>(ceil(m_iWindowLength*(1-a_fOverlapRatio)));

        cRingBuffer.SetRingBufferSize(m_iRingBufferSize);

        cRingBuffer.SetPastSize(m_iWindowShift-1);

        m_afAccSpectrum.resize(m_iNfft);
        m_afSegmentWindowLength.resize(m_iWindowLength);
        m_afSegmentFFTLength.resize(m_iNfft);
        m_afcTempSpectrumFFTLength.resize(m_iNfft);
        m_afTempAccSpectrum.resize(m_iNfft);

        SetAveragingMethod(eAVERAGING_METHOD_ACC);
        SetAveragingDomain(eSPECTRUM_DOMAIN_POWER);
        SetResultDomain(eSPECTRUM_DOMAIN_POWER);
        SetWindowNormalisation(eWINDOW_NORM_POWER_TRUE);

        Reset();
      }

      /* METHOD *********************************************************************/
      /**
      Passes data to be computed
      *
      @param a_pFeedData:       Pointer to data to be feed
      @param a_iLengthFeedData: Length of data to be feed
      *
      *******************************************************************************/
      template <typename T> void CPWelch<T>::
        Feed(const std::complex<T>* a_pFeedData,const int a_iLengthFeedData)
      {
        cRingBuffer.Feed(a_pFeedData,a_iLengthFeedData);
        while (cRingBuffer.GetMaxReadLength()>=m_iWindowLength)
        {
          //Get Signal and shift by iWindowShift
          cRingBuffer.Read(&m_afSegmentWindowLength[0],m_iWindowLength,0,m_iWindowShift);
          ProcessSegment();
        }
      }

      /* METHOD *********************************************************************/
      /**
      Returns calculated spectrum
      *
      @param afAccSpectrumNormalized:   Pointer to already allocated memory where spectrum will be stored
      *
      *******************************************************************************/
      template <typename T> void CPWelch<T>::
        GetSpectrum(T* afAccSpectrumNormalized)
      {
        // Vector container for easy manipulation of the externally allocated memory

        // "Shift" the spectrum such that the zero frequency occurs in the middle of the output vector.
        if (m_iNumAccFFT>0)
        {
          //FFTShift
          //512 lang
          //M vfSpectrumCPP2=[vfSpectrumCPP(257:512);vfSpectrumCPP(1:256)]; in C überall einen abziehen

          //C vfSpectrumCPP2=[vfSpectrumCPP(256:511);vfSpectrumCPP(0:255)]; in C überall einen abziehen
          //                                   256                  256
          //63 lang
          //M vfSpectrumCPP2=[vfSpectrumCPP(33:63);vfSpectrumCPP(1:32)];

          //C vfSpectrumCPP2=[vfSpectrumCPP(32:62);vfSpectrumCPP(0:31)];
          //                                 31                   32
          int iMiddle2,iMiddle1;
          iMiddle2=static_cast<int>(ceil(static_cast<double>(m_iNfft)/2.0)); //256 oder 32
          iMiddle1=static_cast<int>(floor(static_cast<double>(m_iNfft)/2.0)); //256 oder 31

          std::copy(m_afAccSpectrum.begin() + iMiddle2, m_afAccSpectrum.begin() + iMiddle2 + iMiddle1, afAccSpectrumNormalized);
          std::copy(m_afAccSpectrum.begin(), m_afAccSpectrum.begin() + iMiddle2, afAccSpectrumNormalized + iMiddle1);
        }

        // Perform the window normalisation.
        T fNormFactor = 1;
        if ( (m_eWINDOW_NORM == eWINDOW_NORM_LEVEL_TRUE) && (m_iWindowLength > 0) )
        {
          // The "level true" normalisation requires the Fourier transform of the window function
          // to be unity at the zero frequency.  Equivalently, the sum of the window coefficients must
          // be unity.  As the window coefficients are in a protected member of the window object,
          // and there is no "scale coefficients" function available, the coefficients are first
          // copied to compute the sum, then the normalisation is applied directly to the computed spectrum.
          //
          // Note: It might be worth adding a "scale coefficients" function to the window class...
          // LC
          std::vector<T> vfWindowSamples( m_iWindowLength );
          if (m_iWindowLength > 0) {
            m_cWindow.CopyWindowTaps( &vfWindowSamples[0] );
          }
          if ( m_eAVERAGING_DOMAIN == eSPECTRUM_DOMAIN_POWER )
          {
            fNormFactor = std::accumulate(vfWindowSamples.begin(), vfWindowSamples.end(), 0.0);
            fNormFactor *= fNormFactor;
          }
          else if ( m_eAVERAGING_DOMAIN == eSPECTRUM_DOMAIN_MAGNITUDE )
          {
            fNormFactor = std::accumulate(vfWindowSamples.begin(), vfWindowSamples.end(), 0.0);
          }
        }
        else if (m_eWINDOW_NORM == eWINDOW_NORM_POWER_TRUE)
        {
          // The L2 normalisation done in the window class requires further normalisation by the window length
          // such that the average power is correct in the computed spectrum (useful, e.g. for noise or
          // wideband signal analysis).
          if ( m_eAVERAGING_DOMAIN == eSPECTRUM_DOMAIN_MAGNITUDE )
          {
            fNormFactor = sqrt( static_cast<T>(m_iWindowLength) );
          }
          else
          {
            fNormFactor = static_cast<T>(m_iWindowLength);
          }
        }

        // For the accumulate methods, the spectrum should also be normalised by the total number of accumulations
        if ((m_eAVERAGING_METHOD == eAVERAGING_METHOD_ACC) || (m_eAVERAGING_METHOD == eAVERAGING_METHOD_ACCIIR))
        {
          fNormFactor *= static_cast<T>(m_iNumAccFFT);
        }

        // Apply the normalisation
        //if (fNormFactor > std::numeric_limits<T>::epsilon())
        if (fNormFactor != 1)
        {
          //vfAccSpectrumNormalized /= fNormFactor;
          auto factor = 1.0 / fNormFactor;
          std::transform(afAccSpectrumNormalized, afAccSpectrumNormalized + m_iNfft, afAccSpectrumNormalized, std::bind1st(std::multiplies<T>(), factor));
          
        }

        // Convert the result to the required domain if neccessary
        if ( m_eAVERAGING_DOMAIN != m_eRESULT_DOMAIN )
        {
          if ( m_eRESULT_DOMAIN == eSPECTRUM_DOMAIN_MAGNITUDE )
          {
            // Square-root required
            std::transform(afAccSpectrumNormalized, afAccSpectrumNormalized + m_iNfft, afAccSpectrumNormalized, [](T val) { return std::sqrt(val); });
          }
          else if ( m_eRESULT_DOMAIN == eSPECTRUM_DOMAIN_POWER )
          {
            // Square required
            std::transform(afAccSpectrumNormalized, afAccSpectrumNormalized + m_iNfft, afAccSpectrumNormalized, [](T val) { return val*val; });
          }
        }
      }

      /* METHOD *********************************************************************/
      /**
      Treates the remaining samples in input buffer according to m_eTailHandling and clears buffer
      *
      *******************************************************************************/
      template <typename T> void CPWelch<T>::
        Flush()
      {
        if (m_eTailHandling==eTAIL_HANDLING_DELETE)
        {
          //delete tail without processing
          cRingBuffer.Clear();
        }
        else if (m_eTailHandling==eTAIL_HANDLING_COMPUTE)
        {
          int iMaxReadLength;
          iMaxReadLength=cRingBuffer.GetMaxReadLength();
          if (m_iWindowLength-iMaxReadLength!=m_iWindowShift)
          {
            //Read remaining tail with necessary samples from the past
            cRingBuffer.Read(&m_afSegmentWindowLength[0],m_iWindowLength,iMaxReadLength-m_iWindowLength,0);

            ProcessSegment();
          }
          cRingBuffer.Clear();
        }
      }

      /* METHOD *********************************************************************/
      /**
      Clears input buffer and resets calculated spectrum
      *
      *******************************************************************************/
      template <typename T> void CPWelch<T>::
        Reset()
      {
        cRingBuffer.Clear();

        if (m_iNfft > 0) {
          std::fill(m_afAccSpectrum.begin(), m_afAccSpectrum.end(), 0.0);
          std::fill(m_afSegmentFFTLength.begin(), m_afSegmentFFTLength.end(), 0.0);
          std::fill(m_afcTempSpectrumFFTLength.begin(), m_afcTempSpectrumFFTLength.end(), 0.0);
          std::fill(m_afTempAccSpectrum.begin(), m_afTempAccSpectrum.end(), 0.0);
        }
        if (m_iWindowLength > 0) {
          std::fill(m_afSegmentWindowLength.begin(), m_afSegmentWindowLength.end(), 0.0);
        }

        m_iNumAccFFT=0;
      }

      /* METHOD *********************************************************************/
      /**
      Sets the spectrum averaging methods
      *
      @param a_eAVERAGING_METHOD    name of the method
      @param a_fAveragingFactor     Averaging factor for eAVERAGING_METHOD_IIR and eAVERAGING_METHOD_ACCIIR
      @param a_iAveragingNumWaitFFT Wait NumWaitFFT additions until switching from ACC to IIR, only eAVERAGING_METHOD_ACCIIR
      *
      *******************************************************************************/
      template <typename T> void CPWelch<T>::
        SetAveragingMethod(const t_eAVERAGING_METHOD a_eAVERAGING_METHOD,const double a_fAveragingFactor,const int a_iAveragingNumWaitFFT)
      {
        m_eAVERAGING_METHOD=a_eAVERAGING_METHOD;
        m_fAveragingFactor=a_fAveragingFactor;
        m_iAveragingNumWaitFFT=a_iAveragingNumWaitFFT;
      }

      template <typename T> void CPWelch<T>::
        SetAveragingMethod(const t_eAVERAGING_METHOD a_eAVERAGING_METHOD)
      {
        if (a_eAVERAGING_METHOD==eAVERAGING_METHOD_ACC)
          SetAveragingMethod(a_eAVERAGING_METHOD,0.0,0);
        else if (a_eAVERAGING_METHOD==eAVERAGING_METHOD_IIR)
          SetAveragingMethod(a_eAVERAGING_METHOD,0.99,0);
        else if (a_eAVERAGING_METHOD==eAVERAGING_METHOD_ACCIIR)
          SetAveragingMethod(a_eAVERAGING_METHOD,0.99,1000);// wie schön globale konstanten bauen?! const int myconstant=1000
        else if (a_eAVERAGING_METHOD==eAVERAGING_METHOD_MAX)
          SetAveragingMethod(a_eAVERAGING_METHOD,0.0,0);
        else if (a_eAVERAGING_METHOD==eAVERAGING_METHOD_MIN)
          SetAveragingMethod(a_eAVERAGING_METHOD,0.0,0);
      }

      template <typename T> void CPWelch<T>::
        SetAveragingMethod(const t_eAVERAGING_METHOD a_eAVERAGING_METHOD,const double a_fAveragingFactor)
      {
        if (a_eAVERAGING_METHOD==eAVERAGING_METHOD_ACC)
          SetAveragingMethod(a_eAVERAGING_METHOD,0.0,0);
        else if (a_eAVERAGING_METHOD==eAVERAGING_METHOD_IIR)
          SetAveragingMethod(a_eAVERAGING_METHOD,a_fAveragingFactor,0);
        else if (a_eAVERAGING_METHOD==eAVERAGING_METHOD_ACCIIR)
          SetAveragingMethod(a_eAVERAGING_METHOD,a_fAveragingFactor,1000);
        else if (a_eAVERAGING_METHOD==eAVERAGING_METHOD_MAX)
          SetAveragingMethod(a_eAVERAGING_METHOD,0.0,0);
        else if (a_eAVERAGING_METHOD==eAVERAGING_METHOD_MIN)
          SetAveragingMethod(a_eAVERAGING_METHOD,0.0,0);
      }

      /* METHOD *********************************************************************/
      /**
      Sets the spectrum result domain
      *
      @param a_eRESULT_DOMAIN       type of the result domain
      *
      *******************************************************************************/
      template <typename T> void CPWelch<T>::
        SetResultDomain(const t_eSPECTRUM_DOMAIN a_eRESULT_DOMAIN)
      {
        m_eRESULT_DOMAIN = a_eRESULT_DOMAIN;
      }

      /* METHOD *********************************************************************/
      /**
      Sets the spectrum averaging domain
      *
      @param a_eAVERAGING_DOMAIN    type of the averaging domain
      *
      *******************************************************************************/
      template <typename T> void CPWelch<T>::
        SetAveragingDomain(const t_eSPECTRUM_DOMAIN a_eAVERAGING_DOMAIN)
      {
        m_eAVERAGING_DOMAIN = a_eAVERAGING_DOMAIN;
      }

      /* METHOD *********************************************************************/
      /**
      Sets the window normalisation
      *
      @param a_eWINDOW_NORM    type of the window normalisation
      *
      *******************************************************************************/
      template <typename T> void CPWelch<T>::
        SetWindowNormalisation(const t_eWINDOW_NORM a_eWINDOW_NORM)
      {
        m_eWINDOW_NORM = a_eWINDOW_NORM;
      }

      /* METHOD *********************************************************************/
      /**
      Sets and gets the ring buffer size
      *
      *******************************************************************************/
      template <typename T> void CPWelch<T>::
        SetRingBufferSize(const int a_iRingBufferSize)
      {
        m_iRingBufferSize=a_iRingBufferSize;
        cRingBuffer.SetRingBufferSize(m_iRingBufferSize);
      }

      template <typename T> int CPWelch<T>::
        GetRingBufferSize()
      {
        return m_iRingBufferSize;
      }

      /* METHOD *********************************************************************/
      /**
      Set window type
      *
      *******************************************************************************/
      template <typename T> void CPWelch<T>::
        SetWindowType(const t_eWINDOW_TYPE a_eWindowType, const double a_fWindowParam)
      {
        switch( m_eWINDOW_NORM )
        {
        case eWINDOW_NORM_LEVEL_TRUE:
          // A "level-true" normalisation requires that the window has unit value at the zero frequency.
          // Since we can't directly scale the window taps here, the correct scaling factor is applied
          // later in GetSpectrum, when the final result is queried.
          m_cWindow.Configure(a_eWindowType,m_iWindowLength,false,a_fWindowParam);
          break;

        case eWINDOW_NORM_POWER_TRUE:
        default:
          // The window class automatically performs a "power-true" normalisation of the
          // window when the normalisation flag is true.
          m_cWindow.Configure(a_eWindowType,m_iWindowLength,true,a_fWindowParam);
        }
      }

      /* PROTECTED ******************************************************************/

      /* METHOD *********************************************************************/
      /**
      Processes windows and padded segment stored in m_afSegmentWindowLength
      *
      *******************************************************************************/
      template <typename T> void CPWelch<T>::
        ProcessSegment()
      {
        //Apply Window
        m_cWindow.ApplyWindow(&m_afSegmentWindowLength[0]);
        //Zero Padding

        std::copy(m_afSegmentWindowLength.begin(), m_afSegmentWindowLength.begin() + m_iWindowLength, m_afSegmentFFTLength.begin());

        //Calculate FFT
        DoFFTwithComplexSignal(&m_afSegmentFFTLength[0],&m_afcTempSpectrumFFTLength[0]);

        //Calculate spectrum in the required domain
        if (m_eAVERAGING_DOMAIN==eSPECTRUM_DOMAIN_POWER)
        {
          std::transform(m_afcTempSpectrumFFTLength.begin(), m_afcTempSpectrumFFTLength.end(), m_afTempAccSpectrum.begin(), [](std::complex<T> val) { return MagSqr(val); });
        }
        else if (m_eAVERAGING_DOMAIN==eSPECTRUM_DOMAIN_MAGNITUDE)
        {
          std::transform(m_afcTempSpectrumFFTLength.begin(), m_afcTempSpectrumFFTLength.end(), m_afTempAccSpectrum.begin(), [](std::complex<T> val) { return Mag(val); });
        }

        //Add to already processed spectrum according to averaging method
        if (m_eAVERAGING_METHOD==eAVERAGING_METHOD_ACC)
        {
          //Just add
          std::transform(m_afTempAccSpectrum.begin(), m_afTempAccSpectrum.end(), m_afAccSpectrum.begin(), m_afAccSpectrum.begin(), std::plus<T>());
          m_iNumAccFFT++;
        }
        else if (m_eAVERAGING_METHOD==eAVERAGING_METHOD_IIR)
        {
          if (m_iNumAccFFT>0)
          {
            //new=old*factor + newcalculated*(1-factor)
            std::transform(m_afAccSpectrum.begin(), m_afAccSpectrum.end(), m_afAccSpectrum.begin(), std::bind1st(std::multiplies<T>(), m_fAveragingFactor));
            std::transform(m_afTempAccSpectrum.begin(), m_afTempAccSpectrum.end(), m_afTempAccSpectrum.begin(), std::bind1st(std::multiplies<T>(), 1.0 - m_fAveragingFactor));
          }

          std::transform(m_afTempAccSpectrum.begin(), m_afTempAccSpectrum.end(), m_afAccSpectrum.begin(), m_afAccSpectrum.begin(), std::plus<T>());
          m_iNumAccFFT++;
        }
        else if (m_eAVERAGING_METHOD==eAVERAGING_METHOD_ACCIIR)
        {
          //ACC-mode for first  m_iAveragingNumWaitFFT
          if (m_iNumAccFFT<=m_iAveragingNumWaitFFT)
          {
            std::transform(m_afTempAccSpectrum.begin(), m_afTempAccSpectrum.end(), m_afAccSpectrum.begin(), m_afAccSpectrum.begin(), std::plus<T>());
            m_iNumAccFFT++;
          }
          //once reached m_iAveragingNumWaitFFT -> normalize
          if (m_iNumAccFFT==m_iAveragingNumWaitFFT)
          {
            if (m_iNumAccFFT > std::numeric_limits<T>::epsilon())
            {
              auto factor = 1.0 / m_iNumAccFFT;
              std::transform(m_afAccSpectrum.begin(), m_afAccSpectrum.end(), m_afAccSpectrum.begin(), std::bind1st(std::multiplies<T>(), factor));
            }
          }
          //afterwards, IIR-Mode
          else if (m_iNumAccFFT>m_iAveragingNumWaitFFT)
          {
            if (m_iNumAccFFT>0)
            {
              //new=old*factor + newcalculated*(1-factor)
              std::transform(m_afAccSpectrum.begin(), m_afAccSpectrum.end(), m_afAccSpectrum.begin(), std::bind1st(std::multiplies<T>(), m_fAveragingFactor));
              std::transform(m_afTempAccSpectrum.begin(), m_afTempAccSpectrum.end(), m_afTempAccSpectrum.begin(), std::bind1st(std::multiplies<T>(), 1.0 - m_fAveragingFactor));
            }

            std::transform(m_afTempAccSpectrum.begin(), m_afTempAccSpectrum.end(), m_afAccSpectrum.begin(), m_afAccSpectrum.begin(), std::plus<T>());
            m_iNumAccFFT++;
          }
        }
        else if (m_eAVERAGING_METHOD==eAVERAGING_METHOD_MAX)
        {
          if (m_iNumAccFFT>0)
          {
            // Retain the maximum of the previous and current spectrum at each bin
            std::transform(m_afTempAccSpectrum.begin(), m_afTempAccSpectrum.end(), m_afAccSpectrum.begin(), m_afAccSpectrum.begin(), [](T val1, T val2) { return std::max(val1, val2); });
          }
          else
          {
            // Initialize the spectrum estimate with the current segment
            m_afAccSpectrum = m_afTempAccSpectrum;
          }
          m_iNumAccFFT++;
        }
        else if (m_eAVERAGING_METHOD==eAVERAGING_METHOD_MIN)
        {
          if (m_iNumAccFFT>0)
          {
            // Retain the minimum of the previous and current spectrum at each bin
            std::transform(m_afTempAccSpectrum.begin(), m_afTempAccSpectrum.end(), m_afAccSpectrum.begin(), m_afAccSpectrum.begin(), [](T val1, T val2) { return std::min(val1, val2); });
          }
          else
          {
            // Initialize the spectrum estimate with the current segment
            m_afAccSpectrum = m_afTempAccSpectrum;
          }
          m_iNumAccFFT++;
        }
      }

      /* METHOD *********************************************************************/
      /**
      Calculate FFT for complex input signal
      @param afcSignal  Pointer to complex input signal
      @param afSignal  Pointer to real input signal
      @param afcSpectrum  Pointer to allocated memory where spectrum will be stored
      *
      *******************************************************************************/
      template <typename T> void CPWelch<T>::
        DoFFTwithComplexSignal(const std::complex<T>* afcSignal, std::complex<T>* afcSpectrum)
      {
        std::copy(afcSignal, afcSignal + m_iNfft, afcSpectrum);

        int nm1 = m_iNfft - 1;
        int nd2 = m_iNfft / 2.0;
        int logLength = static_cast<int>(std::log(m_iNfft) / std::log(2));
        int j = nd2;
        int k = 0;

        // bit reversal sorting
        for (auto idx = 1; idx <= m_iNfft - 2; idx++)
        {
          if (idx < j)
          {
            auto tmp = afcSpectrum[idx];
            afcSpectrum[idx] = afcSpectrum[j];
            afcSpectrum[j] = tmp;
          }

          k = nd2;
          while (k <= j)
          {
            j = static_cast<int>(j - k);
            k = static_cast<int>(k / 2);
          }

          j += k;
        }

        // loop for each stage

        int le2 = 0;
        int jm1 = 0;

        double ur = 0;
        double ui = 0;

        double tr = 0;
        double ti = 0;

        int butterflyStep = 0;

        for (butterflyStep = 1; butterflyStep <= logLength; butterflyStep++)
        {
          int stepping = static_cast<int>(static_cast<int>(1 << static_cast<int>(butterflyStep)));
          le2 = static_cast<int>(stepping >> 1);
          ur = 1;
          ui = 0;

          //Sine & Cosine
          double sr = std::cos(M_PI / le2);
          double si = -std::sin(M_PI / le2);

          //Loop for each sub DFT
          for (j = 1; j <= le2; j++)
          {
            jm1 = static_cast<int>(j - 1);

            //Loop for each butterfly
            for (auto idx = jm1; idx <= nm1; idx += stepping)
            {
              auto ip = idx + le2;
              tr = (afcSpectrum[ip].real() * ur) - (afcSpectrum[ip].imag() * ui);
              ti = (afcSpectrum[ip].real() * ui) + (afcSpectrum[ip].imag() * ur);
              afcSpectrum[ip].real(afcSpectrum[idx].real() - tr);
              afcSpectrum[ip].imag(afcSpectrum[idx].imag() - ti);
              afcSpectrum[idx].real(afcSpectrum[idx].real() + tr);
              afcSpectrum[idx].imag(afcSpectrum[idx].imag() + ti);
            }

            tr = ur;
            ur = tr * sr - ui * si;
            ui = tr * si + ui * sr;
          }
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