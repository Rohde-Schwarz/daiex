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

/* @cond HIDDEN_SYMBOLS */

/*!
* @file      stride_iterator.h
*
* @brief     This is the header file of class stride_iterator. The iterator returns 
*            every n-th element of the input array.
*
* @details   Stride iterator with configurable step width.
*
* @copyright Copyright (c) Rohde &amp; Schwarz GmbH &amp; Co. KG, Munich.
*            All rights reserved.
*/

#pragma once

#include <iterator>
#include <cassert>

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /**
      * @brief Stride iterator with configurable step width. The iterator returns 
      * every n-th element of the input array.
      */
      template<class T>
      class stride_iterator
      {
      public:
        typedef typename std::iterator_traits<T>::value_type value_type;
        typedef typename std::iterator_traits<T>::reference reference;
        typedef typename std::iterator_traits<T>::difference_type difference_type;
        typedef typename std::iterator_traits<T>::pointer pointer;
        typedef std::random_access_iterator_tag iterator_category;
        typedef stride_iterator self;

        stride_iterator() : m_(nullptr), step_(0) {}
        stride_iterator(const self& x) : m_(x.m_), step_(x.step_) {}
        stride_iterator(T x, difference_type n) : m_(x), step_(n) {}

        self& operator++()
        {
          this->m_ += this->step_;
          return *this;
        }

        self operator++(int)
        {
          self tmp = *this;
          this->m_ += this->step_;
          return tmp;
        }

        self& operator+=(difference_type x)
        {
          this->m_ += x * this->step_;
          return *this;
        }

        self& operator--( )
        {
          this->m_ -= this->step_;
          return *this;
        }

        self operator--(int)
        {
          self tmp = *this;
          this->m_ -= this->step_;
          return tmp;
        }

        self& operator-=(difference_type x)
        {
          this->m_ -= x * this->step_;
          return *this;
        }

        reference operator[](difference_type n)
        {
          return this->m_[n * this->step_];
        }

        reference operator*()
        {
          return *this->m_;
        }

        friend bool operator==(const self& x, const self& y)
        {
          assert(x.step_ == y.step_);
          return x.m_ == y.m_;
        }

        friend bool operator!=(const self& x, const self& y)
        {
          assert(x.step_ == y.step_);
          return x.m_ != y.m_;
        }

        friend bool operator<(const self& x, const self& y)
        {
          assert(x.step_ == y.step_);
          return x.m_ < y.m_;
        }

        friend difference_type operator-(const self& x, const self& y)
        {
          assert(x.step_ == y.step_);
          return (x.m_ - y.m_) / x.step_;
        }

        friend self operator+(const self& x, difference_type y)
        {
          assert(x.step_ == y.step_);
          return x += y * x.step_;
        }

        friend self operator+(difference_type x, const self& y)
        {
          assert(x.step == y.step);
          return y += x * x.step_;
        }

      private:
        T m_;
        difference_type step_;
      };
    }
  }
}

/*** @endcond ***/