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

#include "iqtar_reader.h"
#include "iqtarpimpl.h"

#include <algorithm>

#include "constants.h"
#include "channelinfo.h"
#include "dataimportexportbase.h"

using namespace std;
using namespace memory_mapped_file;
using namespace pugi;

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      IqTarReader::IqTarReader(const std::string& filename, IAnalyzeContentIqTar& updateContent) :
        updateContent_(&updateContent),
        initialized_(false),
        filename_(filename),
        archive_(nullptr),
        dataType_(IqDataType::Float32),
        dataFormat_(IqDataFormat::Complex),
        scalingFactor_(numeric_limits<double>::quiet_NaN()),
        nofSamples_(0)
      {
      }

      IqTarReader::~IqTarReader()
      {
        this->close();
      }

      bool IqTarReader::isInitialized() const
      {
        return this->initialized_;
      }

      void IqTarReader::analyzeContent()
      {
        if (this->initialized_)
        {
          return;
        }

        // read tar header names
        std::vector<string> tarElements;
        this->readArchiveContent(tarElements);
        if (tarElements.size() == 0)
        {
          throw DaiException(ErrorCodes::InvalidTarArchive);
        }

        // load xml content
        this->loadXmlContentFromIqTar(tarElements);

        this->initialized_ = true;
      }

      void IqTarReader::getArrayNames(std::vector<string>& arrayNames) const
      {
        arrayNames.clear();
        
        arrayNames.reserve(this->arrayNameToChannelNo_.size());
        for (auto imap: this->arrayNameToChannelNo_)
        {
          arrayNames.push_back(imap.first);
        }
      }

      void IqTarReader::open()
      {
        this->close();

        this->archive_ = archive_read_new();
        if (this->archive_ == nullptr)
        {
          throw DaiException(ErrorCodes::InternalError);
        }

        // set up libarchive
        Common::archiveAssert(archive_read_support_filter_none(this->archive_));
        Common::archiveAssert(archive_read_support_format_gnutar(this->archive_));
        Common::archiveAssert(archive_read_support_format_tar(this->archive_));

        // archive read with callbacks
        Platform::mmfOpen(this->mmfReader_.mmf, this->filename_, false);
        
        Common::archiveAssert(archive_read_set_skip_callback(this->archive_, IqTarReader::archiveReadSkipCallback));
        Common::archiveAssert(archive_read_open(this->archive_, &this->mmfReader_, nullptr, IqTarReader::archiveReadCallback, IqTarReader::archiveCloseCallback));
      }

      size_t IqTarReader::getNofChannels() const
      {
        if (DataImportExportBase::getValuesPerSample(this->dataFormat_) == 1)
        {
          return this->arrayNameToChannelNo_.size();
        }
        else
        {
          return this->arrayNameToChannelNo_.size() / 2;
        }
      }

      void IqTarReader::readPrepare(const std::string& arrayName, size_t nofReadValues, size_t offset, size_t& readOffset, size_t& ignoreNofChannels)
      {
        if (false == this->containsArray(arrayName))
        {
          throw DaiException(ErrorCodes::InvalidArrayName);
        }

        // get total number of channels and ensure current channel is valid
        int channelNo = this->arrayNameToChannelNo_[arrayName];
        if (static_cast<size_t>(channelNo) > this->getNofChannels())
        {
          throw DaiException(ErrorCodes::InternalError);
        }

        this->open();

        // get tar element containing I/Q data
        struct archive_entry* element = IqTarReader::findTarElement(this->archive_, this->iqDataFilename_);
        if (archive_entry_size(element) <= 0)
        {
          throw DaiException(ErrorCodes::InvalidTarArchive);
        }

        // calculate size of I/Q data arrays contained in stream
        size_t valuesPerSample = DataImportExportBase::getValuesPerSample(this->dataFormat_);
        int wordWidth = DataImportExportBase::getWordWidth(this->dataType_);
        int nofChannels = this->getNofChannels();
        __LA_INT64_T elementSize = archive_entry_size(element);
        size_t offsetData = static_cast<size_t>(archive_filter_bytes(this->archive_, 0));

        this->close();

        size_t arraySize = static_cast<size_t>(elementSize / (valuesPerSample * wordWidth * nofChannels));
        if (arraySize == 0)
        {
          throw DaiException(ErrorCodes::NoDataFoundInFile);
        }

        size_t iqPairs = arraySize - offset;
        if (iqPairs <= 0)
        {
          throw DaiException(ErrorCodes::StartIndexOutOfRange);
        }

        if (nofReadValues > iqPairs)
        {
          throw DaiException(ErrorCodes::InvalidDataInterval);
        }
        else
        {
          iqPairs = nofReadValues;
        }

        if (iqPairs <= 0)
        {
          throw DaiException(ErrorCodes::InvalidDataInterval);
        }

        readOffset = 0;
        ignoreNofChannels = valuesPerSample * (nofChannels - 1);
        readOffset += offsetData;
        readOffset += channelNo * valuesPerSample * wordWidth;
        readOffset += offset * nofChannels * valuesPerSample * wordWidth;
      }

      void IqTarReader::readArchiveContent(std::vector<std::string>& tarElementNames)
      {
        this->open();

        tarElementNames.clear();

        struct archive_entry* element;

		    // TODO remove lock with libarchive 4.0 -> setlocale should then not be used anymore
        Common::localeLock_.lock();
        while (ARCHIVE_OK == archive_read_next_header(this->archive_, &element))
        {
          string elementName(archive_entry_pathname(element));
          if (elementName.empty())
          {
            Common::localeLock_.unlock();
            throw DaiException(ErrorCodes::InvalidTarArchive);
          }

          tarElementNames.push_back(elementName);
        }
        
        Common::localeLock_.unlock();

        this->close();
      }

      archive_entry* IqTarReader::findTarElement(archive* archive, const std::string& tarElementName)
      {
        struct archive_entry* element;

		    // TODO remove lock with libarchive 4.0 -> setlocale should then not be used anymore
        Common::localeLock_.lock();
        while (ARCHIVE_OK == archive_read_next_header(archive, &element))
        {
          string currentElementName(archive_entry_pathname(element));
          if (0 == currentElementName.compare(tarElementName))
          {
            Common::localeLock_.unlock();
            return element;
          }
        }

        Common::localeLock_.unlock();

        throw DaiException(ErrorCodes::InvalidTarArchive);
      }

      void IqTarReader::readElementContent(const std::string& tarElementName, std::vector<char>& data)
      {
        this->open();

        struct archive_entry* element = IqTarReader::findTarElement(this->archive_, tarElementName);
        if (archive_entry_size(element) <= 0)
        {
          throw DaiException(ErrorCodes::InvalidTarArchive);
        }

        size_t entrySize = static_cast<size_t>(archive_entry_size(element));
        data = vector<char>(entrySize);
        archive_read_data(this->archive_, static_cast<void*>(&data[0]), entrySize);

        this->close();
      }

      void IqTarReader::loadXmlContentFromIqTar(const std::vector<std::string>& tarElementNames)
      {
        vector<char> data;
        bool xmlDocFound = false;
        const string xmlEnd = ".xml";

        for (size_t i = 0; i < tarElementNames.size(); ++i)
        {
          if (Common::strEndsWithIgnoreCase(tarElementNames[i], xmlEnd))
          {
            // extract tar content
            this->readElementContent(tarElementNames[i], data);

            xmlDocFound = true;
            break;
          }
        }

        if (false == xmlDocFound)
        {
          throw DaiException(ErrorCodes::InvalidFormatNoXmlFoundInTar);
        }

        // read metadata from xml
        this->parseXmlMetadata(data);
      }

      void IqTarReader::parseXmlMetadata(const std::vector<char>& data)
      {
        // make a safe-to-modify copy of input_xml
        string xmlString (data.data(), data.size());

        try
        {
          xml_document doc;
          xml_parse_result res = doc.load_buffer_inplace(const_cast<char*>(xmlString.c_str()), xmlString.size(), parse_default | parse_comments);
          if (res.status != status_ok)
          {
            throw DaiException(ErrorCodes::InvalidXmlString);
          }

          xml_node root = doc.child(Constants::XmlRootNode.c_str());
          if (root.empty())
          {
            throw DaiException(ErrorCodes::InvalidFormatOfIQTarXmlContent);
          }

          // I/Q data filename
          xml_node filenameNode = root.child(Constants::XmlDataFileName.c_str());
          string filename = filenameNode.child_value();
          if (filename.empty())
          {
            throw DaiException(ErrorCodes::InvalidFormatOfIQTarXmlContent);
          }

          this->iqDataFilename_.assign(filename.begin(), filename.end());

          // application name
          xml_node applicationNameNode = root.child(Constants::XmlApplicationName.c_str());
          string applicationName = applicationNameNode.child_value();
          if (false == applicationName.empty())
          {
            Common::escapeXmlToStr(applicationName);
            this->updateContent_->updateMetadata(Constants::XmlApplicationName, applicationName);
          }

          // comment
          xml_node commentNode = root.child(Constants::XmlComment.c_str());
          string comment = commentNode.child_value();
          if (false == comment.empty())
          {
            Common::escapeXmlToStr(comment);
            this->updateContent_->updateMetadata(Constants::XmlComment, comment);
          }

          // date
          xml_node dateNode = root.child(Constants::XmlDateTime.c_str());
          string dateStr = dateNode.child_value();
          if (false == dateStr.empty())
          {
            time_t timestamp = Platform::getTime(dateStr);
            this->updateContent_->updateTimestamp(timestamp);
          }

          // format
          xml_node formatNode = root.child(Constants::XmlFormat.c_str());
          string format = formatNode.child_value();
          if (format.empty())
          {
            throw DaiException(ErrorCodes::InvalidFormatOfIQTarXmlContent);
          }

          this->dataFormat_ = Common::getDataFormatFromString(format);
          this->updateContent_->updateMetadata(Constants::XmlFormat, format);

          // scaling
          xml_node scalingNode = root.child(Constants::XmlScalingFactor.c_str());
          string scale = scalingNode.child_value();
          if (scale.empty())
          {
            throw DaiException(ErrorCodes::InvalidFormatOfIQTarXmlContent);
          }

          this->updateContent_->updateMetadata(Constants::XmlScalingFactor, scale);
          
          // Pruefen, ob genau 1 oder 1.0000 ( lauter Nullen )
          // -> dann kein Scaling -> Factor auf Double.NaN setzen!
          if (0 == scale.compare("1"))
          {
            this->scalingFactor_ = numeric_limits<double>::quiet_NaN();
          }
          else
          {
            size_t matchCount = count(scale.begin(), scale.end(), '0');
            if (matchCount == scale.size() - 2)
            {
              this->scalingFactor_ = numeric_limits<double>::quiet_NaN();
            }
            else
            {
              this->scalingFactor_ = stod(scale);
            }
          }

          // data type
          xml_node dataTypeNode = root.child(Constants::XmlDataType.c_str());
          string dataType = dataTypeNode.child_value();
          if (dataType.empty())
          {
            throw DaiException(ErrorCodes::InvalidFormatOfIQTarXmlContent);
          }

          this->dataType_ = Common::getDataTypeFromString(dataType);
          this->updateContent_->updateMetadata(Constants::XmlDataType, dataType);

          // get channel information
          this->createChannelInformation(doc);

          // get optional data
          this->readOptionalMetadata(doc);

          // get (optional) deprecated info
          this->readDeprecatedData(doc);
        }
        catch (...)
        {
          // invalidate data
          this->iqDataFilename_.clear();
          this->scalingFactor_ = numeric_limits<double>::quiet_NaN();
          this->dataType_ = IqDataType::Float32;
          this->dataFormat_ = IqDataFormat::Complex;
          this->nofSamples_ = 0;

          throw DaiException(ErrorCodes::InvalidFormatOfIQTarXmlContent);
        }
      }

      void IqTarReader::createChannelInformation(pugi::xml_document& doc)
      {
        xml_node root = doc.child(Constants::XmlRootNode.c_str());

        // number of channels is not mandatory. If no NofChannels tag is found
        // in xml, 1 channel is assumed.
        xml_node channelNode = root.child(Constants::XmlChannels.c_str());
        string nofChannelsStr = channelNode.child_value();

        size_t nofChannels = 1;
        if (false == nofChannelsStr.empty())
        {
          nofChannels = static_cast<size_t>(stoi(nofChannelsStr));
        }
        
        // clock
        xml_node clockNode = root.child(Constants::XmlClock.c_str());
        string clockStr = clockNode.child_value();
        if (clockStr.empty())
        {
          throw DaiException(ErrorCodes::InvalidFormatOfIQTarXmlContent, "XmlClock tag not found");
        }

        double clock = stod(clockStr);

        // samples
        xml_node sampleNode = root.child(Constants::XmlSamples.c_str());
        string samplesStr = sampleNode.child_value();
        if (samplesStr.empty())
        {
          throw DaiException(ErrorCodes::InvalidFormatOfIQTarXmlContent, "XmlSamples tag not found");
        }

        this->nofSamples_ = static_cast<size_t>(stoll(samplesStr));

        // center frequency
        double frequency = this->readCenterFrequency(doc);

        // CenterFrequency, ClockRate and Samples are equal for all channels of iq-tar.
        // ChannelNames are optional in the UserData section
        // Generate default channel names, as they are not mandatory in iq-tar specification.
        vector<string> channelNames;
        channelNames.reserve(nofChannels);
        for (size_t i = 0; i < nofChannels; ++i)
        {
          channelNames.push_back(ChannelInfo::getDefaultChannelName(i));
        }
        
        // If IQTar file has been written with this API, the user data section will
        // contain channel names
        try
        {
          xml_node channelsNode = doc
            .child(Constants::XmlRootNode.c_str())
            .child(Constants::XmlUserData.c_str())
            .child(Constants::XmlRohdeSchwarz.c_str())
            .child(Constants::XmlMandatoryUserData.c_str())
            .child(Constants::XmlChannelNames.c_str());
          if (false == channelsNode.empty())
          {
            size_t childCount = std::distance(channelsNode.children().begin(), channelsNode.children().end());
            if (childCount <= nofChannels)
            {
              int idx = 0;
              for (xml_node child = channelsNode.first_child(); child;)
              {
                channelNames[idx].assign(child.child_value());

                idx++;
                child = child.next_sibling();
              }
            }  
          }
        }
        catch (...) 
        {
          // ChannelNames are not mandatory, therefore this is not an error
        }

        // Create arrayNames from channel names by appending "_I" and "_Q"
        // and store it with reference to the channel number
        this->arrayNameToChannelNo_.clear();
        for (size_t i = 0; i < nofChannels; ++i)
        {
          this->updateContent_->updateChannels(channelNames[i], clock, frequency, this->nofSamples_);

          if (DataImportExportBase::getValuesPerSample(this->dataFormat_) == 1)
          {
            this->arrayNameToChannelNo_.insert(make_pair(channelNames[i], i));
          }
          else
          {
            this->arrayNameToChannelNo_.insert(make_pair(channelNames[i] + "_I", i));
            this->arrayNameToChannelNo_.insert(make_pair(channelNames[i] + "_Q", i));
          }
        }
      }

      double IqTarReader::readCenterFrequency(pugi::xml_document& doc)
      {
        double freq = numeric_limits<double>::quiet_NaN();

        try
        {
          xml_node userDataNode = doc
            .child(Constants::XmlRootNode.c_str())
            .child(Constants::XmlUserData.c_str());
          if (userDataNode.empty())
          {
            return freq;
          }

          xml_node rsNode = userDataNode.child(Constants::XmlRohdeSchwarz.c_str());
          if (rsNode.empty())
          {
            return freq;
          }

          xml_node child = rsNode.first_child();
          while(false == child.empty())
          {
            xml_node freqNode = child.child(Constants::XmlCenterFrequency.c_str());
            if (freqNode.empty())
            {
              child = child.next_sibling();
            }
            else
            {
              string freqStr = freqNode.child_value();
              if (false == freqStr.empty())
              {
                freq = stod(freqStr);
              }

              break;
            }
          }
        }
        catch (...)
        {
          // CenterFrequency is not a mandatory item -> therefore this is not an error!
        }

        return freq;
      }

      void IqTarReader::readOptionalMetadata(pugi::xml_document& doc)
      {
        try
        {
          xml_node optUserDataNode = doc
            .child(Constants::XmlRootNode.c_str())
            .child(Constants::XmlUserData.c_str())
            .child(Constants::XmlRohdeSchwarz.c_str())
            .child(Constants::XmlOptionalUserData.c_str());
          if (optUserDataNode.empty())
          {
            return;
          }

          for (xml_node child = optUserDataNode.first_child(); child;)
          {
            string nodename = child.name();
            if (Common::strCmpIgnoreCase(nodename, "Key"))
            {
              xml_attribute keyattr = child.attribute("name");
              if (false == keyattr.empty())
              {
                // get key
                string key = keyattr.value();
                Common::escapeXmlToStr(key);

                // get value:
                // if this node contains further sub-nodes, we
                // return the plain XML structure, as requested. 
                // If no sub-node is found, we return the escaped value.
                string value;
                
                size_t childCount = std::distance(child.children().begin(), child.children().end());
                if (childCount == 1)
                {
                  value = child.child_value();
                  Common::escapeXmlToStr(value);
                }
                else
                {
                  stringstream ss;

                  for (xml_node subNode = child.first_child(); subNode;)
                  {
                    subNode.print(ss, "");
                    subNode = subNode.next_sibling();
                  }

                  value = ss.str();
                }

                // remove new line
                value.erase(remove(value.begin(), value.end(), '\n'), value.end());

                this->updateContent_->updateMetadata(key, value);
              }
            }

            child = child.next_sibling();
          }
        }
        catch (...)
        {
           // OptionalUserData is optional! --> no error
        }
      }

      void IqTarReader::readDeprecatedData(pugi::xml_document& doc)
      {
        try
        {
          xml_node rsNode = doc
            .child(Constants::XmlRootNode.c_str())
            .child(Constants::XmlUserData.c_str())
            .child(Constants::XmlRohdeSchwarz.c_str());
          if (rsNode.empty())
          {
            return;
          }

          for (xml_node subNode = rsNode.first_child(); subNode;)
          {
            string nodename = subNode.name();

            // if node name does not equal 'mandatory' or 'optional' data,
            // we interpret the first node found as deprecated data node.
            if (false == Common::strCmpIgnoreCase(nodename, Constants::XmlMandatoryUserData) && 
              false == Common::strCmpIgnoreCase(nodename, Constants::XmlOptionalUserData))
            {
              stringstream ss;
              subNode.print(ss, "");

              string deprecatedInfo = ss.str();

              // remove new line
              deprecatedInfo.erase(remove(deprecatedInfo.begin(), deprecatedInfo.end(), '\n'), deprecatedInfo.end());

              this->updateContent_->updateDeprecatedInfo(deprecatedInfo);
              break;
            }

            subNode = rsNode.next_sibling();
          }
        }
        catch (...)
        {
          // deprecated info is optional! --> no error
        }
      }

      bool IqTarReader::containsArray(const std::string& arrayName) const
      {
        return this->arrayNameToChannelNo_.count(arrayName) == 0 ? false : true;
      }

      size_t IqTarReader::getNofSamples() const
      {
        return this->nofSamples_;
      }

      __LA_SSIZE_T IqTarReader::archiveReadCallback(struct archive* /*a*/, void* clientData, const void** buffer)
      {
        struct mmfReadMemoryData* data = (struct mmfReadMemoryData*)clientData;

        if (false == data->mmf.is_open())
        {
          return ARCHIVE_FATAL;
        }

        size_t fileSize = data->mmf.file_size();
        if (data->offset > fileSize)
        {
          return ARCHIVE_FATAL;
        }

        size_t readSize = data->copyBufSize;
        if (data->offset + data->copyBufSize > fileSize)
        {
          readSize = fileSize - data->offset;
        }

        // map data
        data->mmf.map(data->offset, readSize);
        *buffer = data->mmf.data();

        // calculate new offset
        data->offset += readSize;
        
        return (__LA_SSIZE_T)readSize;
      }

      __LA_INT64_T	IqTarReader::archiveReadSkipCallback(struct archive* /*a*/, void* clientData, __LA_INT64_T request)
      {
        struct mmfReadMemoryData* data = (struct mmfReadMemoryData*)clientData;

        if (false == data->mmf.is_open())
        {
          return ARCHIVE_FATAL;
        }

        size_t fileSize = data->mmf.file_size();
        size_t newpos = data->offset + (size_t)request;
        if (newpos > fileSize)
        {
          return 0;
        }

        data->offset += (size_t)request;
        return request;
      }

      int IqTarReader::archiveCloseCallback(struct archive* /*a*/, void* clientData)
      {
        struct mmfReadMemoryData* data = (struct mmfReadMemoryData*)clientData;
        
        try
        {
          data->close();
        }
        catch (...)
        {
          return ARCHIVE_FATAL;
        }
        
        return ARCHIVE_OK;
      }
    }
  }
}