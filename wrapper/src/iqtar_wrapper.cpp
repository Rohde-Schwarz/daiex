
#include "iqtar_wrapper.h"
#include "helpers.h"
#include "exceptions.h"

#include <vector>
#include <map>
#include "errorcodes.h"

namespace RohdeSchwarz
{
  namespace Mosaik
  {
    namespace DataImportExport
    {
      using namespace System;

      IqTar::IqTar(String^ filename)
      {
        this->nativeImpl_ = new rohdeschwarz::mosaik::dataimportexport::IqTar(Helpers::marshalToUTF8String(filename));
      }

      bool IqTar::CalculateIqPreview::get()
      {
        return ((rohdeschwarz::mosaik::dataimportexport::IqTar*)this->nativeImpl_)->getPreviewEnabled();
      }

      void IqTar::CalculateIqPreview::set(bool enable)
      {
        int ret = ((rohdeschwarz::mosaik::dataimportexport::IqTar*)this->nativeImpl_)->setPreviewEnabled(enable);
        if (ret != rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success)
        {
          std::string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }
      }

      String^ IqTar::DeprecatedInfo::get()
      {
        std::string xmlString;
        ((rohdeschwarz::mosaik::dataimportexport::IqTar*)this->nativeImpl_)->getDeprecatedInfo(xmlString);

        return Helpers::marshalUTF8String(xmlString);
      }

      void IqTar::TempDir::set(String^ path)
      {
        int ret = ((rohdeschwarz::mosaik::dataimportexport::IqTar*)this->nativeImpl_)->setTempDir(Helpers::marshalToUTF8String(path));
        if (ret != (rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success))
        {
          std::string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }
      }

      String^ IqTar::TempDir::get()
      {
        return Helpers::marshalUTF8String(((rohdeschwarz::mosaik::dataimportexport::IqTar*)this->nativeImpl_)->getTempDir());
      }

      void IqTar::WriteOpen(
        IqDataFormat format,
        int nofArrays,
        String^ applicationName,
        String^ comment,
        IList<ChannelInfo^>^ channelInfos,
        IDictionary<String^, String^>^ metadata,
        String^ deprecatedInfo)
      {
        rohdeschwarz::mosaik::dataimportexport::IqDataFormat nativeFormat;
        if (format == IqDataFormat::Complex)
        {
          nativeFormat = rohdeschwarz::mosaik::dataimportexport::IqDataFormat::Complex;
        }
        else if (format == IqDataFormat::Polar)
        {
          nativeFormat = rohdeschwarz::mosaik::dataimportexport::IqDataFormat::Polar;
        }
        else
        {
          nativeFormat = rohdeschwarz::mosaik::dataimportexport::IqDataFormat::Real;
        }

        std::vector<rohdeschwarz::mosaik::dataimportexport::ChannelInfo> nativeChannels;
        nativeChannels.reserve(channelInfos->Count);
        for each (ChannelInfo^ channelInfo in channelInfos)
        {
          nativeChannels.push_back(rohdeschwarz::mosaik::dataimportexport::ChannelInfo(
            Helpers::marshalToUTF8String(channelInfo->ChannelName), 
            channelInfo->ClockRate,
            channelInfo->Frequency));
        }

        std::map<std::string, std::string> nativeMetadata;
        for each (KeyValuePair<String^, String^> kvp in metadata)
        {
          nativeMetadata.insert(make_pair(Helpers::marshalToUTF8String(kvp.Key), Helpers::marshalToUTF8String(kvp.Value)));
        }

        int ret = ((rohdeschwarz::mosaik::dataimportexport::IqTar*)this->nativeImpl_)->writeOpen(
          nativeFormat,
          static_cast<size_t>(nofArrays),
          Helpers::marshalToUTF8String(applicationName),
          Helpers::marshalToUTF8String(comment),
          nativeChannels,
          nativeMetadata,
          Helpers::marshalToUTF8String(deprecatedInfo));

        if (ret != rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success)
        {
          std::string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }
      }

      void IqTar::DisableTempFile(uint64_t nofIqValues, size_t nofChannels, IqDataFormat format, IqDataType dataType)
      {
        rohdeschwarz::mosaik::dataimportexport::IqDataFormat nativeFormat;
        if (format == IqDataFormat::Complex)
        {
          nativeFormat = rohdeschwarz::mosaik::dataimportexport::IqDataFormat::Complex;
        }
        else if (format == IqDataFormat::Polar)
        {
          nativeFormat = rohdeschwarz::mosaik::dataimportexport::IqDataFormat::Polar;
        }
        else
        {
          nativeFormat = rohdeschwarz::mosaik::dataimportexport::IqDataFormat::Real;
        }

        rohdeschwarz::mosaik::dataimportexport::IqDataType nativeType;
        if (dataType == IqDataType::Float32)
        {
          nativeType = rohdeschwarz::mosaik::dataimportexport::IqDataType::Float32;
        }
        else
        {
          nativeType = rohdeschwarz::mosaik::dataimportexport::IqDataType::Float64;
        }

        int ret = ((rohdeschwarz::mosaik::dataimportexport::IqTar*)this->nativeImpl_)->disableTempFile(nofIqValues, nofChannels, nativeFormat, nativeType);

        if (ret != rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success)
        {
          std::string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }
      }
    }
  }
}