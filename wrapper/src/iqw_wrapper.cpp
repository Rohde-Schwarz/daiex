
#include "iqw_wrapper.h"
#include "helpers.h"
#include "exceptions.h"

#include <vector>
#include "errorcodes.h"

namespace RohdeSchwarz
{
  namespace Mosaik
  {
    namespace DataImportExport
    {
      using namespace System;

      Iqw::Iqw(String^ filename)
      {
        this->nativeImpl_ = new rohdeschwarz::mosaik::dataimportexport::Iqw(Helpers::marshalToUTF8String(filename));
      }

      void Iqw::DataOrder::set(IqDataOrder order)
      {
        rohdeschwarz::mosaik::dataimportexport::IqDataOrder nativeOrder;
        if (order == IqDataOrder::IIIQQQ)
        {
          nativeOrder = rohdeschwarz::mosaik::dataimportexport::IqDataOrder::IIIQQQ;
        }
        else
        {
          nativeOrder = rohdeschwarz::mosaik::dataimportexport::IqDataOrder::IQIQIQ;
        }

        ((rohdeschwarz::mosaik::dataimportexport::Iqw*)this->nativeImpl_)->setDataOrder(nativeOrder);
      }
        
      IqDataOrder Iqw::DataOrder::get()
      {
        rohdeschwarz::mosaik::dataimportexport::IqDataOrder nativeOrder = ((rohdeschwarz::mosaik::dataimportexport::Iqw*)this->nativeImpl_)->getDataOrder();

        if (nativeOrder == rohdeschwarz::mosaik::dataimportexport::IqDataOrder::IIIQQQ)
        {
          return IqDataOrder::IIIQQQ;
        }
        
        return IqDataOrder::IQIQIQ;
      }

      void Iqw::TempDir::set(String^ path)
      {
        int ret = ((rohdeschwarz::mosaik::dataimportexport::Iqw*)this->nativeImpl_)->setTempDir(Helpers::marshalToUTF8String(path));
        if (ret != (rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success))
        {
          std::string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }
      }

      String^ Iqw::TempDir::get()
      {
        return Helpers::marshalUTF8String(((rohdeschwarz::mosaik::dataimportexport::Iqw*)this->nativeImpl_)->getTempDir());
      }
    }
  }
}