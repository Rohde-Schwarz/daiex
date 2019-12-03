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
@file
@copyright     (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich
@version       $Workfile: TracePreview.h $
*
@responsible
*
@language      C++
*
@description   Calculation of a preview with reduced points for y vs x traces.
*              The trace data comes in blocks of data.
*
@see           TracePreview.pdf
*
*******************************************************************************/

#ifdef _MSC_VER
__pragma(warning(push))
__pragma(warning(disable: 4244))
__pragma(warning(disable: 4996))
__pragma(warning(disable: 4018))
__pragma(warning(disable: 4389))
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic warning "-w"
#endif

#include <vector>
#include <limits>
#include <numeric>

#include "daiexception.h"
#include "errorcodes.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /** Class for the maximum trace detector.
      *
      * Provides the functionality specific to the maximum detector.
      * CTP = Class Trace Preview
      *
      @version
      *
      @pattern
      *******************************************************************************/
      class CTPDetectorMaximum
      {
        /** Maximum detetor. Retruns the maximum of A and B. */
      public: inline static float Detect( const float a_fA, const float a_fB )
              {
                return ((a_fA > a_fB) ? a_fA : a_fB);
              }

              /** Returns the init value of the detected value for the maximum detector. */
      public: inline static float Default()
              {
                return -std::numeric_limits<float>::infinity();
              }
      }; // class CTPDetectorMaximum

      /* CLASS DECLARATION **********************************************************/
      /** Class for the minimum trace detector.
      *
      * Provides the functionality specific to the minimum detector.
      * CTP = Class Trace Preview
      *
      @version
      *
      @pattern
      *******************************************************************************/
      class CTPDetectorMinimum
      {
        /** Minimum detetor. Retruns the minimum of A and B. */
      public: inline static float Detect( const float a_fA, const float a_fB )
              {
                return ((a_fA < a_fB) ? a_fA : a_fB);
              }

              /** Returns the init value of the detected value for the minimum detector. */
      public: inline static float Default()
              {
                return std::numeric_limits<float>::infinity();
              }
      }; // class CTPDetectorMinimum

      /* CLASS DECLARATION **********************************************************/
      /** Class for block-wise prreview data calculation.
      *
      * Berechnet Preview-Daten für einen gegebenen Trace, der blockweise
      * hinzugefügt werden kann. Der Detektor muss beim Anlegen
      * der Instanz angegeben werden.
      *
      * Beispiel:
      *
      *   #include "TracePreview.h"
      *
      *   // Create instance for a maximum preview trace
      *   const int preview_len = 20;
      *   CTracePreview<CTPDetectorMaximum> tObj(preview_len);
      *
      *   // Add block 1 to preview
      *   CVectorIpp< float > tBlock(100);
      *   for (int k=0; k<tBlock.Size(); k++) {
      *     tBlock[k] = sin( 3.14159f * k / tBlock.Size() );
      *   }
      *   tObj.Feed(tBlock);
      *
      *   // Get and display maximum preview trace
      *   CVectorIpp< float > vMaxPrev = tObj.GetPreviewTrace();
      *   cout << "Preview after block 1" << endl;
      *   for (int k=0; k<vMaxPrev.Size(); k++) {
      *     cout << "  vMaxPrev[" << k << "] = " << vMaxPrev[k] << endl;
      *   }
      *
      *   // Add block 2 to preview
      *   for (int k=0; k<tBlock.Size(); k++) {
      *     tBlock[k] = 0.5f * sin( 3.14159f * k / tBlock.Size() );
      *   }
      *   tObj.Feed(tBlock);
      *
      *   // Get and display maximum preview trace
      *   vMaxPrev = tObj.GetPreviewTrace();
      *   cout << "Preview after block 2" << endl;
      *   for (int k=0; k<vMaxPrev.Size(); k++) {
      *     cout << "  vMaxPrev[" << k << "] = " << vMaxPrev[k] << endl;
      *   }
      *
      *
      @version
      *
      @pattern
      *******************************************************************************/
      template < class CTPDetectorType > class CTracePreview
      {
        /** Konstruktor */
      public: CTracePreview(const int a_iPreviewLength = 256, const int a_iNofChannels = 1)
              {
                m_iPreviewLength = (a_iPreviewLength > 0) ? a_iPreviewLength : 0;
                m_iNofChannels = (a_iNofChannels > 0) ? a_iNofChannels : 1;

                // Gedächtnis zurücksetzen und konsistenten Anfangszustand herstellen.
                Reset();
              } // CTracePreview()

              /** Destruktor */
      public: ~CTracePreview()
              {
                // Speicher der Arrays wieder freigeben und Member-Variablen zurücksetzen
                Dealloc();
              } // ~CTracePreview()

              /** Gibt den internen Speicher frei und setzt die Member-Variablen zurück. */
      public: void Dealloc()
              {
                m_iNofChannels = 0;
                m_tRestValue.resize(m_iNofChannels);
                m_tRestCount.resize(m_iNofChannels);
                m_tPreviewTraces.resize(m_iNofChannels);
                Reset();
              } // Dealloc()

              /** Länge der Preview in Samples. PreviewLength >= 0 erforderlich. */
      private:  int (m_iPreviewLength) ;
      public:   inline int GetPreviewLength() { return m_iPreviewLength; }
      public:   void SetPreviewLength( const int a_iPreviewLength )
                {
                  const int iNewPreviewLength = (a_iPreviewLength > 0) ? a_iPreviewLength : 0;

                  // Only init (reset memory) if preview length must be changed
                  if (iNewPreviewLength != m_iPreviewLength)
                  {
                    // Set new preview length
                    m_iPreviewLength = iNewPreviewLength;

                    // Gedächtnis zurücksetzen und konsistenten Anfangszustand herstellen.
                    Reset();
                  }
                } // SetPreviewLength()

                /** Anzahl der Kanäle, bei mehrkanaligen Signalen, z.B. MIMO-Signalen. NofChannels >=1 erforderlich. */
      private:  int (m_iNofChannels) ;
      public:   inline int GetNofChannels() { return m_iNofChannels; }
      public:   void SetNofChannels( const int a_iNofChannels )
                {
                  const int iNewNofChannels = (a_iNofChannels > 0) ? a_iNofChannels : 1;

                  // Only init (reset memory) if number of channels must be changed.
                  if (iNewNofChannels != m_iNofChannels)
                  {
                    // Set new number of channels
                    m_iNofChannels = iNewNofChannels;

                    // Gedächtnis zurücksetzen und konsistenten Anfangszustand herstellen.
                    Reset();
                  }
                } // SetNofChannels()

                /** Dezimations-Faktor. */
      private: int m_iDecimationFactor;
      public:   inline int GetDecimationFactor() { return m_iDecimationFactor; }

                /** Gesamtanzahl der im Preview-Trace berücksichtigten Samples. */
      private: int m_iTotalNofSamples;
      public:   inline int GetTotalNofSamples() { return m_iTotalNofSamples; }

                /** Rest-Wert-Vektor. Enthält bereits detektierte Werte, die noch nicht dem Preview-Trace hinzugefügt wurden. Vektor mit einem Rest-Wert pro Kanal. */
      private: std::vector< float > m_tRestValue;

               /** Rest-Wert-Count-Vektor. Anzahl der bereits detektierten Samples (0<=count<DecimationFactor), die noch nicht dem Preview-Trace hinzugefügt wurden. Vektor mit einem Rest-Count-Wert pro Kanal. */
      private: std::vector< float > m_tRestCount;

               /** Array of preview traces (vectors) for all channels. */
      private: std::vector < std::vector< float > >  m_tPreviewTraces;

               /** Setzt das Gedächtnis zurück und stellt einen konsistenten Zustand der Klasse her. Diese Methode muss immer dann aufgerufen werden, wenn eine neue Preview-Berechnung begonnen werden soll. */
      public: void Reset();

              /** Fügt einen einzelnen Daten-Block eines Traces zur Preview hinzu. */
      public: void Feed( const std::vector<float> (&a_tDataBlock) );

              /** Gibt den Preview-Trace von Kanal 'Ch' zurück (in der gleichen Einheit wie 'DataBlock'. */
      public: std::vector< float > GetPreviewTrace( const int a_iCh = 0 );

              /** Gibt den "Rohen" Preview-Trace von Kanal 'Ch' zurück. Die Länge ergibt sich aus dem Algorithmus.  */
      public: std::vector< float > GetRawPreviewTrace( const int a_iCh = 0 );

              /** Gibt den auf PreviewLength interpolierten Preview-Trace von Kanal 'Ch' zurück (in der gleichen Einheit wie 'DataBlock'. */
      private: std::vector< float > Interpolate( const int a_iCh = 0 );

      private: static inline int lcm(int a, int b)
               {
                 int tmp = gcd(a, b);
                 return tmp ? (a / tmp * b) : 0;
               }

      private: static inline int gcd(int a, int b)
               {
                 for (;;)
                 {
                   if (a == 0) { return b; }
                   b %= a;
                   if (b == 0) { return a; }
                   a %= b;
                 }
               }
      }; // class CTracePreview

      /** Setzt das Gedächtnis zurück und stellt einen konsistenten Zustand der Klasse her. Diese Methode muss immer dann aufgerufen werden, wenn eine neue Preview-Berechnung begonnen werden soll. */
      template < class CTPDetectorType > void CTracePreview<CTPDetectorType>::Reset()
      {
        // Gesamtanzahl der Samples zurücksetzen
        m_iTotalNofSamples = 0;

        // Dezimations-Faktor zuruecksetzen
        m_iDecimationFactor = 1;

        // Arrays neu anlegen
        m_tRestValue.resize(m_iNofChannels);
        m_tRestCount.resize(m_iNofChannels);
        m_tPreviewTraces.resize(m_iNofChannels);

        // Preview-Trace Gedächtnis zurücksetzen
        for (int k=0; k<m_tPreviewTraces.size(); k++)
        {
          m_tPreviewTraces[k].resize(0);
        }

        // Rest-Wert-Vektor zurücksetzen
        for (int k=0; k<m_tRestValue.size(); k++)
        {
          m_tRestValue[k] = CTPDetectorType::Default();
        }

        // Rest-Wert-Count-Vektor zurücksetzen
        for (int k=0; k<m_tRestCount.size(); k++)
        {
          m_tRestCount[k] = 0;
        }
      } // Reset()

      /** Fügt einen einzelnen Daten-Block eines Traces zur Preview hinzu. */
      template < class CTPDetectorType > void CTracePreview<CTPDetectorType>::Feed( const std::vector<float> (&a_tDataBlock) )
      {
        bool bAddBlock = true;

        // Prüfen ob Blocklänge zu Anzahl von Kanälen passt
        if ( 0 != a_tDataBlock.size() % m_iNofChannels )
        {
          // Error: BlockLänge != n*m_iNofChannels => Block nicht hinzufügen
          throw DaiException(ErrorCodes::IQPreviewError, "CTracePreview::Feed() - a_tDataBlock.Size() != n*NofChannels");
        }

        // Prüfen ob überhaupt ein Preview-Trace berechnet werden soll
        if ( m_iPreviewLength < 1 )
        {
          // Info: m_iPreviewLength < 1 => Block nicht hinzufügen, da nichts zu tun ist
          bAddBlock = false;
        }

        //=====================================================================================================
        // Neuen Daten-Bloch hinzufügen (in mehreren Schritten)
        //=====================================================================================================
        if (bAddBlock) {
          // Anzahl Samples eines Channels (gleich für alle Channels)
          const int iNofSamplesOfChannel = (int)a_tDataBlock.size() / m_iNofChannels;

          // Gesamtanzahl der Samples aktualisieren
          m_iTotalNofSamples += iNofSamplesOfChannel;

          // Neuen Teil-Dezimationsfaktor bestimmen (floor-Operation)
          int q = m_iTotalNofSamples / (m_iPreviewLength * m_iDecimationFactor);
          // Sicherstellen, dass der Dezimationsfaktor mindestens 1 ist.
          // Dieser Fall tritt auf falls hier q<1 ist.
          if (q < 1) {
            q = 1;
          }

          // Schleife über alle Kanäle
          // Note: m_iNofChannels == m_tPreviewTraces.Size() ist hier gewährleistet.
          for (int ch=0; ch<m_iNofChannels; ch++) {
            // ----------------------------------------------------------------------------
            // Gespeicherten Preview-Trace um 'q' dezimieren und Rest merken
            // ----------------------------------------------------------------------------

            // Länge des resultierenden dezimierten Preview-Traces berechnen.
            // m_tPreviewTraces[ch].Size() = iNofDecimatedSamples * q + iNofRestSamples
            const int iNofDecimatedSamples = (int)m_tPreviewTraces[ch].size() / q;
            const int iNofRestSamples = (int)m_tPreviewTraces[ch].size() - iNofDecimatedSamples * q; // remainder

            // Nur weitermachen, falls es etwas zu dezimieren gibt, d.h. wenn m_tPreviewTraces[ch][0] existiert.
            if ( m_tPreviewTraces[ch].size() > 0 ) {
              // Source-Pointer für Kopier-Aktion
              float *pSrc = &m_tPreviewTraces[ch][0];

              // Zukünftiger Preview-Trace von Channel 'ch' (Länge = 0 erlaubt)
              std::vector< float > tFuturePreviewTrace(iNofDecimatedSamples);

              // Nur weitermachen, falls es etwas zu kopieren gibt
              if ( tFuturePreviewTrace.size() > 0 ) {
                float *pDst = &tFuturePreviewTrace[0];
                for ( int i=0; i<iNofDecimatedSamples; i++ ) {
                  // Detector: Combine 'DecimationFactor' values of saved Preview Trace
                  float fValue = CTPDetectorType::Default();
                  for ( int k=0; k<q; k++ ) {
                    fValue = CTPDetectorType::Detect(fValue,*pSrc);
                    pSrc++;
                  } // for ( int k=0; k<q; k++ )

                  // Save detected value
                  *pDst = fValue;
                  pDst++;
                } // for ( int i=0; i<iNofDecimatedSamples; i++ )
              } // if ( tFuturePreviewTrace.Size() > 0 )

              // Rest-Handling
              // Detector: Combine remaining samples with detector
              // Init mit Rest-Vektor des zuletzt hinzugefügten Blocks.
              float fRestValue = m_tRestValue[ch];
              for ( int i=0; i<iNofRestSamples; i++ ) {
                fRestValue = CTPDetectorType::Detect(fRestValue,*pSrc);
                pSrc++;
              } // for ( int i=0; i<iNofRestSamples; i++ )

              // Save rest value and count (with decimation factor = 1)
              m_tRestValue[ch] = fRestValue;
              m_tRestCount[ch] += iNofRestSamples * m_iDecimationFactor;

              // Gespeicherten Preview-Trace durch um 'q' dezimierten ersetzen
              // Note: Vektor erst hier, da für die Berechnung des Rest-Vektors der alte m_tPreviewTraces[ch] noch benötigt wird.
              m_tPreviewTraces[ch] = tFuturePreviewTrace;
            } // if ( m_tPreviewTraces[ch].Size > 0 )
          } // for (int ch=0; ch<m_iNofChannels; ch++)

          // Gesamt-Dezimationsfaktor aktualisieren
          m_iDecimationFactor *= q;

          // Schleife über alle Kanäle
          // Note: m_iNofChannels == m_tPreviewTraces.Size() ist hier gewährleistet.
          for (int ch=0; ch<m_iNofChannels; ch++) {
            // ----------------------------------------------------------------------------
            // [Rest,Neuer Block] um 'DecimationFactor' dezimieren und (neuen) Rest merken
            // ----------------------------------------------------------------------------

            // Länge des Vektors berechnen, der aus der Dezimation des
            // Rest-Vektors und des angehängten neuen Blocks resultiert.
            // Länge Rest Vektor + Länge neuer Block =
            // = m_tRestCount[ch] + a_tDataBlock.Size() / m_iNofChannels
            // = m_tRestCount[ch] + iNofSamplesOfChannel
            // = iNofDecimatedSamples * m_iDecimationFactor + iNofRestSamples
            const int iNofDecimatedSamples = ( m_tRestCount[ch] + iNofSamplesOfChannel ) / m_iDecimationFactor;

            // Dezimierte Version des neuen Blocks (+Rest-Vektor davor)
            // Dieser Vektor wird später an den Preview-Trace angehängt.
            std::vector< float > tDecimatedBlock(iNofDecimatedSamples);

            // Nur weitermachen, falls es etwas zu dezimieren gibt, d.h. hier wenn neuer Block nicht leer ist und a_tDataBlock[0] existiert.
            if ( iNofSamplesOfChannel > 0 ) {
              // Source-Pointer für Kopier-Aktion auf gewünschten Kanal setzen.
              float const *pSrc = &a_tDataBlock[ch];

              if ( tDecimatedBlock.size() > 0 ) {
                float *pDst = &tDecimatedBlock[0];

                // Detector: Combine 'DecimationFactor' values
                // Mit Rest-Vektor und Count initialisieren.
                float fValue = m_tRestValue[ch];
                int iCount = m_tRestCount[ch];

                // Schleife über alle Samples in Kanal 'ch' des neuen Blocks
                for ( int i=0; i<iNofSamplesOfChannel; i++ ) {
                  fValue = CTPDetectorType::Detect(fValue,*pSrc);
                  pSrc += m_iNofChannels; // jump to next sample of same channel
                  iCount++;

                  if ( iCount == m_iDecimationFactor ) {
                    // Save decimated vector
                    *pDst = fValue;
                    pDst++;
                    // Reset decimator
                    fValue = CTPDetectorType::Default();
                    iCount = 0 ;
                  } // if ( iCount == m_iDecimationFactor )
                } // for ( int i=0; i<iNofDecimatedSamples; i++ )

                // Rest merken für den nächsten Block
                m_tRestValue[ch] = fValue;
                m_tRestCount[ch] = iCount;

                // Append vector
                //m_tPreviewTraces[ch].Append(tDecimatedBlock);
                m_tPreviewTraces[ch].insert(m_tPreviewTraces[ch].end(), tDecimatedBlock.begin(), tDecimatedBlock.end());
              } // if ( tDecimatedBlock.Size() > 0 )
            } // if ( iNofSamplesOfChannel > 0 )
          } // for (int ch=0; ch<m_iNofChannels; ch++)
        } // if (bAddBlock)
      } // Feed()

      /** Gibt den Preview-Trace von Kanal 'ch' zurück (in der gleichen Einheit wie 'DataBlock'. */
      template < class CTPDetectorType > std::vector< float > CTracePreview<CTPDetectorType>::GetPreviewTrace( const int a_iCh )
      {
        // Check input parameter
        if ( ( a_iCh < 0 ) || ( a_iCh >= m_tPreviewTraces.size() ) )
        {
          // Error: Kanal existiert nicht
          throw DaiException(ErrorCodes::IQPreviewError, "CTracePreview::GetPreviewTrace() - Invalid channel number");
        }
        // Get interpolated trace
        return Interpolate(a_iCh);
      } // GetPreviewTrace()

      /** Gibt den "Rohen" Preview-Trace von Kanal 'Ch' zurück. Die Länge ergibt sich aus dem Algorithmus.  */
      template < class CTPDetectorType > std::vector< float > CTracePreview<CTPDetectorType>::GetRawPreviewTrace( const int a_iCh )
      {
        // Check input parameter
        if ( ( a_iCh < 0 ) || ( a_iCh >= m_tPreviewTraces.size() ) )
        {
          // Error: Kanal existiert nicht
          throw DaiException(ErrorCodes::IQPreviewError, "CTracePreview::GetRawPreviewTrace() - Invalid channel number");
        }
        // Return saved preview trace
        // Note: m_tPreviewTraces[0] always exists here.
        return m_tPreviewTraces.at(a_iCh);
      } // GetRawPreviewTrace()

      /** Gibt den auf PreviewLength interpolierten Preview-Trace von Kanal 'Ch' zurück (in der gleichen Einheit wie 'DataBlock'. */
      template < class CTPDetectorType > std::vector< float > CTracePreview<CTPDetectorType>::Interpolate( const int a_iCh )
      {
        // Init return vector
        std::vector< float > tInterpolatedTrace;

        if ( ( m_tPreviewTraces[a_iCh].size() < 1 ) || ( m_iPreviewLength < 1 ) ) {
          // Length of saved preview trace or desired preview length is zero => do not return a preview trace
          tInterpolatedTrace.resize(0);
        } else if ( m_iPreviewLength == m_tPreviewTraces[a_iCh].size() ) {
          // The desired preview trace has the same number of points as the save preview trace. => Just copy
          tInterpolatedTrace = m_tPreviewTraces.at(a_iCh);
        } else if ( m_iPreviewLength < m_tPreviewTraces[a_iCh].size() ) {
          // The desired preview trace has less points than the saved preview trace => combine points with detector

          // Notation:
          //  x = Vector of stored preview trace, i.e. 'm_tPreviewTraces[a_iCh]'
          //  p = Vector of preview trace, i.e. 'tInterpolatedTrace' to be calcualted.
          const int X = (int)m_tPreviewTraces[a_iCh].size();
          const int P = m_iPreviewLength;
          const int LCM = lcm(X,P); // Least common multiple
          const int xm = LCM / X;   // multiplier for index ix of vector x
          const int pm = LCM / P;   // multiplier for index ip of vector p

          // Resize and init interpolated trace
          tInterpolatedTrace.resize(m_iPreviewLength);
          std::fill(tInterpolatedTrace.begin(), tInterpolatedTrace.end(), CTPDetectorType::Default());

          float fValue = CTPDetectorType::Default();
          int ip = 0;
          for ( int ix=0; ix<X; ix++ ) {
            // Calculate integers values on common x-axis for ix and ip
            const int a = (ix+1)*xm;
            const int b = (ip+1)*pm;
            if (a<=b) {
              // Just detect, i.e. add x[ix] to detected value.
              fValue = CTPDetectorType::Detect(fValue,m_tPreviewTraces[a_iCh][ix]);
            } else {
              // Save value to p and reset detector
              tInterpolatedTrace[ip] = fValue;
              ip++;
              fValue = m_tPreviewTraces[a_iCh][ix];
            }
          } // for ( int ix=0; ix<X; ix++ )
          // Save last value to p
          tInterpolatedTrace[ip] = fValue;
        } else {
          // The desired preview trace has more points than the saved preview trace => Just copy
          tInterpolatedTrace = m_tPreviewTraces.at(a_iCh);
        }

        // Add value of rest vector to last sample (do this in all cases)
        if (tInterpolatedTrace.size() > 0) {
          float *fLast = &tInterpolatedTrace[tInterpolatedTrace.size()-1];
          *fLast = CTPDetectorType::Detect( *fLast, m_tRestValue[a_iCh] );
        }

        return tInterpolatedTrace;
      } // Interpolate()
    }
  }
}

#ifdef _MSC_VER
#pragma warning (pop)
#elif __GNUC__
#pragma GCC diagnostic pop
#endif

/*** @endcond ***/