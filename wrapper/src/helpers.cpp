#include "helpers.h"

#include <msclr/marshal.h>
#include <msclr/marshal_cppstd.h>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;
using namespace msclr::interop;

namespace RohdeSchwarz
{
  namespace Mosaik
  {
    namespace DataImportExport
    {
      String^ Helpers::marshalUTF8String(const std::string &value)
      {
        size_t byteCount = value.size();
        if (byteCount == 0)
        {
          return String::Empty;
        }

        array<unsigned char>^ bytes = gcnew array<unsigned char>(static_cast<int>(byteCount));
        {
          pin_ptr<unsigned char> pinnedBytes = &bytes[0];
          memcpy(pinnedBytes, value.c_str(), byteCount);
        }

        return System::Text::Encoding::UTF8->GetString(bytes);
      }

      std::string Helpers::marshalToUTF8String(System::String^ value)
      {
        if (String::IsNullOrEmpty(value))
        {
          return "";
        }

        array<unsigned char>^ bytes = System::Text::Encoding::UTF8->GetBytes(value);
        pin_ptr<unsigned char> pinnedBytes = &bytes[0];

        return std::string(reinterpret_cast<char*>(pinnedBytes), bytes->Length);
      }
    }
  }
}