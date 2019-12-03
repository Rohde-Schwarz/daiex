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
* @file      iqcsv.h
*
* @brief     This is the header file of class Csv.
*
* @details   Class to read or write I/Q dat from or to a CSV file.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include "idataimportexport.h"
#include "icsvselector.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
        @brief An enumeration that defines allowed combinations of the value separator for the CSV format
        and the decimal separator used to save floating-point values within the CSV format.
        As those two separators need to be different, this enumeration is used to define valid combinations.
        The group separator needed when formatting floating-point values is always set to an empty string.
      */enum CsvSeparatorConfig : int
      {
        /**
          A "," is used as decimal separator, while a ";" separates the individual values.
          Example: 0,123;23,45
        */DecimalColonValueSemicolon = 0,

        /**
          A "." is used as decimal separator, while a ";" separates the individual values.
          Example: 0.123;23.45
        */DecimalDotValueSemicolon,

        /**
          A "." is used as decimal separator, while a "," separates the individual values.
          Example: 0.123,23.45
        */DecimalDotValueColon,
      };

      /**
        @brief Class to read or write I/Q data or general measurement data from or to a *.csv file.
        CSV - "comma-separated values" - is a file format to store tabular data (numbers and text) in plain text. It is widely used, but
        no general standard exists for this file format. This implementation makes the following assumptions/restrictions:
        - Each record ( = line ) in the text file contains the same number of columns/separator chars.
        - First line of the file will contain an 'CreationIdentifier' (i.e. 'saved by Rohde &amp; Schwarz MOSAIK DataImportExport') followed by a block of mandatory
          and a block of optional meta information, where the key is denoted in the first column and the value in the second column.
        - Meta information will be followed by a line containing the array names, which have to be used to extract the array data.
        - The array data is stored column-wise.
        - Separators are restricted to CsvSeparatorConfig.
        - UTF8 character set is used.
        
        Example:
         - @c saved by Rohde &amp; Schwarz MOSAIK DataImportExport;
         - @c DataImportExport_MandatoryData;
         - @c Name;ExampleApplication
         - @c Comment;a comment ...
         - @c DateTime;2013-08-05T10:59:55
         - @c Format;complex
         - @c DataType;float64
         - @c NumberOfChannels;1
         - @c Ch1_ChannelName;MyChannel1
         - @c Ch1_Samples;1001
         - @c Ch1_Clock[Hz];32000000
         - @c Ch1_CenterFrequency[Hz];13250000000
         - @c DataImportExport_OptionalData;
         - @c Key1;Wert1
         - @c Key2;Wert2
         - @c DataImportExport_EndHeaderSection;
         - @c MyChannel1_I;MyChannel1_Q
         - @c MyChannel1_I[0];MyChannel1_Q[0]
         - @c MyChannel1_I[1];MyChannel1_Q[1]
         - @c ...
      */class MOSAIK_MODULE IqCsv : public IDataImportExport, ICsvSelector
      {
      public:
        /**
          @brief Constructor. Initializes a new instance with the specified filename.
          Filename must be UTF-8 encoded. Be aware that the filename cannot be changed after construction.
          @param [in]  filename Name of the file to be read or to be written.
        */IqCsv(const std::string& filename);

        /** @brief Destructor. Calls close()
        */~IqCsv();

        /**
          @brief Sets a valid combination of characters to be used as decimal separator and value separator.
          Must be set before file is opened for write operations (\ref writeOpen()).
          @param [in]  config Separator configuration to be used.
          @returns Returns ErrorCode::Success (=0) if value was set. If file has already been opened 
          for writing, ErrorCodes::WriterAlreadyInitialized is returned.
        */int setSeparatorConfiguration(const CsvSeparatorConfig config);

        /**
          @returns Returns the currently used configuration of decimal and value separator.
          See CsvSeparatorConfig for valid combinations.
        */CsvSeparatorConfig getSeparatorConfiguration() const;

        /**
          @returns Character to be used as separator for the values written in one line of the UTF8-encoded CSV file.
          Can be chosen via setSeparatorConfiguration().
        */char getValueSeparator() const;

        /**
          @returns Character to be used as a decimal separator when converting floating-point numbers to string. 
          Can be chosen via setSeparatorConfiguration().
        */char getDecimalSeparator() const;

        /**
          @brief Sets the format specified used to convert numeric values to string. The format specifier is
          used to convert I/Q values to the specified format while saving data to file. The default value is "7E".
          Must be set before file is opened for writing. Otherwise an error will be returned.
          Accepted specifiers are:
          
          - Exponential (scientific) notation [ "E" / "e" ].
          Converts a number to a string of the form "-d.ddd...E+ddd" or "-d.ddd...e+ddd",
          where each "d" indicates a digit (0-9). The string starts with a minus sign if the number is negative.
          Exactly one digit always precedes the decimal point.
          The precision specifier indicates the desired number of digits after the decimal point.
          The case of the format specifier indicates whether to prefix the exponent with an "E" or an "e".
          - The general format specifier [ "G" ].
          Converts a number to the most compact of either fixed-point or scientific notation,
          depending on the type of the number and whether a precision specifier is present.
          The precision specifier defines the maximum number of significant digits that can appear
          in the result string. 
          - Fixed point notation [ "F" ].
          Converts a number to a string of the form "-ddd.ddd..." where each "d" indicates a digit (0-9).
          The string starts with a minus sign if the number is negative.
          The precision specifier indicates the desired number of decimal places.
          
          For further information see http://www.cplusplus.com/reference/cstdio/scanf/
          @param [in]  format String containing the number format specifier.
          @returns Returns ErrorCode::Success (=0) is specifier was set. If the file has already
          been opened, ErrorCode::WriterAlreadyInitialized will be returned.
        */int setFormatSpecifier(const std::string& format);

        /**
          @returns Returns the format specifier currently used to convert numeric values to string
          while writing a CSV file.
        */const std::string& getFormatSpecifier() const;

        /**
          @brief Sets the format specified used to convert numeric values contained by ChannelInfo objects to string, i.e.
          clock rate and center frequency. The default value is "7E".
          @param [in]  format String containing the number format specifier.
        */int setFormatSpecifierChannelInfo(const std::string& format);

        /**
          @returns Returns the format specifier currently used to convert numeric values contained by ChannelInfo object to string.
        */const std::string& getFormatSpecifierChannelInfo() const;

        time_t getTimestamp() const;
        void setTimestamp(const time_t timestamp);

        int getMetadata(std::vector<ChannelInfo>& channelInfos, std::map<std::string, std::string>& metadata) const;

        int readOpen(std::vector<std::string>& arrayNames);
        int writeOpen(
          IqDataFormat format,
          size_t nofArrays,
          const std::string& applicationName,
          const std::string& comment,
          const std::vector<ChannelInfo>& channelInfos,
          const std::map<std::string, std::string>* metadata = 0);
        int close();

        int64_t getArraySize(const std::string& arrayName) const;

        int readArray(const std::string& arrayName, std::vector<float>& values, size_t nofValues, size_t offset = 0);
        int readArray(const std::string& arrayName, float* values, size_t nofValues, size_t offset = 0);
        int readArray(const std::string& arrayName, std::vector<double>& values, size_t nofValues, size_t offset = 0);
        int readArray(const std::string& arrayName, double* values, size_t nofValues, size_t offset = 0);

        int readChannel(const std::string& channelName, std::vector<float>& values, size_t nofValues, size_t offset = 0);
        int readChannel(const std::string& channelName, float* values, size_t nofValues, size_t offset = 0);
        int readChannel(const std::string& channelName, std::vector<double>& values, size_t nofValues, size_t offset = 0);
        int readChannel(const std::string& channelName, double* values, size_t nofValues, size_t offset = 0);

        int appendArrays(const std::vector<std::vector<float>>& iqdata);
        int appendArrays(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes);
        int appendArrays(const std::vector<std::vector<double>>& iqdata);
        int appendArrays(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes);

        int appendChannels(const std::vector<std::vector<float>>& iqdata);
        int appendChannels(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes);
        int appendChannels(const std::vector<std::vector<double>>& iqdata);
        int appendChannels(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes);

        int64_t getNofRows(size_t column);
        int64_t getNofCols();
        int readRawArray(size_t column, size_t nofValues, std::vector<float>& values, size_t offset = 0);
        int readRawArray(size_t column, size_t nofValues, float* values, size_t offset = 0);
        int readRawArray(size_t column, size_t nofValues, std::vector<double>& values, size_t offset = 0);
        int readRawArray(size_t column, size_t nofValues, double* values, size_t offset = 0);

      private:
        /** @brief Private default constructor. */
        IqCsv();

        /** @brief Private copy constructor. */
        IqCsv(const IqCsv&);

        /** @brief Private assignment operator.*/
        IqCsv& operator=(const IqCsv&);

        /** @brief Private implementation */
        class Impl;

        /** @brief Private implementation */
        Impl* pimpl;
      };
    }
  }
}
