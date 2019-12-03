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

namespace RohdeSchwarz
{
  namespace Mosaik
  {
    namespace DataImportExport
    {
      using namespace System;

      /// <summary>
      /// Specifies the binary format used for samples in I/Q data binary files.
      /// </summary>
      public enum class IqDataType : int
      {
        /// <summary>32 bit floating point data (IEEE 754).</summaray>
        Float32,

        /// <summary>64 bit floating point data (IEEE 754).</summaray>
        Float64
      };


      /// <summary>
      /// Specifies the data format used for samples in I/Q data binary files.
      /// </summary>
      public enum class IqDataFormat : int
      {
        /// <summary>Complex number in Cartesian format. I and Q are unit-less.</summaray>
        Complex,

        /// <summary>Real number (unit-less).</summaray>
        Real,

        /// <summary>Complex number in polar format, magnitude (unit-less) and phase (rad) values.
        /// Requires IQDataType float32 or float64.</summaray>
        Polar
      };

      /// <summary>
      /// Specifies the order in which the I/Q data appears in the binary file format IQW.
      /// </summary>
      public enum class IqDataOrder : int
      {
        /// <summary>The file contains all I samples in 1 block, followed by all Q samples.</summaray>
        IIIQQQ,

        /// <summary>1 I and 1 Q sample alternate within the file (starting with I).</summaray>
        IQIQIQ
      };

      /// <summary>
      /// Specifies the file formats supported by RohdeSchwarz.Mosaik.LibDataImportExport.
      /// </summary>
      public enum class FileType : int
      {
        /// <summary>Rohde &amp; Schwarz iq.tar format.</summaray>
        Iqtar,

        /// <summary>A binary file format used by Rohde &amp; Schwarz containing 1 channel of complex
        /// float32 binary data in data order IIIQQQ or IQIQIQ.</summaray>
        IQW,

        /// <summary>Matlab .MAT file version v4.</summaray>
        Matlab4,

        /// <summary>Matlab .MAT file version v7.3.</summaray>
        Matlab73,

        /// <summary>Comma separated values. ";" or "," can be used as separators.</summaray>
        Csv
      };

      /// <summary>
      ///  MAT file versions supported by LibDaiEx. 
      /// Several size limitations apply for the different matlab file formats with respect to
      /// the maximum number of elements per array, the maximum size of an array in bytes and the 
      /// total file size. For further information see
      /// http://de.mathworks.com/help/matlab/import_export/mat-file-versions.html and 
      /// https://de.mathworks.com/matlabcentral/answers/91711-what-is-the-maximum-matrix-size-for-each-platform
      /// </summary>
      public enum class MatlabVersion : int
      {
        /// <summary>Matlab version 4 file.</summaray>
        Mat4,

        /// <summary>Matlab version 7.3 file.</summaray>
        Mat73
      };

      /// <summary>
      /// An enumeration that defines allowed combinations of the value separator for the CSV format
      /// and the decimal separator used to save floating-point values within the CSV format.
      /// As those two separators need to be different, this enumeration is used to define valid combinations.
      /// The group separator needed when formatting floating-point values is always set to an empty string.
      /// </summary>
      public enum class CsvSeparatorConfig : int
      {
        /// <summary>A "," is used as decimal separator, while a ";" separates the individual values.
        /// Example: 0,123;23,45.</summaray>
        DecimalColonValueSemicolon,

        /// <summary>	A "." is used as decimal separator, while a ";" separates the individual values.
        /// Example: 0.123;23.45.</summaray>
        DecimalDotValueSemicolon,

        /// <summary>	A "." is used as decimal separator, while a "," separates the individual values.
        /// Example: 0.123,23.45.</summaray>
        DecimalDotValueColon
      };
    }
  }
}