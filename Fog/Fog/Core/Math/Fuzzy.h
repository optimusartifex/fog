// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MATH_FUZZY_H
#define _FOG_CORE_MATH_FUZZY_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Convert.h>

namespace Fog {

//! @addtogroup Fog_Core_Math
//! @{

// ============================================================================
// [Fog::Fuzzy<> - Template]
// ============================================================================

template<typename T>
struct Fuzzy {};

// ============================================================================
// [Fog::Fuzzy<> - Macros]
// ============================================================================

#define FOG_FUZZY_DECLARE(_Symbol_, _Code_) \
namespace Fog { \
  template <> \
  struct Fuzzy < _Symbol_ > \
  { \
    static FOG_INLINE bool eq(const _Symbol_& a, const _Symbol_& b) \
    { \
      return (_Code_); \
    } \
  }; \
}

//! @}

} // Fog namespace

namespace Fog {
namespace Math {

// ============================================================================
// [Fog::Math - Fuzzy Comparison Using Epsilon]
// ============================================================================

static FOG_INLINE bool isFuzzyZero(float a, float epsilon = MATH_EPSILON_F) { return abs(a) <= epsilon; }
static FOG_INLINE bool isFuzzyZero(double a, double epsilon = MATH_EPSILON_D) { return abs(a) <= epsilon; }

static FOG_INLINE bool isFuzzyOne(float a, float epsilon = MATH_EPSILON_F) { return abs(a - 1.0f) <= epsilon; }
static FOG_INLINE bool isFuzzyOne(double a, double epsilon = MATH_EPSILON_D) { return abs(a - 1.0) <= epsilon; }

static FOG_INLINE bool isFuzzyPositiveZero(float a, float epsilon = MATH_EPSILON_F) { return a <= epsilon; }
static FOG_INLINE bool isFuzzyPositiveZero(double a, double epsilon = MATH_EPSILON_D) { return a <= epsilon; }

static FOG_INLINE bool isFuzzyPositiveNumber(float a, float epsilon = MATH_EPSILON_F) { return a > epsilon; }
static FOG_INLINE bool isFuzzyPositiveNumber(double a, double epsilon = MATH_EPSILON_D) { return a > epsilon; }

static FOG_INLINE bool isFuzzyNegativeNumber(float a, float epsilon = MATH_EPSILON_F) { return a < -epsilon; }
static FOG_INLINE bool isFuzzyNegativeNumber(double a, double epsilon = MATH_EPSILON_D) { return a < -epsilon; }

static FOG_INLINE bool isFuzzyEq(float a, float b, float epsilon = MATH_EPSILON_F) { return abs(a - b) <= epsilon; }
static FOG_INLINE bool isFuzzyEq(double a, double b, double epsilon = MATH_EPSILON_D) { return abs(a - b) <= epsilon; }

static FOG_INLINE bool isFuzzyLowerEq(float a, float b, float epsilon = MATH_EPSILON_F) { return a < b + epsilon; }
static FOG_INLINE bool isFuzzyLowerEq(double a, double b, double epsilon = MATH_EPSILON_D) { return a < b + epsilon; }

static FOG_INLINE bool isFuzzyGreaterEq(float a, float b, float epsilon = MATH_EPSILON_F) { return a > b - epsilon; }
static FOG_INLINE bool isFuzzyGreaterEq(double a, double b, double epsilon = MATH_EPSILON_D) { return a > b - epsilon; }

static FOG_INLINE bool isFuzzyToInt(float a, int& result, float epsilon = MATH_EPSILON_F)
{
  result = iround(a);
  return isFuzzyEq(a, (float)result, epsilon);
}

static FOG_INLINE bool isFuzzyToInt(double a, int& result, double epsilon = MATH_EPSILON_F)
{
  result = iround(a);
  return isFuzzyEq(a, (double)result, epsilon);
}

static FOG_INLINE bool feqv(const float* a, const float* b, size_t length, float epsilon = MATH_EPSILON_F)
{
  for (size_t i = 0; i < length; i++)
  {
    if (!isFuzzyEq(a[i], b[i]), epsilon) return false;
  }

  return true;
}

static FOG_INLINE bool feqv(const double* a, const double* b, size_t length, double epsilon = MATH_EPSILON_F)
{
  for (size_t i = 0; i < length; i++)
  {
    if (!isFuzzyEq(a[i], b[i]), epsilon) return false;
  }

  return true;
}

} // Math namespace
} // Fog namespace

// ============================================================================
// [Fog::Fuzzy<> - Specialization]
// ============================================================================

FOG_FUZZY_DECLARE(float, Math::isFuzzyEq(a, b))
FOG_FUZZY_DECLARE(double, Math::isFuzzyEq(a, b))

// [Guard]
#endif // _FOG_CORE_MATH_FUZZY_H
