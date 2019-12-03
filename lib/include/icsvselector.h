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
* @file      icsvselector.h
*
* @brief     This is the header file of interface ICsvSelector.
*
* @details   The interface specifies methods to query CSV files that contain 
*						 merely data arrays and do not match the R/S I/Q-CSV file format.
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
      * @brief The interface specifies methods to query CSV files that contain 
      *	merely data arrays. The interface can be used to access I/Q data, if the CSV 
      * file does not match the R/S CSV file format specification, e.g. no header information or meta data
      * is available. In this case, the typical procedure is to first query the number of columns 
      * that contain numeric data using getNofCols() and the number of rows that contain numeric data
      * (getNofRows). Rows will be counted until the end of the file, an empty rows is
      * found, or a row does not contain numeric data. Finally, numeric data can be read using readRawArray().
      * The conversion between float and double will be performed internally, 
      * if the data type of the provided values-array does not match the data type
      * used in the CSV file.
      * 
      * Example:
      * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      * IqCsv readFile(filename);
      * 
      * // get number of columns in CSV file
      * auto cols = readFile.getNofCols();
      *
      * // define column index to read from
      * auto columnToRead = cols - 1;
      *
      * // get number of rows in this column
      * auto rows = readFile.getNofRows(columnToRead);
      *
      * // read all data of the selected column
      * vector<float> values;
      * auto returnCode = readFile.readRawArray(columnToRead, rows, values);
      * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      */
      class MOSAIK_MODULE ICsvSelector
      {
      public:
        /**
          @param [in]  column The CSV column index, starting from 0.
          @returns Returns the number of rows or -1 if an error occurred.
          Rows will be counted until the end of the file, an empty rows is
          found, or a row does not contain numeric data.
        */virtual int64_t getNofRows(size_t column) = 0;

        /**
          @returns Returns the number of columns found in the CSV file or -1 if an 
          error occurred.
        */virtual int64_t getNofCols() = 0;

        /**
          @brief Reads values row-wise from the specified column. 
          @param [in]  column The column index to read.
          @param [in]  nofValues The number of values (that is, the number of rows) to read.
          @param [out]  values The values read. Data read from file will be converted to single precision,
          independent of the original data type.
          @param [in]  offset Defines the start position (that is the row) within the specified column at which the 
          read operation is started.
         @returns If data was read successfully ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int readRawArray(size_t column, size_t nofValues, std::vector<float>& values, size_t offset = 0) = 0;

        /**
          @brief Reads values row-wise from the specified column. 
          @param [in]  column The column index to read.
          @param [in]  nofValues The number of values (that is, the number of rows) to read.
          @param [out]  values The values read. Data read from file will be converted to single precision,
          independent of the original data type.
          @param [in]  offset Defines the start position (that is the row) within the specified column at which the 
          read operation is started.
         @returns If data was read successfully ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int readRawArray(size_t column, size_t nofValues, float* values, size_t offset = 0) = 0;

        /**
          @brief Reads values row-wise from the specified column. 
          @param [in]  column The column index to read.
          @param [in]  nofValues The number of values (that is, the number of rows) to read.
          @param [out]  values The values read. Data read from file will be converted to double precision,
          independent of the original data type.
          @param [in]  offset Defines the start position (that is the row) within the specified column at which the 
          read operation is started.
         @returns If data was read successfully ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int readRawArray(size_t column, size_t nofValues, std::vector<double>& values, size_t offset = 0) = 0;

        /**
          @brief Reads values row-wise from the specified column. 
          @param [in]  column The column index to read.
          @param [in]  nofValues The number of values (that is, the number of rows) to read.
          @param [out]  values The values read. Data read from file will be converted to double precision,
          independent of the original data type.
          @param [in]  offset Defines the start position (that is the row) within the specified column at which the 
          read operation is started.
         @returns If data was read successfully ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */virtual int readRawArray(size_t column, size_t nofValues, double* values, size_t offset = 0) = 0;
      };
    }
  }
}