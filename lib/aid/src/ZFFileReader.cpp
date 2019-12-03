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
#include "ZFFileReader.h"
#include "ZFFileReaderImpl.h"

/* STATIC INITIALIZATION **********************************************************/

/* ------------------------------------------------------------------------------ */

namespace AmlabFiles
{
  /* --- PUBLIC METHODS --- */

  CZFFileReader::CZFFileReader()
    : m_pImpl (new CZFFileReaderImpl())
  {
  }

  CZFFileReader::CZFFileReader(const std::wstring& a_strFilename)
    : m_pImpl (new CZFFileReaderImpl(a_strFilename))
  {
  }

  //lint -save -e1551
  CZFFileReader::~CZFFileReader()
  {
    delete m_pImpl;
  }
  //lint -restore

  void CZFFileReader::setFilename(const std::wstring& a_strFilename)
  {
    if (!isOpen())
    {
      m_pImpl->m_strFilename = a_strFilename;
    }
  }

  const std::wstring& CZFFileReader::getFilename() const
  {
    return m_pImpl->m_strFilename;
  }

  bool CZFFileReader::isOpen() const
  {
    return m_pImpl->isOpen();
  }

  eStatus CZFFileReader::open()
  {
    return m_pImpl->open();
  }

  eStatus CZFFileReader::close()
  {
    return m_pImpl->close();
  }

  eStatus CZFFileReader::reset()
  {
    return m_pImpl->reset();
  }

  //lint -save -e732
  uint64 CZFFileReader::getFileSize() const
  {
    return m_pImpl->m_fileSize;
  }
  //lint -restore

  eStatus CZFFileReader::getLastError( std::wstring& a_strErrorMsg )
  {
    return m_pImpl->getLastError(a_strErrorMsg);
  }

  const std::wstring& CZFFileReader::getIdentifier() const
  {
    return getFilename();
  }

  //lint -save -e1961
  bool CZFFileReader::isSequential()
  {
    return false;
  }
  //lint -restore

  eStatus CZFFileReader::read(ContainerType& a_Data)
  {
    bool l_dataInfoChanged = false;
    return m_pImpl->read(a_Data, a_Data.getCapacity(), l_dataInfoChanged);
  }

  eStatus CZFFileReader::read(ContainerType& a_Data, uint32 a_elements)
  {
    bool l_dataInfoChanged = false;
    return m_pImpl->read(a_Data, a_elements, l_dataInfoChanged);
  }

  eStatus CZFFileReader::read(ContainerType& a_Data, bool& a_rDataInfoChanged)
  {
    a_rDataInfoChanged = false;
    return m_pImpl->read(a_Data, a_Data.getCapacity(), a_rDataInfoChanged);
  }

  eStatus CZFFileReader::read(ContainerType& a_Data, uint32 a_elements, bool& a_rDataInfoChanged)
  {
    a_rDataInfoChanged = false;
    return m_pImpl->read(a_Data, a_elements, a_rDataInfoChanged);
  }

  eStatus CZFFileReader::setReadMarker(uint64 a_startSampleIndex, uint64 a_stopSampleIndex)
  {
    if (!isOpen())
      return ekFileNotOpen;

    if (a_startSampleIndex > a_stopSampleIndex)
      return ekInvalidParameters;

    m_pImpl->m_startSampleIndex   = a_startSampleIndex;
    m_pImpl->m_stopSampleIndex    = a_stopSampleIndex;
    m_pImpl->m_currentSampleIndex = m_pImpl->m_startSampleIndex;

    return ekNoError;
  }

  void CZFFileReader::getReadMarker(uint64& r_startSampleIndex, uint64& r_stopSampleIndex) const
  {
    r_startSampleIndex = m_pImpl->m_startSampleIndex;
    r_stopSampleIndex  = m_pImpl->m_stopSampleIndex;
  }

  uint64 CZFFileReader::getReadPosition() const
  {
    return m_pImpl->m_currentSampleIndex;
  }

  uint32 CZFFileReader::getDataBlockSize() const
  {
    return m_pImpl->m_ZFDataHeader.uintDatablockLength;
  }

  uint32 CZFFileReader::getDataBlockCount() const
  {
    return m_pImpl->m_ZFDataHeader.uintDatablockCount;
  }

  uint32 CZFFileReader::getSampleSize() const
  {
    return m_pImpl->getSampleSize();
  }

  uint32 CZFFileReader::getADCSampleRate() const
  {
    return m_pImpl->m_ZFDataHeader.uintSamplerate;
  }

  uint32 CZFFileReader::getInterpolation() const
  {
    return m_pImpl->m_ZFDataHeader.uintInterpolation;
  }

  uint32 CZFFileReader::getDecimation() const
  {
    return m_pImpl->m_ZFDataHeader.uintDecimation;
  }

  uint64 CZFFileReader::getCenterFrequency() const
  {
    return m_pImpl->getCenterFrequency();
  }

  int32 CZFFileReader::getAntennaVoltageRef() const
  {
    return m_pImpl->getAntennaVoltageRef();
  }

  uint64 CZFFileReader::getFirstTimestamp() const
  {
    return m_pImpl->getFirstTimestamp();
  }

  uint64 CZFFileReader::getDuration() const
  {
    return m_pImpl->getDuration();
  }

  uint64 CZFFileReader::getFirstTimestampNS() const
  {
    return m_pImpl->getFirstTimestampNS();
  }

  uint64 CZFFileReader::getDurationNS() const
  {
    return m_pImpl->getDurationNS();
  }

  uint64 CZFFileReader::getGoldenSampleTimestampNS() const
  {
    return m_pImpl->getGoldenSampleTimestampNS();
  }

  uint32 CZFFileReader::getStatusword() const
  {
    return m_pImpl->m_ZFDataHeader.uintStatusword;
  }

  uint32 CZFFileReader::getSignalSourceID() const
  {
    return m_pImpl->m_lastZFDataHeader.uintSignalSourceID;
  }

  uint32 CZFFileReader::getSignalSourceState() const
  {
    return m_pImpl->m_lastZFDataHeader.uintSignalSourceState;
  }

  uint64 CZFFileReader::getSampleCounter() const
  {
    return m_pImpl->getSampleCounter();
  }

  int32 CZFFileReader::getKFactor() const
  {
    return m_pImpl->m_ZFDataHeader.intKFactor;
  }

  bool CZFFileReader::isComplex() const
  {
    return m_pImpl->isComplex();
  }

  uint32 CZFFileReader::getSampleRate() const
  {
    return m_pImpl->getSampleRate();
  }

  uint32 CZFFileReader::getBandwidth() const
  {
    return m_pImpl->getBandwidth();
  }

  uint64 CZFFileReader::getNumberOfSamples() const
  {
    return m_pImpl->getNumberOfSamples();
  }

  bool CZFFileReader::hasAdditionalDataHeaderPart() const
  {
    return m_pImpl->hasAdditionalDataHeaderPart();
  }

  void CZFFileReader::getAdditionalDataHeaderPart(std::vector<unsigned int>& a_rAdditionalElements) const
  {
    m_pImpl->getAdditionalDataHeaderPart(a_rAdditionalElements);
  }

  void CZFFileReader::setAttributesChangeObserver(IIQObserver* a_pObserver, uint32 a_observerAttributes, uint32 a_samplesForTimestamp)
  {
    m_pImpl->setAttributesChangeObserver(a_pObserver,a_observerAttributes, a_samplesForTimestamp);
  }

}

/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/
