// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MATH_CONVERT_H
#define _FOG_CORE_MATH_CONVERT_H

// [Dependencies]
#include <Fog/Core/Global/Types.h>
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/FloatBits.h>

#if defined(FOG_HARDCODE_SSE)
# include <Fog/Core/Cpu/Intrin_SSE.h>
#endif // FOG_HARDCODE_SSE

#if defined(FOG_HARDCODE_SSE2)
# include <Fog/Core/Cpu/Intrin_SSE2.h>
#endif // FOG_HARDCODE_SSE2

// [Dependencies - C]
#include <math.h>

namespace Fog {
namespace Math {

//! @addtogroup Fog_Core_Math
//! @{

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
// [Fog::Math - Fixed]
// ============================================================================

// The formula used to get the magic number for conversion from 'float->fixed':
//
//   Magic = (1 << (FLOAT_MANTISSA_SIZE - FIXED_FRACTION_SIZE)) * 1.5
//
// This number can be used to do float to signed integer conversion (it handles
// well the unsigned case). The default behavior of the constant is to round,
// subtracting 0.5 from that constant means truncation.

static FOG_INLINE Fixed24x8 fixed24x8FromFloat(double d)
{
  DoubleBits data;
  data.d = d + 26388279066624.0;
  return data.i32Lo;
}

static FOG_INLINE Fixed16x16 fixed16x16FromFloat(double d)
{
  DoubleBits data;
  data.d = d + 103079215104.0;
  return data.i32Lo;
}

static FOG_INLINE Fixed48x16 fixed48x16FromFloat(double d) { return (Fixed48x16)(d * 65536.0); }
static FOG_INLINE Fixed32x32 fixed32x32FromFloat(double d) { return (Fixed32x32)(d * 4294967296.0); }

static FOG_INLINE Fixed24x8 fixed24x8FromFloat(float d) { return fixed24x8FromFloat(double(d)); }
static FOG_INLINE Fixed16x16 fixed16x16FromFloat(float d) { return fixed16x16FromFloat(double(d)); }

static FOG_INLINE Fixed48x16 fixed48x16FromFloat(float d) { return fixed48x16FromFloat(double(d)); }
static FOG_INLINE Fixed32x32 fixed32x32FromFloat(float d) { return fixed32x32FromFloat(double(d)); }

//! @}

} // Math namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MATH_CONVERT_H
