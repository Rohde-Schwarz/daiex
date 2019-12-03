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

#pragma once

namespace RohdeSchwarz
{
  namespace Mosaik
  {
    namespace DataImportExport
    {
      using namespace System::Runtime::Serialization;

      /// <summary>
      /// Base class of a ScpiServerException.
      /// </summary>
      //[System::Serializable()]
      public ref class ExceptionBase abstract : public System::Exception
      {
      public:
        /// <summary>
        /// Initializes a new instance of the <see cref="ExceptionBase" /> class.
        /// </summary>
        ExceptionBase() : System::Exception() {}

      protected:
        /// <summary>
        /// Initializes a new instance of the <see cref="ExceptionBase" /> class.
        /// </summary>
        /// <param name="info">The serialization information.</param>
        /// <param name="context">The streaming context.</param>
        ExceptionBase(System::Runtime::Serialization::SerializationInfo^ info,
        System::Runtime::Serialization::StreamingContext context) : System::Exception(info, context) {}
      };

      /// <summary>
      /// Exception thrown by libdaiex-wrapper. Check the error code for further information.
      /// </summary>
      public ref class DaiException : ExceptionBase
      {
      public:
        /// <summary>
        /// Initializes a new instance of the <see cref="DaiException" /> class.
        /// </summary>
        /// <param name="errorCode">Error code of native libdaiex that caused this exception.</param>
        /// <param name="errorText">The error message text that corresponds to the error code that caused this exception.</param>
        DaiException(int errorCode, String^ errorText) : ExceptionBase() 
        {
          this->errorCode_ = errorCode; 
          this->errorText_ = errorText;
        }

        /// <summary>
        /// Gets the native error code of this exception.
        /// </summary>
        property int ErrorCode
        {
          int get()
          {
            return this->errorCode_;
          }
        }

        /// <summary>
        /// Gets a text that describes the error.
        /// </summary>
        property String^ Message
        {
          virtual String^ get() override 
          {
            return this->errorText_;
          }
        }

      private:
        /// <summary>
        /// Initializes a new instance of the <see cref="DaiException" /> class.
        /// </summary>
        DaiException() : ExceptionBase() {}

        /// <summary>
        /// Native error code that corresponds to this exception.
        /// </summary>
        int errorCode_;

        /// <summary>
        /// Message that corresponds to the native error code, obtained via ErrorCodes::getErrorText().
        /// </summary>
        String^ errorText_;
      };
    }
  }
}