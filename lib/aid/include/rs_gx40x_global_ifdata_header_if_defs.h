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

#ifndef _RS_GX40X_GLOBAL_IFDATA_HEADER_IF_DEFS_H
#define _RS_GX40X_GLOBAL_IFDATA_HEADER_IF_DEFS_H

/*++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "rs_gx40x_p_types.h"
#include "rs_gx40x_global_frame_header_if_defs.h"

/*---- End of include files ------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*++++ DEFINES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/** Mask for extracting 'change flag' from the Intermediate Frequency (IF) data stream status */
#define kIFD_HEADER_STATUSWORD__CHANGE_FLAG                   0x80000000UL

/** Mask for extracting 'dBFS flag' from the Intermediate Frequency (IF) data stream status */
#define kIFD_HEADER_STATUSWORD__DBFS_FLAG                     0x40000000UL

/** Masks for extracting 'user flags' from the Intermediate Frequency (IF) data stream status */
#define kIFD_HEADER_STATUSWORD__USER_FLAG_0                   0x00000001UL
#define kIFD_HEADER_STATUSWORD__USER_FLAG_1                   0x00000002UL
#define kIFD_HEADER_STATUSWORD__USER_FLAG_2                   0x00000004UL
#define kIFD_HEADER_STATUSWORD__USER_FLAG_3                   0x00000008UL
#define kIFD_HEADER_STATUSWORD__USER_FLAG_4                   0x00000010UL
#define kIFD_HEADER_STATUSWORD__USER_FLAG_5                   0x00000020UL
#define kIFD_HEADER_STATUSWORD__USER_FLAG_6                   0x00000040UL
#define kIFD_HEADER_STATUSWORD__USER_FLAG_7                   0x00000080UL

/** Value for 'no kFactor defined' */
#define kIFD_HEADER_KFACTOR__UNDEFINED                        0x80000000L

/** Masks for extracting 'user flags' from the Intermediate Frequency (IF) data block status */
#define kIFD_DATABLOCK_STATUSWORD__USER_FLAG_0                0x00000004UL
#define kIFD_DATABLOCK_STATUSWORD__USER_FLAG_1                0x00000008UL
#define kIFD_DATABLOCK_STATUSWORD__USER_FLAG_2                0x00000010UL
#define kIFD_DATABLOCK_STATUSWORD__USER_FLAG_3                0x00000020UL
#define kIFD_DATABLOCK_STATUSWORD__USER_FLAG_4                0x00000040UL
#define kIFD_DATABLOCK_STATUSWORD__USER_FLAG_5                0x00000080UL

/** Mask for extracting the flag indicating data block validity from
    the Intermediate Frequency (IF) data block status */
#define kIFD_DATABLOCK_STATUSWORD__INVALID_BLOCK              0x00000001UL

/** Mask for extracting the 'blanking' from the Intermediate Frequency (IF) data block status */
#define kIFD_DATABLOCK_STATUSWORD__BLANKING                   0x00000002UL

/** Mask for extracting the Automatic Gain Control reciprocal gain correction value from
    the Intermediate Frequency (IF) data block status */
#define kIFD_DATABLOCK_AGC_RECIPGAIN_MASK                     0xFFFF0000UL

/*---- End of defines ------------------------------------------------------*/


/*++++ GLOBAL TYPES DECLARATION ++++++++++++++++++++++++++++++++++++++++++++*/

/** The term 'fix' ('fixed' point) indicates signed (2s-complement) fixed
    point fractional numbers.

    The abbreviation 'RE' indicates a complex number real part.
    The abbreviation 'IM' indicates a complex number imaginary part.
 */

/**
* The IF DATA HEADER is used for the following data stream frame types:
* ekFRH_TYPE__IFDATA_32RE_32IM_FIX
* ekFRH_TYPE__IFDATA_16RE_16IM_FIX
* ekFRH_TYPE__IFDATA_16RE_16RE_FIX
* ekFRH_TYPE__IFDATA_32RE_32IM_FIX_RESCALED
* ekFRH_TYPE__IFDATA_32RE_32IM_FLOAT_RESCALED */


/** Type for fixed point complex signal sample with a 16bit real and a 16bit imaginary part.
* (ekFRH_TYPE__IFDATA_16RE_16IM_FIX) */
typedef struct
{
  ptypINT intPackedComplex;

} typIFD_SAMPLE_16RE_16IM_FIX;


/** Type for two 16bit fixed point real signal samples.
* (ekFRH_TYPE__IFDATA_16RE_16RE_FIX) */
typedef struct
{
  ptypINT intDoubleReal;

} typIFD_SAMPLE_16RE_16RE_FIX;


/** Type for fixed point complex signal sample with a 32bit real part and a 32bit imaginary part.
* (ekFRH_TYPE__IFDATA_32RE_32IM_FIX, ekFRH_TYPE__IFDATA_32RE_32IM_FIX_RESCALED) */
typedef struct
{
  ptypINT intReal;
  ptypINT intImag;

} typIFD_SAMPLE_32RE_32IM_FIX;


/** Type for a floating point complex signal sample with a 32bit real part and a 32bit imaginary part.
* (ekFRH_TYPE__IFDATA_32RE_32IM_FLOAT_RESCALED) */
typedef struct
{
  ptypFLOAT_SP float_spReal;
  ptypFLOAT_SP float_spImag;

} typIFD_SAMPLE_32RE_32IM_FLOAT;


/** Structure of the Intermediate Frequency (IF) data stream header.
    This header is followed by zero or more data blocks, each with it's own data block header.
    According to the size of uintDataHeaderLength in the frame header the normal or extended version is used. */
typedef struct
{
  /** The number of data blocks in this frame -- following this IF data stream header. */
  ptypUINT uintDatablockCount;

  /** Number of 32bit words (excluding datablock header) of data in each data block. */
  ptypUINT uintDatablockLength;

  /** Timestamp of the first signal data sample in the first data block -- uSeconds since 1.01.1970 00:00:00 */
  ptypBIGTIME bigtimeTimeStamp;

  /** IF data stream status.
      This status is bit-coded as follows (bit #31 is the bit of most significance):
        Bit #31:      Change flag --  '1' indicates that one of the fields in the IF data header, excluding the
                                        time stamp, differs from the previous IF data header.
                                      '0' indicates that none of the other fields in the IF data header, excluding the
                                        time stamp, differs from the previous IF data header.
        Bit #30:      dBFS  flag --   '1' indicates that all samples in this frame are considered to be dBFS (dB full scale)
                                      '0' indicates that the values 'intAntennaVoltageRef' and 'reciprocal gain correction'
                                          can be used to calculate the corresponding uV value for each sample
        Bits #29...#8:  Reserved.
        Bits #7...#0:   'User flags' for specialist signalling between IF data processing components. */
  ptypUINT uintStatusword;

  /** Signal source ID / antenna ID */
  ptypUINT uintSignalSourceID;

  /** Current signal source state.
      Configuration Set identifier of the Task Data Set currently being applied by the Intermediate Frequency signal source
      OR
      in the case of scan operation, the current scan step number.  */
  ptypUINT uintSignalSourceState;

  /** Tuner (center)frequency -- Hz (2x32 bit) */
  ptypUINT uintTunerFrequency_Low;    /** Less significant 32 bits */
  ptypUINT uintTunerFrequency_High;   /** More significant 32 bits */

  /** Intermediate Frequency (IF) bandwidth -- in Hz */
  ptypUINT uintBandwidth;

  /** Analog to Digital Converter (ADC) signal sample rate -- in samples / second. */
  ptypUINT uintSamplerate;

  /** Interpolation factor referred to the ADC signal sample rate. */
  ptypUINT uintInterpolation;

  /** Decimation factor referred to the ADC signal sample rate. */
  ptypUINT uintDecimation;

  /** Device / parameterisation specific correction value for the
      tuner front-end -- in units of 0,1 dBuV. */
  ptypINT intAntennaVoltageRef;

} typIFD_IFDATAHEADER;




/** Structure of the extended Intermediate Frequency (IF) data stream header.
    This header is followed by zero or more data blocks, each with it's own data block header.
    According to the size of uintDataHeaderLength in the frame header the normal or extended version is used. */
typedef struct
{
  /** The number of data blocks in this frame -- following this IF data stream header. */
  ptypUINT uintDatablockCount;

  /** Number of 32bit words (excluding datablock header) of data in each data block. */
  ptypUINT uintDatablockLength;

  /** Timestamp of the first signal data sample in the first data block -- uSeconds since 1.01.1970 00:00:00 */
  ptypBIGTIME bigtimeTimeStamp;

  /** IF data stream status.
      This status is bit-coded as follows (bit #31 is the bit of most significance):
        Bit #31:      Change flag --  '1' indicates that one of the fields in the IF data header, excluding the
                                        time stamp, differs from the previous IF data header.
                                      '0' indicates that none of the other fields in the IF data header, excluding the
                                        time stamp, differs from the previous IF data header.
        Bit #30:      dBFS  flag --   '1' indicates that all samples in this frame are considered to be dBFS (dB full scale)
                                      '0' indicates that the values 'intAntennaVoltageRef' and 'reciprocal gain correction'
                                          can be used to calculate the corresponding uV value for each sample
        Bits #29...#8:  Reserved.
        Bits #7...#0:   'User flags' for specialist signalling between IF data processing components. */
  ptypUINT uintStatusword;

  /** Signal source ID / antenna ID */
  ptypUINT uintSignalSourceID;

  /** Current signal source state.
      Configuration Set identifier of the Task Data Set currently being applied by the Intermediate Frequency signal source
      OR
      in the case of scan operation, the current scan step number.  */
  ptypUINT uintSignalSourceState;

  /** Tuner (center)frequency -- Hz (2x32 bit) */
  ptypUINT uintTunerFrequency_Low;    /** Less significant 32 bits */
  ptypUINT uintTunerFrequency_High;   /** More significant 32 bits */

  /** Intermediate Frequency (IF) bandwidth -- in Hz */
  ptypUINT uintBandwidth;

  /** Analog to Digital Converter (ADC) signal sample rate -- in samples / second. */
  ptypUINT uintSamplerate;

  /** Interpolation factor referred to the ADC signal sample rate. */
  ptypUINT uintInterpolation;

  /** Decimation factor referred to the ADC signal sample rate. */
  ptypUINT uintDecimation;

  /** Device / parameterisation specific correction value for the
      tuner front-end -- in units of 0,1 dBuV. */
  ptypINT intAntennaVoltageRef;


  /** Timestamp of the first sample of the datastream since starting the datastream
      (i.q. SampleCounter == 0 ) in nano seconds since 01.01.1970 00:00:00 UTC */
  ptypBIGTIME_NS bigtimeStartTimeStamp;

  /** 64bit SampleCounter of the first sample of the first datablock of this frame.
      Can be reset due to time synchronization in the tuner or similar. */
  ptypUINT uintSampleCounter_Low;
  ptypUINT uintSampleCounter_High;

  /** kFactor of the current antenna to determine field strength in 0.1dB/m.
      0x80000000 if no kFactor is defined */
  ptypINT  intKFactor;

} typIFD_IFDATAHEADER_EX;


/** Structure of the Intermediate Frequency (IF) data block header */
typedef struct
{
  /** Status for 'this' specific data block.
      This status is bit-coded as follows (bit #31 is the bit of most significance):
      Bits #31...#16: Automatic Gain Control reciprocal gain correction value used
                      for the following IF data samples.
                      These bits are extracted to represent an unsigned fractional.
      Bits #15...8:   Reserved.
      Bits #7...#2:   'User flags' for specialist signalling between IF data processing components.
      Bit #1:         Blanking flag.
                      This flag is set ('1') to indicate that the data in the (this) block may
                      have been falsified by some external event.
      Bit #0:         Flag indicating data block validity -- '1' == invalid.
                      This flag indicates that the data within the (this) block may be corrupt, OR
                      any one of the fields in the Intermediate Frequency (IF) datastream header does
                      not represent the data in the (this) block correctly. */
  ptypUINT uintStatusword; /** The data block status (word) */

} typIFD_DATABLOCKHEADER;


/** Structure for one data block */
typedef struct
{
  /** The Intermediate Frequency (IF) data block header */
  typIFD_DATABLOCKHEADER datablockheaderDatablockHeader;

  /** The Intermediate Frequency (IF) data block -- with a minimum of one datum */
  ptypUINT uintData [1/*typIFD_IFDATAHEADER::uintDatablockLength*/];

} typIFD_DATABLOCK;


/**
 The complete Intermediate Frequency (IF) data frame
 The structure definition for the entire frame format is used to illustrate the structure
 of the data frame and is also suitable for data generation.

 Do not use this structure for data parsing because new frame versions
 can use extended header types which might lead to wrong addressing in the
 frames. Use the Data Header Length information from the global Frame Header to
 correctly access the data samples after the Data Header.
*/
typedef struct
{
  /** The (generic) data stream frame header */
  typFRH_FRAMEHEADER frameheaderFrameHeader;

  /** The Intermediate Frequency (IF) data frame header */
  typIFD_IFDATAHEADER ifdataheaderDataHeader;

  /** The Intermediate Frequency (IF) data */
  typIFD_DATABLOCK datablockDataBody [1/*typIFD_IFDATAHEADER::uintDatablockCount*/];

} typIFD_IFDATA_FRAME;


/**
 The complete Intermediate Frequency (IF) data frame with new extended data header.
 The structure definition for the entire frame format is used to illustrate the structure
 of the data frame and is also suitable for data generation.

 Do not use this structure for data parsing because new frame versions
 can use extended header types which might lead to wrong addressing in the
 frames. Use the Data Header Length information from the global Frame Header to
 correctly access the data samples after the Data Header.
*/
typedef struct
{
  /** The (generic) data stream frame header */
  typFRH_FRAMEHEADER frameheaderFrameHeader;

  /** The Intermediate Frequency (IF) data frame header */
  typIFD_IFDATAHEADER_EX ifdataheaderDataHeader;

  /** The Intermediate Frequency (IF) data */
  typIFD_DATABLOCK datablockDataBody [1/*typIFD_IFDATAHEADER::uintDatablockCount*/];

} typIFD_IFDATA_FRAME_EX;

/*---- End of global types declaration -------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* ifndef for file (multi)inclusion lock */
/***** End of File **********************************************************/
