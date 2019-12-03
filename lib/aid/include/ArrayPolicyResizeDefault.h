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

#ifndef ARRAY_POLICY_RESIZE_DEFAULT_H
#define ARRAY_POLICY_RESIZE_DEFAULT_H

/* INCLUDE FILES ******************************************************************/
#include "Types.h"

namespace AmlabCommon 
{
  /* CLASS DECLARATION *********************************************/
  /**
  * @brief  Provides a resize policy for different implementation of
  *         array container.
  *
  *         This policy contains easy restrictions to avoid unnecessary
  *         memory allocations and memory leaks.
  ******************************************************************/
  struct CArrayPolicyResizeDefault
  {
    /* METHOD *************************************************************************/
    /**
    * @brief  Check if allocation is allowed.
    * 
    *         Allocation is only allowed if no memory is already allocated (to avoid memory leaks)
    *         and if the requested size is not zero (call free instead).
    *
    * @param a_u32Capacity Currently reserved capacity of the container.
    * @param a_u32NewSize  Requested size.
    *
    * @return  True if allocation is allowed.
    ***********************************************************************************/
    bool onAlloc(uint32 a_u32Capacity, uint32 a_u32NewSize)
    {
      if ((a_u32Capacity != 0) || (a_u32NewSize == 0))
        return false;
      else
        return true;
    }

    /* METHOD *************************************************************************/
    /**
    * @brief  Check if reallocation is allowed.
    * 
    *         Reallocation is allowed when the requested size is not zero (call free instead).
    *         If the requested size is already provided by the container the request is denied
    *         to avoid unnecessary calls.
    *
    * @param a_u32Capacity Currently reserved capacity of the container.
    * @param a_u32NewSize  Requested size.
    *
    * @return  True if reallocation is allowed.
    ***********************************************************************************/
    bool onRealloc(uint32 a_u32Capacity, uint32 a_u32NewSize)
    {
      if ((a_u32NewSize == 0) || (a_u32Capacity == a_u32NewSize))
        return false;
      else
        return true;
    }

    /* METHOD *************************************************************************/
    /**
    * @brief  Check if resize if allowed.
    * 
    *         Reallocation is allowed when the requested size is not zero (call free instead).
    *         If the requested size is already provided by the container the request is denied
    *         to avoid unnecessary calls.
    *
    * @param a_u32Capacity Currently reserved capacity of the container.
    * @param a_u32NewSize  Requested size.
    *
    * @return  True if resize is allowed.
    ***********************************************************************************/
    bool onResize(uint32  a_u32Capacity, uint32 a_u32NewSize)
    {
      if ((a_u32NewSize == 0) || (a_u32Capacity == a_u32NewSize))
        return false;
      else
        return true;
    }

    /* METHOD *************************************************************************/
    /**
    * @brief  Check if deallocation is allowed.
    * 
    *         If the container is not already deallocated the request ist allowed.
    *
    * @param a_u32Capacity Currently reserved capacity of the container.
    *
    * @return  True if deallocation is allowed.
    ***********************************************************************************/
    bool onFree(uint32 a_u32Capacity)
    {
      if (a_u32Capacity == 0)
        return false;
      else
        return true;
    }
  };
}

#endif // not ARRAY_POLICY_RESIZE_DEFAULT_H
/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/
