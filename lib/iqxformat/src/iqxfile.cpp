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

#include "iqxformat/iqxfile.h"
#ifdef __APPLE__
#include "errno.h"
#endif	

#include "iqxfileimpl.h"
#ifdef _WIN32
#include <io.h>
#include "wincompat.h"
#else
#include <string.h>
#include <unistd.h>
#endif

namespace IQW
{

using namespace std;

bool IqxFile::isIqxFile(const string& filename)
{
  return IqxFileImpl::isIqxFile(filename);
}

IqxFile::IqxFile(const string& filename, bool edit)
  : m_pimpl(new IqxFileImpl(filename, edit))
{}

IqxFile::IqxFile(int fd)
  : m_pimpl(new IqxFileImpl(fd))
{}

IqxFile::IqxFile(int fd, const string& descname, const vector<pair<string, IqxStreamDescDataIQ>>& iqStreams, vector<string> tags, const pair<string, uint32_t>& gpsStream)
  : m_pimpl(new IqxFileImpl(fd, descname, iqStreams, tags, gpsStream))
{}

IqxFile::IqxFile(const string& filename, string applicationName, string comment, const vector<pair<string, IqxStreamDescDataIQ>>& iqStreams)
    : m_pimpl(new IqxFileImpl(filename, applicationName, comment, iqStreams))
{}

IqxFile::~IqxFile()
{}

IqxFile::operator int()
{
  return m_pimpl->operator int();
}

size_t IqxFile::getPayloadOffset() const
{
  return m_pimpl->getPayloadOffset();
}

size_t IqxFile::getEpilogueOffset() const
{
  return m_pimpl->getEpilogueOffset();
}

const string& IqxFile::getDescriptionName() const
{
  return m_pimpl->getDescriptionName();
}

iqx_timespec IqxFile::getStartTime() const
{
  return m_pimpl->getStartTime();
}

long IqxFile::getTZOffset() const
{
  return m_pimpl->getTZOffset();
}

struct iqx_timespec IqxFile::getDuration() const
{
  return m_pimpl->getDuration();
}

const vector<string>& IqxFile::getTags() const
{
  return m_pimpl->getTags();
}

const string& IqxFile::getComment() const
{
  return m_pimpl->getComment();
}

size_t IqxFile::getNumberOfStreams() const
{
  return m_pimpl->getNumberOfStreams();
}

const vector<size_t>& IqxFile::getStreamMaxFrameSizes() const
{
  return m_pimpl->getStreamMaxFrameSizes();
}

size_t IqxFile::getStreamMaxFrameSize(size_t streamno) const
{
  return m_pimpl->getStreamMaxFrameSize(streamno);
}

const vector<string>& IqxFile::getStreamSources() const
{
  return m_pimpl->getStreamSources();
}

const string& IqxFile::getStreamSource(size_t streamno) const
{
  return m_pimpl->getStreamSource(streamno);
}

const vector<size_t>& IqxFile::getIqStreamNumbers() const
{
  return m_pimpl->getIqStreamNumbers();
}

const vector<uint64_t>& IqxFile::getStreamsNoOfFrames() const
{
    return m_pimpl->getStreamsNoOfFrames();
}

uint64_t IqxFile::getStreamNoOfFrames(size_t streamno) const
{
    return m_pimpl->getStreamNoOfFrames(streamno);
}

const vector<uint64_t>& IqxFile::getStreamsNoOfSamples() const
{
    return m_pimpl->getStreamsNoOfSamples();
}

uint64_t IqxFile::getStreamNoOfSamples(size_t streamno) const
{
    return m_pimpl->getStreamNoOfSamples(streamno);
}

size_t IqxFile::getStreamNo(const std::string& streamOrArrayName) const
{
	return m_pimpl->getStreamNo(streamOrArrayName);
}

double IqxFile::getIqStreamDataRate(size_t streamno) const
{
    return m_pimpl->getIqStreamDataRate(streamno);
}

double IqxFile::getIqStreamSampleRate(size_t streamno) const
{
    return m_pimpl->getIqStreamSampleRate(streamno);
}

IqxStreamDescDataIQ IqxFile::getIqStreamParameters(const std::string& source) const
{
    return m_pimpl->getIqStreamParameters(source);
}

int IqxFile::getSequenceNo(size_t streamno) const
{
	return m_pimpl->getSequenceNo(streamno);
}
void IqxFile::setSequenceNo(size_t streamno, int sequenceNo)
{
	m_pimpl->setSequenceNo(streamno, sequenceNo);
}

void IqxFile::writeDataFrame(int64_t streamno, int64_t sequenceno, std::vector<int16_t>& data)
{
	m_pimpl->writeDataFrame(streamno, sequenceno, data);
}

void IqxFile::setDuration(iqx_timespec duration)
{
    m_pimpl->setDuration(duration);
}

void IqxFile::setComment(std::string comment)
{
    m_pimpl->setComment(comment);
}

void IqxFile::setBookmarks(map<string, iqx_timespec> bookmarks)
{
   m_pimpl->setBookmarks(bookmarks);
}

IqxStreamType IqxFile::getStreamType(size_t streamNo) const
{
   return m_pimpl->getStreamType(streamNo);
}

uint32_t IqxFile::getGpsUpdateRate() const
{
  return m_pimpl->getGpsUpdateRate();
}

map<size_t, IqxStreamType> IqxFile::getStreamTypes() const
{
  return m_pimpl->getStreamTypes();
}

void IqxFile::setStartTime(iqx_timespec time)
{
  m_pimpl->setStartTime(time);
}

IqxExportPermission IqxFile::getExportPermission(size_t streamno) const
{
  return m_pimpl->getExportPermission(streamno);
}

iqx_timespec IqxFile::getTimestampFromSample(size_t streamno, uint64_t sample)
{
  return m_pimpl->getTimestampFromSample(streamno, sample);
}

uint64_t IqxFile::getSampleFromTimestamp(size_t streamno, iqx_timespec timestamp)
{
  return m_pimpl->getSampleFromTimestamp(streamno, timestamp);
}

void IqxFile::addTriggerEntry(IqxTriggerEntry& trigger)
{
   return m_pimpl->addTriggerEntry(trigger);
}

vector<IqxTriggerEntry> IqxFile::getTriggers(size_t streamno)
{
  return m_pimpl->getTriggers(streamno);
}

vector<IqxTriggerEntry> IqxFile::getAllTriggers()
{
  return m_pimpl->getAllTriggers();
}

void IqxFile::addCueEntry(IqxCueEntry& cue)
{
  m_pimpl->addCueEntry(cue);
}

IqxCueEntry IqxFile::getCueEntry(size_t streamno, iqx_timespec timestamp)
{
  return m_pimpl->getCueEntry(streamno, timestamp);
}

IqxCueEntry IqxFile::getNextCueEntry(size_t streamno, iqx_timespec timestamp)
{
  return m_pimpl->getNextCueEntry(streamno, timestamp);
}

void IqxFile::editRecordingName(const string& descname)
{
  m_pimpl->editRecordingName(descname);
}

void IqxFile::editComment(const string& comment)
{
  m_pimpl->editComment(comment);
}

void IqxFile::editTags(const vector<string>& tags)
{
  m_pimpl->editTags(tags);
}

const string& IqxFile::getUuid() const
{
  return m_pimpl->getUuid();
}

void IqxFile::addOverrunEntry(IqxOverrunEntry& overrun)
{
  m_pimpl->addOverrunEntry(overrun);
}

bool IqxFile::hasOverrun() const
{
  return m_pimpl->hasOverrun();
}
} // namespace
