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

#include "dataimportexportbase_wrapper.h"

#include "iqtar.h"

namespace RohdeSchwarz
{
  namespace Mosaik
  {
    namespace DataImportExport
    {
      using namespace System;

      /// <summary>
      /// Wraps up the native IqTar object defined in iqtar.h.
      /// </summary>
      public ref class IqTar : public DataImportExportBase
      {
      public:
        /// <summary>
        /// Initializes a new instance of the <see cref="IqTar"/> class.
        /// Be aware that the filename cannot be changed after construction.
        /// </summary>
        /// <param name="filename">Name of the file to be read or to be written.</param>
        IqTar(String^ filename);

        /// <summary>
        /// Gets or sets a value indicating whether or not an I/Q preview will be calculated
        /// and saved in the iq.tar file. If disabled, no I/Q preview data will be added to the
        /// metadata xml nor the xml scheme 'open_IqTar_xml_file_in_web_browser.xslt' will be
        /// added. Has to be set before writeOpen is called! The default value is TRUE.
        /// </summary>
        /// <exception cref="DaiException">Thrown if an error occurred. See the error code of the native 
        /// implementation for more information.</exception>
        property bool CalculateIqPreview
        {
          void set(bool enable);
          bool get();
        }

        /// <summary>
        /// Gets deprecated information as saved in xml-file of iq.tar at hierarchy level &lt;UserData&gt;&lt;RohdeSchwarz&gt;DEPRECATED_INFO_STRING, as
        /// required by FSW. If no deprecated information was found in the xml-file, an empty string is returned.
        /// </summary>
        property String^ DeprecatedInfo
        {
          String^ get();
        }

        /// <summary>
        /// Gets or sets the directory used to write temporary files to.
        /// If not file-writer has been initialized an empty string is
        /// returned.
        /// </summary>
        property String^ TempDir
        {
          void set(String^ path);
          String^ get();
        }

        /// <summary>
        /// Opens a file in write-only mode. If supported by the file format, the specified meta data will also be stored in the file.
        /// Call close() to finalize the file.
        /// </summary>
        /// <param name="format">Specifies how I/Q values are saved, i.e. complex, real, or polar.</param>
        /// <param name="nofArrays">The number of data arrays to be written. E.g. one complex I/Q channel contains 2 arrays.</param>
        /// <param name="applicationName">Application or instrument name exporting this I/Q data.</param>
        /// <param name="comment">Text that further describes the file contents.</param>
        /// <param name="channelInfos">Channel information with name, clock rate and center frequency. One object is required per channel.</param>
        /// <param name="metadata">Additional, non-standardized meta data; provided as key-value pairs.</param>
        /// <param name="deprecatedInfo">Information that can be added to the XML file at hierarchy level &lt;UserData&gt;&lt;RohdeSchwarz&gt;DEPRECATED_INFO_STRING,
        /// as required by FSW. Make sure to pass a valid XML string. </param>
        /// <exception cref="DaiException">Thrown if an error occurred. See the error code of the native 
        /// implementation for more information.</exception>
        void WriteOpen(
          IqDataFormat format,
          int nofArrays,
          String^ applicationName,
          String^ comment,
          IList<ChannelInfo^>^ channelInfos,
          IDictionary<String^, String^>^ metadata,
          String^ deprecatedInfo);

        /// <summary>
        /// If the number of i/q values to be written is well-known in advance, buffering data
        /// to temporary files can be disabled. Internally, the expected file size will be calculated. If that
        /// this is exceeded, an exception will be raised.
        /// </summary>
        /// <param name="nofIqValues">Number of i/q values per channel. Iq.tar file format requires all channels to have the same length.</param>
        /// <param name="nofChannels">Number of channels to be written.</param>
        /// <param name="format">I/q file format, i.e. complex or real.</param>
        /// <param name="dataType">Type of the i/q values, i.e. float32 or float64.</param>
        void DisableTempFile(uint64_t nofIqValues, size_t nofChannels, IqDataFormat format, IqDataType dataType);
      };
    }
  }
}