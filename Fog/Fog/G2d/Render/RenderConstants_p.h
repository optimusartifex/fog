// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_RENDER_RENDERCONSTANTS_H
#define _FOG_G2D_RENDER_RENDERCONSTANTS_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>

namespace Fog {

//! @addtogroup Fog_G2d_Render
//! @{

// ============================================================================
// [Fog::Render - RENDER_CBLIT]
// ============================================================================

enum RENDER_CBLIT
{
  RENDER_CBLIT_PRGB = 0,
  RENDER_CBLIT_XRGB = 1,
  RENDER_CBLIT_COUNT = 2
};

// ============================================================================
// [Fog::Render - RENDER_VBLIT]
// ============================================================================

//! @brief Compatibility format IDs used by the compositing-ext operators.
enum RENDER_VBLIT
{
  // --------------------------------------------------------------------------
  // [PRGB32]
  // --------------------------------------------------------------------------

  RENDER_VBLIT_PRGB32_VS_PRGB32 = 0,
  RENDER_VBLIT_PRGB32_VS_XRGB32 = 1,
  RENDER_VBLIT_PRGB32_VS_RGB24 = 2,

  // --------------------------------------------------------------------------
  // [XRGB32]
  // --------------------------------------------------------------------------

  RENDER_VBLIT_XRGB32_VS_PRGB32 = 0,
  RENDER_VBLIT_XRGB32_VS_XRGB32 = 1,
  RENDER_VBLIT_XRGB32_VS_RGB24 = 2,

  // --------------------------------------------------------------------------
  // [RGB24]
  // --------------------------------------------------------------------------

  RENDER_VBLIT_RGB24_VS_PRGB32 = 0,
  RENDER_VBLIT_RGB24_VS_XRGB32 = 1,
  RENDER_VBLIT_RGB24_VS_RGB24 = 2,

  // --------------------------------------------------------------------------
  // [A8]
  // --------------------------------------------------------------------------

  RENDER_VBLIT_A8_VS_A8 = 0,

  // --------------------------------------------------------------------------
  // [PRGB64]
  // --------------------------------------------------------------------------

  RENDER_VBLIT_PRGB64_VS_PRGB64 = 0,
  RENDER_VBLIT_PRGB64_VS_RGB48 = 1,
  RENDER_VBLIT_PRGB64_VS_PRGB32 = 2,

  // --------------------------------------------------------------------------
  // [RGB48]
  // --------------------------------------------------------------------------

  RENDER_VBLIT_RGB48_VS_PRGB64 = 0,
  RENDER_VBLIT_RGB48_VS_RGB48 = 1,
  RENDER_VBLIT_RGB48_VS_PRGB32 = 2,

  // --------------------------------------------------------------------------
  // [A16]
  // --------------------------------------------------------------------------

  RENDER_VBLIT_A16_VS_A16 = 0,
  RENDER_VBLIT_A16_VS_A8 = 1,

  // --------------------------------------------------------------------------
  // [...]
  // --------------------------------------------------------------------------

  //! @brief Count of VBlit formats.
  RENDER_VBLIT_COUNT = 3,

  //! @brief Invalid VBlit format.
  RENDER_VBLIT_INVALID = 3
};

// ============================================================================
// [Fog::Render - RENDER_COMPOSITE]
// ============================================================================

enum RENDER_COMPOSITE
{
  // --------------------------------------------------------------------------
  // [CompositeCore]
  // --------------------------------------------------------------------------

  RENDER_COMPOSITE_CORE_START      = 0,
  RENDER_COMPOSITE_CORE_COUNT      = 2,

  RENDER_COMPOSITE_CORE_SRC        = COMPOSITE_SRC         - RENDER_COMPOSITE_CORE_START,
  RENDER_COMPOSITE_CORE_SRC_OVER   = COMPOSITE_SRC_OVER    - RENDER_COMPOSITE_CORE_START,

  // --------------------------------------------------------------------------
  // [CompositeExt]
  // --------------------------------------------------------------------------

  RENDER_COMPOSITE_EXT_START       = 2,
  RENDER_COMPOSITE_EXT_COUNT       = COMPOSITE_COUNT       - RENDER_COMPOSITE_EXT_START,

  RENDER_COMPOSITE_EXT_SRC_IN      = COMPOSITE_SRC_IN      - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_SRC_OUT     = COMPOSITE_SRC_OUT     - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_SRC_ATOP    = COMPOSITE_SRC_ATOP    - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_DST         = COMPOSITE_DST         - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_DST_OVER    = COMPOSITE_DST_OVER    - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_DST_IN      = COMPOSITE_DST_IN      - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_DST_OUT     = COMPOSITE_DST_OUT     - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_DST_ATOP    = COMPOSITE_DST_ATOP    - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_XOR         = COMPOSITE_XOR         - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_CLEAR       = COMPOSITE_CLEAR       - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_ADD         = COMPOSITE_ADD         - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_SUBTRACT    = COMPOSITE_SUBTRACT    - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_MULTIPLY    = COMPOSITE_MULTIPLY    - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_SCREEN      = COMPOSITE_SCREEN      - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_OVERLAY     = COMPOSITE_OVERLAY     - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_DARKEN      = COMPOSITE_DARKEN      - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_LIGHTEN     = COMPOSITE_LIGHTEN     - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_COLOR_DODGE = COMPOSITE_COLOR_DODGE - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_COLOR_BURN  = COMPOSITE_COLOR_BURN  - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_SOFT_LIGHT  = COMPOSITE_SOFT_LIGHT  - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_HARD_LIGHT  = COMPOSITE_HARD_LIGHT  - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_DIFFERENCE  = COMPOSITE_DIFFERENCE  - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_EXCLUSION   = COMPOSITE_EXCLUSION   - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_INVERT      = COMPOSITE_INVERT      - RENDER_COMPOSITE_EXT_START,
  RENDER_COMPOSITE_EXT_INVERT_RGB  = COMPOSITE_INVERT_RGB  - RENDER_COMPOSITE_EXT_START
};

// ============================================================================
// [RENDER_CONVERTER_BUFFER_SIZE]
// ============================================================================

enum { RENDER_CONVERTER_BUFFER_SIZE = 2048 };

// ============================================================================
// [RENDER_CONVERTER_COPY]
// ============================================================================

enum RENDER_CONVERTER_COPY
{
  RENDER_CONVERTER_COPY_8 = 0,
  RENDER_CONVERTER_COPY_16,
  RENDER_CONVERTER_COPY_24,
  RENDER_CONVERTER_COPY_32,
  RENDER_CONVERTER_COPY_48,
  RENDER_CONVERTER_COPY_64,

  RENDER_CONVERTER_COPY_COUNT
};

// ============================================================================
// [RENDER_CONVERTER_BSWAP]
// ============================================================================

enum RENDER_CONVERTER_BSWAP
{
  RENDER_CONVERTER_BSWAP_16 = 0,
  RENDER_CONVERTER_BSWAP_24,
  RENDER_CONVERTER_BSWAP_32,
  RENDER_CONVERTER_BSWAP_48,
  RENDER_CONVERTER_BSWAP_64,

  RENDER_CONVERTER_BSWAP_COUNT
};

// ============================================================================
// [RENDER_CONVERTER_DIB]
// ============================================================================

enum RENDER_CONVERTER_DIB
{
  // --------------------------------------------------------------------------
  // [Indexed]
  // --------------------------------------------------------------------------

  RENDER_CONVERTER_DIB_I8 = 0,

  // --------------------------------------------------------------------------
  // [RGB]
  // --------------------------------------------------------------------------

  RENDER_CONVERTER_DIB_RGB16_555,
  RENDER_CONVERTER_DIB_RGB16_555_BS,

  RENDER_CONVERTER_DIB_RGB16_565,
  RENDER_CONVERTER_DIB_RGB16_565_BS,

  RENDER_CONVERTER_DIB_RGB24_888,
  RENDER_CONVERTER_DIB_RGB24_888_BS,

  RENDER_CONVERTER_DIB_RGB32_888,
  RENDER_CONVERTER_DIB_RGB32_888_BS,

  RENDER_CONVERTER_DIB_RGB48_161616,
  RENDER_CONVERTER_DIB_RGB48_161616_BS,

  RENDER_CONVERTER_DIB_RGB48_DIB,
  RENDER_CONVERTER_DIB_RGB48_DIB_BS,

  // --------------------------------------------------------------------------
  // [ARGB/PRGB]
  // --------------------------------------------------------------------------

  RENDER_CONVERTER_DIB_ARGB16_4444,
  RENDER_CONVERTER_DIB_ARGB16_4444_BS,

  RENDER_CONVERTER_DIB_ARGB16_DIB,
  RENDER_CONVERTER_DIB_ARGB16_DIB_BS,

  RENDER_CONVERTER_DIB_ARGB24_DIB,
  RENDER_CONVERTER_DIB_ARGB24_DIB_BS,

  RENDER_CONVERTER_DIB_ARGB32_8888,
  RENDER_CONVERTER_DIB_ARGB32_8888_BS,

  RENDER_CONVERTER_DIB_ARGB32_DIB,
  RENDER_CONVERTER_DIB_ARGB32_DIB_BS,

  RENDER_CONVERTER_DIB_ARGB48_DIB,
  RENDER_CONVERTER_DIB_ARGB48_DIB_BS,

  RENDER_CONVERTER_DIB_ARGB64_16161616,
  RENDER_CONVERTER_DIB_ARGB64_16161616_BS,

  RENDER_CONVERTER_DIB_ARGB64_DIB,
  RENDER_CONVERTER_DIB_ARGB64_DIB_BS,

  // --------------------------------------------------------------------------
  // [Alpha]
  // --------------------------------------------------------------------------

  RENDER_CONVERTER_DIB_A8,

  RENDER_CONVERTER_DIB_A16,
  RENDER_CONVERTER_DIB_A16_BS,

  // --------------------------------------------------------------------------
  // [Grey]
  // --------------------------------------------------------------------------

  // RENDER_CONVERTER_DIB_GREY8,
  // RENDER_CONVERTER_DIB_GREY16,
  // RENDER_CONVERTER_DIB_GREY16_BS,
  // RENDER_CONVERTER_DIB_GREY32,
  // RENDER_CONVERTER_DIB_GREY32_BS,

  // --------------------------------------------------------------------------
  // [...]
  // --------------------------------------------------------------------------

  RENDER_CONVERTER_DIB_COUNT
};

// ============================================================================
// [RENDER_DITHER]
// ============================================================================

enum RENDER_DITHER
{
  RENDER_DITHER_RGB8_666 = 0,
  RENDER_DITHER_RGB8_484,
  RENDER_DITHER_RGB8_444,
  RENDER_DITHER_RGB8_242,
  RENDER_DITHER_RGB8_222,
  RENDER_DITHER_RGB8_111,

  RENDER_DITHER_RGB16_555,
  RENDER_DITHER_RGB16_565,

  RENDER_DITHER_COUNT
};

// ============================================================================
// [RENDER_FETCH]
// ============================================================================

//! @internal
//!
//! @brief Pattern fetch mode (Fog/G2d/Render).
enum RENDER_FETCH
{
  //! @brief Pattern fetcher can use data that owns source pattern (this is
  //! in most cases the image pixels owned by @c Image).
  //!
  //! This flag is the default and in some cases it is performance improvement,
  //! because the data don't need to be copied to the provided temporary buffer.
  //!
  //! If sharers are used then this mode is never used by pattern fetcher,
  //! because shaders need to overwrite the fetched pixels.
  RENDER_FETCH_REFERENCE = 0,

  //! @brief Pattern fetcher can use only provided buffer to fetch the data to.
  //!
  //! This mode is opposite to @c RENDER_FETCH_REFERENCE. It tells to
  //! fetcher to always use the provided buffer for all fetches. If shaders are
  //! used then these data will be overwritten so it's important to tell this
  //! fact to fetcher.
  //!
  //! This mode can be also used when using @c COMPOSITE_SRC and fetcher is not
  //! called using temporary buffer, but the target image buffer. This means
  //! that pattern fetcher will fetch data into the target buffer and no
  //! another copy is needed (optimization).
  RENDER_FETCH_COPY = 1
};

// ============================================================================
// [Fog::RENDER_COMBINE]
// ============================================================================

//! @internal
//!
//! @brief Compositing operator characteristics used by Fog::Render.
//!
//! The compositing operator characteristics are used internally by
//! the @c RasterPaintEngine and compositing templates in Fog::Render module.
enum RENDER_COMBINE
{
  //! @brief Compositing result is affected by destination color components.
  RENDER_COMBINE_DC = (1 << 0),
  //! @brief Compositing result is affected by destination alpha component.
  RENDER_COMBINE_DA = (1 << 1),
  //! @brief Compositing result is affected by source color components.
  RENDER_COMBINE_SC = (1 << 2),
  //! @brief Compositing result is affected by source alpha component.
  RENDER_COMBINE_SA = (1 << 3),

  //! @brief Operator is not bound.
  //!
  //! Bound operators means that it's possible to multiply pixel by weight value
  //! and compositing operation will be still valid (this is related to
  //! compositing using external mask).
  //!
  //! Typical bounded operator is @c COMPOSITE_SRC_OVER, but for example
  //! @c COMPOSITE_SRC is unbounded.
  RENDER_COMBINE_UNBOUND = (1 << 4),
  //! @brief Operator is not bound, but in formula is always Sca.m or Sa.m.
  //!
  //! This flag is used only as optimization and it must be set together with
  //! @c RENDER_COMBINE_UNBOUND.
  RENDER_COMBINE_UNBOUND_MSK_IN = (1 << 5),

  //! @brief Operator is always nop (@c COMPOSITE_DST).
  RENDER_COMBINE_NOP = (1 << 6),
  //! @brief Operator is nop if destination alpha is zero.
  RENDER_COMBINE_NOP_IF_DA_ZERO = (1 << 7),
  //! @brief Operator is nop if destination alpha is fully opaque.
  RENDER_COMBINE_NOP_IF_DA_FULL = (1 << 8),
  //! @brief Operator is nop if source alpha value is zero.
  RENDER_COMBINE_NOP_IF_SA_ZERO = (1 << 9),
  //! @brief Operator is nop if source alpha value is fully opaque.
  RENDER_COMBINE_NOP_IF_SA_FULL = (1 << 10),

  //! @brief Operator can be done on packed data (this is hint for mmx/sse2
  //! templates).
  RENDER_COMBINE_PACKED = (1 << 11),

  //! @brief Prefer color in 0xFFRRGGBB format instead of 0xXXRRGGBB. Used as
  //! an optimization hint for MMX/SSE2 code.
  RENDER_COMBINE_PREFER_FRGB = (1 << 12)
};

// ============================================================================
// [Fog::Render - Data - CombineProperties]
// ============================================================================

// ============================================================================
// [RENDER_COMBINE_STATIC]
// ============================================================================

// Skip documenting this section.
#if !defined(FOG_DOXYGEN)

// Shorter names are better for our table.
#define DC             RENDER_COMBINE_DC
#define DA             RENDER_COMBINE_DA
#define SC             RENDER_COMBINE_SC
#define SA             RENDER_COMBINE_SA
#define UNBOUND        RENDER_COMBINE_UNBOUND
#define UNBOUND_MSK_IN RENDER_COMBINE_UNBOUND | RENDER_COMBINE_UNBOUND_MSK_IN
#define NOP            RENDER_COMBINE_NOP
#define NOP_DA_Z       RENDER_COMBINE_NOP_IF_DA_ZERO
#define NOP_DA_F       RENDER_COMBINE_NOP_IF_DA_FULL
#define NOP_SA_Z       RENDER_COMBINE_NOP_IF_SA_ZERO
#define NOP_SA_F       RENDER_COMBINE_NOP_IF_SA_FULL
#define HINT_PACKED    RENDER_COMBINE_PACKED
#define HINT_FRGB      RENDER_COMBINE_PREFER_FRGB

#endif // FOG_DOXYGEN

//! @internal
//!
//! @brief Render compositing operators characteristics.
enum RENDER_COMBINE_STATIC
{
  RENDER_COMBINE_OP_SRC        = 0  | 0  | SC | SA | UNBOUND_MSK_IN | 0        | HINT_PACKED,
  RENDER_COMBINE_OP_SRC_OVER   = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,
  RENDER_COMBINE_OP_SRC_IN     = 0  | DA | SC | SA | UNBOUND_MSK_IN | NOP_DA_Z | 0,
  RENDER_COMBINE_OP_SRC_OUT    = 0  | DA | SC | SA | UNBOUND_MSK_IN | 0        | 0,
  RENDER_COMBINE_OP_SRC_ATOP   = DC | DA | SC | SA | 0              | NOP_SA_Z | HINT_FRGB,
  RENDER_COMBINE_OP_DST        = DC | DA | 0  | 0  | UNBOUND        | NOP      | HINT_PACKED,
  RENDER_COMBINE_OP_DST_OVER   = DC | DA | SC | SA | 0              | NOP_DA_F | 0,
  RENDER_COMBINE_OP_DST_IN     = DC | DA | 0  | SA | UNBOUND_MSK_IN | NOP_SA_F | 0,
  RENDER_COMBINE_OP_DST_OUT    = DC | DA | 0  | SA | 0              | NOP_SA_Z | 0,
  RENDER_COMBINE_OP_DST_ATOP   = DC | DA | SC | SA | UNBOUND_MSK_IN | 0        | 0,
  RENDER_COMBINE_OP_XOR        = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,
  RENDER_COMBINE_OP_CLEAR      = 0  | 0  | 0  | 0  | UNBOUND        | 0        | 0,

  RENDER_COMBINE_OP_ADD        = DC | DA | SC | SA | 0              | NOP_SA_Z | HINT_PACKED,
  RENDER_COMBINE_OP_SUBTRACT   = DC | DA | SC | SA | 0              | NOP_SA_Z | HINT_PACKED,
  RENDER_COMBINE_OP_MULTIPLY   = DC | DA | SC | SA | UNBOUND        | NOP_SA_Z | 0,
  RENDER_COMBINE_OP_SCREEN     = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,

  // TODO: COMPOSITING OPERATOR FORMULAS, VERIFICATION.
  RENDER_COMBINE_OP_OVERLAY    = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,

  RENDER_COMBINE_OP_DARKEN     = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,
  RENDER_COMBINE_OP_LIGHTEN    = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,

  // TODO: COMPOSITING OPERATOR FORMULAS, VERIFICATION.
  RENDER_COMBINE_OP_COLOR_DODGE= DC | DA | SC | SA | 0              | NOP_SA_Z | 0,
  RENDER_COMBINE_OP_COLOR_BURN = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,
  RENDER_COMBINE_OP_SOFT_LIGHT = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,
  RENDER_COMBINE_OP_HARD_LIGHT = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,

  RENDER_COMBINE_OP_DIFFERENCE = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,
  RENDER_COMBINE_OP_EXCLUSION  = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,
  RENDER_COMBINE_OP_INVERT     = DC | DA | 0  | SA | 0              | NOP_SA_Z | 0,
  RENDER_COMBINE_OP_INVERT_RGB = DC | DA | SC | SA | 0              | NOP_SA_Z | 0
};

// Cleanup
#undef DC
#undef DA
#undef SC
#undef SA
#undef UNBOUND
#undef UNBOUND_MSK_IN
#undef NOP
#undef NOP_DA_Z
#undef NOP_DA_F
#undef NOP_SA_Z
#undef NOP_SA_F
#undef OP_PACKED

//! @internal
//!
//! @brief Operator characteristics that can be used by raster paint engine
//! dynamically (based on compositing operator).
extern FOG_API uint32_t _render_combineProperties[COMPOSITE_COUNT];

// ============================================================================
// [Fog::Render - Data - LinearBlur8]
// ============================================================================

//! @internal
extern FOG_NO_EXPORT const uint16_t _render_linear_blur8_mul[255];

//! @internal
extern FOG_NO_EXPORT const uint8_t _render_linear_blur8_shr[255];

// ============================================================================
// [Fog::Render - Data - CompatibleFormat]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
struct RenderCompatibleFormat
{
  uint8_t dstFormat;
  uint8_t srcFormat;
  uint8_t vblitId;
  uint8_t reserved;
};
#include <Fog/Core/Pack/PackRestore.h>

//! @internal
extern FOG_NO_EXPORT const RenderCompatibleFormat _g2d_render_compatibleFormat[IMAGE_FORMAT_COUNT][IMAGE_FORMAT_COUNT];

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_RENDER_RENDERCONSTANTS_H
