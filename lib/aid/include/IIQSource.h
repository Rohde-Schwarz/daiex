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

#ifndef IIQSOURCE_H
#define IIQSOURCE_H

//lint -save -e537
#include "Defines.h"
#include <string>
//lint -restore

namespace AmlabFiles
{
  class IIQObserver;

  /* CLASS DECLARATION *********************************************/
  /*!
   * @brief   The interface class IIQSource provide methods to get the
   *          attributes of IQ data samples and to read samples.
   *
  ******************************************************************/
  template<class TContainer>
  class IIQSource
  {
  public:        

    typedef TContainer ContainerType;

    /*! Destructor */
    virtual ~IIQSource() {};

    /*! Returns the identifier or name of the source, e.g. a filename or socket descriptor */
    virtual const std::wstring& getIdentifier() const = 0;

    /*! Return true if it is a stream and navigating is not possible */
    virtual bool isSequential() = 0;

    /*! Read in samples from a file into container
        @param a_Data Container for samples
        @return Error status */
    virtual eStatus read(ContainerType& a_Data) = 0;
    /*! Read a number of samples from a file into container
        @param a_Data     Container for samples
        @param a_elements Number of samples to read
        @return Error status */
    virtual eStatus read(ContainerType& a_Data, uint32 a_elements) = 0;
    /*! Read in samples from file into container
        @param a_Data             Container for samples
        @param a_rDataInfoChanged The out parameter indicates changes within data header
        @return Error status */
    virtual eStatus read(ContainerType& a_Data, bool& a_rDataInfoChanged) = 0;
    /*! Read a number of samples from file into container
        @param a_Data             Container for samples
        @param a_elements         Number of samples to read
        @param a_rDataInfoChanged The out parameter indicates changes within data header
        @return Error status */
    virtual eStatus read(ContainerType& a_Data, uint32 a_elements, bool& a_rDataInfoChanged) = 0;

    /*! Set the observer interface */
    virtual void setAttributesChangeObserver(IIQObserver* a_pObserver, uint32 a_observerAttributes, uint32 a_samplesForTimestamp) = 0;

    /*! Return the number of samples containing within the file */
    virtual uint64 getNumberOfSamples() const = 0;

    /*! Set the start and stop read markers
        @param a_startSampleIndex The index of the sample which start reading
        @param a_stopSampleIndex The index of the last sample to read
        @return Error status */
    virtual eStatus setReadMarker(uint64 a_startSampleIndex, uint64 a_stopSampleIndex) = 0;
    /*! Get the start and stop read markers
        @param r_startSampleIndex The out parameter contains the index of the sample which start reading
        @param r_stopSampleIndex The out parameter contains index of the last sample to read */
    virtual void getReadMarker(uint64& r_startSampleIndex, uint64& r_stopSampleIndex) const = 0;

    /*! Return the sample index to the actual read position */
    virtual uint64 getReadPosition() const = 0;

    /*! Return the signal sample rate in samples per second */
    virtual uint32  getSampleRate() const = 0;
    /*! Return the (center)frequency in [Hz] */
    virtual uint64  getCenterFrequency() const = 0;
    /*! Return the intermediate frequency (IF) bandwidth in [Hz] */
    virtual uint32  getBandwidth() const = 0;

    /*! Return the timestamp of the first signal data sample in the first data block [microsecs] */
    virtual uint64  getFirstTimestamp() const = 0;
    /*! Return the duration of all samples in [microsecs]*/
    virtual uint64  getDuration() const = 0;
    /*! Return the timestamp of the first signal data sample in the first data block in [ns] */
    virtual uint64  getFirstTimestampNS() const = 0;
    /*! Return the duration of all samples in [ns]*/
    virtual uint64  getDurationNS() const = 0;
  };
}

#endif // IIQSOURCE_H

/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/
