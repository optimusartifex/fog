// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MATH_MATHVEC_H
#define _FOG_CORE_MATH_MATHVEC_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Ops.h>

namespace Fog {
namespace Math {

//! @addtogroup Fog_Core_Math
//! @{

// ============================================================================
// [Fog::Math - Cvt]
// ============================================================================

static FOG_INLINE void vFloatFromDouble(float* dst, const double* src, size_t length)
{
  _api.mathf.vecFloatFromDouble(dst, src, length);
}

static FOG_INLINE void vDoubleFromFloat(double* dst, const float* src, size_t length)
{
  _api.mathd.vecDoubleFromFloat(dst, src, length);
}

template<typename DstNumber, typename SrcNumber>
static FOG_INLINE void vConvertFloat(DstNumber* dst, const SrcNumber* src, size_t length)
{
  FOG_ASSERT(sizeof(DstNumber) == sizeof(SrcNumber));
  Memory::copy(dst, src, length * sizeof(DstNumber));
}

template<>
FOG_INLINE void vConvertFloat<double, float>(double* dst, const float* src, size_t length)
{
  _api.mathd.vecDoubleFromFloat(dst, src, length);
}

template<>
FOG_INLINE void vConvertFloat<float, double>(float* dst, const double* src, size_t length)
{
  _api.mathf.vecFloatFromDouble(dst, src, length);
}

//! @}

} // Math namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MATH_MATHVEC_H
