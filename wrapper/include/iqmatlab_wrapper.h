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
#include "iarrayselector_wrapper.h"

#include "iqmatlab.h"

namespace RohdeSchwarz
{
  namespace Mosaik
  {
    namespace DataImportExport
    {
      using namespace System;

      /// <summary>
      /// Wraps up the native IqMatlab object defined in iqmatlab.h
      /// </summary>
      public ref class IqMatlab : public DataImportExportBase, IArraySelector
      {
      public:
        /// <summary>
        /// Initializes a new instance of the <see cref="IqMatlab"/> class.
        /// Be aware that the filename cannot be changed after construction.
        /// </summary>
        /// <param name="filename">Name of the file to be read or to be written.</param>
        IqMatlab(String^ filename);

        /// <summary>
        /// Gets or sets the matlab file version used to write a new matlab file.
        /// Matlab file versions v4 and v7.3 are supported. It is not necessary to set
        /// the file version when reading a file. The version is automatically detected
        /// when the file is opened.
        /// Cf. http://de.mathworks.com/help/matlab/import_export/mat-file-versions.html and 
        /// https://de.mathworks.com/matlabcentral/answers/91711-what-is-the-maximum-matrix-size-for-each-platform
        /// for format-specific limitations.
        /// </summary>
        property MatlabVersion MatlabFileVersion
        {
          void set(MatlabVersion version);
          MatlabVersion get();
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

        /// <inheritdoc />
        virtual void MatchArrayDimensions(int minCols, int minRows, bool exactColMatch, IList<String ^>^ arrayNames);

        /// <inheritdoc />
        virtual Int64 GetNofCols(String^ arrayName);

        /// <inheritdoc />
        virtual Int64 GetNofRows(String^ arrayName);

        /// <inheritdoc />
        virtual void ReadRawArray(String^ arrayName, int column, int nofValues, IList<float>^% values);

        /// <inheritdoc />
        virtual void ReadRawArray(String^ arrayName, int column, int nofValues, IList<float>^% values, int offset);

        /// <inheritdoc />
        virtual void ReadRawArray(String^ arrayName, int column, int nofValues, IList<double>^% values);

        /// <inheritdoc />
        virtual void ReadRawArray(String^ arrayName, int column, int nofValues, IList<double>^% values, int offset);

      };
    }
  }
}