// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MATH_FUZZY_H
#define _FOG_CORE_MATH_FUZZY_H

// [Dependencies]
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Math.h>

namespace Fog {
namespace Math {

//! @addtogroup Fog_Core_Math
//! @{

// ============================================================================
// [Fog::Math - IsFuzzy...]
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

static FOG_INLINE bool isFuzzyGe(float a, float b, float epsilon = MATH_EPSILON_F) { return a > b - epsilon; }
static FOG_INLINE bool isFuzzyGe(double a, double b, double epsilon = MATH_EPSILON_D) { return a > b - epsilon; }

static FOG_INLINE bool isFuzzyLe(float a, float b, float epsilon = MATH_EPSILON_F) { return a < b + epsilon; }
static FOG_INLINE bool isFuzzyLe(double a, double b, double epsilon = MATH_EPSILON_D) { return a < b + epsilon; }

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
    if (!isFuzzyEq(a[i], b[i]), epsilon)
      return false;
  }

  return true;
}

static FOG_INLINE bool feqv(const double* a, const double* b, size_t length, double epsilon = MATH_EPSILON_F)
{
  for (size_t i = 0; i < length; i++)
  {
    if (!isFuzzyEq(a[i], b[i]), epsilon)
      return false;
  }

  return true;
}

//! @}

} // Math namespace
} // Fog namespace

namespace Fog {

//! @addtogroup Fog_Core_Math
//! @{

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

template<typename TypeT>
struct Fuzzy {};

// ============================================================================
// [Fog::Fuzzy<float>]
// ============================================================================

template<>
struct Fuzzy<float>
{
  static FOG_INLINE bool eq(const float &a, const float &b, float epsilon = MATH_EPSILON_F)
  {
    return Math::isFuzzyEq(a, b, epsilon);
  }
};

// ============================================================================
// [Fog::Fuzzy<double>]
// ============================================================================

template<>
struct Fuzzy<double>
{
  static FOG_INLINE bool eq(const double &a, const double &b, double epsilon = MATH_EPSILON_D)
  {
    return Math::isFuzzyEq(a, b, epsilon);
  }
};

// ============================================================================
// [Fog::Fuzzy<> - Macros]
// ============================================================================

#define FOG_FUZZY_DECLARE_F(_Symbol_, _Code_) \
namespace Fog { \
  template <> \
  struct Fuzzy < ::_Symbol_ > \
  { \
    static FOG_INLINE bool eq(const ::_Symbol_& a, const ::_Symbol_& b, float epsilon = MATH_EPSILON_F) \
    { \
      return (_Code_); \
    } \
  }; \
}

#define FOG_FUZZY_DECLARE_D(_Symbol_, _Code_) \
namespace Fog { \
  template <> \
  struct Fuzzy < ::_Symbol_ > \
  { \
    static FOG_INLINE bool eq(const ::_Symbol_& a, const ::_Symbol_& b, double epsilon = MATH_EPSILON_D) \
    { \
      return (_Code_); \
    } \
  }; \
}

#define FOG_FUZZY_DECLARE_F_VEC(_Symbol_, _Length_) \
namespace Fog { \
  template <> \
  struct Fuzzy < ::_Symbol_ > \
  { \
    static FOG_INLINE bool eq(const ::_Symbol_& a, const ::_Symbol_& b, float epsilon = MATH_EPSILON_F) \
    { \
      return Math::feqv( \
        reinterpret_cast<const float*>(&a), \
        reinterpret_cast<const float*>(&b), \
        _Length_, epsilon); \
    } \
  }; \
}

#define FOG_FUZZY_DECLARE_D_VEC(_Symbol_, _Length_) \
namespace Fog { \
  template <> \
  struct Fuzzy < ::_Symbol_ > \
  { \
    static FOG_INLINE bool eq(const ::_Symbol_& a, const ::_Symbol_& b, float epsilon = MATH_EPSILON_F) \
    { \
      return Math::feqv( \
        reinterpret_cast<const double*>(&a), \
        reinterpret_cast<const double*>(&b), \
        _Length_, epsilon); \
    } \
  }; \
}

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MATH_FUZZY_H
