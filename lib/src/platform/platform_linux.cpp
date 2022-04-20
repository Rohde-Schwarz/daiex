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

#include "platform.h"

#include <sstream>
#include <time.h>

#include "daiexception.h"
#include "errorcodes.h"
#include "common.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      std::string Platform::getTmpDir()
      {
        std::string tmpPath(P_tmpdir);
        if (tmpPath.empty())
        {
          throw DaiException(ErrorCodes::InternalError, "No temp dir found");
        }
        
        return tmpPath;
      }

      std::string Platform::getTmpFilename(const std::string& path)
      {
        char * val = tempnam(path.c_str(), "");
        std::string tmpFilePath(val);
        free(val);

        if (tmpFilePath.empty())
        {
          throw DaiException(ErrorCodes::InternalError, "Error occurred creating unique filename.");
        }

        return tmpFilePath;
      }

      uint64_t Platform::getFileSize(const std::string& filename)
      {
        struct stat st;
        stat(filename.c_str(), &st);
        return st.st_size;
      }

      time_t Platform::getTime(const std::string& formattedString)
      {
        struct tm tz;
        if (nullptr == strptime(formattedString.c_str(), "%Y-%m-%d %H:%M:%S", &tz))
        {
          if (nullptr == strptime(formattedString.c_str(), "%Y-%m-%dT%H:%M:%S", &tz))
          {
            throw DaiException(ErrorCodes::InvalidFormatOfIQTarXmlContent);
          }
        }

        return Common::toUtcTime(&tz);
      }

      bool Platform::isFileAccessible(const std::string& filename)
      {
        std::ifstream value(filename);
        bool good = value.good();
        value.close();

        return good;
      }

      bool Platform::isFileWriteable(const std::string& filename)
      {
         // if file exists: remove
         if (Platform::isFileAccessible(filename.c_str()))
         {
            // stop if file could not be deleted
            if (0 != remove(filename.c_str()))
            {
               return false;
            }
         }
         // create file
         std::ofstream value(filename.c_str());
         if (!value.good())
         {
            return false;
         }
         // close file
         value.close();
         if (!value.good())
         {
            return false;
         }
         // remove file
         if (Platform::isFileAccessible(filename.c_str()))
         {
            // stop if file could not be deleted
            if (0 != remove(filename.c_str()))
            {
               return false;
            }
         }
         return true;
      }

      void Platform::mmfOpen(memory_mapped_file::read_only_mmf& mmf, const std::string& filename, bool mapAll)
      {
        mmf.open(filename.c_str(), mapAll);
      }

      void Platform::mmfOpen(memory_mapped_file::writable_mmf& mmf, const std::string& filename, const memory_mapped_file::mmf_exists_mode exists_mode, const memory_mapped_file::mmf_doesnt_exist_mode doesnt_exist_mode)
      {
        mmf.open(filename.c_str(), exists_mode, doesnt_exist_mode);
      }

      void Platform::streamOpen(std::fstream& stream, const std::string& filename, const std::ios::openmode mode)
      {
        stream.open(filename.c_str(), mode);
      }

      void Platform::streamOpen(std::ifstream& stream, const std::string& filename, const std::ios::openmode mode)
      {
        stream.open(filename.c_str(), mode);
      }

      void Platform::streamOpen(std::ofstream& stream, const std::string& filename, const std::ios::openmode mode)
      {
        stream.open(filename.c_str(), mode);
      }

      int Platform::archiveWriteOpen(struct archive* a, const std::string& filename)
      {
        return archive_write_open_filename(a, filename.c_str());
      }
    }
  }
}
