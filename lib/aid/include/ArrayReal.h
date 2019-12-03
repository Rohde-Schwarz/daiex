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

#ifndef ARRAY_REAL_H
#define ARRAY_REAL_H

#define ARRAY_REAL_REDUCED_METHOD_SET 1 

/* INCLUDE FILES ******************************************************************/
#include "ArrayContainerIpp.h"
#include "TypesExtended.h"

namespace AmlabCommon
{
  class CArrayRealTest;

  /* CLASS DECLARATION **************************************************************/
  /**
  * @brief  Implementation of an array for storing and calculating with real values.
  *
  * @see CArrayComplex
  ***********************************************************************************/
  class CArrayReal
    : public CArrayContainerIpp<RealType>
  {
  public:
    friend class CArrayRealTest;

    typedef RealType  StorageType;  /*!< An abbreviation for designating the storage type of the container. */

    CArrayReal();
    CArrayReal(const CArrayReal& a_rhs);
    CArrayReal(uint32 a_u32Capacity);
    CArrayReal(const StorageType * a_pfInitData, uint32 a_u32Size);
    ~CArrayReal();


#ifndef ARRAY_REAL_REDUCED_METHOD_SET
    CArrayReal& operator=(StorageType a_fConstant);
    CArrayReal& operator+=(StorageType a_fConstant);
    CArrayReal& operator-=(StorageType a_fConstant);
    CArrayReal& operator*=(StorageType a_fConstant);
    CArrayReal& operator/=(StorageType a_fConstant);
    
    CArrayReal operator+(StorageType a_fConstant) const;
    CArrayReal operator-(StorageType a_fConstant) const;
    CArrayReal operator*(StorageType a_fConstant) const;
    CArrayReal operator/(StorageType a_fConstant) const;
#endif

    CArrayReal& operator=(const CArrayReal& a_rhs);

    CArrayReal operator+(const CArrayReal& a_rhs) const;
    CArrayReal operator-(const CArrayReal& a_rhs) const;
    CArrayReal operator*(const CArrayReal& a_rhs) const;
    CArrayReal operator/(const CArrayReal& a_rhs) const;

    CArrayReal& operator+=(const CArrayReal& a_rhs);
    CArrayReal& operator-=(const CArrayReal& a_rhs);
    CArrayReal& operator*=(const CArrayReal& a_rhs);
    CArrayReal& operator/=(const CArrayReal& a_rhs);


    bool operator==(const CArrayReal& a_rhs) const;
    bool operator!=(const CArrayReal& a_rhs) const;

    StorageType& operator[](uint32 a_u32Idx);
    const StorageType& operator[](uint32 a_u32Idx) const;

    void setSize(uint32 a_u32Size);
  };
}
AmlabCommon::CArrayReal& operator<<(AmlabCommon::CArrayReal& a_rhs, const AmlabCommon::CArrayReal& a_lhs);
#endif // not ARRAY_REAL_H
/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/
