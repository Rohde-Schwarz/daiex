
#include "iqmatlab_wrapper.h"
#include "helpers.h"
#include "exceptions.h"

#include "errorcodes.h"
#include "iarrayselector.h"

namespace RohdeSchwarz
{
  namespace Mosaik
  {
    namespace DataImportExport
    {
      using namespace System;
      using namespace std;

      IqMatlab::IqMatlab(String^ filename)
      {
        this->nativeImpl_ = new rohdeschwarz::mosaik::dataimportexport::IqMatlab(Helpers::marshalToUTF8String(filename));
      }

      void IqMatlab::MatchArrayDimensions(int minCols, int minRows, bool exactColMatch, IList<String ^>^ arrayNames)
      {
        vector<string> names(static_cast<size_t>(arrayNames->Count));
        for each (String^ name in arrayNames)
        {
          names.push_back(Helpers::marshalToUTF8String(name));
        }

        int ret = ((rohdeschwarz::mosaik::dataimportexport::IArraySelector*)this->nativeImpl_)->matchArrayDimensions(
          static_cast<size_t>(minCols), 
          static_cast<size_t>(minRows),
          exactColMatch,
          names);

        if (ret != rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success)
        {
          string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }
      }

      Int64 IqMatlab::GetNofCols(String^ arrayName)
      {
        return ((rohdeschwarz::mosaik::dataimportexport::IArraySelector*)this->nativeImpl_)->getNofCols(Helpers::marshalToUTF8String(arrayName));
      }

      Int64 IqMatlab::GetNofRows(String^ arrayName)
      {
        return ((rohdeschwarz::mosaik::dataimportexport::IArraySelector*)this->nativeImpl_)->getNofRows(Helpers::marshalToUTF8String(arrayName));
      }

      void IqMatlab::ReadRawArray(String^ arrayName, int column, int nofValues, IList<float>^% values)
      {
        this->ReadRawArray(arrayName, column, nofValues, values, 0);
      }

      void IqMatlab::ReadRawArray(String^ arrayName, int column, int nofValues, IList<float>^% values, int offset)
      {
        vector<float> nativeValues(static_cast<size_t>(nofValues));

        int ret = ((rohdeschwarz::mosaik::dataimportexport::IArraySelector*)this->nativeImpl_)->readRawArray(
          Helpers::marshalToUTF8String(arrayName),
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

      void IqMatlab::ReadRawArray(String^ arrayName, int column, int nofValues, IList<double>^% values)
      {
        this->ReadRawArray(arrayName, column, nofValues, values, 0);
      }

      void IqMatlab::ReadRawArray(String^ arrayName, int column, int nofValues, IList<double>^% values, int offset)
      {
        vector<double> nativeValues(static_cast<size_t>(nofValues));

        int ret = ((rohdeschwarz::mosaik::dataimportexport::IArraySelector*)this->nativeImpl_)->readRawArray(
          Helpers::marshalToUTF8String(arrayName),
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

      void IqMatlab::MatlabFileVersion::set(MatlabVersion version)
      {
        rohdeschwarz::mosaik::dataimportexport::MatlabVersion nativeVersion;
        if (version == MatlabVersion::Mat4)
        {
          nativeVersion = rohdeschwarz::mosaik::dataimportexport::MatlabVersion::Mat4;
        }
        else
        {
          nativeVersion = rohdeschwarz::mosaik::dataimportexport::MatlabVersion::Mat73;
        }

        int ret = ((rohdeschwarz::mosaik::dataimportexport::IqMatlab*)this->nativeImpl_)->setMatlabVersion(nativeVersion);
        if (ret != rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success)
        {
          string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }
      }

      MatlabVersion IqMatlab::MatlabFileVersion::get()
      {
        rohdeschwarz::mosaik::dataimportexport::MatlabVersion nativeVersion = ((rohdeschwarz::mosaik::dataimportexport::IqMatlab*)this->nativeImpl_)->getMatlabVersion();
        if (nativeVersion == rohdeschwarz::mosaik::dataimportexport::MatlabVersion::Mat4)
        {
          return MatlabVersion::Mat4;
        }
        else
        {
          return MatlabVersion::Mat73;
        }
      }

      void IqMatlab::TempDir::set(String^ path)
      {
        int ret = ((rohdeschwarz::mosaik::dataimportexport::IqMatlab*)this->nativeImpl_)->setTempDir(Helpers::marshalToUTF8String(path));
        if (ret != (rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success))
        {
          std::string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }
      }

      String^ IqMatlab::TempDir::get()
      {
        return Helpers::marshalUTF8String(((rohdeschwarz::mosaik::dataimportexport::IqMatlab*)this->nativeImpl_)->getTempDir());
      }
    }
  }
}