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

#include "dataimportexportbase_wrapper.h"

#include "iqw.h"

namespace RohdeSchwarz
{
  namespace Mosaik
  {
    namespace DataImportExport
    {
      using namespace System;

      /// <summary>
      /// Wraps up the native Iqw object defined in iqw.h.
      /// </summary>
      public ref class Iqw : public DataImportExportBase
      {
      public:
        /// <summary>
        /// Initializes a new instance of the <see cref="IqMatlab"/> class.
        /// Be aware that the filename cannot be changed after construction.
        /// </summary>
        /// <param name="filename">Name of the file to be read or to be written.</param>
        Iqw(String^ filename);

        /// <summary>
        /// Gets or sets the order of the I/Q data. Cannot be changed after first
        /// I/Q samples have been written.
        /// </summary>
        property IqDataOrder DataOrder
        {
          void set(IqDataOrder order);
          IqDataOrder get();
        }

        /// <summary>
        /// Gets or sets the directory used to write temporary files to.
        /// If not file-writer has been initialized an empty string is
        /// returned.
        /// </summary>
        property String^ TempDir
        {
          void set(String^ path);
          String^ get();
        }
      };
    }
  }
}