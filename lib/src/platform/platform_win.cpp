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
#include <iomanip>
#include <fcntl.h>
#include <io.h>
#include <windows.h>

#include "common.h"
#include "daiexception.h"
#include "errorcodes.h"

using namespace std;

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      std::string Platform::getTmpDir()
      {
        TCHAR pathBuf[MAX_PATH];
        memset(pathBuf, 0, sizeof(pathBuf));
        UINT ret = GetTempPath(MAX_PATH, pathBuf);
        if (ret == 0)
        {
          throw DaiException(ErrorCodes::InternalError, "No temp dir found.");
        }

        return string(pathBuf);
      }

      std::string Platform::getTmpFilename(const std::string& path)
      {
        TCHAR nameBuf[MAX_PATH];
        memset(nameBuf, 0, sizeof(nameBuf));
        UINT ret = GetTempFileName(path.c_str(), TEXT("dai"), 0, nameBuf);
        if (ret == 0)
        {
          throw DaiException(ErrorCodes::InternalError, "Error occurred creating unique filename.");
        }

        return string(nameBuf);
      }

      uint64_t Platform::getFileSize(const std::string& filename)
      {
        struct _stat64 st;
        _stat64(filename.c_str(), &st);
        return static_cast<uint64_t>(st.st_size);
      }

      time_t Platform::getTime(const std::string& formattedString)
      {
        struct tm tz;
        istringstream ss(formattedString);
        ss >> get_time(&tz, "%Y-%m-%d %H:%M:%S");

        if (ss.fail())
        {
          ss = istringstream(formattedString);
          ss >> get_time(&tz, "%Y-%m-%dT%H:%M:%S");
          if (ss.fail())
          {
            throw DaiException(ErrorCodes::InvalidFormatOfIQTarXmlContent);
          }
        }
        
        return Common::toUtcTime(&tz);
      }

      bool Platform::isFileAccessible(const std::string& filename)
      {
        std::ifstream value(Common::utf8toUtf16(filename));
        bool good = value.good();
        value.close();

        return good;
      }

      void Platform::mmfOpen(memory_mapped_file::read_only_mmf& mmf, const std::string& filename, bool mapAll)
      {
        mmf.open(Common::utf8toUtf16(filename).c_str(), mapAll);
      }

      void Platform::mmfOpen(memory_mapped_file::writable_mmf& mmf, const std::string& filename, const memory_mapped_file::mmf_exists_mode exists_mode, const memory_mapped_file::mmf_doesnt_exist_mode doesnt_exist_mode)
      {
        mmf.open(Common::utf8toUtf16(filename).c_str(), exists_mode, doesnt_exist_mode);
      }

      void Platform::streamOpen(std::fstream& stream, const std::string& filename, const std::ios::openmode mode)
      {
        stream.open(Common::utf8toUtf16(filename).c_str(), mode);
      }

      void Platform::streamOpen(std::ifstream& stream, const std::string& filename, const std::ios::openmode mode)
      {
        stream.open(Common::utf8toUtf16(filename).c_str(), mode);
      }

      void Platform::streamOpen(std::ofstream& stream, const std::string& filename, const std::ios::openmode mode)
      {
        stream.open(Common::utf8toUtf16(filename).c_str(), mode);
      }

      int Platform::archiveWriteOpen(struct archive* a, const std::string& filename) 
      {
        return archive_write_open_filename_w(a, Common::utf8toUtf16(filename).c_str());
      }
    }
  }
}
