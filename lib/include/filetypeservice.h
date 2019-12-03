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
* @file      filetypeservice.h
*
* @brief     This is the header file of class FileTypeService.
*
* @details   This class provides information about supported file formats on the current machine.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include <string>

#include "exportdecl.h"
#include "idataimportexport.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief This class provides information about supported file formats on the current machine.
      **/
      class FileTypeService final
      {
      public:
        /**
          @brief Factory method. Creates an object of the specified file type and returns a pointer
          to the IDataImportExport interface. The caller of this method takes care of memory
          management. Thus, do not forget to release the object when it is not used anymore.
          @param [in]  filename Path to the I/Q data file; UTF-8 encoded.
          @param [in]  fileType Type of the file format.
          @returns Returns an interface pointer to the created object.
        */MOSAIK_MODULE static IDataImportExport* create(const std::string& filename, FileType fileType);

        /**
          @brief Gets the file types that are supported by this library.
          @returns A vector containing the possible file types.
        */MOSAIK_MODULE static std::vector<FileType> getPossibleFileFormats();

        /**
          @brief Auxiliary function to get the file extension for any of the supported file types.
          @param [in]  fileType The file type for which the extension is needed.
          @returns The File extension of the specified file type, i.e. 'iqw', 'iq.tar', etc. as string.
        */MOSAIK_MODULE static std::string getFileExtension(FileType fileType);

        /**
          @brief Auxiliary function that appends a file filter string for the specified file type as
          required for some file open/save dialogs.
          @param [in]  fileType Type of the file format.
          @returns The filter string, e.g. "IQTAR files (*.iq.tar)" for iq.tar format.
        */MOSAIK_MODULE static std::string getFileFilter(FileType fileType);
      };
    }
  }
}