#ifndef _COMMON_H_
#define _COMMON_H_

#include <string>
#include <vector>
#include <type_traits>
#include <fstream>
#include <mutex>

#if defined(_WIN32)
#include <codecvt>
#endif

#include "enums.h"

using namespace std;

class Common final
{
public:
  static std::mutex localeLock_;

#if defined(_WIN32)
  static inline std::wstring utf8toUtf16(const std::string& utf8Src) 
  {
    Common::localeLock_.lock();
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    auto res = converter.from_bytes(utf8Src.data());
    Common::localeLock_.unlock();

    return res;
  }

  static inline std::string utf16ToUtf8(const std::wstring& utf16Src)
  {
    Common::localeLock_.lock();
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    auto res = converter.to_bytes(utf16Src.data());
    Common::localeLock_.unlock();

    return res;
  }

  static inline bool isFileAccessible(const std::string& filename)
  {
    std::ifstream value(Common::utf8toUtf16(filename));
    bool good = value.good();
    value.close();

    return good;
  }
#else
  static inline bool isFileAccessible(const std::string& filename)
  {
    std::ifstream value(filename);
    bool good = value.good();
    value.close();

    return good;
  }
#endif

  template<typename T>
  static void initVector(std::vector<std::vector<T>>& data, size_t nofArrays, size_t nofValues, size_t valueOffset = 0)
  {
    for (size_t a = 0; a < nofArrays; ++a)
    {
      vector<T> val;
      T s = 0.00001;

      val.reserve(nofValues);
      for (size_t v = valueOffset; v < nofValues + valueOffset; ++v)
      {
        val.push_back(a + v * s);
      }

      data.push_back(val);
      valueOffset += nofValues;
    }
  }

  static inline void strReplace(std::string& src, const std::string& fnd, const std::string& rep)
  {
    std::string::size_type pos = src.find(fnd);
    while(pos != std::string::npos)
    {
      src.replace(pos, fnd.length(), rep);
      pos = src.find(fnd, pos + rep.length());
    }
  }

  static inline bool strEndsWithIgnoreCase(const std::string& input, const std::string& substring)
  {
    return input.size() >= substring.size() 
      && equal(substring.rbegin(), substring.rend(), input.rbegin(), [](std::string::value_type l1, std::string::value_type r1) { return tolower(l1) == tolower(r1); });
  }

  static inline bool strStartsWithIgnoreCase(const std::string& input, const std::string& substring)
  {
    return input.size() >= substring.size() 
      && equal(substring.begin(), substring.end(), input.begin(), [](std::string::value_type l1, std::string::value_type r1) { return tolower(l1) == tolower(r1); });
  }

  template<typename T>
  static void strSplit(const std::basic_string<T>& s, T c, std::vector<std::basic_string<T> >& v) 
  {
    typename std::basic_string<T>::size_type i = 0;
    typename std::basic_string<T>::size_type j = s.find(c);

    while (j != std::basic_string<T>::npos) 
    {
      v.push_back(s.substr(i, j-i));
      i = ++j;
      j = s.find(c, j);

      if (j == std::basic_string<T>::npos)
      {
        v.push_back(s.substr(i, s.length()));
      }
    }
  }

  template<typename T, typename T2>
  static inline void almostEqual(const std::vector<T>& a, const std::vector<T2>& b, double precision = 0.00001)
  {
    ASSERT_EQ(a.size(), b.size());

    for (size_t i = 0; i < a.size(); ++i)
    {
      ASSERT_NEAR(a[i], b[i], precision); 
    }
  }

  template<typename T, typename T2>
  static inline void almostEqual(const T* a, const T2* b, size_t size, double precision = 0.00001)
  {
    for (size_t i = 0; i < size; ++i)
    {
      ASSERT_NEAR(a[i], b[i], precision); 
    }
  }

  static bool binCompareFiles(const std::string& file1, const std::string& file2);

  static std::string TestDataDir;
  static std::string TestOutputDir;
};

#endif