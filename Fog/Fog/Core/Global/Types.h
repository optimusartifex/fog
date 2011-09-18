// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_TYPES_H
#define _FOG_CORE_GLOBAL_TYPES_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>

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
// [Fog::Uint64Union]
// ============================================================================

union UInt64Union
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
