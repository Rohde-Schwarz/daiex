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

/*******************************************************************************/
/**
@file
@copyright     (c) Rohde & Schwarz GmbH & Co. KG, Munich
@version       $Workfile: ringbuffer.h $
*
@language      ANSI C++
*
@description   Ringbuffer with possibility to read samples from the past and to shift readpointer manually
*
@see
*
@history
*
*******************************************************************************/

#pragma once

#ifdef _MSC_VER
__pragma(warning(push))
__pragma(warning(disable: 4244))
__pragma(warning(disable: 4996))
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic warning "-w"
#endif

namespace rohdeschwarz
{
  namespace mosaik
  {
    namespace dataimportexport
    {
      /* CLASS DECLARATION **********************************************************/
      /**
      Ringbuffer with possibility to read samples from the past and to shift readpointer manually
      *
      @version       $Workfile: ringbuffer.h $.
      *
      *******************************************************************************/
      template <typename T_PRECISION> class CRingBuffer
      {
      public:

        /// Default constructor.
        CRingBuffer();

        /* */
        /**
        Parameterized constructor
        *
        @param a_iRingBufferSize: size of Ringbuffer
        *
        *******************************************************************************/
        CRingBuffer(const int a_iRingBufferSize);

        /// Destructor
        ~CRingBuffer();

        /* METHOD *********************************************************************/
        /**
        Set Ringbuffer size
        *
        @param  a_iRingBufferSize  New ringbuffer size
        *
        *******************************************************************************/
        void SetRingBufferSize(const int a_iRingBufferSize);

        /* METHOD *********************************************************************/
        /**
        Get Ringbuffer size
        *
        @return  ringbuffer size
        *
        *******************************************************************************/
        int GetRingBufferSize(){return m_iRingBufferSize;};

        /* METHOD *********************************************************************/
        /**
        Set number of samples that can be read from the past
        *
        @param  a_iPastSize
        *
        *******************************************************************************/
        void SetPastSize(const int a_iPastSize);

        /* METHOD *********************************************************************/
        /**
        Get number of samples that can be read from the past
        *
        @return  number of samples that can be read from the past
        *
        *******************************************************************************/
        int GetPastSize(){return m_iPastSize;};

        /* METHOD *********************************************************************/
        /**
        Get maximum of samples that can be read from buffer, without considering past samples
        *
        @return maximum of samples that can be read from buffer, without considering past samples
        *
        *******************************************************************************/
        int GetMaxReadLength();

        /* METHOD *********************************************************************/
        /**
        Get maximum of samples that can be feed to buffer
        *
        @return  maximum of samples that can be feed to buffer
        *
        *******************************************************************************/
        int GetMaxFeedLength();

        /* METHOD *********************************************************************/
        /**
        Feed samples to buffer
        *
        @param a_pFeedData        Pointer to data to be feed
        @param a_iLengthFeedData  Number of samples to be feed
        *
        *******************************************************************************/
        void Feed(const T_PRECISION* a_pFeedData,const int a_iLengthFeedData);

        /* METHOD *********************************************************************/
        /**
        Read samples from buffer
        *
        @param a_pReadData        Pointer to already allocated memory where data will be stored
        @param a_iReadLength      Number of samples to be read
        @param a_iReadOffset      Read Data from the past (negative offset), or skip samples (positive offset)
        @param a_iReadShift       Shift of ReadPointer after reading, can also be 0
        *
        *******************************************************************************/
        void Read(T_PRECISION* a_pReadData,const int a_iReadLength,const int a_iReadOffset,const int a_iReadShift);

        /* METHOD *********************************************************************/
        /**
        Read samples from buffer
        *
        @param a_pReadData        Pointer to already allocated memory where data will be stored
        @param a_iReadLength      Number of samples to be read
        a_iReadOffset is 0 and a_iReadShift is a_iReadLength
        *
        *******************************************************************************/
        void Read(T_PRECISION* a_pReadData,const int a_iReadLength)
        {Read(a_pReadData,a_iReadLength,0,a_iReadLength);};

        /* METHOD *********************************************************************/
        /**
        Clear buffer, but maintains RingBufferSize and PastSize
        *
        *******************************************************************************/
        void Clear();

      protected:
        ///Ringbuffer storage vector
        //RsSigLib::CVectorIpp<T_PRECISION> m_afRingBuffer;
        std::vector<T_PRECISION> m_afRingBuffer;

        ///Size of Ringbuffer
        int m_iRingBufferSize;

        ///Fill level of ringbuffer
        int m_iFillLevel;

        ///Number of samples that can be read of the past
        int m_iPastSize;

        ///Read Pointer - Points to element that will be read next
        int m_iReadPointer;

        ///Write Pointer - Points to element that will be overwritten next
        int m_iWritePointer;

        /* METHOD *********************************************************************/
        /**
        Initialize Buffer
        *
        *******************************************************************************/
        void Init();

        ///Status of IPP function calls
        //Status m_hStatus;

      private:
        /* METHOD *********************************************************************/
        /**
        Calculate modulus, returns unlike % from C++ 8 for mod(-2,10)
        *
        *******************************************************************************/
        inline int mod(const int a,const int b) const;
      };

      /* INLINE METHODS *************************************************************/
      /* METHOD *********************************************************************/
      /**
      Calculate modulus, returns unlike % from C++ 8 for mod(-2,10)
      *
      *******************************************************************************/
      template <typename T_PRECISION>  int CRingBuffer<T_PRECISION>::mod(const int a,const int b) const
      {
        return a-static_cast<int>(floor(static_cast<double>(a)/(static_cast<double>(b)))*static_cast<double>(b));
      }

      /* TEMPLATE METHODS ***********************************************************/
      /* PUBLIC *********************************************************************/

      /// Default constructor.
      template <typename T_PRECISION> CRingBuffer<T_PRECISION>::CRingBuffer()
        :m_iRingBufferSize(0),m_iPastSize(0),m_iFillLevel(0)
      {
        Init();
      }

      /// Constructor, setting the size of Ringbuffer.
      template <typename T_PRECISION> CRingBuffer<T_PRECISION>::CRingBuffer(const int a_iRingBufferSize)
        :m_iRingBufferSize(0),m_iPastSize(0),m_iFillLevel(0)
      {
        SetRingBufferSize(a_iRingBufferSize);
      }

      /// Destructor
      template <typename T_PRECISION> CRingBuffer<T_PRECISION>::~CRingBuffer()
      {
      }

      /* METHOD *********************************************************************/
      /**
      Set Ringbuffer size
      *
      @param  a_iRingBufferSize  New ringbuffer size
      *
      *******************************************************************************/
      template <typename T_PRECISION> void CRingBuffer<T_PRECISION>::SetRingBufferSize(const int a_iRingBufferSize)
      {
        if (m_iPastSize>a_iRingBufferSize-1)
          throw std::length_error("CRingBuffer::SetRingBufferSize() PastSize can't be greater than RingBufferSize-1\n");
        m_iRingBufferSize=a_iRingBufferSize;
        Init();
      }

      /* METHOD *********************************************************************/
      /**
      Get maximum of samples that can be read from buffer, without considering past samples
      *
      @return
      *
      *******************************************************************************/
      template <typename T_PRECISION> int CRingBuffer<T_PRECISION>::GetMaxReadLength()
      {
        return m_iFillLevel;
      }

      /* METHOD *********************************************************************/
      /**
      Get maximum of samples that can be feed to buffer
      *
      @return
      *
      *******************************************************************************/
      template <typename T_PRECISION> int CRingBuffer<T_PRECISION>::GetMaxFeedLength()
      {
        return m_iRingBufferSize-m_iFillLevel-m_iPastSize;
      }

      /* METHOD *********************************************************************/
      /**
      Set number of samples that can be read from the past
      *
      @param  a_iPastSize
      *
      *******************************************************************************/
      template <typename T_PRECISION> void CRingBuffer<T_PRECISION>::SetPastSize(const int a_iPastSize)
      {
        if (a_iPastSize<0)
          throw std::length_error("CRingBuffer::SetPastSize() PastSize must be >=0\n");
        if (a_iPastSize>m_iRingBufferSize-1)
          throw std::length_error("CRingBuffer::SetPastSize() PastSize can't be greater than RingBufferSize-1\n");
        m_iPastSize=a_iPastSize;
      }

      /* METHOD *********************************************************************/
      /**
      Clear buffer, but maintains RingBufferSize and PastSize
      *
      *******************************************************************************/
      template <typename T_PRECISION> void CRingBuffer<T_PRECISION>:: Clear()
      {
        Init();
      }

      /* METHOD *********************************************************************/
      /**
      Read samples from buffer
      *
      @param a_pReadData        Pointer to already allocated memory where data will be stored
      @param a_iReadLength      Number of samples to be read
      @param a_iReadOffset      Read Data from the past (negative offset), or skip samples (positive offset)
      @param a_iReadShift       Shift of ReadPointer after reading, can also be 0
      *
      *******************************************************************************/
      template <typename T_PRECISION> void CRingBuffer<T_PRECISION>::
        Read(T_PRECISION* a_pReadData,const int a_iReadLength,const int a_iReadOffset,const int a_iReadShift)
      {
        int iReadBegin,iSizeFirstPiece;

        if (a_iReadLength>0)
        {
          if (a_iReadOffset*-1>m_iPastSize)
            throw std::length_error("CRingBuffer::Read() Reading too far in past\n");
          if (a_iReadLength+a_iReadOffset>m_iFillLevel)
            throw std::length_error("CRingBuffer::Read() Read vector too long\n");
          if (a_iReadShift>m_iFillLevel)
            throw std::length_error("CRingBuffer::Read() Shift too long\n");
          if (a_iReadShift<0)
            throw std::length_error("CRingBuffer::Read() Shift negative\n");

          iReadBegin=mod(m_iReadPointer+a_iReadOffset,m_iRingBufferSize);

          //Normal case, the vector to be read can be read in one block without wrap around
          if (iReadBegin+a_iReadLength<=m_iRingBufferSize && iReadBegin>=0)
          {
            std::copy(m_afRingBuffer.begin() + iReadBegin, m_afRingBuffer.begin() + iReadBegin + a_iReadLength, a_pReadData);
          }
          //The vector has to be read with wrap around
          else
          {
            iSizeFirstPiece=m_iRingBufferSize-iReadBegin;
            //First piece at end of RingBuffer
            std::copy(m_afRingBuffer.begin() + iReadBegin, m_afRingBuffer.begin() + iReadBegin + iSizeFirstPiece, a_pReadData);
            //Second piece at beginning of RingBuffer
            auto secondPieceSize = a_iReadLength - iSizeFirstPiece;
            std::copy(m_afRingBuffer.begin(), m_afRingBuffer.begin() + secondPieceSize, a_pReadData);
          }
        }
        m_iReadPointer=mod(m_iReadPointer+a_iReadShift,m_iRingBufferSize);
        m_iFillLevel-=a_iReadShift;
      }

      /* METHOD *********************************************************************/
      /**
      Feed samples to buffer
      *
      @param a_pFeedData        Pointer to data to be feed
      @param a_iLengthFeedData  Number of samples to be feed
      *
      *******************************************************************************/
      template <typename T_PRECISION> void CRingBuffer<T_PRECISION>::
        Feed(const T_PRECISION* a_pFeedData,const int a_iLengthFeedData)
      {
        if (a_iLengthFeedData>0)
        {
          if (a_iLengthFeedData>m_iRingBufferSize-m_iFillLevel-m_iPastSize)
            throw std::length_error("CRingBuffer::Feed() Input Vector too long\n");

          //Normal case, the vector to be written can be written in one block without wrap around
          if (m_iWritePointer+a_iLengthFeedData<=m_iRingBufferSize)
          {
            std::copy(a_pFeedData, a_pFeedData + a_iLengthFeedData, m_afRingBuffer.begin() + m_iWritePointer);
            m_iWritePointer=mod(m_iWritePointer+a_iLengthFeedData,m_iRingBufferSize);
          }
          //The vector has to be written with wrap around
          else
          {
            auto iSizeFirstPiece=m_iRingBufferSize-m_iWritePointer;
            std::copy(a_pFeedData, a_pFeedData + iSizeFirstPiece, m_afRingBuffer.begin() + m_iWritePointer);
            std::copy(a_pFeedData + iSizeFirstPiece, a_pFeedData + a_iLengthFeedData, m_afRingBuffer.begin());

            m_iWritePointer=mod(m_iWritePointer+a_iLengthFeedData,m_iRingBufferSize);
          }
          m_iFillLevel+=a_iLengthFeedData;
        }
      }

      /* PROTECTED ******************************************************************/

      /* METHOD *********************************************************************/
      /**
      Initialize Buffer
      *
      *******************************************************************************/
      template <typename T_PRECISION> void CRingBuffer<T_PRECISION>::Init()
      {
        //Default size of RingBuffer
        const int iDefaultRingBufferSize = 100000;
        if (m_iRingBufferSize==0)
          m_iRingBufferSize=iDefaultRingBufferSize;

        m_afRingBuffer.resize(m_iRingBufferSize);
        std::fill(m_afRingBuffer.begin(), m_afRingBuffer.end(), 0);

        m_iReadPointer=0;
        m_iWritePointer=0;
        m_iFillLevel=0;
      }
    }
  }
}

#ifdef _MSC_VER
#pragma warning (pop)
#elif __GNUC__
#pragma GCC diagnostic pop
#endif

/*** @endcond ***/