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
#include "ZFFileWriter.h"
#include "ZFFileWriterImpl.h"

/* STATIC INITIALIZATION **********************************************************/

/* ------------------------------------------------------------------------------ */

namespace AmlabFiles
{
  /* --- PUBLIC METHODS --- */

  CZFFileWriter::CZFFileWriter()
    : m_pImpl (new CZFFileWriterImpl())
  {
  }

  CZFFileWriter::CZFFileWriter(const std::wstring& a_strFilename)
    : m_pImpl (new CZFFileWriterImpl(a_strFilename))
  {
  }

  //lint -save -e1551
  CZFFileWriter::~CZFFileWriter()
  {
    delete m_pImpl;
  }
  //lint -restore

  void CZFFileWriter::setFilename(const std::wstring& a_strFilename)
  {
    if (!isOpen())
    {
      m_pImpl->m_strFilename = a_strFilename;
    }
  }

  const std::wstring& CZFFileWriter::getFilename() const
  {
    return m_pImpl->m_strFilename;
  }

  bool CZFFileWriter::isOpen() const
  {
    return m_pImpl->isOpen();
  }

  eStatus CZFFileWriter::open()
  {
    return m_pImpl->open();
  }

  eStatus CZFFileWriter::close()
  {
    return m_pImpl->close();
  }

  const std::wstring& CZFFileWriter::getIdentifier() const
  {
    return getFilename();
  }

  eStatus CZFFileWriter::write(const ContainerType& a_rData)
  {
    return m_pImpl->write(a_rData,a_rData.getSize());
  }

  AmlabFiles::eStatus CZFFileWriter::write( const ContainerType& a_rData, uint32 a_elements )
  {
    return m_pImpl->write(a_rData,a_elements);
  }

  void CZFFileWriter::setFlushOnClose( bool a_flushOnClose )
  {
    m_pImpl->m_flushOnClose = a_flushOnClose;
  }

  eStatus CZFFileWriter::getLastError( std::wstring& a_strErrorMsg )
  {
    return m_pImpl->getLastError(a_strErrorMsg);
  }

  void CZFFileWriter::setDataHeaderEx( bool a_dataHeaderEx )
  {
    m_pImpl->setDataHeaderEx(a_dataHeaderEx);
  }

  void CZFFileWriter::setFrameType(uint32 a_eFrameType)
  {
    m_pImpl->setFrameType(a_eFrameType);
  }

  void CZFFileWriter::setCenterFrequency( uint64 a_centerFrequency )
  {
    if (!isOpen())
    {
      m_pImpl->m_ZFDataHeader.uintTunerFrequency_High = static_cast<uint32>((a_centerFrequency & 0xFFFFFFFF00000000)>>32);
      m_pImpl->m_ZFDataHeader.uintTunerFrequency_Low  = static_cast<uint32>( a_centerFrequency & 0x00000000FFFFFFFF);
    }
  }

  void CZFFileWriter::setBandwidth( uint32 a_bandwidth )
  {
    if (!isOpen())
    {
      m_pImpl->m_ZFDataHeader.uintBandwidth = a_bandwidth;
    }
  }

  void CZFFileWriter::setDatablockSettings( uint32 a_samplesPerBlock, uint32 a_dataBlockCount )
  {
    m_pImpl->setDatablockSettings(a_samplesPerBlock, a_dataBlockCount);
  }

  void CZFFileWriter::setSampleRate( uint32 a_ADCSampleRate )
  {
    if (!isOpen())
    {
      m_pImpl->m_ZFDataHeader.uintSamplerate = a_ADCSampleRate ? a_ADCSampleRate : 1;
    }
  }

  void CZFFileWriter::setInterpolation( uint32 a_interpolation )
  {
    if (!isOpen())
    {
      m_pImpl->m_ZFDataHeader.uintInterpolation = a_interpolation ? a_interpolation : 1;
    }
  }

  void CZFFileWriter::setDecimation( uint32 a_decimation )
  {
    if (!isOpen())
    {
      m_pImpl->m_ZFDataHeader.uintDecimation = a_decimation ? a_decimation : 1;
    }
  }

  void CZFFileWriter::setAntennaVoltageRef( int32 a_antennaVoltageRef )
  {
    if (!isOpen())
    {
      m_pImpl->m_ZFDataHeader.intAntennaVoltageRef = a_antennaVoltageRef;
    }
  }

  void CZFFileWriter::setStatusword( uint32 a_statusword )
  {
    if (!isOpen())
    {
      m_pImpl->m_ZFDataHeader.uintStatusword = a_statusword;
    }
  }

  void CZFFileWriter::setSignalSourceID( uint32 a_signalSourceID )
  {
    if (!isOpen())
    {
      m_pImpl->m_ZFDataHeader.uintSignalSourceID = a_signalSourceID;
    }
  }

  void CZFFileWriter::setSignalSourceState( uint32 a_signalSourceState )
  {
    if (!isOpen())
    {
      m_pImpl->m_ZFDataHeader.uintSignalSourceState = a_signalSourceState;
    }
  }

  void CZFFileWriter::setKFactor( int32 a_KFactor )
  {
    if (!isOpen())
    {
      m_pImpl->m_ZFDataHeader.intKFactor = a_KFactor;
    }
  }

  void CZFFileWriter::setTimestamp( uint64 a_timestamp )
  {
    if (!isOpen())
    {
      m_pImpl->setTimestamp( a_timestamp );
    }
  }

  void CZFFileWriter::setFirstStartTimestampNS( uint64 a_firstStartTimestampNS )
  {
    if (!isOpen())
    {
      m_pImpl->setFirstStartTimestampNS( a_firstStartTimestampNS );
    }
  }

void CZFFileWriter::setSampleCounter( uint64 a_sampleCounter )
  {
    if (!isOpen())
    {
      m_pImpl->setSampleCounter( a_sampleCounter );
    }
  }

  void CZFFileWriter::setRecipGain( float a_recipGain )
  {
    if (!isOpen())
    {
      m_pImpl->m_recipGain = static_cast<uint16>(a_recipGain * c_recipGain);
    }
  }

  uint32 CZFFileWriter::getFrameType() const
  {
    return m_pImpl->m_ZFFrameHeader.uintFrameType;
  }

  uint64 CZFFileWriter::getCenterFrequency() const
  {
    uint64 l_centerFrequency = m_pImpl->m_ZFDataHeader.uintTunerFrequency_High;
    l_centerFrequency <<= 32;
    l_centerFrequency |= m_pImpl->m_ZFDataHeader.uintTunerFrequency_Low;
    return l_centerFrequency;
  }

  uint32 CZFFileWriter::getBandwidth() const
  {
    return m_pImpl->m_ZFDataHeader.uintBandwidth;
  }

  uint32 CZFFileWriter::getDataBlockSize()  const
  {
    return m_pImpl->m_ZFDataHeader.uintDatablockLength;
  }

  uint32 CZFFileWriter::getDataBlockCount() const
  {
    return m_pImpl->m_ZFDataHeader.uintDatablockCount;
  }

  uint32 CZFFileWriter::getSampleRate() const
  {
    return m_pImpl->getSampleRate();
  }

  uint32 CZFFileWriter::getADCSampleRate() const
  {
    return m_pImpl->m_ZFDataHeader.uintSamplerate;
  }

  uint32 CZFFileWriter::getInterpolation() const
  {
    return m_pImpl->m_ZFDataHeader.uintInterpolation;
  }

  uint32 CZFFileWriter::getDecimation() const
  {
    return m_pImpl->m_ZFDataHeader.uintDecimation;
  }

  int32 CZFFileWriter::getAntennaVoltageRef() const
  {
    return m_pImpl->m_ZFDataHeader.intAntennaVoltageRef;
  }

  uint32 CZFFileWriter::getStatusword() const
  {
    return m_pImpl->m_ZFDataHeader.uintStatusword;
  }

  uint32 CZFFileWriter::getSignalSourceID() const
  {
    return m_pImpl->m_ZFDataHeader.uintSignalSourceID;
  }

  uint32 CZFFileWriter::getSignalSourceState() const
  {
    return m_pImpl->m_ZFDataHeader.uintSignalSourceState;
  }

  int32 CZFFileWriter::getKFactor() const
  {
    return m_pImpl->m_ZFDataHeader.intKFactor;
  }

  uint64 CZFFileWriter::getTimestamp() const
  {
    return m_pImpl->getTimestamp();
  }

  uint64 CZFFileWriter::getFirstStartTimestampNS() const
  {
    uint64 l_startTimestamp = m_pImpl->m_ZFDataHeader.bigtimeStartTimeStamp.structTimeInTwoWords.uintTime_HiOrderBits;
    l_startTimestamp <<= 32;
    l_startTimestamp |= m_pImpl->m_ZFDataHeader.bigtimeStartTimeStamp.structTimeInTwoWords.uintTime_LoOrderBits;
    return l_startTimestamp;
  }

  uint64 CZFFileWriter::getSampleCounter() const
  {
    return m_pImpl->getSampleCounter();
  }

  float CZFFileWriter::getRecipGain() const
  {
    return static_cast<float>(m_pImpl->m_recipGain) / c_recipGain;
  }

}

/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/
