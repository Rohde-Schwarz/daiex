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

/*!
* @file      ianalyzecontentiqtar.h
*
* @brief     This is the header file of class IAnalyzeContentIqTar.
*
* @details   This class contains the interface definition IAnalyzeContentIqTar. Declares methods called
*            after parsing an IQTar file to update the internal object state w.r.t. deprecated meta data that
*            was found in the file. Deprecated meta data must be stored in the xml file at hierarchy level
*            &lt;UserData&gt;&lt;RohdeSchwarz&gt;DEPRECATED_INFO_STRING. Inherits update of channel information, meta data, and timestamp from
*            interface IAnalyzeContent.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include "ianalyzecontent.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief  Declares methods called after parsing an IQTar file to update the internal object state w.r.t. 
      * deprecated meta data that was found in the file. Deprecated meta data must be stored in the xml file at hierarchy level
      * &lt;UserData&gt;&lt;RohdeSchwarz&gt;DEPRECATED_INFO_STRING. Inherits update of channel information, meta data, and
      * timestamp from interface IAnalyzeContent.
      */
      class IAnalyzeContentIqTar : public IAnalyzeContent
      {
      public:
        /**
          @brief Called whenever deprecated information was found in the xml-file at 
          hierarchy level &lt;UserData&gt;&lt;RohdeSchwarz&gt;DEPRECATED_INFO_STRING. Required by FSW.
          @param [in]  xmlString  Deprecated information as xml string as read from xml-file.
        */virtual void updateDeprecatedInfo(const std::string& xmlString) = 0;
      };
    }
  }
}
