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
* @file      platform.h
*
* @brief     This is the header file of class Platform.
*
* @details   The class wraps-up OS-specific methods. The implementation is divided to 
*            platform_win.cpp and platform_linux.cpp
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include <string>
#include <fcntl.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>

#include "archive.h"
#include "memory_mapped_file.hpp"

#ifdef _WIN32
 #if _MSC_VER <= 1700 // until VS2012
namespace std
{
  template<typename T>
  int isnan(const T &x)
  {
    return _isnan(x);
  }
}
 #endif
#elif __GNUC__
  #define _snprintf_s snprintf 
  #define fopen_s(fp, fmt, mode) *(fp)=fopen(fmt, mode)
#endif

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief The class wraps-up OS-specific methods. The implementation is divided to 
      * platform_win.cpp and platform_linux.cpp
      **/
      class Platform final
      {
      public:

        /**
          @returns Returns the path to the system's default folder that holds
          temporary files.
          @throws DaiException(InternalError) if an error occurred while retrieving a temp directory.
        */static std::string getTmpDir();
            
        /**
          @param [in]  path Fully qualified file path to the directory where the temporary file
          shall be located.
          @returns Returns the path to a uniquely named temporary file.
          @throws DaiException(InternalError) if no unique temp file name could be created.
        */static std::string getTmpFilename(const std::string& path);

        /**
          @brief Returns the size of the specified file.
          @param [in]  filename Path to file.
          @returns Returns the file size in bytes.
        */static uint64_t getFileSize(const std::string& filename);

        /**
          @brief Parses strings of format "%Y-%m-%d %H:%M:%S" and "%Y-%m-%dT%H:%M:%S"
          and returns the corresponding time_t.
          @param [in]  formattedString The time as formatted string.
          @returns The time as timestamp.
          @throws Throws a DaiException(InvalidFormatOfIQTarXmlContent) if the string
          does not match the required format.
        */static time_t getTime(const std::string& formattedString);

        /**
          @brief Verifies whether or not the specified file is accessible.
          @param [in]  filename Fully qualified path to the file.
          @returns Returns TRUE if the file can be accessed. If the file does not exists or
          is already opened by another handle FALSE is returned.
        */static bool isFileAccessible(const std::string& filename);

        /**
          @brief Verifies whether or not the specified file is writeable.
          @param [in]  filename Fully qualified path to the file.
          @returns Returns TRUE if the file can be written.
        */static bool isFileWriteable(const std::string& filename);

        /**
          @brief Uses the memory mapped file reader to open the specified file.
          @param [in]  mmf The reader to be used.
          @param [in]  filename The filename to open, UTF-8 encoding.
          @param [in]  mapAll TRUE if the entire l
         */static void mmfOpen(memory_mapped_file::read_only_mmf& mmf, const std::string& filename, bool mapAll);

        /**
          @brief Uses the memory mapped file writer to open the specified file.
          @param [in]  mmf The writer to be used.
          @param [in]  filename The filename to open, UTF-8 encoding.
          @param [in]  exists_mode Handles behavior if file already exists.
          @param [in]  doesnt_exist_mode Handles behavior if file does not exists.
        */static void mmfOpen(memory_mapped_file::writable_mmf& mmf, const std::string& filename, const memory_mapped_file::mmf_exists_mode exists_mode, const memory_mapped_file::mmf_doesnt_exist_mode doesnt_exist_mode);

        /** 
          @brief Opens an fstream.
          @param [in]  stream The stream to be used.
          @param [in]  filename File to be opened, UTF8-encoded.
          @param [in]  mode Open mode.
        */static void streamOpen(std::fstream& stream, const std::string& filename, const std::ios::openmode mode);

        /** 
          @brief Opens an ofstream.
          @param [in]  stream The stream to be used.
          @param [in]  filename File to be opened, UTF8-encoded.
          @param [in]  mode Open mode.
        */static void streamOpen(std::ofstream& stream, const std::string& filename, const std::ios::openmode mode);

        /** 
          @brief Opens a stream.
          @param [in]  stream The stream to be used.
          @param [in]  filename File to be opened, UTF8-encoded.
          @param [in]  mode Open mode.
        */static void streamOpen(std::ifstream& stream, const std::string& filename, const std::ios::openmode mode);

        /**
          @brief Opens an archive from the specified path.
          @param [in]  a Struct representing the archive.
          @param [in]  filename File to be opened, UTF-8 encoded.
          @returns LibArchive error code or ARCHIVE_OK in case of success.
        */static int archiveWriteOpen(struct archive* a, const std::string& filename);
      };
    }
  }
}
