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
@copyright     (c) Rohde & Schwarz GmbH & Co. KG, Munich
@version       $Workfile: IqPreview.h $
*
@responsible
*
@language      C++
*
@description   Calculation of a I/Q preview histogram.
*              The I/Q data comes in blocks.
*
@see
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
#include <algorithm>
#include <iostream>

#include "daiexception.h"
#include "errorcodes.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /** @brief Class for block-wise preview data calculation.
      *
      * Berechnet Preview-Daten für I/Q-Daten, die blockweise hinzugefügt werden.
      *
      @version
      *
      @pattern
      *******************************************************************************/

      template < typename T > class CIqPreview
      {
        /** Konstruktor */
      public: CIqPreview(const int a_iNofPositiveBins = 32, const int a_iNofChannels = 1)
              {
                m_iNofPositiveBins = (a_iNofPositiveBins > 0) ? a_iNofPositiveBins : 1;
                m_iNofChannels = (a_iNofChannels > 0) ? a_iNofChannels : 1;

                // Gedächtnis zurücksetzen und konsistenten Anfangszustand herstellen.
                Reset();
              } // CIqPreview()

              /** Destruktor */
      public: ~CIqPreview()
              {
                // Speicher der Arrays wieder freigeben und Member-Variablen zurücksetzen
                Dealloc();
              } // ~CIqPreview()

              /** Gibt den internen Speicher frei und setzt die Member-Variablen zurück. */
      public: void Dealloc()
              {
                m_iNofChannels = 0;
                m_iNofPositiveBins = 0;
                m_fBinWidth = 0.0f;
                m_tPreviewHistograms.resize(0);
              } // Dealloc()

              /** Anzahl der Bins der positiven I-Achse. Alle 4 Achsen, f.h. positive / negative I / Q Achsen haben die gleiche Anzahl von Bins. NofPositiveBins >= 0 erforderlich. */
      private:  int (m_iNofPositiveBins) ;
      public:   inline int GetNofPositiveBins() { return m_iNofPositiveBins; }
      public:   void SetNofPositiveBins( const int a_iNofPositiveBins )
                {
                  const int iNew = (a_iNofPositiveBins > 0) ? a_iNofPositiveBins : 1;

                  // Only init (reset memory) if preview length must be changed
                  if (iNew != m_iNofPositiveBins)
                  {
                    // Set new preview length
                    m_iNofPositiveBins = iNew;

                    // Gedächtnis zurücksetzen und konsistenten Anfangszustand herstellen.
                    Reset();
                  }
                } // SetNofPositiveBins()

                /** Anzahl der Kanäle, bei mehrkanaligen Signalen, z.B. MIMO-Signalen. NofChannels >=1 erforderlich. */
      private:  int (m_iNofChannels) ;
      public:   inline int GetNofChannels() { return m_iNofChannels; }
      public:   void SetNofChannels( const int a_iNofChannels )
                {
                  const int iNew = (a_iNofChannels > 0) ? a_iNofChannels : 1;

                  // Only init (reset memory) if number of channels must be changed.
                  if (iNew != m_iNofChannels)
                  {
                    // Set new number of channels
                    m_iNofChannels = iNew;

                    // Gedächtnis zurücksetzen und konsistenten Anfangszustand herstellen.
                    Reset();
                  }
                } // SetNofChannels()

                /** Gesamtanzahl der im Preview-Trace berücksichtigten Samples. */
      private:  int m_iTotalNofSamples;
      public:   inline int GetTotalNofSamples() { return m_iTotalNofSamples; }

                /** Breite eines Bins */
      private:    T m_fBinWidth;
      public:     inline T GetBinWidth() { return m_fBinWidth; }

                  /** Array of preview histograms (vectors) for all channels. */
      private: std::vector < std::vector< long long > >  m_tPreviewHistograms;

               /** Findet den betragsmaessig groessten I oder Q Wert eines komplexen Vektors */
      private: T MaxAbs_I_and_Q( const std::vector<std::complex<T>> (&a_tDataBlock) )
               {
                 T fMaxAbs_I = std::abs(std::max_element(
                   a_tDataBlock.begin(),
                   a_tDataBlock.end(),
                   [](std::complex<T> const& lhs, std::complex<T> const& rhs) { return std::abs(lhs.real()) < std::abs(rhs.real()); })->real());

                 T fMaxAbs_Q = std::abs(std::max_element(
                   a_tDataBlock.begin(),
                   a_tDataBlock.end(),
                   [](std::complex<T> const& lhs, std::complex<T> const& rhs) { return std::abs(lhs.imag()) < std::abs(rhs.imag()); })->imag());

                 return std::max(fMaxAbs_I, fMaxAbs_Q);
               }

               /** Fasst 'dezi' Bins des Histogramms von 0->Ende für alle Kanäle zusammen und fügt hinten Bins mit Nullen an. */
      private: void Histogramm_Bins_Zusammenfassen( const int a_iDezi );

               /** Fügt dien aktuellen Block an I/Q-Daten zum Histogramm hinzu. */
      private: void Fuege_Block_zu_Histogramm( const std::vector<std::complex<T>> (&a_tDataBlock) );

               /** Setzt das Gedächtnis zurück und stellt einen konsistenten Zustand der Klasse her. Diese Methode muss immer dann aufgerufen werden, wenn eine neue Preview-Berechnung begonnen werden soll. */
      public: void Reset();

              /** Fügt einen einzelnen I/Q-Daten-Block zur Preview hinzu. */
      public: void Feed( const std::vector<std::complex<T>> (&a_tDataBlock) );

              /** Gibt das I/Q-Preview-Histogramm von Kanal 'Ch' zurück. */
      public: std::vector< long long > GetPreviewHistogram( const int a_iCh = 0 );

              /** Gibt das I/Q-Preview-Histogramm von Kanal 'Ch' als String zurück. Ein Zeichen entspricht einem Bin (Zeile 1 links nacht rechts, Zeile 2 links nach rechts, ... */
      public: std::string GetPreviewHistogramAsString( const int a_iCh = 0 );
      }; // class CIqPreview

      /** Gibt das I/Q-Preview-Histogramm von Kanal 'Ch' zurück. */
      template < typename T > std::vector< long long > CIqPreview<T>::GetPreviewHistogram( const int a_iCh )
      {
        // Check input parameter
        if ( ( a_iCh < 0 ) || ( a_iCh >= m_tPreviewHistograms.size() ) )
        {
          // Error: Kanal existiert nicht
          throw DaiException(ErrorCodes::IQPreviewError, "CIqPreview::GetPreviewHistogram() - Invalid channel number");
        }
        // Return histogram of channel a_icH
        return m_tPreviewHistograms[a_iCh];
      } // GetPreviewHistogram()

      /** Gibt das I/Q-Preview-Histogramm von Kanal 'Ch' als String zurück. Ein Zeichen entspricht einem Bin (Zeile 1 links nacht rechts, Zeile 2 links nach rechts, ... */
      template < typename T > std::string CIqPreview<T>::GetPreviewHistogramAsString( const int a_iCh )
      {
        // Check input parameter
        if ( ( a_iCh < 0 ) || ( a_iCh >= m_tPreviewHistograms.size() ) )
        {
          // Error: Kanal existiert nicht
          throw DaiException(ErrorCodes::IQPreviewError, "CIqPreview::GetPreviewHistogramAsString() - Invalid channel number");
        }

        // Die Ipp64s-Werte des Histogramms muessen so quantisiert werden, dass Zeichen zwischen 0 - 9 herauskommen

        // Zuerst maximale Anzahl an Werten in einem Bin suchen (RsSigLib-Funktion gibt es nicht für Ipp64s)
        long long iMax =  9; // Init, damit 9*(x-1)/iMax+1>0 gilt, was ein gültiger Index sein soll
        const int NofBins = (int)m_tPreviewHistograms[a_iCh].size();
        if (NofBins > 0) {
          long long *ptr = &m_tPreviewHistograms[a_iCh][0];
          for (int k=0; k<NofBins; k++, ptr++ ) {
            if (*ptr > iMax) {
              iMax = *ptr;
            }
          }
        } // if (NofBins > 0)

        // Quantisieren auf 0->9
        //
        //     x      ->  y
        // -------------------
        //     0      ->  '0'
        // a)  1      ->  '1'
        //     ...
        //     iMax   ->  '9'
        // b)  iMax+1 ->  '10' (tritt nie auf)
        //
        // Aus den Punkten a) und b) laesst sich eine Gerade berechnen, die quantisiert folgendes ergibt
        //
        // y = floor(9/iMax*(x-1)+1)
        //
        // mit
        //   x = Anzahl an gezählten Samples im Histogramm <= iMax
        //   y = Quantisierter Wert, 0<=y<=9
        std::ostringstream oss;
        if (NofBins > 0) {
          long long *ptr = &m_tPreviewHistograms[a_iCh][0];
          for (int k=0; k<NofBins; k++, ptr++ ) {
            oss << (9*(*ptr-1)+iMax)/iMax;
          }
        } // if (NofBins > 0)

        return oss.str();
      } // GetPreviewHistogramAsString()

      /** Setzt das Gedächtnis zurück und stellt einen konsistenten Zustand der Klasse her. Diese Methode muss immer dann aufgerufen werden, wenn eine neue Preview-Berechnung begonnen werden soll. */
      template < typename T> void CIqPreview<T>::Reset()
      {
        // Gesamtanzahl der Samples zurücksetzen
        m_iTotalNofSamples = 0;

        // Binbreite zuruecksetzen
        m_fBinWidth = 0.0;

        // Arrays neu anlegen
        m_tPreviewHistograms.resize(m_iNofChannels);

        // Histogramm-Gedächtnis aller Kanäle zurücksetzen
        const int iNofPixels = (2*m_iNofPositiveBins) * (2*m_iNofPositiveBins);
        for (int ch=0; ch<m_tPreviewHistograms.size(); ch++)
        {
          m_tPreviewHistograms[ch].resize(iNofPixels);
          if (iNofPixels>0) {
            std::fill(m_tPreviewHistograms[ch].begin(), m_tPreviewHistograms[ch].end(), 0);
          }
        }
      } // Reset()

      /** Fügt dien aktuellen Block an I/Q-Daten zum Histogramm hinzu. */
      template < typename T > void CIqPreview<T>::Fuege_Block_zu_Histogramm( const std::vector<std::complex<T>> (&a_tDataBlock) )
      {
        // Anzahl Samples eines Channels (gleich für alle Channels)
        const int iNofSamplesOfChannel = (int)a_tDataBlock.size() / m_iNofChannels;

        // Anzahl Bins pro Zeile
        const int iNofBinsPerLine = m_iNofPositiveBins+m_iNofPositiveBins;

        T fInvBinWidth = 1.0f;
        if (m_fBinWidth>0.0f) {
          fInvBinWidth = 1.0f / m_fBinWidth;
        }

        // Sicherheitsabfrage wegen Pointer
        if ( iNofSamplesOfChannel > 0 ) {
          // Gesamtanzahl der Samples aktualisieren
          m_iTotalNofSamples += iNofSamplesOfChannel;

          // Schleife über alle Kanäle
          for (int ch=0; ch<m_tPreviewHistograms.size(); ch++) {
            // Schleife über alle Samples
            const std::complex<T> *ptr = &a_tDataBlock[0]; // Zugriff oben sichergestellt!

            // Note: Das Einsortieren koennte noch durch Vektor-Operationen optimiert werden.
            for ( int k=0; k<iNofSamplesOfChannel; k++,ptr+=m_iNofChannels ) {
              // Reihenfolge = 1. Zeile links->rechts, 2.Zeilte links->rechts, uws...
              const int idx_I = static_cast<int>( floor( fInvBinWidth * ptr->real() ) ) + m_iNofPositiveBins;
              const int idx_Q = static_cast<int>( floor(-fInvBinWidth * ptr->imag() ) ) + m_iNofPositiveBins;
              const int idx_histo = idx_Q*iNofBinsPerLine + idx_I;

              m_tPreviewHistograms[ch].at(idx_histo) += 1; // Increment bin count
            }
          } // for (int ch=0; ch<m_tPreviewHistograms.Size(); ch++)
        } // if ( iNofSamplesOfChannel > 0 )
      } // Fuege_Block_zu_Histogramm()

      /** Fasst 'dezi' Bins des Histogramms von 0->Ende für alle Kanäle zusammen und fügt hinten Bins mit Nullen an. */
      template < typename T > void CIqPreview<T>::Histogramm_Bins_Zusammenfassen( const int a_iDezi )
      {
        // Nur weitermachen, falls überhaupt etwas zu dezimieren ist
        if (a_iDezi > 1) {
          const int iN_Zeile_oder_Spalte = m_iNofPositiveBins + m_iNofPositiveBins;
          const int iN                   = iN_Zeile_oder_Spalte * iN_Zeile_oder_Spalte;

          // Schleife über alle Kanäle
          for (int ch=0; ch<m_tPreviewHistograms.size(); ch++) {
            if ( iN != m_tPreviewHistograms[ch].size() ) {
              throw DaiException(ErrorCodes::IQPreviewError, "CIqPreview::Histogramm_Bins_Zusammenfassen() - Histogramm hat falsche Laenge!");
            }

            // Ziel-Histogramm anlegen und mit Nullen initalisieren ----------------------------------------------------------
            std::vector< long long  >  vZielHisto_links_rechts(m_tPreviewHistograms[ch].size());
            std::fill(vZielHisto_links_rechts.begin(), vZielHisto_links_rechts.end(), 0);

            // 1. I-Bins von 0 -> rechts zusammenfassen ----------------------------------------------------------------------
            int idx_src = 0;
            int idx_dst = 0;
            for ( int zeile = 0; zeile < iN_Zeile_oder_Spalte; zeile++ ) {
              // Idx auf erstes Bin (relativ zum Zeilenanfang) rechts von "Null"
              idx_dst = idx_src = zeile*iN_Zeile_oder_Spalte + m_iNofPositiveBins;
              int nof_merged_bins = 0;
              // Schleife über Spalten: 0->rechts
              for ( int k = 0; k < m_iNofPositiveBins; k++, idx_src++) {
                vZielHisto_links_rechts[idx_dst] += m_tPreviewHistograms[ch][idx_src]; // Merge Bins
                nof_merged_bins++;
                if (nof_merged_bins == a_iDezi) {
                  idx_dst++; // nächstes Bin nach rechts
                  nof_merged_bins = 0;
                }
              }
            }

            // 2. I-Bins von 0 -> links zusammenfassen -----------------------------------------------------------------------
            for ( int zeile = 0; zeile < iN_Zeile_oder_Spalte; zeile++ ) {
              idx_dst = idx_src = zeile*iN_Zeile_oder_Spalte + m_iNofPositiveBins-1; // Idx auf erstes Bin links von "Null"
              int nof_merged_bins = 0;
              // Schleife über Spalten: links<-0
              for ( int k = 0; k < m_iNofPositiveBins; k++, idx_src--) {
                vZielHisto_links_rechts[idx_dst] += m_tPreviewHistograms[ch][idx_src]; // Merge Bins
                nof_merged_bins++;
                if (nof_merged_bins == a_iDezi) {
                  idx_dst--; // nächstes Bin nach links
                  nof_merged_bins = 0;
                }
              }
            }

            // Ziel-Histogramm anlegen und mit Nullen initalisieren ----------------------------------------------------------
            std::vector< long long  >  vZielHisto(m_tPreviewHistograms[ch].size());
            std::fill(vZielHisto.begin(), vZielHisto.end(), 0);

            // 3. Q-Bins von 0 -> oben zusammenfassen ------------------------------------------------------------------------
            for ( int spalte = 0; spalte < iN_Zeile_oder_Spalte; spalte++ ) {
              idx_dst = idx_src = iN_Zeile_oder_Spalte*(m_iNofPositiveBins-1) + spalte; // Idx auf erste Zeile oberhalb von "Null" und gewuenschte Spalte
              int nof_merged_bins = 0;
              // Schleife über Zeilen: 0->oben
              for ( int k = 0; k < m_iNofPositiveBins; k++, idx_src-=iN_Zeile_oder_Spalte ) {
                vZielHisto[idx_dst] += vZielHisto_links_rechts[idx_src]; // Merge Bins
                nof_merged_bins++;
                if (nof_merged_bins == a_iDezi) {
                  idx_dst-=iN_Zeile_oder_Spalte; // eine Zeile nach oben
                  nof_merged_bins = 0;
                }
              }
            }

            // 2. Q-Bins von 0 -> unten zusammenfassen -----------------------------------------------------------------------
            for ( int spalte = 0; spalte < iN_Zeile_oder_Spalte; spalte++ ) {
              idx_dst = idx_src = m_iNofPositiveBins*iN_Zeile_oder_Spalte + spalte; // Idx auf erste Zeile unterhalb von "Null" und gewuenschte Spalte
              int nof_merged_bins = 0;
              // Schleife über Zeilen: 0->unten
              for ( int k = 0; k < m_iNofPositiveBins; k++, idx_src+=iN_Zeile_oder_Spalte ) {
                vZielHisto[idx_dst] += vZielHisto_links_rechts[idx_src]; // Merge Bins
                nof_merged_bins++;
                if (nof_merged_bins == a_iDezi) {
                  idx_dst+=iN_Zeile_oder_Spalte; // eine Zeile nach unten
                  nof_merged_bins = 0;
                }
              }
            }

            // Ziel-Histogramm in Member-Variable kopieren -------------------------------------------------------------------
            m_tPreviewHistograms[ch] = vZielHisto;
          } // for (int ch=0; ch<m_tPreviewHistograms.Size(); ch++)

          // Neue Binbreite berechnen (nur einmal, da diese für alle Kanäle gelten sollt)
          m_fBinWidth *= a_iDezi;
        } // if (a_iDezi > 1)
      } // Histogramm_Bins_Zusammenfassen()

      /** Fügt einen einzelnen Daten-Block eines Traces zur Preview hinzu. */
      template < typename T > void CIqPreview<T>::Feed( const std::vector<std::complex<T>> (&a_tDataBlock) )
      {
        bool bAddBlock = true;

        // Prüfen ob Blocklänge zur Anzahl der Kanälen passt
        if ( 0 != a_tDataBlock.size() % m_iNofChannels )
        {
          // Error: BlockLänge != n*m_iNofChannels => Block nicht hinzufügen
          throw DaiException(ErrorCodes::IQPreviewError, "CIqPreview::Feed() - a_tDataBlock.Size() != n*NofChannels");
        }

        // Prüfen ob überhaupt eine Preview berechnet werden soll
        if ( m_iNofPositiveBins < 1 )
        {
          // Info: m_iNofPositiveBins < 1 => Block nicht hinzufügen, da nichts zu tun ist
          bAddBlock = false;
        }

        //=====================================================================================================
        // Neuen Daten-Bloch hinzufügen (in mehreren Schritten)
        //=====================================================================================================
        if (bAddBlock) {
          // Maximalen Betrag aller I- und Q-Werte bestimmen.
          // Es wird nicht kanalweise unterschieden, damit alle I/Q-Previews die gleichen Achsen haben.
          T fMaxAbs = MaxAbs_I_and_Q(a_tDataBlock);

          // Sonderbehandlung wenn die Binbreite = 0 ist (ist auch beim ersten Block der Fall)
          if (m_fBinWidth == 0.0f) {
            // Binbreite so setzen (initialisieren), dass die Mitte des letzten Bins fMaxAbs entspricht:
            //    fMaxAbs = (NofPositiveBins-0.5) * m_fBinWidth
            // => m_fBinWidth = fMaxAbs / (NofPositiveBins-0.5)
            if (m_iNofPositiveBins<1) {
              // Error: NofPositiveBins zu klein
              throw DaiException(ErrorCodes::IQPreviewError, "CIqPreview::Feed() - NofPositiveBins < 1");
            }

            // Allererste Binbreite berechnen
            m_fBinWidth = fMaxAbs / (static_cast<T>(m_iNofPositiveBins) - 0.5f);
          } // if (m_fBinWidth == 0.0f)

          //-----------------------------------------------------------------------------------------------------
          // Muss das Histogramm vergroessert werden? (gilt für alle Kanäle)
          //-----------------------------------------------------------------------------------------------------
          if (m_fBinWidth > 0.0f) {
            // Nur etwas tun wenn Binbreite > 0 ist

            // Rechte Grenze des Histogramms (ausschliesslich) < m_iNofPositiveBins * m_fBinWidth
            if ( fMaxAbs >= m_fBinWidth*m_iNofPositiveBins ) {
              // Histogramm vergroessern

              // Neue Binbreite berechnen
              //   Neue Binbreite so setzen, dass
              //   - die Mitte des letzten Bins MINDESTENS fMaxAbs entspricht
              //   - eine ganze Zahl 'dezi' bisheriger Bins zusammengefasst werden kann
              //   d.h.
              //   dezi * (NofPositiveBins-0.5) * m_fBinWidth >= fMaxAbs
              //   => dezi = ceil (fMaxAbs/m_fBinWidth/(NofPositiveBins-0.5))
              const double fDezi = ceil(fMaxAbs / m_fBinWidth / (-0.5f+m_iNofPositiveBins));
              const int iDezi = static_cast<int>(fDezi);

              // Bins des Histogramms aller Kanäle zusammenfassen und Binbreite updaten
              Histogramm_Bins_Zusammenfassen(iDezi);
            } // if ( fMaxAbs >= m_fBinWidth*m_iNofPositiveBins )

            //-----------------------------------------------------------------------------------------------------
            // Neuen Block zu Histogramm hinzufuegen
            //-----------------------------------------------------------------------------------------------------
            Fuege_Block_zu_Histogramm(a_tDataBlock);
          } // if (m_fBinWidth > 0.0f)
        } // if (bAddBlock)
      } // Feed()
    }
  }
}

#ifdef _MSC_VER
#pragma warning (pop)
#elif __GNUC__
#pragma GCC diagnostic pop
#endif

/*** @endcond ***/