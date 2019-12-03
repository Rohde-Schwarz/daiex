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
* @file      enums.h
*
* @brief     Contains definitions of enums.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include <map>

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief Specifies the binary format used for samples in I/Q data binary files.
      */enum IqDataType : int
      {
        /** @brief 32 bit floating point data (IEEE 754). **/
        Float32,

        /** @brief 64 bit floating point data (IEEE 754). **/
        Float64,

        /** @brief Max. enum size. Used when converting enum to string. **/
        IQDATATYPE_ENUM_SIZE
      };

      /** @brief String values of IqDataType-enum. **/
      static const char* IqDataTypeNames[] = { "float32", "float64" };

      /** @brief Converts IqDataType-enum value to string. **/
      static_assert(sizeof(IqDataTypeNames) / sizeof(char*) == IQDATATYPE_ENUM_SIZE, "IqDataType enum size mismatch");

      /**
      * @brief Specifies the data format used for samples in I/Q data binary files.
      */enum IqDataFormat : int
      {
        /** @brief Complex number in Cartesian format. I and Q are unit-less. **/
        Complex = 0,

        /** @brief Real number (unit-less). **/
        Real,

        /** @brief Complex number in polar format, magnitude (unit-less) and phase (rad) values.
        * Requires IQDataType float32 or float64. **/
        Polar,

        /** @brief Max. enum size. Used when converting enum to string. **/
        IQDATAFORMAT_ENUM_SIZE
      };

      /** @brief String values of IqDataForm-enum. **/
      static const char* IqDataFormatNames[] = { "complex", "real", "polar" };

      /** @brief Converts IqDataForm-enum value to string. **/
      static_assert(sizeof(IqDataFormatNames) / sizeof(char*) == IQDATAFORMAT_ENUM_SIZE, "IqDataFormat enum size mismatch");

      /**
      * @brief Specifies the order in which the I/Q data appears in the binary file format IQW.
      */enum IqDataOrder : int
      {
        /** @brief The file contains all I samples in 1 block, followed by all Q samples. **/
        IIIQQQ = 0,

        /** @brief 1 I and 1 Q sample alternate within the file (starting with I). **/
        IQIQIQ
      };

      /**
      * @brief Specifies the file formats supported by RohdeSchwarz.Mosaik.LibDataImportExport.
      */enum FileType : int
      {
        /** @brief Rohde &amp; Schwarz iq.tar format. **/
        Iqtar = 0,

        /** @brief A binary file format used by Rohde &amp; Schwarz containing 1 channel of complex
        * float32 binary data in data order IIIQQQ or IQIQIQ. **/
        IQW,

        /** @brief Matlab .MAT file version v4. **/
        Matlab4,

        /** @brief Matlab .MAT file version v7.3. **/
        Matlab73,

        /** @brief Comma separated values. ";" or "," can be used as separators. **/
        Csv,

        /** @brief A binary file format used by Rohde &amp; Schwarz containing channels of complex
        * int16 binary data in data order IQIQIQ. **/
        IQX,

        /** @brief A binary file format used by Rohde &amp; Schwarz containing channels of complex
        * int16 binary data in data order IQIQIQ. **/
        WV,

        /** @brief A binary file format used by Rohde &amp; Schwarz containing channels of complex
        * binary data in several orders and formats. **/
        AID

      };

      /**
      * @brief MAT file versions supported by LibDaiEx.
      */enum MatlabVersion : int
      {
        /** @brief Matlab version 4 file. */
        Mat4,

        /** @brief Matlab version 7.3 file. */
        Mat73
      };
    }
  }
}
