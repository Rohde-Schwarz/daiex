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

#ifndef ARRAY_CONTAINER_H
#define ARRAY_CONTAINER_H

/* INCLUDE FILES ******************************************************************/
#include "Types.h"

namespace AmlabCommon
{
  /* CLASS DECLARATION *********************************************/
  /**
  * @brief  The array container interface for storing data arrays of
  *         different types.
  ******************************************************************/
  template <typename T>
  class IArrayContainer 
  {
  protected:
    /* METHOD *************************************************************************/
    /**
    * @brief  Default Constructor
    ***********************************************************************************/
    IArrayContainer() { }

  public:
    typedef T  StorageType;   /*!< A type that represents the data type stored in the container. */
    typedef T* PointerType;   /*!< A type that provides a pointer to an element in the container. */

    /* METHOD *************************************************************************/
    /**
    * @brief  Destructor
    ***********************************************************************************/
    virtual ~IArrayContainer() { }

    /* METHOD *************************************************************************/
    /**
    * @brief  Returns a pointer to the first element stored in the container.
    * 
    * @return A pointer to the first element stored in the container.
    ***********************************************************************************/
    virtual PointerType getPtr() = 0;

    /* METHOD *************************************************************************/
    /**
    * @brief  Returns a const pointer to the first element stored in the container.
    * 
    * @return A const pointer to the first element stored in the container.
    ***********************************************************************************/
    virtual const PointerType getPtr() const = 0;

    /* METHOD *************************************************************************/
    /**
    * @brief  Returns the number of elements that the container could contain without
    *         allocating more storage.
    *
    * @return  The number of elements that the container could contain without
    *          allocating more storage.
    ***********************************************************************************/
    virtual uint32 getCapacity() const = 0;

    /* METHOD *************************************************************************/
    /**
    * @brief  Returns the number of elements in the container.
    * 
    * @return  The number of elements in the container.
    ***********************************************************************************/
    virtual uint32 getSize() const = 0;

    /* METHOD *************************************************************************/
    /**
    * @brief  Sets the number of valid elements in the container.
    ***********************************************************************************/
    virtual void setSize(uint32 a_u32Size) = 0;

    /* METHOD *************************************************************************/
    /**
    * @brief  Tests if the container is empty.
    *
    * @return True if the number of elements is not zero.
    ***********************************************************************************/    
    virtual bool isEmpty() const = 0;

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
    virtual bool compare(const IArrayContainer& a_rhs) const = 0;

    /* METHOD *************************************************************************/
    /**
    * @brief   Assigns the values of a static array to the storage of the container.
    * 
    * @param  a_pData   The array with elements to be stored in the container.
    * @param  a_u32Size Number of elements in the given array.
    *
    * @return True if a assignment was successful.
    ***********************************************************************************/
    virtual bool assign(const StorageType* a_pData, uint32 a_u32Size) = 0;

    /* METHOD *************************************************************************/
    /**
    * @brief   Assigns the storage elements of another container.
    * 
    * @param  a_rhs A Container to assign.
    *
    * @return True if a assignment was successful.
    ***********************************************************************************/
    virtual bool assign(const IArrayContainer & a_rhs) = 0;

    /* METHOD *************************************************************************/
    /**
    * @brief  Allocates memory for the requested number of elements 
    * 
    * @param a_u32Size   Number of required elements
    *
    * @return True if memory allocation was successful.
    ***********************************************************************************/
    virtual bool alloc(uint32 a_u32Size) = 0;

    /* METHOD *************************************************************************/
    /**
    * @brief    Changes the container capacity.
    * 
    *           Previously stored elements will be lost after reallocation.
    *
    * @param a_u32Size   Number of required elements
    *
    * @see resize() for difference in behaviour.
    *
    * @return True if memory reallocation was successful.
    ***********************************************************************************/
    virtual bool realloc(uint32 a_u32Size) = 0;

    /* METHOD *************************************************************************/
    /**
    * @brief    Changes the container capacity.
    * 
    *           Previously stored elements will be unchanged after resize.
    *
    * @param a_u32Size  Number of required elements.
    *
    * @see realloc() for difference in behaviour.
    *
    * @return True if memory resize was successful.
    ***********************************************************************************/
    virtual bool resize(uint32 a_u32Size) = 0;

    /* METHOD *************************************************************************/
    /**
    * @brief  Deallocates the internal used array.
    ***********************************************************************************/
    virtual void free() = 0;

    virtual void copy(StorageType* a_pDest, const StorageType* a_pSrc, uint32 a_u32Size) = 0;
  };
}
#endif // not ARRAY_CONTAINER_H
/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/
