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

#include "errorcodes.h"

using namespace std;

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
        const int ErrorCodes::Success = 0;

        const int ErrorCodes::OpenFileHasNotBeenCalled = -1;

        const int ErrorCodes::FileNotFound = -10;

        const int ErrorCodes::FolderNotFound = -11;

        const int ErrorCodes::InconsistentInputData = -14;

        const int ErrorCodes::InvalidFormatSpecifier = -15;

        const int ErrorCodes::FileOpenError = -17;

        const int ErrorCodes::InvalidFormatNoXmlFoundInTar = -20;

        const int ErrorCodes::InvalidFormatOfIQTarXmlContent = -21;

        const int ErrorCodes::InvalidTarArchive = -22;

        const int ErrorCodes::InvalidMatlabArrayType = -23;

        const int ErrorCodes::InvalidMatlabArraySize = -24;

        const int ErrorCodes::InvalidMatlabArrayName = -25;

        const int ErrorCodes::InvalidColumnIndex = -26;

        const int ErrorCodes::InvalidFormatOfIqMatlabContent = -27;

        const int ErrorCodes::InvalidXmlString = -28;

        const int ErrorCodes::InvalidDataFormat = -30;

        const int ErrorCodes::StringValuesContainValueSeparator = -40;

        const int ErrorCodes::UnsupportedCsvFormat = -41;

        const int ErrorCodes::EmptyChannelInfo = -100;

        const int ErrorCodes::InvalidArrayName = -101;

        const int ErrorCodes::NoDataFoundInFile = -102;

        const int ErrorCodes::StartIndexOutOfRange = -103;

        const int ErrorCodes::InvalidDataInterval = -104;

        const int ErrorCodes::InvalidArraySize = -105;

        const int ErrorCodes::DataOverflow = -106;

        const int ErrorCodes::FileWriterUninitialized = -107;

        const int ErrorCodes::WrongDataType = -108;

        const int ErrorCodes::WriterAlreadyInitialized = -110;

        const int ErrorCodes::ReaderAlreadyInitialized = -111;

        const int ErrorCodes::CsvUnexpectedEndOfFile = -1000;

        const int ErrorCodes::CsvInvalidSeparatorChar = -1001;

        const int ErrorCodes::CsvKeyNotFound = -1002;

        const int ErrorCodes::CsvInvalidNumberOfColumns = -1003;

        const int ErrorCodes::CsvInvalidNumberFormat = -1005;

        const int ErrorCodes::IQPreviewError = -2000;

        const int ErrorCodes::InternalError = -99999;

      std::string ErrorCodes::getErrorText(int errorCode)
      {
        switch (errorCode)
        {
        case ErrorCodes::Success:
          return "Operation was successful.";

        case ErrorCodes::OpenFileHasNotBeenCalled:
          return "OpenFile has to be called before any of the Get functions can be called.";

        case ErrorCodes::FileNotFound:
          return "The file could not be found.";

        case ErrorCodes::FolderNotFound:
          return "The specified path could not be found";

        case ErrorCodes::InconsistentInputData:
          return "Input data is inconsistent (see documentation of file format requirements).";

        case ErrorCodes::InvalidFormatSpecifier:
          return "The set FormatSpecifier is not valid.";

        case ErrorCodes::FileOpenError:
          return "File could not be opened.";

        case ErrorCodes::InvalidFormatNoXmlFoundInTar:
          return "Given iq-tar file does not contain the required xml file.";

        case ErrorCodes::InvalidFormatOfIQTarXmlContent:
          return "Could not parse the xml file containing the meta data.";

        case ErrorCodes::InvalidTarArchive:
          return "Cannot read Tar archive.";

        case ErrorCodes::InvalidMatlabArrayType:
          return "Matlab array is of unexpected data type.";

        case ErrorCodes::InvalidMatlabArraySize:
          return "Matlab array is of unexpected size.";

        case ErrorCodes::InvalidMatlabArrayName:
          return "No data field with specified name found in matlab file.";

        case ErrorCodes::InvalidColumnIndex:
          return "The specified column index exceeds the number of data columns in the file.";

        case ErrorCodes::InvalidFormatOfIqMatlabContent:
          return "Could not parse the matlab file. No valid R&S I/Q matlab file.";

        case ErrorCodes::InvalidXmlString:
          return "The specified string does not represent valid XML.";

        case ErrorCodes::InvalidDataFormat:
          return "IQW only supports one channel complex float32 data.";

        case ErrorCodes::StringValuesContainValueSeparator:
          return "The separator character to be used for CSV format cannot be used within the string values to be saved";

        case ErrorCodes::UnsupportedCsvFormat:
          return "DataImportExport does not provide a general CSV reader, but expects specific tokens. The current file does not fulfill those requirements.";

        case ErrorCodes::EmptyChannelInfo:
          return "Data read from file does not contain any channels.";

        case ErrorCodes::InvalidArrayName:
          return "File does not contain a data array with the given name.";

        case ErrorCodes::NoDataFoundInFile:
          return "File does not contain the requested data.";

        case ErrorCodes::StartIndexOutOfRange:
          return "Offset exceeds the size of the data to be read from file.";

        case ErrorCodes::InvalidDataInterval:
          return "Combination of offset and the number of values to be read defines a data interval that can not be read.";

        case ErrorCodes::InvalidArraySize:
          return "Invalid array size. Must be divisible by 2.";

        case ErrorCodes::DataOverflow:
          return "Number of bytes written exceeds specified file size.";

        case ErrorCodes::FileWriterUninitialized:
          return "The file writer has not been initialized";

        case ErrorCodes::WrongDataType:
          return "The type of the input data does not match the data type already written to file.";

        case ErrorCodes::WriterAlreadyInitialized:
          return "The file has already been initialized for writing data. Do not initialize twice.";

        case ErrorCodes::ReaderAlreadyInitialized:
          return "The file has already been initialized for reading data. Do not initialize twice.";

        case ErrorCodes::CsvUnexpectedEndOfFile:
          return "The file ended unexpectedly.";

        case ErrorCodes::CsvInvalidSeparatorChar:
          return "The found column separator is invalid.";

        case ErrorCodes::CsvKeyNotFound:
          return "The specified metadata key does not exists.";

        case ErrorCodes::CsvInvalidNumberOfColumns:
          return "The read line from the CSV file does not contain the expected number of columns.";

        case ErrorCodes::CsvInvalidNumberFormat:
          return "No valid decimal separator found.";

        case ErrorCodes::IQPreviewError:
          return "Error while calculating I/Q preview.";

        case ErrorCodes::InternalError:
          return "Internal coding error.";

        default:
          return "unknown error code";
        }
      }
    }
  }
}