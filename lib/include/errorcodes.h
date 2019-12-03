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
* @file      errorcodes.h
*
* @brief     This is the header file of class ErrorCodes.
*
* @details   This class defines all possible errors codes of interface IDataImportExport.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include <string>

#include "exportdecl.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief This class defines all possible errors codes of interface IDataImportExport.
      **/
      class MOSAIK_MODULE ErrorCodes final
      {
      public:
        /**
          @brief Returns a documentary text that describes the specified error code.
          @param [in]  errorCode Code of an error, to which a description is queried.
          @returns A text describing the error situation.
        */static std::string getErrorText(int errorCode);

        /** @brief  ErrorCode to be returned if function call was successful. **/
        static const int Success;

        /** @brief ErrorCode to be returned if an operation has called that requires the underlying file to be opened, but
        the file has not been opened so far. Usually, readOpen() has to be called. 
        */static const int OpenFileHasNotBeenCalled;

        /** @brief ErrorCode to be returned if file could not be found. **/
        static const int FileNotFound;

        /** @brief Errorcode to be returned if path does not exist. **/
        static const int FolderNotFound;

        /** @brief ErrorCode to be returned if data to be written to file is inconsistent. Reasons can
        be mismatch between channel information (e.g different clock rate, center frequency if format does allow
        channels using same settings), ambiguous array names, invalid array lengths, etc. 
        */static const int InconsistentInputData;

        /** @brief ErrorCode to be returned if either FormatSpecifier or FormatSpecifierChannelInfo contains an invalid format specifier. **/
        static const int InvalidFormatSpecifier;

        /** @brief ErrorCode to be returned if the specified file cannot be opened for reading or writing, e.g. caused by an I/O error. 
        */static const int FileOpenError;

        /** @brief ErrorCode to be returned if iq-tar file does not contain xml. **/
        static const int InvalidFormatNoXmlFoundInTar;

        /** @brief ErrorCode to be returned if an error occurs while parsing the iq-tar content xml file. **/
        static const int InvalidFormatOfIQTarXmlContent;

        /** @brief ErrorCode to be returned if tar file is corrupt. **/
        static const int InvalidTarArchive;

        /** @brief ErrorCode to be returned if the read matlab array is not of the expected data type. **/
        static const int InvalidMatlabArrayType;

        /** @brief ErrorCode to be returned if the read matlab array is not of the expected dimension. **/
        static const int InvalidMatlabArraySize;

        /** @brief ErrorCode to be returned if the specified matlab array name does not exists in the opened file. **/
        static const int InvalidMatlabArrayName;

        /** @brief ErrorCode to be returned if the specified data column index exeeds the number of columns available in the file. **/
        static const int InvalidColumnIndex;

        /** @brief ErrorCode to be returned if an error occurs while parsing the matlab file content.
        The specified file is not a valid R/S I/Q matlab file. 
        */static const int InvalidFormatOfIqMatlabContent;

        /** @brief ErrorCode to be returned if the specified string is not valid xml. **/
        static const int InvalidXmlString;

        /** @brief ErrorCode to be returned if data format is not supported by file format. **/
        static const int InvalidDataFormat;

        /** @brief ErrorCode to be returned if any of the meta data to be written to a CSV file, contains the chosen value separator. **/
        static const int StringValuesContainValueSeparator;

        /** @brief ErrorCode to be returned if content of given file does not fulfill format requirements. **/
        static const int UnsupportedCsvFormat;

        /** @brief ErrorCode to be returned if data read from file does not contain any channels. **/
        static const int EmptyChannelInfo;

        /** @brief ErrorCode to be returned if data read from file does not contain a data array with the given name. **/
        static const int InvalidArrayName;

        /** @brief ErrorCode to be returned if no data could be read from file. **/
        static const int NoDataFoundInFile;

        /** @brief ErrorCode to be returned if the specified data offset exceeds the size of the data to be read. **/
        static const int StartIndexOutOfRange;

        /** @brief ErrorCode to be returned if the combination of offset and number of values to be read defines an invalid data interval. **/
        static const int InvalidDataInterval;

        /** @brief ErrorCode to be returned if the size of the data array used for interleaved channel reading is not divisible by 2. **/
        static const int InvalidArraySize;

        /** @brief ErrorCode to be returned if the amount of data written exceeds a specified limit. */
        static const int DataOverflow;

        /** @brief ErrorCode to be returned if the file is accessed for a write operation, but the file has not been initialized for writing. 
        * Usually writeOpen() has to be called first. 
        */static const int FileWriterUninitialized;

        /** @brief ErrorCode to be returned if the specified data type cannot be used in combination with this file format or the file has already
        * been initialized with a different data type.
        **/static const int WrongDataType;

        /** @brief ErrorCode to be returned if the file is being initialized for writing twice. **/
        static const int WriterAlreadyInitialized;

        /** @brief ErrorCode to be returned if the file is being initialized for reading twice. **/
        static const int ReaderAlreadyInitialized;

        /** @brief ErrorCode to be returned if the CSV file ends unepectetely. **/
        static const int CsvUnexpectedEndOfFile;

        /** @brief ErrorCode to be returned if no valid column value separator was found. **/
        static const int CsvInvalidSeparatorChar;

        /** @brief ErrorCode to be returned if a required metadata key was not found in the CSV file. **/
        static const int CsvKeyNotFound;

        /** @brief ErrorCode to be returned if the read CSV text line does not contain the expected number of value separators. **/
        static const int CsvInvalidNumberOfColumns;

        /** @brief ErrorCode to be returned if the specified decimal separator (i.e. ',' or '.') does not
        * match the separator found in the file. 
        */static const int CsvInvalidNumberFormat; 

        /** @brief ErrorCode to be returned if an error occurred while calculating the I/Q preview.
        */static const int IQPreviewError;

        /** @brief Unknown internal error occured. Should never occure. **/
        static const int InternalError;
      };
    }
  }
}