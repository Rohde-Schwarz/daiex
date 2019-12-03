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
			using namespace System::Collections::Generic;

			/// <summary>
			/// Wraps up the native interface ICsvSelector.
			/// </summary>
			public interface class ICsvSelector
			{
			public:
				
				/// <summary>
				/// Returns the number of rows or -1 if an error occurred.
				/// Rows will be counted until the end of the file, an empty rows is
				/// found, or a row does not contain numeric data.
				/// </summary>
				/// <param name="column">The CSV column index, starting from 0.</param>
				/// <returns>Returns the number of rows or -1.</returns>
				Int64 GetNofRows(int column);

				/// <summary>
				/// Returns the number of columns found in the CSV file or -1 if an 
				/// error occurred.
				/// </summary>
				/// <returns>Returns the number of columns or -1.</returns>
				Int64 GetNofCols();

				/// <summary>
				/// Reads values row-wise from the specified column. 
				/// </summary>
				/// <param name="column">The column index to read.</param>
				/// <param name="nofValues">The number of values (that is, the number of rows) to read.</param>
				/// <param name="values">The values read. Data read from file will be converted to single precision,
				/// independent of the original data type.</param>
				/// <exception cref="DaiException">See the error code of the native implementation for more information.</exception>
				void ReadRawArray(int column, int nofValues, IList<float>^% values);

				/// <summary>
				/// Reads values row-wise from the specified column. 
				/// </summary>
				/// <param name="column">The column index to read.</param>
				/// <param name="nofValues">The number of values (that is, the number of rows) to read.</param>
				/// <param name="values">The values read. Data read from file will be converted to single precision,
				/// independent of the original data type.</param>
				/// <param name="offset">Defines the start position (aka row) within the specified column at which the 
				/// read operation is started.</param>
				/// <exception cref="DaiException">See the error code of the native implementation for more information.</exception>
				void ReadRawArray(int column, int nofValues, IList<float>^% values, int offset);

				/// <summary>
				/// Reads values row-wise from the specified column. 
				/// </summary>
				/// <param name="column">The column index to read.</param>
				/// <param name="nofValues">The number of values (that is, the number of rows) to read.</param>
				/// <param name="values">The values read. Data read from file will be converted to double precision,
				/// independent of the original data type.</param>
				/// <exception cref="DaiException">See the error code of the native implementation for more information.</exception>
				void ReadRawArray(int column, int nofValues, IList<double>^% values);

				/// <summary>
				/// Reads values row-wise from the specified column. 
				/// </summary>
				/// <param name="column">The column index to read.</param>
				/// <param name="nofValues">The number of values (that is, the number of rows) to read.</param>
				/// <param name="values">The values read. Data read from file will be converted to double precision,
				/// independent of the original data type.</param>
				/// <param name="offset">Defines the start position (aka row) within the specified column at which the 
				/// read operation is started.</param>
				/// <exception cref="DaiException">See the error code of the native implementation for more information.</exception>
				void ReadRawArray(int column, int nofValues, IList<double>^% values, int offset);
			};
		}
	}
}