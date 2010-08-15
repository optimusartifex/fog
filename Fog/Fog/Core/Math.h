// [Fog-Core Library - Public API]
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
// [Fog::Math - Constants]
// ============================================================================

// Some constants are not defined by MSVC.

// Constants with 'l' suffix are gcc extensions for long double types. Long
// doubles are not normally used by Fog, but they are here for completness.

// e
#if !defined(M_E)
# define M_E            2.7182818284590452354
# define M_El           2.7182818284590452353602874713526625L
#endif

// log_2 e
#if !defined(M_LOG2E)
# define M_LOG2E        1.4426950408889634074
# define M_LOG2El       1.4426950408889634073599246810018921L
#endif

// log_10 e
#if !defined(M_LOG10E)
# define M_LOG10E       0.43429448190325182765
# define M_LOG10El      0.4342944819032518276511289189166051L
#endif

// log_e 2
#if !defined(M_LN2)
# define M_LN2          0.69314718055994530942
# define M_LN2l         0.6931471805599453094172321214581766L
#endif

// log_e 10
#if !defined(M_LN10)
# define M_LN10         2.30258509299404568402
# define M_LN10l        2.3025850929940456840179914546843642L
#endif

// pi
#if !defined(M_PI)
# define M_PI           3.14159265358979323846
# define M_PIl          3.1415926535897932384626433832795029L
#endif

// pi/2
#if !defined(M_PI_2)
# define M_PI_2         1.57079632679489661923
# define M_PI_2l        1.5707963267948966192313216916397514L
#endif

// pi/4
#if !defined(M_PI_4)
# define M_PI_4         0.78539816339744830962
# define M_PI_4l        0.7853981633974483096156608458198757L
#endif

// 1/pi
#if !defined(M_1_PI)
# define M_1_PI         0.31830988618379067154
# define M_1_PIl        0.3183098861837906715377675267450287L
#endif

// 2/pi
#if !defined(M_2_PI)
# define M_2_PI         0.63661977236758134308
# define M_2_PIl        0.6366197723675813430755350534900574L
#endif

// 2/sqrt(pi)
#if !defined(M_2_SQRTPI)
# define M_2_SQRTPI     1.12837916709551257390
# define M_2_SQRTPIl    1.1283791670955125738961589031215452L
#endif

// sqrt(2)
#if !defined(M_SQRT2)
# define M_SQRT2        1.41421356237309504880
# define M_SQRT2l       1.4142135623730950488016887242096981L
#endif

// 1/sqrt(2)
#if !defined(M_SQRT1_2)
# define M_SQRT1_2      0.70710678118654752440
# define M_SQRT1_2l     0.7071067811865475244008443621048490L
#endif

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
// [Fog::Math - Floating point ops with epsilon]
// ============================================================================

//! @brief Default epsilon used in math for 32-bit floats.
static const float DEFAULT_FLOAT_EPSILON = 1e-8f;

//! @brief Default epsilon used in math for 64-bit floats.
static const double DEFAULT_DOUBLE_EPSILON = 1e-14;

template<typename T>
static FOG_INLINE bool feq(T a, T b, T epsilon = (T)0.000001)
{
  return fabs(a - b) <= epsilon;
}

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
// [Fog::Math - Degrees <-> Radians]
// ============================================================================

static FOG_INLINE float deg2rad(float deg) { return deg * ((float)M_PI / 180.0f); }
static FOG_INLINE float rad2deg(float rad) { return rad * (180.0f / (float)M_PI); }

static FOG_INLINE double deg2rad(double deg) { return deg * (M_PI / 180.0); }
static FOG_INLINE double rad2deg(double rad) { return rad * (180.0 / M_PI); }

// ============================================================================
// [Fog::Math - Trigonometric Functions]
// ============================================================================

static FOG_INLINE float sin(float rad) { return ::sinf(rad); }
static FOG_INLINE float cos(float rad) { return ::cosf(rad); }

static FOG_INLINE double sin(double rad) { return ::sin(rad); }
static FOG_INLINE double cos(double rad) { return ::cos(rad); }

#if defined(FOG_CC_GNU) && !defined(FOG_OS_MAC) && !defined(FOG_OS_WINDOWS)
static FOG_INLINE void sincos(float rad, float* sinResult, float* cosResult)
{
  ::sincosf(rad, sinResult, cosResult);
}

static FOG_INLINE void sincos(double rad, double* sinResult, double* cosResult)
{
  ::sincos(rad, sinResult, cosResult);
}
#else
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

// ============================================================================
// [Fog::Math - Sqrt]
// ============================================================================

static FOG_INLINE float sqrt(float x) { return ::sqrtf(x); }
static FOG_INLINE double sqrt(double x) { return ::sqrt(x); }

//! @}

} // Math namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MATH_H
