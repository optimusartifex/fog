// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MATH_H
#define _FOG_CORE_MATH_H

// [Dependencies]
#include <Fog/Core/Build.h>

#include <math.h>

#if defined(FOG_HAVE_FLOAT_H)
#include <float.h>
#endif // FOG_HAVE_FLOAT_H

// ============================================================================
// [Fog::Math - NaN and Inf]
// ============================================================================

// Not defined NAN ?
#if !defined(NAN)
# if defined(FOG_CC_GNU)
#  define NAN __builtin_nan("")
# elif defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
// For MSVC! Is there any other compiler that will not declare NAN ?
static FOG_HIDDEN const uint32_t fog_nan[2] = { 0xFFFFFFFF, 0x7FFFFFFF };
#  define NAN ( *(const double *)fog_nan)
# else
#  error "Fog::Math - Haven't NAN"
# endif
#endif

// Not defined INFINITY ?
#if !defined(INFINITY)
# if defined(FOG_CC_GNU)
#  define INFINITY __builtin_inf()
# elif defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
// For MSVC! Is there any other compiler that will not declare INFINITY ?
static FOG_HIDDEN const uint32_t fog_inf[2] = { 0x00000000, 0x7FF00000 };
#  define INFINITY ( *(const double *)fog_inf)
# else
#  error "Fog::Math - Haven't INFINITY"
# endif
#endif

// ============================================================================
// [Fog::Math - Control87]
// ============================================================================

#if defined(FOG_OS_WINDOWS) && defined(FOG_CC_MSVC)
# define FOG_CONTROL87_BEGIN() uint _control87_old = _control87(0x9001F, FOG_ARCH_32_64(_MCW_DN|_MCW_EM|_MCW_RC, 0xFFFF))
# define FOG_CONTROL87_END() _control87(_control87_old, FOG_ARCH_32_64(_MCW_DN|_MCW_EM|_MCW_RC, 0xFFFF))
#elif defined(FOG_OS_WINDOWS)
# define FOG_CONTROL87_BEGIN() _control87(0, 0)
# define FOG_CONTROL87_END() _clear87()
#elif defined(FOG_HAVE_FENV_H)
# define FOG_CONTROL87_BEGIN() fenv_t _control87_env; feholdexcept(&_control87_env)
# define FOG_CONTROL87_END() fesetenv(&_control87_env)
#else
# define FOG_CONTROL87_BEGIN do {} while(0)
# define FOG_CONTROL87_END do {} while(0)
#endif

namespace Fog {

// ============================================================================
// [Fog::Math - Constants]
// ============================================================================

//! @addtogroup Fog_Core_Math
//! @{

//! @brief e.
static const double MATH_E = 2.7182818284590452354;

//! @brief log_2(e).
static const double MATH_LOG2E = 1.4426950408889634074;

//! @brief log_10(e).
static const double MATH_LOG10E = 0.43429448190325182765;

//! @brief log_e(2).
static const double MATH_LN2 = 0.69314718055994530942;

//! @brief log_e(10).
static const double MATH_LN10 = 2.30258509299404568402;

//! @brief Pi.
static const double MATH_PI = 3.14159265358979323846;

//! @brief pi / 2.
static const double MATH_PI_DIV_2 = 1.57079632679489661923;

//! @brief pi / 4.
static const double MATH_PI_DIV_4 = 0.78539816339744830962;

//! @brief 1 / pi.
static const double MATH_1_DIV_PI = 0.31830988618379067154;

//! @brief 2 / pi.
static const double MATH_2_DIV_PI = 0.63661977236758134308;

//! @brief 2 / sqrt(pi).
static const double MATH_2_DIV_SQRTPI = 1.12837916709551257390;

//! @brief sqrt(2).
static const double MATH_SQRT2 = 1.41421356237309504880;

//! @brief 1/sqrt(2).
static const double MATH_1_DIV_SQRT2 = 0.70710678118654752440;

//! @brief Default epsilon used in math for 32-bit floats.
static const float MATH_EPSILON_F = 1e-8f;

//! @brief Default epsilon used in math for 64-bit floats.
static const double MATH_EPSILON_D = 1e-14;

//! @}

// ============================================================================
// [Fog::Math - Namespace]
// ============================================================================

namespace Math {

//! @addtogroup Fog_Core_Math
//! @{

// ============================================================================
// [Fog::Math - Min/Max/Bound]
// ============================================================================

#if defined(min)
#undef min
#endif // min

#if defined(max)
#undef max
#endif // max

#if defined(bound)
#undef bound
#endif // bound

//! @brief Returns lower number of @a a and @a b.
template<typename T>
static FOG_INLINE const T& min(const T& a, const T& b)
{ return (a < b) ? a : b; }

//! @brief Returns higher number of @a a and @a b.
template<typename T>
static FOG_INLINE const T& max(const T& a, const T& b)
{ return (a > b) ? a : b; }

//! @brief Returns lower number of @a a, @a b and @a c.
//! @overload.
template<typename T>
static FOG_INLINE const T& min(const T& a, const T& b, const T& c)
{ return (a < b) ? ((a > c) ? c : a) : ((b > c) ? c : b); }

//! @brief Returns higher number of @a a, @a b and @a c.
//! @overload.
template<typename T>
static FOG_INLINE const T& max(const T& a, const T& b, const T& c)
{ return (a > b) ? ((a < c) ? c : a) : ((b < c) ? c : b); }

//! @brief Returns value @a val saturated between @a min and @a max.
template<typename T>
static FOG_INLINE const T& bound(const T& val, const T& min, const T& max)
{ return val < max ? (val > min ? val : min) : max; }

static FOG_INLINE uint8_t boundToByte(int val)
{
  if (FOG_LIKELY((uint)val <= 0xFF))
    return (uint8_t)(uint)val;
  else
    return (val < 0) ? 0 : 0xFF;
}

// ============================================================================
// [Fog::Math - Abs]
// ============================================================================

#if defined(abs)
#undef abs
#endif // abs

//! @brief Returns absolute value of @a a
template<typename T>
FOG_INLINE T abs(const T& a)
{ return (a >= 0) ? a : -a; }

template<>
FOG_INLINE float abs(const float& a)
{
  return ::fabsf(a);
}

template<>
FOG_INLINE double abs(const double& a)
{
  return ::fabs(a);
}

// ============================================================================
// [Fog::Math - Nan/Finite/Infinite]
// ============================================================================

static FOG_INLINE bool isNaN(float x)
{
#if defined (FOG_CC_MSVC)
  return _isnan(x);
#else
  return isnan(x);
#endif
}

static FOG_INLINE bool isNaN(double x)
{
#if defined (FOG_CC_MSVC)
  return _isnan(x);
#else
  return isnan(x);
#endif
}

// ============================================================================
// [Fog::Math - Fuzzy Comparison Using Epsilon]
// ============================================================================

static FOG_INLINE bool fzero(float a, float epsilon = MATH_EPSILON_F) { return abs(a) <= epsilon; }
static FOG_INLINE bool fzero(double a, double epsilon = MATH_EPSILON_D) { return abs(a) <= epsilon; }

static FOG_INLINE bool feq(float a, float b, float epsilon = MATH_EPSILON_F) { return abs(a - b) <= epsilon; }
static FOG_INLINE bool feq(double a, double b, double epsilon = MATH_EPSILON_D) { return abs(a - b) <= epsilon; }

static FOG_INLINE bool flt(float a, float b, float epsilon = MATH_EPSILON_F) { return a < b + epsilon; }
static FOG_INLINE bool flt(double a, double b, double epsilon = MATH_EPSILON_D) { return a < b + epsilon; }

static FOG_INLINE bool fgt(float a, float b, float epsilon = MATH_EPSILON_F) { return a > b - epsilon; }
static FOG_INLINE bool fgt(double a, double b, double epsilon = MATH_EPSILON_D) { return a > b - epsilon; }

// ============================================================================
// [Fog::Math - Float <-> Integer]
// ============================================================================

// We define these optimized rounding methods to make conversion from floats
// as fast as possible. There is danger that if there is set incorrect rounding
// mode then we can get something we not expect (incorrect rounding).
#if defined(FOG_CC_MSVC) && FOG_ARCH_BITS == 32 && (!defined(_M_IX86_FP) || _M_IX86_FP < 2)
FOG_INLINE int iround(float v)
{
  int t;
  __asm {
    fld dword ptr [v]
    fistp dword ptr [t]
    //mov eax, dword ptr [t]
  }
  return t;
}

FOG_INLINE uint uround(float v)
{
  uint t;
  __asm {
    fld dword ptr [v]
    fistp dword ptr [t]
    //mov eax, dword ptr [t]
  }
  return t;
}

FOG_INLINE int iround(double v)
{
  int t;
  __asm {
    fld qword ptr [v]
    fistp dword ptr [t]
    //mov eax, dword ptr [t]
  }
  return t;
}

FOG_INLINE uint uround(double v)
{
  unsigned t;
  __asm {
    fld qword ptr [v]
    fistp dword ptr [t]
    //mov eax, dword ptr [t]
  }
  return t;
}
#else
static FOG_INLINE int iround(float v) { return (int)((v < 0.0f) ? v - 0.5f : v + 0.5f); }
static FOG_INLINE uint uround(float v) { return (uint)(int)(v + 0.5f); }

static FOG_INLINE int iround(double v) { return (int)((v < 0.0) ? v - 0.5 : v + 0.5); }
static FOG_INLINE uint uround(double v) { return (uint)(int)(v + 0.5); }
#endif

static FOG_INLINE int ifloor(double v) { return iround(::floor(v)); }
static FOG_INLINE int iceil(double v) { return iround(::ceil(v)); }

static FOG_INLINE uint ufloor(double v) { return uround(::floor(v)); }
static FOG_INLINE uint uceil(double v) { return uround(::ceil(v)); }

// ============================================================================
// [Fog::Math - Double <-> Fixed point]
// ============================================================================

//! @internal
//!
//! @brief Helper union that contains 64-bit float and 64-bit integer.
union DoubleAndInt64
{
  //! @brief Double data.
  double d;

  //! @brief Int32[2] data.
  int32_t i32[2];
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  //! @brief Int32 individual data.
  int32_t i32_0, i32_1;
#else
  //! @brief Int32 individual data.
  int32_t i32_1, i32_0;
#endif

  //! @brief Int64 data.
  int64_t i64;
};

//! @internal
//!
//! @brief Helper union that contains 32-bit float and 32-bit integer.
union FloatAndInt32
{
  //! @brief Float data.
  float f;
  //! @brief Int32 data.
  int32_t i32;
};

static FOG_INLINE int doubleToInt(double d)
{
  return (int)d;
}

static FOG_INLINE int16x16_t doubleToFixed16x16(double d)
{
  // The euqivalent code should be:
  // return (int32_t)(d * 65536.0);
  DoubleAndInt64 data;
  data.d = d + 103079215104.0;
  return data.i32_0;
}

static FOG_INLINE int24x8_t doubleToFixed24x8(double d)
{
  return (int24x8_t)(d * 256.0);
}

static FOG_INLINE int48x16_t doubleToFixed48x16(double d)
{
  return (int48x16_t)(d * 65536.0);
}

// ============================================================================
// [Fog::Math - Recip]
// ============================================================================

static FOG_INLINE float recip(float x) { return 1.0f / x; }
static FOG_INLINE double recip(double x) { return 1.0 / x; }

// ============================================================================
// [Fog::Math - Degrees <-> Radians]
// ============================================================================

static FOG_INLINE float deg2rad(float deg) { return deg * ((float)MATH_PI / 180.0f); }
static FOG_INLINE float rad2deg(float rad) { return rad * (180.0f / (float)MATH_PI); }

static FOG_INLINE double deg2rad(double deg) { return deg * (MATH_PI / 180.0); }
static FOG_INLINE double rad2deg(double rad) { return rad * (180.0 / MATH_PI); }

// ============================================================================
// [Fog::Math - Trigonometric Functions]
// ============================================================================

static FOG_INLINE float sin(float rad) { return ::sinf(rad); }
static FOG_INLINE float cos(float rad) { return ::cosf(rad); }
static FOG_INLINE float tan(float rad) { return ::tanf(rad); }

static FOG_INLINE double sin(double rad) { return ::sin(rad); }
static FOG_INLINE double cos(double rad) { return ::cos(rad); }
static FOG_INLINE double tan(double rad) { return ::tan(rad); }

#if defined(FOG_CC_GNU) && !defined(FOG_OS_MAC) && !defined(FOG_OS_WINDOWS)

// Use GCC sincos() implementation.
static FOG_INLINE void sincos(float rad, float* sinResult, float* cosResult)
{
  ::sincosf(rad, sinResult, cosResult);
}

static FOG_INLINE void sincos(double rad, double* sinResult, double* cosResult)
{
  ::sincos(rad, sinResult, cosResult);
}

#elif defined(FOG_CC_MSVC) && defined(FOG_ARCH_X86)

// Use inline FPU assembly, faster than calling sin() and cos() separately.
// Thanks to Jacques for suggestions about this!
static FOG_INLINE void sincos(float rad, float* sinResult, float* cosResult)
{
  _asm
  {
    fld rad              ; // Load rad into fpu.
    fsincos              ; // Compute both sin and cos.

                         ; // While FPU stalls, load adresses.
    mov edx, cosResult   ; // Load cosResult address.
    mov eax, sinResult   ; // Load sinResult address.

    fstp dword ptr [edx] ; // Pop and store cosResult.
    fstp dword ptr [eax] ; // Pop and store sinResult.
  }
}

static FOG_INLINE void sincos(double rad, double* sinResult, double* cosResult)
{
  _asm
  {
    fld rad              ; // Load rad into fpu.
    fsincos              ; // Compute both sin and cos.

                         ; // While FPU stalls, load adresses.
    mov edx, cosResult   ; // Load cosResult address.
    mov eax, sinResult   ; // Load sinResult address.

    fstp qword ptr [edx] ; // Pop and store cosResult.
    fstp qword ptr [eax] ; // Pop and store sinResult.
  }
}

#else

// No specific version available? Use sin() and cos()...
static FOG_INLINE void sincos(float rad, float* sinResult, float* cosResult)
{
  *sinResult = ::sinf(rad);
  *cosResult = ::cosf(rad);
}

static FOG_INLINE void sincos(double rad, double* sinResult, double* cosResult)
{
  *sinResult = ::sin(rad);
  *cosResult = ::cos(rad);
}
#endif

static FOG_INLINE float asin(float x) { return ::asinf(x); }
static FOG_INLINE double asin(double x) { return ::asin(x); }

static FOG_INLINE float acos(float x) { return ::acosf(x); }
static FOG_INLINE double acos(double x) { return ::acos(x); }

static FOG_INLINE float atan2(float x, float y) { return ::atan2f(x, y); }
static FOG_INLINE double atan2(double x, double y) { return ::atan2(x, y); }

// ============================================================================
// [Fog::Math - Sqrt]
// ============================================================================

static FOG_INLINE float sqrt(float x) { return ::sqrtf(x); }
static FOG_INLINE double sqrt(double x) { return ::sqrt(x); }

// ============================================================================
// [Fog::Math - Hypot]
//
// "Math::hypot(x, y) == Math::sqrt(x * x + y * y)"
// ============================================================================

static FOG_INLINE float hypot(float x, float y) { return ::hypotf(x, y); }
static FOG_INLINE double hypot(double x, double y) { return ::hypot(x, y); }

// ============================================================================
// [Fog::Math - Pow]
// ============================================================================

static FOG_INLINE float pow(float x, float y) { return ::powf(x, y); }
static FOG_INLINE double pow(double x, double y) { return ::pow(x, y); }

static FOG_INLINE float pow2(float x) { return x * x; }
static FOG_INLINE double pow2(double x) { return x * x; }

static FOG_INLINE float pow3(float x) { return x * x * x; }
static FOG_INLINE double pow3(double x) { return x * x * x; }

// ============================================================================
// [Fog::Math - Dist]
// ============================================================================

static FOG_INLINE float dist(
  float x0, float y0,
  float x1, float y1)
{
  float dx = x1 - x0;
  float dy = y1 - y0;
  return Math::sqrt(dx * dx + dy * dy);
}

static FOG_INLINE double dist(
  double x0, double y0,
  double x1, double y1)
{
  double dx = x1 - x0;
  double dy = y1 - y0;
  return Math::sqrt(dx * dx + dy * dy);
}

//! @}

} // Math namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MATH_H
