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

#include "iqxframe.h"

#include <stdlib.h>

#ifdef WIN32
#define aligned_free(p) _aligned_free(p);
#else
#define aligned_free(p) free(p);
#endif

namespace IQW
{

  IqxFrameData::IqxFrameData()
  : header_(nullptr)
  , data_(nullptr)
  , tail_(nullptr)
  {}

  IqxFrameData::~IqxFrameData()
  {
    if (header_ != nullptr)
    {
      aligned_free(header_);
      header_ = nullptr;
    }
    if (data_ != nullptr)
    {
      aligned_free(data_);
      data_ = nullptr;
    }
    if (tail_ != nullptr)
    {
      aligned_free(tail_);
      tail_ = nullptr;
    }
  }

} // namespace