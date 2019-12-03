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

#ifndef ZFFILEREADERIMPL_H
#define ZFFILEREADERIMPL_H

//lint -save -e451 -e537
#include "ArrayComplex.h"
#include "Defines.h"
//lint -restore
#include "rs_gx40x_global_ifdata_header_if_defs.h"
#include "rs_gx40x_global_frame_types_if_defs.h"
#include "IIQObserver.h"

namespace AmlabFiles
{
  class CZFFileReader;
  //lint -save -e763
  class CZFFileReaderTest;
  //lint -restore

  /*! Size of data header in bytes */
  const int32 c_sizeofZFDataHeader   = sizeof(typIFD_IFDATAHEADER);
  /*! Size of extension data header in bytes */
  const int32 c_sizeofZFDataHeaderEx = sizeof(typIFD_IFDATAHEADER_EX);
  /*! Size of frame header in bytes */
  const int32 c_sizeofZFFrameHeader  = sizeof(typFRH_FRAMEHEADER);

  /* CLASS DECLARATION *********************************************/
  /*!
   * @brief   The class CZFFileReaderImpl provide a implementation
   *          of capabilities to read samples from a file.
   *
  ******************************************************************/
  class CZFFileReaderImpl
  {
    friend class CZFFileReader;
    friend class CZFFileReaderTest;

    /*! The type of container used to store samples by reading */
    typedef AmlabCommon::CArrayComplex  ContainerType;

  public:
    /*! Default constructor.
        The constructor initialize the frame and data header attributes.
        The filename is empty, the file can't be opened. */
    CZFFileReaderImpl();

    /*! Constructor with filename of the file to read in.
        The constructor initialize the frame and data header attributes.
        The filename is initialized and the file can be opened. */
    CZFFileReaderImpl(const std::wstring& a_strFilename);

    /*! Destructor */
    ~CZFFileReaderImpl();

    /* BEGIN IFileReader-Interface */
    /*! Return the open status of the file
        true = opened, false = closed */
    bool isOpen() const;

    /*! Open the file to read samples.
        If the filename was initialized, an attempt to open the file
        in read-only mode will be performed.
        The first and last frame and data header will be read.

        @return Error status */
    eStatus open();

    /*! Close the file and reset to initial status
        @return Error status */
    eStatus close();

    /*! Reset to initial status
        @return Error status */
    eStatus reset();

    /*! Return the last error code and error description occurred by writing samples.
        @param a_strErrorMsg The out parameter contains the error string of the last error
        @return Last error code (error status) */
    eStatus getLastError(std::wstring& a_strErrorMsg);
    /* END IFileReader-Interface   */

    /* BEGIN IIQSource-Interface */
    eStatus read(ContainerType& a_Data, uint32 a_elements, bool& a_rDataInfoChanged);

    void setAttributesChangeObserver(IIQObserver* a_pObserver, uint32 a_observerAttributes, uint32 a_samplesForTimestamp);

    /*! Return the number of samples containing within the file */
    uint64 getNumberOfSamples() const;

    /*! Return the signal sample rate in samples per second */
    uint32 getSampleRate() const;
    /*! Return the (center)frequency in [Hz] */
    uint64 getCenterFrequency() const;
    /*! Return the intermediate frequency (IF) bandwidth in [Hz] */
    uint32 getBandwidth() const;

    /*! Return the timestamp of the first signal data sample in the first data block [microsecs] */
    uint64 getFirstTimestamp() const;
    /*! Return the duration of all samples in [microsecs]*/
    uint64 getDuration() const;
    /*! Return the timestamp of the first signal data sample in the first data block in [ns] */
    uint64 getFirstTimestampNS() const;
    /*! Return the duration of all samples in [ns]*/
    uint64 getDurationNS() const;
    /*! Return the timestamp of the golden sample without any recalculation [ns] */
    uint64 getGoldenSampleTimestampNS() const;
    /* END IIQSource-Interface */

  private:
    bool    readFrame(int64 a_frameOffset, ContainerType* a_pData = NULL, uint32 a_samplesToRead = 0, uint64 a_starSampleIndex = 0);
    bool    readZFFrameFromFile(int64 a_offset);
    eStatus readProperties();
    bool    adjustData(uint8* a_pSource, uint8* a_pDestination, uint64 a_bytesToRead, uint32 a_statusword);
    uint64  getStopIndex() const;

    //lint -save -e826 -e1763
    inline typFRH_FRAMEHEADER* frameHeader() const
    {
      return reinterpret_cast<typFRH_FRAMEHEADER*>(m_pZFData);
    }
    //lint -restore
    //lint -save -e826 -e1763
    inline typIFD_IFDATAHEADER_EX* dataHeader() const
    {
      return reinterpret_cast<typIFD_IFDATAHEADER_EX*>(m_pZFData + c_sizeofZFFrameHeader);
    }
    //lint -restore
    inline bool isDataHeaderEx() const
    {
      return frameHeader()->uintDataHeaderLength >= (c_sizeofZFDataHeaderEx / c_sizeof_uint32);
    }

    bool  syncToNextZFData();
    void  realloc();

    eStatus setLastError( eStatus a_newState, const std::wstring& a_strErrorMsg );

    bool   hasAdditionalDataHeaderPart() const;
    void   getAdditionalDataHeaderPart(std::vector<unsigned int>& a_rAdditionalElements) const;
    void   getChangedAttributs();

    uint32 getSampleSize() const;           // sample size in bytes
    uint64 getTimestamp() const;            // [microsecs]
    int32  getAntennaVoltageRef() const;    // [0.1 dBmicroV]
    bool   isComplex() const;

    uint64 getSampleCounter() const;

  //lint -save -e1736
  private:
  //lint -restore

    std::wstring  m_strFilename;
    FILE*         m_pZFFile;
    bool          m_fileOpened;
    int64         m_fileSize;
    eEndian       m_endian;

    typFRH_FRAMEHEADER      m_ZFFrameHeader;
    typIFD_IFDATAHEADER_EX  m_ZFDataHeader;
    int64                   m_firstFrameHeaderPos;
    typFRH_FRAMEHEADER      m_lastZFFrameHeader;
    typIFD_IFDATAHEADER_EX  m_lastZFDataHeader;
    typFRH_FRAMEHEADER      m_prevFrameHeader;
    typIFD_IFDATAHEADER_EX  m_prevDataHeader;

    bool          m_dataHeader;
    int64         m_frameHeaderPos;
    int64         m_frameCount;

    uint8*        m_pZFData;
    uint32        m_ZFDataLength;

    uint64        m_currentSampleIndex;
    uint64        m_startSampleIndex;
    uint64        m_stopSampleIndex;

    bool                      m_dataInfoChanged;
    IIQObserver*              m_pObserver;
    uint32                    m_observerAttributes;
    IIQObserver::ObserverMap  m_attributes;
    uint32                    m_samplesForTimestamp;
    uint64                    m_timestampDelta;

    std::wstring  m_strErrorMsg;
    eStatus       m_eStatus;
  };
}

#endif // ZFFILEREADERIMPL_H

/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/
