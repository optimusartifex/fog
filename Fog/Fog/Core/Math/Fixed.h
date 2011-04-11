// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MATH_FIXED_H
#define _FOG_CORE_MATH_FIXED_H

// [Dependencies]
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Math/FloatBits.h>
#include <Fog/Core/Math/Math.h>

namespace Fog {

// ============================================================================
// [Fog::FixedNxN]
// ============================================================================

//! @addtogroup Fog_Core_Tools
//! @{

// 32-bit fixed point.
typedef int32_t Fixed8x24;
typedef int32_t Fixed16x16;
typedef int32_t Fixed24x8;

// 64-bit fixed point.
typedef int64_t Fixed32x32;
typedef int64_t Fixed48x16;
typedef int64_t Fixed56x8;

// ============================================================================
// [Fog::FIXED_8x24]
// ============================================================================

enum FIXED_8x24_CONST
{
  FIXED_8x24_ONE  = 0x01000000,
  FIXED_8x24_HALF = 0x00800000
};

// ============================================================================
// [Fog::FIXED_16x16]
// ============================================================================

enum FIXED_16x16_CONST
{
  FIXED_16x16_ONE  = 0x00010000,
  FIXED_16x16_HALF = 0x00008000
};

// ============================================================================
// [Fog::FIXED_24x8]
// ============================================================================

enum FIXED_24x8_CONST
{
  FIXED_24x8_ONE  = 0x00000100,
  FIXED_24x8_HALF = 0x00000080
};

//! @}

// ============================================================================
// [Fog::Math]
// ============================================================================

namespace Math {

//! @addtogroup Fog_Core_Tools
//! @{

static FOG_INLINE Fixed16x16 fixed16x16FromFloat(double d)
{
  // The equivalent code is:
  // return (int32_t)(d * 65536.0);
  DoubleBits data;
  data.d = d + 103079215104.0;
  return data.i32Lo;
}

static FOG_INLINE Fixed8x24 fixed8x24FromFloat(double d) { return (Fixed8x24)(d * (double)FIXED_8x24_ONE); }
static FOG_INLINE Fixed24x8 fixed24x8FromFloat(double d) { return (Fixed24x8)(d * (double)FIXED_24x8_ONE); }
static FOG_INLINE Fixed48x16 fixed48x16FromFloat(double d) { return (Fixed48x16)(d * (double)FIXED_16x16_ONE); }
static FOG_INLINE Fixed32x32 fixed32x32FromFloat(double d) { return (Fixed32x32)(d * 4294967296.0); }

static FOG_INLINE Fixed8x24 fixed8x24FromFloat(float d) { return (Fixed8x24)(d * (float)FIXED_8x24_ONE); }
static FOG_INLINE Fixed8x24 fixed16x16FromFloat(float d) { return (Fixed16x16)(d * (float)FIXED_16x16_ONE); }
static FOG_INLINE Fixed24x8 fixed24x8FromFloat(float d) { return (Fixed24x8)(d * (float)FIXED_24x8_ONE); }
static FOG_INLINE Fixed48x16 fixed48x16FromFloat(float d) { return (Fixed48x16)(d * (float)FIXED_16x16_ONE); }
static FOG_INLINE Fixed32x32 fixed32x32FromFloat(float d) { return (Fixed32x32)((double)d * 4294967296.0); }

//! @}

} // Math namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MATH_FIXED_H
