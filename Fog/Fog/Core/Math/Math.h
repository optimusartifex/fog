// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MATH_MATH_H
#define _FOG_CORE_MATH_MATH_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>
#include <Fog/Core/Global/Api.h>
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/FloatBits.h>

#include <math.h>

#if defined(FOG_HAVE_FLOAT_H)
#include <float.h>
#endif // FOG_HAVE_FLOAT_H

#include <Fog/Core/Math/Cleanup.h>

namespace Fog {
namespace Math {

//! @addtogroup Fog_Core_Math
//! @{

// ============================================================================
// [Fog::Math - Min/Max/Bound]
// ============================================================================

//! @brief Returns lower number of @a a and @a b.
template<typename T>
static FOG_INLINE T min(const T& a, const T& b)
{
  return (a < b) ? a : b;
}

//! @brief Returns lower number of @a a, @a b and @a c.
//! @overload.
template<typename T>
static FOG_INLINE T min(const T& a, const T& b, const T& c)
{
  T t = a;
  if (t > b) t = b;
  if (t > c) t = c;
  return t;
}

//! @brief Returns lower number of @a a, @a b, @a c and @a d.
//! @overload.
template<typename T>
static FOG_INLINE T min(const T& a, const T& b, const T& c, const T& d)
{
  T t = a;
  if (t > b) t = b;
  if (t > c) t = c;
  if (t > d) t = d;
  return t;
}

//! @brief Returns higher number of @a a and @a b.
template<typename T>
static FOG_INLINE T max(const T& a, const T& b)
{
  return (a > b) ? a : b;
}

//! @brief Returns higher number of @a a, @a b and @a c.
//! @overload.
template<typename T>
static FOG_INLINE T max(const T& a, const T& b, const T& c)
{
  T t = a;
  if (t < b) t = b;
  if (t < c) t = c;
  return t;
}

//! @brief Returns higher number of @a a, @a b, @a c and @a d.
//! @overload.
template<typename T>
static FOG_INLINE T max(const T& a, const T& b, const T& c, const T& d)
{
  T t = a;
  if (t < b) t = b;
  if (t < c) t = c;
  if (t < d) t = d;
  return t;
}

//! @brief Returns value @a val saturated between @a min and @a max.
template<typename T>
static FOG_INLINE T bound(const T& val, const T& min, const T& max)
{
  return val < max ? (val > min ? val : min) : max;
}

template<typename T>
static FOG_INLINE bool isBounded(const T& x, const T& xmin, const T& xmax) { return x >= xmin && x <= xmax; }

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
// [Fog::Math - Finite / Infinite / NaN]
// ============================================================================

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
// Little Endian.
#define _FOG_MATH_DECLARE_CONST_F(_Var_, _V0_, _V1_, _V2_, _V3_) \
  static const uint8_t _Var_[4] = { _V3_, _V2_, _V1_, _V0_ }
#define _FOG_MATH_DECLARE_CONST_D(_Var_, _V0_, _V1_, _V2_, _V3_, _V4_, _V5_, _V6_, _V7_) \
  static const uint8_t _Var_[8] = { _V7_, _V6_, _V5_, _V4_, _V3_, _V2_, _V1_, _V0_ }
#else
// Big Endian.
#define _FOG_MATH_DECLARE_CONST_F(_Var_, _V0_, _V1_, _V2_, _V3_) \
  static const uint8_t _Var_[4] = { _V0_, _V1_, _V2_, _V3_ }
#define _FOG_MATH_DECLARE_CONST_D(_Var_, _V0_, _V1_, _V2_, _V3_, _V4_, _V5_, _V6_, _V7_) \
  static const uint8_t _Var_[8] = { _V0_, _V1_, _V2_, _V3_, _V4_, _V5_, _V6_, _V7_ }
#endif // FOG_BYTE_ORDER

#define _FOG_MATH_GET_CONST_F(_Var_) reinterpret_cast<const float*>(_Var_)[0]
#define _FOG_MATH_GET_CONST_D(_Var_) reinterpret_cast<const double*>(_Var_)[0]

#define _FOG_MATH_DECLARE_VARIANT_TEMPLATE(_Func_) \
  template<typename T> \
  FOG_INLINE T _Func_##T() { FOG_ASSERT_NOT_REACHED(); } \
  \
  template<> \
  FOG_INLINE float _Func_##T<float>() { return _Func_##F(); } \
  \
  template<> \
  FOG_INLINE double _Func_##T<double>() { return _Func_##D(); }

static FOG_INLINE float getSNanF()
{
#if defined(FOG_CC_GNU)
  return __builtin_nanf("");
#else
  _FOG_MATH_DECLARE_CONST_F(_const_signaling_nan_f, 0x8F, 0xFF, 0xFF, 0xFF);
  return _FOG_MATH_GET_CONST_F(_const_signaling_nan_f);
#endif
}

static FOG_INLINE double getSNanD()
{
#if defined(FOG_CC_GNU)
  return __builtin_nan("");
#else
  _FOG_MATH_DECLARE_CONST_D(_const_signaling_nan_d, 0x8F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
  return _FOG_MATH_GET_CONST_D(_const_signaling_nan_d);
#endif
}

static FOG_INLINE float getQNanF()
{
#if defined(FOG_CC_GNU)
  return -__builtin_nanf("");
#else
  _FOG_MATH_DECLARE_CONST_F(_const_quiet_nan_f, 0xFF, 0xFF, 0xFF, 0xFF);
  return _FOG_MATH_GET_CONST_F(_const_quiet_nan_f);
#endif
}

static FOG_INLINE double getQNanD()
{
#if defined(FOG_CC_GNU)
  return -__builtin_nan("");
#else
  _FOG_MATH_DECLARE_CONST_D(_const_quiet_nan_d, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
  return _FOG_MATH_GET_CONST_D(_const_quiet_nan_d);
#endif
}

static FOG_INLINE float getPInfF()
{
#if defined(FOG_CC_GNU)
  return __builtin_inff();
#else
  _FOG_MATH_DECLARE_CONST_F(_const_positive_inf_f, 0x7F, 0x80, 0x00, 0x00);
  return _FOG_MATH_GET_CONST_F(_const_positive_inf_f);
#endif
}

static FOG_INLINE double getPInfD()
{
#if defined(FOG_CC_GNU)
  return __builtin_inf();
#else
  _FOG_MATH_DECLARE_CONST_D(_const_positive_inf_d, 0x7F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  return _FOG_MATH_GET_CONST_D(_const_positive_inf_d);
#endif
}

static FOG_INLINE float getNInfF()
{
#if defined(FOG_CC_GNU)
  return -__builtin_inff();
#else
  _FOG_MATH_DECLARE_CONST_F(_const_negative_inf_f, 0xFF, 0x80, 0x00, 0x00);
  return _FOG_MATH_GET_CONST_F(_const_negative_inf_f);
#endif
}

static FOG_INLINE double getNInfD()
{
#if defined(FOG_CC_GNU)
  return -__builtin_inf();
#else
  _FOG_MATH_DECLARE_CONST_D(_const_negative_inf_d, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  return _FOG_MATH_GET_CONST_D(_const_negative_inf_d);
#endif
}

// TypeVariant support.
_FOG_MATH_DECLARE_VARIANT_TEMPLATE(getSNan)
_FOG_MATH_DECLARE_VARIANT_TEMPLATE(getQNan)
_FOG_MATH_DECLARE_VARIANT_TEMPLATE(getPInf)
_FOG_MATH_DECLARE_VARIANT_TEMPLATE(getNInf)

static FOG_INLINE bool isFinite(float x)
{
  // TODO: There is some mac problem, fix it.
#if defined (FOG_CC_GNU) && !defined(FOG_OS_MAC)
  return __builtin_finitef(x);
#elif defined (FOG_CC_MSC)
  return _finite(x);
#else
  return finitef(x);
#endif
}

static FOG_INLINE bool isFinite(double x)
{
#if defined (FOG_CC_MSC)
  return _finite(x);
#else
  return finite(x);
#endif
}

static FOG_INLINE bool isInfinite(float x)
{
#if defined (FOG_CC_GNU)
  return __builtin_isinff(x);
#elif defined (FOG_CC_MSC)
  return (_fpclass(x) & (_FPCLASS_NINF | _FPCLASS_PINF)) != 0;
#else
  return isinff(x);
#endif
}

static FOG_INLINE bool isInfinite(double x)
{
#if defined (FOG_CC_MSC)
  return (_fpclass(x) & (_FPCLASS_NINF | _FPCLASS_PINF)) != 0;
#else
  return isinf(x);
#endif
}

static FOG_INLINE bool isNaN(float x)
{
#if defined (FOG_CC_GNU)
  return __builtin_isnanf(x);
#elif defined (FOG_CC_MSC)
  return _isnan(x);
#else
  return isnanf(x);
#endif
}

static FOG_INLINE bool isNaN(double x)
{
#if defined (FOG_CC_MSC)
  return _isnan(x);
#else
  return isnan(x);
#endif
}

// Clean-up.
#undef _FOG_MATH_DECLARE_CONST_F
#undef _FOG_MATH_DECLARE_CONST_D
#undef _FOG_MATH_GET_CONST_F
#undef _FOG_MATH_GET_CONST_D
#undef _FOG_MATH_DECLARE_VARIANT_TEMPLATE

// ============================================================================
// [Fog::Math - Float <-> Integer]
// ============================================================================

// We define these optimized rounding methods to make conversion from floats
// as fast as possible. There is danger that if there is set incorrect rounding
// mode then we can get something we not expect (incorrect rounding).
#if defined(FOG_CC_MSC) && FOG_ARCH_BITS == 32 && (!defined(_M_IX86_FP) || _M_IX86_FP < 2)
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
// [Fog::Math - Mod]
// ============================================================================

static FOG_INLINE float mod(float x, float y) { return ::fmodf(x, y); }
static FOG_INLINE double mod(double x, double y) { return ::fmod(x, y); }

static FOG_INLINE float repeat(float x, float y)
{
  if (x >= y || x <= -y) x = mod(x, y);
  if (x < 0.0f) x += y;
  return x;
}

static FOG_INLINE double repeat(double x, double y)
{
  if (x >= y || x <= -y) x = mod(x, y);
  if (x < 0.0) x += y;
  return x;
}

// ============================================================================
// [Fog::Math - Recip]
// ============================================================================

static FOG_INLINE float recip(float x) { return 1.0f / x; }
static FOG_INLINE double recip(double x) { return 1.0 / x; }

// ============================================================================
// [Fog::Math - Fraction]
// ============================================================================

static FOG_INLINE float fraction(float v) { return mod(v, 1.0f); }
static FOG_INLINE double fraction(double v) { return mod(v, 1.0); }

static FOG_INLINE float positiveFraction(float v) { v = mod(v, 1.0f); if (v < 0.0f) v += 1.0f; return v; }
static FOG_INLINE double positiveFraction(double v) { v = mod(v, 1.0); if (v < 0.0) v += 1.0; return v; }

// ============================================================================
// [Fog::Math - Degrees <-> Radians]
// ============================================================================

static FOG_INLINE float deg2rad(float deg) { return deg * float(MATH_RAD_FROM_DEG); }
static FOG_INLINE float rad2deg(float rad) { return rad * float(MATH_DEG_FROM_RAD); }

static FOG_INLINE double deg2rad(double deg) { return deg * double(MATH_RAD_FROM_DEG); }
static FOG_INLINE double rad2deg(double rad) { return rad * double(MATH_DEG_FROM_RAD); }

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

#elif defined(FOG_ARCH_X86) && defined(FOG_CC_MSC)

// Use inline FPU assembly, faster than calling sin() and cos() separately.
// Thanks to Jacques for suggestions about this!
static FOG_INLINE void sincos(float rad, float* sinResult, float* cosResult)
{
  _asm
  {
    fld rad              ; // Load rad into FPU.
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
    fld rad              ; // Load rad into FPU.
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

static FOG_INLINE float atan(float x) { return ::atanf(x); }
static FOG_INLINE double atan(double x) { return ::atan(x); }

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
// "Math::hypot(x, y) ~~ Math::sqrt(x * x + y * y)"
// ============================================================================

#if defined(FOG_CC_MSC)
static FOG_INLINE float hypot(float x, float y) { return ::_hypotf(x, y); }
static FOG_INLINE double hypot(double x, double y) { return ::_hypot(x, y); }
#else
static FOG_INLINE float hypot(float x, float y) { return ::hypotf(x, y); }
static FOG_INLINE double hypot(double x, double y) { return ::hypot(x, y); }
#endif

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
// [Fog::Math - Log]
// ============================================================================

static FOG_INLINE float log(float x) { return ::logf(x); }
static FOG_INLINE double log(double x) { return ::log(x); }

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
#endif // _FOG_CORE_MATH_MATH_H
