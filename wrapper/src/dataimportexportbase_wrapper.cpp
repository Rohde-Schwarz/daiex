#include "dataimportexportbase_wrapper.h"

#include "helpers.h"
#include "exceptions.h"

#include <vector>
#include <map>
#include <windows.h>
#include "channelinfo.h"
#include "errorcodes.h"

namespace RohdeSchwarz
{
  namespace Mosaik
  {
    namespace DataImportExport
    {
      using namespace std;

      DataImportExportBase::~DataImportExportBase()
      {
        this->!DataImportExportBase();
      }

      DataImportExportBase::!DataImportExportBase()
      {
        delete this->nativeImpl_;
      }

      void DataImportExportBase::Timestamp::set(Nullable<DateTime>^ time)
      {
        if (time == nullptr)
        {
          this->nativeImpl_->setTimestamp(0);
          return;
        }

        DateTime startTime = DateTime(1970, 1, 1, 0, 0, 0, 0);
        TimeSpan span = (DateTime)*time - startTime;

        time_t timet = static_cast<time_t>(span.TotalSeconds);
        this->nativeImpl_->setTimestamp(timet);
      }

      Nullable<DateTime>^ DataImportExportBase::Timestamp::get()
      {
        time_t timet = this->nativeImpl_->getTimestamp();
        if (timet == 0)
        {
          return nullptr;
        }

        long long filetime = Int32x32To64(timet, 10000000) + 116444736000000000;
        return Nullable<DateTime>(DateTime::FromFileTimeUtc(filetime));
      }

      IList<ChannelInfo^>^ DataImportExportBase::ChannelInformation::get()
      {
        vector<rohdeschwarz::mosaik::dataimportexport::ChannelInfo> nativeChannelInfos;
        map<string, string> nativeMetadata;

        int ret = this->nativeImpl_->getMetadata(nativeChannelInfos, nativeMetadata);
        if (ret != rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success)
        {
          string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }

        IList<ChannelInfo^>^ channelInfos = gcnew List<ChannelInfo^>(static_cast<int>(nativeChannelInfos.size()));
        for (rohdeschwarz::mosaik::dataimportexport::ChannelInfo nativeInfo : nativeChannelInfos)
        {
          channelInfos->Add(gcnew ChannelInfo(Helpers::marshalUTF8String(nativeInfo.getChannelName()), nativeInfo.getClockRate(), nativeInfo.getFrequency()));
        }

        return channelInfos;
      }

      IDictionary<String^, String^>^ DataImportExportBase::Metadata::get()
      {
        vector<rohdeschwarz::mosaik::dataimportexport::ChannelInfo> nativeChannelInfos;
        map<string, string> nativeMetadata;

        int ret = this->nativeImpl_->getMetadata(nativeChannelInfos, nativeMetadata);
        if (ret != rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success)
        {
          string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }

        IDictionary<String^, String^>^ metadata = gcnew Dictionary<String^, String^>(static_cast<int>(nativeMetadata.size()));
        for (pair<string, string> kvp : nativeMetadata)
        {
          metadata->Add(Helpers::marshalUTF8String(kvp.first), Helpers::marshalUTF8String(kvp.second));
        }

        return metadata;
      }

      IList<System::String^>^ DataImportExportBase::ReadOpen()
      {
        vector<string> nativeNames;
        int ret = this->nativeImpl_->readOpen(nativeNames);
        if (ret != rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success)
        {
          string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }

        IList<String^>^ arrayNames = gcnew List<String^>(static_cast<int>(nativeNames.size()));
        for (size_t i = 0; i < nativeNames.size(); ++i)
        {
          arrayNames->Add(Helpers::marshalUTF8String(nativeNames[i]));
        }

        return arrayNames;
      }

      void DataImportExportBase::WriteOpen(
        IqDataFormat format,
        int nofArrays,
        String^ applicationName,
        String^ comment,
        IList<ChannelInfo^>^ channelInfos)
      {
        this->WriteOpen(format, nofArrays, applicationName, comment, channelInfos, nullptr);
      }

      void DataImportExportBase::WriteOpen(
        IqDataFormat format,
        int nofArrays,
        String^ applicationName,
        String^ comment,
        IList<ChannelInfo^>^ channelInfos,
        IDictionary<String^, String^>^ metadata)
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

        vector<rohdeschwarz::mosaik::dataimportexport::ChannelInfo> nativeChannels;
        nativeChannels.reserve(channelInfos->Count);
        for each (ChannelInfo^ channelInfo in channelInfos)
        {
          nativeChannels.push_back(rohdeschwarz::mosaik::dataimportexport::ChannelInfo(
            Helpers::marshalToUTF8String(channelInfo->ChannelName), 
            channelInfo->ClockRate,
            channelInfo->Frequency));
        }

        int ret = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success;
        if (metadata == nullptr)
        {
          ret = this->nativeImpl_->writeOpen(
            nativeFormat,
            static_cast<size_t>(nofArrays),
            Helpers::marshalToUTF8String(applicationName),
            Helpers::marshalToUTF8String(comment),
            nativeChannels);
        }
        else
        {
          map<string, string> nativeMetadata;
          for each (KeyValuePair<String^, String^> kvp in metadata)
          {
            nativeMetadata.insert(make_pair(Helpers::marshalToUTF8String(kvp.Key), Helpers::marshalToUTF8String(kvp.Value)));
          }

          ret = this->nativeImpl_->writeOpen(
            nativeFormat,
            static_cast<size_t>(nofArrays),
            Helpers::marshalToUTF8String(applicationName),
            Helpers::marshalToUTF8String(comment),
            nativeChannels,
            &nativeMetadata);
        }

        if (ret != rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success)
        {
          string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }
      }

      void DataImportExportBase::Close()
      {
        int ret = this->nativeImpl_->close();
        if (ret != rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success)
        {
          string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }
      }

      Int64 DataImportExportBase::GetArraySize(String^ arrayname)
      {
        return this->nativeImpl_->getArraySize(Helpers::marshalToUTF8String(arrayname));
      }

      void DataImportExportBase::ReadArray(String^ arrayName, IList<float>^% values, int nofValues)
      {
        this->ReadArray(arrayName, values, nofValues, 0);
      }

      void DataImportExportBase::ReadArray(String^ arrayName, IList<float>^% values, int nofValues, int offset)
      {
        vector<float> data(nofValues);
        int ret = this->nativeImpl_->readArray(Helpers::marshalToUTF8String(arrayName), data, nofValues, offset);
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
          values->Add(data[i]);
        }
      }

      void DataImportExportBase::ReadArray(String^ arrayName, IList<double>^% values, int nofValues)
      {
        this->ReadArray(arrayName, values, nofValues, 0);
      }

      void DataImportExportBase::ReadArray(String^ arrayName, IList<double>^% values, int nofValues, int offset)
      {
        vector<double> data(nofValues);
        int ret = this->nativeImpl_->readArray(Helpers::marshalToUTF8String(arrayName), data, nofValues, offset);
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
          values->Add(data[i]);
        }
      }

      void DataImportExportBase::ReadChannel(String^ channelName, IList<float>^% values, int nofValues)
      {
        this->ReadChannel(channelName, values, nofValues, 0);
      }

      void DataImportExportBase::ReadChannel(String^ channelName, IList<float>^% values, int nofValues, int offset)
      {
        vector<float> data(nofValues);
        int ret = this->nativeImpl_->readChannel(Helpers::marshalToUTF8String(channelName), data, nofValues, offset);
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
          values->Add(data[i]);
        }
      }

      void DataImportExportBase::ReadChannel(String^ channelName, IList<double>^% values, int nofValues)
      {
        this->ReadChannel(channelName, values, nofValues, 0);
      }

      void DataImportExportBase::ReadChannel(String^ channelName, IList<double>^% values, int nofValues, int offset)
      {
        vector<double> data(nofValues);
        int ret = this->nativeImpl_->readChannel(Helpers::marshalToUTF8String(channelName), data, nofValues, offset);
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
          values->Add(data[i]);
        }
      }

      void DataImportExportBase::AppendArrays(IList<IList<float>^>^ iqdata)
      {
        vector<vector<float>> data(iqdata->Count);
        for (int i = 0; i < iqdata->Count; ++i)
        {
          IList<float>^ col = iqdata[i];
          data[i].resize(col->Count);
          for (int d = 0; d < col->Count; ++d)
          {
            data[i][d] = col[d];
          }
        }

        int ret = this->nativeImpl_->appendArrays(data);
        if (ret != rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success)
        {
          string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }
      }

      void DataImportExportBase::AppendArrays(IList<IList<double>^>^ iqdata)
      {
        vector<vector<double>> data(iqdata->Count);
        for (int i = 0; i < iqdata->Count; ++i)
        {
          IList<double>^ col = iqdata[i];
          data[i].resize(col->Count);
          for (int d = 0; d < col->Count; ++d)
          {
            data[i][d] = col[d];
          }
        }

        int ret = this->nativeImpl_->appendArrays(data);
        if (ret != rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success)
        {
          string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }
      }

      void DataImportExportBase::AppendChannels(IList<IList<float>^>^ iqdata)
      {
        vector<vector<float>> data(iqdata->Count);
        for (int i = 0; i < iqdata->Count; ++i)
        {
          IList<float>^ col = iqdata[i];
          data[i].resize(col->Count);
          for (int d = 0; d < col->Count; ++d)
          {
            data[i][d] = col[d];
          }
        }

        int ret = this->nativeImpl_->appendChannels(data);
        if (ret != rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success)
        {
          string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }
      }

      void DataImportExportBase::AppendChannels(IList<IList<double>^>^ iqdata)
      {
        vector<vector<double>> data(iqdata->Count);
        for (int i = 0; i < iqdata->Count; ++i)
        {
          IList<double>^ col = iqdata[i];
          data[i].resize(col->Count);
          for (int d = 0; d < iqdata[i]->Count; ++d)
          {
            data[i][d] = col[d];
          }
        }

        int ret = this->nativeImpl_->appendChannels(data);
        if (ret != rohdeschwarz::mosaik::dataimportexport::ErrorCodes::Success)
        {
          string text = rohdeschwarz::mosaik::dataimportexport::ErrorCodes::getErrorText(ret);
          throw gcnew DaiException(ret, Helpers::marshalUTF8String(text));
        }
      }
    }
  }
}