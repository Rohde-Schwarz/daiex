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

#include <vector>
#include "iqxformat/iqxtypes.h"

namespace IQW
{

/// @brief structure of a IQX frame
struct IqxFrameData
{
  IqxFrameData();
  ~IqxFrameData();

  /// header of the frame
  void* header_;
  /// data of the frame
  void* data_;
  /// tail of the frame, reserved at the moment
  void* tail_;
};

template<int IFrameType, typename THeader, typename TData, typename TTail>

/// @brief generic class, which can be used for all frame types
class IqxFrame : public IqxFrameData
{
public:
  /// Template for header
  inline THeader& header() {return *reinterpret_cast<THeader*>(header_); }
  /// Temlate for data
  inline TData& data() {return *reinterpret_cast<TData*>(data_); }
  /// Template for tail
  inline TTail& tail() {return *reinterpret_cast<TTail*>(tail_); }
};

typedef struct {} IqxEmpty;
/// file description frame
typedef IqxFrame<IQX_FRAME_TYPE_FILEDESC, IqxFileDescHeader, IqxEmpty, IqxEmpty> IqxFileDescFrame;
/// stream description frame
typedef IqxFrame<IQX_FRAME_TYPE_STREAMDESC, IqxStreamDescHeader, IqxStreamDescDataIQ16, IqxEmpty> IqxStreamDescFrame;
/// user text frame
typedef IqxFrame<IQX_FRAME_TYPE_USERTEXT, IqxUserTextHeader, IqxEmpty, IqxEmpty> IqxUserTextFrame;
/// tag frame
typedef IqxFrame<IQX_FRAME_TYPE_TAG, IqxTagHeader, IqxEmpty, IqxEmpty> IqxTagFrame;
/// bookmark frame
typedef IqxFrame<IQX_FRAME_TYPE_BOOKMARK, IqxBookmarkHeader, IqxEmpty, IqxEmpty> IqxBookmarkFrame;

} // namespace
