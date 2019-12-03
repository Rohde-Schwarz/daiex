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

#pragma once

#include "idataimportexport.h"
#include "idataimportexport_wrapper.h"

namespace RohdeSchwarz
{
  namespace Mosaik
  {
    namespace DataImportExport
    {
      using namespace System;

      /// <summary>
      /// Implements the interface IDataImportExport and provides common functionality of
      /// all I/Q data formats, such as opening, closing or reading a file.
      /// </summary>
      public ref class DataImportExportBase abstract : public IDataImportExport
      {
      public:
        /// <summary>
        /// Finalizes an instance of the <see cref="DataImportExportBase"/> class
        /// and cleans up native resources.
        /// </summary>
        ~DataImportExportBase();

        /// <inheritdoc />
        virtual property Nullable<DateTime>^ Timestamp
        {
          void set(Nullable<DateTime>^ time);
          Nullable<DateTime>^ get();
        }

        /// <inheritdoc />
        virtual property IList<ChannelInfo^>^ ChannelInformation
        {
          IList<ChannelInfo^>^ get();
        }

        /// <inheritdoc />
        virtual property IDictionary<String^, String^>^ Metadata
        {
          IDictionary<String^, String^>^ get();
        }

        /// <inheritdoc />
        virtual IList<System::String^>^ ReadOpen();

        /// <inheritdoc />
        virtual void WriteOpen(
          IqDataFormat format,
          int nofArrays,
          String^ applicationName,
          String^ comment,
          IList<ChannelInfo^>^ channelInfos);

        /// <inheritdoc />
        virtual void WriteOpen(
          IqDataFormat format,
          int nofArrays,
          String^ applicationName,
          String^ comment,
          IList<ChannelInfo^>^ channelInfos,
          IDictionary<String^, String^>^ metadata);

        /// <inheritdoc />
        virtual void Close();

        /// <inheritdoc />
        virtual Int64 GetArraySize(String^ arrayname);

        /// <inheritdoc />
        virtual void ReadArray(String^ arrayName, IList<float>^% values, int nofValues);

        /// <inheritdoc />
        virtual void ReadArray(String^ arrayName, IList<float>^% values, int nofValues, int offset);
        
        /// <inheritdoc />
        virtual void ReadArray(String^ arrayName, IList<double>^% values, int nofValues);

        /// <inheritdoc />
        virtual void ReadArray(String^ arrayName, IList<double>^% values, int nofValues, int offset);

        /// <inheritdoc />
        virtual void ReadChannel(String^ channelName, IList<float>^% values, int nofValues);

        /// <inheritdoc />
        virtual void ReadChannel(String^ channelName, IList<float>^% values, int nofValues, int offset);

        /// <inheritdoc />
        virtual void ReadChannel(String^ channelName, IList<double>^% values, int nofValues);
        
        /// <inheritdoc />
        virtual void ReadChannel(String^ channelName, IList<double>^% values, int nofValues, int offset);

        /// <inheritdoc />
        virtual void AppendArrays(IList<IList<float>^>^ iqdata);

        /// <inheritdoc />
        virtual void AppendArrays(IList<IList<double>^>^ iqdata);

        /// <inheritdoc />
        virtual void AppendChannels(IList<IList<float>^>^ iqdata);

        /// <inheritdoc />
        virtual void AppendChannels(IList<IList<double>^>^ iqdata);

      protected:
        /// <summary>Finalizer, cleans up native resources.</summary>
        !DataImportExportBase();

        /// <summary>Handle to the native object that is wrapped up by this class.</summary>
        rohdeschwarz::mosaik::dataimportexport::IDataImportExport* nativeImpl_;
      };
    }
  }
}