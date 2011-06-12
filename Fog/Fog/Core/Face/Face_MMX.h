// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACE_MMX_H
#define _FOG_CORE_FACE_FACE_MMX_H

// [Dependencies]
#include <Fog/Core/Cpu/Intrin_MMX.h>
#include <Fog/Core/Face/Features.h>

namespace Fog {
namespace Face {

// [Warnings]
#if defined(FOG_CC_MSC)
#pragma warning(push)
#pragma warning(disable:4799) // function '...' has no EMMS instruction.
#endif // FOG_CC_MSC

//! @addtogroup Fog_Face_MMX
//! @{

// ============================================================================
// [Fog::Face - MMX - Types]
// ============================================================================

typedef __m64 m64;
typedef __m64 m64i;
typedef __m64 m64f;

// ============================================================================
// [Fog::Face - MMX - Constants]
// ============================================================================



// ============================================================================
// [Fog::Face - MMX - Load/Store]
// ============================================================================

template<typename T>
static FOG_INLINE void m64i_load4(m64i& dst0, const T* srcp)
{
  dst0 = _mm_cvtsi32_si64(reinterpret_cast<const int*>(srcp)[0]);
}

template<typename T>
static FOG_INLINE void m64i_load8(m64i& dst0, const T* srcp)
{
  dst0 = reinterpret_cast<const m64i*>(srcp)[0];
}

template<typename T>
static FOG_INLINE void m64i_store4(T* dstp, const m64i& src0)
{
  reinterpret_cast<int*>(dstp)[0] = _mm_cvtsi64_si32(src0);
}

template<typename T>
static FOG_INLINE void m64i_store8(T* dstp, const m64i& src0)
{
  reinterpret_cast<m64i*>(dstp)[0] = src0;
}

//! @}

// [Warnings]
#if defined(FOG_CC_MSC)
#pragma warning(pop)
#endif // FOG_CC_MSC

} // Face namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_FACE_FACE_MMX_H
