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

#include "idataimportexport_wrapper.h"
#include "enums_wrapper.h"

namespace RohdeSchwarz
{
  namespace Mosaik
  {
    namespace DataImportExport
    {
      using namespace System;
      using namespace System::Collections::Generic;

      /// <summary>
      /// Wraps up the native FileTypeService object defined in filetypeservice.h.
      /// </summary>
      public ref class FileTypeService
      {
      public:

        /// <summary>
        /// Gets a list of all file types that are supported by this library.
        ///	</summary>
        static property IList<FileType>^ PossibleFileFormats
        {
          IList<FileType>^ get();
        }

        /// <summary>
        /// Factory method. Creates an object of the specified file type and returns a reference
        /// to the IDataImportExport interface. 
        ///	</summary>
        /// <param name="filename">Path to the I/Q data file.</param>
        /// <param name="fileType">Type of the file format.</param>
        /// <returns>Returns a reference to the IDataImportExport interface of the created object.</returns>
        static IDataImportExport^ Create(String^ filename, FileType fileType);

        /// <summary>
        /// Auxiliary function to get the file extension for any of the supported file types.
        ///	</summary>
        /// <param name="fileType">The file type for which the extension is needed.</param>
        /// <returns>The File extension of the specified file type, i.e. 'iqw', 'iq.tar', etc.</returns>
        static String^ GetFileExtension(FileType fileType);

        /// <summary>
        /// Auxiliary function that appends a file filter string for the specified file type as
        /// required for some file open/save dialogs.
        ///	</summary>
        /// <param name="fileType">Type of the file format.</param>
        /// <returns>The filter string, e.g. "IQTAR files (*.iq.tar)" for iq.tar format.</returns>
        static String^ GetFileFilter(FileType fileType);

      private:
        /// <summary>
        /// Converts from managed to native version of FileType.
        ///	</summary>
        /// <param name="fileType">The managed file type.</param>
        /// <returns>Returns the native version of the input value.</returns>
        static rohdeschwarz::mosaik::dataimportexport::FileType GetFileType(FileType fileType);

        /// <summary>
        /// Converts from native to managed version of FileType.
        ///	</summary>
        /// <param name="fileType">The native file type.</param>
        /// <returns>Returns the managed version of the input value.</returns>
        static FileType GetFileType(rohdeschwarz::mosaik::dataimportexport::FileType fileType);
      };
    }
  }
}