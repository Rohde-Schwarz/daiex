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

#include "channelinfo.h"

#include <algorithm>

using namespace std;

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      ChannelInfo::ChannelInfo(const std::string &name, double clock, double centerFreq) :
        name_(name),
        clock_(clock),
        freq_(centerFreq),
        samples_(0)
      {
      }

      ChannelInfo::ChannelInfo(const std::string &name, double clock, double centerFreq, size_t samples) :
        name_(name),
        clock_(clock),
        freq_(centerFreq),
        samples_(samples)
      {
      }

      std::string ChannelInfo::getDefaultChannelName(int channelCount)
      {
        int counter = ++channelCount;
        return "Channel" + to_string(counter);
      }

      std::string ChannelInfo::getDefaultArrayName(int channelCount, bool representI)
      {
        if (representI)
        {
          return ChannelInfo::getDefaultChannelName(channelCount) + "_I";
        }
        else
        {
          return ChannelInfo::getDefaultChannelName(channelCount) + "_Q";
        }
      }

      const std::string& ChannelInfo::getChannelName() const
      {
        return this->name_;
      }

      double ChannelInfo::getClockRate() const
      {
        return this->clock_;
      }

      double ChannelInfo::getFrequency() const
      {
        return this->freq_;
      }

      size_t ChannelInfo::getSamples() const
      {
        return this->samples_;
      }
    }
  }
}