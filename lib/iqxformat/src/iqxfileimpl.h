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

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <utility>

#include "aligned_allocator.h"
#include "iqxframe.h"

#ifdef _WIN32
//#define aligned_alloc(alignement, size) _aligned_malloc(size, alignement)
//#define aligned_free _aligned_free
#define ALIGNED_VAR(type,name) type name //__declspec(align(4096)) type name
#else
#define ALIGNED_VAR(type,name) type name __attribute__ ((aligned(4096)))
//#define aligned_free free
#endif

namespace IQW
{

#ifdef _WIN32
	/// file open 
#define portable_open  _open
  /// file read
#define portable_read _read
	/// file close
#define portable_close _close
  /// ftell
#define portable_ftell _ftelli64
	/// seek for large files > 1TB
#define portable_lseek _lseeki64
	/// seek for large files > 1TB
#define portable_fseek _fseeki64
	/// stat command
#define portable_stat _stat64
#define portable_fstat _fstat64
	/// max calculation
#define portable_max max
#else
#ifdef __APPLE__
	/// file open 
#define portable_open  open
  /// file read
#define portable_read read
	/// file close
#define portable_close ::close
	/// seek for large files > 1TB
#define portable_lseek lseek
	/// seek for large files > 1TB
#define portable_fseek fseeko
	/// stat command
#define portable_stat stat
#define portable_fstat fstat
	/// max calculation
#define portable_max std::max
#else
	/// file open 
#define portable_open  open
  /// file read
#define portable_read read
	/// file close
#define portable_close ::close
  /// ftell
#define portable_ftell ftell
  /// seek for large files > 1TB
#define portable_lseek lseek64
	/// seek for large files > 1TB
#define portable_fseek fseeko
	/// stat command
#define portable_stat stat
#define portable_fstat fstat
	/// max calculation
#define portable_max std::max
#endif
#endif

/// @brief Implements access methods for the IQX file format
class IqxFileImpl
{
public:

  /// @brief tests if the file has IQX format
  static bool isIqxFile(const std::string& filename);

  /// @brief constructor. To be used only for reading/editing
  IqxFileImpl(const std::string& filename, bool edit = false);
  /// @brief constructor taking in file descriptor as argument. To be used only for reading
  IqxFileImpl(int fd);
  /// @brief constructor. To be used only for writing offline files
  IqxFileImpl(const std::string& filename, std::string applicationName, std::string comment, const std::vector<std::pair<std::string, IqxStreamDescDataIQ>>& iqStreams);
  /// @brief constructor. To be used only for on-the-fly writing
  IqxFileImpl(int fd, const std::string& descname, const std::vector<std::pair<std::string, IqxStreamDescDataIQ>>& iqStreams, std::vector<std::string> tags, const std::pair<std::string, uint32_t>& gpsStream);
  /// @brief destructor
  ~IqxFileImpl();

#if _MSC_VER > 1700 // from VS2013
  /// @brief copy constructor, forbidden
  IqxFileImpl(const IqxFileImpl&) = delete;
  /// @brief assign operator, forbidden
  IqxFileImpl& operator=(const IqxFileImpl&) = delete;
#endif
  /// @brief ???
  operator int();
  /// @brief gets the offset to the payload in the file
  size_t getPayloadOffset() const;
  /// @brief gets the offset to the epilogue
  size_t getEpilogueOffset() const;
  /// @brief returns a user readable description for the file
  const std::string& getDescriptionName() const;
  /// @brief the start time of the recording
  struct iqx_timespec getStartTime() const;
  /// @brief the timezone offset
  long getTZOffset() const;
  /// @brief the duration of the recording
  struct iqx_timespec getDuration() const;
  /// @brief get the tags
  const std::vector<std::string>& getTags() const;
  /// @brief get the file comment
  const std::string& getComment() const;
  /// @brief get the number of streams
  size_t getNumberOfStreams() const;
  /// @brief get the max frame sizes of all streams
  const std::vector<size_t>& getStreamMaxFrameSizes() const;
  /// @brief get the max frame size of a particular stream
  size_t getStreamMaxFrameSize(size_t streamno) const;
  /// @brief get the source names of all streams
  const std::vector<std::string>& getStreamSources() const;
  /// @brief get the source name of a particular stream
  const std::string& getStreamSource(size_t streamno) const;
  /// @brief get the ids of all IQ streams
  const std::vector<size_t>& getIqStreamNumbers() const;
  /// @brief get the number of frames for all streams
  const std::vector<uint64_t>& getStreamsNoOfFrames() const;
  /// @brief get the number of frames for a particular stream
  uint64_t getStreamNoOfFrames(size_t streamno) const;
  /// @brief get the number of samples of all IQ streams
  const std::vector<uint64_t>& getStreamsNoOfSamples() const;
  /// @brief get the number of samples for a particular stream
  uint64_t getStreamNoOfSamples(size_t streamno) const;
  /// @brief get the stream id for the corresponding iq stream name
  size_t getStreamNo(const std::string& streamOrArrayName) const;
  /// @brief get data rate of the corresponding the stream number
  double getIqStreamDataRate(size_t streamno) const;
  /// @brief get sample rate of the corresponding iq stream number
  double getIqStreamSampleRate(size_t streamno) const;
  /// @brief get measurement parameters (ref. level, center frequency, resolution, sample rate) of the corresponding source
  IqxStreamDescDataIQ getIqStreamParameters(const std::string& source) const;
  /// @brief get data type of a stream
  IqxStreamType getStreamType(size_t streamNo) const;
  /// @brief get gps update rate
  uint32_t getGpsUpdateRate() const;
  /// @brief get stream types of all streams in file
  std::map<size_t, IqxStreamType> getStreamTypes() const;
  /// @brief get export permission of particular stream
  IqxExportPermission getExportPermission(size_t streamno) const;
  /// @brief get timestamp of a particular sample
  iqx_timespec getTimestampFromSample(size_t streamno, uint64_t sample);
  /// @brief get sample number of a given timestamp
  uint64_t getSampleFromTimestamp(size_t streamno, iqx_timespec timestamp);
  /// @brief get trigger table of a particular stream
  std::vector<IqxTriggerEntry> getTriggers(size_t streamno);
  /// @brief get trigger table of a all streams in the recording
  std::vector<IqxTriggerEntry> getAllTriggers() const;
  /// @brief get cue entry of a given timestamp
  IqxCueEntry getCueEntry(size_t streamno, iqx_timespec timestamp);
  /// @brief get the next cue entry of a given timestamp. This is needed for partial export: the exported duration must contain the full second!
  IqxCueEntry getNextCueEntry(size_t streamno, iqx_timespec timestamp);
  /// @brief read uuid
  const std::string& getUuid() const;
  ///
  int getSequenceNo(size_t streamno) const;
  ///
  void setSequenceNo(size_t streamno, int sequenceNo);  
  /// @brief update recording duration
  void setDuration(iqx_timespec duration);
  /// @brief set comment for iqx file (optional)
  void setComment(std::string comment);
  /// @brief set bookmarks for iqx file (optional)
  void setBookmarks(std::map<std::string, iqx_timespec> bookmarks);  
  /// @brief set the time when recording is created. This is important for files that are imported
  void setStartTime(iqx_timespec time);
   /// @brief add a single trigger entry to trigger table
  void addTriggerEntry(IqxTriggerEntry& trigger);
  /// @brief add a single cue entry to cue table
  void addCueEntry(IqxCueEntry& cue);
  /// @brief add overrun entry
  void addOverrunEntry(IqxOverrunEntry& overrun);
  /// @brief edit description name
  void editRecordingName(const std::string& descname);
  /// @brief edit tags
  void editTags(const std::vector<std::string>& tags);
  /// @brief edit comment (user text)
  void editComment(const std::string& comment);
  /// @brief write a data frame to file
  void writeDataFrame(int64_t streamno, int64_t sequenceno, std::vector<int16_t>& data);
  /// @brief indicate if file has overrun
  bool hasOverrun() const;


private:
  /// @brief constructor help function
  void initRead(bool edit);
  /// @brief constructor help function
  void initWrite(const std::string& descname, const std::vector<std::pair<std::string, IqxStreamDescDataIQ>>& iqStreams, const std::pair<std::string, uint32_t>& gpsStream);

  /// @brief read a IQX file frame
  void readFrame(IqxPreamble& preamble, IqxFrameData& frame);

  /// @brief read data into buffer
  void readToBuffer(void*& buffer, size_t bytes);

  /// @brief test frame type
  void assertFrameType(IqxFrameType frameType, const IqxPreamble& preamble);

  /// @brief read the descriptors of all streams
  void readStreamDescriptors();

  /// @brief read meta data frames
  void readMetaData();

  /// @brief read all IQ data frames and calculate the sample count of each IQ stream (channel)
  void readIqFrameData();

  /// @brief write a frame to file from header, data an tail pointers and according size fields
  /// seek according to size field if pointer is NULL.
  void writeFrame(IqxPreamble& preamble, const void* head, const void* data, const void* tail);

  /// @brief write a file description frame to file
  void writeFileDescriptionFrame(IqxFileDescHeader& header, const void* data);

  /// @brief write a IQ stream description frame to file
  void writeIqStreamDescriptionFrame(size_t streamNumber, const std::string& streamName, const IqxStreamDescDataIQ& iqStreamDescriptor);

  /// @brief write a GPS stream description frame to file
  void writeGpsStreamDescriptionFrame(size_t streamNumber, const std::string& streamName, uint32_t updateRate);

  /// @brief write a payload start frame to file
  void writePayloadStartFrame();

  /// @brief write a payload end frame to file
  void writePayloadEndFrame();

  /// @brief write an EOF frame to file
  void writeEofFrame();

  /// @brief write a tag frame to file
  void writeTagFrame(const char* tag);

  /// @brief write a user text to file
  void writeTextFrame(const char* text);

  /// @brief update the file description at the end of recording
  void updateFileDescriptor(int32_t chunknext);

  /// @brief update the file description at the end of recording
  void setTags(std::vector<std::string> tags);

  /// @brief evaluate if iqx file is valid
  void validateFile();

  /// @brief write bookmark frames to iqx file
  void writeBookmarkFrame(const char* bookmark, iqx_timespec timestamp);

  /// @brief write trigger frame to iqx file
  void writeTriggerFrame();

  /// @brief read trigger frame from iqx file to member variable m_triggers
  void readTriggerFrame(IqxPreamble& preamble);

  /// @brief write cue frame to iqx file
  void writeCueFrame();

  /// @brief read cue frame from iqx file to member variable m_cues
  void readCueFrame(IqxPreamble& preamble);

  /// @brief overwrite existing tags in the recording
  void overwriteExistingTags(const std::vector<std::string>& tags, size_t size);

  /// @brief make the existing tag invalid (used for file editing)
  void invalidateExistingTag();

  /// @brief get cue table of a particular stream
  std::vector<IqxCueEntry> getCues(size_t streamno);

  /// @brief write overrun frame to iqx file
  void writeOverrunFrame();

  /// @brief read overrun frame from iqx file to member variable m_overruns
  void readOverrunFrame(IqxPreamble& preamble);

  // ------------------------------------ Properties -------------------------------------------------------

  /// distiguish between read and write
  bool m_write;
  /// file descriptor
  int m_fd;
  /// file description frame
  mutable IqxFileDescFrame m_fileDescFrame;
  /// description of the file
  std::string m_descName;
  /// tags of the file
  std::vector<std::string> m_tags;
  /// Comment of the file
  std::string m_comment;
  /// max frame sizes of all iq streams
  std::vector<size_t> m_streamMaxFrameSizes;
  /// names of all iq streams
  std::vector<std::string> m_streamSources;
  /// ids of all iq streams
  std::vector<size_t> m_iqStreamNumbers;
  /// stream types of all streams
  std::map<size_t, IqxStreamType> m_streams;
  /// number of frames for each iq stream
  std::vector<uint64_t> m_iqStreamNoOfFrames;
  /// stream ids and their number of samples
  std::vector<uint64_t> m_iqStreamNoOfSamples;
  /// a map which holds the iq stream no for each iq stream name
  std::map<std::string, size_t> m_streamNo;
  /// sequence of the stream for writing;
  std::vector<int> m_sequenceno;
  /// payload end offset
  uint64_t m_payloadOffset;
  /// epilog offset
  uint64_t m_epilogOffset;
  /// data rate of each stream
  std::vector<double> m_streamDataRate;
  /// measurement data of each iq stream
  std::map<std::string, IqxStreamDescDataIQ> m_iqProperties;
  /// recording duration
  iqx_timespec m_duration;
  /// update rate of Ublox
  uint32_t m_gpsUpdateRate;
  /// time when the recording starts
  iqx_timespec m_startTime;
  /// list of sample rate for each stream. Needed for calculation of duration
  std::vector<float64_t> m_streamSampleRate;
  /// list of samples for each stream. Needed for calculation of duration
  std::vector<float64_t> m_samples;
  /// list of bookmarks
  std::map<std::string, iqx_timespec> m_bookmarks;
  /* Note: the size of m_cues, m_triggers and m_overruns is always a multiple of IQX_DATA_ALIGNMENT as required by O_DIRECT write/read */
  /* Therefore, to get the vector size which contains actual data use variable m_cueNoOfEntries, m_trigNoOfEntries and m_overrunNoOfEntries accordingly */
  /// list of cue entries
  std::vector<IqxCueEntry, AlignedAllocator<IqxCueEntry, IQX_DATA_ALIGNMENT> > m_cues;
  uint64_t m_cueNoOfEntries;
  /// list of trigger entries
  std::vector<IqxTriggerEntry, AlignedAllocator<IqxTriggerEntry, IQX_DATA_ALIGNMENT> > m_triggers;
  uint64_t m_trigNoOfEntries;
  /// list of overrun entries
  std::vector<IqxOverrunEntry, AlignedAllocator<IqxOverrunEntry, IQX_DATA_ALIGNMENT> > m_overruns;
  uint64_t m_overrunNoOfEntries;
  /// flag to indicate file has overrun
  bool m_hasOverrun;
  bool m_edit;  
  /// uuid of the iqx file
  std::string m_uuid;
  /// list of necessary information of iqx file to be used for comment editing feature
  class MetaOffset
  {
  public:
    size_t descname{0};
    size_t comment{0};
    std::vector<size_t> tags;
    size_t eof{0};
  };
  MetaOffset m_metaOffset;
};

} // namespace
