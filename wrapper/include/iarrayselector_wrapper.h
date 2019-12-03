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
			/// Wraps up the native interface IArraySelector.
			/// </summary>
			public interface class IArraySelector
			{
			public:
				/// <summary>
				/// Searches a matlab file for all arrays of type float or double that match the specified 
				/// conditions w.r.t the	minimum number of columns and rows. The names of all matching arrays are 
				/// returned with parameter arrayNames.
				/// </summary>
				/// <param name="minCols">The minimum number of columns that the matlab array must contain.</param>
				/// <param name="minRows">The minimum number of rows that the matlab array must contain.</param>
				/// <param name="exactColMatch">If TRUE, the specified number of columns (minCols) must match exactly.</param>
				/// <param name="arrayNames">The resulting array names of the query.</param>
				/// <exception cref="DaiException">See the error code of the native implementation for more information.</exception>
				void MatchArrayDimensions(int minCols, int minRows, bool exactColMatch, IList<String^>^ arrayNames);

				/// <summary>
				/// Returns the number of rows of the matlab array corresponding to the specified name. In case of any
				/// error, -1 is returned.
				/// </summary>
				/// <param name="arrayName">Name of the matlab array to read.</param>
				/// <returns>Returns the number of rows or -1.</returns>
				Int64 GetNofCols(String^ arrayName);

				/// <summary>
				/// Returns the number of columns of the matlab array corresponding to the specified name. In case of any
				/// error, -1 is returned.
				/// </summary>
				/// <param name="arrayName">Name of the matlab array to read.</param>
				/// <returns>Returns the number of columns or -1.</returns>
				Int64 GetNofRows(String^ arrayName);

				/// <summary>
				/// Reads values row-wise from the specified matlab array. The matlab array type must be float or double.
				/// </summary>
				/// <param name="arrayName">Name of the array to read.</param>
				/// <param name="column">The column to read.</param>
				/// <param name="nofValues">The number of values (that is, the number of rows) to read.</param>
				/// <param name="values">values The values read. Data read from file will be converted to single precision,
				/// independent of the original data type.</param>
				/// <exception cref="DaiException">See the error code of the native implementation for more information.</exception>
				void ReadRawArray(String^ arrayName, int column, int nofValues, IList<float>^% values);

				/// <summary>
				/// Reads values row-wise from the specified matlab array. The matlab array type must be float or double.
				/// </summary>
				/// <param name="arrayName">Name of the array to read.</param>
				/// <param name="column">The column to read.</param>
				/// <param name="nofValues">The number of values (that is, the number of rows) to read.</param>
				/// <param name="values">values The values read. Data read from file will be converted to single precision,
				/// independent of the original data type.</param>
				/// <param name="offset">Defines the start position (aka row) within the specified column at which the 
				/// read operation is started.</param>
				/// <exception cref="DaiException">See the error code of the native implementation for more information.</exception>
				void ReadRawArray(String^ arrayName, int column, int nofValues, IList<float>^% values, int offset);

				/// <summary>
				/// Reads values row-wise from the specified matlab array. The matlab array type must be float or double.
				/// </summary>
				/// <param name="arrayName">Name of the array to read.</param>
				/// <param name="column">The column to read.</param>
				/// <param name="nofValues">The number of values (that is, the number of rows) to read.</param>
				/// <param name="values">values The values read. Data read from file will be converted to double precision,
				/// independent of the original data type.</param>
				/// <exception cref="DaiException">See the error code of the native implementation for more information.</exception>
				void ReadRawArray(String^ arrayName, int column, int nofValues, IList<double>^% values);

				/// <summary>
				/// Reads values row-wise from the specified matlab array. The matlab array type must be float or double.
				/// </summary>
				/// <param name="arrayName">Name of the array to read.</param>
				/// <param name="column">The column to read.</param>
				/// <param name="nofValues">The number of values (that is, the number of rows) to read.</param>
				/// <param name="values">values The values read. Data read from file will be converted to double precision,
				/// independent of the original data type.</param>
				/// <param name="offset">Defines the start position (aka row) within the specified column at which the 
				/// read operation is started.</param>
				/// <exception cref="DaiException">See the error code of the native implementation for more information.</exception>
				void ReadRawArray(String^ arrayName, int column, int nofValues, IList<double>^% values, int offset);
			};
		}
	}
}