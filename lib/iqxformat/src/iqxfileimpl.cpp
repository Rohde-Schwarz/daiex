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

#include "iqxfileimpl.h"
#ifdef __APPLE__
#include "errno.h"
#endif	

#include <cmath>
#include <time.h>
#include <fcntl.h>
#ifdef _WIN32
#include <io.h>
#include "wincompat.h"
#else
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#ifndef _WIN32
#include <uuid/uuid.h>
#endif
#include <cstring>
#include <memory>
#include <stdexcept>
#include <iqxformat/iqxtypes.h>

#include "iqxformat/iqxfile.h"
#include "iqxformat/iqxtypes.h"

namespace IQW
{

using namespace std;

bool IqxFileImpl::isIqxFile(const string& filename)
{
  try
  {
    IqxFileImpl iqxFile(filename);
  }
  catch (const iqxformat_error&)
  {
    return false;
  }
  return true;
}

IqxFileImpl::IqxFileImpl(const string& filename, bool edit)
#ifndef _WIN32
  : m_fd(portable_open(filename.c_str(), (edit == true)? O_RDWR : O_RDONLY))
#else
  : m_fd(portable_open(filename.c_str(), ((edit == true)? O_RDWR : O_RDONLY) | O_BINARY))
#endif
{
  initRead(edit);
}

IqxFileImpl::IqxFileImpl(int fd)
  : m_fd(fd)
  , m_gpsUpdateRate(0)
{
  initRead(false);
}

void IqxFileImpl::initRead(bool edit)
{
  m_write = false;  
  m_edit = edit;
  m_cueNoOfEntries = 0;
  m_trigNoOfEntries = 0;
  m_overrunNoOfEntries = 0;
  m_hasOverrun = false;

  if (m_fd == -1)
  {
    throw iqxformat_error(strerror(errno));
  }

  if (m_edit)
  {
    m_metaOffset.comment = 0;
    m_metaOffset.descname = 0;
    m_metaOffset.eof = 0;
    m_metaOffset.tags.clear();
  }

  struct portable_stat st = {0};
  if (portable_fstat(m_fd, &st))
  {
    throw iqxformat_error(strerror(errno));
  }
  if (!S_ISREG(st.st_mode))
  {
    throw iqxformat_error("file is not regular");
  }

  try
  {
    ALIGNED_VAR(IqxPreamble, preamble);
    readFrame(preamble, m_fileDescFrame);
    assertFrameType(IQX_FRAME_TYPE_FILEDESC, preamble);
    validateFile();
    m_descName = m_fileDescFrame.header().name;
    m_duration = m_fileDescFrame.header().duration;
    readStreamDescriptors();
    readMetaData();
    readIqFrameData();
    portable_lseek(m_fd, getPayloadOffset(), SEEK_SET);
  }
  catch (const iqxformat_error&)
  {
    portable_close(m_fd);
    m_fd = -1;
    throw;
  }
}

IqxFileImpl::IqxFileImpl(const string& filename, string applicationName, string comment, const vector<pair<string, IqxStreamDescDataIQ>>& iqStreams)
#ifdef _WIN32
  : m_fd(portable_open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC | O_BINARY, S_IWRITE | S_IREAD))
#else
  : m_fd(portable_open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IWRITE | S_IREAD))
#endif
{
 initWrite(applicationName, iqStreams, make_pair("", 0));
}

IqxFileImpl::IqxFileImpl(int fd, const string& descname, const vector<pair<string, IqxStreamDescDataIQ>>& iqStreams, vector<string> tags, const pair<string, uint32_t>& gpsStream)
  : m_fd(fd)
{
  initWrite(descname, iqStreams, gpsStream);
  setTags(tags);
}

void IqxFileImpl::initWrite(const string& descname, const vector<pair<string, IqxStreamDescDataIQ>>& iqStreams, const pair<string, uint32_t>& gpsStream)
{
  if (m_fd == -1)
  {
    throw iqxformat_error(strerror(errno));
  }
  m_write = true;
  m_cueNoOfEntries = 0;
  m_trigNoOfEntries = 0;
  m_overrunNoOfEntries = 0;
  ALIGNED_VAR(IqxFileDescHeader, header) = {0};
  header.framesize_min = IQX_FRAMESIZE_MIN;
  header.framesize_max = IQX_FRAMESIZE_MAX;
  bool gps = gpsStream.second ? true : false;
  header.nstreams = gps ? (static_cast<uint32_t>(iqStreams.size()) + 1) : (static_cast<uint32_t>(iqStreams.size()));
  strncpy(header.name, descname.c_str(), IQX_MAX_FILENAME_STRLEN);
  header.name[IQX_MAX_FILENAME_STRLEN-1] = '\0';

#ifdef _WIN32
  // Todo: tm under windows
#else
#ifdef __APPLE__
#else
  struct tm loctime = {0};
  struct timeval startTime;
  if (gettimeofday(&startTime, nullptr) < 0)
  {
    m_startTime = {0};
  }
  m_startTime.tv_sec = startTime.tv_sec;
  m_startTime.tv_nsec = startTime.tv_usec * 1000;
  header.starttime.tv_sec = m_startTime.tv_sec;
  header.starttime.tv_nsec = m_startTime.tv_nsec;

  localtime_r(&header.starttime.tv_sec, &loctime);
  header.timezoneoff = loctime.tm_gmtoff;
  header.daylight = loctime.tm_isdst;
#endif
#endif
  writeFileDescriptionFrame(header, nullptr);

  m_sequenceno.resize(iqStreams.size());
  m_samples.resize(iqStreams.size());
  m_streamSampleRate.resize(iqStreams.size());

  for (size_t i = 0; i < iqStreams.size(); ++i)
  {
    m_sequenceno[i] = 0;
    m_samples[i] = 0;
    if (iqStreams[i].second.samplerate_valid)
    {
      m_streamSampleRate[i] = iqStreams[i].second.samplerate;
    }
    else
    {
      m_streamSampleRate[i] = 0;
    }
    writeIqStreamDescriptionFrame(i, iqStreams[i].first, iqStreams[i].second);
  }
  if(gps)
  {
    writeGpsStreamDescriptionFrame(iqStreams.size(), gpsStream.first, gpsStream.second);
  }
  writePayloadStartFrame();
  m_payloadOffset = portable_lseek(m_fd, 0, SEEK_CUR);
}

IqxFileImpl::~IqxFileImpl()
{
  if (m_fd > 0)
  {
    if (m_write)
    {
      writePayloadEndFrame();
      m_epilogOffset = portable_lseek(m_fd, 0, SEEK_CUR);

      // --- write meta data ----
      for (auto& bookmark : m_bookmarks)
      {
        writeBookmarkFrame((char *)bookmark.first.c_str(), bookmark.second);
      }

      for (string &s : m_tags)
      {
        writeTagFrame((char *)s.c_str());
      }

      // user defined text
      if(!m_comment.empty())
      {
        writeTextFrame((char *)m_comment.c_str());
      }

      writeTriggerFrame();
      writeCueFrame();
      writeOverrunFrame();
      writeEofFrame();
      updateFileDescriptor(0);
    }
    portable_close(m_fd);
    m_fd = -1;
  }
}

IqxFileImpl::operator int()
{
  return m_fd;
}

size_t IqxFileImpl::getPayloadOffset() const
{
  return static_cast<size_t>(m_fileDescFrame.header().payloadoffset);
}

size_t IqxFileImpl::getEpilogueOffset() const
{
  return static_cast<size_t>(m_fileDescFrame.header().epilogoffset);
}

const string& IqxFileImpl::getDescriptionName() const
{
  return m_descName;
}

iqx_timespec IqxFileImpl::getStartTime() const
{
  return m_fileDescFrame.header().starttime;
}

long IqxFileImpl::getTZOffset() const
{
  return m_fileDescFrame.header().timezoneoff;
}

iqx_timespec IqxFileImpl::getDuration() const
{
  return m_fileDescFrame.header().duration;
}

const string& IqxFileImpl::getComment() const
{
  return m_comment;
}

const vector<string>& IqxFileImpl::getTags() const
{
  return m_tags;
}

size_t IqxFileImpl::getNumberOfStreams() const
{
  return m_fileDescFrame.header().nstreams;
}

const vector<size_t>& IqxFileImpl::getStreamMaxFrameSizes() const
{
  return m_streamMaxFrameSizes;
}

size_t IqxFileImpl::getStreamMaxFrameSize(size_t streamno) const
{
  return m_streamMaxFrameSizes.at(streamno);
}

const vector<string>& IqxFileImpl::getStreamSources() const
{
  return m_streamSources;
}

const string& IqxFileImpl::getStreamSource(size_t streamno) const
{
  return m_streamSources.at(streamno);
}

const vector<size_t>& IqxFileImpl::getIqStreamNumbers() const
{
  return m_iqStreamNumbers;
}

void IqxFileImpl::readFrame(IqxPreamble& preamble, IqxFrameData& frame)
{
  if (read(m_fd, &preamble, sizeof(preamble)) < static_cast<ssize_t>(sizeof(preamble)))
  {
    throw iqxformat_error("preample frame incomplete");
  }

  if (memcmp(&preamble.sync, iqxsync, sizeof(iqxsync)) != 0)
  {
    throw iqxformat_error("wrong frame magic number");
  }

  if (preamble.frametype != IQX_FRAME_TYPE_TRIGGER && preamble.frametype != IQX_FRAME_TYPE_CUE && preamble.frametype != IQX_FRAME_TYPE_OVERRUN)
  {
    readToBuffer(frame.header_, preamble.headsize);
    readToBuffer(frame.data_, preamble.datasize);
    readToBuffer(frame.tail_, preamble.framesize - sizeof(IqxPreamble) - preamble.headsize - preamble.datasize);
  }
}

void IqxFileImpl::readToBuffer(void*& buffer, size_t bytes)
{
  if (bytes <= IQX_FRAMESIZE_MAX)
  {
    if (bytes > 0)
    {
#ifndef _WIN32
      buffer = aligned_alloc(IQX_DATA_ALIGNMENT, bytes);
#else
#ifdef __APPLE__
      buffer = malloc(bytes);
#else

      buffer = _aligned_malloc(bytes, IQX_DATA_ALIGNMENT);
#endif
#endif
      if (read(m_fd, buffer, bytes) < static_cast<ssize_t>(bytes) )
      {
        throw iqxformat_error("frame incomplete");
      }
    }
  }
  else
  {
    throw iqxformat_error("frame size too large");
  }
}

void IqxFileImpl::assertFrameType(IqxFrameType frameType, const IqxPreamble& preamble)
{
  if (frameType != preamble.frametype)
  {
    throw iqxformat_error("wrong frame type");
  }
}

void IqxFileImpl::readStreamDescriptors()
{
  m_streamMaxFrameSizes.resize(m_fileDescFrame.header().nstreams);
  m_streamSources.resize(m_fileDescFrame.header().nstreams);

  for (size_t i = 0; i < m_fileDescFrame.header().nstreams; ++i)
  {
    ALIGNED_VAR(IqxPreamble, preamble);
    IqxStreamDescFrame streamDescFrame;
    readFrame(preamble, streamDescFrame);
    assertFrameType(IQX_FRAME_TYPE_STREAMDESC, preamble);
    m_streamMaxFrameSizes[i] = streamDescFrame.header().framesize_max;

    if (streamDescFrame.header().sourcestr[0] == 0)
    {
      m_streamSources[i] = "stream#" + to_string(i);
    }
    else
    {
      m_streamSources[i] = streamDescFrame.header().sourcestr;
    }

    IqxStreamType type = streamDescFrame.header().type;
    if ((type == IQX_STREAM_TYPE_IQDATA16) || (type == IQX_STREAM_TYPE_IQDATA12))
    {
      m_iqStreamNumbers.push_back(i);
      m_streamNo[m_streamSources[i]] = i;
      m_streamNo[m_streamSources[i] + "_I"] = i;
      m_streamNo[m_streamSources[i] + "_Q"] = i;
      m_streamDataRate.push_back(streamDescFrame.header().datarate);
      m_streamSampleRate.push_back(streamDescFrame.data().samplerate);

      if (preamble.datasize >= sizeof(IqxStreamDescDataIQ))
      {
        m_iqProperties.emplace(make_pair(m_streamSources[i], streamDescFrame.data()));
      }
      else
      {
        IqxStreamDescDataIQ iq = {0}; // fill with empty struct if no info available
        m_iqProperties.emplace(make_pair(m_streamSources[i], iq));
      }
    }
    else if(streamDescFrame.header().type == IQX_FRAME_TYPE_GEOLOC)
    {
      m_gpsUpdateRate = static_cast<uint32_t>(streamDescFrame.header().datarate);
    }
    m_streams.emplace(make_pair(i, streamDescFrame.header().type));
  }
}

void IqxFileImpl::readMetaData()
{
  /*int64_t res = */portable_lseek(m_fd, m_fileDescFrame.header().epilogoffset, SEEK_SET);

  for (bool eof = false; !eof;)
  {
    ALIGNED_VAR(IqxPreamble, preamble);
    IqxFrameData frame;

    readFrame(preamble, frame);

    switch (preamble.frametype)
    {
    case IQX_FRAME_TYPE_USERTEXT:
    {
      const IqxUserTextHeader& userText = reinterpret_cast<IqxUserTextFrame*>(&frame)->header();

      if (userText.isvalid)
      {
        m_comment = userText.text;
      }
      if (m_edit)
      {
        m_metaOffset.comment = portable_lseek(m_fd, 0, SEEK_CUR) - preamble.framesize;
      }
      break;
    }

    case IQX_FRAME_TYPE_TAG:
    {
      const IqxTagHeader& tag = reinterpret_cast<IqxTagFrame*>(&frame)->header();
      if (tag.isvalid)
      {
        m_tags.emplace_back(tag.tag);
      }
      if (m_edit)
      {
        m_metaOffset.tags.push_back(portable_lseek(m_fd, 0, SEEK_CUR) - preamble.framesize);
      }
      break;
    }

    case IQX_FRAME_TYPE_BOOKMARK:
    {
      const IqxBookmarkHeader& bookmark = reinterpret_cast<IqxBookmarkFrame*>(&frame)->header();
      if (bookmark.isvalid)
      {
        m_bookmarks.emplace(make_pair(bookmark.text, bookmark.timestamp));
      }
      break;
    }

    case IQX_FRAME_TYPE_TRIGGER:
    {
      readTriggerFrame(preamble);
      break;
    }

    case IQX_FRAME_TYPE_CUE:
    {
      readCueFrame(preamble);
      break;
    }

    case IQX_FRAME_TYPE_OVERRUN:
    {
      m_hasOverrun = true;
      readOverrunFrame(preamble);
      break;
    }

    case IQX_FRAME_TYPE_EOF:
      eof = true;
      if (m_edit)
      {
        m_metaOffset.eof = portable_lseek(m_fd, 0, SEEK_CUR) - preamble.framesize;
      }
      break;

    default:
      break;
    }
  }
}

void IqxFileImpl::readIqFrameData()
{
  m_iqStreamNoOfFrames.resize(m_fileDescFrame.header().nstreams);
  m_iqStreamNoOfSamples.resize(m_fileDescFrame.header().nstreams);
  for (size_t i=0; i< m_fileDescFrame.header().nstreams; i++)
  {
    m_iqStreamNoOfFrames[i] = 0;
    m_iqStreamNoOfSamples[i] = 0;
  }

  // if there is a cur frame, there is no need to inspect all data frames
  if (m_cueNoOfEntries > 0)
  {
    for (size_t i = 0; i< m_fileDescFrame.header().nstreams; i++)
    {
      // calculate the number of samples for iq streams
      IqxStreamType strtype = getStreamType(i);
      if ((strtype == IQX_STREAM_TYPE_IQDATA16) || (strtype == IQX_STREAM_TYPE_IQDATA12))
      {
        m_iqStreamNoOfSamples[i] = getSampleFromTimestamp(i, m_duration);
      }
    }
    for (size_t i = 0; i < m_cueNoOfEntries; i++)
    {
      // calculate the number of frames for all streams
      m_iqStreamNoOfFrames[m_cues[i].streamnum]++;
    }
  }
  else
  {
    iqx_off_t offset = portable_lseek(m_fd, m_fileDescFrame.header().payloadoffset, SEEK_SET);

    for (bool eof = false; !eof;)
    {
      ALIGNED_VAR(IqxPreamble, preamble) = {0};

      if (read(m_fd, &preamble, sizeof(IqxPreamble)) < static_cast<ssize_t>(sizeof(IqxPreamble)) )
      {
        throw iqxformat_error("frame incomplete");
      }

      if (memcmp(preamble.sync, iqxsync, sizeof(iqxsync)) != 0)
      {
        throw iqxformat_error("wrong frame magic number");
      }
      // remeber the filepos and
      // skip rest of frame
      iqx_off_t newOffset = portable_lseek(m_fd, preamble.framesize - sizeof(IqxPreamble), SEEK_CUR);

      switch (preamble.frametype)
      {
      case IQX_FRAME_TYPE_IQDATA:
      {
        // create cue entry, so that mosaik can always workwith cues
        IqxCueEntry cue;
        cue.streamnum = preamble.streamnum;
        cue.offset = offset;
        offset = newOffset;
        cue.timestamp = getTimestampFromSample(static_cast<size_t>(cue.streamnum), m_iqStreamNoOfSamples[preamble.streamnum]);
        addCueEntry(cue);

        // calculate frames/stream samples/stream
        m_iqStreamNoOfFrames[preamble.streamnum]++;

        // calculate number of samples
        IqxStreamType strtype = getStreamType(preamble.streamnum);
        m_iqStreamNoOfSamples[preamble.streamnum] += (strtype == IQX_STREAM_TYPE_IQDATA16) ? (preamble.datasize / 4) : (static_cast<float>(preamble.datasize) / 3 * 240 / 256);
        break;
      }
      case IQX_FRAME_TYPE_PAYLOADEND:
      {
        eof = true;
        break;
      }
      default:
        break;
      }
    }
  }
}

const vector<uint64_t>& IqxFileImpl::getStreamsNoOfFrames() const
{
  return m_iqStreamNoOfFrames;
}

uint64_t IqxFileImpl::getStreamNoOfFrames(size_t streamno) const
{
  return m_iqStreamNoOfFrames.at(streamno);
}

const vector<uint64_t>& IqxFileImpl::getStreamsNoOfSamples() const
{
  return m_iqStreamNoOfSamples;
}

uint64_t IqxFileImpl::getStreamNoOfSamples(size_t streamno) const
{
  return m_iqStreamNoOfSamples.at(streamno);
}

size_t IqxFileImpl::getStreamNo(const string& streamOrArrayName) const
{
  return m_streamNo.at(streamOrArrayName);
}

double IqxFileImpl::getIqStreamDataRate(size_t streamno) const
{
  return m_streamDataRate.at(streamno);
}

double IqxFileImpl::getIqStreamSampleRate(size_t streamno) const
{
  return m_streamSampleRate.at(streamno);
}


void IqxFileImpl::writeFrame(IqxPreamble& preamble, const void* head, const void* data, const void* tail)
{
  memcpy(&preamble.sync[0], iqxsync, sizeof(iqxsync));

  // Frame size if always a multiple of 4k. Tail is zero for now
  preamble.framesize = IQX_PREAMBLESIZE + preamble.headsize + preamble.datasize;

  if (write(m_fd, &preamble, sizeof(IqxPreamble)) < (static_cast<ssize_t>(sizeof(IqxPreamble))))
  {
    throw iqxformat_error(strerror(errno));
  }

  if (head != nullptr)
  {
    if (write(m_fd, head, (size_t)preamble.headsize) < (static_cast<ssize_t>(preamble.headsize)))
    {
      throw iqxformat_error(strerror(errno));
    }
  }
  else
  {
     portable_lseek(m_fd, preamble.headsize, SEEK_CUR);
  }

  if (data != nullptr)
  {
    if (write(m_fd, data, (size_t)preamble.datasize) < (static_cast<ssize_t>(preamble.datasize)))
    {
      throw iqxformat_error(strerror(errno));
    }
  }
  else
  {
    portable_lseek(m_fd, preamble.datasize, SEEK_CUR);
  }
}

void IqxFileImpl::writeFileDescriptionFrame(IqxFileDescHeader& header, const void* data)
{
  ALIGNED_VAR(IqxPreamble, preamble) = {0};
  preamble.streamnum = IQX_STREAM_INDEPENDENT;
  preamble.datasize = 0;
  preamble.headsize = portable_max(sizeof(IqxFileDescHeader), static_cast<size_t>(IQX_HEADSIZE_MIN));
  preamble.frametype = IQX_FRAME_TYPE_FILEDESC;
  header.iqxversion = IQX_VERSION;
  header.iqxreadversion = IQX_MINREADVERSION;
#ifdef _WIN32
  UUID uuid;
  UuidCreate(&uuid);
#else
  uuid_generate(header.uuid);
  // save uuid to class attribute
  char uuidstr[37] = "\0";
  uuid_unparse(header.uuid, uuidstr);
  m_uuid = string(uuidstr);
#endif
  writeFrame(preamble, &header, data, nullptr);
}

void IqxFileImpl::writeIqStreamDescriptionFrame(size_t streamNumber, const string& streamName, const IqxStreamDescDataIQ& iqStreamDescriptor)
{
  ALIGNED_VAR(IqxPreamble, preamble) = {0};
  preamble.datasize = sizeof(IqxStreamDescDataIQ);
  preamble.headsize = portable_max(sizeof(IqxStreamDescHeader), static_cast<size_t>(IQX_HEADSIZE_MIN));
  preamble.frametype = IQX_FRAME_TYPE_STREAMDESC;
  preamble.streamnum = static_cast<int32_t>(streamNumber);

  ALIGNED_VAR(IqxStreamDescHeader, header) = {0};
  strncpy(header.sourcestr, streamName.c_str(), IQX_MAX_SOURCENAME_STRLEN);
  header.sourcestr[IQX_MAX_SOURCENAME_STRLEN - 1] = '\0';  
  double bytesPerSample = iqStreamDescriptor.resolution * 2 / 8;
  header.datarate = static_cast<double>(iqStreamDescriptor.samplerate) * ((iqStreamDescriptor.resolution == 12) ? (bytesPerSample * 256/240) : bytesPerSample); // data rate = bytes/s. sample rate = samples/s. 1 sample = 4 bytes (16I + 16Q) or 1 sample = 3 bytes (12I + 12Q)
  header.framerate = 0;
  header.framesize_min = preamble.framesize; // TODO
  header.framesize_max = preamble.framesize; // TODO
  header.type = (iqStreamDescriptor.resolution == 16) ? IQX_STREAM_TYPE_IQDATA16 : IQX_STREAM_TYPE_IQDATA12;

  ALIGNED_VAR(IqxStreamDescDataIQ, iqdata) = {0};
  memcpy(&iqdata, &iqStreamDescriptor, sizeof(IqxStreamDescDataIQ));

  writeFrame(preamble, &header, &iqdata, nullptr);
}

void IqxFileImpl::writeGpsStreamDescriptionFrame(size_t streamNumber, const string& streamName, uint32_t updateRate)
{
  ALIGNED_VAR(IqxPreamble, preamble) = {0};
  preamble.datasize = 0;
  preamble.headsize = portable_max(sizeof(IqxStreamDescHeader), static_cast<size_t>(IQX_HEADSIZE_MIN));
  preamble.frametype = IQX_FRAME_TYPE_STREAMDESC;
  preamble.streamnum = static_cast<int32_t>(streamNumber);

  ALIGNED_VAR(IqxStreamDescHeader, header) = {0};
  strncpy(header.sourcestr, streamName.c_str(), IQX_MAX_SOURCENAME_STRLEN);
  header.sourcestr[IQX_MAX_SOURCENAME_STRLEN - 1] = '\0';
  header.datarate = static_cast<double>(updateRate);
  header.framerate = 0;
  header.framesize_min = preamble.framesize; // TODO
  header.framesize_max = preamble.framesize; // TODO
  header.type = IQX_STREAM_TYPE_GEOLOC;

  writeFrame(preamble, &header, nullptr, nullptr);
}

void IqxFileImpl::writePayloadStartFrame()
{
  ALIGNED_VAR(IqxPreamble, preamble) = {0};
  memcpy(&preamble.sync[0], iqxsync, sizeof(iqxsync));
  preamble.streamnum = IQX_STREAM_INDEPENDENT;
  preamble.datasize = 0;
  preamble.headsize = IQX_HEADSIZE_MIN;
  preamble.frametype = IQX_FRAME_TYPE_PAYLOADSTART;

  writeFrame(preamble, nullptr, nullptr, nullptr);
}

void IqxFileImpl::writePayloadEndFrame()
{
  ALIGNED_VAR(IqxPreamble, preamble) = {0};
  memcpy(&preamble.sync[0], iqxsync, sizeof(iqxsync));
  preamble.streamnum = IQX_STREAM_INDEPENDENT;
  preamble.datasize = 0;
  preamble.headsize = IQX_HEADSIZE_MIN;
  preamble.frametype = IQX_FRAME_TYPE_PAYLOADEND;
  writeFrame(preamble, nullptr, nullptr, nullptr);
}

void IqxFileImpl::writeEofFrame()
{
  ALIGNED_VAR(IqxPreamble, preamble) = {0};
  memcpy(&preamble.sync[0], iqxsync, sizeof(iqxsync));
  preamble.streamnum = IQX_STREAM_INDEPENDENT;
  preamble.datasize = 0;
  preamble.headsize = IQX_HEADSIZE_MIN;
  preamble.frametype = IQX_FRAME_TYPE_EOF;
  ALIGNED_VAR(IqxEofHeader, header) = {0};
  //header.eof = '\0';
  writeFrame(preamble, &header, nullptr, nullptr);
}

void IqxFileImpl::writeDataFrame(int64_t streamno, int64_t sequenceno, vector<int16_t>& data)
{
  ALIGNED_VAR(IqxPreamble, preamble) = {0};
  memcpy(&preamble.sync[0], iqxsync, sizeof(iqxsync));
  preamble.streamnum = static_cast<int32_t>(streamno);
  preamble.datasize = data.size() * 2;
  preamble.frametype = IQX_FRAME_TYPE_IQDATA;
  preamble.headsize =  max(sizeof(IqxIqDataHeader), static_cast<size_t>(IQX_HEADSIZE_MIN));
  //preamble.previousframesize = ;
  preamble.timestamp = m_duration;
  m_samples[streamno] += data.size() / 2;
  if (m_streamSampleRate[streamno] > 0)
  {
    float64_t duration = m_samples[streamno] / m_streamSampleRate[streamno];
    double seconds = 0.0;
    m_duration.tv_nsec = static_cast<int64_t>(modf(duration, &seconds) * 1.0e9);
    m_duration.tv_sec = static_cast<int64_t>(seconds);
  }
  ALIGNED_VAR(IqxIqDataHeader, datahead) = {0};
  datahead.sequencenum = sequenceno;
  writeFrame(preamble, &datahead, &data[0], nullptr);
}

void IqxFileImpl::writeTagFrame(const char* tag)
{
  ALIGNED_VAR(IqxPreamble, preamble) = {0};
  ALIGNED_VAR(IqxTagHeader, taghead) = {0};
  memcpy(&preamble.sync[0], iqxsync, sizeof(iqxsync));
  preamble.streamnum = IQX_STREAM_INDEPENDENT;
  preamble.datasize = 0;
  preamble.headsize = max(sizeof(IqxTagHeader), static_cast<size_t>(IQX_HEADSIZE_MIN));
  preamble.frametype = IQX_FRAME_TYPE_TAG;

  // fill tag fields
  strncpy(taghead.tag, tag, IQX_MAX_TAG_STRLEN);
  taghead.tag[IQX_MAX_TAG_STRLEN - 1] = '\0';
  taghead.isvalid = IQX_BOOL_TRUE;
  writeFrame(preamble, &taghead, nullptr, nullptr);
}

void IqxFileImpl::writeTextFrame(const char* text)
{
  ALIGNED_VAR(IqxPreamble, preamble) = {0};
  ALIGNED_VAR(IqxUserTextHeader, texthead) = {0};
  memcpy(&preamble.sync[0], iqxsync, sizeof(iqxsync));
  preamble.streamnum = IQX_STREAM_INDEPENDENT;
  preamble.datasize = 0;
  preamble.headsize = max(sizeof(IqxUserTextHeader), static_cast<size_t>(IQX_HEADSIZE_MIN));
  preamble.frametype = IQX_FRAME_TYPE_USERTEXT;

  // fill text fields
  strncpy(texthead.text, text, IQX_MAX_USERTEXT_STRLEN);
  texthead.text[IQX_MAX_USERTEXT_STRLEN - 1] = '\0';
  texthead.isvalid = IQX_BOOL_TRUE;

  writeFrame(preamble, &texthead, nullptr, nullptr);
}

void IqxFileImpl::writeBookmarkFrame(const char* bookmark, iqx_timespec timestamp)
{
  ALIGNED_VAR(IqxPreamble, preamble) = {0};
  ALIGNED_VAR(IqxBookmarkHeader, bookmarkhead) = {0};
  memcpy(&preamble.sync[0], iqxsync, sizeof(iqxsync));
  preamble.streamnum = IQX_STREAM_INDEPENDENT;
  preamble.datasize = 0;
  preamble.headsize = max(sizeof(IqxBookmarkHeader), static_cast<size_t>(IQX_HEADSIZE_MIN));
  preamble.frametype = IQX_FRAME_TYPE_BOOKMARK;

  // fill bookmark fields
  strncpy(bookmarkhead.text, bookmark, IQX_MAX_BOOKMARK_STRLEN);
  bookmarkhead.text[IQX_MAX_BOOKMARK_STRLEN - 1] = '\0';
  bookmarkhead.timestamp.tv_sec = timestamp.tv_sec;
  bookmarkhead.timestamp.tv_nsec = timestamp.tv_nsec;
  bookmarkhead.isvalid = IQX_BOOL_TRUE;
  writeFrame(preamble, &bookmarkhead, nullptr, nullptr);
}

void IqxFileImpl::updateFileDescriptor(int32_t chunknext)
{
  ALIGNED_VAR(IqxPreamble, preamble) = {0};
  ALIGNED_VAR(IqxFileDescHeader, filedhead) = {0};

  // save current file position 
  int64_t lastpos = portable_lseek(m_fd, 0, SEEK_CUR);

  // and seek to file description frame
  portable_lseek(m_fd, 0, SEEK_SET);

  // get preamble
  if (read(m_fd, &preamble, sizeof(IqxPreamble)) < (static_cast<ssize_t>(sizeof(IqxPreamble))))
  {
    throw iqxformat_error("incomplete size of preamble");
  }
  if (preamble.frametype != IQX_FRAME_TYPE_FILEDESC)
  {
    throw iqxformat_error("wrong frame type: expecting file descriptor frame");
  }
  if (memcmp(&preamble.sync, iqxsync, sizeof(iqxsync)) != 0)
  {
    throw iqxformat_error("wrong magic number in preamble of file description frame");
  }

  // get file description
  if (read(m_fd, &filedhead, sizeof(IqxFileDescHeader)) < (static_cast<ssize_t>(sizeof(IqxFileDescHeader))))
  {
      throw iqxformat_error("could not read file description header");
  }
  filedhead.payloadoffset = m_payloadOffset;
  filedhead.epilogoffset = m_epilogOffset;
  memcpy(&filedhead.duration, &m_duration, sizeof(struct iqx_timespec));
  memcpy(&filedhead.starttime, &m_startTime, sizeof(m_startTime));
  filedhead.chunknext = chunknext;
  filedhead.complete = 1;

  // write back preamble and frame description;
  portable_lseek(m_fd, 0, SEEK_SET);
  if (write(m_fd, &preamble, sizeof(IqxPreamble)) < (static_cast<ssize_t>(sizeof(IqxPreamble))))
  {
      throw iqxformat_error("could not update description header preamble");
  }
  if (write(m_fd, &filedhead, sizeof(IqxFileDescHeader)) < (static_cast<ssize_t>(sizeof(IqxFileDescHeader))))
  {
      throw iqxformat_error("could not update description header");
  }
  // seek to previous position
  portable_lseek(m_fd, lastpos, SEEK_SET);
}

IqxStreamDescDataIQ IqxFileImpl::getIqStreamParameters(const string& source) const
{
  return m_iqProperties.at(source);
}

int IqxFileImpl::getSequenceNo(size_t streamno) const
{return m_sequenceno[streamno];
}

void IqxFileImpl::setSequenceNo(size_t streamno, int sequenceNo)
{
  m_sequenceno[streamno] = sequenceNo;
}

void IqxFileImpl::setDuration(iqx_timespec duration)
{
  m_duration = duration;
}

void IqxFileImpl::setTags(vector<string>tags)
{
  m_tags = tags;
}

void IqxFileImpl::setComment(string comment)
{
  if (m_comment.empty())
  {
    m_comment = comment;
  }
  else
  {
    m_comment += "; " + comment;
  }
}

void IqxFileImpl::setBookmarks(map<string, iqx_timespec> bookmarks)
{
  m_bookmarks = bookmarks;
}

void IqxFileImpl::validateFile()
{
  if((!m_fileDescFrame.header().complete) || (!m_fileDescFrame.header().epilogoffset) || (!m_fileDescFrame.header().payloadoffset))
  {
     throw iqxformat_error("file is corrupted");
  }
  if (!m_fileDescFrame.header().duration.tv_sec && !m_fileDescFrame.header().duration.tv_nsec)
  {
     throw iqxformat_error("recording duration is zero");
  }
}

IqxStreamType IqxFileImpl::getStreamType(size_t streamNo) const
{
  return m_streams.at(streamNo);
}

uint32_t IqxFileImpl::getGpsUpdateRate() const
{
   return m_gpsUpdateRate;
}

map<size_t, IqxStreamType> IqxFileImpl::getStreamTypes() const
{
    return m_streams;
}

void IqxFileImpl::setStartTime(iqx_timespec time)
{
   memcpy(&m_startTime, &time, sizeof(m_startTime));
}

IqxExportPermission IqxFileImpl::getExportPermission(size_t streamno) const
{
  IqxStreamDescDataIQ iqdata = m_iqProperties.at(getStreamSource(streamno));
  return (iqdata.permission_valid ? iqdata.export_permission : IQX_EXPORT_PERMISSION_UNKNOWN);
}

void IqxFileImpl::addTriggerEntry(IqxTriggerEntry& trigger)
{
  if (m_triggers.size() < MAX_TRIGGER_ENTRIES)
  {
    m_triggers.push_back(trigger);
    ++m_trigNoOfEntries;
  }
}

void IqxFileImpl::writeTriggerFrame()
{
  if (m_trigNoOfEntries == 0)
  {
    return;
  }
  ALIGNED_VAR(IqxPreamble, preamble) = {0};
  ALIGNED_VAR(IqxTriggerHeader, triggerhead) = {0};
  memcpy(&preamble.sync[0], iqxsync, sizeof(iqxsync));
  preamble.streamnum = IQX_STREAM_INDEPENDENT;
  preamble.headsize = max(sizeof(IqxTriggerHeader), static_cast<size_t>(IQX_HEADSIZE_MIN));
  preamble.frametype = IQX_FRAME_TYPE_TRIGGER;

  // fill trigger header
  triggerhead.numentries = m_trigNoOfEntries;

  // calculate data size and make it align to 4k
  uint64_t sizebytes = m_trigNoOfEntries * sizeof(IqxTriggerEntry);
  uint64_t numblocks = ((sizebytes % IQX_DATA_ALIGNMENT) > 0.0) ? ((sizebytes/IQX_DATA_ALIGNMENT)+1) : (sizebytes/IQX_DATA_ALIGNMENT);
  preamble.datasize = numblocks * IQX_DATA_ALIGNMENT;

  uint64_t zeroEntries = (preamble.datasize - sizebytes) / sizeof(IqxTriggerEntry);
  IqxTriggerEntry zero = {0};
  for (size_t i = 0; i < zeroEntries; ++i)
  {
    m_triggers.push_back(zero);
  }
  writeFrame(preamble, &triggerhead, &m_triggers[0], nullptr);
}

iqx_timespec IqxFileImpl::getTimestampFromSample(size_t streamno, uint64_t sample)
{
  iqx_timespec timets;
  double timefp;

  IqxStreamDescDataIQ iqstream = getIqStreamParameters(getStreamSource(streamno));
  timefp = sample / iqstream.samplerate;
  timets.tv_sec = (int64_t) timefp;
  timets.tv_nsec = (int64_t) ((timefp - timets.tv_sec) * 1e9 + .5);
  return timets;
}

uint64_t IqxFileImpl::getSampleFromTimestamp(size_t streamno, iqx_timespec timestamp)
{
    uint64_t sample;
    double timefp;

    IqxStreamDescDataIQ iqstream = getIqStreamParameters(getStreamSource(streamno));
    timefp = timestamp.tv_sec + timestamp.tv_nsec * 1e-9;
    sample = (uint64_t) (iqstream.samplerate * timefp + .5);    
    return sample;
}

void IqxFileImpl::readTriggerFrame(IqxPreamble& preamble)
{
  ALIGNED_VAR(IqxTriggerHeader, header) = {0};
  if (read(m_fd, &header, sizeof(header)) < static_cast<ssize_t>(sizeof(header)))
  {
    throw iqxformat_error("trigger header incomplete");
  }
  m_trigNoOfEntries = header.numentries;
  m_triggers.resize(preamble.datasize/sizeof(IqxTriggerEntry));

  if (portable_read(m_fd, &m_triggers[0], preamble.datasize) < static_cast<ssize_t>(preamble.datasize))
  {
    throw iqxformat_error("trigger data incomplete");
  }
}

void IqxFileImpl::readCueFrame(IqxPreamble& preamble)
{
  ALIGNED_VAR(IqxCueHeader, header) = { 0 };
  if (read(m_fd, &header, sizeof(header)) < static_cast<ssize_t>(sizeof(header)))
  {
    throw iqxformat_error("cue header incomplete");
  }
  m_cueNoOfEntries = header.numentries;
  m_cues.resize(preamble.datasize/sizeof(IqxCueEntry));
  if (portable_read(m_fd, &m_cues[0], preamble.datasize) < static_cast<ssize_t>(preamble.datasize))
  {
    throw iqxformat_error("cue data incomplete");
  }
}

vector<IqxTriggerEntry> IqxFileImpl::getTriggers(size_t streamno)
{
  vector<IqxTriggerEntry> result;

  for (size_t i = 0; i < m_trigNoOfEntries; ++i)
  {
    if (static_cast<size_t>(m_triggers[i].streamnum) == streamno)
    {
      result.push_back(m_triggers[i]);
    }
  }
  return result;
}

vector<IqxTriggerEntry> IqxFileImpl::getAllTriggers() const
{
  vector<IqxTriggerEntry> result;
  for (size_t i = 0; i < m_trigNoOfEntries; ++i)
  {
    result.push_back(m_triggers[i]);
  }
  return result;
}

void IqxFileImpl::addCueEntry(IqxCueEntry& cue)
{  
  if (m_cueNoOfEntries < MAX_CUE_ENTRIES)
  {
    m_cues.push_back(cue);
    ++m_cueNoOfEntries;
  }
}

vector<IqxCueEntry> IqxFileImpl::getCues(size_t streamno)
{
  vector<IqxCueEntry> result;

  for (size_t i = 0; i < m_cueNoOfEntries; ++i)
  {
    if (static_cast<size_t>(m_cues[i].streamnum) == streamno)
    {
      result.push_back(m_cues[i]);
    }
  }
  return result;
}

IqxCueEntry IqxFileImpl::getCueEntry(size_t streamno, iqx_timespec timestamp)
{
  double time = timestamp.tv_sec + timestamp.tv_nsec * 1e-9;
  vector<IqxCueEntry> cues = getCues(streamno);
  for (int64_t i = m_cueNoOfEntries - 1; i >= 0; i--)
  {
    double timeOfFrame = cues[i].timestamp.tv_sec + cues[i].timestamp.tv_nsec * 1e-9;
    if (time >= timeOfFrame)
    {
      return cues[i];
    }
  }
  throw iqxformat_error("no matching cue entry found at given timestamp");
}

IqxCueEntry IqxFileImpl::getNextCueEntry(size_t streamno, iqx_timespec timestamp)
{
  double time = timestamp.tv_sec + timestamp.tv_nsec * 1e-9;
  vector<IqxCueEntry> cues = getCues(streamno);
  for (size_t i = 0; i < m_cueNoOfEntries-1; i++)
  {
    double timeOfFrame = cues[i].timestamp.tv_sec + cues[i].timestamp.tv_nsec * 1e-9;
    double timeOfNextFrame = cues[i+1].timestamp.tv_sec + cues[i+1].timestamp.tv_nsec * 1e-9;
    if ((time >= timeOfFrame) && (time < timeOfNextFrame))
    {
      return cues[i+1];
    }
  }
  throw iqxformat_error("no matching cue entry found at given timestamp");
}

void IqxFileImpl::writeCueFrame()
{
  if (m_cueNoOfEntries == 0)
  {
    return;
  }
  ALIGNED_VAR(IqxPreamble, preamble) = {0};
  ALIGNED_VAR(IqxCueHeader, cuehead) = {0};
  memcpy(&preamble.sync[0], iqxsync, sizeof(iqxsync));
  preamble.streamnum = IQX_STREAM_INDEPENDENT;
  preamble.headsize = max(sizeof(IqxCueHeader), static_cast<size_t>(IQX_HEADSIZE_MIN));
  preamble.frametype = IQX_FRAME_TYPE_CUE;

  // fill cue header
  cuehead.numentries = m_cueNoOfEntries;

  uint64_t sizebytes = m_cueNoOfEntries * sizeof(IqxCueEntry);
  uint64_t numblocks = ((sizebytes % IQX_DATA_ALIGNMENT) > 0.0) ? ((sizebytes/IQX_DATA_ALIGNMENT)+1) : (sizebytes/IQX_DATA_ALIGNMENT);
  preamble.datasize = numblocks * IQX_DATA_ALIGNMENT;

  uint64_t zeroEntries = (preamble.datasize - sizebytes) / sizeof(IqxCueEntry);
  IqxCueEntry zero = {0};
  for (size_t i = 0; i < zeroEntries; ++i)
  {
    m_cues.push_back(zero);
  }
  writeFrame(preamble, &cuehead, &m_cues[0], nullptr);
}

void IqxFileImpl::editRecordingName(const string& descname)
{ 
  portable_lseek(m_fd, 0, SEEK_SET);
  snprintf(m_fileDescFrame.header().name, IQX_MAX_FILENAME_STRLEN, "%s", descname.c_str());
  m_fileDescFrame.header().name[IQX_MAX_FILENAME_STRLEN-1] = '\0';
  writeFileDescriptionFrame(m_fileDescFrame.header(), nullptr);
}

void IqxFileImpl::editComment(const string& comment)
{
  if (!m_comment.empty())
  {
    portable_lseek(m_fd, m_metaOffset.comment, SEEK_SET);
    writeTextFrame(comment.c_str());
  }
  else // no comment frame in file
  {
    portable_lseek(m_fd, m_metaOffset.eof, SEEK_SET);
    writeTextFrame(comment.c_str());
    m_metaOffset.eof = portable_lseek(m_fd, 0, SEEK_CUR);
    writeEofFrame();
  }
  m_comment = comment;
}

void IqxFileImpl::editTags(const vector<string>& tags)
{
  size_t existing_size = m_tags.size();
  size_t new_size = tags.size();

  if (existing_size) // tag frame(s) exist
  {
    if (existing_size < new_size) // overwrite existing ones, add new tags at eof, rewrite eof
    {
      overwriteExistingTags(tags, existing_size);
      portable_lseek(m_fd, m_metaOffset.eof, SEEK_SET);
      for (size_t i = existing_size; i < new_size; ++i)
      {
        writeTagFrame(tags[i].c_str());
      }
      writeEofFrame();
    }
    else if (existing_size > new_size) // overwrite existing ones, invalidate the rest
    {
      overwriteExistingTags(tags, new_size);
      for (size_t i = new_size; i < existing_size; ++i)
      {
        portable_lseek(m_fd, m_metaOffset.tags[i], SEEK_SET);
        invalidateExistingTag();
      }
    }
    else // same size: overwrite existing tags
    {
      overwriteExistingTags(tags, existing_size);
    }
  }
  else // tag frame does not exist in file: write tag at eof frame and rewrite eof
  {
    portable_lseek(m_fd, m_metaOffset.eof, SEEK_SET);
    for (auto & tag : tags)
    {
      writeTagFrame(tag.c_str());
    }
    writeEofFrame();
  }
  m_tags.resize(new_size);
  m_tags = tags;
}

const string& IqxFileImpl::getUuid() const
{
  return m_uuid;
}

void IqxFileImpl::overwriteExistingTags(const vector<string>& tags, size_t size)
{
  for (size_t i = 0; i < size; ++i)
  {
    portable_lseek(m_fd, m_metaOffset.tags[i], SEEK_SET);
    writeTagFrame(tags[i].c_str());
  }
}

void IqxFileImpl::invalidateExistingTag()
{
  size_t offset = portable_lseek(m_fd, 0, SEEK_CUR);
  ALIGNED_VAR(IqxPreamble, preamble) = {0};
  ALIGNED_VAR(IqxTagHeader, header) = {0};
  IqxFrameData frame;
  readFrame(preamble, frame);
  header.isvalid = IQX_BOOL_FALSE;
  portable_lseek(m_fd, offset, SEEK_SET);
  writeFrame(preamble, &header, nullptr, nullptr);
}

void IqxFileImpl::addOverrunEntry(IqxOverrunEntry& overrun)
{
  if (m_overruns.size() < MAX_OVERRUN_ENTRIES)
  {
    m_overruns.push_back(overrun);
    ++m_overrunNoOfEntries;
  }
}

void IqxFileImpl::writeOverrunFrame()
{
  if (m_overrunNoOfEntries == 0)
  {
    return;
  }
  ALIGNED_VAR(IqxPreamble, preamble) = {0};
  ALIGNED_VAR(IqxOverrunHeader, overrunhead) = {0};
  memcpy(&preamble.sync[0], iqxsync, sizeof(iqxsync));
  preamble.streamnum = IQX_STREAM_INDEPENDENT;
  preamble.headsize = max(sizeof(IqxOverrunHeader), static_cast<size_t>(IQX_HEADSIZE_MIN));
  preamble.frametype = IQX_FRAME_TYPE_OVERRUN;

  // fill overrun header
  overrunhead.numentries = m_overrunNoOfEntries;

  // calculate data size and make it align to 4k
  uint64_t sizebytes = m_overrunNoOfEntries * sizeof(IqxOverrunEntry);
  uint64_t numblocks = ((sizebytes % IQX_DATA_ALIGNMENT) > 0.0) ? ((sizebytes/IQX_DATA_ALIGNMENT)+1) : (sizebytes/IQX_DATA_ALIGNMENT);
  preamble.datasize = numblocks * IQX_DATA_ALIGNMENT;

  uint64_t zeroEntries = (preamble.datasize - sizebytes) / sizeof(IqxOverrunEntry);
  IqxOverrunEntry zero = {0};
  for (size_t i = 0; i < zeroEntries; ++i)
  {
    m_overruns.push_back(zero);
  }
  writeFrame(preamble, &overrunhead, &m_overruns[0], nullptr);
}

void IqxFileImpl::readOverrunFrame(IqxPreamble& preamble)
{
  ALIGNED_VAR(IqxOverrunHeader, header) = {0};
  if (read(m_fd, &header, sizeof(header)) < static_cast<ssize_t>(sizeof(header)))
  {
    throw iqxformat_error("overrun header incomplete");
  }
  m_overrunNoOfEntries = header.numentries;
  m_overruns.resize(preamble.datasize/sizeof(IqxOverrunEntry));

  if (portable_read(m_fd, &m_overruns[0], preamble.datasize) < static_cast<ssize_t>(preamble.datasize))
  {
    throw iqxformat_error("overrun data incomplete");
  }
}

bool IqxFileImpl::hasOverrun() const
{
  return m_hasOverrun;
}
} // namespace
