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
/*******************************************************************************/
/**
@file iqxtypes.h
@description type definitions for iqx file format
* *******************************************************************************/

#pragma once

// system include files
#include <stdint.h>
#include <time.h>

// uuid
#ifdef WIN32
  #include <Winsock2.h>
  #include "wincompat.h"
#else
  #include <uuid/uuid.h>
#endif

// make sure we have LFS enabled with 64 bits
#ifndef WIN32
  #if _FILE_OFFSET_BITS != 64
  #error _FILE_OFFSET_BITS
  #endif
#endif 

// helpers
#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)
#ifdef __COUNTER__
  #define STATIC_ASSERT(e,m) enum { ASSERT_CONCAT(static_assert_, __COUNTER__) = 1/(int)(!!(e)) }
#else
  #define STATIC_ASSERT(e,m) enum { ASSERT_CONCAT(assert_line_, __LINE__) = 1/(int)(!!(e)) }
#endif

/******************************************************************************/
/**  IQX file format version */
#define IQX_VERSION 0x0013UL
/** minimum required version to understand the format */
#define IQX_MINREADVERSION 0x0010UL

/******************************************************************************/
/** memory aligned to be used for IQX frames */
#define IQX_DATA_ALIGNMENT 4096ULL
#define IQX_TRIGGER_ENTRY_ALIGNMENT 64ULL
#define IQX_CUE_ENTRY_ALIGNMENT 32ULL

/******************************************************************************/
/** @brief IQX frame size = preamble + header + data + tail size */
/** fixed preamble size - never ever change this */
#define IQX_PREAMBLESIZE  sizeof(IqxPreamble)
/** no header shall be smaller than IQX_HEADSIZE_MIN */
#define IQX_HEADSIZE_MIN  4096ULL
/** no frame shall be smaller than IQX_FRAMESIZE_MIN */
#define IQX_FRAMESIZE_MIN 8192ULL
/** no frame shall be larger than IQX_FRAMESIZE_MAX */
#define IQX_FRAMESIZE_MAX 0x20000000ULL

/** minimum frame (interrupt) rate per IQ channel (stream) */
#define IQX_IQFRAMERATE_MIN 8.0F
/** maximum frame (interrupt) rate per IQ channel (stream) */
#define IQX_IQFRAMERATE_MAX 16.0F

/** IQX header sync words (magic numbers) */
/** sync0 */
#define IQX_SYNC0 0xF7F67574F3F27170ULL
/** sync1 */
#define IQX_SYNC1 0x7776F5F47372F1F0ULL
/** sync2 */
#define IQX_SYNC2 0x8778F99F8118F22FULL
/** sync3 */
#define IQX_SYNC3 0x78876FF675576EE6ULL

/** all magic numbers to be checked */
const uint64_t iqxsync[] = {IQX_SYNC0, IQX_SYNC1, IQX_SYNC2, IQX_SYNC3};

/******************************************************************************/
// IQX file, bookmark etc. description text
/** max stringlen for file names */
#define IQX_MAX_FILENAME_STRLEN 128ULL
/** max stringlen for source name */
#define IQX_MAX_SOURCENAME_STRLEN 128ULL
/** max stringlen for user text */
#define IQX_MAX_USERTEXT_STRLEN 4092ULL
/** max stringlen for tags */
#define IQX_MAX_TAG_STRLEN 128ULL
/** max stringlen for bookmarks */
#define IQX_MAX_BOOKMARK_STRLEN 128ULL

/******************************************************************************/
/** stream number for stream independent frames */
#define IQX_STREAM_INDEPENDENT (-1)
/**  sequence number for independent frames */
#define SEQUENCENUM_INVALID (-1)
/** next chunk number if there is none */
#define IQX_NONEXTCHUNK (-1)
/** byte offset if unknown */
#define IQX_OFFS_UNKNOWN (-1)

/******************************************************************************/
/// @brief frame types
/// type of the frame
typedef uint32_t IqxFrameType;
/// most probably the field was not initialized
#define IQX_FRAME_TYPE_UNDEFINED        ((IqxFrameType) 0)
/// something went wrong here
#define IQX_FRAME_TYPE_UNKNOWN          ((IqxFrameType) 1)
/// file description frame
#define IQX_FRAME_TYPE_FILEDESC         ((IqxFrameType) 2)
/// stream description frame 
#define IQX_FRAME_TYPE_STREAMDESC       ((IqxFrameType) 3)
/// start of payload frame
#define IQX_FRAME_TYPE_PAYLOADSTART     ((IqxFrameType) 4)
/// end of payload frame
#define IQX_FRAME_TYPE_PAYLOADEND       ((IqxFrameType) 5)
/// tag frame
#define IQX_FRAME_TYPE_TAG              ((IqxFrameType) 6)
/// bockmark frame
#define IQX_FRAME_TYPE_BOOKMARK         ((IqxFrameType) 7)
/// cue frame
#define IQX_FRAME_TYPE_CUE              ((IqxFrameType) 8)
/// user text frame
#define IQX_FRAME_TYPE_USERTEXT         ((IqxFrameType) 9)
/// end of file frame
#define IQX_FRAME_TYPE_EOF              ((IqxFrameType) 10)
/// trigger frame
#define IQX_FRAME_TYPE_TRIGGER          ((IqxFrameType) 11)
/// data frame
#define IQX_FRAME_TYPE_DATA             ((IqxFrameType) 65536)
/// unknown data frame
#define IQX_FRAME_TYPE_UNKNOWNDATA      ((IqxFrameType) 65537)
/// iq data frames
#define IQX_FRAME_TYPE_IQDATA           ((IqxFrameType) 65538)
/// geolocation metadata frame
#define IQX_FRAME_TYPE_GEOLOC           ((IqxFrameType) 65792)

/******************************************************************************/
typedef uint32_t IqxExportPermission;
/// most probably the field was not initialized
#define IQX_EXPORT_PERMISSION_UNDEFINED        ((IqxExportPermission) 0)
/// something went wrong here
#define IQX_EXPORT_PERMISSION_UNKNOWN          ((IqxExportPermission) 1)
/// export not allowed
#define IQX_EXPORT_PERMISSION_PROHIBITED       ((IqxExportPermission) 2)
/// allow to export
#define IQX_EXPORT_PERMISSION_ALLOWED          ((IqxExportPermission) 3)

/******************************************************************************/
/// @brief stream types
/// type of the stream
typedef uint32_t IqxStreamType;
/// most probably the field was not initialized
#define IQX_STREAM_TYPE_UNDEFINED       ((IqxStreamType) 0)
/// something went wrong here
#define IQX_STREAM_TYPE_UNKNOWN         IQX_FRAME_TYPE_UNKNOWN
/// iq data frames
#define IQX_STREAM_TYPE_IQDATA16        (IQX_FRAME_TYPE_IQDATA + 0)
#define IQX_STREAM_TYPE_IQDATA8         (IQX_FRAME_TYPE_IQDATA + 1)
#define IQX_STREAM_TYPE_IQDATA24        (IQX_FRAME_TYPE_IQDATA + 3)
#define IQX_STREAM_TYPE_IQDATA32        (IQX_FRAME_TYPE_IQDATA + 4)
/// geolocation metadata frame
#define IQX_STREAM_TYPE_GEOLOC          IQX_FRAME_TYPE_GEOLOC

/******************************************************************************/
/// @brief trigger/marker types
/// type of the digiq metadata or other trigger source
typedef int64_t IqxTriggerType;

/// digiq metadata trigger
#define IQX_TRIGGER_TYPE_META_NA        ((IqxTriggerType) 0)
#define IQX_TRIGGER_TYPE_META_TDT0      ((IqxTriggerType) 1)
#define IQX_TRIGGER_TYPE_META_TDT1      ((IqxTriggerType) 2)
#define IQX_TRIGGER_TYPE_META_HBT       ((IqxTriggerType) 3)
#define IQX_TRIGGER_TYPE_META_EXT0      ((IqxTriggerType) 4)
#define IQX_TRIGGER_TYPE_META_SWT       ((IqxTriggerType) 5)
#define IQX_TRIGGER_TYPE_META_FMT       ((IqxTriggerType) 6)
#define IQX_TRIGGER_TYPE_META_PMT       ((IqxTriggerType) 7)
#define IQX_TRIGGER_TYPE_META_CFT       ((IqxTriggerType) 8)
#define IQX_TRIGGER_TYPE_META_RFPT      ((IqxTriggerType) 9)
#define IQX_TRIGGER_TYPE_META_IFPT      ((IqxTriggerType) 10)
#define IQX_TRIGGER_TYPE_META_IQPT      ((IqxTriggerType) 11)
#define IQX_TRIGGER_TYPE_META_MODT      ((IqxTriggerType) 12)
#define IQX_TRIGGER_TYPE_META_EXT1      ((IqxTriggerType) 13)
#define IQX_TRIGGER_TYPE_META_EXT2      ((IqxTriggerType) 14)
#define IQX_TRIGGER_TYPE_META_EXT3      ((IqxTriggerType) 15)
// TODO define IQW device triggers

/******************************************************************************/
/// portable standard types

/// float64
typedef double float64_t;

/// iqx_off_t like posix off_t
typedef int64_t iqx_off_t;

/******************************************************************************/
/// portable struct for timespec
typedef struct iqx_timespec
{
  /// seconds
  int64_t tv_sec;
  /// nano seconds
  int64_t tv_nsec;
} iqx_timespec;



/// portable bool type
typedef uint32_t iqx_bool;
#define IQX_BOOL_FALSE 0
#define IQX_BOOL_TRUE  1

/******************************************************************************/
/// portable struct for geolocation (gps fix)
typedef struct iqx_geolocation
{
  /// time of update
  iqx_timespec timestamp;
  /// latitude (degrees)
  double latitude;
  /// longitude (degrees)
  double longitude;
  /// altitude (meters)
  double altitude;
  /// course made good relative to true north (CMG)
  double track;
  /// speed over ground (meters/sec)
  double speed;
  /// vertical speed (meters/sec)
  double climb;
} iqx_geolocation;

/******************************************************************************/
/// @brief IQX frame preamble
#pragma pack(push, 1)
typedef struct
{
  /// sync with magic bytes
  uint64_t sync[4];
  /// size of the whole frame
  uint64_t framesize;
  /// size of the frame header
  uint64_t headsize;
  /// size of the frame data
  uint64_t datasize;
  /// a time stamp
  iqx_timespec timestamp;
  /// the type of the frame
  IqxFrameType frametype;
  /// set to IQX_STREAM_INDEPENDENT for stream independent frames
  int32_t streamnum;
  /// the size of the previous frame
  uint64_t previousframesize;
  /// we should we reserve some space here & align to 4k
  uint8_t reserved[4008];
} IqxPreamble;
#pragma pack(pop)
STATIC_ASSERT(sizeof(IqxPreamble) == IQX_DATA_ALIGNMENT, "IqxPreamble unaligned size");

/******************************************************************************/
/// @brief portable struct for uuid
typedef uint8_t iqx_uuid[16];

/******************************************************************************/
/// @brief file description frame header
#pragma pack(push, 1)
typedef struct 
{
  /// iqx file format version used in this recording
  uint32_t iqxversion;
  /// minimum iqx reader version understanding this recording
  uint32_t iqxreadversion;
  /// uuid conforming to OSF DCE 1.1 (identical for all chunks of this recording)
  iqx_uuid uuid;
  /// time since the epoch in s and ns, use with gettimeofday()
  iqx_timespec starttime;
  /// timezone offset from UTC in s, use with struct tm tm_gmtoff field
  int32_t timezoneoff;
  /// indicates wether DST is in effect, use with struct tm tm_isdst field
  int32_t daylight;
  /// this file is chunk number x
  uint32_t chunknum;
  /// next file is chunk number y (set to IQX_NONEXTCHUNK if none)
  int32_t chunknext;
  /// total number of streams in recording
  uint32_t nstreams;
  /// minimum frame size in this recording
  uint64_t framesize_min;
  /// maximum frams size in this recording
  uint64_t framesize_max;
  /// payload file offset (= first payload frame) from SEEK_SET (0 if not specified)
  iqx_off_t payloadoffset;
  /// epilogue file offset (= first epilogue frame) from SEEK_SET (0 if not specified)
  iqx_off_t epilogoffset;
  /// user pseudo file name
  char name[IQX_MAX_FILENAME_STRLEN];
  /// recording duration of this chunk
  struct iqx_timespec duration;
  /// '1' if this chunk was completed without errors
  int32_t complete;
  /// reserve some space to have 4k size
  uint8_t reserved[3856];
} IqxFileDescHeader;
#pragma pack(pop)
STATIC_ASSERT(sizeof(IqxFileDescHeader) == IQX_DATA_ALIGNMENT, "IqxFileDescHeader unaligned size");

/******************************************************************************/
/// @brief stream description frame header
#pragma pack(push, 1)
typedef struct
{
  // type of stream
  IqxStreamType type;
  /// min frame size this particular stream is using
  uint64_t framesize_min;
  /// max frame size this particular stream is using
  uint64_t framesize_max;
  /// data rate
  float64_t datarate;
  /// frame rate
  float64_t framerate;
  /// name of the recording source
  char sourcestr[IQX_MAX_SOURCENAME_STRLEN];
  /// reserve some space to have 4k size
  uint8_t reserved[3932];
} IqxStreamDescHeader;
#pragma pack(pop)
STATIC_ASSERT(sizeof(IqxStreamDescHeader) == IQX_DATA_ALIGNMENT, "IqxStreamDescHeader unaligned size");

/******************************************************************************/
/// @brief stream description frame data struct for digital IQ
#pragma pack(push, 1)
typedef struct 
{
  /// reference level
  float64_t reflevel;
  /// is the reference level valid
  iqx_bool reflevel_valid;
  /// is the reference level variable
  iqx_bool reflevel_variable;
  /// sample rate
  float64_t samplerate;
  /// is the sample rate valid
  iqx_bool samplerate_valid;
  /// is the sample rate variable (currently not supported)
  iqx_bool samplerate_variable;
  /// bandwidth
  float64_t bandwidth;
  /// is the bandwidth valid
  iqx_bool bandwidth_valid;
  /// is the bandwidth variable
  iqx_bool bandwidth_variable;
  /// center frequency
  float64_t center_frequency;
  /// is the center frequency valid
  iqx_bool centfreq_valid;
  /// resolution
  uint32_t resolution;
  /// is the resolution valid
  iqx_bool resolution_valid;
  /// is export allowed?
  IqxExportPermission export_permission;
  /// is the resolution valid
  iqx_bool permission_valid;
  /// reserve some space to have 4k size
  uint8_t reserved[4020];
} IqxStreamDescDataIQ16;
#pragma pack(pop)
STATIC_ASSERT(sizeof(IqxStreamDescDataIQ16) == IQX_DATA_ALIGNMENT, "IqxStreamDescDataIQ unaligned size");

/******************************************************************************/
/// @brief generic data frame header (should not be used unless data type is unknown)
#pragma pack(push, 1)
typedef struct
{
  /// set to SEQUENCENUM_INVALID if not applicable
  int64_t sequencenum;
  /// TODO is this crc32 of data? set to 0 if invalid?
  uint32_t checksum;
  /// reserve some space to have 4k size
  uint8_t reserved[4084];
} IqxGenericDataHeader;
#pragma pack(pop)
STATIC_ASSERT(sizeof(IqxGenericDataHeader) == IQX_DATA_ALIGNMENT, "IqxDataHeader unaligned size");

/******************************************************************************/
/// @brief IQ data frame header
#pragma pack(push, 1)
typedef struct
{
  /// set to SEQUENCENUM_INVALID if not applicable
  int64_t sequencenum;
  /// TODO is this crc32 of data? set to 0 if invalid?
  uint32_t checksum;
  /// reserve some space to have 4k size
  uint8_t reserved[4084];
} IqxIqDataHeader;
#pragma pack(pop)
STATIC_ASSERT(sizeof(IqxIqDataHeader) == IQX_DATA_ALIGNMENT, "IqxIqDataHeader unaligned size");

/******************************************************************************/
/// @brief geolocation header
#pragma pack(push, 1)
typedef struct
{
  /// set to SEQUENCENUM_INVALID if not applicable
  int64_t sequencenum;
  /// TODO is this crc32 of data? set to 0 if invalid?
  uint32_t checksum;
  /// reserve some space to have 4k size
  uint8_t reserved[4084];
} IqxGeolocationHeader;
#pragma pack(pop)
STATIC_ASSERT(sizeof(IqxGeolocationHeader) == IQX_DATA_ALIGNMENT, "IqxGeolocationHeader unaligned size");

/******************************************************************************/
/// @brief geolocation metadata frame data part
#pragma pack(push, 1)
typedef struct
{
  struct iqx_geolocation gpsdata;
  uint8_t reserved[4032];
} GeolocData;
#pragma pack(pop)
STATIC_ASSERT(sizeof(GeolocData) == IQX_DATA_ALIGNMENT, "GeolocData unaligned size");

/******************************************************************************/
/// @brief user text frame header
#pragma pack(push, 1)
typedef struct
{
  /// TODO should text be UTF-8 or something?
  char text[IQX_MAX_USERTEXT_STRLEN];
  /// is the text valid
  iqx_bool isvalid;
} IqxUserTextHeader;
#pragma pack(pop)
STATIC_ASSERT(sizeof(IqxUserTextHeader) == IQX_DATA_ALIGNMENT, "IqxUserTextHeader unaligned size");

/******************************************************************************/
/// @brief tag frame header
#pragma pack(push, 1)
typedef struct
{
  /// TODO should text be UTF-8 or something?
  char tag[IQX_MAX_TAG_STRLEN];
  /// is the text valid
  iqx_bool isvalid;
  /// reserve some space to have 4k size
  uint8_t reserved[3964];
} IqxTagHeader;
#pragma pack(pop)
STATIC_ASSERT(sizeof(IqxTagHeader) == IQX_DATA_ALIGNMENT, "IqxTagHeader unaligned size");

/******************************************************************************/
/// @brief bookmark frame header
#pragma pack(push, 1)
typedef struct
{
  /// TODO should text be UTF-8 or something?
  char text[IQX_MAX_TAG_STRLEN];
  /// is the text valid
  iqx_bool isvalid;
  iqx_timespec timestamp;
  /// reserve some space to have 4k size
  uint8_t reserved[3948];
} IqxBookmarkHeader;
#pragma pack(pop)
STATIC_ASSERT(sizeof(IqxBookmarkHeader) == IQX_DATA_ALIGNMENT, "IqxBookmarkHeader unaligned size");

/******************************************************************************/
/// @brief cue frame header
/// cue frame data part is an array of IqxCueEntry
#pragma pack(push, 1)
typedef struct
{
  /// number of cue entries in data part
  uint64_t numentries;
  /// reserve some space to have 4k size
  uint8_t reserved[4088];
} IqxCueHeader;
#pragma pack(pop)
STATIC_ASSERT(sizeof(IqxCueHeader) == IQX_DATA_ALIGNMENT, "IqxCueHeader unaligned size");

/******************************************************************************/
/// @brief cue entry
#pragma pack(push, 1)
typedef struct
{
  /// recording time (relative to 00:00:00)
  iqx_timespec timestamp;
  /// byte offset from SEEK_SET like off_t
  iqx_off_t offset;
  /// set to IQX_STREAM_INDEPENDENT if unknown
  int32_t streamnum;
  /// align to 32 byte size
  uint32_t reserved;
} IqxCueEntry;
#pragma pack(pop)
STATIC_ASSERT(sizeof(IqxCueEntry) == IQX_CUE_ENTRY_ALIGNMENT, "IqxCueEntry unaligned size");

// maximum number of cue entries
#define MAX_CUE_ENTRIES ((IQX_FRAMESIZE_MAX - sizeof(IqxCueHeader) - sizeof(IqxPreamble)) / sizeof(IqxCueEntry))

/******************************************************************************/
/// @brief trigger frame header
/// trigger frame data part is an array of IqxTriggerEntry
/// anything else we need in the header?
#pragma pack(push, 1)
typedef struct
{
  /// number of trigger entries in data part
  uint64_t numentries;
  /// reserve some space to have 4k size
  uint8_t reserved[4088];
} IqxTriggerHeader;
#pragma pack(pop)
STATIC_ASSERT(sizeof(IqxTriggerHeader) == IQX_DATA_ALIGNMENT, "IqxTriggerHeader unaligned size");

/******************************************************************************/
/// @brief trigger entry
/// define trigger types
#pragma pack(push, 1)
typedef struct
{
  /// recording time (relative to 00:00:00)
  iqx_timespec timestamp;
  /// timestamp and offset as indicated on digiq interface (0 if not applicable)
  uint64_t digiq_timestamp;
  uint16_t digiq_intersample_offset;
  /// trigger type
  IqxTriggerType type;
  /// set to IQX_STREAM_INDEPENDENT if unknown
  int32_t streamnum;
  /// align to 64 byte size
  uint8_t reserved[26];
} IqxTriggerEntry;
#pragma pack(pop)
STATIC_ASSERT(sizeof(IqxTriggerEntry) == IQX_TRIGGER_ENTRY_ALIGNMENT, "IqxTriggerEntry unaligned size");

// maximum number of tirgger entries
#define MAX_TRIGGER_ENTRIES ((IQX_FRAMESIZE_MAX - sizeof(IqxTriggerHeader) - sizeof(IqxPreamble)) / sizeof(IqxTriggerEntry))

/******************************************************************************/
/// @brief eof frame
#pragma pack(push, 1)
typedef struct
{
  uint8_t reserve[IQX_HEADSIZE_MIN];
} IqxEofHeader;
#pragma pack(pop)
STATIC_ASSERT(sizeof(IqxEofHeader) == IQX_DATA_ALIGNMENT, "IqxEofHeader unaligned size");
