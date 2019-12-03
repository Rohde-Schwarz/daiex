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

#include "TypesExtended.h"
#include "replacement.h"

const char* DefaultStatus = "no error";
const char* ErrorStatus = "unknown error";


char* ippGetStatusString(Status ipps)
{
  if (ipps == 0)
  {
    return (char *)DefaultStatus;
  }
  else
  {
    return (char *)ErrorStatus;
  }
}

uint32_t swap_uint32(uint32_t val)
{
  val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
  return (val << 16) | (val >> 16);
}

Status SwapBytes_uint32_t(uint32_t* pSrcDst, int len)
{
  for (int i = 0; i < len; i++)
  {
    pSrcDst[i] = swap_uint32(pSrcDst[i]);
  }
  return StsNoErr;
}

Status Convert_int16_t_float(const int16_t* pSrc, float* pDst, int len)
{
  for (int i = 0; i < len; i++)
  {
    pDst[i] = (float)pSrc[i];
  }
  return StsNoErr;
}

Status Convert_int32_t_float(const int32_t * pSrc, float * pDst, int len)
{
  for (int i = 0; i < len; i++)
  {
    pDst[i] = (float) pSrc[i];
  }
  return StsNoErr;
}

Status MulC_float(float val, float * pSrcDst, int len)
{
  for (int i = 0; i < len; i++)
  {
    pSrcDst[i] *= val;
  }
  return StsNoErr;
}

Status MulC_float(const float * pSrc, float val, float * pDst, int len)
{
  for (int i = 0; i < len; i++)
  {
    pDst[i] = pSrc[i] * val;
  }
  return StsNoErr;
}


Status RealToCplx_float(const float* pSrcRe, const float* pSrcIm, floatc* pDst, int len)
{
  for (int i = 0; i < len; i++)
  {
    pDst[i].re = pSrcRe[i];
    pDst[i].im = pSrcIm[i];
  }
  return StsNoErr;
}

Status CplxToReal_floatc(const floatc* pSrc, float* pDstRe, float* pDstIm, int len) 
{
  for (int i = 0; i < len; i++)
  {
    pDstRe[i] = pSrc[i].re;
    pDstIm[i] = pSrc[i].im;
  }
  return StsNoErr;
}


Status Convert_float_int16_t_Sfs(const float* pSrc, int16_t* pDst, int len, RoundMode /*rndMode*/, int /*scaleFactor*/)
{
  for (int i = 0; i < len; i++)
  {
    pDst[i] = (int16_t)pSrc[i];
  }
  return StsNoErr;
}

Status Convert_float_int32_t_Sfs(const float* pSrc, int32_t* pDst, int len, RoundMode /*rndMode*/, int /*scaleFactor*/)
{
  for (int i = 0; i < len; i++)
  {
    pDst[i] = (int32_t)pSrc[i];

  }
  return StsNoErr;
}

