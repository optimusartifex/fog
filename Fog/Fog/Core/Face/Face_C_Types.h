// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACE_C_TYPES_H
#define _FOG_CORE_FACE_FACE_C_TYPES_H

// [Dependencies]
#include <Fog/Core/Face/Features.h>

namespace Fog {
namespace Face {

//! @addtogroup Fog_Face_C
//! @{

// ============================================================================
// [Fog::Face - P32 - Types]
// ============================================================================

//! @typedef p32
//! @brief 32-bit packed unsigned integer used by Fog::Face framework.

typedef uint32_t p32;

// ============================================================================
// [Fog::Face - P64 - Types]
// ============================================================================

//! @typedef p64
//! @brief 64-bit packed unsigned integer used by Fog::Face framework.

#if defined(FOG_FACE_P64_IS_TYPE)
typedef uint64_t p64;
#else
union p64
{
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
#endif // FOG_FACE_P64_IS_TYPE

} // Face namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_FACE_FACE_C_TYPES_H
