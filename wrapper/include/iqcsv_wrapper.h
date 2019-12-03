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
#include "icsvselector_wrapper.h"

#include "iqcsv.h"

namespace RohdeSchwarz
{
  namespace Mosaik
  {
    namespace DataImportExport
    {
      using namespace System;

      /// <summary>
      /// Wraps up the native IqCsv object defined in iqcsv.h.
      /// </summary>
      public ref class IqCsv : public DataImportExportBase, ICsvSelector
      {
      public:
        /// <summary>
        /// Initializes a new instance of the <see cref="IqCsv"/> class.
        /// Be aware that the filename cannot be changed after construction.
        /// </summary>
        /// <param name="filename">Name of the file to be read or to be written.</param>
        IqCsv(String^ filename);

        /// <summary>
        /// Gets or sets a valid combination of characters to be used as decimal separator 
        /// and value separator. Must be set before file is opened for write operations (writeOpen()).
        /// </summary>
        property CsvSeparatorConfig SeparatorConfiguration 
        {
          void set(CsvSeparatorConfig config);
          CsvSeparatorConfig get();
        }

        /// <summary>
        /// Gets the character to be used as separator for the values written in one line of the UTF8-encoded CSV file.
        /// </summary>
        property String^ ValueSeparator
        {
          String^ get();
        }

        /// <summary>
        /// Gets the character to be used as a decimal separator when converting floating-point numbers to string. 
        /// </summary>
        property String^ DecimalSeparator
        {
          String^ get();
        }

        /// <summary>
        /// Gets or sets the format specified used to convert numeric values to string. 
        /// Be aware that the used format specified follows c++ style (i.e. "7E"), not .net style ("E7").
        /// The format specifier is used to convert I/Q values to the specified format while saving data to file. 
        /// The default value is "7E". Must be set before file is opened for writing. Otherwise an error will be returned.
        ///	Accepted specifiers are:
        ///
        /// - Exponential (scientific) notation [ "E" / "e" ].
        ///	Converts a number to a string of the form "-d.ddd...E+ddd" or "-d.ddd...e+ddd",
        /// where each "d" indicates a digit (0-9). The string starts with a minus sign if the number is negative.
        /// Exactly one digit always precedes the decimal point.
        /// The precision specifier indicates the desired number of digits after the decimal point.
        /// The case of the format specifier indicates whether to prefix the exponent with an "E" or an "e".
        /// - The general format specifier [ "G" ].
        /// Converts a number to the most compact of either fixed-point or scientific notation,
        /// depending on the type of the number and whether a precision specifier is present.
        /// The precision specifier defines the maximum number of significant digits that can appear
        /// in the result string. 
        /// - Fixed point notation [ "F" ].
        /// Converts a number to a string of the form "-ddd.ddd..." where each "d" indicates a digit (0-9).
        /// The string starts with a minus sign if the number is negative.
        /// The precision specifier indicates the desired number of decimal places.
        /// 
        ///	For further information see http://www.cplusplus.com/reference/cstdio/scanf/
        /// </summary>
        property String^ FormatSpecifier
        {
          void set(String^ format);
          String^ get();
        }

        /// <summary>
        /// Gets or sets the format specified used to convert numeric values contained by ChannelInfo 
        /// objects to string, i.e. clock rate and center frequency. The default value is "7E".
        /// </summary>
        property String^ FormatSpecifierChannelInfo
        {
          void set(String^ format);
          String^ get();
        }

        /// <inheritdoc />
        virtual Int64 GetNofCols();

        /// <inheritdoc />
        virtual Int64 GetNofRows(int column);

        /// <inheritdoc />
        virtual void ReadRawArray(int column, int nofValues, IList<float>^% values);

        /// <inheritdoc />
        virtual void ReadRawArray(int column, int nofValues, IList<float>^% values, int offset);

        /// <inheritdoc />
        virtual void ReadRawArray(int column, int nofValues, IList<double>^% values);

        /// <inheritdoc />
        virtual void ReadRawArray(int column, int nofValues, IList<double>^% values, int offset);
      };
    }
  }
}