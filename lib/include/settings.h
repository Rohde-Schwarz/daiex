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
* @file      settings.h
*
* @brief     This is the header file of class Settings.
*
* @details   Class to group globally used settings of this library.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include <string>

#include "exportdecl.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief Class to group globally used settings of this library.
      */
      class Settings final
      {
      public:
        /**
          @brief Sets the buffer size in byte used to copy data using memory
          mapped files.
          @param [in]  buffSize
          @returns 
        */MOSAIK_MODULE static void setBufferSize(const size_t buffSize);

        /**
          @brief Gets the buffer size in bytes used to copy data using memory
          mapped files. Default size is 4096000.
          @returns Returns the buffer size in bytes.
        */MOSAIK_MODULE static size_t getBufferSize();

      private:
        /** @brief Memory mapped file copy buffer size. */
        static size_t bufferSize_;
      };
    }
  }
}