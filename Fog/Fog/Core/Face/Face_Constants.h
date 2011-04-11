// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACE_CONSTANTS_H
#define _FOG_CORE_FACE_FACE_CONSTANTS_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>

namespace Fog {
namespace Face {

// ============================================================================
// [Constants]
// ============================================================================

//! @internal
//!
//! @brief Reciprocal table that can be used to demultiply color value in
//! 24-bit precision.
//!
//! Reciprocal table based on material provided by M Joonas Pihlaja, see
//! <http://cgit.freedesktop.org/~joonas/unpremultiply/tree/README>
//!
//! The reciprocal_table[i] entries are defined by
//!
//!  0       when i = 0
//!  255 / i when i > 0
//!
//! represented in fixed point format with RECIPROCAL_BITS of
//! precision and errors rounded up.
//!
//! #define ceil_div(a, b) ((a) + (b)-1) / (b)
//!
//! for each:
//!   (i) ? ceil_div(255 * (1 << 16), (i)) : 0) : 0
//!
//! Example code how the table can be used for PRGB32 pixel demultiply:
//!
//! @verbatim
//! static void FOG_FASTCALL demultiply_argb(uint32_t* dst, const uint32_t* src, int w)
//! {
//!   int i;
//!
//!   for (i = 0; i < w; i++)
//!   {
//!     uint32_t rgba = src[i];
//!     uint32_t a = (rgba >> 24);
//!     uint32_t r = (rgba >> 16) & 0xFF;
//!     uint32_t g = (rgba >>  8) & 0xFF;
//!     uint32_t b = (rgba      ) & 0xFF;
//!
//!     uint32_t recip = Face::_u8_divide_table_d[a];
//!
//!     r = ((r * recip)      ) & 0x00FF0000;
//!     g = ((g * recip) >>  8) & 0x0000FF00;
//!     b = ((b * recip) >> 16) & 0x000000FF;
//!
//!     dst[i] = r | g | b | (a << 24);
//!   }
//! }
//! @endverbatim
extern FOG_API const uint32_t _u8_divide_table_d[256];

//! @internal
//!
//! @brief Reciprocal table that can be used to demultiply color value in
//! 16-bit precision (used in MMX/SSE2 demultiply-code).
extern FOG_API const uint16_t _u8_divide_table_w[256 * 4];

//! @internal
//!
//! @brief Reciprocal table that can be used to demultiply color value in
//! floating point precision (for image formats and filters that need floating
//! point accuracy).
extern FOG_API const float _u8_divide_table_f[256];

} // Face namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_FACE_FACE_CONSTANTS_H
