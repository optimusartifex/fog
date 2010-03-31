// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_CONSTANTS_P_H
#define _FOG_GRAPHICS_CONSTANTS_P_H

// [Dependencies]
#include <Fog/Build/Build.h>
#include <Fog/Graphics/Constants.h>

//! @addtogroup Fog_Graphics_Private
//! @{

namespace Fog {

// ============================================================================
// [Fog::ALPHA_TYPE]
// ============================================================================

//! @brief Results from some color analyzer functions.
//!
//! Usualy used to improve performance of image processing. Algorithm designed
//! for full opaque pixels is always faster than generic algorithm for image
//! with or without alpha channel.
enum ALPHA_TYPE
{
  //! @brief Alpha values are variant.
  ALPHA_VARIANT = -1,
  //! @brief All alpha values are transparent (all 0).
  ALPHA_TRANSPARENT = 0,
  //! @brief All alpha values are opaque (all 255).
  ALPHA_OPAQUE = 255
};

// ============================================================================
// [Fog::OPERATOR_CHAR]
// ============================================================================

//! @brief Operator characteristics (used internally by @c PaintEngine).
//!
//! Characteristics are always stored in bitfield.
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
  //! @brief Operator is nop if destination alpha is zero.
  OPERATOR_CHAR_NOP_IF_DST_A_FULL = (1 << 8),
  //! @brief Operator is nop if source alpha value is zero.
  OPERATOR_CHAR_NOP_IF_SRC_A_ZERO = (1 << 9),
  //! @brief Operator is nop if source alpha value is zero.
  OPERATOR_CHAR_NOP_IF_SRC_A_FULL = (1 << 10),

  //! @brief Operator can done on packed data (this is hint for mmx/sse2 templates).
  OPERATOR_CHAR_PACKED = (1 << 1)
};

// ============================================================================
// [OPERATOR_CHAR_STATIC]
// ============================================================================

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
  OPERATOR_CHAR_SRC_ATOP   = DST_C | DST_A | SRC_C | SRC_A | 0              | NOP_SRC_A_Z | 0,
  OPERATOR_CHAR_DST_ATOP   = DST_C | DST_A | SRC_C | SRC_A | UNBOUND_MSK_IN | 0           | 0,
  OPERATOR_CHAR_XOR        = DST_C | DST_A | SRC_C | SRC_A | 0              | NOP_SRC_A_Z | 0,
  OPERATOR_CHAR_CLEAR      = 0     | 0     | 0     | 0     | UNBOUND        | 0           | 0,
  OPERATOR_CHAR_ADD        = DST_C | DST_A | SRC_C | SRC_A | 0              | NOP_SRC_A_Z | HINT_PACKED,
  OPERATOR_CHAR_SUBTRACT   = DST_C | DST_A | SRC_C | SRC_A | 0              | NOP_SRC_A_Z | 0,
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

extern FOG_HIDDEN uint32_t OperatorCharacteristics[OPERATOR_COUNT];

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_CONSTANTS_P_H
