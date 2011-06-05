// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MATH_SOLVE_H
#define _FOG_CORE_MATH_SOLVE_H

// [Dependencies]
#include <Fog/Core/Global/Api.h>
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Interval.h>
#include <Fog/Core/Math/Math.h>

namespace Fog {
namespace Math {

//! @addtogroup Fog_Core_Math
//! @{

// ============================================================================
// [Fog::Math - SolveQuadraticFunction]
// ============================================================================

//! @brief Solve the quadratic or cubic function and return the roots.
//!
//! Quadratic function:
//!
//!   Ax^2 + Bx + C == 0
//!
//!   A == func[0]
//!   B == func[1]
//!   C == func[2]
//!
//! Cubic function:
//!
//!   Ax^3 + Bx^2 + Cx + D == 0
//!
//!   A == func[0]
//!   B == func[1]
//!   C == func[2]
//!   D == func[3]
static FOG_INLINE int solve(float* dst, const float* func, uint32_t type)
{
  FOG_ASSERT(type < MATH_SOLVE_COUNT);
  return _core.mathf.solve[type](dst, func);
}

//! @overload
static FOG_INLINE int solve(double* dst, const double* func, uint32_t type)
{
  return _core.mathd.solve[type](dst, func);
}

static FOG_INLINE int solve(float* dst, const float* func, uint32_t type, const IntervalF& interval)
{
  FOG_ASSERT(type < MATH_SOLVE_COUNT);
  return _core.mathf.solveAt[type](dst, func, interval);
}

//! @overload
static FOG_INLINE int solve(double* dst, const double* func, uint32_t type, const IntervalD& interval)
{
  return _core.mathd.solveAt[type](dst, func, interval);
}

//! @}

} // Math namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MATH_SOLVE_H
