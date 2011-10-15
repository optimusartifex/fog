// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MATH_INTEGRATE_H
#define _FOG_CORE_MATH_INTEGRATE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Interval.h>
#include <Fog/Core/Math/Math.h>

namespace Fog {
namespace Math {

//! @addtogroup Fog_Core_Math
//! @{

// ============================================================================
// [Fog::Math - Integrate]
// ============================================================================

//! @brief Integrate function @a f at interval @a interval using method @a method.
static FOG_INLINE err_t integrate(float* dst, const FunctionF& f, const IntervalF& interval, uint32_t method, uint32_t steps)
{
  FOG_ASSERT(method < MATH_INTEGRATION_METHOD_COUNT);
  return fog_api.mathf_integrate[method](dst, &f, &interval, steps);
}

//! @overload
static FOG_INLINE err_t integrate(double* dst, const FunctionD& f, const IntervalD& interval, uint32_t method, uint32_t steps)
{
  FOG_ASSERT(method < MATH_INTEGRATION_METHOD_COUNT);
  return fog_api.mathd_integrate[method](dst, &f, &interval, steps);
}

//! @}

} // Math namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MATH_INTEGRATE_H
