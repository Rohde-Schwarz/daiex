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
#include "ArrayComplex.h"
#include "AmlabCommon.h"

using namespace std;
using AmlabCommon::CArrayReal;
using AmlabCommon::CArrayComplex;

/* METHOD *************************************************************************/
/**
* @brief  Standard Constructor
***********************************************************************************/
CArrayComplex::CArrayComplex()
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
CArrayComplex::CArrayComplex(uint32 a_u32Capacity)
: CArrayContainerIpp<StorageType>(a_u32Capacity)
{

}

/* METHOD *************************************************************************/
/**
* @brief  Constructor with array initialization.
* 
*         Creates and initializes the array with the provided data.
*
* @param a_pInitData   Array with init data,
* @param a_u32Size     Number of elements in the provided array.
***********************************************************************************/
CArrayComplex::CArrayComplex(const StorageType* a_pInitData, uint32 a_u32Size)
: CArrayContainerIpp<StorageType>(a_pInitData, a_u32Size)
{

}

/* METHOD *************************************************************************/
/**
* @brief  Constructor with array initialization.
* 
*         Creates and initializes the array with the provided data. 
*
* @note   If one of the provided pointer is zero than the data will be assigned
*         a zero. If both pointers a zero, initialization will fail.
*
* @param a_pDataReal   Array with init data containing the real part of a complex number.
* @param a_pDataImag   Array with init data containing the imaginary part of a complex number.
* @param a_u32Size     Number of elements in the provided arrays.
***********************************************************************************/
CArrayComplex::CArrayComplex(const AR::StorageType* a_pDataReal, const AR::StorageType* a_pDataImag, uint32 a_u32Size)
{
  assign(a_pDataReal, a_pDataImag, a_u32Size);
}


/* METHOD *************************************************************************/
/**
* @brief  Constructor with array initialization.
* 
*         Creates and initializes the array with the provided data. Both provided
*         arrays have to be the same size.
*
* @param a_DataReal   Array container with init data containing the real part of a complex number.
* @param a_DataImag   Array container with init data containing the imaginary part of a complex number.
* @param a_u32Size     Number of elements in the provided arrays.
***********************************************************************************/
CArrayComplex::CArrayComplex(const CArrayReal& a_DataReal, const CArrayReal& a_DataImag)
{
  assign(a_DataReal, a_DataImag);
}

/* METHOD *************************************************************************/
/**
* @brief  Copy Constructor, makes a deep copy of the provided array.
***********************************************************************************/
CArrayComplex::CArrayComplex(const CArrayComplex& a_rhs)
: CArrayContainerIpp<StorageType>(a_rhs)
{

}

/* METHOD *************************************************************************/
/**
* @brief  Destructor.
***********************************************************************************/
CArrayComplex::~CArrayComplex()
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
bool CArrayComplex::operator==(const CArrayComplex& a_rhs) const
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
bool CArrayComplex::operator!=(const CArrayComplex& a_rhs) const
{
  return !operator==(a_rhs);
}

#ifndef ARRAY_REAL_REDUCED_METHOD_SET

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
CArrayComplex& CArrayComplex::operator=(const StorageType& a_fConstant)
{
  //Ipp::Set(a_fConstant, getPtr(), getSize());

  return *this;
}

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
CArrayComplex& CArrayComplex::operator=(const CArrayComplex & a_rhs)
{
  uint32 l_u32Size = a_rhs.getSize(); 

  // check if this array has enough capacity
  if (getCapacity() < l_u32Size)
    realloc(l_u32Size);

  // copy all elememts from rhs to lhs
  copy(getPtr(), a_rhs.getPtr(), l_u32Size);

  setSize(l_u32Size);

  return *this;
}

/* METHOD *************************************************************************/
/**
* @brief  Adds a constant to the elements in the array.
*   
* @param  a_fConstant A constant that is added to the stored elements.
*
* @return A reference to this array.
***********************************************************************************/
CArrayComplex& CArrayComplex::operator+=(const StorageType& a_fConstant)
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
CArrayComplex& CArrayComplex::operator-=(const StorageType& a_fConstant)
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
CArrayComplex& CArrayComplex::operator*=(const StorageType& a_fConstant)
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
CArrayComplex& CArrayComplex::operator/=(const StorageType& a_fConstant)
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
CArrayComplex CArrayComplex::operator+(const StorageType& a_fConstant) const
{
  return CArrayComplex(*this).operator+=(a_fConstant);
}

/* METHOD *************************************************************************/
/**
* @brief  Subtracts a constant from the elements in the array.
*   
* @param  a_fConstant A constant that is subtracted from the stored elements.
*
* @return A copy of the resulted array.
***********************************************************************************/
CArrayComplex CArrayComplex::operator-(const StorageType& a_fConstant) const
{
  return CArrayComplex(*this).operator-=(a_fConstant);
}

/* METHOD *************************************************************************/
/**
* @brief  Multiplies a constant to the elements in the array.
*   
* @param  a_fConstant A constant that is multiplied to the stored elements.
*
* @return A copy of the resulted array.
***********************************************************************************/
CArrayComplex CArrayComplex::operator*(const StorageType& a_fConstant) const
{
  return CArrayComplex(*this).operator*=(a_fConstant);
}

/* METHOD *************************************************************************/
/**
* @brief  Divides a constant from the elements in the array.
*   
* @param  a_fConstant A constant that is divided from the stored elements.
*
* @return A copy of the resulted array.
***********************************************************************************/
CArrayComplex CArrayComplex::operator/(const StorageType& a_fConstant) const
{
  return CArrayComplex(*this).operator/=(a_fConstant);
}

/* METHOD *************************************************************************/
/**
* @brief  Adds an array. Both arrays have to be the same size.
*   
* @param  a_rhs The other array that is added.
*
* @return A reference to this array.
***********************************************************************************/
CArrayComplex& CArrayComplex::operator+=(const CArrayComplex& a_rhs)
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
CArrayComplex& CArrayComplex::operator-=(const CArrayComplex& a_rhs)
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
CArrayComplex& CArrayComplex::operator*=(const CArrayComplex& a_rhs)
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
CArrayComplex& CArrayComplex::operator/=(const CArrayComplex& a_rhs)
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
CArrayComplex CArrayComplex::operator+(const CArrayComplex& a_rhs) const
{
  return CArrayComplex(*this).operator+=(a_rhs);
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
CArrayComplex CArrayComplex::operator-(const CArrayComplex& a_rhs) const
{
  return CArrayComplex(*this).operator-=(a_rhs);
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
CArrayComplex CArrayComplex::operator*(const CArrayComplex& a_rhs) const
{
  return CArrayComplex(*this).operator*=(a_rhs);
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
CArrayComplex CArrayComplex::operator/(const CArrayComplex& a_rhs) const
{
  return CArrayComplex(*this).operator/=(a_rhs);
}
#endif
/* METHOD *************************************************************************/
/**
* @brief  Assigns a real array to a complex array, the imaginary part of the
*         complex array is set to zero.
* 
*         To assign another array the capacity of the lhs must big enough to hold
*         all the elements of the rhs. Therefore before copying is done the storage
*         of lhs is reallocated if it is too small. 
*
* @param  a_rhs Another array that will be assigned.
*
* @return A reference to this array.
***********************************************************************************/
CArrayComplex& CArrayComplex::operator=(const CArrayReal& a_rhs)
{
  bool l_bSucceed = assign(a_rhs.getPtr(), 0, a_rhs.getSize());
  AC_ASSERT( l_bSucceed );
  AC_UNUSED( l_bSucceed );

  return *this;
}

/* METHOD *************************************************************************/
/**
* @brief  Assigns an const array to the storage.
* 
*         Creates and initializes the array with the provided data.
*
* @param a_pData    Array with complex values
* @param a_u32Size  Number of elements in the provided array.
*
* @return False if an memory allocation error occurs, while resizing the array.
***********************************************************************************/
bool CArrayComplex::assign(const StorageType* a_pData, uint32 a_u32Size)
{
  do 
  {
    bool  l_bReallocResult = realloc(a_u32Size);  // allocate the required size
    AC_ASSERT( l_bReallocResult );                // check if allocation was successful

    if (!l_bReallocResult)
      break;

    copy(getPtr(), a_pData, a_u32Size);

    setSize(a_u32Size);

    return true;
  } while(false);

  return false;
}

/* METHOD *************************************************************************/
/**
* @brief  Assigns an const array to the storage.
* 
* @note   If one of the provided pointer is zero than the data will be assigned
*         a zero. If both pointers a zero, initialization will fail.
*
* @param a_pDataReal   Array with init data containing the real part of a complex number.
* @param a_pDataImag   Array with init data containing the imaginary part of a complex number.
* @param a_u32Size     Number of elements in the provided arrays.
*
* @return False if an memory allocation error occurs, while resizing the array.
***********************************************************************************/
bool CArrayComplex::assign(const AR::StorageType* a_pDataReal, const AR::StorageType* a_pDataImag, uint32 a_u32Size)
{
  do 
  {
    bool  l_bReallocResult = realloc(a_u32Size);  // allocate the required size
    AC_ASSERT( l_bReallocResult );                // check if allocation was successful

    if (!l_bReallocResult)
      break;
    if (StsNoErr != RealToCplx_float(a_pDataReal, a_pDataImag, getPtr(), a_u32Size))
    {
      setSize(0);
      break;
    }
    setSize(a_u32Size);

    return true;
  } while(false);

  return false;
}

/* METHOD *************************************************************************/
/**
* @brief  Assigns two real arrays to this complex array.
* 
* @note   Both provided arrays have to be the same size.
*
* @param a_DataReal   Array container with init data containing the real part of a complex number.
* @param a_DataImag   Array container with init data containing the imaginary part of a complex number.
* @param a_u32Size     Number of elements in the provided arrays.
***********************************************************************************/
bool CArrayComplex::assign(const CArrayReal& a_DataReal, const CArrayReal& a_DataImag)
{
  AC_ASSERT(a_DataReal.getSize() == a_DataImag.getSize());

  return assign(a_DataReal.getPtr(), a_DataImag.getPtr(), a_DataReal.getSize());
}

/* METHOD *************************************************************************/
/**
* @brief  Extracts the real and imaginary part of the complex array.
*
* @param a_DataReal Result array where the extracted real part will be assigned to.
* @param a_DataImag Result array where the extracted imaginary part will be assigned to.
*
* @return False if an memory allocation error occurs, while resizing the result arrays.
***********************************************************************************/
bool CArrayComplex::toReal(CArrayReal& a_DataReal, CArrayReal& a_DataImag) const
{
  do 
  {
    uint32  l_u32Size = getSize();

    // Resize the destination array(real)
    if (a_DataReal.getCapacity() < l_u32Size)
      if (!a_DataReal.realloc(l_u32Size))
        break;

    // Resize the destination array(imag)
    if (a_DataImag.getCapacity() < l_u32Size)
      if (!a_DataImag.realloc(l_u32Size))
        break;      

    // split the complex array into real- and imaginary part
    if (StsNoErr != CplxToReal_floatc(getPtr(), a_DataReal.getPtr(), a_DataImag.getPtr(), l_u32Size))
      break;
    a_DataReal.setSize(l_u32Size);
    a_DataImag.setSize(l_u32Size);

    return true;
  } while(false);

  return false;
}

/* METHOD *************************************************************************/
/**
* @brief  Extracts the real part of the complex array.
*
* @note   For better performance use toReal() if possible.
*
* @param a_Data Result array where the extracted values will be assigned to.
*
* @return False if an memory allocation error occurs, while resizing the result array.
***********************************************************************************/
bool CArrayComplex::getReal(CArrayReal& a_Data) const
{
  do 
  {
    uint32  l_u32Size = getSize();

    // Resize the destination array(real)
    if (a_Data.getCapacity() < l_u32Size)
      if (!a_Data.realloc(l_u32Size))
        break;

    RealType* m_pDataOut = a_Data.getPtr();
    CplxType* m_pDataIn  = getPtr();

    for (unsigned k = 0; k < l_u32Size; ++k)
      m_pDataOut[k] = m_pDataIn[k].re;

    a_Data.setSize(l_u32Size);

    return true;
  } while(false);

  return false;
}

/* METHOD *************************************************************************/
/**
* @brief  Extracts the imaginary part of the complex array.
*
* @note   For better performance use toReal() if possible.
*
* @param a_Data Result array where the extracted values will be assigned to.
*
* @return False if an memory allocation error occurs, while resizing the result array.
***********************************************************************************/
bool CArrayComplex::getImag(CArrayReal& a_Data) const
{
  do 
  {
    uint32  l_u32Size = getSize();

    // Resize the destination array(real)
    if (a_Data.getCapacity() < l_u32Size)
      if (!a_Data.realloc(l_u32Size))
        break;

    RealType* m_pDataOut = a_Data.getPtr();
    CplxType* m_pDataIn  = getPtr();

    for (unsigned k = 0; k < l_u32Size; ++k)
      m_pDataOut[k] = m_pDataIn[k].im;

    a_Data.setSize(l_u32Size);

    return true;
  } while(false);

  return false;
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
void CArrayComplex::setSize(uint32 a_u32Size)
{
  uint32 l_u32Capacity = getCapacity();  
  uint32 l_u32Size     = getSize();

  if ( a_u32Size > l_u32Capacity)
  { 
    bool  l_bResizeResult = resize( a_u32Size );
    AC_ASSERT( l_bResizeResult );
    AC_UNUSED( l_bResizeResult );

    CplxType* l_pStorage = getPtr();

    for ( uint32 k = l_u32Size; k < a_u32Size; ++k )
    {
      l_pStorage[k].re = 0.0f;
      l_pStorage[k].im = 0.0f;
    }
  } 

  CArrayContainerIpp<CplxType>::setSize( a_u32Size );
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
CArrayComplex::StorageType& CArrayComplex::operator[](uint32 a_u32Idx)
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
const CArrayComplex::StorageType& CArrayComplex::operator[](uint32 a_u32Idx) const
{
  AC_ASSERT( a_u32Idx < getCapacity() );

  return getPtr()[a_u32Idx];
}

/* METHOD *************************************************************************/
/**
* @brief  appends buffer shifted to other
*
* @note   
*
* @param 
*
* @return 
***********************************************************************************/
AmlabCommon::CArrayComplex& operator<<(AmlabCommon::CArrayComplex& rhs, const AmlabCommon::CArrayComplex& lhs)
{
  uint32    l_u32Offset = rhs.getSize();
  uint32    l_u32Size   = lhs.getSize();

  if ( rhs.getCapacity() < (l_u32Offset+l_u32Size) )
  {
    rhs.resize(l_u32Offset+l_u32Size);
  }

  CplxType* l_pDst        = rhs.getPtr();
  const CplxType* l_pSrc  = lhs.getPtr();

  memcpy(l_pDst+l_u32Offset, l_pSrc, l_u32Size*sizeof(CplxType));
  rhs.setSize(l_u32Size+l_u32Offset);
  return rhs;
}

/**********************************************************************************/
/* END OF FILE                                                                    */
/**********************************************************************************/
