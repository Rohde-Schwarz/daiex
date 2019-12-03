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

#ifndef ARRAY_CONTAINER_IPP_H
#define ARRAY_CONTAINER_IPP_H

/* INCLUDE FILES ******************************************************************/
#include <memory>

#include "AmlabCommon.h"
#include "Generic.h"
#include "ArrayContainer.h"
#include "ArrayPolicyResizeDefault.h"

namespace AmlabCommon 
{
  class CArrayContainerIppTest;

  /* CLASS DECLARATION **************************************************************/
  /**
  * @brief  Implementation of an array container using IPP memory allocation.
  *
  * @see IArrayContainer
  * @see CArrayPolicyResizeDefault
  ***********************************************************************************/
  template
    <
    typename  T,
    class     ResizePolicy = CArrayPolicyResizeDefault
    >
  class CArrayContainerIpp
    : public  IArrayContainer<T>
    , private ResizePolicy
  {    
  public:
    friend class CArrayContainerIppTest;

    typedef ResizePolicy  RP;                                       /*!< An abbreviation for accessing the resize policy. */
    typedef typename IArrayContainer<T>::StorageType  StorageType;  /*!< A type that represents the data type stored in the container. */
    typedef typename IArrayContainer<T>::PointerType  PointerType;  /*!< A type that provides a pointer to an element in the container. */

    /* METHOD *************************************************************************/
    /**
    * @brief  Standard constructor.
    *
    *         Constructs an empty array container with an initial capacity of zero.
    ***********************************************************************************/
    CArrayContainerIpp()
      : m_pStorage    ( 0 )
      , m_u32Capacity ( 0u )
      , m_u32Size     ( 0u )
    { }

    /* METHOD *************************************************************************/
    /**
    * @brief  Constructor with initial capacity.
    * 
    *         Constructs an array container with an initial capacity.
    *
    * @param  a_u32Capacity Initial capacity
    ***********************************************************************************/
    CArrayContainerIpp(uint32 a_u32Capacity)
      : m_pStorage    ( 0 )
      , m_u32Capacity ( 0u )
      , m_u32Size     ( 0u )
    {
      bool  l_bAllocResult = alloc(a_u32Capacity);  // allocate the required size
      AC_ASSERT( l_bAllocResult );                  // check if allocation was successful
      AC_UNUSED( l_bAllocResult );
      if (!l_bAllocResult)
        //throw std::bad_alloc("Memory allocation failed.");  // not supported by VS 2013
        throw std::bad_alloc();
    }

    /* METHOD *************************************************************************/
    /**
    * @brief  Copy Constructor, makes a deep copy of the provided container.
    ***********************************************************************************/
    CArrayContainerIpp(const CArrayContainerIpp & a_rhs)
      : m_pStorage    ( 0 )
      , m_u32Capacity ( 0u )
      , m_u32Size     ( 0u )
    {
      bool  l_bAllocResult = alloc(a_rhs.m_u32Capacity);  // allocate the required size
      AC_ASSERT( l_bAllocResult );                        // check if allocation was successful
      AC_UNUSED( l_bAllocResult );

      if (!l_bAllocResult)
        //throw std::bad_alloc("Memory allocation failed.");  // not supported by VS 2013
        throw std::bad_alloc();
      else
      {
        copy(m_pStorage, a_rhs.m_pStorage, a_rhs.m_u32Size);  // make a deep copy of the storage
        m_u32Size = a_rhs.m_u32Size;                          // set the number of stored elements
      }
    }

    /* METHOD *************************************************************************/
    /**
    * @brief  Constructor with storage initialization.
    * 
    *         Creates and initializes the container with the provided data.
    *
    * @param a_pInitData   Array with init data.
    * @param a_u32Size     Number of elements in the provided array.
    ***********************************************************************************/
    CArrayContainerIpp(const StorageType * a_pInitData, uint32 a_u32Size)
      : m_pStorage    ( 0 )
      , m_u32Capacity ( 0u )
      , m_u32Size     ( 0u )
    {
      bool  l_bAllocResult = alloc(a_u32Size);          // allocate the required size
      AC_ASSERT( l_bAllocResult );                      // check if allocation was successful
      AC_UNUSED( l_bAllocResult );
      
      if (!l_bAllocResult)
        //throw std::bad_alloc("Memory allocation failed.");  // not supported by VS 2013
        throw std::bad_alloc();
      else
      {
        copy(m_pStorage, a_pInitData, a_u32Size); // make a deep copy of the storage
        m_u32Size = a_u32Size;                    // set the number of stored elements
      }
    }

    /* METHOD *************************************************************************/
    /**
    * @brief  Destructor deallocates the reserved memory.
    ***********************************************************************************/
    ~CArrayContainerIpp()
    {
      free();
    }

    /* METHOD *************************************************************************/
    /**
    * @brief  Returns a pointer to the first element stored in the container.
    * 
    * @return A pointer to the first element stored in the container.
    ***********************************************************************************/
    inline PointerType getPtr()
    {
      return m_pStorage;
    }

    /* METHOD *************************************************************************/
    /**
    * @brief  Returns a const pointer to the first element stored in the container.
    * 
    * @return A const pointer to the first element stored in the container.
    ***********************************************************************************/
    inline const PointerType getPtr() const
    {
      return m_pStorage;
    }

    /* METHOD *************************************************************************/
    /**
    * @brief  Returns the number of elements that the container could contain without
    *         allocating more storage.
    *
    * @return  The number of elements that the container could contain without
    *          allocating more storage.
    ***********************************************************************************/
    uint32 getCapacity() const
    {
      return m_u32Capacity;
    }

    /* METHOD *************************************************************************/
    /**
    * @brief  Returns the number of elements in the container.
    * 
    * @return  The number of elements in the container.
    ***********************************************************************************/
    uint32 getSize() const
    {
      return m_u32Size;
    }

    /* METHOD *************************************************************************/
    /**
    * @brief  Sets the number of valid elements in the container.
    *
    *         If the given size above the capacity of the container, the size is
    *         set to the capacity.
    ***********************************************************************************/
    void setSize(uint32 a_u32Size)
    {      
      m_u32Size = a_u32Size > m_u32Capacity ? m_u32Capacity : a_u32Size; 
    }

    /* METHOD *************************************************************************/
    /**
    * @brief  Tests if the container is empty.
    *
    * @return True if the number of elements is not zero.
    ***********************************************************************************/    
    bool isEmpty() const
    {
      return (0 == m_u32Size);
    }

    /* METHOD *************************************************************************/
    /**
    * @brief  Method for comparing elements in the container. 
    * 
    *         Compares two different container if their stored elements are equal.
    *
    * @param a_rhs   A container to compare with.
    *
    * @return True if the stored elements are all equal.
    ***********************************************************************************/
    bool compare(const IArrayContainer<T> & a_rhs) const
    {
      do 
      {
        // check if the number of stored elements is equal
        if (m_u32Size != a_rhs.getSize())   
          break;

        // check if the stored elements are equal
        if (0 != memcmp(m_pStorage, a_rhs.getPtr(), sizeof(StorageType) * m_u32Size))
          break;

        return true;
      } while(false);

      return false;
    }

    /* METHOD *************************************************************************/
    /**
    * @brief   Assigns the values of a static array to the storage of the container.
    * 
    * @param  a_pData   The array with elements to be stored in the container.
    * @param  a_u32Size Number of elements in the given array.
    *
    * @return True if a required memory resize was successful.
    ***********************************************************************************/
    bool assign(const StorageType* a_pData, uint32 a_u32Size)
    {
      bool  l_bReallocResult = realloc(a_u32Size);            // allocate the required size
      AC_UNUSED( l_bReallocResult );                          // check if allocation was successful

      if (l_bReallocResult == false)
        return false;

      copy(m_pStorage, a_pData, a_u32Size);   // make a deep copy of the storage
      m_u32Size = a_u32Size;                  // set the number of stored elements

      return true;
    }

    /* METHOD *************************************************************************/
    /**
    * @brief   Assigns the storage elements of another container.
    * 
    * @param  a_rhs A Container to assign.
    *
    * @return True if a required memory resize was successful.
    ***********************************************************************************/
    bool assign(const IArrayContainer<T> & a_rhs)
    {
      uint32 l_u32Size = a_rhs.getSize();

      bool  l_bReallocResult = realloc(l_u32Size);  // allocate the required size
      AC_ASSERT( l_bReallocResult );                // check if allocation was successful

      if (l_bReallocResult == false)
        return false;

      copy(m_pStorage, a_rhs.getPtr(), l_u32Size);  // make a deep copy of the storage
      m_u32Size = l_u32Size;                        // set the number of stored elements

      return true;
    }

    /* METHOD *************************************************************************/
    /**
    * @brief  Allocates memory for the requested number of elements 
    *
    * @param a_u32Size   Number of required elements
    *
    * @see CArrayPolicyResizeDefault::onAlloc()
    *
    * @return True if memory allocation was successful, otherwise false if it failed.
    ***********************************************************************************/
    bool alloc(uint32 a_u32Size)
    {
      // check the policy if allocation is allowed
      if (RP::onAlloc(m_u32Capacity, a_u32Size))
      {
        // allocate the requested size of memory
        //m_pStorage = Ipp::Malloc<StorageType>(a_u32Size); 
        m_pStorage = new StorageType [a_u32Size];

        // check if allocation was successful
        if (0 != m_pStorage)
        {
          m_u32Capacity = a_u32Size;
          return true;
        } else {
          m_u32Capacity = 0;
          return false;
        }
      }

      return true;
    }

    /* METHOD *************************************************************************/
    /**
    * @brief    Changes the container capacity.
    * 
    *           Previously stored elements will be lost after reallocation.
    *             
    *
    * @param a_u32Size   Number of required elements
    *
    * @see resize() for difference in behaviour.
    * @see CArrayPolicyResizeDefault::onRealloc()
    *
    * @return True if memory reallocation was successful.
    ***********************************************************************************/
    bool realloc(uint32 a_u32Size)
    {
      // check the policy if reallocation is allowed
      if (RP::onRealloc(getCapacity(), a_u32Size))
      {
        free();

        // allocate memory and check if it was successful
        return alloc(a_u32Size);
      }

      return true;
    }

    /* METHOD *************************************************************************/
    /**
    * @brief    Changes the container capacity.
    * 
    *           Previously stored elements will be unchanged after resize.
    *
    * @param a_u32Size  Number of required elements.
    *
    * @see realloc() for difference in behaviour.
    * @see CArrayPolicyResizeDefault::onResize()
    *
    * @return True if memory resize was successful.
    ***********************************************************************************/
    bool resize(uint32 a_u32Size)
    {
      // check the policy if resize is allowed
      if (RP::onResize(m_u32Capacity, a_u32Size))
      {
        PointerType   l_pNewStorage;
        uint32        l_u32Elements;

        // Allocate memory for the new storage
        l_pNewStorage = 0;
        // Ipp::Malloc<StorageType>(a_u32Size);
        l_pNewStorage = new StorageType [a_u32Size];

        // Check if memory allocation was successful
        if (0 == l_pNewStorage)
          return false;

        // Determine the number of elements to copy from the old storage
        //  to the new one
        l_u32Elements = a_u32Size > m_u32Size ? m_u32Size : a_u32Size;


        // Copy the elements from the old to the new storage
        copy(l_pNewStorage, m_pStorage, l_u32Elements);

        // deallocate the old storage
        free();

        m_u32Size     = l_u32Elements;
        m_u32Capacity = a_u32Size;
        m_pStorage    = l_pNewStorage;
      }

      return true;
    }

    /* METHOD *************************************************************************/
    /**
    * @brief  Deallocates the internal used array.
    *
    * @see CArrayPolicyResizeDefault::onFree()
    ***********************************************************************************/
    void free()
    {
      // check the policy if deallocation is allowed
      if (RP::onFree(m_u32Capacity))
      {
        //Ipp::Free(m_pStorage);
        delete[] m_pStorage;

        // set all members to zero
        m_pStorage    = 0;
        m_u32Capacity = 0;
        m_u32Size     = 0;
      }
    }

    /* METHOD *************************************************************************/
    /**
    * @brief   Appends the storage elements of another container, starting at .getPtr() + u32StartOffset 
    *          and writing a_u32Len values. 
    * @param  a_rhs A Container to append.
    *
    * @return True if a required memory resize was successful.
    ***********************************************************************************/
    void appendPartial(const IArrayContainer<T> & a_rhs,uint32 u32StartOffset, uint32 a_u32Len)
    {
       uint32    l_u32Size   = getSize();
       if ( getCapacity() < (a_u32Len+l_u32Size) )
       {
          resize(a_u32Len+l_u32Size);
       }

       copy(m_pStorage+l_u32Size, a_rhs.getPtr()+u32StartOffset,a_u32Len);
       setSize(a_u32Len+l_u32Size);
    }

  protected:
    void copy(StorageType* a_pDest, const StorageType* a_pSrc, uint32 a_u32Size)
    {
      //Ipp::Copy<StorageType>(a_pSrc, a_pDest, a_u32Size);
      for (uint32 i = 0; i < a_u32Size; i++)
      {
        a_pDest[i] = a_pSrc[i];
      }
    }

  private:
    PointerType   m_pStorage;       /*!< Pointer to a static array. */
    uint32        m_u32Capacity;    /*!< Size of the storage. */
    uint32        m_u32Size;        /*!< Number of valid elements in the container. */
  };

  
}



#endif // not ARRAY_CONTAINER_IPP_H
/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/
