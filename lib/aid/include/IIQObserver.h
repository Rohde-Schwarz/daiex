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

#ifndef IIQOBSERVER_H
#define IIQOBSERVER_H

#include <map>

namespace AmlabFiles
{
  /*! Enum type with attributes which should be observed */
  enum eObserverAttributes
  {
    ekCenterFrequency =  1,
    ekBandwidth       =  2,
    ekSampleRate      =  4,
    ekStatusword      =  8,
    ekTimestamp       = 16
  };

  /* CLASS DECLARATION *********************************************/
  /*!
   * @brief   The interface class IIQObserver provide a callback method
   *          to check continuing read after attribute changes..
   *
  ******************************************************************/
  class IIQObserver
  {
  public:
    /*! Type of a map to store attributes which changed value */
    typedef std::map<uint32,uint64> ObserverMap;

    /*! Destructor */
    virtual ~IIQObserver() {};

    /*! Callback method to check continuing read after attribute changes.
        @param a_sampleIndex Index of actual sample
        @param a_attributes Map of attributes which changed value */
    virtual bool observer(uint64 a_sampleIndex, ObserverMap& a_attributes) = 0;
  };
}

#endif // IIQOBSERVER_H

/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/
