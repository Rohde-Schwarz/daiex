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

#ifndef ZFFILEWRITERIMPL_H
#define ZFFILEWRITERIMPL_H

//lint -save -e451 -e537
#include "ArrayComplex.h"
#include "Defines.h"
//lint -restore
#include "rs_gx40x_global_ifdata_header_if_defs.h"
#include "rs_gx40x_global_frame_types_if_defs.h"

namespace AmlabFiles
{
  class CZFFileWriter;
  //lint -save -e763
  class CZFFileWriterTest;
  //lint -restore

  /*! Default recip gain - used from ZFFileWriter as well */
  const uint16 c_recipGain           = 0xffff;

  /* CLASS DECLARATION *********************************************/
  /*!
   * @brief   The class CZFFileWriterImpl implements methods to write
   *          samples to files.
   *
  ******************************************************************/
  class CZFFileWriterImpl
  {
    friend CZFFileWriter;
    friend CZFFileWriterTest;

    /*! The type of container containing samples for writing */
    typedef AmlabCommon::CArrayComplex  ContainerType;

  public:
    /*! Default constructor */
    CZFFileWriterImpl();

    /*! Constructor with filename of the file */
    CZFFileWriterImpl(const std::wstring& a_strFilename);

    /*! Destructor */
    ~CZFFileWriterImpl();

    /* BEGIN IFileReader-Interface */
    /*! Return the open status of the file
        true = opened, false = closed */
    bool isOpen() const;

    /*! Open the file for writing samples.
        @return Error status */
    eStatus open();

    /*! Close the file
        @return Error status */
    eStatus close();

    /*! Return the last error code and error description occurred by writing samples.
        @param a_strErrorMsg The out parameter contains the error string of the last error
        @return Last error code (error status) */
    eStatus getLastError(std::wstring& a_strErrorMsg);
    /* END IFileReader-Interface   */

    /* BEGIN IIQSink-Interface */
    eStatus write(const ContainerType& a_rData, uint32 a_elements);

    /*! Set the type of data carried in the frames
        @param a_eFrameType The frame type */
    void setFrameType(uint32 a_eFrameType);

    /*! Set the number of samples per data block an the number of data blocks
        @param a_samplesPerBlock Number of samples per data block
        @param a_dataBlockCount Number of data blocks per frame */
    void setDatablockSettings(uint32 a_samplesPerBlock, uint32 a_dataBlockCount);

    /*! Set the timestamp of the first signal data sample in the first frame
        @param a_timestamp The timestamp in microsecs */
    void setTimestamp(uint64 a_timestamp);

    /*! Set the timestamp of the first signal data sample in the first frame
        @param a_firstStartTimestampNS The timestamp in ns */
    void setFirstStartTimestampNS(uint64 a_firstStartTimestampNS);
    /* END IIQSink-Interface */

    /*! Set the data header type
        @param a_dataHeaderEx true = extended data header */
    void setDataHeaderEx( bool a_dataHeaderEx );

    /*! Set the sample counter of the first sample of the first frame.
        @param a_sampleCounter The sample counter */
    void setSampleCounter(uint64 a_sampleCounter);

    /*! Return the signal sample rate in samples per second */
    uint32 getSampleRate() const;
    /*! Return the size of a sample in bytes */
    uint32 getSampleSize() const;
    /*! Return the timestamp of the first signal data sample in the first data block [microsecs] */
    uint64 getTimestamp() const;
    /*! Return the samplecounter of the first sample of the first data block of this frame */
    uint64 getSampleCounter() const;

  private:
    void    writeFrame(const uint8* a_data, uint32 a_elements);
    void    flush();
    bool    convert(const float* a_src, uint8* a_dest, int32 a_elements) const;
    CZFFileWriterImpl& operator<<(const typFRH_FRAMEHEADER& a_frameHeader);
    CZFFileWriterImpl& operator<<(const char* const a_pDataHeader);
    CZFFileWriterImpl& operator<<(const typIFD_IFDATAHEADER_EX* const a_pDataHeader);
    void    setFrameLength();
    uint32  getSamplesPerBlockMax() const;
    uint32  getDatablockSizeMax() const;
    eStatus setLastError( eStatus a_newState, const std::wstring& a_strErrorMsg );

  //lint -save -e1736
  private:
  //lint -restore
    std::wstring  m_strFilename;
    FILE*         m_pZFFile;
    bool          m_flushOnClose;
    bool          m_dataHeaderEx;
    uint8*        m_pZFData;
    uint32        m_bufferSamplesMax;
    uint32        m_bufferSamples;

    typFRH_FRAMEHEADER      m_ZFFrameHeader;
    typIFD_IFDATAHEADER_EX  m_ZFDataHeader;
    uint16                  m_recipGain;

    std::wstring  m_strErrorMsg;
    eStatus       m_eStatus;
  };
}

#endif // ZFFILEWRITERIMPL_H

/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/
