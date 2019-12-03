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

#ifndef ZFFILEWRITER_H
#define ZFFILEWRITER_H

#include "ArrayComplex.h"
#include "IFileWriter.h"
#include "IIQSink.h"

namespace AmlabFiles
{
  class CZFFileWriterImpl;
  class CZFFileWriterTest;

  /* CLASS DECLARATION *********************************************/
  /*!
   * @brief   The class CZFFileWriter provide operations to write
   *          samples to a file by implementing the interfaces
   *          IFileWriter and IIQSink.
   *
  ******************************************************************/
  class CZFFileWriter : public IFileWriter
                      , public IIQSink<AmlabCommon::CArrayComplex>
  {
    friend CZFFileWriterTest;

    /*! The type of container containing samples for writing */
    typedef AmlabCommon::CArrayComplex  ContainerType;

  public:
    /*! Default constructor */
    CZFFileWriter();

    /*! Constructor with filename of the file */
    CZFFileWriter(const std::wstring& a_strFilename);

    /*! Destructor */
    ~CZFFileWriter();

    /* BEGIN IFileWriter-Interface */
    /*! Set the filename of the file.
        These setting have only effect on closed mode(isOpen() == fales).
        @param a_strFilename The filename of the file */
    void setFilename(const std::wstring& a_strFilename);
    /*! Return the filename */
    const std::wstring& getFilename() const;

    /*! Return the open status of the file
        @return true = opened, false = closed */
    bool isOpen() const;
    /*! Open the file for writing samples.
        @return Error status */
    eStatus open();
    /*! Close the file
        @return Error status */
    eStatus close();

    /*! Return the last error code and error description occurred by writing samples.
        @param a_strErrorMsg The out parameter contains the error string of the last error*/
    eStatus getLastError(std::wstring& a_strErrorMsg);
    /* END IFileWriter-Interface   */

    /* BEGIN IIQSink-Interface */
    /*! Returns the identifier or name of the sink, e.g. a filename or socket descriptor */
    virtual const std::wstring& getIdentifier() const;

    /*! Write samples to file
        @param a_rData Container containing the samples */
    eStatus write(const ContainerType& a_rData);
    /*! Write samples to file
        @param a_rData Container containing the samples
        @param a_elements Number of samples to write */
    eStatus write(const ContainerType& a_rData, uint32 a_elements);

    /*! Set the flag to flush the last samples not written.
        @param a_flushOnClose Flag to flush on file close */
    void setFlushOnClose(bool a_flushOnClose);

    /*! Set the type of data carried in the frames.
        These setting have only effect on closed mode(isOpen() == fales).
        @param a_eFrameType The frame type */
    void setFrameType(uint32 a_eFrameType);

    /*! Set the number of samples per data block an the number of data blocks.
        These setting have only effect on closed mode(isOpen() == fales).
        @param a_samplesPerBlock Number of samples per data block
        @param a_dataBlockCount Number of data blocks per frame */
    void setDatablockSettings(uint32 a_samplesPerBlock, uint32 a_dataBlockCount);

    /*! Set the (center)frequency.
        These setting have only effect on closed mode(isOpen() == fales).
        @param a_centerFrequency The tuner (center)frequency in [Hz] */
    void setCenterFrequency(uint64 a_centerFrequency);

    /*! Set the intermediate frequency (IF) bandwidth.
        These setting have only effect on closed mode(isOpen() == fales).
        @param a_bandwidth The intermediate frequency (IF) bandwidth in [Hz] */
    void setBandwidth(uint32 a_bandwidth);

    /*! Set the signal sample rate.
        These setting have only effect on closed mode(isOpen() == fales).
        @param a_sampleRate The signal sample rate in samples per second */
    void setSampleRate(uint32 a_sampleRate);

    /*! Set the interpolation factor.
        These setting have only effect on closed mode(isOpen() == fales).
        @param a_interpolation The interpolation factor referred to the ADC signal sample rate */
    void setInterpolation(uint32 a_interpolation);

    /*! Set the decimation factor.
        These setting have only effect on closed mode(isOpen() == fales).
        @param a_decimation The decimation factor referred to the ADC signal sample rate */
    void setDecimation(uint32 a_decimation);

    /*! Set the device / parametrization specific correction value
        for the tuner front-end.
        These setting have only effect on closed mode(isOpen() == fales).
        @param a_antennaVoltageRef The specific correction value in units of 0.1 dBuV */
    void setAntennaVoltageRef(int32 a_antennaVoltageRef);

    /*! Set the data stream status.
        These setting have only effect on closed mode(isOpen() == fales).
        @param a_statusword The data stream status */
    void setStatusword(uint32 a_statusword);

    /*! Set the signal source ID / antenna ID.
        These setting have only effect on closed mode(isOpen() == fales).
        @param a_signalSourceID The signal source ID / antenna ID */
    void setSignalSourceID(uint32 a_signalSourceID);

    /*! Set the current signal source state.
        These setting have only effect on closed mode(isOpen() == fales).
        @param a_signalSourceState The current signal source state */
    void setSignalSourceState(uint32 a_signalSourceState);

    /*! Set the timestamp of the first signal data sample in the first frame.
        These setting have only effect on closed mode(isOpen() == fales).
        @param a_timestamp The timestamp in microsecs */
    void setTimestamp(uint64 a_timestamp);

    /*! Set the timestamp of the first signal data sample in the first frame.
        These setting have only effect on closed mode(isOpen() == fales).
        @param a_firstStartTimestampNS The timestamp in ns */
    void setFirstStartTimestampNS(uint64 a_firstStartTimestampNS);

    /*! Set the sample counter of the first sample of the first frame.
        These setting have only effect on closed mode(isOpen() == fales).
        @param a_sampleCounter The sample counter */
    void setSampleCounter(uint64 a_sampleCounter);

    /*! Set the kFactor of the current antenna to determine field strength.
        These setting have only effect on closed mode(isOpen() == fales).
        @param a_KFactor The kFactor in 0.1dB/m */
    void setKFactor(int32 a_KFactor);

    /*! Set the reciprocal gain correction value for the data samples.
        These setting have only effect on closed mode(isOpen() == fales).
        @param a_recipGain The reciprocal gain correction value */
    void setRecipGain(float a_recipGain);
    /* END IIQSink-Interface */

    /*! Set the data header type.
        These setting have only effect on closed mode(isOpen() == fales).
        @param a_dataHeaderEx true = extended data header */
    void setDataHeaderEx(bool a_dataHeaderEx);

    // Access methods: getter
    /*! Return the type of data carried in the frames */
    uint32 getFrameType() const;

    /*! Return the (center)frequency in [Hz] */
    uint64 getCenterFrequency() const;

    /*! Return the intermediate frequency (IF) bandwidth in [Hz] */
    uint32 getBandwidth() const;

    /*! Return the size of data block */
    uint32 getDataBlockSize()  const;

    /*! Return the count of data blocks */
    uint32 getDataBlockCount() const;

    /*! Return the signal sample rate in samples per second */
    uint32 getSampleRate() const;

    /*! Return the Analog to Digital Converter (ADC) signal sample rate */
    uint32 getADCSampleRate() const;

    /*! Return the interpolation factor referred to the ADC signal sample rate */
    uint32 getInterpolation() const;

    /*! Return the decimation factor referred to the ADC signal sample rate */
    uint32 getDecimation() const;

    /*! Return the device / parametrization specific correction value
        for the tuner front-end in units of 0.1 dBuV */
    int32  getAntennaVoltageRef() const;

    /*! Return the data stream status */
    uint32 getStatusword() const;

    /*! Return the signal source ID / antenna ID */
    uint32 getSignalSourceID() const;

    /*! Return the current signal source state */
    uint32 getSignalSourceState() const;

    /*! Return the kFactor of the current antenna to determine field strength in 0.1dB/m */
    int32  getKFactor() const;

    /*! Return the timestamp of the first signal data sample in the first data block [microsecs] */
    uint64 getTimestamp() const;

    /*! Return the timestamp of the first signal data sample in the first data block in [ns] */
    uint64 getFirstStartTimestampNS() const;

    /*! Return the sample counter of the first sample of the first data block of this frame */
    uint64 getSampleCounter() const;

    /*! Return the reciprocal gain correction value for the data samples */
    float  getRecipGain() const;

  private:
    CZFFileWriterImpl*  m_pImpl;
  };
}

#endif // ZFFILEWRITER_H

/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/
