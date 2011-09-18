// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MATH_FLOATBITS_H
#define _FOG_CORE_MATH_FLOATBITS_H

// [Dependencies]
#include <Fog/Core/Global/TypeInfo.h>

namespace Fog {

//! @addtogroup Fog_Core_Math
//! @{

// ============================================================================
// [Fog::Math - FloatBits]
// ============================================================================

//! @internal
//!
//! @brief Helper union that contains 32-bit float and 32-bit integer.
union FloatBits
{
  //! @brief Float data.
  float f;

  //! @brief Int32 data.
  int32_t i32;
  //! @brief UInt32 data.
  uint32_t u32;
};

// ============================================================================
// [Fog::Math - DoubleBits]
// ============================================================================

//! @internal
//!
//! @brief Helper union that contains 64-bit float and 64-bit integer.
union DoubleBits
{
  //! @brief Double data.
  double d;

  //! @brief Int32[2] data.
  int32_t i32[2];
  //! @brief UInt32[2] data.
  uint32_t u32[2];

  //! @brief Int64 data.
  int64_t i64;
  //! @brief UInt64 data.
  uint64_t u64;

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  struct { int32_t  i32Lo; int32_t  i32Hi; };
  struct { uint32_t u32Lo; uint32_t u32Hi; };
#else
  struct { int32_t  i32Hi; int32_t  i32Lo; };
  struct { uint32_t u32Hi; uint32_t u32Lo; };
#endif
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MATH_FLOATBITS_H
