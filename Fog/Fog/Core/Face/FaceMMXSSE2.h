// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACEMMXSSE2_H
#define _FOG_CORE_FACE_FACEMMXSSE2_H

// [Dependencies]
#include <Fog/Core/C++/IntrinMMX.h>
#include <Fog/Core/C++/IntrinSSE.h>
#include <Fog/Core/Face/Features.h>

namespace Fog {
namespace Face {

// [Warnings]
#if defined(FOG_CC_MSC)
# pragma warning(push)
# pragma warning(disable:4799) // function '...' has no EMMS instruction.
#endif // FOG_CC_MSC

//! @addtogroup Fog_Face_MMX
//! @addtogroup Fog_Face_SSE2
//! @{

// ============================================================================
// [Fog::Face - MMX_SSE - Move]
// ============================================================================

static FOG_INLINE void m64iFromM128i(m64i& dst, const m128i& src)
{
  dst = _mm_movepi64_pi64(src);
}

static FOG_INLINE void m128iFromM64i(m128i& dst, const m64i& src)
{
  dst = _mm_movpi64_epi64(src);
}

//! @}

// [Warnings]
#if defined(FOG_CC_MSC)
# pragma warning(pop)
#endif // FOG_CC_MSC

// [Guard]
#endif // _FOG_CORE_FACE_FACEMMXSSE2_H
