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

#include "csv_writer.h"

#include "common.h"

using namespace std;

#ifdef _WIN32
#define CRLF "\r\n"
#else
#define CRLF "\n"
#endif

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      CsvWriter::CsvWriter(const std::string& filename, const char& separator, const std::string& postfix) :
        initialized_(false),
        filename_(filename),
        columnSeperator_(separator),
        postfix_(postfix)
      {
      }

      CsvWriter::~CsvWriter()
      {
        this->close();
      }

      void CsvWriter::open()
      {
        // Set exceptions to be thrown on failure
        Platform::streamOpen(this->stream_, this->filename_, ios::out | ios::trunc | ios::binary);
        if (false == this->stream_.is_open())
        {
          throw DaiException(ErrorCodes::FileOpenError);
        }

        // write UTF-8 BOM
        this->stream_ << "\xEF\xBB\xBF";
        this->initialized_ = true;
      }

      void CsvWriter::close()
      {
        this->stream_.flush();
        this->stream_.close();
        this->initialized_ = false;
      }

      void CsvWriter::flush()
      {
        if (false == this->initialized_)
        {
          throw DaiException(ErrorCodes::FileWriterUninitialized);
        }

        this->stream_.flush();
      }

      void CsvWriter::beg()
      {
        if (false == this->initialized_)
        {
          throw DaiException(ErrorCodes::FileWriterUninitialized);
        }

        this->stream_.seekp(3, ios::beg);
      }

      void CsvWriter::writeLine(const std::string& value)
      {
        if (false == this->initialized_)
        {
          throw DaiException(ErrorCodes::FileWriterUninitialized);
        }

        this->stream_ << value << CRLF;
      }

      void CsvWriter::writeWithoutNewLine(const std::string& value)
      {
        if (false == this->initialized_)
        {
          throw DaiException(ErrorCodes::FileWriterUninitialized);
        }

        this->stream_ << value;
      }

      void CsvWriter::writeMetadataLine(const std::string& key, const std::string& value)
      {
        if (false == this->initialized_)
        {
          throw DaiException(ErrorCodes::FileWriterUninitialized);
        }

        this->stream_ << key << this->columnSeperator_ << value << this->postfix_ << CRLF;
      }

      void CsvWriter::writeChannelInfoLine(int channelNumber, const std::string& key, const std::string& value)
      {
        if (false == this->initialized_)
        {
          throw DaiException(ErrorCodes::FileWriterUninitialized);
        }

        this->stream_ << "Ch" << channelNumber << "_" << key << this->columnSeperator_ << value << this->postfix_ << CRLF;
      }
    }
  }
}