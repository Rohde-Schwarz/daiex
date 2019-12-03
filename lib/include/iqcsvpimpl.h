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
* @file      iqcsvpimpl.h
*
* @brief     This is the header file of class IqCsv::PImpl.
*
* @details   Private implementation of class IqCsv.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include <sstream>
#include <algorithm>

#include "icsvselector.h"
#include "dataimportexportbase.h"
#include "iqcsv.h"
#include "csv_reader.h"
#include "csv_writer.h"
#include "platform.h"
#include "constants.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief Private implementation of class IqCsv.
      */
      class IqCsv::Impl final : public DataImportExportBase, IAnalyzeContent, ICsvSelector
      {
      public:
        /**
          @brief Constructor. Initializes a new instance with the specified filename.
          Be aware that the filename cannot be changed after construction.
          @param [in]  filename Name of the file to be read or to be written.
        */Impl(const std::string& filename);

        /** Destructor. */
        ~Impl();

        int readOpen(std::vector<std::string>& arrayNames);
        int writeOpen(
          IqDataFormat format,
          size_t nofArrays,
          const std::string& applicationName,
          const std::string& comment,
          const std::vector<ChannelInfo>& channelInfos,
          const std::map<std::string, std::string>* metadata = 0);
        int close();

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
        Impl();

        /** @brief Private copy constructor. */
        Impl(const Impl&);

        /** @brief Private assignment operator.*/
        Impl& operator=(const Impl&);

        /**
          @brief Checks if the specified string represents a valid format specifier for decimal values.
          Compare \ref setFormatSpecifier() for valid specifiers.
          @param [in]  formatSpecifier The string representing a format specifier.
          @returns Returns TRUE if the string represents a valid format specifier, otherwise FALSE is returned.
        */static bool validateFormatSpecifier(std::string& formatSpecifier);

        /**
          @brief Verifies if the specified meta data strings contain the specified separator. For a valid CSV file,
          the meta data must not contain the separator.
          @param [in]  separator Separator character that must not be contained by meta data.
          @param [in]  applicationName Meta data: application name.
          @param [in]  comment Meta data: comment.
          @param [in]  metadata Meta data: key-value pairs containing user-defined meta data.
          @returns Returns TRUE if separator was not found in meta data. The meta data is ready to be written to file.
          If FALSE is returned, the separator was found and the meta data cannot be written to file. 
        */static bool metadataContainsValueSeparator(const char separator, const std::string& applicationName, const std::string& comment, const std::map<std::string, std::string>* metadata);

        /**
          @brief Sets default channel names, if channel names are empty and removes white spaces from
          channel names.
          @param [in]  channelNames Currently used channel information.
          @param [in]  out Channel information with updated channel names.
        */static void completeChannelInfoNames(const std::vector<ChannelInfo>& channelNames, std::vector<ChannelInfo>& out);

        /**
          @brief Converts the specified decimal number to string using formatSpecifier as
          value separator. Do not change locale, as locale is always valid within entire thread!
          @tparam Template parameter of the value, float or double.
          @param [in]  value Float or double value.
          @param [in]  formatSpecifier Format specifier used to convert value to string.
          @param [in]  numberDecimalSeparator Specifies whether a dot or a column is used as decimal separator.
          @returns Returns the converted value as string.
          @throws DaiException(InternalError) if the conversion fails.
        */template<typename T>
        inline static std::string getFormattedValue(T value, const std::string& formatSpecifier, const char numberDecimalSeparator)
        {
          char buffer[200];
          std::string format = "%." + formatSpecifier;
          int ret = _snprintf_s(buffer, sizeof(buffer), format.c_str(), value);
          if (ret == 0)
          {
            throw DaiException(ErrorCodes::InternalError);
          }

          std::string strValue(buffer);

          // _snprintf_s always formats value with '.' -> if ',' separator is configured,
          // change separator
          if (numberDecimalSeparator == Constants::SeparatorColon)
          {
            Common::strReplace(strValue, ".", ",");
          }

          return strValue;
        }

        void updateChannels(const std::string& channelName, double clock, double frequency, size_t samples);
        void updateMetadata(const std::string& key, const std::string& value);
        void updateTimestamp(const time_t timestamp);

        /**
          @brief Writes the file header containing all meta data. 
          The file header is written twice. Once at the time of initialization (\ref writeOpen()), 
          where all known information is written to file and placeholders are inserted for 
          currently unknown parameters. To complete the missing information, the method is called a 
          second time after I/Q data has been written and the file is closed.
          @param [in]  finalize TRUE if I/Q data has been written and the file is being closed, TRUE
          if the file is currently being initialized (writeOpen()).
        */void writeMetadata(bool finalize);

        /**
         @brief Closes the file completes the meta data information by calling writeMetadata(TRUE).
        */void finalizeTemporarySequence();

        /**
          @brief Appends I/Q data arrays to the CSV file.
          @tparam Template parameter of the I/Q data precision, i.e. float or double.
          @param [in]  iqdata Vector of pointers to the first data element of I/Q data.
          @param [in]  sizes Vector containing the length of the iqdata arrays.
          @throws DaiException(FileWriterUninitialized) If file writer has not been initialized.
          @throws DaiException(InconsistentInputData) If input data does not match file format criteria.
          @throws DaiException(WrongDataType) If input data does not match the data type of the file. The data type
          is set while adding the first I/Q samples.
          @throws DaiException(InternalError) Probably the conversion of numerical values to float failed.
        */template<typename T>
        void writeArray(const std::vector<T*>& iqdata, const std::vector<size_t>& sizes)
        {
          // check if file writer is initialized
          if (this->writer_ == nullptr)
          {
            throw DaiException(ErrorCodes::FileWriterUninitialized);
          }

          // init data type to float or double. 
          DataImportExportBase::initializeDataType<T>(this->dataType_, this->lockDataType_);

          // validate input data consistence
          if (false == DataImportExportBase::validateInputData(this->channelInfos_, sizes, this->dataFormat_))
          {
            throw DaiException(ErrorCodes::InconsistentInputData);
          }

          // Minimum number of columns in the csv file is 2 as all meta data is represented as a key - value pair
          // arrayCount could be 1 in case of single channel real data!
          size_t rowCount = *std::max_element(sizes.begin(), sizes.end());
          size_t arrayCount = sizes.size();
          size_t columnCount = std::max(static_cast<size_t>(2), arrayCount);

          std::string firstVal = IqCsv::Impl::getFormattedValue(iqdata[0][0], this->formatSpecifier_, this->numberDecimalSeparator_);
          auto rowSize = arrayCount * (firstVal.size() + 1);
          std::string rowString;
          rowString.reserve(rowSize);

          // write CSV data row-wise
          for (size_t rowIdx = 0; rowIdx < rowCount; ++rowIdx)
          {
            // clear string but keep reserved memory
            std::string().swap(rowString);

            // write data columns
            for (size_t arrayIdx = 0; arrayIdx < arrayCount - 1; ++arrayIdx)
            {
              // As value arrays can have differing length, check if the current array still has values to be added
              // to the current row.
              if (sizes[arrayIdx] > rowIdx)
              {
                rowString += IqCsv::Impl::getFormattedValue(iqdata[arrayIdx][rowIdx], this->formatSpecifier_, this->numberDecimalSeparator_) + this->valueSeparator_;
              }
              else
              {
                // Row number exceeds the length of this array-> Only add a separator for an empty column.
                rowString += this->valueSeparator_;
              }
            }

            if (sizes[arrayCount - 1] > rowIdx)
            {
              rowString += IqCsv::Impl::getFormattedValue(iqdata[arrayCount - 1][rowIdx], this->formatSpecifier_, this->numberDecimalSeparator_);
            }
            // else, if last column is empty no separator is needed!

            // Handle special case, where file has 2 columns due to meta data but only one
            // value array ... empty column is added in order to fulfill the following requirement
            // "Each record "should" contain the same number of comma-separated fields."
            if (arrayCount < columnCount)
            {
              rowString += this->valueSeparator_;
            }

            this->writer_->writeLine(rowString);
          }

          // count number of samples written
          if (this->dataFormat_ == IqDataFormat::Real)
          {
            for (size_t i = 0; i < sizes.size(); ++i)
            {
              this->nofSamplesWrittenPerChannel_[i] += sizes[i];
            }
          }
          else
          {
            for (size_t i = 0; i < sizes.size(); i += 2)
            {
              this->nofSamplesWrittenPerChannel_[i / 2] += sizes[i];
            }
          }

          // write to file
          this->writer_->flush();
        }

        /**
          @brief Appends I/Q channels to the CSV file.
          @tparam Template parameter of the I/Q data precision, i.e. float or double.
          @param [in]  iqdata Vector of pointers to the first channel element. Data is stored
          in interleaved (IQIQIQ) format per channel.
          @param [in]  sizes Vector containing the length of each channel.
          @throws DaiException(FileWriterUninitialized) If file writer has not been initialized.
          @throws DaiException(InconsistentInputData) If input data does not match file format criteria.
          @throws DaiException(WrongDataType) If input data does not match the data type of the file. The data type
          is set while adding the first I/Q samples.
          @throws DaiException(InternalError) Probably the conversion of numerical values to float failed.
        */template<typename T>
        void writeChannel(const std::vector<T*>& iqdata, const std::vector<size_t>& sizes)
        {
          // check if file writer is initialized
          if (this->writer_ == nullptr)
          {
            throw DaiException(ErrorCodes::FileWriterUninitialized);
          }

          // handle real-valued data
          if (this->dataFormat_ == IqDataFormat::Real)
          {
            this->appendArrays(iqdata, sizes);
         
            return;
          }

          if (this->channelInfos_.size() != iqdata.size() || this->channelInfos_.size() != sizes.size())
          {
            throw DaiException(ErrorCodes::InconsistentInputData);
          }

          // init data type
          DataImportExportBase::initializeDataType<T>(this->dataType_, this->lockDataType_);

          // Minimum number of columns in the csv file is 2 as all meta data is represented as a key - value pair
          // arrayCount could be 1 in case of single channel real data!
          size_t rowCount = *std::max_element(sizes.begin(), sizes.end()) / 2;
          size_t arrayCount = sizes.size();

          std::string firstVal = IqCsv::Impl::getFormattedValue(iqdata[0][0], this->formatSpecifier_, this->numberDecimalSeparator_);
          auto rowSize = arrayCount * (firstVal.size() + 1);
          std::string rowString;
          rowString.reserve(rowSize);

          for (size_t rowIdx = 0; rowIdx < rowCount; ++rowIdx)
          {
            // clear string but keep reserved memory
            std::string().swap(rowString);

            for (size_t arrayIdx = 0; arrayIdx < arrayCount; ++arrayIdx)
            {
              // As value arrays can have differing length, check if the current array still has values to be added
              // to the current row.
              if (sizes[arrayIdx] / 2 > rowIdx)
              {
                // I
                rowString += IqCsv::Impl::getFormattedValue(iqdata[arrayIdx][2 * rowIdx], this->formatSpecifier_, this->numberDecimalSeparator_) + this->valueSeparator_;

                // Q
                rowString += IqCsv::Impl::getFormattedValue(iqdata[arrayIdx][2 * rowIdx +1], this->formatSpecifier_, this->numberDecimalSeparator_) + this->valueSeparator_;
              }
              else
              {
                // Row number exceeds the length of this array-> Only add a separator for an empty column.
                rowString += this->valueSeparator_;
              }
            }

            this->writer_->writeLine(rowString);
          }

          // count number of samples written
          for (size_t i = 0; i < sizes.size(); ++i)
          {
            if (this->dataFormat_ == IqDataFormat::Real)
            {
              this->nofSamplesWrittenPerChannel_[i] += sizes[i];
            }
            else
            {
              this->nofSamplesWrittenPerChannel_[i] += sizes[i] / 2;
            }
          }

          this->writer_->flush();
        }

        /** @brief CSV reader instance. */
        CsvReader* reader_;

        /** @brief CSV writer instance. */
        CsvWriter* writer_;

        /** @brief Number of values written per channel, where each element of the vector
        * represents one channel. */
        std::vector<size_t> nofSamplesWrittenPerChannel_;

        /** @brief Character to be used as separator for the values written in one line of the UTF8-encoded file. 
        * See CsvSeparatorConfig for a valid configuration. 
        */char valueSeparator_;

        /** @brief Character used to format decimal numbers, i.e. '2.44' or '2,44'.
        * See CsvSeparatorConfig for a valid configuration. 
        */char numberDecimalSeparator_;

        /** @brief The format specified used to convert numeric values of I/Q data to string. 
        * The default value is "7E".
        */std::string formatSpecifier_;

        /** @brief The format specified used to convert numeric values occurring in meta data to string. 
        * The default value is "7E".
        */std::string formatSpecifierChannelInfo_;

        /** @brief If set TRUE, dataType_ cannot be changed. Value is set at first call
        of appendArrays() or appendChannels(). */
        bool lockDataType_;

        /** @brief Number of data arrays. */
        size_t nofArrays_;

        /** @brief Format of I/Q data. */
        IqDataFormat dataFormat_;

        /** @brief Precision used to write data to file. */
        IqDataType dataType_;

        /** @brief Name of the application or instrument exporting its data. */
        std::string applicationName_;

        /** @brief Text that further describes the contents of the file. */
        std::string comment_;
      };
    }
  }
}
