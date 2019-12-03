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

#include "common.h"

#include "pugixml.hpp"

#include <iomanip>

#if defined(_WIN32)
#include <windows.h>
#endif

#include "version.h"
#include "errorcodes.h"
#include "daiexception.h"

using namespace std;

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      std::mutex Common::localeLock_;

      std::string Common::getVersion()
      {
        stringstream ss;
        ss << VERMAJOR << "." << VERMINOR << "." << VERBUILD;
        return ss.str();
      }

      IqDataFormat Common::getDataFormatFromString(const std::string& value)
      {
        if (Common::strCmpIgnoreCase(value, "REAL"))
        {
          return IqDataFormat::Real;
        }
        else if (Common::strCmpIgnoreCase(value, "POLAR"))
        {
          return IqDataFormat::Polar;
        }
        else if (Common::strCmpIgnoreCase(value, "COMPLEX"))
        {
          return IqDataFormat::Complex;
        }
        else
        {
          throw DaiException(ErrorCodes::InvalidFormatOfIQTarXmlContent, "unknown IqDataFormat");
        }
      }

      IqDataType Common::getDataTypeFromString(const std::string& value)
      {
        if (Common::strCmpIgnoreCase(value, "FLOAT32"))
        {
          return IqDataType::Float32;
        }
        else if (Common::strCmpIgnoreCase(value, "FLOAT64"))
        {
          return IqDataType::Float64;
        }
        else
        {
          throw DaiException(ErrorCodes::InvalidFormatOfIQTarXmlContent, "unknown IqDataType");
        }
      }

      std::string Common::toString(const time_t time)
      {
        struct tm* tz = gmtime(&time);

        char mbstr[100];
        strftime(mbstr, 100, "%Y-%m-%d %H:%M:%S", tz);

        return string(mbstr);
      }

      bool Common::isValidXml(const std::string& xmlString)
      {
        try
        {
          pugi::xml_document doc;

          // copy string since value is modified by pugi
          string xmlCopy;
          xmlCopy.append(xmlString);
          
          pugi::xml_parse_result res = doc.load_buffer_inplace(const_cast<char*>(xmlCopy.c_str()), xmlCopy.size());
          if (res.status != pugi::status_ok)
          {
            return false;
          }
        }
        catch (...)
        {
          return false;
        }

        return true;
      }

      time_t Common::toUtcTime(struct tm* time)
      {
        int m, y = time->tm_year;

        if ((m = time->tm_mon) <2) 
        {
          m += 12; 
          --y; 
        }

        return ((((time_t)(y-69)*365u+y/4-y/100*3/4+(m+2)*153/5-446+time->tm_mday)*24u+time->tm_hour)*60u+time->tm_min)*60u+time->tm_sec;
      }

#if defined(_WIN32)
      std::string Common::getShortFilePath(const std::string& filename)
      {
        long length = 0;
        wchar_t* buffer = NULL;
          
        std::wstring pathUtf16 = Common::utf8toUtf16(filename);
        length = GetShortPathNameW(pathUtf16.c_str(), nullptr, 0);
        buffer = new wchar_t[length];
        GetShortPathNameW(pathUtf16.c_str(), buffer, length);
        std::string shortFilename = Common::utf16ToUtf8(std::wstring(buffer));
        delete [] buffer;

        return shortFilename;
      }
#endif
    }
  }
}
