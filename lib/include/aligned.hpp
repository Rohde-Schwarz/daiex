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

#include <stdlib.h>
#include <unistd.h>

#include <new>

namespace IQW
{

template<typename T>
class Aligned : public T
{
public:
  using T::T;

  void* operator new(size_t size)
  {
    void* buf;
    static int alignment = getpagesize();

    if (posix_memalign(&buf, alignment, size))
    {
      throw std::bad_alloc();
    }

    return buf;
  }

  void* operator new[](size_t size)
  {
    return Aligned::operator new(size);
  }

  void operator delete(void* buf)
  {
    free(buf);
  }

  void operator delete[](void* buf)
  {
    Aligned::operator delete(buf);
  }
};
} // namespace
/* @endcond HIDDEN_SYMBOLS */
