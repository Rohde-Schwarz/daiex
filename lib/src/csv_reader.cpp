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

#include "csv_reader.h"

#include "common.h"

#include "iqcsvpimpl.h"
#include "daiexception.h"
#include "constants.h"

using namespace std;

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /** @brief Array containing possible BOMs.*/
      const std::string boms[11] =
      {
        "\xEF\xBB\xBF",
        "\xFE\xFF",
        "\xFF\xFE"
        "\x00\x00\xFE\xFF",
        "\xFF\xFE\x00\x00",
        "\x2B\x2F\x76",
        "\xF7\x64\x4C",
        "\xDD\x73\x66\x73",
        "\x0E\xFE\xFF",
        "\xFB\xEE\x28",
        "\x84\x31\x95\x33"
      };

      const std::vector<std::string> CsvReader::boms_(boms, boms + 11);
    

      CsvReader::CsvReader(const std::string& filename, IAnalyzeContent& updateContent) :
        filename_(filename),
        updateContent_(&updateContent),
        initialized_(false),
        extractDecimalSeparator_(true),
        decimalSeperator_(Constants::SeparatorColon),
        columnSeperator_(Constants::SeparatorSemiColon),
        channelCount_(0)
      {
      }

      CsvReader::~CsvReader()
      {
      }

      size_t CsvReader::openStreamIgnoreBom(const std::string& filename, std::fstream& stream)
      {
        Platform::streamOpen(stream, filename, ios::in | ios::binary);
        if (false == stream.is_open())
        {
          throw DaiException(ErrorCodes::FileOpenError);
        }

        // remove BOM, if existing
        string read;
        read.resize(4);
        stream >> read[0] >> read[1] >> read[2] >> read[3];

        for (size_t i = 0; i < CsvReader::boms_.size(); ++i)
        {
          if (Common::strStartsWithIgnoreCase(read, CsvReader::boms_[i]))
          {
            size_t bomSize = boms_[i].size();
            stream.seekg(bomSize, ios::beg);
            return bomSize;
          }
        }

        return 0;
      }

      void CsvReader::openStreamFeedForward(std::fstream& stream)
      {
        Platform::streamOpen(stream, this->filename_, ios::in | ios::binary);
        if (false == stream.is_open())
        {
          throw DaiException(ErrorCodes::FileOpenError);
        }

        stream.seekg(this->headerSectionEndOffset_, ios::beg);
      }

      void CsvReader::analyzeContent()
      {
        if (this->initialized_)
        {
          return;
        }

        if (this->filename_.empty())
        {
          throw DaiException(ErrorCodes::FileNotFound);
        }

        fstream stream;
        try
        {
          size_t bomSize = CsvReader::openStreamIgnoreBom(this->filename_, stream);

          // Allow file format with and without CreationIdentifier in first line. Request of Bjoern Coenen.
          string line;
          this->readLine(stream, line);
          if (false == Common::strStartsWithIgnoreCase(line, "saved by"))
          {
            stream.seekg(bomSize, ios::beg);
          }

          this->readLine(stream, line);

          // get column separator
          this->columnSeperator_ = CsvReader::extractColumnSeperator(line);

          // read mandatory data
          this->readMandatoryMetadata(stream);
          
          // read optional user data
          this->readLine(stream, line);
          if (Common::strStartsWithIgnoreCase(line, Constants::XmlOptionalUserData))
          {
            this->readOptionalMetadata(stream);
          }

          // Read line with array names. Has to follow the EndData tag.
          vector<string> arrayList;
          this->readLineAndSplit(stream, arrayList);
          
          // remember position
          this->headerSectionEndOffset_ = stream.tellg();

          if (this->dataFormat_ == IqDataFormat::Real && this->channelCount_ == 1)
          {
            // special case for one channel real data
            // remove empty string
            string temp = arrayList[0];
            arrayList.resize(1);
            arrayList[0] = temp;
          }

          if (arrayList.size() != this->channelCount_ * DataImportExportBase::getValuesPerSample(this->dataFormat_))
          {
            throw DaiException(ErrorCodes::UnsupportedCsvFormat);
          }

          // check that array names read from this line are equal to the array names generated from the channel names
          // while parsing mandatory meta data.
          for (size_t i = 0; i < arrayList.size(); ++i)
          {
            if (false == this->containsArray(arrayList[i]))
            {
              throw DaiException(ErrorCodes::UnsupportedCsvFormat);
            }

            this->arrayNameToColumnIndex_.insert(make_pair(arrayList[i], i));
          }

          stream.close();
          this->initialized_ = true;
        }
        catch (DaiException)
        {
          this->initialized_ = false;
          stream.close();
          
          throw;
        }
      }

      void CsvReader::getArrayNames(std::vector<string>& arrayNames) const
      {
        arrayNames.clear();

        arrayNames.reserve(this->arrayNameToSamples_.size());
        for (auto imap: this->arrayNameToSamples_)
        {
          arrayNames.push_back(imap.first);
        }
      }

      void CsvReader::readMandatoryMetadata(std::fstream& stream)
      {
        string appName = this->readValueForKey(stream, Constants::XmlApplicationName);
        this->updateContent_->updateMetadata(Constants::XmlApplicationName, appName);

        string comment = this->readValueForKey(stream, Constants::XmlComment);
        this->updateContent_->updateMetadata(Constants::XmlComment, comment);
        
        string dateString = this->readValueForKey(stream, Constants::XmlDateTime);
        time_t timestamp = Platform::getTime(dateString);
        this->updateContent_->updateTimestamp(timestamp);

        string formatStr = this->readValueForKey(stream, Constants::XmlFormat);
        this->dataFormat_ = Common::getDataFormatFromString(formatStr);
        this->updateContent_->updateMetadata(Constants::XmlFormat, formatStr);

        string dataTypeStr = this->readValueForKey(stream, Constants::XmlDataType);
        this->dataType_ = Common::getDataTypeFromString(dataTypeStr);
        this->updateContent_->updateMetadata(Constants::XmlDataType, dataTypeStr);

        string nofChannelsStr = this->readValueForKey(stream, Constants::XmlChannels);
        this->channelCount_ = static_cast<size_t>(stoi(nofChannelsStr));
        for (size_t i = 1; i <= this->channelCount_; ++i)
        {
          this->readChannel(stream, i);
        }
      }

      void CsvReader::readOptionalMetadata(std::fstream& stream)
      {
        string line;
        vector<string> columnValues;
        do
        {
          this->readLine(stream, line);

          Common::strSplit(line, this->columnSeperator_, columnValues);
          if (columnValues.size() < 2)
          {
            throw DaiException(ErrorCodes::CsvInvalidNumberOfColumns);
          }

          if (columnValues[0].size() > 0 && columnValues[1].size() > 0)
          {
            this->updateContent_->updateMetadata(columnValues[0], columnValues[1]);
          }
        }
        while (false == Common::strStartsWithIgnoreCase(line, Constants::EndUserData));
      }

      void CsvReader::readChannel(std::fstream& stream, size_t channel)
      {
        string channelBase = "Ch" + to_string(channel) + "_";
        
        string channelName = this->readValueForKey(stream, channelBase + Constants::XmlChannelName);
        
        string samplesStr = this->readValueForKey(stream, channelBase + Constants::XmlSamples);
        size_t samples = stoll(samplesStr.c_str());
        
        string clockStr = this->readValueForKey(stream, channelBase + "Clock[Hz]");
        
        // determine locale
        this->extractAndSetDecimalLocale(clockStr);
        double clock = Common::toDecimal<double>(clockStr.c_str(), this->decimalSeperator_);

        string freqStr = this->readValueForKey(stream, channelBase + "CenterFrequency[Hz]");
        double freq = Common::toDecimal<double>(freqStr.c_str(), this->decimalSeperator_);
        
        this->updateContent_->updateChannels(channelName, clock, freq, samples);

         // store array length of this channel for later usage
        if (this->dataFormat_ == IqDataFormat::Real)
        {
          this->arrayNameToSamples_.insert(make_pair(channelName, samples));
        }
        else
        {
          this->arrayNameToSamples_.insert(make_pair(channelName + "_I", samples));
          this->arrayNameToSamples_.insert(make_pair(channelName + "_Q", samples));
        }
      }

      void CsvReader::readLine(std::fstream& stream, std::string& line)
      {
        getline(stream, line);

        // right trim \r and \n
        line.erase(line.find_last_not_of('\n') + 1);
        line.erase(line.find_last_not_of('\r') + 1);

        // trim whitespace at end
        // will not work with special chars:
        // line.erase(find_if(line.rbegin(), line.rend(), std::not1(ptr_fun<int, int>(isspace))).base(), line.end());
        line.erase(line.find_last_not_of(" \n\r\t")+1);

        if (line.empty())
        {
          throw DaiException(ErrorCodes::CsvUnexpectedEndOfFile);
        }
      }

      void CsvReader::readLineAndSplit(std::fstream& stream, std::vector<std::string>& result)
      {
        string line;
        this->readLine(stream, line);

        Common::strSplit(line, this->columnSeperator_, result);
      }

      std::string CsvReader::readValueForKey(std::fstream& stream, const std::string& key)
      {
        vector<string> columnValues;
        this->readLineAndSplit(stream, columnValues);

        // check for key and value
        if (columnValues.size() < 2)
        {
          throw DaiException(ErrorCodes::CsvInvalidNumberOfColumns);
        }

        if (false == Common::strCmpIgnoreCase(columnValues[0], key))
        {
          throw DaiException(ErrorCodes::CsvKeyNotFound);
        }

        return columnValues[1];
      }

      char CsvReader::extractColumnSeperator(const std::string& line)
      {
        char lastChar = line[line.size() - 1];
        
        if (lastChar == Constants::SeparatorSemiColon)
        {
          return Constants::SeparatorSemiColon;
        }
        else if (lastChar == Constants::SeparatorColon)
        {
          return Constants::SeparatorColon;
        }
        else
        {
#if 0
			throw DaiException(ErrorCodes::CsvInvalidSeparatorChar);
#else
			// normally the algorithm is:
			// count every ; and ,. The one which occurences per each line are equal is the separator.
			// If both met this condition and one is 1 more than the other, then the less wins (1,0;1,0;1,0 => ; is separator)
			// else the max wins.
			// we have only one line, so we adapt the algorithm and ignore the first sentence.
			unsigned int commas = 0;
			unsigned int semicolons = 0;
			bool quoted = false;
			for (size_t i = 0; i < line.size(); i++)
			{
				if (quoted)
				{
					if (line[i] == '"')
					{
						quoted = false;
					}
				}
				else
				{
					switch (line[i])
					{
					case '"':
						quoted = true;
						break;
					case ';':
						semicolons++;
						break;
					case ',':
						commas++;
						break;
					default:
						break;
					}
				}
			}
			if (commas + semicolons == 0)
			{
				return Constants::SeparatorSemiColon;
			}
			if (commas == semicolons + 1)
			{
				return Constants::SeparatorSemiColon;
			}
			if (commas > semicolons)
			{
				return Constants::SeparatorColon;
			}
			else
			{
				return Constants::SeparatorSemiColon;
			}
#endif
		}
      }

      char CsvReader::extractColumnSeperatorForNumericLine(const std::string& line)
      {
        try
        {
          return CsvReader::extractColumnSeperator(line);
        }
        catch (DaiException)
        {
          // do nothing
        }

        vector<string> columnsSemiColon;
        vector<string> columnsColon;
        Common::strSplit(line, Constants::SeparatorSemiColon, columnsSemiColon);
        Common::strSplit(line, Constants::SeparatorColon, columnsColon);

        size_t nofSemiColons = columnsSemiColon.size();
        size_t nofColons = columnsColon.size();

        if ((nofSemiColons > 0 && nofColons == 0) || (nofSemiColons + 1 == nofColons))
        {
          return Constants::SeparatorSemiColon;
        }
        else if ((nofSemiColons == 0 && nofColons > 0) || (nofSemiColons == nofColons + 1))
        {
          return Constants::SeparatorColon;
        }
        else
        {
          throw DaiException(ErrorCodes::CsvInvalidSeparatorChar);
        }
      }

      void CsvReader::extractAndSetDecimalLocale(const std::string& input)
      {
        // locale already set
        if (false == this->extractDecimalSeparator_)
        {
          return;
        }

        bool localeSet = false;
        if (this->columnSeperator_ == Constants::SeparatorSemiColon)
        {
          if (input.find(Constants::SeparatorColon) != string::npos)
          {
            this->decimalSeperator_ = Constants::SeparatorColon;
            localeSet = true;
          }
          else if (input.find(Constants::SeparatorDot) != string::npos)
          {
            this->decimalSeperator_ = Constants::SeparatorDot;
            localeSet = true;
          }
        }                                                                         // patch tm, was colon  
        else if (this->columnSeperator_ == Constants::SeparatorColon && input.find(Constants::SeparatorDot) != string::npos)
        {
          this->decimalSeperator_ = Constants::SeparatorDot;
          localeSet = true;
        }

        if (false == localeSet)
        {
          // patch tm: wrong freq / clock else, when number without delimiter
          return;
          //throw DaiException(ErrorCodes::CsvInvalidNumberFormat);
        }

        this->extractDecimalSeparator_ = false;
      }

      bool CsvReader::containsArray(const std::string& arrayName) const
      {
        return this->arrayNameToSamples_.count(arrayName) == 0 ? false : true;
      }

      long CsvReader::getNofSamples(const std::string& arrayName) const
      {
        if (this->arrayNameToSamples_.count(arrayName) == 1)
        {
          return this->arrayNameToSamples_.at(arrayName);
        }

        return -1;
      }

      /** ICsvSelector start */
      int64_t CsvReader::getNofCols()
      {
        if (this->filename_.empty())
        {
          throw DaiException(ErrorCodes::FileNotFound);
        }

        fstream stream;
        try
        {
          // open file
          CsvReader::openStreamIgnoreBom(this->filename_, stream);

          // read first line
          string line;
          this->readLine(stream, line);

          // get column separator and decimal separator
          this->columnSeperator_ = CsvReader::extractColumnSeperatorForNumericLine(line);
          CsvReader::extractAndSetDecimalLocale(line);

          // if no decimal separator was extracted, the first line does not contain any number
          if (this->extractDecimalSeparator_)
          {
            stream.close();
            return -1;
          }

          // split line
          vector<string> columnValues;
          Common::strSplit(line, this->columnSeperator_, columnValues);

          // error, no columns found;
          if (columnValues.size() == 0)
          {
            stream.close();
            return -1;
          }

          stream.close();
          return static_cast<int64_t>(columnValues.size());
        }
        catch (DaiException)
        {
          stream.close();
          return -1;
        }
      }

      int64_t CsvReader::getNofRows(size_t column)
      {
        if (this->filename_.empty())
        {
          throw DaiException(ErrorCodes::FileNotFound);
        }

        int64_t lineCount = 0;
        fstream stream;
        try
        {
          // open file
          CsvReader::openStreamIgnoreBom(this->filename_, stream);

          string line;
          getline(stream, line);

          // get column separator and decimal separator
          this->columnSeperator_ = CsvReader::extractColumnSeperatorForNumericLine(line);
          CsvReader::extractAndSetDecimalLocale(line);

          // if no decimal separator was extracted, the first line does not contain any number
          if (this->extractDecimalSeparator_)
          {
            stream.close();
            return -1;
          }

          vector<string> columnValues;
          do
          {
            Common::strSplit(line, this->columnSeperator_, columnValues);
            
            // current rows does not contain column -> done
            if (columnValues.size() <= column)
            {
              stream.close();
              return -1;
            }

            float value = numeric_limits<float>::quiet_NaN();
            try
            {
   			  value = Common::toDecimal<float>(columnValues[column].c_str(), this->decimalSeperator_);
            }
            catch (...)
            {
              // do nothing
            }

            if (isnan(value))
            {
              stream.close();
              return lineCount;
            }

            // valid value found, proceed...
            lineCount++;
          }
          while (getline(stream, line));

          stream.close();

          // remember value and return it.
          return lineCount;
        }
        catch (DaiException)
        {
          stream.close();
          return -1;
        }
      }
      /** ICsvSelector end */
    }
  }
}
