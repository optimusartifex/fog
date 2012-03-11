// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_ACC_ACCMMX_H
#define _FOG_CORE_ACC_ACCMMX_H

// [Dependencies]
#include <Fog/Core/C++/IntrinMmx.h>

namespace Fog {
namespace Acc {

// [Warnings]
#if defined(FOG_CC_MSC)
# pragma warning(push)
# pragma warning(disable:4799) // function '...' has no EMMS instruction.
#endif // FOG_CC_MSC

//! @addtogroup Fog_Core_Acc_Mmx
//! @{

// ============================================================================
// [Fog::Acc - MMX - Constants]
// ============================================================================



// ============================================================================
// [Fog::Acc - MMX - Load/Store]
// ============================================================================

template<typename T>
static FOG_INLINE void m64iLoad4(__m64& dst0, const T* srcp)
{
  dst0 = _mm_cvtsi32_si64(reinterpret_cast<const int*>(srcp)[0]);
}

template<typename T>
static FOG_INLINE void m64iLoad8(__m64& dst0, const T* srcp)
{
  dst0 = reinterpret_cast<const __m64*>(srcp)[0];
}

template<typename T>
static FOG_INLINE void m64iStore4(T* dstp, const __m64& src0)
{
  reinterpret_cast<int*>(dstp)[0] = _mm_cvtsi64_si32(src0);
}

template<typename T>
static FOG_INLINE void m64iStore8(T* dstp, const __m64& src0)
{
  reinterpret_cast<__m64*>(dstp)[0] = src0;
}

//! @}

// [Warnings]
#if defined(FOG_CC_MSC)
# pragma warning(pop)
#endif // FOG_CC_MSC

} // Acc namespace
} // Fog namespace

// MMX+SSE extensions require both, FaceMMX.h, and FaceSSE.h headers.
#if defined(_FOG_CORE_ACC_ACCSSE_H)
# include <Fog/Core/Acc/AccMmxSse.h>
#endif // _FOG_CORE_ACC_ACCSSE_H

// MMX+SSE2 extensions require both, FaceMMX.h, and FaceSSE2.h headers.
#if defined(_FOG_CORE_ACC_ACCSSE2_H)
# include <Fog/Core/Acc/AccMmxSse2.h>
#endif // _FOG_CORE_ACC_ACCSSE2_H

// MMX+SSSE3 extensions require both, FaceMMX.h, and FaceSSSE3.h headers.
#if defined(_FOG_CORE_ACC_ACCSSSE3_H)
# include <Fog/Core/Acc/AccMmxSsse3.h>
#endif // _FOG_CORE_ACC_ACCSSSE3_H

// [Guard]
#endif // _FOG_CORE_ACC_ACCMMX_H
