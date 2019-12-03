#include "channelinfo_wrapper.h"

#include "helpers.h"

using namespace std;

namespace RohdeSchwarz
{
  namespace Mosaik
  {
    namespace DataImportExport
    {
      ChannelInfo::ChannelInfo(System::String ^name, double clock, double centerFreq)
      {
        this->nativeImpl_ = new rohdeschwarz::mosaik::dataimportexport::ChannelInfo(Helpers::marshalToUTF8String(name), clock, centerFreq);
      }

      ChannelInfo::~ChannelInfo()
      {
        this->!ChannelInfo();
      }

      ChannelInfo::!ChannelInfo()
      {
        delete this->nativeImpl_;
      }

      System::String^ ChannelInfo::ChannelName::get() 
      {
        return Helpers::marshalUTF8String(this->nativeImpl_->getChannelName());
      }

      double ChannelInfo::ClockRate::get() 
      {
        return this->nativeImpl_->getClockRate();
      }

      double ChannelInfo::Frequency::get()
      {
        return this->nativeImpl_->getFrequency();
      }

      size_t ChannelInfo::Samples::get()
      {
        return this->nativeImpl_->getSamples();
      }
    }
  }
}