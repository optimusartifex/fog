// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_CONSTANTS_P_H
#define _FOG_GRAPHICS_CONSTANTS_P_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Graphics/Constants.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Fog::FIXED]
// ============================================================================

enum FIXED_16x16_ENUM
{
  FIXED_16x16_ONE  = 0x00010000,
  FIXED_16x16_HALF = 0x00008000
};

// ============================================================================
// [Fog::OPERATOR_CHAR]
// ============================================================================

//! @internal
//!
//! @brief Operator characteristics.
//! 
//! Operator characteristics are used internally by @c RasterPaintEngine and 
//! compositing templates in @c Fog::RasterEngine.
enum OPERATOR_CHAR
{
  //! @brief Operator uses destination color value.
  OPERATOR_CHAR_DST_C_USED = (1 << 0),
  //! @brief Operator uses destination alpha value.
  OPERATOR_CHAR_DST_A_USED = (1 << 1),
  //! @brief Operator uses source color value.
  OPERATOR_CHAR_SRC_C_USED = (1 << 2),
  //! @brief Operator uses source alpha value.
  OPERATOR_CHAR_SRC_A_USED = (1 << 3),

  //! @brief Operator is not bound.
  //!
  //! Bound operators means that it's possible to multiply pixel by weight value
  //! and compositing operation will be still valid (this is related to
  //! compositing using external mask).
  //!
  //! Typical bound operator is @c OPERATOR_SRC_OVER, but for example
  //! @c OPERATOR_SRC is not bound.
  OPERATOR_CHAR_UNBOUND = (1 << 4),
  //! @brief Operator is not bound, but in formula is always Sca.m or Sa.m.
  //!
  //! This flag is used only as optimization and it must be set together with
  //! @c OPERATOR_CHAR_UNBOUND.
  OPERATOR_CHAR_UNBOUND_MSK_IN = (1 << 5),

  //! @brief Operator is always nop (@c OPERATOR_DST).
  OPERATOR_CHAR_NOP_ALWAYS = (1 << 6),
  //! @brief Operator is nop if destination alpha is zero.
  OPERATOR_CHAR_NOP_IF_DST_A_ZERO = (1 << 7),
  //! @brief Operator is nop if destination alpha is fully opaque.
  OPERATOR_CHAR_NOP_IF_DST_A_FULL = (1 << 8),
  //! @brief Operator is nop if source alpha value is zero.
  OPERATOR_CHAR_NOP_IF_SRC_A_ZERO = (1 << 9),
  //! @brief Operator is nop if source alpha value is fully opaque.
  OPERATOR_CHAR_NOP_IF_SRC_A_FULL = (1 << 10),

  //! @brief Operator can be done on packed data (this is hint for mmx/sse2
  //! templates).
  OPERATOR_CHAR_PACKED = (1 << 11),

  //! @brief Prefer color in 0xFFRRGGBB format instead of 0xXXRRGGBB. Used as
  //! an optimization hint for MMX/SSE2 code.
  OPERATOR_CHAR_PREFER_FRGB = (1 << 12)
};

// ============================================================================
// [OPERATOR_CHAR_STATIC]
// ============================================================================

// Skip documenting this section.
#if !defined(FOG_DOXYGEN)

// Shorter names are better for our table.
#define DST_C          OPERATOR_CHAR_DST_C_USED
#define DST_A          OPERATOR_CHAR_DST_A_USED
#define SRC_C          OPERATOR_CHAR_SRC_C_USED
#define SRC_A          OPERATOR_CHAR_SRC_A_USED
#define UNBOUND        OPERATOR_CHAR_UNBOUND
#define UNBOUND_MSK_IN OPERATOR_CHAR_UNBOUND | OPERATOR_CHAR_UNBOUND_MSK_IN
#define NOP            OPERATOR_CHAR_NOP_ALWAYS
#define NOP_DST_A_Z    OPERATOR_CHAR_NOP_IF_DST_A_ZERO
#define NOP_DST_A_F    OPERATOR_CHAR_NOP_IF_DST_A_FULL
#define NOP_SRC_A_Z    OPERATOR_CHAR_NOP_IF_SRC_A_ZERO
#define NOP_SRC_A_F    OPERATOR_CHAR_NOP_IF_SRC_A_FULL
#define HINT_PACKED    OPERATOR_CHAR_PACKED
#define HINT_FRGB      OPERATOR_CHAR_PREFER_FRGB

#endif // FOG_DOXYGEN

//! @internal
//!
//! @brief Operator characteristics for each operator (used by blitter templates).
enum OPERATOR_CHAR_STATIC
{
  OPERATOR_CHAR_SRC        = 0     | 0     | SRC_C | SRC_A | UNBOUND_MSK_IN | 0           | HINT_PACKED,
  OPERATOR_CHAR_DST        = DST_C | DST_A | 0     | 0     | UNBOUND        | NOP         | HINT_PACKED,
  OPERATOR_CHAR_SRC_OVER   = DST_C | DST_A | SRC_C | SRC_A | 0              | NOP_SRC_A_Z | 0,
  OPERATOR_CHAR_DST_OVER   = DST_C | DST_A | SRC_C | SRC_A | 0              | NOP_DST_A_F | 0,
  OPERATOR_CHAR_SRC_IN     = 0     | DST_A | SRC_C | SRC_A | UNBOUND_MSK_IN | NOP_DST_A_Z | 0,
  OPERATOR_CHAR_DST_IN     = DST_C | DST_A | 0     | SRC_A | UNBOUND_MSK_IN | NOP_SRC_A_F | 0,
  OPERATOR_CHAR_SRC_OUT    = 0     | DST_A | SRC_C | SRC_A | UNBOUND_MSK_IN | 0           | 0,
  OPERATOR_CHAR_DST_OUT    = DST_C | DST_A | 0     | SRC_A | 0              | NOP_SRC_A_Z | 0,
  OPERATOR_CHAR_SRC_ATOP   = DST_C | DST_A | SRC_C | SRC_A | 0              | NOP_SRC_A_Z | HINT_FRGB,
  OPERATOR_CHAR_DST_ATOP   = DST_C | DST_A | SRC_C | SRC_A | UNBOUND_MSK_IN | 0           | 0,
  OPERATOR_CHAR_XOR        = DST_C | DST_A | SRC_C | SRC_A | 0              | NOP_SRC_A_Z | 0,
  OPERATOR_CHAR_CLEAR      = 0     | 0     | 0     | 0     | UNBOUND        | 0           | 0,
  OPERATOR_CHAR_ADD        = DST_C | DST_A | SRC_C | SRC_A | 0              | NOP_SRC_A_Z | HINT_PACKED,
  OPERATOR_CHAR_SUBTRACT   = DST_C | DST_A | SRC_C | SRC_A | 0              | NOP_SRC_A_Z | HINT_PACKED,
  OPERATOR_CHAR_MULTIPLY   = DST_C | DST_A | SRC_C | SRC_A | UNBOUND        | NOP_SRC_A_Z | 0,
  OPERATOR_CHAR_SCREEN     = DST_C | DST_A | SRC_C | SRC_A | 0              | NOP_SRC_A_Z | 0,
  OPERATOR_CHAR_DARKEN     = DST_C | DST_A | SRC_C | SRC_A | 0              | NOP_SRC_A_Z | 0,
  OPERATOR_CHAR_LIGHTEN    = DST_C | DST_A | SRC_C | SRC_A | 0              | NOP_SRC_A_Z | 0,
  OPERATOR_CHAR_DIFFERENCE = DST_C | DST_A | SRC_C | SRC_A | 0              | NOP_SRC_A_Z | 0,
  OPERATOR_CHAR_EXCLUSION  = DST_C | DST_A | SRC_C | SRC_A | 0              | NOP_SRC_A_Z | 0,
  OPERATOR_CHAR_INVERT     = DST_C | DST_A | 0     | SRC_A | 0              | NOP_SRC_A_Z | 0,
  OPERATOR_CHAR_INVERT_RGB = DST_C | DST_A | SRC_C | SRC_A | 0              | NOP_SRC_A_Z | 0
};

// Cleanup
#undef DST_C
#undef DST_A
#undef SRC_C
#undef SRC_A
#undef UNBOUND
#undef UNBOUND_MSK_IN
#undef NOP
#undef NOP_DST_A_Z
#undef NOP_DST_A_F
#undef NOP_SRC_A_Z
#undef NOP_SRC_A_F
#undef OP_PACKED

//! @internal
//!
//! @brief Operator characteristics that can be used by raster paint engine
//! dynamically (based on operator).
extern FOG_API uint32_t OperatorCharacteristics[OPERATOR_COUNT];

// ============================================================================
// [PATTERN_FETCH_MODE]
// ============================================================================

//! @internal
//!
//! @brief Pattern fetch mode (see @c RasterEngine).
enum PATTERN_FETCH_MODE
{
  //! @brief Pattern fetcher can use data that owns source pattern (this is
  //! in most cases the image pixels owned by @c Image).
  //!
  //! This flag is the default and in some cases it is performance improvement,
  //! because the data don't need to be copied to the provided temporary buffer.
  //!
  //! If sharers are used then this mode is never used by pattern fetcher,
  //! because shaders need to overwrite the fetched pixels.
  PATTERN_FETCH_CAN_USE_SRC = 0,

  //! @brief Pattern fetcher can use only provided buffer to fetch the data to.
  //!
  //! This mode is opposite to @c PATTERN_FETCH_CAN_USE_SRC. It tells to 
  //! fetcher to always use the provided buffer for all fetches. If shaders are
  //! used then these data will be overwritten so it's important to tell this
  //! fact to fetcher.
  //!
  //! This mode can be also used when using @c OPERATOR_SRC and fetcher is not 
  //! called using temporary buffer, but the target image buffer. This means 
  //! that pattern fetcher will fetch data into the target buffer and no 
  //! another copy is needed (optimization).
  PATTERN_FETCH_BUFFER_ONLY = 1
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_CONSTANTS_P_H
