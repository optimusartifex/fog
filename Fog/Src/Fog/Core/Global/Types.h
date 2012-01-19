// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_TYPES_H
#define _FOG_CORE_GLOBAL_TYPES_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>

// ============================================================================
// [__p64]
// ============================================================================

//! @typedef __p64
//! @brief 64-bit packed unsigned integer used by Fog::Face framework.

#if defined(FOG_ARCH_NATIVE_P64)
typedef uint64_t __p64;
#else
union __p64
{
  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE uint64_t& operator()() { return u64; }
  FOG_INLINE const uint64_t& operator()() const { return u64; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  struct
  {
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
    // MEM:33332222 11110000
    // HEX:00001111 22223333
    //
    // [7 6 5 4|3 2 1 0]
    uint32_t u32Lo;
    uint32_t u32Hi;
#else
    // MEM:00001111 22223333
    // HEX:00001111 22223333
    //
    // [0 1 2 3|4 5 6 7]
    uint32_t u32Hi;
    uint32_t u32Lo;
#endif
  };

  uint64_t u64;
};
#endif // FOG_ARCH_BITS

// ============================================================================
// [Fog::]
// ============================================================================

namespace Fog {

//! @addtogroup Fog_Core_Global
//! @{

// ============================================================================
// [Fog::Fixed...]
// ============================================================================

typedef int32_t Fixed24x8;
typedef int32_t Fixed16x16;

typedef int64_t Fixed32x32;
typedef int64_t Fixed48x16;

// ============================================================================
// [Fog::Math - FloatBits]
// ============================================================================

//! @internal
//!
//! @brief Single precision floating point bits.
union FloatBits
{
  //! @brief Float value.
  float f;

  //! @brief Int32 value.
  int32_t i32;
  //! @brief UInt32 value.
  uint32_t u32;
};

// ============================================================================
// [Fog::Math - DoubleBits]
// ============================================================================

//! @internal
//!
//! @brief Double precision floating point bits.
union DoubleBits
{
  //! @brief Double value.
  double d;

  //! @brief Int32[2] array.
  int32_t i32[2];
  //! @brief UInt32[2] array.
  uint32_t u32[2];

  //! @brief Int64 value.
  int64_t i64;
  //! @brief UInt64 value.
  uint64_t u64;

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  struct { int32_t  i32Lo; int32_t  i32Hi; };
  struct { uint32_t u32Lo; uint32_t u32Hi; };
#else
  struct { int32_t  i32Hi; int32_t  i32Lo; };
  struct { uint32_t u32Hi; uint32_t u32Lo; };
#endif
};

// ============================================================================
// [Fog::UInt64Bits]
// ============================================================================

//! @internal
//!
//! @brief 64-bit unsigned integer bits.
union UInt64Bits
{
  int8_t i8[8];
  uint8_t u8[8];

  int32_t i32[2];
  uint32_t u32[2];

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  struct { uint32_t u32Lo, u32Hi; };
  struct { int32_t  i32Lo, i32Hi; };
#else
  struct { uint32_t u32Hi, u32Lo; };
  struct { int32_t  i32Hi, i32Lo; };
#endif

  int64_t i64;
  uint64_t u64;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_GLOBAL_TYPES_H
