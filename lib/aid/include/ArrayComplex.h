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

#ifndef ARRAY_COMPLEX_H
#define ARRAY_COMPLEX_H

/* INCLUDE FILES ******************************************************************/
// #include "Object.h"
#include "replacement.h"
#include "ArrayReal.h"

namespace AmlabCommon
{
  class CArrayComplexTest;

  /* CLASS DECLARATION **************************************************************/
  /**
  * @brief  Implementation of an array for storing and calculating with complex values.
  ***********************************************************************************/
  class CArrayComplex
    : public CArrayContainerIpp<CplxType>
  {
  public:
    friend class CArrayComplexTest;

    typedef CArrayReal  AR;           /*!< An abbreviation for designating a real array. */
    typedef CplxType    StorageType;  /*!< An abbreviation for designating the storage type of the container. */

    CArrayComplex();
    CArrayComplex(const CArrayComplex& a_rhs);
    CArrayComplex(uint32 a_u32Capacity);
    CArrayComplex(const AR::StorageType* a_pDataReal, const AR::StorageType* a_pDataImag, uint32 a_u32Size);
    CArrayComplex(const StorageType* a_pInitData, uint32 a_u32Size);
    CArrayComplex(const CArrayReal& a_DataReal, const CArrayReal& a_DataImag);

    ~CArrayComplex();

    CArrayComplex& operator=(const StorageType& a_fConstant);

#ifndef ARRAY_REAL_REDUCED_METHOD_SET
    CArrayComplex& operator+=(const StorageType& a_fConstant);
    CArrayComplex& operator-=(const StorageType& a_fConstant);
    CArrayComplex& operator*=(const StorageType& a_fConstant);
    CArrayComplex& operator/=(const StorageType& a_fConstant);

    CArrayComplex operator+(const StorageType& a_fConstant) const;   // Folgeproblem von GenericIpp.h:  inval abstract Returntype
    CArrayComplex operator-(const StorageType& a_fConstant) const;
    CArrayComplex operator*(const StorageType& a_fConstant) const;
    CArrayComplex operator/(const StorageType& a_fConstant) const;


    CArrayComplex& operator=(const CArrayComplex& a_rhs);

    CArrayComplex& operator+=(const CArrayComplex& a_rhs);
    CArrayComplex& operator-=(const CArrayComplex& a_rhs);
    CArrayComplex& operator*=(const CArrayComplex& a_rhs);
    CArrayComplex& operator/=(const CArrayComplex& a_rhs);

    CArrayComplex operator+(const CArrayComplex& a_rhs) const;
    CArrayComplex operator-(const CArrayComplex& a_rhs) const;
    CArrayComplex operator*(const CArrayComplex& a_rhs) const;
    CArrayComplex operator/(const CArrayComplex& a_rhs) const;
#endif
    CArrayComplex& operator=(const CArrayReal& a_rhs);
  
    bool operator==(const CArrayComplex& a_rhs) const;
    bool operator!=(const CArrayComplex& a_rhs) const;

    StorageType& operator[](uint32 a_u32Idx);
    const StorageType& operator[](uint32 a_u32Idx) const;

    bool assign(const StorageType* a_pData, uint32 a_u32Size);
    bool assign(const AR::StorageType* a_pDataReal, const AR::StorageType* a_pDataImag, uint32 a_u32Size);
    bool assign(const CArrayReal& a_DataReal, const CArrayReal& a_DataImag);

    bool toReal(CArrayReal& a_DataReal, CArrayReal& a_DataImag) const;
    bool getReal(CArrayReal& a_Data) const;
    bool getImag(CArrayReal& a_Data) const;

    void setSize(uint32 a_u32Size);
  };
}
AmlabCommon::CArrayComplex& operator<<(AmlabCommon::CArrayComplex& rhs, const AmlabCommon::CArrayComplex& lhs);

#endif // not ARRAY_COMPLEX_H
/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/
