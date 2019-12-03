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
* @file      common.h
*
* @brief     This is the header file of class Common.
*
* @details   Class to group all common helper functions.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#ifdef _MSC_VER
__pragma(warning(push))
__pragma(warning(disable: 4244))
__pragma(warning(disable: 4996))
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreorder"
#endif

#include <algorithm>
#include <functional>
#include <string>
#include <vector>
#include <complex>
#include <fstream>
#include <mutex>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _MSC_VER
#pragma warning (pop)
#elif __GNUC__
#pragma GCC diagnostic pop
#endif

#ifdef WIN32
#include <codecvt>
#endif

#include "archive.h"
#include "memory_mapped_file.hpp"

#include "enums.h"
#include "daiexception.h"
#include "errorcodes.h"
#include "platform.h"
#include "stride_iterator.h"
#include "channelinfo.h"

/** @brief definition of white spaces */
#define white_space(c) ((c) == ' ' || (c) == '\t')

/** @brief definition of digits */
#define valid_digit(c) ((c) >= '0' && (c) <= '9')

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief Class to group all common helper functions.
      */class Common final
      {
      public:

        /**
          @returns Returns a string representing the current version of LibDataImportExport, formatted
          as MAJOR_VERSION.MINOR_VERSION.BUILD_NO. See version.h for the actual version information.
        */static std::string getVersion();

        /**
          @brief Converts a string-representation of IqDataFormat to the corresponding enum-value.
          @param [in]  value String representation of an IqDataFormat.
          @returns The corresponding upper-case IqDataFormat enum-value, i.e. 'REAL'.
          @throws DaiException(InvalidFormatOfIQTarXmlContent) if the value could not be resolved.
        */static IqDataFormat getDataFormatFromString(const std::string& value);

        /**
          @brief Converts a string-representation of IqDataType to the corresponding enum-value.
          @param [in]  value String representation of an IqDataFormat.
          @returns The corresponding upper-case IqDataType enum-value, e.g. 'FLOAT32'.
          @throws DaiException(InvalidFormatOfIQTarXmlContent) if the value could not be resolved.
        */static IqDataType getDataTypeFromString(const std::string& value);

        /**
          @brief Converts the specified timestamp to a string representation with format ""%Y-%m-%d %H:%M:%S".
          @param [in]  time The timestamp to convert.
          @returns Returns a formatted string representation of the timestamp.
        */static std::string toString(const time_t time);

        /**
          @brief Verifies if the specified string represents a valid XML structure.
          @param [in]  xmlString The string to be checked.
          @returns TRUE if the specified string represents a valid XML, otherwise
          FALSE is returned.
        */static bool isValidXml(const std::string& xmlString);

        /** 
          @brief Verifies whether or not the specified folder exists.
          @param [in]  path Fully qualified path.
          @returns Returns TRUE is the folder exists, otherwise FALSE is returned.
        */static inline bool directoryExists(const std::string& path)
        {
          struct stat info;
          if (stat(path.c_str(), &info) != 0)
          {
            return false;
          }
          else if (info.st_mode & S_IFDIR)
          {
            return true;
          }
          else
          {
            return false;
          }
        }

        /**
          @brief Returns the size of the specified file.
          @param [in]  filename Fully qualified path to the file.
          @returns The file size in bytes.
        */static inline size_t getFileSize(const std::string& filename)
        {
          std::ifstream value(filename, std::ifstream::ate | std::ifstream::binary);
          size_t size = static_cast<size_t>(value.tellg());
          value.close();

          return size;
        }

#if defined(_WIN32)
        /**
          @brief Converts the UTF-8 encoded source string to an UTF-16 encoded wstring.
          @param [in]  utf8Src UTF-8 encoded input string.
          @returns The UTF-16 encoded wstring, containing the source content.
          @throws DaiException::InternalError in any error case.
        */static inline std::wstring utf8toUtf16(const std::string& utf8Src) 
        {
          try
          {
            Common::localeLock_.lock();
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            auto ret = converter.from_bytes(utf8Src.data());
            Common::localeLock_.unlock();

            return ret;
          }
          catch (...)
          {
            throw DaiException(ErrorCodes::InternalError);
          }
        }

        /**
          @brief Converts the UTF-16 encoded source wstring to an UTF-8 encoded string.
          @param [in]  utf16Src UTF-16 encoded input wstring.
          @returns The UTF-8 encoded string, containing the source content. 
          @throws DaiException::InternalError in any error case.
        */static inline std::string utf16ToUtf8(const std::wstring& utf16Src)
        {
          try
          {
            Common::localeLock_.lock();
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            Common::localeLock_.unlock();

            return converter.to_bytes(utf16Src.data());
          }
          catch (...)
          {
            throw DaiException(ErrorCodes::InternalError);
          }
        }

        /**
          @brief Returns the short windows file path version (i.e. c:\Progra~1\) of the specified path.
          @param [in]  The fully qualified file path.
          @returns The short windows version of the path.
        */static std::string getShortFilePath(const std::string& filename);
#endif

        /** 
          @brief Returns the filename without file extension of the specified path.
          @param [in]  path The fully qualified file path.
          @returns Filename without extension.
        */static inline std::string getBaseNameWithoutExtension(const std::string& path)
        {
          std::string filename = path.substr(path.find_last_of("/\\") + 1);

          // remove iq.tar
          std::string::size_type p(filename.find_last_of('.'));
          filename = p > 0 && p != std::string::npos ? filename.substr(0, p) : filename;
          p = filename.find_last_of('.');
          return p > 0 && p != std::string::npos ? filename.substr(0, p) : filename;
        }

        /**
         * @brief Returns the path without the filename.
         * @param filepath Fully quallified file path.
         * @return Returns the path only.
         */
        static inline std::string getPath(const std::string& filepath)
        {
          return filepath.substr(0, filepath.find_last_of("/\\"));
        }

        /**
          @brief In-place replacement of all occurrences of the specified string with the new string.
          @param [in]  src String that is searched for value fnd.
          @param [in]  fnd Occurrence to be replaced.
          @param [in]  rep The replacement string of fnd.
        */static void strReplace(std::string& src, const std::string& fnd, const std::string& rep)
        {
          std::string::size_type pos = src.find(fnd);
          while(pos != std::string::npos)
          {
            src.replace(pos, fnd.length(), rep);
            pos = src.find(fnd, pos + rep.length());
          }
        }

        /**
          @brief A sequence of calls to this function splits str into tokens, which are sequences of contiguous 
          characters separated by any of the characters that are part of delimiters.
          @tparam Template parameter of std::basic_string<T>.
          @param [in]  str String to be tokenized.
          @param [in]  delimiter Character containing the delimiter. 
          @param [in]  result Vector containing all found tokens.
        */template<typename T>
        static void strSplit(const std::basic_string<T>& str, T delimiter, std::vector<std::basic_string<T> >& result) 
        {
          if (result.size() > 0)
          {
            result.clear();
          }
          
          typename std::basic_string<T>::size_type i = 0;
          typename std::basic_string<T>::size_type j = str.find(delimiter);

          while (j != std::basic_string<T>::npos) 
          {
            result.push_back(str.substr(i, j-i));
            i = ++j;
            j = str.find(delimiter, j);
            
            if (j == std::basic_string<T>::npos)
            {
              result.push_back(str.substr(i, str.length()));
            }
          }
        }

        /**
          @brief Verifies whether or not a string starts with the specified sub-string. The case of the strings is ignored.
          @param [in]  input The source string.
          @param [in]  substring The sub-string that is supposed to terminate the input string.
          @returns Returns TRUE if the input string starts with substring, otherwise FALSE is returned.
        */static inline bool strStartsWithIgnoreCase(const std::string& input, const std::string& substring)
        {
          return input.size() >= substring.size()
            && equal(substring.begin(), substring.end(), input.begin(), [](std::string::value_type l1, std::string::value_type r1) { return tolower(l1) == tolower(r1); });
        }

        /**
          @brief Verifies whether or not a string ends with the specified sub-string. The case of the strings is ignored.
          @param [in]  input The source string.
          @param [in]  substring The sub-string that is supposed to terminate the input string.
          @returns Returns TRUE if the input string ends with substring, otherwise FALSE is returned.
        */static inline bool strEndsWithIgnoreCase(const std::string& input, const std::string& substring)
        {
          return input.size() >= substring.size()
            && equal(substring.rbegin(), substring.rend(), input.rbegin(), [](std::string::value_type l1, std::string::value_type r1) { return tolower(l1) == tolower(r1); });
        }

        /**
          @brief Compares two strings. The case of the strings is ignored.
          @param [in]  a Input string a.
          @param [in]  b Input string b.
          @returns Returns TRUE if a and b are equal, otherwise FALSE is returned.
        */static inline bool strCmpIgnoreCase(const std::string& a, const std::string& b)
        {
          if (a.length() == b.length())
          {
            return std::equal(b.begin(), b.end(), a.begin(), [](std::string::value_type va, std::string::value_type vb){ return tolower(va) == tolower(vb); });
          }

          return false;
        }

        /**
          @brief In-place removal of leading and trailing character.
          @param [in]  value The string to be processed.
          @param [in]  t The character to be removed.
          @returns Returns the string with removed character.
        */static inline std::string& trim(std::string& value, const char* t)
        {
          // left trim
          value.erase(0, value.find_first_not_of(t));

          // right trim
          return value.erase(value.find_last_not_of(t) + 1);
        }

        /**
          @brief Replaces all "special characters" of the specified string for XML compatibility,
          i.e. "&" is replaced by "&amp;".
          @param [in]  src The source string.
          @returns The escaped string, which is ready to use in XML sequences.
        */static inline void escapeStrToXml(std::string& src)
        {
          Common::strReplace(src, "&", "&amp;");
          Common::strReplace(src, "<", "&lt;");
          Common::strReplace(src, ">", "&gt;");
          Common::strReplace(src, "\"", "&quot;");
          Common::strReplace(src, "\'", "&apos;");
        }

        /**
          @brief Replaces all escaped "special characters" of the specified input XML string,
          i.e. "&amp;" is replaced by "&".
          @param [in]  src The XML string.
          @returns The string without escaped symbols.
        */static inline void escapeXmlToStr(std::string& src)
        {
          Common::strReplace(src, "amp;", "&");
          Common::strReplace(src, "lt;", "<");
          Common::strReplace(src, "gt;", ">");
          Common::strReplace(src, "quot;", "\"");
          Common::strReplace(src, "apos;", "\'");
        }

        /**
          @brief Provides pointers to the I/Q data arrays from the specified iqdata alongside with the
          length if each array.
          @tparam Template parameter of the I/Q data precision, i.e. float or double.
          @param [in]  iqdata Input I/Q data vector, where each vector element represents an I/Q data vector.
          @param [out]  dataPtrs Vector holding pointers to the first element of each I/Q data vector.
          @param [out]  sizes Vector containing the corresponding sizes of each data vector.
          @returns 
        */template<typename T>
        static inline void getVectorOfPtrAndSize(const std::vector<std::vector<T>>& iqdata, std::vector<T*>& dataPtrs, std::vector<size_t>& sizes)
        {
          dataPtrs.reserve(iqdata.size());
          sizes.reserve(iqdata.size());

          for (size_t i = 0; i < iqdata.size(); ++i)
          {
            dataPtrs.push_back(const_cast<T*>(iqdata[i].data()));
            sizes.push_back(iqdata[i].size());
          }
        }

        /**
          @brief Combines the data of two input arrays to an vector of complex values.
          @tparam T Type of complex data, i.e. float for complex<float>.
          @tparam InputIterator Type of input iterator.
          @param [in]  first1 Start position of the array containing real-valued data.
          @param [in]  last1 End position of the array containing real-valued data.
          @param [in]  first2 Start position of the array containing imaginary data.
          The length of the array must equal the length of the real-valued data array.
          @param [out]  complexData Vector containing the complex result.
        */template<typename T, typename InputIterator>
        static inline void toComplex(InputIterator first1, InputIterator last1, InputIterator first2, std::vector<std::complex<T>>& complexData)
        {
          auto d = std::distance(first1, last1);
          complexData.reserve(d);
          std::transform(first1, last1, first2, std::back_inserter(complexData), [](T re, T im) { return std::complex<T>(re, im); });
        }

        /**
          @brief Merges the two data arrays to one interleaved arrays, i.e. first1 = I1I2I3 and first2 = Q1Q2Q3
          will be merged to I1Q1I2Q2I3Q3. Both arrays must have the same size.
          @tparam Template parameter of the I/Q data precision, i.e. float or double.
          @param [in]  first1 Start position of first data.
          @param [in]  last1 End position of first data.
          @param [in]  first2 Start position of second data. Second data must be of same length than first data. 
          @param [out]  dest Merged output data. Make sure to provide sufficient memory (2 * first1.size()).
        */template<typename InputIterator, typename OutputIterator>
        static inline void mergeInterleaved(InputIterator first1, InputIterator last1, InputIterator first2, OutputIterator dest)
        {
          while (first1 != last1)
          {
            *dest = *first1;
            dest++;
            first1++;

            *dest = *first2;
            dest++;
            first2++;
          }
        }

        /**
          @brief Copies one value from src to dest and then skips ignoreNofChannelValues values. The procedure
          is repeated until nofValues is reached. The precision of the source data is converted to the
          precision of the destination array.
          @tparam T Precision of source I/Q data.
          @tparam T2 Precision of destination I/Q data.
          @param [in]  src Start position of input data.
          @param [in]  dest Start position of output data.
          @param [in]  nofValues Number of values read from source, including ignoreNofChannelValues.
          @param [in]  ignoreNofChannelValues Number of values to be ignored after reading a single source value.
          @returns 
        */template<typename T, typename T2>
        static inline void strideCopy(T* src, T2* dest, size_t nofValues, size_t ignoreNofChannelValues)
        {
          for (long i = -1, srcPos = 0; i < (long)nofValues - 1;)
          {
            dest[++i] = static_cast<T2>(src[srcPos++]);
            srcPos += ignoreNofChannelValues;
          }
        }

        /**
          @brief Copies a pair of I/Q-values from src to dest and then skips ignoreNofChannelValues. The procedure
          is repeated until nofValues is reached. The precision of the source data is converted to the
          precision of the destination array.
          @tparam T Precision of source I/Q data.
          @tparam T2 Precision of destination I/Q data.
          @param [in]  src Start position of input data.
          @param [in]  dest Start position of output data.
          @param [in]  nofValues Number of values read from source, including ignoreNofChannelValues.
          @param [in]  ignoreNofChannelValues Number of values to be ignored after reading a single source value.
        */template<typename T, typename T2>
        static inline void strideCopyIqPairs(T* src, T2* dest, size_t nofValues, size_t ignoreNofChannelValues)
        {
          for (long i = -1, srcPos = 0; i < (long)nofValues - 1;)
          {
            dest[++i] = static_cast<T2>(src[srcPos++]);
            dest[++i] = static_cast<T2>(src[srcPos++]);
            srcPos += ignoreNofChannelValues;
          }
        }

        /**
          @brief Splits the input array, which contains I/Q data in interleaved format (IQIQIQ)
          into two separate arrays, containing I values and Q values. Make sure to provide
          sufficiently sized destination arrays.
          @tparam T1 Template parameter defining the precision of the incoming interleaved I/Q data, 
          i.e. float or double.
          @tparam T2 Template parameter defining the precision of the destination array, i.e. float or double.
          @param [in]  src Input array containing interleaved data.
          @param [out]  dest Destination arrays of I data.
          @param [out]  dest2 Destination array of Q data.
          @param [in]  nofValues Number of value contained in src array.
        */template<typename T, typename T2>
        static inline void split(T* src, T2* dest, T2* dest2, size_t nofValues)
        {
          for (size_t pos = 0, i = 0; i < nofValues; pos++, i += 2)
          {
            dest[pos] = static_cast<T2>(src[i]);
            dest2[pos] = static_cast<T2>(src[i + 1]);
          }
        }

        /**
          @brief Verifies that the specified result code equals ARCHIVE_OK.
          @param [in]  res LibArchive result code.
          @throws Throws a DaiException(InvalidTarArchive) if res does not
          equal ARCHIVE_OK.
        */static inline void archiveAssert(int res)
        {
          if (res != ARCHIVE_OK)
          {
            throw DaiException(ErrorCodes::InvalidTarArchive);
          }
        }

        /**
          @brief Verifies if a memory block has been mapped for the specified memory mapped file.
          @param [in]  mmf Read-only memory mapped file.
          @throws Throws a DaiException(InternalError) if mmf is NULL or no data has been mapped.
        */static inline void mmfDataAssert(const memory_mapped_file::read_only_mmf& mmf)
        {
          if (mmf.data() == nullptr || mmf.mapped_size() == 0)
          {
            throw DaiException(ErrorCodes::InternalError);
          }
        }

        /**
          @brief Verifies if a memory block has been mapped for the specified memory mapped file.
          @param [in]  mmf Writable memory mapped file.
          @throws Throws a DaiException(InternalError) if mmf is NULL or no data has been mapped.
        */static inline void mmfDataAssert(memory_mapped_file::writable_mmf& mmf)
        {
          if (mmf.data() == nullptr || mmf.mapped_size() == 0)
          {
            throw DaiException(ErrorCodes::InternalError);
          }
        }

        /**
          @brief Converts the broken-down time to time_t in UTC.
          @param [in]  time Input time.
          @returns Converted time.
        */static time_t toUtcTime(struct tm* time);

        /**
          @brief Converts the specified string to a decimal number (e.g. float or double).
          The decimal separator specifies whether a ',' or a '.' is used to encode decimal values.
          Based on http://pastebin.com/dHP1pgQ4.
          @tparam The precision of the destination format, e.g. float or double.
          @returns The decimal number that represents the input string. If the string could not be 
          converted to a decimal value, std::numeric_limits<T>::quiet_NaN() is returned.
        */template<typename T>
        static T toDecimal(const char *p, const char decimalSeparator) 
        {
          // skip leading white space, if any.
          while (white_space(*p) ) 
          {
            p += 1;
          }

          // counter to check how many numbers we got!
          T r = 0.0;
          int c = 0; 

          // Get the sign
          bool neg = false;
          if (*p == '-') 
          {
            neg = true;
            ++p;
          }
          else if (*p == '+')
          {
            neg = false;
            ++p;
          }

          // Get the digits before decimal point
          while (valid_digit(*p)) 
          {
            r = (r * 10.0) + (*p - '0');
            ++p; 
            ++c;
          }

          // Get the digits after decimal point
          if (*p == decimalSeparator) 
          {
            T f = 0.0;
            T scale = 1.0;
            ++p;
            while (*p >= '0' && *p <= '9') 
            {
              f = (f * 10.0) + (*p - '0');
              ++p;
              scale*=10.0;
              ++c;
            }

            r += f / scale;
          }

          // first check
          // we got no decimal! this cannot be any number!
          if (c == 0)
          {
            return std::numeric_limits<T>::quiet_NaN();
          } 

          // Get the digits after the "e"/"E" (exponent)
          if (*p == 'e' || *p == 'E')
          {
            unsigned int e = 0;

            bool negE = false;
            ++p;
            if (*p == '-') 
            {
              negE = true;
              ++p;
            }
            else if(*p == '+')
            {
              negE = false;
              ++p;
            }
            
            // Get exponent
            c = 0;
            while (valid_digit(*p)) 
            {
              e = (e * 10) + (*p - '0');
              ++p; 
              ++c;
            }

            if (!neg && static_cast<T>(e) > std::numeric_limits<T>::max_exponent10)
            {
              e = std::numeric_limits<T>::max_exponent10;
            }
            else if (static_cast<T>(e) < std::numeric_limits<T>::min_exponent10 )
            {
              e = std::numeric_limits<T>::max_exponent10;
            }
            
            // second check
            // we got no exponent! this was not intended!!
            if (c == 0)
            { 
              return std::numeric_limits<T>::quiet_NaN();
            } 

            T scaleE = 1.0;
            
            // Calculate scaling factor.
            while (e >= 50) 
            { 
              scaleE *= 1E50; 
              e -= 50; 
            }

            while (e >= 8) 
            { 
              scaleE *= 1E8;  
              e -=  8; 
            }

            while (e > 0) 
            { 
              scaleE *= 10.0; 
              e -=  1; 
            }

            if (negE)
            {
              r /= scaleE;
            }
            else
            {
              r *= scaleE;
            }
          }

          // skip post whitespace
          while(white_space(*p))
          {
            ++p;
          }

          // error if next character is not the terminating character
          if(*p != '\0' && *p != '\r' && *p != '\n')
          {
            return std::numeric_limits<T>::quiet_NaN();
          } 

          // Apply sign to number
          if(neg)
          {
            r = -r;
          }

          return r;
        }

        /** @brief Protects setlocale function, e.g. used in libarchive functions and utf8-conversion.
        Setlocale is not thread-safe, but will probably be removed in libarchive version 4.
        */static std::mutex localeLock_;
      };
    }
  }
}
