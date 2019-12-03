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

#ifndef _RS_GX40X_P_TYPES_H
#define _RS_GX40X_P_TYPES_H

/*++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/**  Nested includes are non preferred. */
#include <limits.h>
#include <float.h>

/*---- End of include files -------------------------------------------------*/


/*++++ GLOBAL DEFINES +++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*++++ Define limits for project types ++++++++++++++++++++++++++++++++++++++*/
/** Limits for 32-bit integer types corresponding to the
    definitions in limits.h */
#define pkINT_MAX   2147483647
#define pkINT_MIN (-2147483647 - 1) /**< -2147483648 */
#define pkCHAR_MAX  pkINT_MAX
#define pkCHAR_MIN  pkINT_MIN
#define pkUINT_MAX  0xffffffffU     /**<  4294967295 */

/** Characteristics for 32-bit IEEE 754/854 compatible single precision floating
   point format corresponding to the definitions in float.h */
#define pkFLT_SP_RADIX     2
#define pkFLT_SP_MANT_DIG  24
#define pkFLT_SP_MIN_EXP (-125)
#define pkFLT_SP_MAX_EXP   128
/*---------------------------------------------------------------------------*/

/*++++ Definition of the constants pkTRUE and pkFALSE for ptypBOOL ++++++++++*/
#define pkFALSE ((ptypBOOL)0)
#define pkTRUE  ((ptypBOOL)1)
/*---------------------------------------------------------------------------*/

/*++++ Stop compilation on platforms with less than 32 bit int's ++++++++++++*/
#if INT_MAX < pkINT_MAX
  #error Integer type (int) has less than 32 bits. This would lead to \
         problems with some enums in other project files which contain \
         32 bit wide members. Compilation halted.
#endif
/*---------------------------------------------------------------------------*/

/*---- End of global defines ------------------------------------------------*/


/*++++ GLOBAL TYPES DECLARATION +++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/**  Define a project 'type' for 'character'. The 8 LSBits of this 32-bit type
    contain one 8-bit character. */
#if CHAR_MAX == pkCHAR_MAX
  #if CHAR_MIN == pkCHAR_MIN
    typedef char ptypCHAR;
  #else
    #error Fault assigning typedef for 'ptypCHAR' (character).
  #endif

#elif SHRT_MAX == pkCHAR_MAX
  #if SHRT_MIN == pkCHAR_MIN
    typedef short ptypCHAR;
  #else
    #error Fault assigning typedef for 'ptypCHAR' (character).
  #endif

#elif INT_MAX == pkCHAR_MAX
  #if INT_MIN == pkCHAR_MIN
    typedef int ptypCHAR;
  #else
    #error Fault assigning typedef for 'ptypCHAR' (character).
  #endif

#elif LONG_MAX == pkCHAR_MAX
  #if LONG_MIN == pkCHAR_MIN
    typedef long ptypCHAR;
  #else
    #error Fault assigning typedef for 'ptypCHAR' (character).
  #endif

#else
  #error Fault assigning typedef for 'ptypCHAR' (character).
#endif
/*---------------------------------------------------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/** Define a project 'type' which may contain up to four 8-bit characters
   'packed' into a 32-bit word.
   If the original four characters form a (null-terminated) C-string,
   then the first character of the string is located to the 8-LSBits of
   the packed char, the next char to the next 8-Bits of the packed char
   and so on.

   EXAMPLE:
      (null-terminated) string "abc" maps to:
      32-bit word:   |    '\0'    |    'c'    |    'b'    |    'a'    |
                     31(MSBit)  24 23       16 15        8 7          0(LSBit)
*/

#if CHAR_MAX == pkCHAR_MAX
  #if CHAR_MIN == pkCHAR_MIN
    typedef char ptypPACKEDCHAR;
  #else
    #error Fault assigning typedef for 'ptypPACKEDCHAR'.
  #endif

#elif SHRT_MAX == pkCHAR_MAX
  #if SHRT_MIN == pkCHAR_MIN
    typedef short ptypPACKEDCHAR;
  #else
    #error Fault assigning typedef for 'ptypPACKEDCHAR'.
  #endif

#elif INT_MAX == pkCHAR_MAX
  #if INT_MIN == pkCHAR_MIN
    typedef int ptypPACKEDCHAR;
  #else
    #error Fault assigning typedef for 'ptypPACKEDCHAR'.
  #endif

#elif LONG_MAX == pkCHAR_MAX
  #if LONG_MIN == pkCHAR_MIN
    typedef long ptypPACKEDCHAR;
  #else
    #error Fault assigning typedef for 'ptypPACKEDCHAR'.
  #endif

#else
  #error Fault assigning typedef for 'ptypPACKEDCHAR'.
#endif
/*---------------------------------------------------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/** Define a project 'type' which may contain up to 
    two 16-bit UNICODE characters 'packed' into a 32-bit word.
    If the original two UNICODE characters form a (null-terminated) C-string,
    then the first character of the string is located to the 16-LSBits of
    the packed char, and the second UNICODE character is packed in the
    next 16 Bits

   EXAMPLE:
      two UNICODE characters "ab" maps to:
      32-bit word:   |    'b'    |    'a'    |
                     31(MSBit)  16 15       0(LSBit)
*/

#if CHAR_MAX == pkCHAR_MAX
  #if CHAR_MIN == pkCHAR_MIN
    typedef char ptypPACKEDUNICODE;
  #else
    #error Fault assigning typedef for 'ptypPACKEDUNICODE'.
  #endif

#elif SHRT_MAX == pkCHAR_MAX
  #if SHRT_MIN == pkCHAR_MIN
    typedef short ptypPACKEDUNICODE;
  #else
    #error Fault assigning typedef for 'ptypPACKEDUNICODE'.
  #endif

#elif INT_MAX == pkCHAR_MAX
  #if INT_MIN == pkCHAR_MIN
    typedef int ptypPACKEDUNICODE;
  #else
    #error Fault assigning typedef for 'ptypPACKEDUNICODE'.
  #endif

#elif LONG_MAX == pkCHAR_MAX
  #if LONG_MIN == pkCHAR_MIN
    typedef long ptypPACKEDUNICODE;
  #else
    #error Fault assigning typedef for 'ptypPACKEDUNICODE'.
  #endif

#else
  #error Fault assigning typedef for 'ptypPACKEDUNICODE'.
#endif
/*---------------------------------------------------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/**  Define a project 'type' for 'signed integer' numbers. */
#if SHRT_MAX == pkINT_MAX
  #if SHRT_MIN == pkINT_MIN
    typedef short ptypINT;
  #else
    #error Fault assigning typedef for 'ptypINT' (signed integer).
  #endif

#elif INT_MAX == pkINT_MAX
  #if INT_MIN == pkINT_MIN
    typedef int ptypINT;
  #else
    #error Fault assigning typedef for 'ptypINT' (signed integer).
  #endif

#elif LONG_MAX == pkINT_MAX
  #if LONG_MIN == pkINT_MIN
    typedef long ptypINT;
  #else
    #error Fault assigning typedef for 'ptypINT' (signed integer).
  #endif

#else
  #error Fault assigning typedef for 'ptypINT' (signed integer).
#endif
/*---------------------------------------------------------------------------*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/**  Define a project 'type' for 'unsigned integer' numbers. */
#if USHRT_MAX == pkUINT_MAX
  typedef unsigned short ptypUINT;

#elif UINT_MAX == pkUINT_MAX
  typedef unsigned int ptypUINT;

#elif ULONG_MAX == pkUINT_MAX
  typedef unsigned long ptypUINT;

#else
  #error Fault assigning typedef for 'ptypUINT' (unsigned integer).
#endif
/*---------------------------------------------------------------------------*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/** Define a project 'type' for IEEE 754/854 compatible 32-bit floating point
   format */
#if FLT_RADIX == pkFLT_SP_RADIX
  #if (FLT_MANT_DIG == pkFLT_SP_MANT_DIG) && \
      (FLT_MIN_EXP ==  pkFLT_SP_MIN_EXP) && \
      (FLT_MAX_EXP ==  pkFLT_SP_MAX_EXP)
    typedef float ptypFLOAT_SP;

  #elif (DBL_MANT_DIG == pkFLT_SP_MANT_DIG) && \
        (DBL_MIN_EXP ==  pkFLT_SP_MIN_EXP) && \
        (DBL_MAX_EXP ==  pkFLT_SP_MAX_EXP)
    typedef double ptypFLOAT_SP;

  #else
    #error Fault assigning typedef for 'ptypFLOAT_SP' \
           (floating point single precision).
  #endif
#else
  #error Fault assigning typedef for 'ptypFLOAT_SP' \
         (floating point single precision).
#endif
/*---------------------------------------------------------------------------*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/**  Define a project 'type' for 'boolean'. */
#if USHRT_MAX == pkUINT_MAX
  typedef unsigned short ptypBOOL;

#elif UINT_MAX == pkUINT_MAX
  typedef unsigned int ptypBOOL;

#elif ULONG_MAX == pkUINT_MAX
  typedef unsigned long ptypBOOL;

#else
  #error Fault assigning typedef for 'ptypBOOL' (boolean).
#endif
/*---------------------------------------------------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/**  Define a project 'type' to contain a complex number made up of
    two 16-bit (signed)integer numbers.
    The 16-MSBits of the packed complex type contain the real part of the
    complex number.
    The 16-LSBits of the packed complex type contain the imaginary part of the
    complex number.
*/

#if USHRT_MAX == pkUINT_MAX
  typedef unsigned short ptypPACKEDCOMPLEXI16;

#elif UINT_MAX == pkUINT_MAX
  typedef unsigned int ptypPACKEDCOMPLEXI16;

#elif ULONG_MAX == pkUINT_MAX
  typedef unsigned long ptypPACKEDCOMPLEXI16;

#else
  #error Fault assigning typedef for 'ptypPACKEDCOMPLEXI16' \
         (packed complex_int16).
#endif
/*---------------------------------------------------------------------------*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/** Define a project 'type' for the representation of 'time'. Time is
   represented as a 64-bit count of microseconds elapsed since 01.01.1970,
   0:00 h UTC (UTC as described in: "RECOMMENDATION ITU-R TF.460-5,
   STANDARD-FREQUENCY and TIME-SIGNAL EMISSIONS, 1970-1974-1978-1982-1986-1997").
   This 64-bit count of microseconds rolls over in over 500000 years!
   The ptype is implemented as a union whose members may vary on
   differing platforms. */
#ifdef _WIN32
#pragma pack ( push, 4 )
#endif
typedef union
{
  /** Structure of two 32-bit values for use on all 32-bit platforms */
  struct
  {
    ptypUINT     uintTime_LoOrderBits; /**< 32 LSBits of the microseconds count */

    ptypUINT     uintTime_HiOrderBits; /**< 32 MSBits of the microseconds count */

  } structTimeInTwoWords;

  /** The following array is only a 'dummy' which should never be acessed. It's
     use is to force the compiler to hold the two members of the above struct
     in adjacent memory words. */
  ptypUINT       uintTimeAsArray[2];

#if  (( __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 9)) && _EXTENSION_WRS == 1) || WINVER >= 0x0501

  /** only true for VxWorks Pentium GNU-Compiler
      On platforms supporting 64-bit integer types an additional 64-bit word
      for the entire microsec count is declared. */

  unsigned long long uint64TimeInOneWord;

#endif
}  ptypBIGTIME;
#ifdef _WIN32
#pragma pack ( pop )
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/** Define a project 'type' for the representation of 'time' with higher
   resolution than ptypBIGTIME. The following type represents 'time'
   a 64-bit count of nanoseconds elapsed since 01.01.1970,  0:00 h UTC 
   (UTC as described in: "RECOMMENDATION ITU-R TF.460-6,
   STANDARD-FREQUENCY and TIME-SIGNAL EMISSIONS, 2002").
   This 64-bit count of nanoseconds rolls over in about 580 years!
   The ptype is implemented as a union whose members may vary on
   differing platforms. */
typedef ptypBIGTIME ptypBIGTIME_NS;
/*---------------------------------------------------------------------------*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/** Define a 'project type' for the return values of functions returning
   fault codes. */
typedef ptypINT ptypEX;
/*---------------------------------------------------------------------------*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/** Define a macro for definition of enum types. This shall be used
 *  where enum types are used in FlexComm messages. The macro will only define
 *  a type, based on the name of an enumeration, if the enumerator
 *  values are represented as 32-bit numbers on the current platform.
 *  Otherwise the desired type will be defined as of type ptypINT
 *
 *  example:
 *      this enum type (typeBase) is defined in the context of the user's header file
 *
 *      enum eBase {
 *           ebase1 = 256,
 *           ebase2 = 512
 *      };
 *      pmtypMSG_ENUM(enum eBase, typBase);   <- this is really new, user calls makro to define an enum-type !
 */
#if (INT_MAX==pkINT_MAX)
  #if (INT_MIN==pkINT_MIN)
    #define pmtypMSG_ENUM(enumerator, enumtyp) typedef enum enumerator enumtyp
  #endif
#endif
#ifndef pmtypMSG_ENUM
  #warning platform deviates from standard size for enumerator, enum types are replaced by ptypINT
  #define pmtypMSG_ENUM(enumerator, enumtyp) typedef ptypINT enumtyp
#endif

/*---------------------------------------------------------------------------*/

/*---- End of global types declaration --------------------------------------*/


#endif /* ifndef for file (multi)inclusion lock. */
/****** End of file. *********************************************************/
