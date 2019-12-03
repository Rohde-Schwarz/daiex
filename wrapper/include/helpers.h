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

#include <string>

namespace RohdeSchwarz
{
  namespace Mosaik
  {
    namespace DataImportExport
    {
      using namespace System;

      /// <summary>
      /// Provides little helpers.
      /// </summary>
      private ref class Helpers
      {
      public:
        /// <summary>
        /// Marshals the native UTF-8 encoded input std::string to a .NET UTF-16 String object.
        /// </summary>
        /// <param name="value">UTF-8 encoded input string.</param>
        /// <returns>UTF-16 encoded .NET String object.</returns>
        static String^ marshalUTF8String(const std::string &value);

        /// <summary>
        /// Marshals the .NET UTF-16 encoded input String object to a native UTF-8 encoded std::string.
        /// </summary>
        /// <param name="value">UTF-16 encoded .NET String object.</param>
        /// <returns>UTF-16 encoded .NET String object.</returns>
        static std::string marshalToUTF8String(String^ value);
      };
    }
  }
}