
#include "iqcsv_wrapper.h"
#include "helpers.h"
#include "exceptions.h"

#include "errorcodes.h"

namespace RohdeSchwarz
{
  namespace Mosaik
  {
    namespace DataImportExport
    {
      using namespace System;
      using namespace std;

      IqCsv::IqCsv(String^ filename)
      {
        this->nativeImpl_ = new rohdeschwarz::mosaik::dataimportexport::IqCsv(Helpers::marshalToUTF8String(filename));
      }

      Int64 IqCsv::GetNofCols()
      {
        return ((rohdeschwarz::mosaik::dataimportexport::ICsvSelector*)this->nativeImpl_)->getNofCols();
      }

      Int64 IqCsv::GetNofRows(int column)
      {
        return ((rohdeschwarz::mosaik::dataimportexport::ICsvSelector*)this->nativeImpl_)->getNofRows(static_cast<size_t>(column));
      }

      void IqCsv::ReadRawArray(int column, int nofValues, IList<float>^% values)
      {
        this->ReadRawArray(column, nofValues, values, 0);
      }

      void IqCsv::ReadRawArray(int column, int nofValues, IList<float>^% values, int offset)
      {
        vector<float> nativeValues(static_cast<size_t>(nofValues));

        int ret = ((rohdeschwarz::mosaik::dataimportexport::ICsvSelector*)this->nativeImpl_)->readRawArray(
          static_cast<size_t>(column),
          static_cast<size_t>(nofValues),
          nativeValues,
          static_cast<size_t>(offset));

        if (ret != rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success)
        {
          string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }

        if (values->Count != nofValues)
        {
          values = gcnew List<float>(nofValues);
        }

        values->Clear();
        for (int i = 0; i < nofValues; ++i)
        {
          values->Add(nativeValues[i]);
        }
      }

      void IqCsv::ReadRawArray(int column, int nofValues, IList<double>^% values)
      {
        this->ReadRawArray(column, nofValues, values, 0);
      }

      void IqCsv::ReadRawArray(int column, int nofValues, IList<double>^% values, int offset)
      {
        vector<double> nativeValues(static_cast<size_t>(nofValues));

        int ret = ((rohdeschwarz::mosaik::dataimportexport::ICsvSelector*)this->nativeImpl_)->readRawArray(
          static_cast<size_t>(column),
          static_cast<size_t>(nofValues),
          nativeValues,
          static_cast<size_t>(offset));

        if (ret != rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success)
        {
          string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }

        if (values->Count != nofValues)
        {
          values = gcnew List<double>(nofValues);
        }

        values->Clear();
        for (int i = 0; i < nofValues; ++i)
        {
          values->Add(nativeValues[i]);
        }
      }

      void IqCsv::SeparatorConfiguration::set(CsvSeparatorConfig config)
      {
        rohdeschwarz::mosaik::dataimportexport::CsvSeparatorConfig nativeConfig = rohdeschwarz::mosaik::dataimportexport::CsvSeparatorConfig::DecimalColonValueSemicolon;
        switch (config)
        {
        case CsvSeparatorConfig::DecimalColonValueSemicolon:
          nativeConfig = rohdeschwarz::mosaik::dataimportexport::CsvSeparatorConfig::DecimalColonValueSemicolon;
          break;
        case CsvSeparatorConfig::DecimalDotValueSemicolon:
          nativeConfig = rohdeschwarz::mosaik::dataimportexport::CsvSeparatorConfig::DecimalDotValueSemicolon;
          break;
        case CsvSeparatorConfig::DecimalDotValueColon:
          nativeConfig = rohdeschwarz::mosaik::dataimportexport::CsvSeparatorConfig::DecimalDotValueColon;
          break;
        }

        int ret = ((rohdeschwarz::mosaik::dataimportexport::IqCsv*)this->nativeImpl_)->setSeparatorConfiguration(nativeConfig);
        if (ret != rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success)
        {
          string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }
      }
    
      CsvSeparatorConfig IqCsv::SeparatorConfiguration::get()
      {
        rohdeschwarz::mosaik::dataimportexport::CsvSeparatorConfig nativeConfig = ((rohdeschwarz::mosaik::dataimportexport::IqCsv*)this->nativeImpl_)->getSeparatorConfiguration();
        switch (nativeConfig)
        {
          default:
          case rohdeschwarz::mosaik::dataimportexport::DecimalColonValueSemicolon:
            return CsvSeparatorConfig::DecimalColonValueSemicolon;
          case rohdeschwarz::mosaik::dataimportexport::DecimalDotValueSemicolon:
            return CsvSeparatorConfig::DecimalDotValueSemicolon;
          case rohdeschwarz::mosaik::dataimportexport::DecimalDotValueColon:
            return CsvSeparatorConfig::DecimalDotValueColon;
        }
      }

      String^ IqCsv::ValueSeparator::get()
      {
        char value = ((rohdeschwarz::mosaik::dataimportexport::IqCsv*)this->nativeImpl_)->getValueSeparator();
        string strVa(1, value);
        return Helpers::marshalUTF8String(strVa);
      }

      String^ IqCsv::DecimalSeparator::get()
      {
        char value = ((rohdeschwarz::mosaik::dataimportexport::IqCsv*)this->nativeImpl_)->getDecimalSeparator();
        string strVa(1, value);
        return Helpers::marshalUTF8String(strVa);
      }

      void IqCsv::FormatSpecifier::set(String^ format)
      {
        int ret = ((rohdeschwarz::mosaik::dataimportexport::IqCsv*)this->nativeImpl_)->setFormatSpecifier(Helpers::marshalToUTF8String(format));
        if (ret != rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success)
        {
          string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }
      }

      String^ IqCsv::FormatSpecifier::get()
      {
        string nativeFormat = ((rohdeschwarz::mosaik::dataimportexport::IqCsv*)this->nativeImpl_)->getFormatSpecifier();
        return Helpers::marshalUTF8String(nativeFormat);
      }

      void IqCsv::FormatSpecifierChannelInfo::set(String^ format)
      {
        int ret = ((rohdeschwarz::mosaik::dataimportexport::IqCsv*)this->nativeImpl_)->setFormatSpecifierChannelInfo(Helpers::marshalToUTF8String(format));
        if (ret != rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success)
        {
          string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }
      }

      String^ IqCsv::FormatSpecifierChannelInfo::get()
      {
        string nativeFormat = ((rohdeschwarz::mosaik::dataimportexport::IqCsv*)this->nativeImpl_)->getFormatSpecifierChannelInfo();
        return Helpers::marshalUTF8String(nativeFormat);
      }
    }
  }
}