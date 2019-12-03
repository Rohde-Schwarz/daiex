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

#include "iqcsvpimpl.h"

#include <limits>

#include "common.h"
#include "constants.h"

using namespace std;

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      IqCsv::Impl::Impl(const std::string& filename) : DataImportExportBase(filename),
        reader_(nullptr),
        writer_(nullptr),
        valueSeparator_(Constants::SeparatorSemiColon),
        numberDecimalSeparator_(Constants::SeparatorColon),
        formatSpecifier_("7E"),
        formatSpecifierChannelInfo_("7E"),
        lockDataType_(false),
        dataFormat_(IqDataFormat::Complex),
        dataType_(IqDataType::Float32)
      {
      }

      IqCsv::Impl::~Impl()
      {
        this->close();
      }

      int IqCsv::Impl::readOpen(std::vector<std::string>& arrayNames)
      {
        int ret = DataImportExportBase::readOpen(arrayNames);
        if (ret != ErrorCodes::Success)
        {
          return ret;
        }

        if (this->reader_ != nullptr)
        {
          return ErrorCodes::ReaderAlreadyInitialized;
        }

        if (this->writer_ != nullptr)
        {
          return ErrorCodes::WriterAlreadyInitialized;
        }

        if (this->filename_.empty())
        {
          return ErrorCodes::FileNotFound;
        }

        // open reader and analyze content
        try
        {
          this->reader_ = new CsvReader(this->filename_, *this);
          this->reader_->analyzeContent();

          if (this->getChannelCount() == 0)
          {
            return ErrorCodes::EmptyChannelInfo;
          }

          // get array names
          this->reader_->getArrayNames(arrayNames);
        }
        catch (DaiException &e)
        {
          this->close();
          return ErrorCodes::UnsupportedCsvFormat;
        }

        return ErrorCodes::Success;
      }

      int IqCsv::Impl::writeOpen(
        IqDataFormat format,
        size_t nofArrays,
        const std::string& applicationName,
        const std::string& comment,
        const std::vector<ChannelInfo>& channelInfos,
        const std::map<std::string, std::string>* metadata)
      {
        if (this->writer_ != nullptr)
        {
          return ErrorCodes::WriterAlreadyInitialized;
        }

        int ret = DataImportExportBase::writeOpen(format, nofArrays, applicationName, comment, channelInfos, metadata);
        if (ErrorCodes::Success != ret)
        {
          return ret;
        }

        if (this->reader_ != nullptr)
        {
          return ErrorCodes::ReaderAlreadyInitialized;
        }

        if (this->writer_ != nullptr)
        {
          return ErrorCodes::WriterAlreadyInitialized;
        }

        bool valid = IqCsv::Impl::validateFormatSpecifier(this->formatSpecifier_);
        if (!valid)
        {
          return ErrorCodes::InvalidFormatSpecifier;
        }

        valid = IqCsv::Impl::validateFormatSpecifier(this->formatSpecifierChannelInfo_);
        if (!valid)
        {
          return ErrorCodes::InvalidFormatSpecifier;
        }

        this->nofArrays_ = nofArrays;
        this->dataFormat_ = format;
        this->applicationName_ = applicationName;
        this->comment_ = comment;

        if (metadata != nullptr)
        {
          this->metadata_ = *metadata;
        }

        // verify that channel names are unique
        if (false == DataImportExportBase::verifyUniqueChannelNames(channelInfos))
        {
          return ErrorCodes::InconsistentInputData;
        }

        // complete channel names, if no channel name was set
        IqCsv::Impl::completeChannelInfoNames(channelInfos, this->channelInfos_);

        if (IqCsv::Impl::metadataContainsValueSeparator(this->valueSeparator_, applicationName, comment, metadata))
        {
          return ErrorCodes::StringValuesContainValueSeparator;
        }

        // init sample counter
        this->nofSamplesWrittenPerChannel_.resize(this->channelInfos_.size());
        fill(this->nofSamplesWrittenPerChannel_.begin(), this->nofSamplesWrittenPerChannel_.end(), 0);

        // Minimum number of columns in the csv file is 2 as all meta data is represented as a key - value pair
        // nofArrays could be 1 in case of single channel real data!
        int columnCount = max(2, static_cast<int>(nofArrays));

        // Meta data fills the first 2 columns. If more columns are needed, create
        // string that represents the missing empty columns.
        string emptyColsMetaData = string(columnCount - 2, this->valueSeparator_);

        try
        {
          this->writer_ = new CsvWriter(
            this->filename_,
            this->valueSeparator_,
            emptyColsMetaData);

          this->writer_->open();
          this->writeMetadata(false);
        }
        catch (DaiException &e)
        {
          this->close();
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int IqCsv::Impl::close()
      {
        try
        {
          if (this->reader_ != nullptr)
          {
            delete this->reader_;
            this->reader_ = nullptr;
          }

          if (this->writer_ != nullptr)
          {
            // write meta data
            this->finalizeTemporarySequence();

            this->writer_->close();
            delete this->writer_;
            this->writer_ = nullptr;
          }
        }
        catch (DaiException &e)
        {
          return e.code();
        }
        catch (...)
        {
          return ErrorCodes::InternalError;
        }

        return ErrorCodes::Success;
      }

      void IqCsv::Impl::finalizeTemporarySequence()
      {
        this->writer_->flush();
        this->writer_->beg();
        this->writeMetadata(true);
      }

      bool IqCsv::Impl::validateFormatSpecifier(std::string& formatSpecifier)
      {
        if (formatSpecifier.size() != 2)
        {
          return false;
        }

        // to lower case
        transform(formatSpecifier.begin(), formatSpecifier.end(), formatSpecifier.begin(), ::tolower);

        string digit = formatSpecifier.substr(0, 1);
        if (find_if(digit.begin(), digit.end(), ::isdigit) == digit.end())
        {
          return false;
        }

        string spec = formatSpecifier.substr(1, 2);
        if (Common::strCmpIgnoreCase(spec, "G") ||
          Common::strCmpIgnoreCase(spec, "E") ||
          Common::strCmpIgnoreCase(spec, "G"))
        {
          return true;
        }

        return false;
      }

      void IqCsv::Impl::completeChannelInfoNames(const std::vector<ChannelInfo>& in, std::vector<ChannelInfo>& out)
      {
        out.clear();
        out.reserve(in.size());
        
        int channelCount = 0;
        for (auto info: in)
        {
          string name = info.getChannelName();
          
          // will not work with special chars:
          // remove_if(name.begin(), name.end(), isspace);
          name.erase(remove(name.begin(), name.end(), ' '), name.end());

          if (name.empty())
          {
            name = ChannelInfo::getDefaultChannelName(channelCount);
            out.push_back(ChannelInfo(name, info.getClockRate(), info.getFrequency(), info.getSamples()));
          }
          else
          {
            out.push_back(info);
          }

          channelCount++;
        }
      }

      bool IqCsv::Impl::metadataContainsValueSeparator(const char separator, const std::string& applicationName, const std::string& comment, const std::map<std::string, std::string>* metadata)
      {
        if (applicationName.find(separator) != string::npos)
        {
          return true;
        }

        if (comment.find(separator) != string::npos)
        {
          return true;
        }

        if (metadata != nullptr)
        {
          for (auto pair : *metadata)
          {
            if (pair.first.find(separator) != string::npos)
            {
              return true;
            }

            if (pair.second.find(separator) != string::npos)
            {
              return true;
            }
          }
        }

        return false;
      }

      void IqCsv::Impl::writeMetadata(bool finalize)
      {
        // Minimum number of columns in the csv file is 2 as all meta data is represented as a key - value pair
        // nofArrays could be 1 in case of single channel real data!
        int columnCount = max(2, static_cast<int>(this->nofArrays_));

        // "written by" headline only fills first column. If more columns are needed, create
        // string that represents the missing empty columns.
        string headlinePostfix = string(columnCount - 1, this->valueSeparator_);
        
        stringstream ss;

        // add creation identifier
        ss << Constants::CreationIdentifier << " (version " << Common::getVersion() << ")" << headlinePostfix;
        this->writer_->writeLine(ss.str());
        
        // create entries for mandatory data
        ss.str("");
        ss.clear();
        ss << Constants::XmlMandatoryUserData << headlinePostfix;
        this->writer_->writeLine(ss.str());

        // application name
        if (this->applicationName_.empty())
        {
          this->writer_->writeMetadataLine(Constants::XmlApplicationName, "created by MOSAIK LibDataImportExport");
        }
        else
        {
          this->writer_->writeMetadataLine(Constants::XmlApplicationName, this->applicationName_);
        }

        // comment
        this->writer_->writeMetadataLine(Constants::XmlComment, this->comment_);

        // if no timestamp was set, use current date
        time_t writeTime = (this->timestamp_ == 0) ? time(0) : this->timestamp_;
        this->writer_->writeMetadataLine(Constants::XmlDateTime, Common::toString(writeTime));

        // data format
        this->writer_->writeMetadataLine(Constants::XmlFormat, IqDataFormatNames[this->dataFormat_]);
        
        // data type
        if (finalize)
        {
          this->writer_->writeMetadataLine(Constants::XmlDataType, IqDataTypeNames[this->dataType_]);
        }
        else
        {
          this->writer_->writeMetadataLine(Constants::XmlDataType, "");
        }
                
        // This entry must be the last line before ChannelInfos start, as it is used as
        // identifier when reading this type of file, indicating the start of the ChannelInfos.
        this->writer_->writeMetadataLine(Constants::XmlChannels, to_string(this->channelInfos_.size()));

        // channels
        int channelNumber = 0;
        stringstream arrayNamesLine;
        for (auto channel: this->channelInfos_)
        {
          this->writer_->writeChannelInfoLine(channelNumber + 1, Constants::XmlChannelName, channel.getChannelName());
          this->writer_->writeChannelInfoLine(channelNumber + 1, Constants::XmlSamples, to_string(this->nofSamplesWrittenPerChannel_[channelNumber]));
          this->writer_->writeChannelInfoLine(channelNumber + 1, Constants::XmlClock + "[Hz]", IqCsv::Impl::getFormattedValue(channel.getClockRate(), this->formatSpecifierChannelInfo_, this->numberDecimalSeparator_));
          this->writer_->writeChannelInfoLine(channelNumber + 1, Constants::XmlCenterFrequency + "[Hz]", IqCsv::Impl::getFormattedValue(channel.getFrequency(), this->formatSpecifierChannelInfo_, this->numberDecimalSeparator_));

          if (arrayNamesLine.str().size() > 0)
          {
            arrayNamesLine << this->valueSeparator_;
          }

          if (this->dataFormat_ == IqDataFormat::Real)
          {
            arrayNamesLine << channel.getChannelName();

            // Handle special case, where file has 2 columns due to meta data but only one
            // value array ... empty column is added in order to fulfill the following requirement
            // "Each record "should" contain the same number of comma-separated fields."
            if (this->nofArrays_ < static_cast<size_t>(columnCount))
            {
              arrayNamesLine << this->valueSeparator_;
            }
          }
          else
          {
            arrayNamesLine << this->channelInfos_[channelNumber].getChannelName() + "_I";
            arrayNamesLine << this->valueSeparator_;
            arrayNamesLine << this->channelInfos_[channelNumber].getChannelName() + "_Q";
          }

          channelNumber++;
        }

        // Create entries for optional user data
        if (this->metadata_.size() > 0)
        {
          ss.str("");
          ss.clear();
          ss << Constants::XmlOptionalUserData << headlinePostfix;
          this->writer_->writeLine(ss.str());

          for (auto pair: this->metadata_)
          {
            this->writer_->writeMetadataLine(pair.first, pair.second);
          }
        }

        // Add "end of meta data" identifier + placeholder
        ss.str("");
        ss.clear();
        ss << Constants::EndUserData << headlinePostfix;

        this->writer_->writeLine(ss.str());

        // if this is not the final meta data, data type and nof samples
        // are still unknown. We need to reserve space...
        string placeholder;
        if (!finalize)
        {
          // max chars used by data type field
          size_t maxSizeDataType = 0;
          for (size_t i = 0; i < IqDataType::IQDATATYPE_ENUM_SIZE; ++i)
          {
            maxSizeDataType = max(maxSizeDataType, string(IqDataTypeNames[i]).size());
          }

          // max chars used by nofSamples field * nofChannels
          size_t maxSize = this->channelInfos_.size() * to_string(numeric_limits<size_t>::max()).size();

          placeholder = string(maxSizeDataType + maxSize, ' ');
        }

        arrayNamesLine << placeholder;

        if (finalize)
        {
          this->writer_->writeWithoutNewLine(arrayNamesLine.str());
        }
        else 
        {
          this->writer_->writeLine(arrayNamesLine.str());
        }
      }

      int IqCsv::Impl::setSeparatorConfiguration(const CsvSeparatorConfig config)
      {
        if (this->writer_ != nullptr)
        {
          return ErrorCodes::WriterAlreadyInitialized;
        }

        switch (config)
        {
          default:
          case CsvSeparatorConfig::DecimalColonValueSemicolon:
            this->numberDecimalSeparator_ = Constants::SeparatorColon;
            this->valueSeparator_ = Constants::SeparatorSemiColon;
            break;
          case CsvSeparatorConfig::DecimalDotValueColon:
            this->numberDecimalSeparator_ = Constants::SeparatorDot;
            this->valueSeparator_ = Constants::SeparatorColon;
            break;
          case CsvSeparatorConfig::DecimalDotValueSemicolon:
            this->numberDecimalSeparator_ = Constants::SeparatorDot;
            this->valueSeparator_ = Constants::SeparatorSemiColon;
            break;
        }

        return ErrorCodes::Success;
      }

      CsvSeparatorConfig IqCsv::Impl::getSeparatorConfiguration() const
      {
        if (this->numberDecimalSeparator_ == Constants::SeparatorColon &&
          this->valueSeparator_ == Constants::SeparatorSemiColon)
        {
          return CsvSeparatorConfig::DecimalColonValueSemicolon;
        }
        else if (this->numberDecimalSeparator_ == Constants::SeparatorDot &&
          this->valueSeparator_ == Constants::SeparatorColon)
        {
          return CsvSeparatorConfig::DecimalDotValueColon;
        }
        else
        {
          return CsvSeparatorConfig::DecimalDotValueColon;
        }
      }

      char IqCsv::Impl::getValueSeparator() const
      {
        return this->valueSeparator_;
      }

      char IqCsv::Impl::getDecimalSeparator() const
      {
        return this->numberDecimalSeparator_;
      }

      int IqCsv::Impl::setFormatSpecifier(const std::string& format)
      {
        if (this->writer_ != nullptr)
        {
          return ErrorCodes::WriterAlreadyInitialized;
        }

        this->formatSpecifier_ = format;
        return ErrorCodes::Success;
      }

      const std::string& IqCsv::Impl::getFormatSpecifier() const
      {
        return this->formatSpecifier_;
      }

      int IqCsv::Impl::setFormatSpecifierChannelInfo(const std::string& format)
      {
        if (this->writer_ != nullptr)
        {
          return ErrorCodes::WriterAlreadyInitialized;
        }

        this->formatSpecifierChannelInfo_ = format;
        return ErrorCodes::Success;
      }

      const std::string& IqCsv::Impl::getFormatSpecifierChannelInfo() const
      {
        return this->formatSpecifierChannelInfo_;
      }

      int64_t IqCsv::Impl::getArraySize(const std::string& arrayName) const
      {
        if (this->reader_ == nullptr)
        {
          return -1;
        }

        return this->reader_->getNofSamples(arrayName);
      }

      int IqCsv::Impl::readArray(const std::string& arrayName, std::vector<float>& values, size_t nofValues, size_t offset)
      {
        values.resize(nofValues);
        return this->readArray(arrayName, values.data(), nofValues, offset);
      }

      int IqCsv::Impl::readArray(const std::string& arrayName, float* values, size_t nofValues, size_t offset)
      {
        if (this->reader_ == nullptr)
        {
          return ErrorCodes::OpenFileHasNotBeenCalled;
        }

        try
        {
          this->reader_->readArray(arrayName, values, nofValues, offset);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int IqCsv::Impl::readArray(const std::string& arrayName, std::vector<double>& values, size_t nofValues, size_t offset)
      {
        values.resize(nofValues);
        return this->readArray(arrayName, values.data(), nofValues, offset);
      }

      int IqCsv::Impl::readArray(const std::string& arrayName, double* values, size_t nofValues, size_t offset)
      {
        if (this->reader_ == nullptr)
        {
          return ErrorCodes::OpenFileHasNotBeenCalled;
        }

        try
        {
          this->reader_->readArray(arrayName, values, nofValues, offset);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int IqCsv::Impl::readChannel(const std::string& channelName, std::vector<float>& values, size_t nofValues, size_t offset)
      {
        values.resize(nofValues);
        return this->readChannel(channelName, values.data(), nofValues, offset);
      }

      int IqCsv::Impl::readChannel(const std::string& channelName, float* values, size_t nofValues, size_t offset)
      {
        if (this->reader_ == nullptr)
        {
          return ErrorCodes::OpenFileHasNotBeenCalled;
        }

        try
        {
          this->reader_->readChannel(channelName, values, nofValues, offset);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int IqCsv::Impl::readChannel(const std::string& channelName, std::vector<double>& values, size_t nofValues, size_t offset)
      {
        values.resize(nofValues);
        return this->readChannel(channelName, values.data(), nofValues, offset);
      }

      int IqCsv::Impl::readChannel(const std::string& channelName, double* values, size_t nofValues, size_t offset)
      {
        if (this->reader_ == nullptr)
        {
          return ErrorCodes::OpenFileHasNotBeenCalled;
        }

        try
        {
          this->reader_->readChannel(channelName, values, nofValues, offset);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int IqCsv::Impl::appendArrays(const std::vector<std::vector<float>>& iqdata)
      {
        vector<float*> dataPtr;
        vector<size_t> sizes;
        Common::getVectorOfPtrAndSize(iqdata, dataPtr, sizes);

        return this->appendArrays(dataPtr, sizes);
      }

      int IqCsv::Impl::appendArrays(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
      {
        if (this->writer_ == nullptr)
        {
          return ErrorCodes::FileWriterUninitialized;
        }

        try
        {
          this->writeArray(iqdata, sizes);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int IqCsv::Impl::appendArrays(const std::vector<std::vector<double>>& iqdata)
      {
        vector<double*> dataPtr;
        vector<size_t> sizes;
        Common::getVectorOfPtrAndSize(iqdata, dataPtr, sizes);

        return this->appendArrays(dataPtr, sizes);
      }

      int IqCsv::Impl::appendArrays(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
      {
        if (this->writer_ == nullptr)
        {
          return ErrorCodes::FileWriterUninitialized;
        }

        try
        {
          this->writeArray(iqdata, sizes);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int IqCsv::Impl::appendChannels(const std::vector<std::vector<float>>& iqdata)
      {
        vector<float*> dataPtr;
        vector<size_t> sizes;
        Common::getVectorOfPtrAndSize(iqdata, dataPtr, sizes);

        return this->appendChannels(dataPtr, sizes);
      }

      int IqCsv::Impl::appendChannels(const std::vector<float*>& iqdata, const std::vector<size_t>& sizes)
      {
        if (this->writer_ == nullptr)
        {
          return ErrorCodes::FileWriterUninitialized;
        }

        try
        {
          this->writeChannel(iqdata, sizes);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int IqCsv::Impl::appendChannels(const std::vector<std::vector<double>>& iqdata)
      {
        vector<double*> dataPtr;
        vector<size_t> sizes;
        Common::getVectorOfPtrAndSize(iqdata, dataPtr, sizes);

        return this->appendChannels(dataPtr, sizes);
      }

      int IqCsv::Impl::appendChannels(const std::vector<double*>& iqdata, const std::vector<size_t>& sizes)
      {
        if (this->writer_ == nullptr)
        {
          return ErrorCodes::FileWriterUninitialized;
        }

        try
        {
          this->writeChannel(iqdata, sizes);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      /** IAnalyzeContent start */
      void IqCsv::Impl::updateChannels(const std::string& channelName, double clock, double frequency, size_t samples)
      {
        this->addChannel(channelName, clock, frequency, samples);
      }

      void IqCsv::Impl::updateMetadata(const std::string& key, const std::string& value)
      {
        this->addMetadata(key, value);
      }

      void IqCsv::Impl::updateTimestamp(const time_t timestamp)
      {
        this->setTimestamp(timestamp);
      }
      /** IAnalyzeContent end */

      /** ICsvSelector start */
      int64_t IqCsv::Impl::getNofRows(size_t column)
      {
        if (this->filename_.empty())
        {
          return ErrorCodes::FileNotFound;
        }

        int64_t tmp = -1;
        try
        {
          CsvReader reader(this->filename_, *this);
          tmp = reader.getNofRows(column);
        }
        catch (...)
        {
        }

        return tmp;
      }

      int64_t IqCsv::Impl::getNofCols()
      {
        if (this->filename_.empty())
        {
          return ErrorCodes::FileNotFound;
        }

        int64_t tmp = -1;
        try
        {
          CsvReader reader(this->filename_, *this);
          tmp = reader.getNofCols();
        }
        catch (...)
        {
        }

        return tmp;
      }

      int IqCsv::Impl::readRawArray(size_t column, size_t nofValues, std::vector<float>& values, size_t offset)
      {
        values.resize(nofValues);
        return this->readRawArray(column, nofValues, values.data(), offset);
      }

      int IqCsv::Impl::readRawArray(size_t column, size_t nofValues, float* values, size_t offset)
      {
        if (this->filename_.empty())
        {
          return ErrorCodes::FileNotFound;
        }

        try
        {
          CsvReader reader(this->filename_, *this);
          reader.readRawArray(column, nofValues, values, offset);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }

      int IqCsv::Impl::readRawArray(size_t column, size_t nofValues, std::vector<double>& values, size_t offset)
      {
        values.resize(nofValues);
        return this->readRawArray(column, nofValues, values.data(), offset);
      }

      int IqCsv::Impl::readRawArray(size_t column, size_t nofValues, double* values, size_t offset)
      {
        if (this->filename_.empty())
        {
          return ErrorCodes::FileNotFound;
        }

        try
        {
          CsvReader reader(this->filename_, *this);
          reader.readRawArray(column, nofValues, values, offset);
        }
        catch (DaiException &e)
        {
          return e.code();
        }

        return ErrorCodes::Success;
      }
      /** ICsvSelector end */
    }
  }
}