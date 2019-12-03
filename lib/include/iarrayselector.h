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

/*!
* @file      iarrayselector.h
*
* @brief     This is the header file of interface IArraySelector.
*
* @details   The interface specifies methods to query matlab arrays of type float/double by name.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "exportdecl.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief The interface specifies methods to query matlab arrays of type float/double by name.
      * The interface can be used to search float/double arrays in a matlab file by name and accessing the data
      * using index-based access. Use the methods if the current file does not match the R/S matlab
      * file format specification, e.g. the metadata definition is incomplete. If a file does not match
      * the R/S file format specification, an error code will be returned when readOpen() is called.
      * The typical procedure is to first search the file for valid data fields using matchArrayDimensions(), 
      * subsequently read the dimensions of the data field using getNofRows() and getNofCols(), and finally
      * reading the data using readRawArray(). The conversion between float and double will be 
      * performed internally, if the data type of the provided values-array does not match the data type
      * used in the matlab file.
      * 
      * Example:
      * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      * IqMatlab readFile(filename);
      *
      * // find all matlab variables of type single or double that have exactly 2 columns 
      * // and at least 100 rows.
      * vector<string> arrayNames;
      * readFile.matchDimensions(2, 100, true, arrayNames);
      *
      * // get number of rows of first column of first variable found
      * auto nofRows = readFile.getNofRows(arrayNames[0]);
      *
      * // read first column of first variable found
      * vector<float> values;
      * readFile.readRawData(arrayNames[0], 0, nofRows, values);
      * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      */
      class MOSAIK_MODULE IArraySelector
      {
      public:
        /**
          @brief Searches a matlab file for all arrays of type float or double that match the specified 
          conditions w.r.t. the	minimum number of columns and rows. The names of all matching arrays are 
          returned with parameter arrayNames.
          @param [in]  minCols The minimum number of columns that the matlab array must contain.
          @param [in]  minRows The minimum number of rows that the matlab array must contain.
          @param [in]  exactColMatch If TRUE, the specified number of columns (minCols) must match exactly.
          @param [out]  arrayNames The resulting array names of the query.
          @returns Returns ErrorCode::Success (=0) if no error occurred. For further error codes, see \ref ErrorCodes.
        */virtual int matchArrayDimensions(size_t minCols, size_t minRows, bool exactColMatch, std::vector<std::string>& arrayNames) = 0;

        /**
          @param [in]  arrayName Name of the matlab array to read.
          @returns Returns the number of rows of the matlab array corresponding to the specified name. In case of any
          error, -1 is returned.
        */virtual int64_t getNofRows(const std::string& arrayName) = 0;

        /**
          @param [in]  arrayName Name of the matlab array to read.
          @returns Returns the number of columns of the matlab array corresponding to the specified name. In case of any
          error, -1 is returned.
        */virtual int64_t getNofCols(const std::string& arrayName) = 0;

        /**
          @brief Reads values row-wise from the specified matlab array. The matlab array type must be float or double.
          @param [in]  arrayName The name of the array to read.
          @param [in]  column The column to read.
          @param [in]  nofValues The number of values (that is, the number of rows) to read.
          @param [out]  values The values read. Data read from file will be converted to single precision,
          independent of the original data type.
          @param [in]  offset Defines the start position (that is the row) within the specified column at which the 
          read operation is started.
          @returns If data was read successfully ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int readRawArray(const std::string& arrayName, size_t column, size_t nofValues, std::vector<float>& values, size_t offset = 0) = 0;

        /**
          @brief Reads values row-wise from the specified matlab array. The matlab array type must be float or double.
          @param [in]  arrayName The name of the array to read.
          @param [in]  column The column to read.
          @param [in]  nofValues The number of values (that is, the number of rows) to read.
          @param [out]  values The values read. Data read from file will be converted to single precision,
          independent of the original data type.
          @param [in]  offset Defines the start position (that is the row) within the specified column at which the 
          read operation is started.
          @returns If data was read successfully ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int readRawArray(const std::string& arrayName, size_t column, size_t nofValues, float* values, size_t offset = 0) = 0;

        /**
          @brief Reads values row-wise from the specified matlab array. The matlab array type must be float or double.
          @param [in]  arrayName The name of the array to read.
          @param [in]  column The column to read.
          @param [in]  nofValues The number of values (that is, the number of rows) to read.
          @param [out]  values The values read. Data read from file will be converted to double precision,
          independent of the original data type.
          @param [in]  offset Defines the start position (that is the row) within the specified column at which the 
          read operation is started.
          @returns If data was read successfully ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int readRawArray(const std::string& arrayName, size_t column, size_t nofValues, std::vector<double>& values, size_t offset = 0) = 0;

        /**
          @brief Reads values row-wise from the specified matlab array. The matlab array type must be float or double.
          @param [in]  arrayName The name of the array to read.
          @param [in]  column The column to read.
          @param [in]  nofValues The number of values (that is, the number of rows) to read.
          @param [out]  values The values read. Data read from file will be converted to double precision,
          independent of the original data type.
          @param [in]  offset Defines the start position (that is the row) within the specified column at which the 
          read operation is started.
          @returns If data was read successfully ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int readRawArray(const std::string& arrayName, size_t column, size_t nofValues, double* values, size_t offset = 0) = 0;
      };
    }
  }
}