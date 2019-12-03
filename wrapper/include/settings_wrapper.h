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

#pragma once

#include "settings.h"

namespace RohdeSchwarz
{
  namespace Mosaik
  {
    namespace DataImportExport
    {
      using namespace System;

      /// <summary>
      /// Wraps up the native Settings object defined in settings.h.
      /// </summary>
      public ref class Settings abstract sealed
      {
      public:
        /// <summary>
        /// Gets or sets the buffer size in byte used to copy data using memory
        /// mapped files.
        /// </summary>
        static property size_t BufferSize
        {
          void set(size_t size);
          size_t get();
        }
      };
    }
  }
}