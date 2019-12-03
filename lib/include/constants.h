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
* @file      constants.h
*
* @brief     This is the header file of class Constants.
*
* @details   This class defines constants, mainly those specified by the iq-tar format.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include <string>

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief This class defines constants, mainly those specified by the iq-tar format.
      */
      class Constants final
      {
      public:

        /**
        @returns Returns the xslt saved with the iq-tar file.
        */static std::string getXslt();

        /** @brief Name of the XSLT file inside the I/Q-tar file. */
        static const std::string XsltFileName;

        /** @brief Name of the root node. */
        static const std::string XmlRootNode;

        /** @brief Name of the node representing the application that created the file. */
        static const std::string XmlApplicationName;

        /** @brief Name of the node containing a comment. */
        static const std::string XmlComment;

        /** @brief Name of the node representing the timestamp when the file was created. */
        static const std::string XmlDateTime;

        /** @brief Name of the node naming the data file name. */
        static const std::string XmlDataFileName;

        /** @brief Name of the node naming the data format. */
        static const std::string XmlFormat;

        /** @brief Name of the node naming the data type.*/
        static const std::string XmlDataType;

        /** @brief Name of the node naming the number of channels. */
        static const std::string XmlChannels;

        /** @brief Name of the node containing the number of samples of each channel. */
        static const std::string XmlSamples;

        /** @brief Clock rate [Hz] specified in the file. */
        static const std::string XmlClock;

        /** @brief Scaling factor. */
        static const std::string XmlScalingFactor;

        /** @brief Name of the node under which user data can be stored. */
        static const std::string XmlUserData;

        /** @brief Name of the node under which user data written by a RohdeSchwarz application will be stored. */
        static const std::string XmlRohdeSchwarz;

        /** @brief CenterFrequency [Hz] specified in the user data section of the file. */
        static const std::string XmlCenterFrequency;

        /** @brief Name of the nodes, whose values store the channel names. */
        static const std::string XmlChannelName;

        /** @brief Name of the node, which represents the list of channel names. */
        static const std::string XmlChannelNames;

        /** @brief Name of the node, that encapsulates all mandatory meta data created by this implementation of I/Q-tar. */
        static const std::string XmlMandatoryUserData;

        /** @brief Name of the node, that encapsulates all optional meta data created by this implementation of I/Q-tar. */
        static const std::string XmlOptionalUserData;

        /** @brief In CSV file format an extra tag is needed to identify the end of the meta data section of a file. */
        static const std::string EndUserData;

        /** @brief Constant representing the possible separator character ',' as string. */
        static const char SeparatorColon;

        /** @brief Constant representing the possible separator character ';' as string. */
        static const char SeparatorSemiColon;

        /** @brief Constant representing the possible separator character '.' as string. */
        static const char SeparatorDot;

        /** @brief Constant representing the string to be placed in first line of each file 
        that is written using this class.
        */static const std::string CreationIdentifier;
      };
    }
  }
}