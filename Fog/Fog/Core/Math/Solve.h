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
#include <Fog/Core/Math/Math.h>

namespace Fog {
namespace Math {

//! @addtogroup Fog_Core_Math
//! @{

// ============================================================================
// [Fog::Math - SolveQuadraticFunction]
// ============================================================================

//! @brief Solve the quadratic function and return the roots.
//!
//! The form of quadratic function:
//!
//!   Ax^2 + Bx + C == 0
//!
//! The form of arguments:
//!
//!   A == src[0]
//!   B == src[1]
//!   C == src[2]
static FOG_INLINE int solveQuadraticFunction(float* dst, const float* src)
{
  return _core.mathf.solveQuadraticFunction(dst, src);
}

//! @overload
static FOG_INLINE int solveQuadraticFunction(double* dst, const double* src)
{
  return _core.mathd.solveQuadraticFunction(dst, src);
}

// ============================================================================
// [Fog::Math - SolveCubicFunction]
// ============================================================================

//! @brief Solve the cubic function and return the roots.
//!
//! The form of cubic function:
//!
//!   Ax^3 + Bx^2 + Cx + D == 0
//!
//! The form of arguments:
//!
//!   A == src[0]
//!   B == src[1]
//!   C == src[2]
//!   D == src[3]
static FOG_INLINE int solveCubicFunction(float* dst, const float* src)
{
  return _core.mathf.solveCubicFunction(dst, src);
}

//! @overload
static FOG_INLINE int solveCubicFunction(double* dst, const double* src)
{
  return _core.mathd.solveCubicFunction(dst, src);
}

//! @}

} // Math namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MATH_SOLVE_H
