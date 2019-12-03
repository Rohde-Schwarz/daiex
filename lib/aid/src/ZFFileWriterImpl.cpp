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

#include "ErrCtrl.h"
#include "ZFFileWriterImpl.h"
#include <cmath>
#include <cstring>

/* STATIC INITIALIZATION **********************************************************/

/* ------------------------------------------------------------------------------ */

using namespace std;
using namespace AmlabCommon;

#define ACCURATE_TIMESTAMP

namespace AmlabFiles
{
  /*! Size of frame header in bytes */
  const int32 c_sizeofFrameHeader    = sizeof(typFRH_FRAMEHEADER);
  /*! Size of data header in bytes */
  const int32 c_sizeofZFDataHeader   = sizeof(typIFD_IFDATAHEADER);
  /*! Size of extension data header in bytes */
  const int32 c_sizeofZFDataHeaderEx = sizeof(typIFD_IFDATAHEADER_EX);

  /* --- PUBLIC METHODS --- */

  CZFFileWriterImpl::CZFFileWriterImpl()
    : m_strFilename()
    , m_pZFFile(0)
    , m_flushOnClose(true)
    , m_dataHeaderEx(true)
    , m_pZFData(0)
    , m_bufferSamplesMax(0)
    , m_bufferSamples(0)
    , m_strErrorMsg()
    , m_eStatus(ekNoError)
  {
    memset(&m_ZFFrameHeader,0,sizeof(m_ZFFrameHeader));
    m_ZFFrameHeader.uintMagicWord        = kFRH_MAGIC_WORD;
    //m_ZFFrameHeader.uintFrameLength;  // kFRH_FRAME_LENGTH_MAX
    m_ZFFrameHeader.uintFrameCount       = 0;
    m_ZFFrameHeader.uintFrameType        = (int)ekFRH_DATASTREAM__IFDATA_32RE_32IM_FLOAT_RESCALED;
    m_ZFFrameHeader.uintDataHeaderLength = c_sizeofZFDataHeaderEx / c_sizeof_uint32;

    memset(&m_ZFDataHeader,0,sizeof(m_ZFDataHeader));
    m_ZFDataHeader.uintSamplerate       = 1;
    m_ZFDataHeader.uintInterpolation    = 1;
    m_ZFDataHeader.uintDecimation       = 1;
    m_ZFDataHeader.intAntennaVoltageRef = 1200;
    m_ZFDataHeader.uintDatablockCount   = 1;
    m_ZFDataHeader.uintDatablockLength  = getSamplesPerBlockMax();
    //lint -save -e569
    m_ZFDataHeader.intKFactor           = kIFD_HEADER_KFACTOR__UNDEFINED;
    //lint -restore
    m_recipGain                         = c_recipGain;
  }

  CZFFileWriterImpl::CZFFileWriterImpl(const std::wstring& a_strFilename)
    : m_strFilename(a_strFilename)
    , m_pZFFile(0)
    , m_flushOnClose(true)
    , m_dataHeaderEx(true)
    , m_pZFData(0)
    , m_bufferSamplesMax(0)
    , m_bufferSamples(0)
    , m_strErrorMsg()
    , m_eStatus(ekNoError)
  {
    memset(&m_ZFFrameHeader,0,sizeof(m_ZFFrameHeader));
    m_ZFFrameHeader.uintMagicWord        = kFRH_MAGIC_WORD;
    //m_ZFFrameHeader.uintFrameLength;  // kFRH_FRAME_LENGTH_MAX
    m_ZFFrameHeader.uintFrameCount       = 0;
    m_ZFFrameHeader.uintFrameType        = (int)ekFRH_DATASTREAM__IFDATA_32RE_32IM_FLOAT_RESCALED;
    m_ZFFrameHeader.uintDataHeaderLength = c_sizeofZFDataHeaderEx / c_sizeof_uint32;

    memset(&m_ZFDataHeader,0,sizeof(m_ZFDataHeader));
    m_ZFDataHeader.uintSamplerate       = 1;
    m_ZFDataHeader.uintInterpolation    = 1;
    m_ZFDataHeader.uintDecimation       = 1;
    m_ZFDataHeader.intAntennaVoltageRef = 1200;
    m_ZFDataHeader.uintDatablockCount   = 1;
    m_ZFDataHeader.uintDatablockLength  = getSamplesPerBlockMax();
    //lint -save -e569
    m_ZFDataHeader.intKFactor           = kIFD_HEADER_KFACTOR__UNDEFINED;
    //lint -restore
    m_recipGain                         = c_recipGain;
  }

  CZFFileWriterImpl::~CZFFileWriterImpl()
  {
    if (m_pZFFile != 0)
    {
      ::fclose(m_pZFFile);
      m_pZFFile = 0;
    }
    delete[] m_pZFData;
  }

  bool CZFFileWriterImpl::isOpen() const
  {
    return (m_pZFFile != 0);
  }

  eStatus CZFFileWriterImpl::open()
  {
    if (isOpen())
      return ekFileAlreadyOpen;

    if (m_strFilename.empty())
    {
      return setLastError(ekNoFileSpecified, L"Filename not specified.");
    }

    switch ( m_ZFFrameHeader.uintFrameType )
    {
    case ekFRH_DATASTREAM__IFDATA_16RE_16IM_FIX:
    case ekFRH_DATASTREAM__IFDATA_32RE_32IM_FIX:
    case ekFRH_DATASTREAM__IFDATA_32RE_32IM_FIX_RESCALED:
    case ekFRH_DATASTREAM__IFDATA_32RE_32IM_FLOAT_RESCALED:
      break;
    default:
      return ekInvalidParameters;
    }

    // check for valid datablock settings
    if (m_ZFDataHeader.uintDatablockLength > getSamplesPerBlockMax())
      return ekInvalidParameters;

#ifdef _WIN32
    errno_t l_errno = ::_wfopen_s(&m_pZFFile, m_strFilename.c_str(), L"wb" );
    if (l_errno != 0 || !isOpen())
    {
      return setLastError(ekWriteFileOpenError, L"File could not be opened for writing.");
    }
#else
    const std::string s( m_strFilename.begin(), m_strFilename.end() );
    m_pZFFile = fopen(s.c_str(), "wb");
    if (m_pZFFile == 0)
    {
      return setLastError(ekWriteFileOpenError, L"File could not be opened for writing.");
    }
#endif    

    uint32 l_samplesPerBlock = (m_ZFDataHeader.uintDatablockLength * c_sizeof_uint32) / getSampleSize();
    m_bufferSamplesMax = l_samplesPerBlock * m_ZFDataHeader.uintDatablockCount;
    m_bufferSamples    = 0;

    uint32 l_bufferSize = m_bufferSamplesMax * c_sizeof_uint32 * 2;
    delete[] m_pZFData;
    m_pZFData = new uint8[l_bufferSize];

    setFrameLength();

    return ekNoError;
  }

  eStatus CZFFileWriterImpl::close()
  {
    if (!isOpen())    // File not opened
      return ekFileNotOpen;

    flush();

    eStatus l_eRetValue = ekNoError;
    if (m_pZFFile != 0)
    {
      if (::fclose(m_pZFFile))
      {
        l_eRetValue = ekEndOfFile;
      }
      m_pZFFile = 0;
    }

    delete[] m_pZFData;
    m_pZFData = 0;

    return l_eRetValue;
  }

  /* METHOD ***********************************************************/
  /*!
  * @brief  Write a number of samples to file
  *
  *         Die Anzahl samples aus dem Container werden frame by frame
  *         in die Datei geschrieben.
  *         Vor dem Schreiben werden die samples angepasst (convert())
  *         und in Data buffer �bertragen.
  *         Nur vollst�ndige Frames werden geschrieben. Ist ein
  *         unvollst�ndiger (letzter) Frame beim Schliessen der
  *         Datei vorhanden, wird dieser nur geschrieben, wenn das Flag
  *         m_flushOnClose gesetzt ist.
  *
  * @param a_rData            Container with samples
  * @param a_elements         Number of samples to write
  *
  * @return Error status
  *
  * @see convert(), writeFrame(), flush()
  *********************************************************************/
  AmlabFiles::eStatus CZFFileWriterImpl::write( const ContainerType& a_rData, uint32 a_elements )
  {
    if (!isOpen())
      return ekFileNotOpen;

    const CplxType* l_pfcData = a_rData.getPtr();
    if (l_pfcData == 0 || m_pZFData == 0 || a_rData.getSize() < a_elements)
      return ekInvalidParameters;

    uint32 l_dataOffset      = 0;
    uint32 l_sampleSize      = getSampleSize();
    uint32 l_samplesPerBlock = (m_ZFDataHeader.uintDatablockLength * c_sizeof_uint32) / l_sampleSize;

    int32 l_samplesToCopy;
    while (a_elements)
    {
      uint32 l_samples = (a_elements > (m_bufferSamplesMax - m_bufferSamples)) ? (m_bufferSamplesMax - m_bufferSamples) : a_elements;
      //lint -save -e713
      l_samplesToCopy = (l_samplesPerBlock > l_samples) ? l_samples : l_samplesPerBlock;
      //lint -restore

      if (!convert((const float*)&l_pfcData[l_dataOffset], &m_pZFData[m_bufferSamples * l_sampleSize], 2 * l_samplesToCopy))
      {
        return ekWriteFileError;
      }

      //lint -save -e737
      a_elements      -= l_samplesToCopy;
      l_dataOffset    += l_samplesToCopy;
      m_bufferSamples += l_samplesToCopy;
      //lint -restore

      if (m_bufferSamples == m_bufferSamplesMax)
      {
        uint32 l_u32Dwords = (m_bufferSamples * l_sampleSize) / c_sizeof_uint32;
        writeFrame(m_pZFData, l_u32Dwords);
        m_bufferSamples = 0;
      }
    }

    return ekNoError;
  }

  void CZFFileWriterImpl::setDataHeaderEx( bool a_dataHeaderEx )
  {
    if (!isOpen())
    {
      m_dataHeaderEx = a_dataHeaderEx;
      m_ZFFrameHeader.uintDataHeaderLength  = m_dataHeaderEx ? c_sizeofZFDataHeaderEx : c_sizeofZFDataHeader;
      m_ZFFrameHeader.uintDataHeaderLength /= c_sizeof_uint32;
    }
  }

  void CZFFileWriterImpl::setFrameType( uint32 a_eFrameType )
  {
    if (!isOpen())
    {
      uint32 l_samplesPerBlock = (m_ZFDataHeader.uintDatablockLength * c_sizeof_uint32) / getSampleSize();
      m_ZFFrameHeader.uintFrameType = a_eFrameType;
      setDatablockSettings( l_samplesPerBlock, m_ZFDataHeader.uintDatablockCount );
    }
  }

  void CZFFileWriterImpl::setSampleCounter( uint64 a_sampleCounter )
  {
    m_ZFDataHeader.uintSampleCounter_High = static_cast<uint32>((a_sampleCounter & 0xFFFFFFFF00000000)>>32);
    m_ZFDataHeader.uintSampleCounter_Low  = static_cast<uint32>( a_sampleCounter & 0x00000000FFFFFFFF);
  }

  void CZFFileWriterImpl::setDatablockSettings( uint32 a_samplesPerBlock, uint32 a_dataBlockCount )
  {
    if (!isOpen())
    {
      m_ZFDataHeader.uintDatablockLength = a_samplesPerBlock * getSampleSize() / c_sizeof_uint32;
      m_ZFDataHeader.uintDatablockLength = m_ZFDataHeader.uintDatablockLength == 0 ? getSamplesPerBlockMax() :m_ZFDataHeader.uintDatablockLength;
      m_ZFDataHeader.uintDatablockCount  = a_dataBlockCount > 0 ? a_dataBlockCount : 1;
    }
  }

  uint32 CZFFileWriterImpl::getSampleRate() const
  {
    if (m_ZFDataHeader.uintDecimation)
    {
      return (m_ZFDataHeader.uintSamplerate * m_ZFDataHeader.uintInterpolation) / m_ZFDataHeader.uintDecimation;
    }
    return m_ZFDataHeader.uintSamplerate;
  }

  eStatus CZFFileWriterImpl::getLastError( std::wstring& a_strErrorMsg )
  {
    a_strErrorMsg = m_strErrorMsg;
    m_strErrorMsg.clear();
    eStatus l_lastError = m_eStatus;
    m_eStatus = ekNoError;

    return l_lastError;
  }

  /* --- PRIVATE METHODS --- */

  eStatus CZFFileWriterImpl::setLastError( eStatus a_newState, const std::wstring& a_strErrorMsg )
  {
    m_strErrorMsg = a_strErrorMsg;
    m_eStatus     = a_newState;
    return m_eStatus;
  }

  uint32 CZFFileWriterImpl::getSampleSize() const
  {
    switch (m_ZFFrameHeader.uintFrameType)
    {
    //case ekFRH_DATASTREAM__IFDATA_16RE_16RE_FIX:
    //  return sizeof(short);
    case ekFRH_DATASTREAM__IFDATA_16RE_16IM_FIX:
      return 2 * sizeof(short);
    case ekFRH_DATASTREAM__IFDATA_32RE_32IM_FIX:
    case ekFRH_DATASTREAM__IFDATA_32RE_32IM_FIX_RESCALED:
      return 2 * sizeof(int);
    case ekFRH_DATASTREAM__IFDATA_32RE_32IM_FLOAT_RESCALED:
      return 2 * sizeof(float);
    default:
      AC_ASSERT( false );
      return c_sizeof_uint32;
    }
  }

  void CZFFileWriterImpl::setTimestamp( uint64 a_timestamp )
  {
    m_ZFDataHeader.bigtimeTimeStamp.structTimeInTwoWords.uintTime_HiOrderBits = static_cast<uint32>((a_timestamp & 0xFFFFFFFF00000000)>>32);
    m_ZFDataHeader.bigtimeTimeStamp.structTimeInTwoWords.uintTime_LoOrderBits = static_cast<uint32>( a_timestamp & 0x00000000FFFFFFFF);
    if (!isOpen())
    { // do not change the nanoseconds after starting writing, because it will remain stable
      a_timestamp *= 1000;
      m_ZFDataHeader.bigtimeStartTimeStamp.structTimeInTwoWords.uintTime_HiOrderBits = static_cast<uint32>((a_timestamp & 0xFFFFFFFF00000000)>>32);
      m_ZFDataHeader.bigtimeStartTimeStamp.structTimeInTwoWords.uintTime_LoOrderBits = static_cast<uint32>( a_timestamp & 0x00000000FFFFFFFF);
    }
  }

  void CZFFileWriterImpl::setFirstStartTimestampNS( uint64 a_firstStartTimestampNS )
  {
    m_ZFDataHeader.bigtimeStartTimeStamp.structTimeInTwoWords.uintTime_HiOrderBits = static_cast<uint32>((a_firstStartTimestampNS & 0xFFFFFFFF00000000)>>32);
    m_ZFDataHeader.bigtimeStartTimeStamp.structTimeInTwoWords.uintTime_LoOrderBits = static_cast<uint32>( a_firstStartTimestampNS & 0x00000000FFFFFFFF);
    a_firstStartTimestampNS /= 1000;
    m_ZFDataHeader.bigtimeTimeStamp.structTimeInTwoWords.uintTime_HiOrderBits = static_cast<uint32>((a_firstStartTimestampNS & 0xFFFFFFFF00000000)>>32);
    m_ZFDataHeader.bigtimeTimeStamp.structTimeInTwoWords.uintTime_LoOrderBits = static_cast<uint32>( a_firstStartTimestampNS & 0x00000000FFFFFFFF);
  }

  uint64 CZFFileWriterImpl::getTimestamp() const
  {
    uint64 l_timestamp = m_ZFDataHeader.bigtimeTimeStamp.structTimeInTwoWords.uintTime_HiOrderBits;
    l_timestamp <<= 32;
    l_timestamp |= m_ZFDataHeader.bigtimeTimeStamp.structTimeInTwoWords.uintTime_LoOrderBits;
    return l_timestamp;
  }

  uint64 CZFFileWriterImpl::getSampleCounter() const
  {
    uint64 l_sampleCounter = m_ZFDataHeader.uintSampleCounter_High;
    l_sampleCounter <<= 32;
    l_sampleCounter |= m_ZFDataHeader.uintSampleCounter_Low;
    return l_sampleCounter;
  }

  uint32 CZFFileWriterImpl::getDatablockSizeMax() const
  {
    int32 l_sizeofZFDataHeader = c_sizeofZFDataHeader;
    if (m_dataHeaderEx)
    {
      l_sizeofZFDataHeader = c_sizeofZFDataHeaderEx;
    }
    int32 l_datablockSizeMax  = kFRH_FRAME_LENGTH_MAX;                    // max. allowed frame length in ints
          l_datablockSizeMax -= c_sizeofFrameHeader / c_sizeof_uint32;  // subtract the frame header in ints
          l_datablockSizeMax -= l_sizeofZFDataHeader  / c_sizeof_uint32;  // subtract the data header in ints
          //lint -save -e713 -e737
          l_datablockSizeMax -= m_ZFDataHeader.uintDatablockCount;        // subtract the data block headers count
          //lint -restore

    if(l_datablockSizeMax < 0)
    {
      return 0;
    }

    return ((uint32)l_datablockSizeMax / m_ZFDataHeader.uintDatablockCount); // maximum ints per block in the frame
  }

  void CZFFileWriterImpl::setFrameLength()
  {
    m_ZFFrameHeader.uintFrameLength  = c_sizeofFrameHeader;
    m_ZFFrameHeader.uintFrameLength += m_dataHeaderEx ? c_sizeofZFDataHeaderEx : c_sizeofZFDataHeader;
    m_ZFFrameHeader.uintFrameLength /= c_sizeof_uint32;
    m_ZFFrameHeader.uintFrameLength += m_ZFDataHeader.uintDatablockCount * (m_ZFDataHeader.uintDatablockLength + 1);  // 1 = Datablock-Statusword
  }

  uint32 CZFFileWriterImpl::getSamplesPerBlockMax() const
  {
    uint32 l_datablockSizeMax    = getDatablockSizeMax() * c_sizeof_uint32;
    uint32 l_samplesPerBlockMax  = l_datablockSizeMax / getSampleSize();

    return l_samplesPerBlockMax;
  }

  //lint -save -e826
  /* METHOD ***********************************************************/
  /*!
  * @brief  Write a complete frame with samples to file
  *
  *         Ein vollst�ndiger Frame (Frame header, data header, data blocks)
  *         wird data block by data block in die Datei geschrieben.
  *         Nach dem Schreiben werden Timestamp und Samplecounter angepasst.
  *
  * @param a_data             Buffer with samples
  * @param a_elements         Number of samples to write
  *
  *********************************************************************/
  void CZFFileWriterImpl::writeFrame( const uint8* a_data, uint32 a_elements )
  {
    if (0 == a_elements)
      return;

    *this << m_ZFFrameHeader;

    // I don't know why we did override the status word in any case, but ist stupid
    // -> only set the flag if required but do not override any other set flag
    // <-- previous code
    //m_ZFDataHeader.uintStatusword = m_ZFDataHeader.intAntennaVoltageRef ? 0 : kIFD_HEADER_STATUSWORD__DBFS_FLAG;
    // --> new code
    if ( m_ZFDataHeader.intAntennaVoltageRef == 0 )
      m_ZFDataHeader.uintStatusword |= kIFD_HEADER_STATUSWORD__DBFS_FLAG;
    
    
    if(m_dataHeaderEx)
    {
      *this << &m_ZFDataHeader;
    }
    else
    {
      *this << reinterpret_cast<char*>(&m_ZFDataHeader);
    }

    uint32 l_datablockSize       = m_ZFDataHeader.uintDatablockLength;
    uint32 l_dataBlockStatusword = static_cast<uint32>( m_recipGain)<<16;
    const uint32* l_pu32Data = (const uint32 *)a_data;
    for (uint32 i = 0; i < m_ZFDataHeader.uintDatablockCount; i++)
    {
      ::fwrite(reinterpret_cast<const char*>(&l_dataBlockStatusword), 1, c_sizeof_uint32, m_pZFFile);
      if (a_elements < m_ZFDataHeader.uintDatablockLength)
      {
        l_datablockSize = a_elements;
      }
      ::fwrite(reinterpret_cast<const char*>(l_pu32Data), 1, l_datablockSize * c_sizeof_uint32, m_pZFFile);
      l_pu32Data += m_ZFDataHeader.uintDatablockLength;
      a_elements -= l_datablockSize;
    }

    // increase timestamp
    //lint -save -e647
    uint64 l_sampleFrameCount = (m_ZFDataHeader.uintDatablockLength * m_ZFDataHeader.uintDatablockCount * c_sizeof_uint32) / getSampleSize();
    //lint -restore
    // increase framecount
    m_ZFFrameHeader.uintFrameCount++;
    // increase samplecount
    setSampleCounter(getSampleCounter() + l_sampleFrameCount);

#ifdef ACCURATE_TIMESTAMP
    // A. Frick 20.2.2013: Accurate time stamp not ignoring fractional part of increment, but limited to file length of 
    uint64 l_highResTS = m_ZFDataHeader.bigtimeStartTimeStamp.structTimeInTwoWords.uintTime_HiOrderBits;
    l_highResTS <<= 32;
    l_highResTS  |= m_ZFDataHeader.bigtimeStartTimeStamp.structTimeInTwoWords.uintTime_LoOrderBits;
    double l_accumulatedSampleTime = getSampleCounter() * ( 1000000000.0 / getSampleRate() ); // time in nanoseconds that will be added to first time stamp
    setTimestamp( (l_highResTS + static_cast<uint64>(l_accumulatedSampleTime)) / 1000 );
#else
    uint64 l_timestamp = getTimestamp();
    l_timestamp += (1000000u * l_sampleFrameCount) / getSampleRate();
    setTimestamp(l_timestamp);
#endif
  }
  //lint -restore

  void CZFFileWriterImpl::flush()
  {
    if(m_flushOnClose && m_bufferSamples > 0)
    {
      m_ZFDataHeader.uintDatablockLength = (m_bufferSamples * getSampleSize()) / c_sizeof_uint32;
      m_ZFDataHeader.uintDatablockCount  = 1;
      setFrameLength();
      writeFrame(m_pZFData, m_ZFDataHeader.uintDatablockLength);
    }
  }

  //lint -save -e668
  CZFFileWriterImpl& CZFFileWriterImpl::operator<<( const typFRH_FRAMEHEADER& a_frameHeader )
  {
    ::fwrite(reinterpret_cast<const char*>(&a_frameHeader), 1, c_sizeofFrameHeader, m_pZFFile);
    return (*this);
  }
  CZFFileWriterImpl& CZFFileWriterImpl::operator<<( const char* const a_pDataHeader )
  {
    ::fwrite(a_pDataHeader, 1, c_sizeofZFDataHeader, m_pZFFile);
    return (*this);
  }
  CZFFileWriterImpl& CZFFileWriterImpl::operator<<( const typIFD_IFDATAHEADER_EX* const a_pDataHeader )
  {
    ::fwrite(reinterpret_cast<const char*>(a_pDataHeader), 1, c_sizeofZFDataHeaderEx, m_pZFFile);
    return (*this);
  }
  //lint -restore

  //lint -save -e826
  bool CZFFileWriterImpl::convert(const float* a_src, uint8* a_dest, int32 a_elements) const
  {
    Status l_status = StsNoErr;
    float l_fAntennaVoltageRef = c_fMicroVolt2Volt * powf(10.0f, m_ZFDataHeader.intAntennaVoltageRef * 0.005f);
    float l_fScaleFactor       = 1.0f/((static_cast<float>(m_recipGain) / c_recipGain) * l_fAntennaVoltageRef);

    switch (m_ZFFrameHeader.uintFrameType)
    {
    //case ekFRH_DATASTREAM__IFDATA_16RE_16RE_FIX:
    case ekFRH_DATASTREAM__IFDATA_16RE_16IM_FIX:
      {
        l_fScaleFactor *= SHRT_MAX;
        l_status = MulC_float((const float *)a_src, l_fScaleFactor, (float *)a_dest, a_elements);
        if(l_status == StsNoErr)
          l_status = Convert_float_int16_t_Sfs((const float *)a_dest, (int16_t*)a_dest, a_elements, RndZero, 0);
      }
      break;

    case ekFRH_DATASTREAM__IFDATA_32RE_32IM_FIX:
    case ekFRH_DATASTREAM__IFDATA_32RE_32IM_FIX_RESCALED:
      {
        l_fScaleFactor *= INT_MAX;
        l_status = MulC_float((const float *)a_src, l_fScaleFactor, (float *)a_dest, a_elements);
        if(l_status == StsNoErr)
          l_status = Convert_float_int32_t_Sfs((const float*)a_dest, (int32_t*)a_dest, a_elements, RndZero, 0);
      }
      break;

    case ekFRH_DATASTREAM__IFDATA_32RE_32IM_FLOAT_RESCALED:
      {
        l_status = MulC_float((const float *)a_src, l_fScaleFactor, (float *)a_dest, a_elements);
      }
      break;

    default:
      {
        return false;
      }
    }

    if(l_status != StsNoErr)   
    {
      string s;
      CErrCtrl::Instance().error(CErrCode::kERR_FILES_IPP_ERROR, __LINE__,__FILE__, s); 
      return false;
    }
    return true;
  }
  //lint -restore

}

/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/
