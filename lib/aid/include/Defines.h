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

#ifndef DEFINES_H
#define DEFINES_H

#include "Types.h"

#ifdef _WIN32

#else // Linux
#ifdef __APPLE__
	#define _UI64_MAX UINT64_MAX
	#define _fseeki64 fseeko
	#define _ftelli64 ftello
	#define __min min
//	#define errno_t int 
#else
   #define _UI64_MAX UINT64_MAX
   #define _fseeki64 fseeko64
   #define _ftelli64 ftello64
   #define __min(a,b) a<b?a:b
   #define errno_t int 
#endif
#endif

namespace AmlabFiles
{
  const int32 c_sizeof_uint32     = sizeof(uint32);              /*! Size of 32bit unsigned integer in bytes */
  const int32 c_sizeof_uint16     = sizeof(uint16);              /*! Size of 16bit unsigned integer in bytes */
  const float c_fMicroVolt2Volt   = 1e-6F;

  enum eStatus {
    ekNoError = 0,
    ekError,
    ekResourceAlloc,
    ekInvalidParameters,
    ekNoFileSpecified = 16,
    ekFileNotFound,
    ekFileCannotBeOpened,
    ekFileNotOpen,
    ekFileAlreadyOpen,
    ekUnsupportedFileFormat,
    ekInvalidFileHeader,
    ekEndOfFile,
    ekNotImplemented,
    ekMemoryAllocationError,
    ekWriteFileOpenError,
    ekWriteFileError,
    ekReadFileError
  };

  enum eSampleSize {
    ekUnsupportedSize =  0,
    ek8BitPerSample   =  8,
    ek16BitPerSample  = 16,
    ek24BitPerSample  = 24,
    ek32BitPerSample  = 32
  };

  enum eSpectrumFormat {
    ekUnknown           = 0,
    ekSpectrum          = 1,
    ekSpectrumExtended  = 2
  };

  enum eIqTarFormat
  {
    ekIQ_TAR_FORMAT_COMPLEX, /**< Complex - Complex-valued samples with in-phase (I) and quadrature (Q) values interleaved I/Q/I/Q/I/Q/... */
    ekIQ_TAR_FORMAT_REAL,    /**< Real - Real-valued samples */
    ekIQ_TAR_FORMAT_POLAR    /**< Polar - Complex-valued samples with magnitude (M) and phase (P) values interleaved M/P/M/P/M/P... */
  };

  enum eIqTarDataType
  {
    ekIQ_TAR_DATATYPE_INT16,   /**< int 16 - 16-bit signed integer */
    ekIQ_TAR_DATATYPE_INT32,   /**< int 32 - 32-bit signed integer */
    ekIQ_TAR_DATATYPE_FLOAT32, /**< float 32 - 32-bit floating-point */
    ekIQ_TAR_DATATYPE_FLOAT64  /**< float 64 - 64-bit floating-point */
  };


  /*! Enumerator for endian format (little endian, big endian) */
  enum eEndian
  {
    ekUnknownEndian = 0,
    ekLittleEndian = 1,
    ekBigEndian = 2
  };

}

#endif // not DEFINES_H
