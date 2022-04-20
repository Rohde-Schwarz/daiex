#pragma once

// set target cpu for IPC11/4
#pragma GCC push_options
#pragma GCC target ("sse4.2")

// enable/disable intel intrinsics for IPC11/4
#define IQBITCONV_ENABLE_INTRINSICS

#include <stdio.h>
#include <stdint.h>
#include <math.h>

#ifdef IQBITCONV_ENABLE_INTRINSICS
#include <immintrin.h>
#endif

#ifdef __cplusplus
namespace IQW
{
#endif

#define DIGIQ_WORD_SIZE                  32     /* 256 bit = 32 byte */
#define SAMPLES_REAL12_PER_DIGIQ_WORD    20     /* real samples per 256 bit */
#define SAMPLES_COMPLEX12_PER_DIGIQ_WORD 10     /* complex samples per 256 bit */
#define SAMPLES_REAL16_PER_DIGIQ_WORD    16     /* real 12 bit samples per 256 bit */
#define SAMPLES_COMPLEX16_PER_DIGIQ_WORD 8      /* complex 12 bit samples per 256 bit */

#ifdef __cplusplus
class IqBitConverter
{
public:
#endif

/******************************************************************************
* calculate destination size for 12 to 16 bit conversion from count source bytes
* on success, the number of destination bytes is returned
* on error, -1 is returned
******************************************************************************/
static inline ssize_t conv12to16_dstsize(size_t count)
{
  if (count % DIGIQ_WORD_SIZE)
  {
    return -1;
  }
  return count / DIGIQ_WORD_SIZE * SAMPLES_REAL12_PER_DIGIQ_WORD * sizeof(uint16_t);
}

/******************************************************************************
* calculate source size for 12 to 16 bit conversion from num samples
* the number of source bytes is returned
* this might actually match more than num samples to make the source size
* match an integral multiple of the diqiq word size
******************************************************************************/
static inline ssize_t conv12to16_srcsize_from_samplenum(size_t num)
{
  return (ssize_t)(ceil((float)num / SAMPLES_COMPLEX12_PER_DIGIQ_WORD) * DIGIQ_WORD_SIZE);
}

/******************************************************************************
* calculate source size for 16 to 12 bit conversion from count destination bytes
* on success, the number of destination bytes is returned
* on error, -1 is returned
******************************************************************************/
static inline ssize_t conv16to12_srcsize(size_t count)
{
  if (count % DIGIQ_WORD_SIZE)
  {
    return -1;
  }
  else
  {
    return (ssize_t)(count / DIGIQ_WORD_SIZE * SAMPLES_REAL12_PER_DIGIQ_WORD * sizeof(uint16_t));
  }
}

#ifndef IQBITCONV_ENABLE_INTRINSICS
/******************************************************************************
* convert count src bytes of 12 bit digiq data to 16 bit dst by using arrays
* count must be a multiple of 32 byte
* on success, the number of src bytes processed is returned
* on error, -1 is returned
******************************************************************************/
static inline ssize_t __attribute__((optimize("Ofast"))) conv12to16(const void* src, const void* dst, size_t count)
{
  uint8_t* src8 = (uint8_t*)src;
  uint16_t* dst16 = (uint16_t*)dst;

  if (count % DIGIQ_WORD_SIZE)
  {
    return -1;
  }

  // frame loop (process half a DIGIQ 256 bit word per loop)
  for (src8 = (uint8_t*)src; src8 < (uint8_t*)src + count; src8 += DIGIQ_WORD_SIZE / 2)
  {
    dst16[0] = *(uint16_t*)&src8[0] << 4;
    dst16[1] = *(uint16_t*)&src8[1] & 0xFFF0;
    dst16[2] = *(uint16_t*)&src8[3] << 4;
    dst16[3] = *(uint16_t*)&src8[4] & 0xFFF0;
    dst16[4] = *(uint16_t*)&src8[6] << 4;
    dst16[5] = *(uint16_t*)&src8[7] & 0xFFF0;
    dst16[6] = *(uint16_t*)&src8[9] << 4;
    dst16[7] = *(uint16_t*)&src8[10] & 0xFFF0;
    dst16[8] = *(uint16_t*)&src8[12] << 4;
    dst16[9] = *(uint16_t*)&src8[13] & 0xFFF0;

    dst16 += SAMPLES_REAL12_PER_DIGIQ_WORD / 2;
  }
  return count;
}
#else
/******************************************************************************
* convert count src bytes of 12 bit digiq data to 16 bit dst by using intrinsics
* count must be a multiple of 32 byte
* on success, the number of src bytes processed is returned
* on error, -1 is returned
******************************************************************************/
static inline ssize_t conv12to16(const void* src, const void* dst, size_t count)
{
  uint8_t* src8 = (uint8_t*)src;
  uint16_t* dst16 = (uint16_t*)dst;
  __m128i tmp128a, tmp128b, tmp128c, shuffle_mask, and_mask;

  if (count % DIGIQ_WORD_SIZE)
  {
    return -1;
  }

  shuffle_mask = _mm_setr_epi8(0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 10, 11);
  and_mask = _mm_setr_epi16(
    (short)0xFFFF, (short)0xFFF0, (short)0xFFFF, (short)0xFFF0,
    (short)0xFFFF, (short)0xFFF0, (short)0xFFFF, (short)0xFFF0);

  // frame loop (process half a DIGIQ 256 bit word per loop)
  for (src8 = (uint8_t*)src; src8 < (uint8_t*)src + count; src8 += DIGIQ_WORD_SIZE / 2)
  {
    // load and reorder bytes
    tmp128a = _mm_load_si128((__m128i*)src8); // SSE2
    tmp128b = _mm_shuffle_epi8(tmp128a, shuffle_mask); // SSSE3
    // left shift 16bit integers by 4 bit (needed on even indices)
    tmp128a = _mm_slli_epi16(tmp128b, 4); // SSE2
    // interleave 16bit integers from even and odd indices
    tmp128c = _mm_blend_epi16(tmp128a, tmp128b, 0xAA);  // SSE4.1
    // clear lower nibble on odd indices
    tmp128a = _mm_and_si128(tmp128c, and_mask);  // SSE2
    // store
    _mm_storeu_si128((__m128i*)dst16, tmp128a);  // SSE2
    // convert indices 8..9
    dst16[8] = *(uint16_t*)&src8[12] << 4;
    dst16[9] = *(uint16_t*)&src8[13] & 0xFFF0;

    dst16 += SAMPLES_REAL12_PER_DIGIQ_WORD / 2;
  }
  return count;
}
#endif

/******************************************************************************
* convert count src bytes of 2x12=24 bit digiq data to 32 bit lsb right aligned
* dst by using arrays (e.g. or interpreting fpga test counters)
* count must be a multiple of 32 byte
* on success, the number of src bytes processed is returned
* on error, -1 is returned
******************************************************************************/
static inline ssize_t conv2x12to32r_array(const void* src, const void* dst, size_t count)
{
  uint8_t* src8 = (uint8_t*)src;
  uint32_t* dst32 = (uint32_t*)dst;

  if (count % DIGIQ_WORD_SIZE)
  {
    return -1;
  }

  // frame loop (process half a DIGIQ 256 bit word per loop)
  for (src8 = (uint8_t*)src; src8 < (uint8_t*)src + count; src8 += DIGIQ_WORD_SIZE / 2)
  {
    dst32[0] = *(uint32_t*)&src8[0] & 0x00FFFFFF;
    dst32[1] = *(uint32_t*)&src8[3] & 0x00FFFFFF;
    dst32[2] = *(uint32_t*)&src8[6] & 0x00FFFFFF;
    dst32[3] = *(uint32_t*)&src8[9] & 0x00FFFFFF;
    dst32[4] = *(uint32_t*)&src8[12] & 0x00FFFFFF;

    dst32 += SAMPLES_REAL12_PER_DIGIQ_WORD / 4;
  }
  return count;
}

/******************************************************************************
* convert count src bytes of 16 bit digiq data to 12 bit dst by using arrays
* count must be a multiple of SAMPLES_REAL12_PER_DIGIQ_WORD * sizeof(uint16_t)
* = 40 byte
* on success, the number of src bytes processed is returned
* on error, -1 is returned
******************************************************************************/
static inline ssize_t conv16to12_array(const void* src, const void* dst, size_t count)
{
  uint8_t* dst8 = (uint8_t*)dst;
  uint16_t* src16 = (uint16_t*)src;

  if (count % (SAMPLES_REAL12_PER_DIGIQ_WORD * sizeof(uint16_t)))
  {
    return -1;
  }

  // frame loop (process half a DIGIQ 256 bit word per loop)
  for (src16 = (uint16_t*)src; src16 < (uint16_t*)src + (count / 2); src16 += SAMPLES_REAL12_PER_DIGIQ_WORD / 2)
  {
    *(uint32_t*)&dst8[0] = src16[0] >> 4 | ((src16[1] & 0xFFF0) << 8);
    *(uint32_t*)&dst8[3] = src16[2] >> 4 | ((src16[3] & 0xFFF0) << 8);
    *(uint32_t*)&dst8[6] = src16[4] >> 4 | ((src16[5] & 0xFFF0) << 8);
    *(uint32_t*)&dst8[9] = src16[6] >> 4 | ((src16[7] & 0xFFF0) << 8);
    *(uint32_t*)&dst8[12] = src16[8] >> 4 | ((src16[9] & 0xFFF0) << 8);
    dst8[15] = 0;

    dst8 += DIGIQ_WORD_SIZE / 2;
  }
  return count;
}

#ifdef __cplusplus
};
#endif

#ifdef __cplusplus
} // namespace
#endif

#pragma GCC push_options
