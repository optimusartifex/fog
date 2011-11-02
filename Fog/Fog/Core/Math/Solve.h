// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MATH_SOLVE_H
#define _FOG_CORE_MATH_SOLVE_H

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
// [Fog::Math - SolvePolynomial]
// ============================================================================

//! @brief Solve linear, quadratic, cubic, quartic, or higher degree polynomial
//! function.
//!
//! Linear function:
//!
//!   Ax + B == 0
//!
//!   A == polynomial[0]
//!   B == polynomial[1]
//!
//! Quadratic function:
//!
//!   Ax^2 + Bx + C == 0
//!
//!   A == polynomial[0]
//!   B == polynomial[1]
//!   C == polynomial[2]
//!
//! Cubic function:
//!
//!   Ax^3 + Bx^2 + Cx + D == 0
//!
//!   A == polynomial[0]
//!   B == polynomial[1]
//!   C == polynomial[2]
//!   D == polynomial[3]
//!
//! Quartic function:
//!
//!   Ax^4 + Bx^3 + Cx^2 + Dx + E == 0
//!
//!   A == polynomial[0]
//!   B == polynomial[1]
//!   C == polynomial[2]
//!   D == polynomial[3]
//!   E == polynomial[4]
static FOG_INLINE int solvePolynomial(float* dst, const float* polynomial, uint32_t degree)
{
  if (degree <= MATH_POLYNOMIAL_DEGREE_MAX_ANALYTIC)
    return fog_api.mathf_solvePolynomialA[degree](dst, polynomial, NULL);
  else
    return fog_api.mathf_solvePolynomialN[MATH_POLYNOMIAL_SOLVE_DEFAULT](dst, polynomial, degree, NULL);
}

//! @overload
static FOG_INLINE int solvePolynomial(double* dst, const double* polynomial, uint32_t degree)
{
  if (degree <= MATH_POLYNOMIAL_DEGREE_MAX_ANALYTIC)
    return fog_api.mathd_solvePolynomialA[degree](dst, polynomial, NULL);
  else
    return fog_api.mathd_solvePolynomialN[MATH_POLYNOMIAL_SOLVE_DEFAULT](dst, polynomial, degree, NULL);
}

static FOG_INLINE int solvePolynomial(float* dst, const float* polynomial, uint32_t degree, const IntervalF& interval)
{
  if (degree <= MATH_POLYNOMIAL_DEGREE_MAX_ANALYTIC)
    return fog_api.mathf_solvePolynomialA[degree](dst, polynomial, &interval);
  else
    return fog_api.mathf_solvePolynomialN[MATH_POLYNOMIAL_SOLVE_DEFAULT](dst, polynomial, degree, &interval);
}

//! @overload
static FOG_INLINE int solvePolynomial(double* dst, const double* polynomial, uint32_t degree, const IntervalD& interval)
{
  if (degree <= MATH_POLYNOMIAL_DEGREE_MAX_ANALYTIC)
    return fog_api.mathd_solvePolynomialA[degree](dst, polynomial, &interval);
  else
    return fog_api.mathd_solvePolynomialN[MATH_POLYNOMIAL_SOLVE_DEFAULT](dst, polynomial, degree, &interval);
}

static FOG_INLINE int solvePolynomial(float* dst, const float* polynomial, uint32_t degree, const IntervalF* interval)
{
  if (degree <= MATH_POLYNOMIAL_DEGREE_MAX_ANALYTIC)
    return fog_api.mathf_solvePolynomialA[degree](dst, polynomial, interval);
  else
    return fog_api.mathf_solvePolynomialN[MATH_POLYNOMIAL_SOLVE_DEFAULT](dst, polynomial, degree, interval);
}

//! @overload
static FOG_INLINE int solvePolynomial(double* dst, const double* polynomial, uint32_t degree, const IntervalD* interval)
{
  if (degree <= MATH_POLYNOMIAL_DEGREE_MAX_ANALYTIC)
    return fog_api.mathd_solvePolynomialA[degree](dst, polynomial, interval);
  else
    return fog_api.mathd_solvePolynomialN[MATH_POLYNOMIAL_SOLVE_DEFAULT](dst, polynomial, degree, interval);
}

// ============================================================================
// [Fog::Math - SolvePolynomialN]
// ============================================================================

static FOG_INLINE int solvePolynomialN(float* dst, const float* polynomial, uint32_t degree, uint32_t method)
{
  FOG_ASSERT(method < MATH_POLYNOMIAL_SOLVE_COUNT);
  return fog_api.mathf_solvePolynomialN[method](dst, polynomial, degree, NULL);
}

//! @overload
static FOG_INLINE int solvePolynomialN(double* dst, const double* polynomial, uint32_t degree, uint32_t method)
{
  FOG_ASSERT(method < MATH_POLYNOMIAL_SOLVE_COUNT);
  return fog_api.mathd_solvePolynomialN[method](dst, polynomial, degree, NULL);
}

static FOG_INLINE int solvePolynomialN(float* dst, const float* polynomial, uint32_t degree, uint32_t method, const IntervalF& interval)
{
  FOG_ASSERT(method < MATH_POLYNOMIAL_SOLVE_COUNT);
  return fog_api.mathf_solvePolynomialN[method](dst, polynomial, degree, &interval);
}

//! @overload
static FOG_INLINE int solvePolynomialN(double* dst, const double* polynomial, uint32_t degree, uint32_t method, const IntervalD& interval)
{
  FOG_ASSERT(method < MATH_POLYNOMIAL_SOLVE_COUNT);
  return fog_api.mathd_solvePolynomialN[method](dst, polynomial, degree, &interval);
}

static FOG_INLINE int solvePolynomialN(float* dst, const float* polynomial, uint32_t degree, uint32_t method, const IntervalF* interval)
{
  FOG_ASSERT(method < MATH_POLYNOMIAL_SOLVE_COUNT);
  return fog_api.mathf_solvePolynomialN[method](dst, polynomial, degree, interval);
}

//! @overload
static FOG_INLINE int solvePolynomialN(double* dst, const double* polynomial, uint32_t degree, uint32_t method, const IntervalD* interval)
{
  FOG_ASSERT(method < MATH_POLYNOMIAL_SOLVE_COUNT);
  return fog_api.mathd_solvePolynomialN[method](dst, polynomial, degree, interval);
}
//! @}

} // Math namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MATH_SOLVE_H
