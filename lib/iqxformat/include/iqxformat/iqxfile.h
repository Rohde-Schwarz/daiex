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

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <time.h>
#include <vector>

#include "iqxtypes.h"

namespace IQW
{


/// @brief returns errors for wrong file format
class iqxformat_error : public std::runtime_error
{
public:
  /// @brief constructor
  iqxformat_error(const std::string& err) : std::runtime_error(err) {}
};

class IqxFileImpl;

/// @brief encapsulates a iqx file
class IqxFile
{
public:

	/// test if a given file is an IQX file
  static bool isIqxFile(const std::string& filename);

  /// constructor with the full absolute filename, including the path
  IqxFile(const std::string& filename, bool edit = false);

  /// constructor with the file descriptor
  IqxFile(int fd);

  /// @brief constructor. To be used only for writing offline files
  IqxFile(const std::string& filename, std::string applicationName, std::string comment, const std::vector<std::pair<std::string, IqxStreamDescDataIQ>>& iqStreams);

  /// constructor with stream information for writing iqx file
  IqxFile(int fd, const std::string& descname, const std::vector<std::pair<std::string, IqxStreamDescDataIQ>>& iqStreams, std::vector<std::string> tags, const std::pair<std::string, uint32_t>& gpsStream);

  /// destructor
  ~IqxFile();

#if _MSC_VER > 1700 // from VS2013
  /// copy constructor does not exist
  IqxFile(const IqxFile&) = delete;

  /// copy assignment does not exist
  IqxFile& operator=(const IqxFile&) = delete;
#endif
  /// cast to file descriptor
  operator int();

  /// offset to the stream payload
  size_t getPayloadOffset() const;

  /// offset to the epilogue
  size_t getEpilogueOffset() const;

  /// get the user defined file description name
  const std::string& getDescriptionName() const;

  /// get the start time of the recording (in seconds since the epoch)
  struct iqx_timespec getStartTime() const;

  /// get the timezone offset when the file was recorded
  long getTZOffset() const;

  /// get the recording's duration in seconds
  struct iqx_timespec getDuration() const;

  /// get all tags of the recording
  const std::vector<std::string>& getTags() const;

  /// get the recording's comment
  const std::string& getComment() const;

  /// get the number of contained streams in the recording
  size_t getNumberOfStreams() const;

  /// get stream max frame sizes
  const std::vector<size_t>& getStreamMaxFrameSizes() const;

  /// get max frame size of particular stream
  size_t getStreamMaxFrameSize(size_t streamno) const;

  /// get stream source names
  const std::vector<std::string>& getStreamSources() const;

  /// get stream source name of particular stream
  const std::string& getStreamSource(size_t streamno) const;

  /// get the stream numbers corresponding to IQ data streams const;
  const std::vector<size_t>& getIqStreamNumbers() const;

  /// get stream number of frames
  const std::vector<uint64_t>& getStreamsNoOfFrames() const;

  /// get stream number of frames of particular stream
  uint64_t getStreamNoOfFrames(size_t streamno) const;

  /// get stream number of symbols
  const std::vector<uint64_t>& getStreamsNoOfSamples() const;

  /// get stream number of symbols of particular stream
  uint64_t getStreamNoOfSamples(size_t streamno) const;

  /// @brief get the stream id for the corresponding iq stream name
  size_t getStreamNo(const std::string& streamOrArrayName) const;

  /// get data rate of the corresponding stream number
  double getIqStreamDataRate(size_t streamno) const;

  /// get sample rate of the corresponding stream number
  double getIqStreamSampleRate(size_t streamno) const;

  /// get measurement parameters of the corresponding iq stream number
  IqxStreamDescDataIQ getIqStreamParameters(const std::string& source) const;

  /// @brief get data type of a stream
  IqxStreamType getStreamType(size_t streamNo) const;

  /// @brief get gps update rate
  uint32_t getGpsUpdateRate() const;

  /// @brief get export permission of particular stream
  IqxExportPermission getExportPermission(size_t streamno) const;

  /// @brief get timestamp of a particular sample
  iqx_timespec getTimestampFromSample(size_t streamno, uint64_t sample);

  /// @brief get sample number of a given timestamp
  uint64_t getSampleFromTimestamp(size_t streamno, iqx_timespec timestamp);

  /// @brief get trigger table of a particular stream
  std::vector<IqxTriggerEntry> getTriggers(size_t streamno);

  /// @brief get trigger table of all streams in the recording
  std::vector<IqxTriggerEntry> getAllTriggers();

  /// @brief get corresponding cue entry of a given timestamp
  IqxCueEntry getCueEntry(size_t streamno, iqx_timespec timestamp);

  /// @brief get the next cue entry of a given timestamp. This is needed for partial export: the exported duration must contain the full second!
  IqxCueEntry getNextCueEntry(size_t streamno, iqx_timespec timestamp);

  /// @brief get the uuid of iqxfile
  const std::string& getUuid() const;

  /// @brief get stream types of all streams in file
  std::map<size_t, IqxStreamType> getStreamTypes() const;

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

  /// @brief add a single overrun entry to overrun table
  void addOverrunEntry(IqxOverrunEntry& overrun);

  /// @brief edit name of the recording
  void editRecordingName(const std::string& descname);

  /// @brief edit comment (user text)
  void editComment(const std::string& comment);

  /// @brief edit tags
  void editTags(const std::vector<std::string>& tags);

  /// @brief write a data frame to file
  void writeDataFrame(int64_t streamno, int64_t sequenceno, std::vector<int16_t>& data);

  /// @brief indicate if file has overrun
  bool hasOverrun() const;

private:
  /// pointer to the implementation
  std::unique_ptr<IqxFileImpl> m_pimpl;
};

} // namespace
