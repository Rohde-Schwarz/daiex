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

#ifndef IIQSINK_H
#define IIQSINK_H

#include "Defines.h"
#include <string>

namespace AmlabFiles
{
  /* CLASS DECLARATION *********************************************/
  /*!
   * @brief   The interface class IIQSink provide methods to set the
   *          attributes of IQ data samples write and to write samples.
   *
  ******************************************************************/
  template<class ContainerType>
  class IIQSink
  {
  public:
    /*! Destructor */
    virtual ~IIQSink() {};

    /*! Returns the identifier or name of the sink, e.g. a filename or socket descriptor */
    virtual const std::wstring& getIdentifier() const = 0;

    /*! Return the open status of the sink
        @return true = opened, false = closed */
    virtual bool isOpen() const = 0;
    /*! Open the sink for writing samples.
        @return Error status */
    virtual eStatus open() = 0;
    /*! Close the sink
        @return Error status */
    virtual eStatus close() = 0;

    /*! Write samples to file
        @param a_rData Container containing the samples */
    virtual eStatus write(const ContainerType& a_rData) = 0;
    /*! Write samples to file
        @param a_rData Container containing the samples
        @param a_elements Number of samples to write */
    virtual eStatus write(const ContainerType& a_rData, uint32 a_elements) = 0;
    /*! Set the flag to flush the last samples not written.
        @param a_flushOnClose Flag to flush on file close */
    virtual void setFlushOnClose(bool a_flushOnClose) = 0;

    // Access methods: setter
    /*! Set the type of data carried in the frames
        @param a_eFrameType The frame type */
    virtual void setFrameType(uint32 a_eFrameType) = 0;

    /*! Set the number of samples per data block an the number of data blocks
        @param a_samplesPerBlock Number of samples per data block
        @param a_dataBlockCount Number of data blocks per frame */
    virtual void setDatablockSettings(uint32 a_samplesPerBlock, uint32 a_dataBlockCount) = 0;

    /*! Set the (center)frequency
        @param a_centerFrequency The tuner (center)frequency in [Hz] */
    virtual void setCenterFrequency(uint64 a_centerFrequency) = 0;
    /*! Set the intermediate frequency (IF) bandwidth
        @param a_bandwidth The intermediate frequency (IF) bandwidth in [Hz] */
    virtual void setBandwidth(uint32 a_bandwidth) = 0;

    /*! Set the signal sample rate
        @param a_sampleRate The signal sample rate in samples per second */
    virtual void setSampleRate(uint32 a_sampleRate) = 0;
    /*! Set the interpolation factor
        @param a_interpolation The interpolation factor referred to the ADC signal sample rate */
    virtual void setInterpolation(uint32 a_interpolation) = 0;
    /*! Set the decimation factor
        @param a_decimation The decimation factor referred to the ADC signal sample rate */
    virtual void setDecimation(uint32 a_decimation) = 0;

    /*! Set the device / parametrization specific correction value
        for the tuner front-end
        @param a_antennaVoltageRef The specific correction value in units of 0.1 dBuV */
    virtual void setAntennaVoltageRef(int32 a_antennaVoltageRef) = 0;
    /*! Set the data stream status
        @param a_statusword The data stream status */
    virtual void setStatusword(uint32 a_statusword) = 0;
    /*! Set the signal source ID / antenna ID
        @param a_signalSourceID The signal source ID / antenna ID */
    virtual void setSignalSourceID(uint32 a_signalSourceID) = 0;
    /*! Set the current signal source state
        @param a_signalSourceState The current signal source state */
    virtual void setSignalSourceState(uint32 a_signalSourceState) = 0;

    /*! Set the timestamp of the first signal data sample in the first frame
        @param a_timestamp The timestamp in microsecs */
    virtual void setTimestamp(uint64 a_timestamp) = 0;
    /*! Set the timestamp of the first signal data sample in the first frame
        @param a_firstStartTimestampNS The timestamp in ns */
    virtual void setFirstStartTimestampNS(uint64 a_firstStartTimestampNS) = 0;
    /*! Set the sample counter of the first sample of the first frame.
        @param a_sampleCounter The sample counter */
    virtual void setSampleCounter(uint64 a_sampleCounter) = 0;
    /*! Set the kFactor of the current antenna to determine field strength
        @param a_KFactor The kFactor in 0.1dB/m */
    virtual void setKFactor(int32 a_KFactor) = 0;
    /*! Set the reciprocal gain correction value for the data samples
        @param a_recipGain The reciprocal gain correction value */
    virtual void setRecipGain(float a_recipGain) = 0;
  };
}

#endif // not IIQSINK_H

/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/
