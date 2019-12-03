
#pragma once

#include <cstdint>
#include "TypesExtended.h"

typedef uint32_t uint32_t;
typedef int Status;
typedef int RoundMode;
#define RndZero (RoundMode)0

#define StsNoErr  (Status)0

char * ippGetStatusString(Status ipps);

Status SwapBytes_uint32_t(uint32_t* pSrcDst, int len);
Status Convert_int16_t_float(const int16_t* pSrc, float* pDst, int len);
Status Convert_int32_t_float(const int32_t* pSrc, float* pDst, int len);
Status MulC_float(float val, float* pSrcDst, int len);
Status MulC_float(const float* pSrc, float val, float* pDst, int len);
Status RealToCplx_float(const float* pSrcRe, const float* pSrcIm, floatc* pDst, int len);
Status CplxToReal_floatc(const floatc* pSrc, float* pDstRe, float* pDstIm, int len);


Status Convert_float_int16_t_Sfs(const float* pSrc, int16_t* pDst, int len, RoundMode rndMode, int scaleFactor);
Status Convert_float_int32_t_Sfs(const float* pSrc, int32_t* pDst, int len, RoundMode rndMode, int scaleFactor);
