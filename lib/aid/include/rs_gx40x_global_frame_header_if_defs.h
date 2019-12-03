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

/* Types and constant definitions for data stream frames. */

#ifndef _RS_GX40X_GLOBAL_FRAME_HEADER_IF_DEFS_H
#define _RS_GX40X_GLOBAL_FRAME_HEADER_IF_DEFS_H

/*++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "rs_gx40x_p_types.h"

/*---- End of include files ------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*++++ DEFINES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/** Magic word - used for synchronising data stream frames. */
#define kFRH_MAGIC_WORD               0xFB746572UL

/** The maximum length, in units of 32bit words, of a data stream frame.
    This length includes the length of the frame the header! */
#define kFRH_FRAME_LENGTH_MAX         0x100000UL


/** The maximum length, in units of 32bit words, of a data stream frame with extended size.
    In the statusword the frame must be marked with the flag kFRH_STATUSWORD__LENGTH_MAX_EX_FLAG.
    This length includes the length of the frame the header! */
#define kFRH_FRAME_LENGTH_MAX_EX      (64 * kFRH_FRAME_LENGTH_MAX)


/** Mask for extracting 'frame size may exceed kFRH_FRAME_LENGTH_MAX and limits to kFRH_FRAME_LENGTH_MAX_EX' from the frame header statusword.
    Only frames of particular data streams may exceed the maximum frame size, see frame type definitions. */
#define kFRH_STATUSWORD__LENGTH_MAX_EX_FLAG            0x00000001UL

/*---- End of defines ------------------------------------------------------*/


/*++++ TYPES DECLARATION +++++++++++++++++++++++++++++++++++++++++++++++++++*/

/** Structure type for a generic data stream frame header */
typedef struct
{
  /** A magic word; used for synchronisation. */
  ptypUINT uintMagicWord;

  /** The total length, in units of 32bit words, of the data stream frame.
      This includes the length of the header.
      Magic Words occur after (this number - 1) of 32bit words. */
  ptypUINT uintFrameLength;  /** This may not exceed 'kFRH_FRAME_LENGTH_MAX' */
  
  /** The (running) number of the current frame. */
  ptypUINT uintFrameCount;

  /** The type of data carried in this frame.
      The possible list of possible data stream frame types is in: rs_gx40x_global_frame_types_if_defs.h */
  ptypUINT uintFrameType;

  /** The length, in 32bit units, of the data header following this frame header.
      This value does not vary within any continuous data stream. */
  ptypUINT uintDataHeaderLength;

  #ifdef __cplusplus
    union {
      /** Reserved field */
      ptypUINT uintReserved;

      /** Frame header status.
          This status is bit-coded as follows (bit #31 is the bit of most significance):
            Bit #31...#1:   Reserved.
            Bit #0:         The size of a frame of this data stream may exceed kFRH_FRAME_LENGTH_MAX 32bit words. */
      ptypUINT uintStatusword;
    };
  #else
    /** Reserved field */
    ptypUINT uintReserved;
  #endif

} typFRH_FRAMEHEADER;


#define __C_ASSERT(expr) typedef char __C_ASSERT__rs_gx40x_global_frame_header_if_defs_h [(expr)?1:-1]
__C_ASSERT(sizeof(typFRH_FRAMEHEADER) == 6 * sizeof(ptypUINT));

/*---- End of types declaration --------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* ifndef for file (multi)inclusion lock */
/***** End of File **********************************************************/
