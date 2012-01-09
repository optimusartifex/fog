// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACEMMX_H
#define _FOG_CORE_FACE_FACEMMX_H

// [Dependencies]
#include <Fog/Core/C++/IntrinMMX.h>
#include <Fog/Core/Face/Features.h>

namespace Fog {
namespace Face {

// [Warnings]
#if defined(FOG_CC_MSC)
# pragma warning(push)
# pragma warning(disable:4799) // function '...' has no EMMS instruction.
#endif // FOG_CC_MSC

//! @addtogroup Fog_Face_MMX
//! @{

// ============================================================================
// [Fog::Face - MMX - Constants]
// ============================================================================



// ============================================================================
// [Fog::Face - MMX - Load/Store]
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

} // Face namespace
} // Fog namespace

// MMX+SSE extensions require both, FaceMMX.h, and FaceSSE.h headers.
#if defined(_FOG_CORE_FACE_FACESSE_H)
# include <Fog/Core/Face/FaceMMXSSE.h>
#endif // _FOG_CORE_FACE_FACESSE_H

// MMX+SSE2 extensions require both, FaceMMX.h, and FaceSSE2.h headers.
#if defined(_FOG_CORE_FACE_FACESSE2_H)
# include <Fog/Core/Face/FaceMMXSSE2.h>
#endif // _FOG_CORE_FACE_FACESSE2_H

// MMX+SSSE3 extensions require both, FaceMMX.h, and FaceSSSE3.h headers.
#if defined(_FOG_CORE_FACE_FACESSSE3_H)
# include <Fog/Core/Face/FaceMMXSSSE3.h>
#endif // _FOG_CORE_FACE_FACESSSE3_H

// [Guard]
#endif // _FOG_CORE_FACE_FACEMMX_H
