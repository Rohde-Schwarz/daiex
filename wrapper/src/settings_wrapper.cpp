#include "settings_wrapper.h"
#include "helpers.h"

namespace RohdeSchwarz
{
  namespace Mosaik
  {
    namespace DataImportExport
    {
      using namespace System;

      void Settings::BufferSize::set(size_t size)
      {
        rohdeschwarz::mosaik::dataimportexport::Settings::setBufferSize(size);
      }
        
      size_t Settings::BufferSize::get()
      {
        return rohdeschwarz::mosaik::dataimportexport::Settings::getBufferSize();
      }
    }
  }
}