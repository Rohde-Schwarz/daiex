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

#ifndef ZFFILEREADER_H
#define ZFFILEREADER_H

#include <vector>
#include "ArrayComplex.h"
#include "IFileReader.h"
#include "IIQSource.h"

namespace AmlabFiles
{
  class CZFFileReaderImpl;
  class CZFFileReaderTest;

  /* CLASS DECLARATION *********************************************/
  /*!
   * @brief   The class CZFFileReader provide capabilities to read
   *          samples from a file by implementing the interfaces
   *          IFileReader and IIQSource.
   *
  ******************************************************************/
  class CZFFileReader : public IFileReader
                      , public IIQSource<AmlabCommon::CArrayComplex>
  {
    friend class CZFFileReaderTest;    

  public:
    /*! Default constructor */
    CZFFileReader();

    /*! Constructor with filename of the file to read in */
    CZFFileReader(const std::wstring& a_strFilename);

    /*! Destructor */
    ~CZFFileReader();

    /* BEGIN IFileReader-Interface */
    /*! Set the filename of the file to open/read.
        These setting have only effect on closed mode(isOpen() == fales).
        @param a_strFilename The filename of the file to read in */
    void setFilename(const std::wstring& a_strFilename);

    /*! Return the filename */
    const std::wstring& getFilename() const;

    /*! Return the open status of the file
        @return true = opened, false = closed */
    bool isOpen() const;

    /*! Open the file for reading.
        @return Error status */
    eStatus open();

    /*! Close the file and reset to initial status
        @return Error status */
    eStatus close();

    /*! Reset to initial status
        @return Error status */
    eStatus reset();

    /*! Return the size of the file in number of bytes */
    uint64 getFileSize() const;

    /*! Return the last error code and error description occurred by reading samples.
        @param a_strErrorMsg The out parameter contains the error string of the last error
        @return The last error code (error status) */
    eStatus getLastError(std::wstring& a_strErrorMsg);
    /* END IFileReader-Interface   */

    /* BEGIN IIQSource-Interface */
    /*! Returns the identifier or name of the source, e.g. a filename or socket descriptor
        @return the file name if set */
    const std::wstring& getIdentifier() const;

    /*! Return true if it is a stream
        @return always false */
    bool isSequential();

    /*! Read in samples from file into container
        @param a_Data Container for samples
        @return Error status */
    eStatus read(ContainerType& a_Data);

    /*! Read a number of samples from file into container
        @param a_Data     Container for samples
        @param a_elements Number of samples to read
        @return Error status */
    eStatus read(ContainerType& a_Data, uint32 a_elements);

    /*! Read in samples from file into container
        @param a_Data             Container for samples
        @param a_rDataInfoChanged The out parameter indicates changes within data header
        @return Error status */
    eStatus read(ContainerType& a_Data, bool& a_rDataInfoChanged);

    /*! Read a number of samples from file into container
        @param a_Data             Container for samples
        @param a_elements         Number of samples to read
        @param a_rDataInfoChanged The out parameter indicates changes within data header
        @return Error status */
    eStatus read(ContainerType& a_Data, uint32 a_elements, bool& a_rDataInfoChanged);

    /*! Set the observer interface
        The observer interface IIQObserver providing a method to ask
        the observer of continuing to read after changes of attributes
        (centerfrequency, bandwidth, samplerate, statusword, timestamp).
        The observer interface can be set only at file closed status.
        These setting have only effect on closed mode(isOpen() == fales).
        @param a_pObserver            The observer interface
        @param a_observerAttributes   Attributes to watch of changes
        @param a_samplesForTimestamp  Time delta to watch changes of timestamps */
    void setAttributesChangeObserver(IIQObserver* a_pObserver, uint32 a_observerAttributes, uint32 a_samplesForTimestamp);

    /*! Return the number of samples containing within the file */
    uint64 getNumberOfSamples() const;

    /*! Set the start and stop read markers
        @param a_startSampleIndex The index of the sample which start reading
        @param a_stopSampleIndex The index of the last sample to read
        @return Error status */
    eStatus setReadMarker(uint64 a_startSampleIndex, uint64 a_stopSampleIndex);

    /*! Get the start and stop read markers
        @param r_startSampleIndex The out parameter contains the index of the sample which start reading
        @param r_stopSampleIndex The out parameter contains index of the last sample to read */
    void getReadMarker(uint64& r_startSampleIndex, uint64& r_stopSampleIndex) const;

    /*! Return the sample index to the actual read position */
    uint64  getReadPosition() const;

    /*! Return the signal sample rate in samples per second */
    uint32  getSampleRate() const;
    /*! Return the (center)frequency in [Hz] */
    uint64  getCenterFrequency() const;
    /*! Return the intermediate frequency (IF) bandwidth in [Hz] */
    uint32  getBandwidth() const;

    /*! Return the timestamp of the first signal data sample in the first data block [microsecs] */
    uint64  getFirstTimestamp() const;
    /*! Return the duration of all samples in [microsecs]*/
    uint64  getDuration() const;
    /*! Return the timestamp of the first signal data sample in the first data block in [ns] */
    uint64  getFirstTimestampNS() const;
    /*! Return the timestamp of the golden sample without any recalculation [ns] */
    uint64  getGoldenSampleTimestampNS() const;
    /*! Return the duration of all samples in [ns]*/
    uint64  getDurationNS() const;
    /* END IIQSource-Interface */

    /*! Returns true if the file contains additional dataheader parts */
    bool hasAdditionalDataHeaderPart() const;
    /*! Get the additional dataheader parts
        @param a_rAdditionalElements The out parameter with the additional dataheader parts */
    void getAdditionalDataHeaderPart(std::vector<unsigned int>& a_rAdditionalElements) const;

    // Access methods: getter
    /*! Return the size of data block */
    uint32 getDataBlockSize()  const;
    /*! Return the count of data blocks */
    uint32 getDataBlockCount() const;
    /*! Return the sample size in bytes */
    uint32 getSampleSize() const;

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
    /*! Return the samplecounter of the first sample of the first data block of this frame */
    uint64 getSampleCounter() const;
    /*! Return the kFactor of the current antenna to determine field strength in 0.1dB/m */
    int32  getKFactor() const;

    /*! Returns true if the file contains complex samples (RE/IM) */
    bool   isComplex() const;

  private:
    CZFFileReaderImpl*  m_pImpl;
  };
}

#endif // ZFFILEREADER_H

/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/
