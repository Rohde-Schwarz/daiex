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
* @file      daiexception.h
*
* @brief     This is the header file of class DaiException.
*
* @details   The one and only exception that is thrown by libdaiex. Only for in-library use,
*						 IDataImportExport has to return error codes (requirement 1ES).
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include <exception>

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
        @brief The one and only exception that is thrown by libdaiex. Only for in-library use,
        IDataImportExport has to return error codes (requirement 1ES).
      **/
      class DaiException : public std::exception
      {
      public:
        /**
          @brief Constructor.
          @param [in]  errorCode rohdeschwarz::mosaik::dataimportexport::ErrorCodes code.
        */DaiException(int errorCode) : errorCode_(errorCode){}

        /**
          @brief Constructor.
          @param [in]  errorMessage Error message of this exception.
        */DaiException(const char* errorMessage) : errorMessage_(errorMessage){}

        /**
          @brief Constructor.
          @param [in]  errorCode  rohdeschwarz::mosaik::dataimportexport::ErrorCodes code.
          @param [in]  errorMessage Message related to the specified error code.
        */DaiException(int errorCode, const char* errorMessage) : errorCode_(errorCode), errorMessage_(errorMessage){}

        /**
          @returns Returns the error message of this exception.
        */const char* what() const throw() { return this->errorMessage_; }

        /**
          @returns Returns the error code of this exception.
        */int code() const { return this->errorCode_; }

      private:
        /** @brief Error code of this exception. */
        int errorCode_{0};

        /** @brief Error message of this exception. */
        const char* errorMessage_{""};
      };
    }
  }
}