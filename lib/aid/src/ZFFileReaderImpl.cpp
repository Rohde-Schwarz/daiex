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

/* INCLUDE FILES ******************************************************************/
#include <cstring>
#include "ErrCtrl.h"
#include "ZFFileReaderImpl.h"
#include "replacement.h"
#include <iostream>
#include <sstream>
#ifndef _WIN32
#include <cmath>
#endif
/* STATIC INITIALIZATION **********************************************************/

/* ------------------------------------------------------------------------------ */

using namespace std;
using namespace AmlabCommon;

namespace AmlabFiles
{

  const uint64  kFRH_MAGIC_WORD_BE = 0x726574FBUL;

  const float   kf16BitMax     = 65535.0F;
  const float   kf16BitReciMax = 1.0F / kf16BitMax;

  const float   kf31BitMax     = 2147483648.0F;         // abs(INT_MIN)
  const float   kf31BitReciMax = 1.0F / kf31BitMax;

  const float   kfSHORTMAX     = (1.0F/SHRT_MAX);  //0.0000305185094F;
  //const float   kfSHORTMAX     = 0.00003051757813F;  // A. Frick 8.5.2014: ! The value is 1.0/32768 - this is wrong!

  const uint32  c_samplesForTimestamp = 10;

  /* --- PUBLIC METHODS --- */

  CZFFileReaderImpl::CZFFileReaderImpl()
    : m_strFilename()
    , m_pZFFile(0)
    , m_fileOpened(false)
    , m_fileSize(0)
    , m_endian(ekUnknownEndian)
    , m_firstFrameHeaderPos(0)
    , m_dataHeader(false)
    , m_frameHeaderPos(-1)
    , m_frameCount(0)
    , m_pZFData(0)
    , m_currentSampleIndex(0)
    , m_startSampleIndex(0)
    , m_stopSampleIndex(_UI64_MAX)
    , m_dataInfoChanged(false)
    , m_pObserver(0)
    , m_observerAttributes(0)
    , m_samplesForTimestamp(c_samplesForTimestamp)
    , m_timestampDelta(0)
    , m_strErrorMsg()
    , m_eStatus(ekNoError)
  {
    memset(&m_ZFFrameHeader, 0, c_sizeofZFFrameHeader);
    memset(&m_ZFDataHeader,  0, c_sizeofZFDataHeaderEx);

    memset(&m_lastZFFrameHeader, 0, c_sizeofZFFrameHeader);
    memset(&m_lastZFDataHeader,  0, c_sizeofZFDataHeaderEx);

    memset(&m_prevFrameHeader, 0, c_sizeofZFFrameHeader);
    memset(&m_prevDataHeader,  0, c_sizeofZFDataHeaderEx);

    //lint -save -e569
    m_ZFDataHeader.intKFactor     = kIFD_HEADER_KFACTOR__UNDEFINED;
    m_lastZFDataHeader.intKFactor = kIFD_HEADER_KFACTOR__UNDEFINED;
    //lint -restore

    m_ZFDataLength = c_sizeofZFFrameHeader + c_sizeofZFDataHeaderEx;
    m_pZFData = new uint8[m_ZFDataLength];
  }

  CZFFileReaderImpl::CZFFileReaderImpl(const std::wstring& a_strFilename)
    : m_strFilename(a_strFilename)
    , m_pZFFile(0)
    , m_fileOpened(false)
    , m_fileSize(0)
    , m_endian(ekUnknownEndian)
    , m_firstFrameHeaderPos(0)
    , m_dataHeader(false)
    , m_frameHeaderPos(-1)
    , m_frameCount(0)
    , m_pZFData(0)
    , m_currentSampleIndex(0)
    , m_startSampleIndex(0)
    , m_stopSampleIndex(_UI64_MAX)
    , m_dataInfoChanged(false)
    , m_pObserver(0)
    , m_observerAttributes(0)
    , m_samplesForTimestamp(c_samplesForTimestamp)
    , m_timestampDelta(0)
    , m_strErrorMsg()
    , m_eStatus(ekNoError)
  {
    memset(&m_ZFFrameHeader, 0, c_sizeofZFFrameHeader);
    memset(&m_ZFDataHeader,  0, c_sizeofZFDataHeaderEx);

    memset(&m_lastZFFrameHeader, 0, c_sizeofZFFrameHeader);
    memset(&m_lastZFDataHeader,  0, c_sizeofZFDataHeaderEx);

    memset(&m_prevFrameHeader, 0, c_sizeofZFFrameHeader);
    memset(&m_prevDataHeader,  0, c_sizeofZFDataHeaderEx);

    //lint -save -e569
    m_ZFDataHeader.intKFactor     = kIFD_HEADER_KFACTOR__UNDEFINED;
    m_lastZFDataHeader.intKFactor = kIFD_HEADER_KFACTOR__UNDEFINED;
    //lint -restore

    m_ZFDataLength = c_sizeofZFFrameHeader + c_sizeofZFDataHeaderEx;
    m_pZFData = new uint8[m_ZFDataLength];
  }

  CZFFileReaderImpl::~CZFFileReaderImpl()
  {
    if (m_pZFFile != 0)
    {
      ::fclose(m_pZFFile);
      m_pZFFile = 0;
    }
    delete [] m_pZFData;
    m_pObserver = 0;
  }

  bool CZFFileReaderImpl::isOpen() const
  {
    return (m_pZFFile != 0);
  }

  eStatus CZFFileReaderImpl::open()
  {
    if (isOpen())
      return ekFileAlreadyOpen;

    if (m_strFilename.empty())
      return ekNoFileSpecified;

#ifdef _WIN32
    errno_t l_errno = ::_wfopen_s(&m_pZFFile, m_strFilename.c_str(), L"rb" );
    if (l_errno != 0 || !isOpen())
    {
      return setLastError(ekFileCannotBeOpened, L"File could not be opened for reading.");
    }
#else
    const std::string s( m_strFilename.begin(), m_strFilename.end() );
    m_pZFFile = fopen(s.c_str(), "rb");
    if (m_pZFFile == 0)
    {
      return setLastError(ekFileCannotBeOpened, L"File could not be opened for reading.");
    }
#endif    

    // file size
    (void)::_fseeki64(m_pZFFile,0LL,SEEK_END);
    m_fileSize = ::_ftelli64(m_pZFFile);
    (void)::_fseeki64(m_pZFFile,0LL,SEEK_SET);

    // check first frame
    if (!readFrame(0LL))
    {
      return setLastError(ekInvalidFileHeader, L"The dataheader is invalid.");
    }
    else
    {
      // Frame header
      memcpy(&m_ZFFrameHeader, frameHeader(), c_sizeofZFFrameHeader);
      // Data header
      memset(&m_ZFDataHeader, 0,            c_sizeofZFDataHeaderEx);
      memcpy(&m_ZFDataHeader, dataHeader(), c_sizeofZFDataHeaderEx);

      if (!isDataHeaderEx())
      {
        m_ZFDataHeader.bigtimeStartTimeStamp.structTimeInTwoWords.uintTime_HiOrderBits = 0;
        m_ZFDataHeader.bigtimeStartTimeStamp.structTimeInTwoWords.uintTime_LoOrderBits = 0;
        m_ZFDataHeader.uintSampleCounter_High = 0;
        m_ZFDataHeader.uintSampleCounter_Low  = 0;
        //lint -save -e569
        m_ZFDataHeader.intKFactor = kIFD_HEADER_KFACTOR__UNDEFINED;
        //lint -restore
      }
    }

    m_timestampDelta = (1000000ULL * m_samplesForTimestamp) / getSampleRate();

    m_firstFrameHeaderPos = m_frameHeaderPos;
    eStatus l_status = readProperties();
    (void)::_fseeki64(m_pZFFile,m_firstFrameHeaderPos,SEEK_SET);

    m_fileOpened = true;

    return l_status;
  }

  eStatus CZFFileReaderImpl::close()
  {
    if (!isOpen())    // File not opened
      return ekFileNotOpen;

    (void)reset();

    if (m_pZFFile != 0)
    {
      ::fclose(m_pZFFile);
      m_pZFFile = 0;
    }

    return ekNoError;
  }

  eStatus CZFFileReaderImpl::reset()
  {
    if (!isOpen())
      return ekFileNotOpen;

    m_endian             = ekUnknownEndian;
    m_dataHeader         = false;
    m_frameHeaderPos     = -1;
    m_currentSampleIndex = m_startSampleIndex;
    (void)::_fseeki64(m_pZFFile,0LL,SEEK_SET);

    m_observerAttributes  = 0;
    m_pObserver           = 0;
    m_samplesForTimestamp = c_samplesForTimestamp;

    m_strErrorMsg.clear();
    m_eStatus = ekNoError;

    return ekNoError;
  }

  eStatus CZFFileReaderImpl::getLastError( std::wstring& a_strErrorMsg )
  {
    a_strErrorMsg = m_strErrorMsg;
    m_strErrorMsg.clear();
    eStatus l_lastError = m_eStatus;
    m_eStatus = ekNoError;

    return l_lastError;
  }

  bool CZFFileReaderImpl::hasAdditionalDataHeaderPart() const
  {
    if(!isOpen())
      return false;

    if (isDataHeaderEx())
    {
      return frameHeader()->uintDataHeaderLength > (c_sizeofZFDataHeaderEx / c_sizeof_uint32);
    } 
    return frameHeader()->uintDataHeaderLength > (c_sizeofZFDataHeader / c_sizeof_uint32);
  }

  void CZFFileReaderImpl::getAdditionalDataHeaderPart(std::vector<unsigned int>& a_rAdditionalElements) const
  {
    a_rAdditionalElements.clear();
    if (hasAdditionalDataHeaderPart())
    {
      const uint32* l_pAdditionalDataHeaderPart = reinterpret_cast<const uint32*>(dataHeader());
      uint32 l_numberOfElements = 0;
      if (isDataHeaderEx())
      {
        l_pAdditionalDataHeaderPart += (c_sizeofZFDataHeaderEx / c_sizeof_uint32);
        l_numberOfElements = frameHeader()->uintDataHeaderLength - (c_sizeofZFDataHeaderEx / c_sizeof_uint32);
      }
      else
      {
        l_pAdditionalDataHeaderPart += (c_sizeofZFDataHeader / c_sizeof_uint32);
        l_numberOfElements = frameHeader()->uintDataHeaderLength - (c_sizeofZFDataHeader / c_sizeof_uint32);
      }
      while(l_numberOfElements)
      {
        a_rAdditionalElements.push_back(*l_pAdditionalDataHeaderPart);
        l_pAdditionalDataHeaderPart++;
        l_numberOfElements--;
      }
    }
  }

  void CZFFileReaderImpl::getChangedAttributs()
  {
    if (m_fileOpened)
    {
      m_attributes.clear();
      m_dataInfoChanged = false;

      {
        uint64 l_currentCF = dataHeader()->uintTunerFrequency_High;
        l_currentCF <<= 32;
        l_currentCF |= dataHeader()->uintTunerFrequency_Low;
        uint64 l_prevCF = m_prevDataHeader.uintTunerFrequency_High;
        l_prevCF <<= 32;
        l_prevCF |= m_prevDataHeader.uintTunerFrequency_Low;
        if (l_currentCF != l_prevCF)
        {
          if((m_observerAttributes & (uint32)ekCenterFrequency) == (uint32)ekCenterFrequency)
          {
            m_attributes[(uint32)ekCenterFrequency] = l_currentCF;
          }
          m_dataInfoChanged = true;
        }
      }
      {
        if (dataHeader()->uintBandwidth != m_prevDataHeader.uintBandwidth)
        {
          if((m_observerAttributes & (uint32)ekBandwidth) == (uint32)ekBandwidth)
          {
            m_attributes[(uint32)ekBandwidth] = dataHeader()->uintBandwidth;
          }
          m_dataInfoChanged = true;
        }
      }
      {
        uint32 l_sampleRate = dataHeader()->uintSamplerate;
        if (dataHeader()->uintDecimation)
        {
          l_sampleRate = (dataHeader()->uintSamplerate * dataHeader()->uintInterpolation) / dataHeader()->uintDecimation;
        }
        uint32 l_prevSampleRate = dataHeader()->uintSamplerate;
        if (m_prevDataHeader.uintDecimation)
        {
          l_prevSampleRate = (m_prevDataHeader.uintSamplerate * m_prevDataHeader.uintInterpolation) / m_prevDataHeader.uintDecimation;
        }
        if (l_sampleRate != l_prevSampleRate)
        {
          if((m_observerAttributes & (uint32)ekSampleRate) == (uint32)ekSampleRate)
          {
            m_attributes[(uint32)ekSampleRate] = l_sampleRate;
          }
          m_dataInfoChanged = true;
        }
      }
      {
        if(dataHeader()->uintStatusword != m_prevDataHeader.uintStatusword)
        {
          if((m_observerAttributes & (uint32)ekStatusword) == (uint32)ekStatusword)
          {
            m_attributes[(uint32)ekStatusword] = dataHeader()->uintStatusword;
          }
          m_dataInfoChanged = true;
        }
      }
      {
        uint64 l_expectedTimestamp = getFirstTimestamp();
        l_expectedTimestamp += (1000000ULL * m_currentSampleIndex) / getSampleRate();
        uint64 l_timestamp = getTimestamp();
        if(l_expectedTimestamp != l_timestamp)
        {
          if (                                             (l_expectedTimestamp + m_timestampDelta) < l_timestamp
              || l_expectedTimestamp > m_timestampDelta && (l_expectedTimestamp - m_timestampDelta) > l_timestamp)
          {
            if((m_observerAttributes & (uint32)ekTimestamp) == (uint32)ekTimestamp)
            {
              m_attributes[(uint32)ekTimestamp] = l_timestamp;
            }
            m_dataInfoChanged = true;
          }
        }
      }
    }
  }

  uint32 CZFFileReaderImpl::getSampleSize() const
  {
    if (!m_ZFFrameHeader.uintMagicWord)
      return 0;

    uint32 l_bytePerSample = 0;
    switch(m_ZFFrameHeader.uintFrameType)
    {
    case ekFRH_DATASTREAM__IFDATA_32RE_32IM_FIX:            { l_bytePerSample = 8;  break; }
    case ekFRH_DATASTREAM__IFDATA_32RE_32IM_FIX_RESCALED:   { l_bytePerSample = 8;  break; }
    case ekFRH_DATASTREAM__IFDATA_16RE_16IM_FIX:            { l_bytePerSample = 4;  break; }
    case ekFRH_DATASTREAM__IFDATA_16RE_16RE_FIX:            { l_bytePerSample = 2;  break; }
    case ekFRH_DATASTREAM__IFDATA_32RE_32IM_FLOAT_RESCALED: { l_bytePerSample = 8;  break; }
    case ekFRH_DATASTREAM__SPECDATA_FLOAT:                  { l_bytePerSample = 4;  break; }
    default:                                                { l_bytePerSample = 1;  break; }
    }
    return l_bytePerSample;
  }

  uint64 CZFFileReaderImpl::getTimestamp() const
  {
    uint64 l_timeStamp = dataHeader()->bigtimeTimeStamp.structTimeInTwoWords.uintTime_HiOrderBits;
    l_timeStamp <<= 32;
    l_timeStamp |= dataHeader()->bigtimeTimeStamp.structTimeInTwoWords.uintTime_LoOrderBits;
    return l_timeStamp;
  }

  int32 CZFFileReaderImpl::getAntennaVoltageRef() const
  {
    return m_lastZFDataHeader.intAntennaVoltageRef;
  }

  uint64 CZFFileReaderImpl::getCenterFrequency() const
  {
    uint64 l_centerFrequency = m_lastZFDataHeader.uintTunerFrequency_High;
    l_centerFrequency <<= 32;
    l_centerFrequency |= m_lastZFDataHeader.uintTunerFrequency_Low;
    return l_centerFrequency;
  }

  uint64 CZFFileReaderImpl::getFirstTimestamp() const
  {
    uint64 l_timeStamp = m_ZFDataHeader.bigtimeTimeStamp.structTimeInTwoWords.uintTime_HiOrderBits;
    l_timeStamp <<= 32;
    l_timeStamp |= m_ZFDataHeader.bigtimeTimeStamp.structTimeInTwoWords.uintTime_LoOrderBits;
    return l_timeStamp;
  }

  uint64 CZFFileReaderImpl::getDuration() const
  {
    if ( !getSampleRate() )
      return 0;

    return uint64(((double)getNumberOfSamples() / (double)getSampleRate()) * 1000000.0 + 0.5);
  }

  uint64 CZFFileReaderImpl::getFirstTimestampNS() const
  {
    if (isDataHeaderEx())
    {
      uint64 l_u64Timestamp = getGoldenSampleTimestampNS();

      uint64 l_u64SampleCounter = m_ZFDataHeader.uintSampleCounter_High;
      l_u64SampleCounter <<= 32;
      l_u64SampleCounter |= m_ZFDataHeader.uintSampleCounter_Low;

      l_u64Timestamp += uint64(((double)l_u64SampleCounter / (double)getSampleRate()) * 1000000000.0 + 0.5);
      return l_u64Timestamp;
    }
    return getFirstTimestamp() * 1000;
  }

  uint64 CZFFileReaderImpl::getDurationNS() const
  {
    if ( !getSampleRate() )
      return 0;

    return uint64(((double)(getNumberOfSamples()) / (double)getSampleRate()) * 1000000000.0 + 0.5);
  }

  uint64 CZFFileReaderImpl::getGoldenSampleTimestampNS() const
  {
    if (isDataHeaderEx())
    {
      uint64 l_timeStamp = dataHeader()->bigtimeStartTimeStamp.structTimeInTwoWords.uintTime_HiOrderBits;
      l_timeStamp <<= 32;
      l_timeStamp |= dataHeader()->bigtimeStartTimeStamp.structTimeInTwoWords.uintTime_LoOrderBits;
      return l_timeStamp;
    }
    return 0;
  }

  uint64 CZFFileReaderImpl::getSampleCounter() const
  {
    if ( !m_ZFFrameHeader.uintMagicWord )
      return 0;

    uint64 l_sampleCounter = dataHeader()->uintSampleCounter_High;
    l_sampleCounter <<= 32;
    l_sampleCounter |= dataHeader()->uintSampleCounter_Low;
    return l_sampleCounter;
  }

  bool CZFFileReaderImpl::isComplex() const
  {
    if ( !m_ZFFrameHeader.uintMagicWord )
      return true;
 
    return m_ZFFrameHeader.uintFrameType != (int)ekFRH_DATASTREAM__IFDATA_16RE_16RE_FIX;
  }

  /* METHOD ***********************************************************/
  /*!
  * @brief  Set the observer interface
  *
  *         The observer interface IIQObserver providing a method to ask
  *         the observer of continuing to read after changes of attributes
  *         (centerfrequency, bandwidth, samplerate, statusword, timestamp).
  *         The observer interface can be set only at file closed status.
  *
  * @param a_pObserver            The observer interface
  * @param a_observerAttributes   Attributes to watch of changes
  * @param a_samplesForTimestamp  Time delta to watch changes of timestamps
  *********************************************************************/
  void CZFFileReaderImpl::setAttributesChangeObserver(IIQObserver* a_pObserver, uint32 a_observerAttributes, uint32 a_samplesForTimestamp)
  {
    if (!isOpen())
    {
      m_pObserver           = a_pObserver;
      m_observerAttributes  = a_observerAttributes;
      if((m_observerAttributes & (uint32)ekTimestamp) == (uint32)ekTimestamp)
      {
        m_samplesForTimestamp = a_samplesForTimestamp;
      }
    }
  }

  //lint -save -e668
  /* METHOD ***********************************************************/
  /*!
  * @brief  Read a number of samples from file into container
  *
  *         Das Einlesen der samples erfolgt frame by frame.
  *         Zun�chst wird der aktuelle Leseindex und die Anzahl der einzulesenden
  *         samples unter Ber�cksichtigung von m_currentSampleIndex,
  *         m_startSampleIndex und m_stopSampleIndex ermittelt.
  *         Nach jedem eingelesenen Frame wird auf �nderung der data header
  *         Attribute gepr�ft und bei Bedarf der Observer benachrichtigt.
  *
  * @param a_Data             Container for samples
  * @param a_elements         Number of samples to read
  * @param a_rDataInfoChanged The out parameter indicates changes within data header
  *
  * @return Error status
  *
  * @see readFrame()
  *********************************************************************/
  eStatus CZFFileReaderImpl::read( ContainerType& a_Data, uint32 a_elements, bool& a_rDataInfoChanged )
  {
    a_rDataInfoChanged = false;

    if (!isOpen())
      return ekFileNotOpen;

    if (!m_ZFFrameHeader.uintMagicWord)
      return ekInvalidFileHeader;

    if (   m_currentSampleIndex > getStopIndex()
        || (::feof(m_pZFFile) != 0) )
    {
      a_Data.setSize( 0 );
      return ekEndOfFile;
    }

    if (a_elements > a_Data.getCapacity())
    {
      if (!a_Data.realloc(a_elements))
      {
        return ekMemoryAllocationError;
      }
    }
    a_Data.setSize( 0u );

    uint32 l_samplesToRead = a_elements;
    uint64 l_stopIndex     = getStopIndex();
    if (l_stopIndex < m_currentSampleIndex )
    {
      return ekEndOfFile;
    }
    if (l_samplesToRead > ((l_stopIndex - m_currentSampleIndex) + 1))
    {
      //lint -save -e712
      l_samplesToRead = (uint32) ((l_stopIndex - m_currentSampleIndex) + 1);
      //lint -restore
    }
    uint32 l_sampleSize   = getSampleSize();
    uint64 l_blockSize    = m_ZFDataHeader.uintDatablockLength;
           l_blockSize   *= m_ZFDataHeader.uintDatablockCount;
           l_blockSize   *= c_sizeof_uint32;
    uint64 l_frame        = m_currentSampleIndex * l_sampleSize;
           l_frame       /= l_blockSize;
    uint64 l_readIndex    = ((m_currentSampleIndex*l_sampleSize) % l_blockSize)/l_sampleSize;
    //lint -save -e713 -e737
    int64  l_frameOffset  = m_firstFrameHeaderPos + (l_frame * m_ZFFrameHeader.uintFrameLength * c_sizeof_uint32);
    //lint -restore

    // Daten einlesen
    do 
    {
      uint64 l_currentSampleIndex = m_currentSampleIndex;

      // Samples aus einem frame einlesen
      if (!readFrame(l_frameOffset, &a_Data, l_samplesToRead - a_Data.getSize(), l_readIndex))
      {
        return setLastError(ekReadFileError, L"Error reading data.");
      }
      a_rDataInfoChanged = m_dataInfoChanged;
      if (a_rDataInfoChanged && m_pObserver)
      {
        if (!m_pObserver->observer(l_currentSampleIndex, m_attributes))
        {
          // Stop reading
          m_currentSampleIndex = m_stopSampleIndex;
          return ekNoError;
        }
      }
      m_attributes.clear();
      m_dataInfoChanged = false;

      // Falls notwendig, Samples aus n�chsten Frame einlesen
      l_frameOffset += ((int64)frameHeader()->uintFrameLength * c_sizeof_uint32);
      l_readIndex    = 0;
    } while (a_Data.getSize() < l_samplesToRead);

    return ekNoError;
  }
  //lint -restore

  /* --- PRIVATE METHODS --- */

  /* METHOD ***********************************************************/
  /*!
  * @brief  Read a frame into data buffer
  *
  *         Ein Frame wird nur eingelesen, wenn er nicht schon vorher
  *         gelesen wurde.
  *         Die aktuellen Frame- und Data-header werden gesichert.
  *         Die eingelesenen Frame- und Data-header werden auf ihre
  *         G�ltigkeit gepr�ft: MagicWord, FrameLength, DataHeaderLength,
  *         DatablockLength, DatablockCount.
  *         Die Header Attribute werden auf �nderungen gepr�ft
  *         Die Daten (samples) des Frames werden vollst�ndig eingelesen
  *         und bei Bedarf (BigEndian) konvertiert (ippsSwapBytes_32u_I()).
  *
  * @param a_offset   File offset of frame
  *
  * @return true = ok, false = error
  *
  * @see getChangedAttributs()
  *********************************************************************/
  bool CZFFileReaderImpl::readZFFrameFromFile(int64 a_offset)
  {
    // reading frame-header
    if (m_dataHeader && m_frameHeaderPos == a_offset)
    {
      return true;
    }
    int64 l_startPos = ::_ftelli64(m_pZFFile);
    if (m_dataHeader && m_frameHeaderPos == l_startPos)
    {
      return true;
    }
    if( a_offset != l_startPos )
    {
      (void)::_fseeki64(m_pZFFile,a_offset,SEEK_SET);
      l_startPos = a_offset;
    }

    memcpy(&m_prevFrameHeader,frameHeader(), c_sizeofZFFrameHeader);
    memcpy(&m_prevDataHeader, dataHeader(),  c_sizeofZFDataHeaderEx);

    m_dataHeader     = false;
    m_frameHeaderPos = a_offset;
    //lint -save -e668
    if (::fread(reinterpret_cast<char*>(m_pZFData), 1, c_sizeofZFFrameHeader, m_pZFFile) != c_sizeofZFFrameHeader)
    //lint -restore
    {
      if (::feof(m_pZFFile) != 0 || ::ferror(m_pZFFile) != 0)
      {
        return false;
      }
    }

    // check the magic word
    typFRH_FRAMEHEADER* l_pZFHeader = frameHeader();
    if (m_endian == ekUnknownEndian)
    {
      if (l_pZFHeader->uintMagicWord != kFRH_MAGIC_WORD)
      {
        if (l_pZFHeader->uintMagicWord != kFRH_MAGIC_WORD_BE) // magic word in BigEndian - actually this indicates an invalid AMMOS-file
        {
          (void)::_fseeki64(m_pZFFile,l_startPos+1,SEEK_SET);
          return syncToNextZFData();
        }
        // swap to little endian
        m_endian = ekBigEndian;
        (void)SwapBytes_uint32_t(reinterpret_cast<uint32_t*>(l_pZFHeader), c_sizeofZFFrameHeader / c_sizeof_uint32);
      }
      else
      {
        m_endian = ekLittleEndian;
      }
    }
    else if (m_endian == ekLittleEndian && l_pZFHeader->uintMagicWord == kFRH_MAGIC_WORD)
    {
    } 
    else if (m_endian == ekBigEndian && l_pZFHeader->uintMagicWord == kFRH_MAGIC_WORD_BE)
    {
      (void)SwapBytes_uint32_t(reinterpret_cast<uint32_t*>(l_pZFHeader), c_sizeofZFFrameHeader / c_sizeof_uint32);
    }
    else
    {
      return false;
    }

    //jumbo frame bit in LSB of uintReserved (jumbo frames are used for pulse IQ data from AMREC)
    bool l_bIsJumboFrame = ((l_pZFHeader->uintStatusword & kFRH_STATUSWORD__LENGTH_MAX_EX_FLAG) == kFRH_STATUSWORD__LENGTH_MAX_EX_FLAG);

    // check for frame length (0x100000) only if it is not a jumbo frame, which can be bigger
    // Plausibility check
    if (  (!l_bIsJumboFrame && (l_pZFHeader->uintFrameLength > kFRH_FRAME_LENGTH_MAX))
        || l_pZFHeader->uintDataHeaderLength > (l_pZFHeader->uintFrameLength - c_sizeofZFFrameHeader / c_sizeof_uint32))
    {
      return false;
    }

    // Check if it is a IF-Frame at all
    if (  l_pZFHeader->uintFrameType != ekFRH_DATASTREAM__IFDATA_32RE_32IM_FIX && 
          l_pZFHeader->uintFrameType != ekFRH_DATASTREAM__IFDATA_16RE_16IM_FIX && 
          l_pZFHeader->uintFrameType != ekFRH_DATASTREAM__IFDATA_16RE_16RE_FIX && 
          l_pZFHeader->uintFrameType != ekFRH_DATASTREAM__IFDATA_32RE_32IM_FIX_RESCALED && 
          l_pZFHeader->uintFrameType != ekFRH_DATASTREAM__IFDATA_32RE_32IM_FLOAT_RESCALED )
      return false;
      
    // reading data-header
    if (::fread(reinterpret_cast<char*>(m_pZFData + c_sizeofZFFrameHeader), 1,l_pZFHeader->uintDataHeaderLength * c_sizeof_uint32,m_pZFFile) != l_pZFHeader->uintDataHeaderLength * c_sizeof_uint32)
    {
      if (::feof(m_pZFFile) != 0 || ::ferror(m_pZFFile) != 0)
      {
        return false;
      }
    }

    typIFD_IFDATAHEADER_EX* l_pDataHeader = dataHeader();
    if (m_endian == ekBigEndian)
    {
      //lint -save -e713
      (void)SwapBytes_uint32_t(reinterpret_cast<uint32_t*>(l_pDataHeader), l_pZFHeader->uintDataHeaderLength);
      //lint -restore
    }

    // check for frame length (0x100000) only if it is not a jumbo frame, which can be bigger
    // Plausibility check
    if(   l_pDataHeader->uintDatablockLength == 0
       || l_pDataHeader->uintDatablockCount  == 0
       || (!l_bIsJumboFrame && (l_pDataHeader->uintDatablockLength > (kFRH_FRAME_LENGTH_MAX * 2))))
    {
      return false;
    }

    if (m_ZFDataLength < l_pZFHeader->uintFrameLength * c_sizeof_uint32)
    {
      realloc();
      l_pZFHeader = frameHeader();
    }

    getChangedAttributs();

    // reading data
    uint32 l_headerSize = c_sizeofZFFrameHeader + l_pZFHeader->uintDataHeaderLength * c_sizeof_uint32;
    if (::fread(reinterpret_cast<char*>(m_pZFData + l_headerSize), 1,l_pZFHeader->uintFrameLength * c_sizeof_uint32 - l_headerSize,m_pZFFile) != l_pZFHeader->uintFrameLength * c_sizeof_uint32 - l_headerSize)
    {
      if (::feof(m_pZFFile) != 0 || ::ferror(m_pZFFile) != 0)
      {
        return false;
      }
    }

    if (m_endian == ekBigEndian)
    {
      //lint -save -e713 -e826
      (void)SwapBytes_uint32_t( (uint32_t*)(m_pZFData + l_headerSize), l_pZFHeader->uintFrameLength - l_headerSize / c_sizeof_uint32);
      //lint -restore
    }

    m_dataHeader = true;

    return true;
  }

  bool CZFFileReaderImpl::syncToNextZFData()
  {
    if (m_pZFFile == 0)
    {
      return false;
    }

    bool l_found = false;
    uint32 l_pos = m_ZFDataLength - c_sizeof_uint32;
    memset(m_pZFData,0,m_ZFDataLength);
    unsigned int l_cnt = 0;
    do
    {
      // sync to correct magic word
      memcpy(m_pZFData,m_pZFData+m_ZFDataLength-c_sizeof_uint32,c_sizeof_uint32);
      if (::fread(reinterpret_cast<char*>(m_pZFData+c_sizeof_uint32), 1, m_ZFDataLength-c_sizeof_uint32, m_pZFFile) != m_ZFDataLength-c_sizeof_uint32)
      {
        if (::feof(m_pZFFile) != 0 || ::ferror(m_pZFFile) != 0)
        {
          return false;
        }
      }

      // find the magic word
      l_pos = m_ZFDataLength - c_sizeof_uint32;
      //lint -save -e954
      for (uint8* l_pData = m_pZFData; l_pData < m_pZFData+m_ZFDataLength-c_sizeof_uint32; l_pData++)
      {
        typFRH_FRAMEHEADER* l_pZFHeader = reinterpret_cast<typFRH_FRAMEHEADER*>(l_pData);
        if (   ((m_endian == ekUnknownEndian || m_endian == ekLittleEndian) && l_pZFHeader->uintMagicWord == kFRH_MAGIC_WORD)
            || ((m_endian == ekUnknownEndian || m_endian == ekBigEndian)    && l_pZFHeader->uintMagicWord == kFRH_MAGIC_WORD_BE) )
        {
          l_pos = m_ZFDataLength - (uint32)(l_pData-m_pZFData);
          l_found = true;
          break;
        }
      }
      //lint -restore

      // Bad idea to search the whole file for a matching magic word. Limit the search to the first ~8M bytes, this should be enough for 
      //                      all AMREC headers (usually 4k) and other fucked up files where up to 8 max-size frames are bullshit
      l_cnt += m_ZFDataLength-c_sizeof_uint32;
      if ( l_cnt > 0x800000 )
        return false;

    } while (!l_found);

    int64 l_filePos = 0;
    l_filePos -= l_pos;
    (void)::_fseeki64(m_pZFFile,l_filePos,SEEK_CUR);
    return readZFFrameFromFile(::_ftelli64(m_pZFFile));
  }

  uint64 CZFFileReaderImpl::getStopIndex() const
  {
    uint64 l_stopIndexMax = getNumberOfSamples();
    if (l_stopIndexMax > 0)
    {
      --l_stopIndexMax;
    }

    if (m_stopSampleIndex >= l_stopIndexMax)
      return l_stopIndexMax;
    else
      return m_stopSampleIndex;
  }

  /* METHOD ***********************************************************/
  /*!
  * @brief  Read the last frame to initialize the properties.
  * 
  *         Um den letzten Frame zu ermitteln wird wie folgt vorgegangen:
  *         Ausgehend von Dateigr�sse, Position des ersten Frames und
  *         der Framel�nge in Bytes wird die max. Anzahl Frames und
  *         die Position des letzten Frames ermittelt.
  *         Nun wird die Position (Fileoffset) des letzten g�ltigen Frames
  *         ermittelt.
  * 
  * @return Error status
  *
  * @see readZFFrameFromFile()
  *********************************************************************/
  eStatus CZFFileReaderImpl::readProperties()
  {
    // Frame count
    int64 l_framLengthInByte = (int64)m_ZFFrameHeader.uintFrameLength * c_sizeof_uint32;
    m_frameCount = (m_fileSize - m_frameHeaderPos) / l_framLengthInByte;
    // last frame
    int64 l_lastFrameOffset = m_frameHeaderPos + (m_frameCount - 1) * l_framLengthInByte;

    // Gibt es einen letzten Frame, der kleiner ist als die errechnete Framl�nge in Bytes?
    int64 l_dataHeaderLength = isDataHeaderEx() ? c_sizeofZFDataHeaderEx : c_sizeofZFDataHeader;
    int64 l_lastFramLengthInByte = (m_fileSize - m_frameHeaderPos) % l_framLengthInByte;
    if (l_lastFramLengthInByte > c_sizeofZFFrameHeader + l_dataHeaderLength)
    {
      m_frameCount++;
      l_lastFrameOffset += l_framLengthInByte;
    }

    // FileAnalyzer - improve the loading of continuous recordings
    /* - the problem is that at the assumed search locations for a frame header there is nothing for a huge amount of
         frames (the file was 59GB) - because after 2% of the file there was a 4K section that shifted the whole frame offset.
         And so no magic word was found searching backwards 58GB. Takes a long time!
       - the problem does not only occur for looped recordings, it may also occur when there are
         changes in the frame size - you won't find a magic word at the given position and you search until the end
         of the first contiguous section.

       Solution: limit the backwards search for the last frame to 
         - 10.000 frames for very big files > 100.000 frames 
         - 50% of the frames for smaller files
         -> use the first frame information as last frame information - this may result in a wrong number of samples
            and a slightly wrong duration but we don't have any other chance.
    */
    int64 l_initialMaxFrameCount = m_frameCount;
    int64 l_maxFramesCheck = l_initialMaxFrameCount < 100000 ? (int64) (0.5 * l_initialMaxFrameCount) : 10000;
    bool   l_validLastFrame = true;

    while (m_frameCount)
    {
      if (!readFrame(l_lastFrameOffset))
      {
        m_frameCount--;
        l_lastFrameOffset -= l_framLengthInByte;
      }
      else
      {
        const typFRH_FRAMEHEADER* l_pZFFrameHeader = frameHeader();
        if (l_pZFFrameHeader->uintFrameType != m_ZFFrameHeader.uintFrameType )
        {
          m_frameCount--;
          l_lastFrameOffset -= l_framLengthInByte;
        }
        else
        {
          break;
        }
      }

      // RTC Defect#147654: FileAnalyzer - improve the loading of continuous recordings
      if ( l_initialMaxFrameCount - m_frameCount > l_maxFramesCheck )
      {
        l_validLastFrame = false;
        memcpy(&m_lastZFFrameHeader, &m_ZFFrameHeader, c_sizeofZFFrameHeader);
        memset(&m_lastZFDataHeader,  0,                c_sizeofZFDataHeaderEx);
        memcpy(&m_lastZFDataHeader,  &m_ZFDataHeader,  c_sizeofZFDataHeaderEx);
        break;
      }
    }

    if ( l_validLastFrame )
    {
      // Frame header
      memcpy(&m_lastZFFrameHeader, frameHeader(), c_sizeofZFFrameHeader);
      // Data header
      memset(&m_lastZFDataHeader, 0,            c_sizeofZFDataHeaderEx);
      memcpy(&m_lastZFDataHeader, dataHeader(), c_sizeofZFDataHeaderEx);
    }

    if (!isDataHeaderEx())
    {
      m_lastZFDataHeader.bigtimeStartTimeStamp.structTimeInTwoWords.uintTime_HiOrderBits = 0;
      m_lastZFDataHeader.bigtimeStartTimeStamp.structTimeInTwoWords.uintTime_LoOrderBits = 0;
      m_lastZFDataHeader.uintSampleCounter_High = 0;
      m_lastZFDataHeader.uintSampleCounter_Low  = 0;
      //lint -save -e569
      m_lastZFDataHeader.intKFactor = kIFD_HEADER_KFACTOR__UNDEFINED;
      //lint -restore
    }
    memcpy(&m_prevFrameHeader,&m_lastZFFrameHeader, c_sizeofZFFrameHeader);
    memcpy(&m_prevDataHeader, &m_lastZFDataHeader,  c_sizeofZFDataHeaderEx);

    m_currentSampleIndex = m_startSampleIndex;

    return ekNoError;
  }

  //lint -save -e826
  /* METHOD ***********************************************************/
  /*!
  * @brief  Read a number of samples from file into container
  *
  *         Der aktuelle Frame inkl. data header und Daten wird vollst�ndig
  *         eingelesen. Die Daten (samples) werden data block by data block
  *         eingelesen.
  *         Zun�chst wird die Start-Leseposition ermittelt.
  *         In einer Schleife werden bis zu max. alle Samples eines data blocks
  *         in den Container �bertragen und angepasst.
  *
  * @param a_frameOffset      Fileoffset of the frame
  * @param a_pData            Container for samples
  * @param a_samplesToRead    Number of samples to read
  * @param a_starSampleIndex  Startindex to the sample within the actual frame
  *
  * @return true = ok, false = error
  *
  * @see adjustData()
  *********************************************************************/
  bool CZFFileReaderImpl::readFrame(int64 a_frameOffset, ContainerType* a_pData, uint32 a_samplesToRead, uint64 a_starSampleIndex)
  {
    // read ZF dataheader
    if ( !readZFFrameFromFile(a_frameOffset) )
    {
      (void)setLastError(ekReadFileError, L"Error while reading.");
      return false;
    }

    // reading samples?
    if (a_pData && a_pData->getSize() < a_pData->getCapacity())
    {
      // Daten aus Datenblock in Array einlesen
      uint32 l_sampleSize = getSampleSize();
      uint32 l_outFactor  = (c_sizeof_uint32 * 2)/l_sampleSize;
      uint32 l_blockSize  = dataHeader()->uintDatablockLength * c_sizeof_uint32;
      uint64 l_blockIndex = (a_starSampleIndex * l_sampleSize) / l_blockSize;
      a_starSampleIndex  -= (l_blockSize/l_sampleSize)*l_blockIndex;

      uint8* l_pBuffer = reinterpret_cast<uint8*>(dataHeader());
      l_pBuffer += frameHeader()->uintDataHeaderLength * c_sizeof_uint32;
      //lint -save -e776
      l_pBuffer += l_blockIndex * (l_blockSize + c_sizeof_uint32);
      //lint -restore
      uint8* l_pDstData = (uint8*)a_pData->getPtr();
      l_pDstData += a_pData->getSize() * l_sampleSize * l_outFactor;
      uint64 l_toReadBytes = (uint64)a_samplesToRead * l_sampleSize;
      do
      {
        uint32 l_statusword = reinterpret_cast<typIFD_DATABLOCK*>(l_pBuffer)->datablockheaderDatablockHeader.uintStatusword;
        l_pBuffer += c_sizeof_uint32;

        uint64 l_readOffset = a_starSampleIndex * l_sampleSize;
        uint64 l_readLeft   = (uint64)dataHeader()->uintDatablockLength * c_sizeof_uint32 - l_readOffset;
        uint64 l_readBytes  = __min(l_toReadBytes, l_readLeft);

        if (!adjustData(l_pBuffer + l_readOffset, l_pDstData, l_readBytes, l_statusword))
        {
          return false;
        }

        l_pDstData += l_readBytes * l_outFactor;
        l_pBuffer  += l_blockSize;

        l_toReadBytes -= l_readBytes;
        l_blockIndex++;
        a_starSampleIndex = 0;
      } while (l_toReadBytes && l_blockIndex < dataHeader()->uintDatablockCount);

      //lint -save -e712
      uint32 l_samplesReaded = (int32) (a_samplesToRead - l_toReadBytes/l_sampleSize);
      //lint -restore
      a_pData->setSize(a_pData->getSize() + l_samplesReaded);
      m_currentSampleIndex += l_samplesReaded;
    }

    return true;
  }
  //lint -restore

  //lint -save -e826 -e1762
  /* METHOD ***********************************************************/
  /*!
  * @brief  Copy a number of samples from source-buffer into container
  *
  *         Die Daten (samples) werden bei der entsprechenden (IPP)
  *         Anpassung in den Container kopiert. Die Anpassung erfolgt
  *         unter Ber�cksichtigung von RecipGain und AntennaVoltageRef.
  *
  * @param a_pSource        Source buffer
  * @param a_pDestination   Container for samples
  * @param a_bytesToRead    Number of samples to trnasfer
  * @param a_statusword     Statusword with the reciprocal gain correction value
  *********************************************************************/
  bool CZFFileReaderImpl::adjustData(uint8* a_pSource, uint8* a_pDestination, uint64 a_bytesToRead, uint32 a_statusword)
  {
    Status l_status = StsNoErr;
    float* l_pfDestination = reinterpret_cast<float*>(a_pDestination);

    //*** Rescale of AntennaVoltageReference [dBmicroV] to a voltage value [V]
    float fAntennaVoltageRef = c_fMicroVolt2Volt * powf(10.0F, static_cast<float>(dataHeader()->intAntennaVoltageRef) * 0.005F);
    float fRecipGain         = static_cast<float>(a_statusword>>16) * kf16BitReciMax;
    float fFactor            = fRecipGain * fAntennaVoltageRef;

    switch( m_ZFFrameHeader.uintFrameType )
    { //***************************************************************************
      case ekFRH_DATASTREAM__IFDATA_16RE_16IM_FIX:   
      {
        //lint -save -e712
        int32 l_size = (int32) (a_bytesToRead / c_sizeof_uint16);
        //lint -restore
        l_status = Convert_int16_t_float(reinterpret_cast<int16*>(a_pSource), l_pfDestination, l_size);
        if(l_status == StsNoErr)
          l_status = MulC_float(kfSHORTMAX * fFactor, l_pfDestination, l_size);
        break;
      }

      //***************************************************************************
      case ekFRH_DATASTREAM__IFDATA_32RE_32IM_FLOAT_RESCALED:
      {
        //lint -save -e712
        int32 l_size = (int32) (a_bytesToRead / c_sizeof_uint32);
        //lint -restore
        l_status = MulC_float(reinterpret_cast<float*>(a_pSource), fFactor, l_pfDestination, l_size);
        break;
      } 

      //***************************************************************************
      case ekFRH_DATASTREAM__IFDATA_32RE_32IM_FIX: 
      {
        //lint -save -e712
        int32 l_size = (int32) (a_bytesToRead / c_sizeof_uint32);
        //lint -restore
        l_status = Convert_int32_t_float(reinterpret_cast<int32*>(a_pSource), l_pfDestination, l_size);
        if(l_status == StsNoErr)   
          l_status = MulC_float(kf31BitReciMax * fFactor, l_pfDestination, l_size);
        break;
      }

      //***************************************************************************
      case ekFRH_DATASTREAM__IFDATA_32RE_32IM_FIX_RESCALED:  
      {
        //lint -save -e712
        int32 l_size = (int32) (a_bytesToRead / c_sizeof_uint32);
        //lint -restore
        l_status = Convert_int32_t_float(reinterpret_cast<int32*>(a_pSource), l_pfDestination, l_size);
        if(l_status == StsNoErr)   
          l_status = MulC_float(kf31BitReciMax * fFactor, l_pfDestination, l_size);
        break;
      }

      //***************************************************************************
      //lint -save -e954
      case ekFRH_DATASTREAM__IFDATA_16RE_16RE_FIX:  
      {
        //lint -save -e712
        int32 l_size      = (int32) a_bytesToRead / 2;
        //lint -restore
        int16* l_pSource  = reinterpret_cast<int16*>(a_pSource);
        for (int32 ulCounter = 0; ulCounter < l_size; ulCounter++)
        {
          l_pfDestination[ulCounter * 2    ] = kfSHORTMAX * fFactor * static_cast<float>(l_pSource[ulCounter]);
          l_pfDestination[ulCounter * 2 + 1] = 0.0F;
        }
        break;
      }
      //lint -restore

      //***************************************************************************
      default:
      {
        return false;
      }
    }

    if(l_status != StsNoErr)   
    {
      std::ostringstream l_ss;
      l_ss << "Code = " << (int)l_status << " " << ippGetStatusString(l_status);
      CErrCtrl::Instance().error(CErrCode::kERR_FILES_IPP_ERROR, __LINE__,__FILE__, l_ss.str());
      return false;
    }
    return true;
  }
  //lint -restore

  void CZFFileReaderImpl::realloc()
  {
    typFRH_FRAMEHEADER l_ZFFrameHeader;
    memcpy(&l_ZFFrameHeader, frameHeader(), c_sizeofZFFrameHeader);
    typIFD_IFDATAHEADER_EX l_ZFDataHeader;
    memset(&l_ZFDataHeader,  0,             c_sizeofZFDataHeaderEx);
    memcpy(&l_ZFDataHeader,  dataHeader(),  l_ZFFrameHeader.uintDataHeaderLength * c_sizeof_uint32);

    uint32 l_ZFDataLength = m_ZFDataLength;
    m_ZFDataLength = l_ZFFrameHeader.uintFrameLength * c_sizeof_uint32;
    uint8* l_pZFData = m_pZFData;

    m_pZFData = new uint8[m_ZFDataLength];
    memcpy(m_pZFData, l_pZFData, l_ZFDataLength);

    delete[] l_pZFData;
  }

  eStatus CZFFileReaderImpl::setLastError( eStatus a_newState, const std::wstring& a_strErrorMsg )
  {
    m_strErrorMsg = a_strErrorMsg;
    m_eStatus     = a_newState;
    return m_eStatus;
  }

  uint32 CZFFileReaderImpl::getSampleRate() const
  {
    if (m_ZFDataHeader.uintDecimation)
    {
      return (m_ZFDataHeader.uintSamplerate * m_ZFDataHeader.uintInterpolation) / m_ZFDataHeader.uintDecimation;
    }
    return m_ZFDataHeader.uintSamplerate;
  }

  uint32 CZFFileReaderImpl::getBandwidth() const
  {
    return m_lastZFDataHeader.uintBandwidth;
  }

  uint64 CZFFileReaderImpl::getNumberOfSamples() const
  {
    if (getSampleSize() == 0 || m_frameCount <= 0)
      return 0;

    uint64 l_blockSize       = m_ZFDataHeader.uintDatablockLength;
           l_blockSize      *= m_ZFDataHeader.uintDatablockCount;
           l_blockSize      *= c_sizeof_uint32;
    uint64 l_dataSizeInByte  = l_blockSize * (uint64)(m_frameCount-1);
           l_blockSize       = m_lastZFDataHeader.uintDatablockLength;
           l_blockSize      *= m_lastZFDataHeader.uintDatablockCount;
           l_blockSize      *= c_sizeof_uint32;
           l_dataSizeInByte += l_blockSize;

    return l_dataSizeInByte / getSampleSize();
  }

}

/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/
