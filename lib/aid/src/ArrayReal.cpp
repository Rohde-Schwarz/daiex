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

/* INCLUDE FILES ******************************************************************/
#include <cstring>
#include "AmlabCommon.h"
#include "ArrayReal.h"

using namespace std;
using AmlabCommon::CArrayReal;

/* METHOD *************************************************************************/
/**
* @brief  Standard Constructor
***********************************************************************************/
CArrayReal::CArrayReal()
{

}

/* METHOD *************************************************************************/
/**
* @brief  Constructor with initial capacity.
* 
*         Constructs an array with an initial capacity.
*
* @param  a_u32Capacity Initial capacity
***********************************************************************************/
CArrayReal::CArrayReal(uint32 a_u32Capacity)
: CArrayContainerIpp<StorageType>(a_u32Capacity)
{

}

/* METHOD *************************************************************************/
/**
* @brief  Constructor with array initialization.
* 
*         Creates and initializes the array with the provided data.
*
* @param a_pInitData   Array with init data.
* @param a_u32Size     Number of elements in the provided array.
***********************************************************************************/
CArrayReal::CArrayReal(const StorageType *a_pInitData, uint32 a_u32Size)
: CArrayContainerIpp<StorageType>(a_pInitData, a_u32Size)
{

}

/* METHOD *************************************************************************/
/**
* @brief  Copy Constructor, makes a deep copy of the provided array.
***********************************************************************************/
CArrayReal::CArrayReal(const CArrayReal& a_rhs)
: CArrayContainerIpp<StorageType>(a_rhs)
{

}

/* METHOD *************************************************************************/
/**
* @brief  Destructor.
***********************************************************************************/
CArrayReal::~CArrayReal()
{

}

/* METHOD *************************************************************************/
/**
* @brief  Method for comparing elements in the array. 
* 
*         Compares two different arrays if their stored elements are equal.
*
* @param a_rhs   An array to compare with.
*
* @see CArrayContainerIpp::compare()
*
* @return True if the stored elements are all equal.
***********************************************************************************/
bool CArrayReal::operator==(const CArrayReal& a_rhs) const
{
  return compare(a_rhs);
}

/* METHOD *************************************************************************/
/**
* @brief  Method for comparing elements in the array. 
* 
*         Compares two different arrays if their stored elements are unequal.
*
* @param a_rhs   An array to compare with.
*
* @see operator==()
* @see CArrayContainerIpp::compare()
*
* @return True if the stored elements are all unequal.
***********************************************************************************/
bool CArrayReal::operator!=(const CArrayReal& a_rhs) const
{
  return !operator==(a_rhs);
}

/* METHOD *************************************************************************/
/**
* @brief  Assigns a constant to the array.
* 
*         All elements of the array are set to the constant. If the size of the
*         array is zero no assignment is done, although the capacity is non-zero.
*
* @param  a_fConstant A constant value that is assigned to the array.
*
* @see IArrayContainer for the difference of size and capacity.
*
* @return A reference to this array.
***********************************************************************************/
#ifndef ARRAY_REAL_REDUCED_METHOD_SET
CArrayReal& CArrayReal::operator=(StorageType a_fConstant)
{
  //Ipp::Set<StorageType>(a_fConstant, getPtr(), getSize());

  return *this;
}
#endif
/* METHOD *************************************************************************/
/**
* @brief  Assigns another array to this array.
* 
*         To assign another array the capacity of the lhs must big enough to hold
*         all the elements of the rhs. Therefore before copying is done the storage
*         of lhs is reallocated if it is too small.
*
* @param  a_rhs Another array that will be assigned.
*
* @return A reference to this array.
***********************************************************************************/
CArrayReal& CArrayReal::operator=(const CArrayReal& a_rhs)
{
  uint32 l_u32Size = a_rhs.getSize();

  // check if this array has enough capacity
  if (getCapacity() < l_u32Size)
    realloc(l_u32Size);

  // copy all elememts from rhs to lhs
  copy(getPtr(), a_rhs.getPtr(), l_u32Size);

  CArrayContainerIpp<RealType>::setSize(l_u32Size);

  return *this;
}

#ifndef ARRAY_REAL_REDUCED_METHOD_SET

/* METHOD *************************************************************************/
/**
* @brief  Adds a constant to the elements in the array.
*   
* @param  a_fConstant A constant that is added to the stored elements.
*
* @return A reference to this array.
***********************************************************************************/
CArrayReal& CArrayReal::operator+=(StorageType a_fConstant)
{
  //Ipp::AddC<StorageType>(a_fConstant, getPtr(), getSize());
  return *this;
}

/* METHOD *************************************************************************/
/**
* @brief  Subtracts a constant from the elements in the array.
*   
* @param  a_fConstant A constant that is subtracted from the stored elements.
*
* @return A reference to this array.
***********************************************************************************/
CArrayReal& CArrayReal::operator-=(StorageType a_fConstant)
{
  //Ipp::SubC<StorageType>(a_fConstant, getPtr(), getSize());
  return *this;
}

/* METHOD *************************************************************************/
/**
* @brief  Multiplies a constant to the elements in the array.
*   
* @param  a_fConstant A constant that is multiplied to the stored elements.
*
* @return A reference to this array.
***********************************************************************************/
CArrayReal& CArrayReal::operator*=(StorageType a_fConstant)
{
  //Ipp::MulC<StorageType>(a_fConstant, getPtr(), getSize());
  return *this;
}

/* METHOD *************************************************************************/
/**
* @brief  Divides a constant from the elements in the array.
*   
* @param  a_fConstant A constant that is divided from the stored elements.
*
* @return A reference to this array.
***********************************************************************************/
CArrayReal& CArrayReal::operator/=(StorageType a_fConstant)
{
  //Ipp::DivC<StorageType>(a_fConstant, getPtr(), getSize());
  return *this;
}

/* METHOD *************************************************************************/
/**
* @brief  Adds a constant to the elements in the array.
*   
* @param  a_fConstant A constant that is added to the stored elements.
*
* @return A copy of the resulted array.
***********************************************************************************/
CArrayReal CArrayReal::operator+(StorageType a_fConstant) const
{
  return CArrayReal(*this).operator+=(a_fConstant);
}

/* METHOD *************************************************************************/
/**
* @brief  Subtracts a constant from the elements in the array.
*   
* @param  a_fConstant A constant that is subtracted from the stored elements.
*
* @return A copy of the resulted array.
***********************************************************************************/
CArrayReal CArrayReal::operator-(StorageType a_fConstant) const
{
  return CArrayReal(*this).operator-=(a_fConstant);
}

/* METHOD *************************************************************************/
/**
* @brief  Multiplies a constant to the elements in the array.
*   
* @param  a_fConstant A constant that is multiplied to the stored elements.
*
* @return A copy of the resulted array.
***********************************************************************************/
CArrayReal CArrayReal::operator*(StorageType a_fConstant) const
{
  return CArrayReal(*this).operator*=(a_fConstant);
}

/* METHOD *************************************************************************/
/**
* @brief  Divides a constant from the elements in the array.
*   
* @param  a_fConstant A constant that is divided from the stored elements.
*
* @return A copy of the resulted array.
***********************************************************************************/
CArrayReal CArrayReal::operator/(StorageType a_fConstant) const
{
  return CArrayReal(*this).operator/=(a_fConstant);
}

/* METHOD *************************************************************************/
/**
* @brief  Adds an array. Both arrays have to be the same size.
*   
* @param  a_rhs The other array that is added.
*
* @return A reference to this array.
***********************************************************************************/
CArrayReal& CArrayReal::operator+=(const CArrayReal& a_rhs)
{
  AC_ASSERT(getSize() == a_rhs.getSize());

  //Ipp::Add<StorageType>(a_rhs.getPtr(), getPtr(), getSize());
  return *this;
}

/* METHOD *************************************************************************/
/**
* @brief  Subtracts an array. Both arrays have to be the same size.
*   
* @param  a_rhs The other array that is subtracted.
*
* @return A reference to this array.
***********************************************************************************/
CArrayReal& CArrayReal::operator-=(const CArrayReal& a_rhs)
{
  AC_ASSERT(getSize() == a_rhs.getSize());

  //Ipp::Sub<StorageType>(a_rhs.getPtr(), getPtr(), getSize());
  return *this;
}

/* METHOD *************************************************************************/
/**
* @brief  Multiplies an array. Both arrays have to be the same size.
*   
* @param  a_rhs The other array that is multiplied.
*
* @return A reference to this array.
***********************************************************************************/
CArrayReal& CArrayReal::operator*=(const CArrayReal& a_rhs)
{
  AC_ASSERT(getSize() == a_rhs.getSize());

  //Ipp::Mul<StorageType>(a_rhs.getPtr(), getPtr(), getSize());
  return *this;
}

/* METHOD *************************************************************************/
/**
* @brief  Divides an array. Both arrays have to be the same size.
*   
* @param  a_rhs The other array that is divided.
*
* @return A reference to this array.
***********************************************************************************/
CArrayReal& CArrayReal::operator/=(const CArrayReal& a_rhs)
{
  AC_ASSERT(getSize() == a_rhs.getSize());

  //Ipp::Div<StorageType>(a_rhs.getPtr(), getPtr(), getSize());
  return *this;
}

/* METHOD *************************************************************************/
/**
* @brief  Adds an array. Both arrays have to be the same size.
*
* @note   For better performance use "+=" if possible.
*
* @param  a_rhs The other array that is added.
*
* @return A copy of the resulted array.
***********************************************************************************/
CArrayReal CArrayReal::operator+(const CArrayReal& a_rhs) const
{
  return CArrayReal(*this).operator+=(a_rhs);
}

/* METHOD *************************************************************************/
/**
* @brief  Subtracts an array. Both arrays have to be the same size.
*
* @note   For better performance use "-=" if possible.
*  
* @param  a_rhs The other array that is subtracted.
*
* @return A copy of the resulted array.
***********************************************************************************/
CArrayReal CArrayReal::operator-(const CArrayReal& a_rhs) const
{
  return CArrayReal(*this).operator-=(a_rhs);
}

/* METHOD *************************************************************************/
/**
* @brief  Multiplies an array. Both arrays have to be the same size.
*
* @note   For better performance use "*=" if possible.
*
* @param  a_rhs The other array that is multiplied.
*
* @return A copy of the resulted array.
***********************************************************************************/
CArrayReal CArrayReal::operator*(const CArrayReal& a_rhs) const
{
  return CArrayReal(*this).operator*=(a_rhs);
}

/* METHOD *************************************************************************/
/**
* @brief  Divides an array. Both arrays have to be the same size.
*
* @note   For better performance use "/=" if possible.
*
* @param  a_rhs The other array that is divided.
*
* @return A copy of the resulted array.
***********************************************************************************/
CArrayReal CArrayReal::operator/(const CArrayReal& a_rhs) const
{
  return CArrayReal(*this).operator/=(a_rhs);
}

#endif

/* METHOD *************************************************************************/
/**
* @brief  Returns a reference to the stored element at the provided position.
*
* @note   The specified index has to be less than the storage capacity.
*
* @param a_u32Idx Index of the stored element [0, capacity-1]
*
* @return A reference to the stored element at the provided position.
***********************************************************************************/
CArrayReal::StorageType& CArrayReal::operator[](uint32 a_u32Idx)
{
  AC_ASSERT( a_u32Idx < getCapacity() );

  return getPtr()[a_u32Idx];
}

/* METHOD *************************************************************************/
/**
* @brief  Returns a reference to the stored element at the provided position.
*
* @note   The specified index has to be less than the storage capacity.
*
* @param a_u32Idx Index of the stored element [0, capacity-1]
*
* @return A reference to the stored element at the provided position.
***********************************************************************************/
const CArrayReal::StorageType& CArrayReal::operator[](uint32 a_u32Idx) const
{
  AC_ASSERT( a_u32Idx < getCapacity() );

  return getPtr()[a_u32Idx];
}

/* METHOD *************************************************************************/
/**
* @brief  Changes the size and the capacity if necessary.
*
*         Sets the size of the array to the provided size. If the capacity is less
*         than the provided size the storage will be resized.
*
* @param a_u32Size  New size of the array
***********************************************************************************/
void CArrayReal::setSize(uint32 a_u32Size)
{
  uint32 l_u32Capacity = getCapacity();  
  uint32 l_u32Size     = getSize();

  if ( a_u32Size > l_u32Capacity)
  {    
    bool  l_bResizeResult = resize( a_u32Size );
    AC_ASSERT( l_bResizeResult );
    AC_UNUSED( l_bResizeResult );
    
    RealType* l_pStorage = getPtr();

    for ( uint32 k = l_u32Size; k < a_u32Size; ++k )
      l_pStorage[k] = 0.0f;
  } 

  CArrayContainerIpp<RealType>::setSize( a_u32Size ); 
}

/* METHOD *************************************************************************/
/**
* @brief  append
*
***********************************************************************************/
AmlabCommon::CArrayReal& operator<<(AmlabCommon::CArrayReal& a_rhs, const AmlabCommon::CArrayReal& a_lhs)
{
  uint32    l_u32Offset = a_rhs.getSize();
  uint32    l_u32Size   = a_lhs.getSize();

  if ( a_rhs.getCapacity() < (l_u32Offset+l_u32Size) )
  {
    a_rhs.resize(l_u32Offset+l_u32Size);
  }

  RealType* l_pDst        = a_rhs.getPtr();
  const RealType* l_pSrc  = a_lhs.getPtr();

  memcpy(l_pDst+l_u32Offset, l_pSrc, l_u32Size*sizeof(RealType));
  a_rhs.setSize(l_u32Size+l_u32Offset);
  return a_rhs;
}


/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/
