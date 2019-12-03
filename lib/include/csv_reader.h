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
* @file      csv_reader.h
*
* @brief     This is the header file of class CsvReader
*
* @details   Class providing functionality to write MOSAIK Csv format.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include <stdint.h>
#include <limits>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#include "ianalyzecontent.h"
#include "daiexception.h"
#include "common.h"
#include "errorcodes.h"
#include "enums.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief Class providing functionality to write MOSAIK Csv format.
      * TODOs: The read performance must be improved (Task 223795).
      */
      class CsvReader
      {
      public:
        /**
          @brief Constructor. Initializes a new instance with the specified filename.
          Be aware that the filename cannot be changed after construction.
          @param [in]  filename Name of the file to be read or to be written.
          @param [in]  updateContent Provides callback methods to process meta data found in file.
        */CsvReader(const std::string& filename, IAnalyzeContent& updateContent);

        /* @copydoc IqCsv::~Csv() */
        ~CsvReader();

        /**
          @brief Parses the file content and verifies that the file contains all information required
          to be a valid I/Q data file. I.e. all mandatory data and optional meta data is parsed and
          promoted to the callback methods that implement IAnalyzeContent.
        */void analyzeContent();

        /**
          @brief Returns all I/Q data array names found in this file. 
          @pre analyzeContent() must be called to beforehand, to parse the file.
          @param [out]  arrayNames Names of I/Q data arrays.
        */void getArrayNames(std::vector<std::string>& arrayNames) const;

        /**
          @brief Checks if the specified I/Q data array exists in the file read.
          @pre analyzeContent() must be called to beforehand, to parse the file.
          @param [in]  arrayName Name of the data array which is assumed in the file.
          @returns Returns TRUE if the I/Q data array was found in the file, otherwise
          FALSE is returned.
        */bool containsArray(const std::string& arrayName) const;

        /**
          @brief Returns the number of I/Q values contained by the specified array.
          @pre analyzeContent() must be called to beforehand, to parse the file.
          @param [in]  arrayName Name of the corresponding I/Q data array.
          @returns Returns the number of I/Q samples of the specified array.
          If the array does not exist in the file, -1 is returned.
        */long getNofSamples(const std::string& arrayName) const;

        /**
          @brief Reads the values of the specified array (e.g. 'Data_I' or 'Data_Q') and returns the values as single or 
          double precision vector, as defined by the type parameter. If the CSV file unexpectedly contains a
          string instead of a numeric value, the NaN will be set in the values-array for this position.
          @pre Make sure to call analyzeContent() to parse the data file.
          @remarks Reserve the size of the values-vector with the number of values to read in order
          to avoid memory reallocation within the method.
          @tparam The target precision of the value array - float or double.
          @param [in]  arrayName The name of the array to read.
          @param [out]  values The values read. Data read from file will be converted to the specified precision,
          independent of the original data type. Make sure that the values array has sufficient size.
          @param [in]  nofValues Number of values to read.
          @param [in]  offset Defines the number of values (real data) or I/Q pairs (for complex/polar data) respectively
          to be ignored at the beginning the the data arrays. To read the complete array, set offset value 0. Otherwise this parameter specifies the
          position in the array where the read operation starts.
          @returns If data was read successfully ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */template<typename T>
        void readArray(const std::string& arrayName, T* values, size_t nofValues, size_t offset)
        {
          // check if array name exists in file
          if (false == this->containsArray(arrayName))
          {
            throw DaiException(ErrorCodes::InvalidArrayName);
          }

          // check if combination of number of values to read, offset, and number of values in I/Q array
          // makes sense
          size_t samples = this->arrayNameToSamples_.at(arrayName);
          if (samples - offset <= 0)
          {
            throw DaiException(ErrorCodes::StartIndexOutOfRange);
          }

          if (nofValues + offset > samples)
          {
            throw DaiException(ErrorCodes::InvalidDataInterval);
          }

          // initialize array with NaN: If Csv contains a string value at a single cell,
          // the value will not be written to the values-array but the NaN will be returned 
          // at this position.
          std::fill(values, values + nofValues, std::numeric_limits<T>::quiet_NaN());

          std::fstream stream;
          try
          {
            this->openStreamFeedForward(stream);

            size_t columnIndex = this->arrayNameToColumnIndex_.at(arrayName);
            size_t valuesRead = 0;
            std::vector<std::string> columnValues;
            do
            {
              // read data
              if (valuesRead >= offset)
              {
                this->readLineAndSplit(stream, columnValues);
                if (columnValues.size() < columnIndex + 1)
                {
                  throw DaiException(ErrorCodes::CsvInvalidNumberOfColumns);
                }

                // convert string to floating number
                values[valuesRead - offset] = Common::toDecimal<T>(columnValues[columnIndex].c_str(), this->decimalSeperator_);
              }
              else
              {
                // skip line
                std::string line;
                this->readLine(stream, line);
              }

              // read next value
              valuesRead++;
            }
            while (valuesRead < nofValues + offset);

            stream.close();
          }
          catch (DaiException)
          {
            stream.close();
            throw;
          }
          catch (...)
          {
            stream.close();
            throw DaiException(ErrorCodes::InternalError);
          }
        }

        /**
          @brief Reads the values of the specified channel (e.g. 'Data') and returns them as single or
          double precision vector, as defined by the type parameter. If the CSV file unexpectedly contains a
          string instead of a numeric value, the NaN will be set in the values-array for this position.
          @tparam The target precision of the value array - float or double.
          @pre Make sure to call analyzeContent() to initialize an I/Q record before reading.
          @remarks Reserve the size of the values-vector with the number of values to read in order
          to avoid memory reallocation within the method.
          @param [in]  channelName The name of the channel to read.
          @param [out]  values The values read. Data read from file will be converted to single precision,
          independent of the original data type.
          @param [in]  nofValues Number of values to read.
          @param [in]  offset offset Defines the number of values (real data) or I/Q pairs (for complex/polar data) respectively
          to be ignored at the beginning the data arrays. To read the complete array, set offset value 0. Otherwise this parameter specifies the
          position in the array where the read operation starts.
          @returns If data was read successfully ErrorCodes.Success (=0) is returned. For further error codes, see \ref ErrorCodes.
        */template<typename T>
        void readChannel(const std::string& channelName, T* values, size_t nofValues, size_t offset)
        {
          // construct array name if this is a real-valued array
          std::string arrayName;
          if (this->dataFormat_ == IqDataFormat::Real)
          {
            arrayName = channelName;
          }
          else
          {
            arrayName = channelName + "_I";
          }

          // check if array name exists in file
          if (false == this->containsArray(arrayName))
          {
            throw DaiException(ErrorCodes::InvalidArrayName);
          }

          // get number of samples of this array
          size_t samples = this->arrayNameToSamples_.at(arrayName);

          // check if combination of number of values to read, offset, and number of values in I/Q array
          // makes sense
          if (samples - offset <= 0)
          {
            throw DaiException(ErrorCodes::StartIndexOutOfRange);
          }

          size_t samplesToRead = 0;
          if (this->dataFormat_ == IqDataFormat::Real)
          {
            samplesToRead = nofValues;
          }
          else
          {
            if (nofValues % 2 != 0)
            {
              throw DaiException(ErrorCodes::InvalidArraySize);
            }

            samplesToRead = nofValues / 2;
          }

          if (offset + samplesToRead > samples)
          {
            throw DaiException(ErrorCodes::InvalidDataInterval);
          }

          // initialize array with NaN: If Csv contains a string value at a single cell,
          // the value will not be written to the values-array but the NaN will be returned 
          // at this position.
          std::fill(values, values + nofValues, std::numeric_limits<T>::quiet_NaN());

          std::fstream stream;
          try
          {
            this->openStreamFeedForward(stream);

            size_t columnIndex = this->arrayNameToColumnIndex_.at(arrayName);
            size_t valuesRead = 0;
            size_t valueIndex = 0;
            std::vector<std::string> columnValues;
            do
            {
              if (valuesRead >= offset)
              {
                this->readLineAndSplit(stream, columnValues);
                if (columnValues.size() < columnIndex + 1)
                {
                  throw DaiException(ErrorCodes::CsvInvalidNumberOfColumns);
                }

                // read I data and convert string to floating number
                values[valueIndex] = Common::toDecimal<T>(columnValues[columnIndex].c_str(), this->decimalSeperator_);
                valueIndex++;

                // read Q data, if necessary and convert to floating number
                if (this->dataFormat_ != IqDataFormat::Real)
                {
                  values[valueIndex] = Common::toDecimal<T>(columnValues[columnIndex + 1].c_str(), this->decimalSeperator_);
                  valueIndex++;
                }
              }
              else
              {
                // skip line
                std::string line;
                this->readLine(stream, line);
              }

              valuesRead++;
            }
            while (valuesRead < samplesToRead + offset);

            stream.close();
          }
          catch (DaiException)
          {
            stream.close();
            throw;
          }
          catch (...)
          {
            stream.close();
            throw DaiException(ErrorCodes::InternalError);
          }
        }

        /**
          @returns Returns the number of columns found in the CSV file or -1 if an 
          error occurred.
        */int64_t getNofCols();

        /**
          @param [in]  column The CSV column index, starting from 0.
          @returns Returns the number of rows or -1 if an error occurred.
          Rows will be counted until the end of the file, an empty rows is
          found, or a row does not contain numeric data.
        */int64_t getNofRows(size_t column);

        /**
          @brief Reads values row-wise from the specified column. If the CSV file unexpectedly contains a
          string instead of a numeric value, the NaN will be set in the values-array for this position.
          @tparam The target precision of the value array - float or double.
          @param [in]  column The column index to read.
          @param [in]  nofValues The number of values (that is, the number of rows) to read.
          @param [out]  values The values read. Data read from file will be converted to single precision,
          independent of the original data type.
          @param [in]  offset Defines the start position (in this case this equals the row count) within the specified column at which the 
          read operation is started.
          @throws DaiException(CsvInvalidNumberFormat) Thrown if the used decimal separator could not be extracted from the CSV file.
          @throws DaiException(InvalidDataInterval) Thrown if the combination of offset and the number of values to read exceeds the 
          number of rows of the CSV file.
          @throws DaiException(CsvInvalidNumberOfColumns) Thrown if the column index exceeds the number of columns of the CSV file.
          @throws DaiException(InternalError) Thrown if an unknown error occurred. 
        */template<typename T>
        void readRawArray(size_t column, size_t nofValues, T* values, size_t offset)
        {
          // get number of columns in file
          auto nofCols = this->getNofCols();
          if (nofCols < static_cast<int64_t>(column))
          {
            throw DaiException(ErrorCodes::InvalidColumnIndex);
          }

          // initialize array with NaN: If Csv contains a string value at a single cell,
          // the value will not be written to the values-array but the NaN will be returned 
          // at this position.
          std::fill(values, values + nofValues, std::numeric_limits<T>::quiet_NaN());
          
          std::fstream stream;
          try
          {
            // get BOM size of file
            auto bomSize = this->openStreamIgnoreBom(this->filename_, stream);

            // remove header
            std::string line;
            this->readLine(stream, line);
            if (line.empty())
            {
              return;
            }

            // get column separator and decimal separator
            this->columnSeperator_ = CsvReader::extractColumnSeperatorForNumericLine(line);
            CsvReader::extractAndSetDecimalLocale(line);

            // if no decimal separator was extracted, the first line does not contain any number
            if (this->extractDecimalSeparator_)
            {
              stream.close();
              throw DaiException(ErrorCodes::CsvInvalidNumberFormat);
            }

            // skip BOM and seek to first value
            stream.seekg(bomSize, std::ios::beg);
            std::vector<std::string> columnValues;
            size_t valuesRead = 0;
            do
            {
              // read data
              if (valuesRead >= offset)
              {
                try
                {
                  this->readLineAndSplit(stream, columnValues);
                }
                catch (DaiException)
                {
                  // end of file exception -> actually the interval was out of range.
                  throw DaiException(ErrorCodes::InvalidDataInterval);
                }
                
                if (columnValues.size() < column + 1)
                {
                  throw DaiException(ErrorCodes::CsvInvalidNumberOfColumns);
                }

                // convert string to floating number
                values[valuesRead - offset] = Common::toDecimal<T>(columnValues[column].c_str(), this->decimalSeperator_);
              }
              else
              {
                // skip line
                std::string line;
                this->readLine(stream, line);
              }

              valuesRead++;
            }
            while (valuesRead < nofValues + offset);

            stream.close();
          }
          catch (DaiException &e)
          {
            stream.close();
            throw;
          }
          catch (...)
          {
            stream.close();
            throw DaiException(ErrorCodes::InternalError);
          }
        }

      private:
        /** @brief Private default constructor. */
        CsvReader();

        /** @brief Private copy constructor. */
        CsvReader(const CsvReader&);

        /** @brief Private assignment operator.*/
        CsvReader& operator=(const CsvReader&);

        /**
          @brief Identifies the column separator used in the CSV file.
          @param [in]  line First line of IqCsv file format, must end with a column separator.
          @returns Found column separator of CSV file.
          @throws DaiException(CsvInvalidSeparatorChar) Thrown if no valid separator was found.
        */static char extractColumnSeperator(const std::string& line);

        /**
          @brief Identifies the column separator used in a CSV file that does not match
          the R/S IqCsv file format, but merely contains columns with raw I/Q data.
          @param [in]  line Input CSV formatted line containing numeric data.
          @returns Found column separator of CSV file.
          @throws DaiException(CsvInvalidSeparatorChar) Thrown if no valid separator was found.
        */static char extractColumnSeperatorForNumericLine(const std::string& line);

        /**
          @brief Opens a stream of the specified file. If a BOM was detected at the
          beginning of the file, the BOM is skipped and the stream position is set
          to the first byte after the BOM. The actual encoding used for the file
          is not of interest, since the data is just passed to the host application.
          Therewith, the host application takes the responsibility to correctly display
          the string contents.
          @param [in]  filename Path to the file to be opened.
          @param [in]  stream Stream to be used to open the file.
          @returns Returns the size of the BOM mask found in bytes. If no BOM was found,
          zero is returned.
        */static size_t openStreamIgnoreBom(const std::string& filename, std::fstream& stream);

        /**
          @brief Opens a stream using filename_ and sets the cursor position to the end the 
          "array names"-line. Lines beyond this position will be I/Q data.
          @param [out]  stream Open file stream with cursor set to headerSectionEndOffset_.
        */void openStreamFeedForward(std::fstream& stream);

        /**
          @brief Method to encapsulate reading the next line in combination with error handling 
          and incrementing the index of the currently read line.
          @param [out]  stream Open file stream.
          @param [in]  line A string representing the next line that was read.
          @throws DaiException(CsvUnexpectedEndOfFile) Thrown if read line is empty.
        */void readLine(std::fstream& stream, std::string& line);

        /**
          @brief Reads the next line from the stream and splits the content
          after each separator. 
          @param [in]  stream The input file stream.
          @param [out]  result Vector containing all found tokens.
        */void readLineAndSplit(std::fstream& stream, std::vector<std::string>& result);

        /**
          @brief Reads the mandatory metadata, which is: ApplicationName, Comment, Timestamp, DataFormat, and ChannelCount.
          @param [in]  stream Input file stream.
        */void readMandatoryMetadata(std::fstream& stream);

        /**
          @brief Read all key - value pairs provided as optional metadata.
          @param [in]  stream Input file stream.
        */void readOptionalMetadata(std::fstream& stream);

        /**
          @brief Reads channel infos (channelName, samples, clock, center frequency, data format).
          @param [in]  stream Input file stream.
          @param [in]  channel Channel number to read.
        */void readChannel(std::fstream& stream, size_t channel);

        /**
          @brief Reads the next line from the stream and interprets the content
          as key-value metadata. The specified key is expected to be the first token of the line.
          @param [in]  stream Input file stream.
          @param [in]  key The key of the expected metadata.
          @returns Returns the corresponding value of the metadata key.
          @throws DaiException(CsvInvalidNumberOfColumns) if the number of found separators is
          incorrect.
          @throws DaiException(CsvKeyNotFound) If the line read does not contain the specified key.
        */std::string readValueForKey(std::fstream& stream, const std::string& key);

        /**
          @brief Extracts the locale of decimal values, e.g. if a point is used as 
          decimal separator (1.2222), or a comma (1,2222) respectively.
          @param [in]  input Input string that contains a decimal value.
        */void extractAndSetDecimalLocale(const std::string& input);

        /** @brief Vector containing byte order masks (BOM). */
        static const std::vector<std::string> boms_;

        /** @brief Filename of the CSV file. */
        std::string filename_;

        /** @brief I/Q data type of data to be written. */
        IqDataType dataType_;

        /** @brief Data format of this file. */
        IqDataFormat dataFormat_;

        /** @brief Callback functions that process meta data found in file.  Called by analyzeContent(). */
        IAnalyzeContent* updateContent_;

        /** @brief Set true, if analyseContent() was called and the file was parsed successfully. */
        bool initialized_;

        /** @brief A value indicating whether the decimal separator used when saving the file still has to be extracted or not.
          Depending on the content of the file, it can either be detected within the channel settings Clock or
          CenterFrequency or in the first row of data values.
        */bool extractDecimalSeparator_;

        /** @brief Character used as separator of floating values, e.g. 1,00 or 1.00. */
        char decimalSeperator_;

        /** @brief Character used as column separator in CSV file. */
        char columnSeperator_;

        /** @brief Number of I/Q channels contained in file. */
        size_t channelCount_;

        /** @brief The byte offset to the row which contains the index 0 value of all data arrays. */
        std::fstream::pos_type headerSectionEndOffset_;

        /** @brief Mapping between an array name and the number of samples contained
          by the array. 
        */std::map<std::string, size_t> arrayNameToSamples_;

        /** @brief Mapping between an array name and the column of the CSV file that stores the
          I/Q data of this array. 
        */std::map<std::string, size_t> arrayNameToColumnIndex_;
      };
    }
  }
}