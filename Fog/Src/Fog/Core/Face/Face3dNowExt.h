// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACE3DNOWEXT_H
#define _FOG_CORE_FACE_FACE3DNOWEXT_H

// [Dependencies]
#include <Fog/Core/Face/Face3dNow.h>

namespace Fog {
namespace Face {

// [Warnings]
#if defined(FOG_CC_MSC)
# pragma warning(push)
# pragma warning(disable:4799) // function '...' has no EMMS instruction.
#endif // FOG_CC_MSC

//! @addtogroup Fog_Face_3dNow
//! @{

// ============================================================================
// [Fog::Face - 3dNow - Convert]
// ============================================================================

static FOG_INLINE void m64fFromM64I16(__m64& dst, const __m64& a)
{
  dst = _m_pi2fw(a);
}

static FOG_INLINE void m64fToM64I16(__m64& dst, const __m64& a)
{
  dst = _m_pf2iw(a);
}

// ============================================================================
// [Fog::Face - 3dNow - Sub]
// ============================================================================

//! @brief Horizontal Subtraction (3dNow Negative Accumulation).
//!
//! @verbatim
//! dst[ 0-31] = a[0-31] - a[32-63]
//! dst[32-63] = b[0-31] - b[32-63]
//! @endverbatim
static FOG_INLINE void m64fHSub(__m64& dst, const __m64& a, const __m64& b)
{
  dst = _m_pfnacc(a, b);
}

// ============================================================================
// [Fog::Face - 3dNow - AddSub]
// ============================================================================

//! @brief Horizontal Addition and Subtraction (3dNow Positive/Negative Accumulation).
//!
//! @verbatim
//! dst[ 0-31] = a[0-31] + a[32-63]
//! dst[32-63] = b[0-31] - b[32-63]
//! @endverbatim
static FOG_INLINE void m64fHAddSub(__m64& dst, const __m64& a, const __m64& b)
{
  dst = _m_pfpnacc(a, b);
}

//! @}

// [Warnings]
#if defined(FOG_CC_MSC)
# pragma warning(pop)
#endif // FOG_CC_MSC

} // Face namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_FACE_FACE3DNOWEXT_H
