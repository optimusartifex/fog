// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MATH_MATH_H
#define _FOG_CORE_MATH_MATH_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Constants.h>

#if defined(FOG_HARDCODE_SSE)
# include <Fog/Core/C++/IntrinSSE.h>
#endif // FOG_HARDCODE_SSE

#if defined(FOG_HARDCODE_SSE2)
# include <Fog/Core/C++/IntrinSSE2.h>
#endif // FOG_HARDCODE_SSE2

// [Dependencies - C]
#include <math.h>

#if defined(FOG_HAVE_FLOAT_H)
# include <float.h>
#endif // FOG_HAVE_FLOAT_H

// [Cleanup]
#include <Fog/Core/Math/Cleanup.h>

// I experienced that some platforms has no support for single precision
// floating point functions like isFinite(), isInfinite(), isNaN(), etc...
//
// If compiler supports these functions then these macros are defined to
// the function names. In case that compiler doesn't support the functions,
// macro is not defined, thus the special code is generated.
//
// Macros Listing:
//
//   #define _FOG_MATH_IS_FINITE_F
//   #define _FOG_MATH_IS_FINITE_D
//
//   #define _FOG_MATH_IS_INFINITE_F
//   #define _FOG_MATH_IS_INFINITE_D
//
//   #define _FOG_MATH_IS_NAN_F
//   #define _FOG_MATH_IS_NAN_D

#if defined(FOG_CC_MSC)
# define _FOG_MATH_IS_FINITE_D(_Value_) _finite(_Value_)
# define _FOG_MATH_IS_NAN_D(_Value_) _isnan(_Value_)
#endif // FOG_CC_MSC

#if defined(FOG_CC_GNU) && !defined(FOG_OS_MAC)
# define _FOG_MATH_GET_SNAN_F() __builtin_nansf("")
# define _FOG_MATH_GET_SNAN_D() __builtin_nans("")
# define _FOG_MATH_GET_QNAN_F() __builtin_nanf("")
# define _FOG_MATH_GET_QNAN_D() __builtin_nan("")
# define _FOG_MATH_GET_PINF_F() __builtin_inff()
# define _FOG_MATH_GET_PINF_D() __builtin_inf()
# define _FOG_MATH_GET_NINF_F() (-__builtin_inff())
# define _FOG_MATH_GET_NINF_D() (-__builtin_inf())

# define _FOG_MATH_IS_FINITE_F(_Value_) __builtin_finitef(_Value_)
# define _FOG_MATH_IS_FINITE_D(_Value_) __builtin_finite(_Value_)
# define _FOG_MATH_IS_INFINITE_F(_Value_) __builtin_isinff(_Value_)
# define _FOG_MATH_IS_INFINITE_D(_Value_) __builtin_isinf(_Value_)
# define _FOG_MATH_IS_NAN_F(_Value_) __builtin_isnanf(_Value_)
# define _FOG_MATH_IS_NAN_D(_Value_) __builtin_isnan(_Value_)
#endif // FOG_CC_GNU && !FOG_OS_MAC

// Floating point storage layout:
// 
//                  | Sign |  Expponent | Fraction | Bias
// -----------------+------+------------+----------+-----
// Single Precision | 1[31]|  8 [30-23] |23 [22-00]|  127
// Double Precision | 1[63]| 11 [62-52] |52 [51-00]| 1023

namespace Fog {
namespace Math {

//! @addtogroup Fog_Core_Math
//! @{

// ============================================================================
// [Fog::Math - Finite / Infinite / NaN]
// ============================================================================

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
// Little Endian.
#define _FOG_MATH_DECLARE_CONST_F(_Var_, _V0_, _V1_, _V2_, _V3_) \
  FOG_ALIGNED_VAR(static const uint8_t, _Var_[4], 4) = { _V3_, _V2_, _V1_, _V0_ }
#define _FOG_MATH_DECLARE_CONST_D(_Var_, _V0_, _V1_, _V2_, _V3_, _V4_, _V5_, _V6_, _V7_) \
  FOG_ALIGNED_VAR(static const uint8_t, _Var_[8], 8) = { _V7_, _V6_, _V5_, _V4_, _V3_, _V2_, _V1_, _V0_ }
#else
// Big Endian.
#define _FOG_MATH_DECLARE_CONST_F(_Var_, _V0_, _V1_, _V2_, _V3_) \
  FOG_ALIGNED_VAR(static const uint8_t, _Var_[4], 4) = { _V0_, _V1_, _V2_, _V3_ }
#define _FOG_MATH_DECLARE_CONST_D(_Var_, _V0_, _V1_, _V2_, _V3_, _V4_, _V5_, _V6_, _V7_) \
  FOG_ALIGNED_VAR(static const uint8_t, _Var_[8], 8) = { _V0_, _V1_, _V2_, _V3_, _V4_, _V5_, _V6_, _V7_ }
#endif // FOG_BYTE_ORDER

#define _FOG_MATH_GET_CONST_F(_Var_) reinterpret_cast<const float*>(_Var_)[0]
#define _FOG_MATH_GET_CONST_D(_Var_) reinterpret_cast<const double*>(_Var_)[0]

#define _FOG_MATH_DECLARE_VARIANT_TEMPLATE(_Func_) \
  template<typename T> \
  FOG_STATIC_INLINE_T T _Func_##T() { FOG_ASSERT_NOT_REACHED(); } \
  \
  template<> \
  FOG_STATIC_INLINE_T float _Func_##T<float>() { return _Func_##F(); } \
  \
  template<> \
  FOG_STATIC_INLINE_T double _Func_##T<double>() { return _Func_##D(); }

static FOG_INLINE float getSNanF()
{
#if defined(_FOG_MATH_GET_SNAN_F)
  return _FOG_MATH_GET_SNAN_F();
#else
  _FOG_MATH_DECLARE_CONST_F(_const_snan_f, 0x7F, 0x80, 0x00, 0x00);
  return _FOG_MATH_GET_CONST_F(_const_snan_f);
#endif // _FOG_MATH_GET_SNAN_F
}

static FOG_INLINE double getSNanD()
{
#if defined(_FOG_MATH_GET_SNAN_D)
  return _FOG_MATH_GET_SNAN_D();
#else
  _FOG_MATH_DECLARE_CONST_D(_const_snan_d, 0x7F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  return _FOG_MATH_GET_CONST_D(_const_snan_d);
#endif // _FOG_MATH_GET_SNAN_D
}

static FOG_INLINE float getQNanF()
{
#if defined(_FOG_MATH_GET_QNAN_F)
  return _FOG_MATH_GET_QNAN_F();
#else
  _FOG_MATH_DECLARE_CONST_F(_const_qnan_f, 0x7F, 0xC0, 0x00, 0x00);
  return _FOG_MATH_GET_CONST_F(_const_qnan_f);
#endif // _FOG_MATH_GET_QNAN_F
}

static FOG_INLINE double getQNanD()
{
#if defined(_FOG_MATH_GET_QNAN_D)
  return _FOG_MATH_GET_QNAN_D();
#else
  _FOG_MATH_DECLARE_CONST_D(_const_qnan_d, 0x7F, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  return _FOG_MATH_GET_CONST_D(_const_qnan_d);
#endif // _FOG_MATH_GET_QNAN_D
}

static FOG_INLINE float getPInfF()
{
#if defined(_FOG_MATH_GET_PINF_F)
  return _FOG_MATH_GET_PINF_F();
#else
  _FOG_MATH_DECLARE_CONST_F(_const_pinf_f, 0x7F, 0x80, 0x00, 0x00);
  return _FOG_MATH_GET_CONST_F(_const_pinf_f);
#endif
}

static FOG_INLINE double getPInfD()
{
#if defined(_FOG_MATH_GET_PINF_D)
  return _FOG_MATH_GET_PINF_D();
#else
  _FOG_MATH_DECLARE_CONST_D(_const_pinf_d, 0x7F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  return _FOG_MATH_GET_CONST_D(_const_pinf_d);
#endif
}

static FOG_INLINE float getNInfF()
{
#if defined(_FOG_MATH_GET_NINF_F)
  return _FOG_MATH_GET_NINF_F();
#else
  _FOG_MATH_DECLARE_CONST_F(_const_ninf_f, 0xFF, 0x80, 0x00, 0x00);
  return _FOG_MATH_GET_CONST_F(_const_ninf_f);
#endif
}

static FOG_INLINE double getNInfD()
{
#if defined(_FOG_MATH_GET_NINF_D)
  return _FOG_MATH_GET_NINF_D();
#else
  _FOG_MATH_DECLARE_CONST_D(_const_ninf_d, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  return _FOG_MATH_GET_CONST_D(_const_ninf_d);
#endif
}

// Abstract.
_FOG_MATH_DECLARE_VARIANT_TEMPLATE(getSNan)
_FOG_MATH_DECLARE_VARIANT_TEMPLATE(getQNan)
_FOG_MATH_DECLARE_VARIANT_TEMPLATE(getPInf)
_FOG_MATH_DECLARE_VARIANT_TEMPLATE(getNInf)

static FOG_INLINE bool isFinite(float x)
{
#if defined(_FOG_MATH_IS_FINITE_F)
  return _FOG_MATH_IS_FINITE_F(x);
#else
  FloatBits bits;
  bits.f = x;
  return (bits.u32 & 0x7F800000U) != 0x7F800000U;
#endif // _FOG_MATH_IS_FINITE_F
}

static FOG_INLINE bool isFinite(double x)
{
#if defined(_FOG_MATH_IS_FINITE_D)
  return _FOG_MATH_IS_FINITE_D(x);
#else
  DoubleBits bits;
  bits.d = x;
  return (bits.u32Hi & 0x7FF00000U) != 0x7FF00000U;
#endif // _FOG_MATH_IS_FINITE_D
}

static FOG_INLINE bool isInfinite(float x)
{
#if defined(_FOG_MATH_IS_INFINITE_F)
  return _FOG_MATH_IS_INFINITE_F(x);
#else
  FloatBits bits;
  bits.f = x;
  return (bits.u32 & 0x7F800000U) == 0x7F800000U;
#endif // _FOG_MATH_IS_INFINITE_F
}

static FOG_INLINE bool isInfinite(double x)
{
#if defined(_FOG_MATH_IS_INFINITE_D)
  return _FOG_MATH_IS_INFINITE_D(x);
#else
  DoubleBits bits;
  bits.d = x;
  return (bits.u32Hi & 0x7FF00000U) == 0x7FF00000U;
#endif // _FOG_MATH_IS_INFINITE_D
}

static FOG_INLINE bool isNaN(float x)
{
#if defined(_FOG_MATH_IS_NAN_F)
  return _FOG_MATH_IS_NAN_F(x);
#else
  return !(x == x);
#endif // _FOG_MATH_IS_NAN_F
}

static FOG_INLINE bool isNaN(double x)
{
#if defined(_FOG_MATH_IS_NAN_D)
  return _FOG_MATH_IS_NAN_D(x);
#else
  return !(x == x);
#endif // _FOG_MATH_IS_NAN_D
}

// Clean-up.
#undef _FOG_MATH_DECLARE_CONST_F
#undef _FOG_MATH_DECLARE_CONST_D
#undef _FOG_MATH_GET_CONST_F
#undef _FOG_MATH_GET_CONST_D
#undef _FOG_MATH_DECLARE_VARIANT_TEMPLATE

// ============================================================================
// [Fog::Math - IRound]
// ============================================================================

// According to MSDN the _M_IX86_FP is defined for x86 compilation. If the
// _M_IX86_FP is set to 2 then the SSE2-Enabled code generation is used,
// otherwise the x87 floating point stack is used.
//
// Note 1: When compiling for AMD64 the _M_IX86_FP is usually defined to 0, but
// the code is SSE2 enabled.
//
// Note 2: This code assumes that the rounding mode is set to NEAREST,
// otherwise the rounding will be incorrect and it's not our bug.

static FOG_INLINE int iround(float x)
{
#if defined(FOG_ARCH_X86_64) || defined(FOG_HARDCODE_SSE)
  return _mm_cvtss_si32(_mm_load_ss(&x));
#elif defined(FOG_ARCH_X86) && defined(FOG_CC_MSC)
  int t;
  __asm {
    fld dword ptr [x]
    fistp dword ptr [t]
  }
  return t;
#elif defined(FOG_ARCH_X86) && defined(FOG_CC_GNU)
  int t;
  __asm__ __volatile__("flds %1; fistpl %0;" : "=m" (t) : "m" (x));
  return t;
#else
  return (int)(x + ((x < 0.0f) ? -0.5f : 0.5f));
#endif
}

static FOG_INLINE int iround(double x)
{
#if defined(FOG_ARCH_X86_64) || defined(FOG_HARDCODE_SSE2)
  return _mm_cvtsd_si32(_mm_load_sd(&x));
#elif defined(FOG_ARCH_X86) && defined(FOG_CC_MSC)
  int t;
  __asm {
    fld qword ptr [x]
    fistp dword ptr [t]
  }
  return t;
#elif defined(FOG_ARCH_X86) && defined(FOG_CC_GNU)
  int t;
  __asm__ __volatile__("fldl %1; fistpl %0;" : "=m" (t) : "m" (x));
  return t;
#else
  return (int)(x + ((x < 0.0) ? -0.5 : 0.5));
#endif
}

// ============================================================================
// [Fog::Math - URound]
// ============================================================================

static FOG_INLINE uint uround(float x) { return (uint)iround(x); }
static FOG_INLINE uint uround(double x) { return (uint)iround(x); }

// ============================================================================
// [Fog::Math - IFloor]
// ============================================================================

static FOG_INLINE int ifloor(float x)
{
#if defined(FOG_ARCH_X86_64) || defined(FOG_HARDCODE_SSE)
  return _mm_cvttss_si32(_mm_load_ss(&x));
#else
  return (int)x;
#endif
}

static FOG_INLINE int ifloor(double x)
{
#if defined(FOG_ARCH_X86_64) || defined(FOG_HARDCODE_SSE2)
  return _mm_cvttsd_si32(_mm_load_sd(&x));
#else
  return (int)x;
#endif
}

// ============================================================================
// [Fog::Math - UFloor]
// ============================================================================

static FOG_INLINE uint ufloor(float x) { return (uint)ifloor(x); }
static FOG_INLINE uint ufloor(double x) { return (uint)ifloor(x); }

// ============================================================================
// [Fog::Math - ICeil]
// ============================================================================

static FOG_INLINE int iceil(float x) { return (int)::ceilf(x); }
static FOG_INLINE int iceil(double x) { return (int)::ceil(x); }

// ============================================================================
// [Fog::Math - UCeil]
// ============================================================================

static FOG_INLINE uint uceil(float x) { return (uint)iceil(x); }
static FOG_INLINE uint uceil(double x) { return (uint)iceil(x); }

// ============================================================================
// [Fog::Math - ByteFromFloat]
// ============================================================================

static FOG_INLINE uint32_t uroundToByte255(float x) { return uround(x * 255.0f); }
static FOG_INLINE uint32_t uroundToByte255(double x) { return uround(x * 255.0); }

static FOG_INLINE uint32_t uroundToByte256(float x) { return uround(x * 256.0f); }
static FOG_INLINE uint32_t uroundToByte256(double x) { return uround(x * 256.0); }

static FOG_INLINE uint32_t uroundToWord65535(float x) { return uround(x * 65535.0f); }
static FOG_INLINE uint32_t uroundToWord65535(double x) { return uround(x * 65535.0); }

static FOG_INLINE uint32_t uroundToWord65536(float x) { return uround(x * 65536.0f); }
static FOG_INLINE uint32_t uroundToWord65536(double x) { return uround(x * 65536.0); }

// ============================================================================
// [Fog::Math - Shift]
// ============================================================================

template<typename T, int Y>
static FOG_INLINE T shift(const T& x)
{
  if (Y < 0)
    return x << (-Y);
  else
    return x >> Y;
}

// ============================================================================
// [Fog::Math - FixedFromFloat]
// ============================================================================

// The formula used to get the magic number for conversion from 'float->fixed':
//
//   Magic = (1 << (FLOAT_MANTISSA_SIZE - FIXED_FRACTION_SIZE)) * 1.5
//
// This number can be used to do float to signed integer conversion (it handles
// well the unsigned case). The default behavior of the constant is to round,
// subtracting 0.5 from that constant means truncation.
//
// This code was buggy, mainly because there is unwanted rounding which cannot
// be disabled (Fog-Framework do not set FPU/XMM rounding mode).
//
// static FOG_INLINE Fixed24x8 fixed24x8FromFloat(double d)
// {
//   DoubleBits data;
//   data.d = d + 26388279066624.0;
//   return data.i32Lo;
// }
//
// static FOG_INLINE Fixed16x16 fixed16x16FromFloat(double d)
// {
//   DoubleBits data;
//   data.d = d + 103079215104.0;
//   return data.i32Lo;
// }

static FOG_INLINE Fixed24x8 fixed24x8FromFloat(double d) { return Math::ifloor(d * 256.0); }
static FOG_INLINE Fixed24x8 fixed24x8FromFloat(float d) { return Math::ifloor(d * 256.0f); }

static FOG_INLINE Fixed16x16 fixed16x16FromFloat(double d) { return Math::ifloor(d * 65536.0); }
static FOG_INLINE Fixed16x16 fixed16x16FromFloat(float d) { return Math::ifloor(d * 65536.0f); }

static FOG_INLINE Fixed48x16 fixed48x16FromFloat(double d) { return (Fixed48x16)(d * 65536.0); }
static FOG_INLINE Fixed48x16 fixed48x16FromFloat(float d) { return fixed48x16FromFloat(double(d)); }

static FOG_INLINE Fixed32x32 fixed32x32FromFloat(double d) { return (Fixed32x32)(d * 4294967296.0); }
static FOG_INLINE Fixed32x32 fixed32x32FromFloat(float d) { return fixed32x32FromFloat(double(d)); }

// ============================================================================
// [Fog::Math - Min]
// ============================================================================

//! @brief Returns lower number of @a a and @a b.
template<typename T>
FOG_STATIC_INLINE_T T min(const T& a, const T& b)
{
  return (a < b) ? a : b;
}

//! @brief Returns lower number of @a a, @a b and @a c.
//! @overload.
template<typename T>
FOG_STATIC_INLINE_T T min(const T& a, const T& b, const T& c)
{
  T t = a;
  if (t > b) t = b;
  if (t > c) t = c;
  return t;
}

//! @brief Returns lower number of @a a, @a b, @a c and @a d.
//! @overload.
template<typename T>
FOG_STATIC_INLINE_T T min(const T& a, const T& b, const T& c, const T& d)
{
  T t = a;
  if (t > b) t = b;
  if (t > c) t = c;
  if (t > d) t = d;
  return t;
}

// ============================================================================
// [Fog::Math - Max]
// ============================================================================

//! @brief Returns higher number of @a a and @a b.
template<typename T>
FOG_STATIC_INLINE_T T max(const T& a, const T& b)
{
  return (a > b) ? a : b;
}

//! @brief Returns higher number of @a a, @a b and @a c.
//! @overload.
template<typename T>
FOG_STATIC_INLINE_T T max(const T& a, const T& b, const T& c)
{
  T t = a;
  if (t < b) t = b;
  if (t < c) t = c;
  return t;
}

//! @brief Returns higher number of @a a, @a b, @a c and @a d.
//! @overload.
template<typename T>
FOG_STATIC_INLINE_T T max(const T& a, const T& b, const T& c, const T& d)
{
  T t = a;
  if (t < b) t = b;
  if (t < c) t = c;
  if (t < d) t = d;
  return t;
}

// ============================================================================
// [Fog::Math - Bound]
// ============================================================================

//! @brief Get value @a val bounded to @a min and @a max.
template<typename T>
FOG_STATIC_INLINE_T T bound(const T& val, const T& min, const T& max)
{
  return val < max ? (val > min ? val : min) : max;
}

// ============================================================================
// [Fog::Math - BoundToByte / BoundToWord]
// ============================================================================

static FOG_INLINE uint8_t boundToByte(int val)
{
  if (FOG_LIKELY((uint)val <= 0xFF))
    return (uint8_t)(uint)val;
  else
    return (val < 0) ? 0 : 0xFF;
}

static FOG_INLINE uint16_t boundToWord(int val)
{
  if (FOG_LIKELY((uint)val <= 0xFFFF))
    return (uint8_t)(uint)val;
  else
    return (val < 0) ? 0 : 0xFFFF;
}

// ============================================================================
// [Fog::Math - IsBounded]
// ============================================================================

template<typename T>
FOG_STATIC_INLINE_T bool isBounded(const T& x, const T& min, const T& max)
{
  return x >= min && x <= max;
}

#if defined(FOG_CC_HAS_NATIVE_CHAR_TYPE)
template<>
FOG_STATIC_INLINE_T bool isBounded(const char& x, const char& min, const char& max)
{
  return (unsigned char)(x - min) <= (unsigned char)(max - min);
}
#endif // FOG_CC_HAS_NATIVE_CHAR_TYPE

template<>
FOG_STATIC_INLINE_T bool isBounded(const signed char& x, const signed char& min, const signed char& max)
{
  return (unsigned char)(x - min) <= (unsigned char)(max - min);
}

template<>
FOG_STATIC_INLINE_T bool isBounded(const unsigned char& x, const unsigned char& min, const unsigned char& max)
{
  return (x - min) <= (max - min);
}

template<>
FOG_STATIC_INLINE_T bool isBounded(const short& x, const short& min, const short& max)
{
  return (unsigned short)(x - min) <= (unsigned short)(max - min);
}

template<>
FOG_STATIC_INLINE_T bool isBounded(const unsigned short& x, const unsigned short& min, const ushort& max)
{
  return (x - min) <= (max - min);
}

template<>
FOG_STATIC_INLINE_T bool isBounded(const int& x, const int& min, const int& max)
{
  return (unsigned int)(x - min) <= (unsigned int)(max - min);
}

template<>
FOG_STATIC_INLINE_T bool isBounded(const unsigned int& x, const unsigned int& min, const uint& max)
{
  return (x - min) <= (max - min);
}

template<>
FOG_STATIC_INLINE_T bool isBounded(const int64_t& x, const int64_t& min, const int64_t& max)
{
  return (uint64_t)(x - min) <= (uint64_t)(max - min);
}

template<>
FOG_STATIC_INLINE_T bool isBounded(const uint64_t& x, const uint64_t& min, const uint64_t& max)
{
  return (x - min) <= (max - min);
}

// ============================================================================
// [Fog::Math - CanSum]
// ============================================================================

//! @brief Get whether it is possible to sum @a a and @a without overflow.
//!
//! @note This function is designed for unsigned integers only.
template<typename T>
FOG_STATIC_INLINE_T bool canSum(const T& a, const T& b)
{
  // There are several possibilities which can be used. Maybe different
  // approach for each compiler is good idea.
  //
  //   - return a + b >= a
  //   - return a >= ~0 - b

  return a + b >= a;
}

// ============================================================================
// [Fog::Math - Abs]
// ============================================================================

//! @brief Get absolute value of @a a
template<typename T>
FOG_STATIC_INLINE_T T abs(const T& a)
{ return (a >= 0) ? a : -a; }

template<>
FOG_STATIC_INLINE_T float abs(const float& a)
{
  return ::fabsf(a);
}

template<>
FOG_STATIC_INLINE_T double abs(const double& a)
{
  return ::fabs(a);
}


// ============================================================================
// [Fog::Math - Ceil / Floor]
// ============================================================================

static FOG_INLINE float ceil(float x) { return ::ceilf(x); }
static FOG_INLINE double ceil(double x) { return ::ceil(x); }

static FOG_INLINE float floor(float x) { return ::floorf(x); }
static FOG_INLINE double floor(double x) { return ::floor(x); }

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
// [Fog::Math - Pow]
// ============================================================================

static FOG_INLINE float pow(float x, float y) { return ::powf(x, y); }
static FOG_INLINE double pow(double x, double y) { return ::pow(x, y); }

static FOG_INLINE float pow2(float x) { return x * x; }
static FOG_INLINE double pow2(double x) { return x * x; }

static FOG_INLINE float pow3(float x) { return x * x * x; }
static FOG_INLINE double pow3(double x) { return x * x * x; }

static FOG_INLINE float pow4(float x) { float x2 = x * x; return x2 * x2; }
static FOG_INLINE double pow4(double x) { double x2 = x * x; return x2 * x2; }

// ============================================================================
// [Fog::Math - Exp]
// ============================================================================

static FOG_INLINE float exp(float x) { return ::expf(x); }
static FOG_INLINE double exp(double x) { return ::exp(x); }

// ============================================================================
// [Fog::Math - Sqrt]
// ============================================================================

static FOG_INLINE float sqrt(float x) { return ::sqrtf(x); }
static FOG_INLINE double sqrt(double x) { return ::sqrt(x); }

// ============================================================================
// [Fog::Math - Cbrt]
// ============================================================================

static FOG_INLINE float cbrt(float x)
{
  return (x > 0.0f) ?  pow( x, float(MATH_1_DIV_3)) :
         (x < 0.0f) ? -pow(-x, float(MATH_1_DIV_3)) : 0.0f;
}

static FOG_INLINE double cbrt(double x)
{
  return (x > 0.0f) ?  pow( x, MATH_1_DIV_3) :
         (x < 0.0f) ? -pow(-x, MATH_1_DIV_3) : 0.0f;
}

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
// [Fog::Math - Log]
// ============================================================================

static FOG_INLINE float log(float x) { return ::logf(x); }
static FOG_INLINE double log(double x) { return ::log(x); }

// ============================================================================
// [Fog::Math - Bessel]
// ============================================================================

//! @brief Calculates Bessel function of the first kind of order n.
//!
//! @param x Value at which the Bessel function is required.
//! @param n The order.
static FOG_INLINE float besj(float x, int n)
{
  return fog_api.mathf_besj(x, n);
}

//! @overload
static FOG_INLINE double besj(double x, int n)
{
  return fog_api.mathd_besj(x, n);
}

// ============================================================================
// [Fog::Math - Dist]
// ============================================================================

static FOG_INLINE float squaredDistance(float x, float y) { return x * x + y * y; }
static FOG_INLINE double squaredDistance(double x, double y) { return x * x + y * y; }

static FOG_INLINE float squaredDistance(float x0, float y0, float x1, float y1) { return squaredDistance(x1 - x0, y1 - y0); }
static FOG_INLINE double squaredDistance(double x0, double y0, double x1, double y1) { return squaredDistance(x1 - x0, y1 - y0); }

static FOG_INLINE float euclideanDistance(float x, float y) { return Math::sqrt(x * x + y * y); }
static FOG_INLINE double euclideanDistance(double x, double y) { return Math::sqrt(x * x + y * y); }

static FOG_INLINE float euclideanDistance(float x0, float y0, float x1, float y1) { return euclideanDistance(x1 - x0, y1 - y0); }
static FOG_INLINE double euclideanDistance(double x0, double y0, double x1, double y1) { return euclideanDistance(x1 - x0, y1 - y0); }

//! @}

} // Math namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MATH_MATH_H
