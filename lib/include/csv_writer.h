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
* @file      csv_writer.h
*
* @brief     This is the header file of class CsvWriter.
*
* @details   Class providing special write functions to generate MOSAIK CSV format.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include <fstream>
#include <vector>

#include "daiexception.h"
#include "errorcodes.h"

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief Class providing special write functions to generate MOSAIK CSV format.
      */
      class CsvWriter
      {
      public:
        /**
          @brief Constructor.
          @param [in]  filename Name of the file to be written.
          @param [in]  separator Character used to separate columns in CSV format.
          @param [in]  postfix String to be added at the end of each line, that is written using methods 
          writeMetadataLine() and writeChannelInfoLine().
        */CsvWriter(const std::string& filename, const char& separator, const std::string& postfix);

        /** @brief Destructor. Calls close()
        */~CsvWriter();

        /** @brief Opens the file for writing and writes a 3 byte BOM indicating UTF-8 encoding
        to the file.
        */void open();

        /** @brief Closes the file and releases the file handle.
        */void close();

        /** @brief Flushes the stream memory to file.
        */void flush();

        /** @brief Rewinds the file cursor to the position after the BOM mask.
        */void beg();

        /**
          @brief Adds a single line to the end of the current text stream. 
          @param [in]  value String to be added as line content.
          @throws DaiException(FileWriterUninitialized) if stream was not opened.
        */void writeLine(const std::string& value);

        /**
          @brief Adds a single line to the end of the current text stream, but no
          new line is added behind the added value.
          @param [in]  value String to be added as line content.
          @throws DaiException(FileWriterUninitialized) if stream was not opened.
        */void writeWithoutNewLine(const std::string& value);

        /**
          @brief Adds one meta data line to the end of the current text stream. Meta data is given as 
          key-value pair. Format: {key}{separator}{value}{postfix}.
          @param [in]  key String representing the key of the meta data key/value pair to be written to file.
          @param [in]  value String representing the value of the meta data key/value pair to be written to file.
          @throws DaiException(FileWriterUninitialized) if stream was not opened.
        */void writeMetadataLine(const std::string& key, const std::string& value);

        /**
          @brief Adds a line to the end of the current text stream that represents a single channel property (see ChannelInfo()).
          Format: Ch{channelNumber}_{key}{separator}{value}{postfix}
          @param [in]  channelNumber Number of the channel, whose property is written. Numbering starts with 1.
          @param [in]  key String representing the name of the ChannelInfo property.
          @param [in]  value String representing the value of the ChannelInfo property.
          @throws DaiException(FileWriterUninitialized) if stream was not opened.
        */void writeChannelInfoLine(int channelNumber, const std::string& key, const std::string& value);

      private:
        /** @brief Private default constructor. */
        CsvWriter();

        /** @brief Private copy constructor. */
        CsvWriter(const CsvWriter&);

        /** @brief Private assignment operator.*/
        CsvWriter& operator=(const CsvWriter&);

        /** @brief TRUE if stream was opened. */
        bool initialized_;

        /** @brief The actual file stream. */
        std::ofstream stream_;

        /** @brief Path to the output file. */
        std::string filename_;

        /** @brief Column separator of the CSV format. */
        char columnSeperator_;

        /** @brief String to be added at the end of each line, that is written using methods 
         writeMetadataLine() and writeChannelInfoLine(). 
        */std::string postfix_;
      };
    }
  }
}