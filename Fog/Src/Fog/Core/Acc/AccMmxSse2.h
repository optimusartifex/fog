// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_ACC_ACCMMXSSE2_H
#define _FOG_CORE_ACC_ACCMMXSSE2_H

// [Dependencies]
#include <Fog/Core/C++/IntrinMmx.h>
#include <Fog/Core/C++/IntrinSse.h>

namespace Fog {
namespace Acc {

// [Warnings]
#if defined(FOG_CC_MSC)
# pragma warning(push)
# pragma warning(disable:4799) // function '...' has no EMMS instruction.
#endif // FOG_CC_MSC

//! @addtogroup Fog_Core_Acc_Mmx
//! @addtogroup Fog_Core_Acc_Sse2
//! @{

// ============================================================================
// [Fog::Acc - MMX_SSE - Move]
// ============================================================================

static FOG_INLINE void m64iFromM128i(__m64& dst, const __m128i& src)
{
  dst = _mm_movepi64_pi64(src);
}

static FOG_INLINE void m128iFromM64i(__m128i& dst, const __m64& src)
{
  dst = _mm_movpi64_epi64(src);
}

//! @}

// [Warnings]
#if defined(FOG_CC_MSC)
# pragma warning(pop)
#endif // FOG_CC_MSC

// [Guard]
#endif // _FOG_CORE_ACC_ACCMMXSSE2_H
