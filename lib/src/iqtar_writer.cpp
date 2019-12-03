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

#include "iqtar_writer.h"
#include "iqtarpimpl.h"

#include <sstream>

#include "archive_entry.h"

#include "constants.h"
#include "common.h"
#include "platform.h"
#include "settings.h"

using namespace std;
using namespace memory_mapped_file;

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      IqTarWriter::IqTarWriter(
        const std::string& filename,
        IqDataFormat dataFormat,
        IqDataType dataType,
        size_t nofArrays,
        const std::string& applicationName,
        const std::string& comment,
        const time_t timestamp,
        const std::vector<ChannelInfo>& channelInfos,
        bool enablePreview,
        const std::string& tempPath,
        const std::map<std::string, std::string>* metadata,
        const std::string* deprecatedInfoXml,
        const uint64_t expectedNofIqBytes) :
      initialized_(false),
      filename_(filename),
      tempPath_(tempPath),
      nofSamplesWritten_(0),
      applicationName_(applicationName),
      comment_(comment),
      dataFormat_(dataFormat),
      dataType_(dataType),
      lockDataType_(false),
      nofArrays_(nofArrays),
      timestamp_(timestamp),
      channelInfos_(channelInfos),
      metadata_(),
      expectedNofIqBytes_(expectedNofIqBytes),
      writtenIqBytes_(0),
      archive_(nullptr),
      archiveEntry_(nullptr),
      enablePreview_(enablePreview)
      {	
        if (metadata != nullptr)
        {
          this->metadata_ = map<string, string>(metadata->begin(), metadata->end());
        }

        if (deprecatedInfoXml != nullptr)
        {
          this->deprecatedInfoXml_ = *deprecatedInfoXml;
        }
      }

      IqTarWriter::~IqTarWriter()
      {
        this->close();
      }

      std::string IqTarWriter::generateIqDataFilename(const std::string& tarFilename, int nofChannels, IqDataFormat dataFormat, IqDataType dataType, time_t timestamp)
      {
        string time = Common::toString(timestamp);

        // no ":" in filenames !
        Common::strReplace(time, ":", "");
        Common::strReplace(time, " ", "");

        stringstream ss;
        ss << "File_"
           << time << "."
           << IqDataFormatNames[dataFormat]
           << "."
           << nofChannels
           << "ch."
           << IqDataTypeNames[dataType];

        return ss.str();
      }

      std::string IqTarWriter::generateXml(const std::string& iqDataFilename)
      {
        ostringstream ss;

        ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        ss << "<!-- Please open this xml file in the web browser. If the stylesheet 'open_IqTar_xml_file_in_web_browser.xslt' is in the same directory the web browser can nicely display the xml file. -->\n";
        ss << "<?xml-stylesheet type=\"text/xsl\" href=\"open_IqTar_xml_file_in_web_browser.xslt\"?>\n";
        ss << "<RS_IQ_TAR_FileFormat fileFormatVersion=\"1\" xsi:noNamespaceSchemaLocation=\"http://www.rohde-schwarz.com/file/RsIqTar.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n";
        string appNameXml = this->applicationName_;
        Common::escapeStrToXml(appNameXml);
        ss << "  <" << Constants::XmlApplicationName << ">" << appNameXml << "</" << Constants::XmlApplicationName << ">\n";
        string commentXml = this->comment_;
        Common::escapeStrToXml(commentXml);
        ss << "  <" << Constants::XmlComment << ">" << commentXml << "</" << Constants::XmlComment << ">\n";

		string time = Common::toString(this->timestamp_);
		Common::strReplace(time, " ", "T");
        ss << "  <" << Constants::XmlDateTime << ">" << time << "</" << Constants::XmlDateTime << ">\n";
        
        ss << "  <Samples>" << ::to_string(this->nofSamplesWritten_) << "</Samples>\n"; 
        ss << "  <Clock unit=\"Hz\">" << ::to_string(this->channelInfos_[0].getClockRate()) << "</Clock>\n";
        
        string format = IqDataFormatNames[this->dataFormat_];
        transform(format.begin(), format.end(), format.begin(), ::tolower);
        ss << "  <" << Constants::XmlFormat << ">" << format << "</" << Constants::XmlFormat << ">\n";
        
        string type = IqDataTypeNames[this->dataType_];
        transform(type.begin(), type.end(), type.begin(), ::tolower);
        ss << "  <" << Constants::XmlDataType << ">" << type << "</" << Constants::XmlDataType << ">\n";


        ss << "  <ScalingFactor unit=\"V\">1</ScalingFactor>" << "\n";
        ss << "  <" << Constants::XmlChannels << ">" << this->channelInfos_.size() << "</" << Constants::XmlChannels << ">\n";
        ss << "  <DataFilename>" << iqDataFilename << "</DataFilename>\n";

        // Add channel names and center frequency to the UserData section
        // those values are not mandatory concerning the I/Q-tar specification
        // but are always written by this implementation
        
        // 1ES backward compatibility: UserData must be written in one line
        // </UserData> end tag must be in next line.
        ss << "  <UserData>";
        ss << "<RohdeSchwarz>";

        // if deprecated information was specified, add some additional xml.
        // requirement for old FSW versions
        if (false == this->deprecatedInfoXml_.empty())
        {
          ss << this->deprecatedInfoXml_;
        }

        ss << "<DataImportExport_MandatoryData>";
        ss << "<ChannelNames>";

        for (size_t i = 0; i < this->channelInfos_.size(); ++i)
        {
          string channelName = this->channelInfos_[i].getChannelName();
          if (channelName.empty())
          {
            channelName = ChannelInfo::getDefaultChannelName(i);
          }

          Common::escapeStrToXml(channelName);
          ss << "<ChannelName>" << channelName << "</ChannelName>";
        }

        ss << "</ChannelNames>";
        ss << "<CenterFrequency unit=\"Hz\">" << ::to_string(this->channelInfos_[0].getFrequency()) << "</CenterFrequency>";
        ss << "</DataImportExport_MandatoryData>";

        // Add optional metadata - specified by the user - to the UserData section if applicable
        if (this->metadata_.size() > 0)
        {
          ss << "<DataImportExport_OptionalData>";

          for (const auto& pair : this->metadata_)
          {
            string key = pair.first;
            string value = pair.second;

            Common::escapeStrToXml(key);

            // requirement by 1ES1:
            // if value contains valid xml, we write the raw xml,
            // otherwise we escape the string.
            if (false == Common::isValidXml(value))
            {
              Common::escapeStrToXml(value);
            }

            ss << "<Key name=\"" << key << "\">" << value << "</Key>";
          }

          ss << "</DataImportExport_OptionalData>";
        }

        ss << "</RohdeSchwarz>\n"; // <-- new line 
        ss << "  </UserData>\n";

        // add preview data
        if (this->enablePreview_)
        {
          ss << "  <PreviewData>\n";
          ss << "    <ArrayOfChannel length=\"" << ::to_string(this->channelInfos_.size()) << "\">\n";

          // get preview data
          vector<SChannelPreview> previewData;
          this->tarPreview_.getPreviews(previewData);

          int channelNo = 0;
          for (auto channel : this->channelInfos_)
          {
            // Start channel
            ss << "    <Channel>\n";

            if (channel.getChannelName().empty())
            {
              ss << "      <Name>Channel " << ChannelInfo::getDefaultChannelName(channelNo) << "</Name>\n";
              ss << "      <Comment>Channel " << ::to_string(channelNo) << " of " << ::to_string(this->channelInfos_.size()) << "</Comment>\n";
            }
            else
            {
              ss << "      <Name>" << channel.getChannelName() << "</Name>\n";
            }

            // write I/Q-Tar preview
            // in place conversion from V^2 to db.
            IqTarPreview::vVtoDb(previewData[channelNo].tPowerVsTime.vfMax, previewData[channelNo].tPowerVsTime.vfMin);
            IqTarPreview::vVtoDb(previewData[channelNo].tSpectrum.vfMax, previewData[channelNo].tSpectrum.vfMin);

            ss << "      <PowerVsTime>\n";
            ss << "        <Min>\n";
            ss << "          <ArrayOfFloat length=\"" + to_string(previewData[channelNo].tPowerVsTime.vfMin.size())  + "\">\n";
            for (auto val : previewData[channelNo].tPowerVsTime.vfMin)
            {
              ss << "            <float>" << to_string(static_cast<int>(val)) << "</float>\n";
            }
            ss << "          </ArrayOfFloat>\n";
            ss << "        </Min>\n";
            ss << "        <Max>\n";
            ss << "          <ArrayOfFloat length=\"" + to_string(previewData[channelNo].tPowerVsTime.vfMax.size())  + "\">\n";
            for (auto val : previewData[channelNo].tPowerVsTime.vfMax)
            {
              ss << "            <float>" << to_string(static_cast<int>(val)) << "</float>\n";
            }

            ss << "          </ArrayOfFloat>\n";
            ss << "        </Max>\n";
            ss << "      </PowerVsTime>\n";

            ss << "      <Spectrum>\n";
            ss << "        <Min>\n";
            ss << "          <ArrayOfFloat length=\"" + to_string(previewData[channelNo].tSpectrum.vfMin.size())  + "\">\n";
            for (auto val : previewData[channelNo].tSpectrum.vfMin)
            {
              ss << "            <float>" << to_string(static_cast<int>(val)) << "</float>\n";
            }
            ss << "          </ArrayOfFloat>\n";
            ss << "        </Min>\n";
            ss << "        <Max>\n";
            ss << "          <ArrayOfFloat length=\"" + to_string(previewData[channelNo].tSpectrum.vfMax.size())  + "\">\n";
            for (auto val : previewData[channelNo].tSpectrum.vfMax)
            {
              ss << "            <float>" << to_string(static_cast<int>(val)) << "</float>\n";
            }
            ss << "          </ArrayOfFloat>\n";
            ss << "        </Max>\n";
            ss << "      </Spectrum>\n";

            ss << "      <IQ>\n";
            ss << "        <Histogram width=\"" + to_string(previewData[channelNo].tIQ.uiWidth) + "\" height=\"" + to_string(previewData[channelNo].tIQ.uiWidth) + "\">" 
              <<	previewData[channelNo].tIQ.sHistogram << "</Histogram>\n";
            ss << "      </IQ>\n";

            channelNo++;

            // Finish channel
            ss << "    </Channel>\n";
          }

          ss << "    </ArrayOfChannel>\n";
          ss << "  </PreviewData>\n";
        }

        // Finish file and return
        ss << "</RS_IQ_TAR_FileFormat>";

        return ss.str();
      }

      void IqTarWriter::open()
      {
        if (this->initialized_)
        {
          return;
        }

        if (this->filename_.empty())
        {
          throw DaiException(ErrorCodes::FileNotFound);
        }

        // check if I/Q-tar data is consistent
        if (false == this->validateChannelInformation())
        {
          throw DaiException(ErrorCodes::InconsistentInputData);
        }

        // i/q file size unknown -> write temp file and create final tar when IqTar file is closed.
        if (this->expectedNofIqBytes_ == 0)
        {
          this->tempFile_ = Platform::getTmpFilename(this->tempPath_);
          Platform::mmfOpen(this->mmfWriter_, this->tempFile_, if_exists_truncate , if_doesnt_exist_create);
          if (false == this->mmfWriter_.is_open())
          {
            throw DaiException(ErrorCodes::FileOpenError);
          }
        }
        else 
        {
          // user has specified the number of i/q bytes to be written -> we will directly write to 
          // tar file without buffering data to tmp file.
          this->archive_ = archive_write_new();
          if (this->archive_ == nullptr)
          {
            throw DaiException(ErrorCodes::InternalError);
          }

          // init PAX tar archive, use binary filename encoding.
          Common::archiveAssert(archive_write_set_format_pax_restricted(this->archive_));
          Common::archiveAssert(Platform::archiveWriteOpen(this->archive_, this->filename_));

          // create tar header for i/q data file with specified i/q file size
          string iqDataFileName = IqTarWriter::generateIqDataFilename(this->filename_, static_cast<int>(this->channelInfos_.size()), this->dataFormat_, this->dataType_, this->timestamp_);

          this->archiveEntry_ = archive_entry_new2(this->archive_);
          archive_entry_set_pathname(this->archiveEntry_, iqDataFileName.c_str());
          archive_entry_set_size(this->archiveEntry_, this->expectedNofIqBytes_);
          archive_entry_set_filetype(this->archiveEntry_, AE_IFREG);
          archive_entry_set_perm(this->archiveEntry_, 0644);

		      // TODO remove lock with libarchive 4.0 -> setlocale should then not be used anymore
          Common::localeLock_.lock();
          Common::archiveAssert(archive_write_header(this->archive_, this->archiveEntry_));
          Common::localeLock_.unlock();
        }

        // init tar preview
        this->tarPreview_.initialize(256, 8, 32, this->channelInfos_.size());

        this->initialized_ = true;
      }

      void IqTarWriter::close()
      {
        if (false == this->initialized_)
        {
          return;
        }

        try
        {
          // finalize temp and write actual tar file
          if (this->expectedNofIqBytes_ == 0)
          {
            if (this->mmfWriter_.is_open())
            {
              this->mmfWriter_.flush();
              this->mmfWriter_.close();
            }

            this->finalizeTemporarySequence();
            this->deleteTempFiles();
          }
          else // finalize tar file, no tmp file written
          {
            this->finalizeTarArchive();
          }
        }
        catch (DaiException)
        {
          throw;
        }
        catch (const std::exception &e)
        {
          throw DaiException(ErrorCodes::InternalError, e.what());
        }

        this->initialized_ = false;
      }

      void IqTarWriter::deleteTempFiles()
      {
        remove(this->tempFile_.c_str());
      }

      bool IqTarWriter::validateChannelInformation()
      {
        if (this->channelInfos_.size() == 0)
        {
          return false;
        }

        // verify that channel names are unique
        if (false == DataImportExportBase::verifyUniqueChannelNames(this->channelInfos_))
        {
          return false;
        }

        // clock rate and center frequency have to be equal for each channel,
        // since only saved once in I/Q-tar format
        double clock = numeric_limits<double>::quiet_NaN();
        double freq = numeric_limits<double>::quiet_NaN();
        for (auto channel : this->channelInfos_)
        {
          if (0 != std::isnan(clock))
          {
            clock = channel.getClockRate();
          }
          else if (abs(clock - channel.getClockRate()) >= (double)0.00001)
          {
            return false;
          }

          if (0 != std::isnan(freq))
          {
            freq = channel.getFrequency();
          }
          else if (abs(freq - channel.getFrequency()) >= (double)0.00001)
          {
            return false;
          }
        }

        return true;
      }

      bool IqTarWriter::validateInputDataArray(size_t nofArrays, const std::vector<size_t>& sizeOfArrays)
      {
        // verify correct number of value arrays
        if (this->dataFormat_ == IqDataFormat::Real)
        {
          if (false == (this->channelInfos_.size() == nofArrays))
          {
            return false;
          }
        }
        else
        {
          if (nofArrays % 2 != 0)
          {
            return false;
          }

          if (false == (this->channelInfos_.size() == (nofArrays / 2)))
          {
            return false;
          }
        }

        if (false == this->validateEqualArrayLength(sizeOfArrays))
        {
          return false;
        }

        return true;
      }
      
      bool IqTarWriter::validateInputDataChannel(size_t nofChannels, const std::vector<size_t>& sizeOfArrays)
      {
        // verify correct number of value arrays
        if (this->channelInfos_.size() != nofChannels)
        {
          return false;
        }

        // all arrays have same length 
        if (false == this->validateEqualArrayLength(sizeOfArrays))
        {
          return false;
        }

        // array length must be dividable by 2 as array contains I/Q pairs.
        for (size_t i = 0; i < sizeOfArrays.size(); ++i)
        {
          if (sizeOfArrays[i] % 2 != 0)
          {
            return false;
          }
        }
        
        return true;
      }

      bool IqTarWriter::validateEqualArrayLength(const std::vector<size_t>& sizeOfArrays)
      {
        // verify array length
        size_t sampleSize = 0;
        for (auto size : sizeOfArrays)
        {
          if (sampleSize == 0)
          {
            sampleSize = size;
          }

          if (sampleSize != size)
          {
            return false;
          }
        }

        return true;
      }

      void IqTarWriter::finalizeTemporarySequence()
      {
        // create name of I/Q data file and generate xml file
        string iqDataFileName = IqTarWriter::generateIqDataFilename(this->filename_, static_cast<int>(this->channelInfos_.size()), this->dataFormat_, this->dataType_, this->timestamp_);

        struct archive* archive = archive_write_new();
        if (archive == nullptr)
        {
          throw DaiException(ErrorCodes::InternalError);
        }

        // init PAX tar archive, use binary filename encoding.
        Common::archiveAssert(archive_write_set_format_pax_restricted(archive));
        Common::archiveAssert(Platform::archiveWriteOpen(archive, this->filename_));

        this->addMetadataFilesToArchiv(archive);

        // add data file to tar
        long long size = static_cast<long long>(Platform::getFileSize(this->tempFile_));
        if (size < 0)
        {
          throw DaiException(ErrorCodes::InternalError);
        }
        
        struct archive_entry* entry = archive_entry_new2(archive);

        archive_entry_set_pathname(entry, iqDataFileName.c_str());
        archive_entry_set_size(entry, size);
        archive_entry_set_filetype(entry, AE_IFREG);
        archive_entry_set_perm(entry, 0644);

		    // TODO remove lock with libarchive 4.0 -> setlocale should then not be used anymore
        Common::localeLock_.lock();
        Common::archiveAssert(archive_write_header(archive, entry));
        Common::localeLock_.unlock();

        // copy data -> TODO: mmf?
        FILE* fp;
        fopen_s(&fp, this->tempFile_.c_str(), "rb");

        char buff[8192];
        size_t len = fread(buff, sizeof(buff[0]), sizeof(buff), fp);
        while (len > 0)
        {
          archive_write_data(archive, buff, len);
          len = fread(buff, sizeof(buff[0]), sizeof(buff), fp);
        }

        fclose(fp);

        archive_entry_free(entry);
        Common::archiveAssert(archive_write_close(archive));
        Common::archiveAssert(archive_write_free(archive));
      }

      void IqTarWriter::finalizeTarArchive()
      {
          if (this->writtenIqBytes_ == this->expectedNofIqBytes_ && this->archiveEntry_ != nullptr && this->archive_ != nullptr)
          {
              // finish i/q data file
              archive_entry_free(this->archiveEntry_);
              this->archiveEntry_ = nullptr;

              // add meta data (xml + xslt)
              this->addMetadataFilesToArchiv(this->archive_);

              // done
              Common::archiveAssert(archive_write_close(this->archive_));
              Common::archiveAssert(archive_write_free(this->archive_));
              this->archive_ = nullptr;
              this->mmfWriter_.flush();
              this->mmfWriter_.close();;
          }
          else if (this->archiveEntry_ != nullptr && this->archive_ != nullptr) // exception -> writtenIqBytes_ != expectedNofIqBytes
          {
              // finish i/q data file
              archive_entry_free(this->archiveEntry_);
              this->archiveEntry_ = nullptr;

              // done
              Common::archiveAssert(archive_write_close(this->archive_));
              Common::archiveAssert(archive_write_free(this->archive_));
              this->archive_ = nullptr;
              this->mmfWriter_.flush();
              this->mmfWriter_.close();;

              throw DaiException(ErrorCodes::InvalidTarArchive, "number of i/q samples written does not match the expected number of samples");
          }

      }

      void IqTarWriter::addMetadataFilesToArchiv(struct archive* a)
      {
        string iqDataFileName = IqTarWriter::generateIqDataFilename(this->filename_, static_cast<int>(this->channelInfos_.size()), this->dataFormat_, this->dataType_, this->timestamp_);
        string xml = IqTarWriter::generateXml(iqDataFileName);

        struct archive_entry* entry = archive_entry_new2(this->archive_);

        // add xml file to tar
        string xmlFileName = iqDataFileName + ".xml";
        archive_entry_set_pathname(entry, xmlFileName.c_str());
        archive_entry_set_size(entry, xml.size());
        archive_entry_set_filetype(entry, AE_IFREG);
        archive_entry_set_perm(entry, 0644);

		    // TODO remove lock with libarchive 4.0 -> setlocale should then not be used anymore
        Common::localeLock_.lock();
        Common::archiveAssert(archive_write_header(a, entry));
        Common::localeLock_.unlock();

        archive_write_data(a, xml.data(), xml.size());

        // if preview is enabled, add xslt file to tar
        if (this->enablePreview_)
        {
          const string xslt = Constants::getXslt();

          archive_entry_clear(entry);
          archive_entry_set_pathname(entry, Constants::XsltFileName.c_str());
          archive_entry_set_size(entry, xslt.size());
          archive_entry_set_filetype(entry, AE_IFREG);
          archive_entry_set_perm(entry, 0644);

 		      // TODO remove lock with libarchive 4.0 -> setlocale should then not be used anymore
          Common::localeLock_.lock();
          Common::archiveAssert(archive_write_header(a, entry));
          Common::localeLock_.unlock();

          archive_write_data(a, xslt.data(), xslt.size());
        }

        archive_entry_free(entry);
      }
    }
  }
}
