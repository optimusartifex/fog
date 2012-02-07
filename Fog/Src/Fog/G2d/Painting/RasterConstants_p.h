// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERCONSTANTS_P_H
#define _FOG_G2D_PAINTING_RASTERCONSTANTS_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Fog::Raster - Debug]
// ============================================================================

// #define FOG_DEBUG_RASTER_CMD
// #define FOG_DEBUG_RASTER_SYNC
// #define FOG_DEBUG_RASTERIZER

// ============================================================================
// [Fog::Raster - RASTER_CBLIT]
// ============================================================================

enum RASTER_CBLIT
{
  RASTER_CBLIT_PRGB = 0,
  RASTER_CBLIT_XRGB = 1,
  RASTER_CBLIT_COUNT = 2
};

// ============================================================================
// [Fog::Raster - RASTER_VBLIT]
// ============================================================================

//! @brief Compatibility format IDs used by the composite-ext operators.
enum RASTER_VBLIT
{
  // --------------------------------------------------------------------------
  // [PRGB32]
  // --------------------------------------------------------------------------

  RASTER_VBLIT_PRGB32_AND_PRGB32 = 0,
  RASTER_VBLIT_PRGB32_AND_XRGB32 = 1,
  RASTER_VBLIT_PRGB32_AND_RGB24 = 2,
  RASTER_VBLIT_PRGB32_AND_A8 = 3,

  // --------------------------------------------------------------------------
  // [XRGB32]
  // --------------------------------------------------------------------------

  RASTER_VBLIT_XRGB32_AND_PRGB32 = 0,
  RASTER_VBLIT_XRGB32_AND_XRGB32 = 1,
  RASTER_VBLIT_XRGB32_AND_RGB24 = 2,

  // --------------------------------------------------------------------------
  // [RGB24]
  // --------------------------------------------------------------------------

  RASTER_VBLIT_RGB24_AND_PRGB32 = 0,
  RASTER_VBLIT_RGB24_AND_XRGB32 = 1,
  RASTER_VBLIT_RGB24_AND_RGB24 = 2,

  // --------------------------------------------------------------------------
  // [A8]
  // --------------------------------------------------------------------------

  RASTER_VBLIT_A8_AND_PRGB32 = 0,
  RASTER_VBLIT_A8_AND_A8 = 1,

  // --------------------------------------------------------------------------
  // [PRGB64]
  // --------------------------------------------------------------------------

  RASTER_VBLIT_PRGB64_AND_PRGB64 = 0,
  RASTER_VBLIT_PRGB64_AND_RGB48 = 1,
  RASTER_VBLIT_PRGB64_AND_PRGB32 = 2,

  // --------------------------------------------------------------------------
  // [RGB48]
  // --------------------------------------------------------------------------

  RASTER_VBLIT_RGB48_AND_PRGB64 = 0,
  RASTER_VBLIT_RGB48_AND_RGB48 = 1,
  RASTER_VBLIT_RGB48_AND_PRGB32 = 2,

  // --------------------------------------------------------------------------
  // [A16]
  // --------------------------------------------------------------------------

  RASTER_VBLIT_A16_AND_PRGB64 = 0,
  RASTER_VBLIT_A16_AND_A16 = 1,
  RASTER_VBLIT_A16_AND_A8 = 2,

  // --------------------------------------------------------------------------
  // [...]
  // --------------------------------------------------------------------------

  //! @brief Count of VBlit formats.
  RASTER_VBLIT_COUNT = 4,

  //! @brief Invalid VBlit format.
  RASTER_VBLIT_INVALID = RASTER_VBLIT_COUNT
};

// ============================================================================
// [Fog::Raster - RASTER_COMPOSITE]
// ============================================================================

enum RASTER_COMPOSITE
{
  // --------------------------------------------------------------------------
  // [CompositeCore]
  // --------------------------------------------------------------------------

  RASTER_COMPOSITE_CORE_START      = 0,
  RASTER_COMPOSITE_CORE_COUNT      = 2,

  RASTER_COMPOSITE_CORE_SRC        = COMPOSITE_SRC         - RASTER_COMPOSITE_CORE_START,
  RASTER_COMPOSITE_CORE_SRC_OVER   = COMPOSITE_SRC_OVER    - RASTER_COMPOSITE_CORE_START,

  // --------------------------------------------------------------------------
  // [CompositeExt]
  // --------------------------------------------------------------------------

  RASTER_COMPOSITE_EXT_START       = RASTER_COMPOSITE_CORE_COUNT,
  RASTER_COMPOSITE_EXT_COUNT       = COMPOSITE_COUNT       - RASTER_COMPOSITE_EXT_START,

  RASTER_COMPOSITE_EXT_SRC_IN      = COMPOSITE_SRC_IN      - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_SRC_OUT     = COMPOSITE_SRC_OUT     - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_SRC_ATOP    = COMPOSITE_SRC_ATOP    - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_DST         = COMPOSITE_DST         - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_DST_OVER    = COMPOSITE_DST_OVER    - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_DST_IN      = COMPOSITE_DST_IN      - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_DST_OUT     = COMPOSITE_DST_OUT     - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_DST_ATOP    = COMPOSITE_DST_ATOP    - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_XOR         = COMPOSITE_XOR         - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_CLEAR       = COMPOSITE_CLEAR       - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_ADD         = COMPOSITE_ADD         - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_SUBTRACT    = COMPOSITE_SUBTRACT    - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_MULTIPLY    = COMPOSITE_MULTIPLY    - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_SCREEN      = COMPOSITE_SCREEN      - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_OVERLAY     = COMPOSITE_OVERLAY     - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_DARKEN      = COMPOSITE_DARKEN      - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_LIGHTEN     = COMPOSITE_LIGHTEN     - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_COLOR_DODGE = COMPOSITE_COLOR_DODGE - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_COLOR_BURN  = COMPOSITE_COLOR_BURN  - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_SOFT_LIGHT  = COMPOSITE_SOFT_LIGHT  - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_HARD_LIGHT  = COMPOSITE_HARD_LIGHT  - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_DIFFERENCE  = COMPOSITE_DIFFERENCE  - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_EXCLUSION   = COMPOSITE_EXCLUSION   - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_INVERT      = COMPOSITE_INVERT      - RASTER_COMPOSITE_EXT_START,
  RASTER_COMPOSITE_EXT_INVERT_RGB  = COMPOSITE_INVERT_RGB  - RASTER_COMPOSITE_EXT_START
};

// ============================================================================
// [RASTER_CONVERT_BUFFER_SIZE]
// ============================================================================

enum
{
  //! @brief Internal buffer-size used for multi-pass image conversion.
  RASTER_CONVERT_BUFFER_SIZE = 2048
};

// ============================================================================
// [RASTER_COPY]
// ============================================================================

enum RASTER_COPY
{
  RASTER_COPY_8 = 0,
  RASTER_COPY_16,
  RASTER_COPY_24,
  RASTER_COPY_32,
  RASTER_COPY_48,
  RASTER_COPY_64,

  RASTER_COPY_COUNT
};

// ============================================================================
// [RASTER_BSWAP]
// ============================================================================

enum RASTER_BSWAP
{
  RASTER_BSWAP_16 = 0,
  RASTER_BSWAP_24,
  RASTER_BSWAP_32,
  RASTER_BSWAP_48,
  RASTER_BSWAP_64,

  RASTER_BSWAP_COUNT
};

// ============================================================================
// [RASTER_FILL]
// ============================================================================

enum RASTER_FILL
{
  RASTER_FILL_8 = 0,
  RASTER_FILL_16,

  RASTER_FILL_COUNT
};

enum RASTER_FORMAT
{
  RASTER_FORMAT_RGB16_555 = 0,
  RASTER_FORMAT_RGB16_555_BS,

  RASTER_FORMAT_RGB16_565,
  RASTER_FORMAT_RGB16_565_BS,

  RASTER_FORMAT_ARGB16_4444,
  RASTER_FORMAT_ARGB16_4444_BS,

  RASTER_FORMAT_ARGB16_CUSTOM,
  RASTER_FORMAT_ARGB16_CUSTOM_BS,

  RASTER_FORMAT_RGB24_888,
  RASTER_FORMAT_RGB24_888_BS,

  RASTER_FORMAT_ARGB24_CUSTOM,
  RASTER_FORMAT_ARGB24_CUSTOM_BS,

  RASTER_FORMAT_RGB32_888,
  RASTER_FORMAT_RGB32_888_BS,

  RASTER_FORMAT_ARGB32_8888,
  RASTER_FORMAT_ARGB32_8888_BS,

  RASTER_FORMAT_ARGB32_CUSTOM,
  RASTER_FORMAT_ARGB32_CUSTOM_BS,

  RASTER_FORMAT_ARGB48_CUSTOM,
  RASTER_FORMAT_ARGB48_CUSTOM_BS,

  RASTER_FORMAT_RGB48_161616,
  RASTER_FORMAT_RGB48_161616_BS,

  RASTER_FORMAT_RGB48_CUSTOM,
  RASTER_FORMAT_RGB48_CUSTOM_BS,

  RASTER_FORMAT_ARGB64_16161616,
  RASTER_FORMAT_ARGB64_16161616_BS,

  RASTER_FORMAT_ARGB64_CUSTOM,
  RASTER_FORMAT_ARGB64_CUSTOM_BS,

  RASTER_FORMAT_A8,

  RASTER_FORMAT_A16,
  RASTER_FORMAT_A16_BS,

  // --------------------------------------------------------------------------
  // [Grey]
  // --------------------------------------------------------------------------

  // RASTER_FORMAT_GREY8,
  // RASTER_FORMAT_GREY16,
  // RASTER_FORMAT_GREY16_BS,

  // --------------------------------------------------------------------------
  // [Indexed]
  // --------------------------------------------------------------------------

  RASTER_FORMAT_I8,

  // --------------------------------------------------------------------------
  // [...]
  // --------------------------------------------------------------------------

  RASTER_FORMAT_COUNT,
  RASTER_FORMAT_INVALID = RASTER_FORMAT_COUNT
};

// ============================================================================
// [RASTER_DITHER]
// ============================================================================

enum RASTER_DITHER
{
  RASTER_DITHER_RGB8_666 = 0,
  RASTER_DITHER_RGB8_484,
  RASTER_DITHER_RGB8_444,
  RASTER_DITHER_RGB8_242,
  RASTER_DITHER_RGB8_222,
  RASTER_DITHER_RGB8_111,

  RASTER_DITHER_RGB16_555,
  RASTER_DITHER_RGB16_565,

  RASTER_DITHER_COUNT
};

// ============================================================================
// [RASTER_FETCH]
// ============================================================================

//! @internal
//!
//! @brief Pattern fetch mode.
enum RASTER_FETCH
{
  //! @brief Raster fetcher can use data that owns source pattern (this is
  //! in most cases the image pixels owned by @c Image).
  //!
  //! This flag is the default and in some cases it is an performance
  //! improvement, because the data don't need to be copied to the temporary
  //! buffer.
  //!
  //! @note RASTER_FETCH_REFERENCE is never used in conjunction with shaders.
  RASTER_FETCH_REFERENCE = 0,

  //! @brief Raster fetcher must copy the data to the provided buffer.
  //!
  //! This mode is opposite to the @c RASTER_FETCH_REFERENCE. All pixels will
  //! be written to the provided buffer.
  //!
  //! This mode can be also used when using @c COMPOSITE_SRC operator without
  //! any mask to fetch pattern directly to the target raster.
  RASTER_FETCH_COPY = 1
};

// ============================================================================
// [Fog::RASTER_PRGB_PREPARE]
// ============================================================================

//! @brief Prepare steps done when compositing pixels.
enum RASTER_PRGB_PREPARE
{
  //! @brief There is nothing in prepare step.
  RASTER_PRGB_PREPARE_NONE = 0,

  //! @brief Prepare means to fill X byte or word to get PRGB32/PRGB64
  //! (FRGB32/FRGB64) result.
  RASTER_PRGB_PREPARE_FRGB = 1,

  //! @brief Prepare means to clear X byte or word to get ZRGB32/ZRGB64 result.
  RASTER_PRGB_PREPARE_ZRGB = 2
};

// ============================================================================
// [Fog::RASTER_COMBINE]
// ============================================================================

//! @internal
//!
//! @brief Compositing operator characteristics used by Fog::Raster.
//!
//! The compositing operator characteristics are used internally by
//! the @c RasterPaintEngine and compositing templates in Fog::Raster module.
enum RASTER_COMBINE
{
  //! @brief Compositing result is affected by dst-color component(s).
  RASTER_COMBINE_DC = 0x0001,
  //! @brief Compositing result is affected by dst-alpha component.
  RASTER_COMBINE_DA = 0x0002,
  //! @brief Compositing result is affected by src-color component(s).
  RASTER_COMBINE_SC = 0x0004,
  //! @brief Compositing result is affected by src-alpha component.
  RASTER_COMBINE_SA = 0x0008,

  //! @brief Operator is not bound.
  //!
  //! Bound operators means that it's possible to multiply pixel by weight value
  //! and compositing operation will be still valid (this is related to
  //! compositing using external mask).
  //!
  //! Typical bounded operator is @c COMPOSITE_SRC_OVER, but for example
  //! @c COMPOSITE_SRC is unbounded.
  RASTER_COMBINE_UNBOUND = 0x0010,
  //! @brief Operator is not bound, but in formula is always Sca.m or Sa.m.
  //!
  //! This flag is used only as optimization and it must be set together with
  //! @c RASTER_COMBINE_UNBOUND.
  RASTER_COMBINE_UNBOUND_MSK_IN = 0x0020,

  //! @brief Operator is always nop (@c COMPOSITE_DST).
  RASTER_COMBINE_NOP = 0x0040,
  //! @brief Operator is nop if destination alpha is zero.
  RASTER_COMBINE_NOP_IF_DA_ZERO = 0x0080,
  //! @brief Operator is nop if destination alpha is fully opaque.
  RASTER_COMBINE_NOP_IF_DA_FULL = 0x0100,
  //! @brief Operator is nop if source alpha value is zero.
  RASTER_COMBINE_NOP_IF_SA_ZERO = 0x0200,
  //! @brief Operator is nop if source alpha value is fully opaque.
  RASTER_COMBINE_NOP_IF_SA_FULL = 0x0400,

  //! @brief Operator can be done on packed data (this is hint for mmx/sse2
  //! templates).
  RASTER_COMBINE_PREFER_PACKED = 0x1000,

  //! @brief Prefer color in 0xFFRRGGBB format instead of 0xXXRRGGBB. Used as
  //! an optimization hint for MMX/SSE2 code.
  RASTER_COMBINE_PREFER_FRGB = 0x2000
};

// ============================================================================
// [Fog::Raster - Data - CombineProperties]
// ============================================================================

// ============================================================================
// [RASTER_COMBINE_STATIC]
// ============================================================================

// Skip documenting this section.
#if !defined(FOG_DOXYGEN)

// Shorter names are better for our table.
#define DC             RASTER_COMBINE_DC
#define DA             RASTER_COMBINE_DA
#define SC             RASTER_COMBINE_SC
#define SA             RASTER_COMBINE_SA
#define UNBOUND        RASTER_COMBINE_UNBOUND
#define UNBOUND_MSK_IN RASTER_COMBINE_UNBOUND | RASTER_COMBINE_UNBOUND_MSK_IN
#define NOP            RASTER_COMBINE_NOP
#define NOP_DA_Z       RASTER_COMBINE_NOP_IF_DA_ZERO
#define NOP_DA_F       RASTER_COMBINE_NOP_IF_DA_FULL
#define NOP_SA_Z       RASTER_COMBINE_NOP_IF_SA_ZERO
#define NOP_SA_F       RASTER_COMBINE_NOP_IF_SA_FULL
#define HINT_PACKED    RASTER_COMBINE_PREFER_PACKED
#define HINT_FRGB      RASTER_COMBINE_PREFER_FRGB

#endif // FOG_DOXYGEN

//! @internal
//!
//! @brief Render compositing operators characteristics.
enum RASTER_COMBINE_STATIC
{
  RASTER_COMBINE_OP_SRC        = 0  | 0  | SC | SA | UNBOUND_MSK_IN | 0        | HINT_PACKED,
  RASTER_COMBINE_OP_SRC_OVER   = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,
  RASTER_COMBINE_OP_SRC_IN     = 0  | DA | SC | SA | UNBOUND_MSK_IN | NOP_DA_Z | 0,
  RASTER_COMBINE_OP_SRC_OUT    = 0  | DA | SC | SA | UNBOUND_MSK_IN | 0        | 0,
  RASTER_COMBINE_OP_SRC_ATOP   = DC | DA | SC | SA | 0              | NOP_SA_Z | HINT_FRGB,
  RASTER_COMBINE_OP_DST        = DC | DA | 0  | 0  | UNBOUND        | NOP      | HINT_PACKED,
  RASTER_COMBINE_OP_DST_OVER   = DC | DA | SC | SA | 0              | NOP_DA_F | 0,
  RASTER_COMBINE_OP_DST_IN     = DC | DA | 0  | SA | UNBOUND_MSK_IN | NOP_SA_F | 0,
  RASTER_COMBINE_OP_DST_OUT    = DC | DA | 0  | SA | 0              | NOP_SA_Z | 0,
  RASTER_COMBINE_OP_DST_ATOP   = DC | DA | SC | SA | UNBOUND_MSK_IN | 0        | 0,
  RASTER_COMBINE_OP_XOR        = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,
  RASTER_COMBINE_OP_CLEAR      = 0  | 0  | 0  | 0  | UNBOUND        | 0        | 0,

  RASTER_COMBINE_OP_ADD        = DC | DA | SC | SA | 0              | NOP_SA_Z | HINT_PACKED,
  RASTER_COMBINE_OP_SUBTRACT   = DC | DA | SC | SA | 0              | NOP_SA_Z | HINT_PACKED,
  RASTER_COMBINE_OP_MULTIPLY   = DC | DA | SC | SA | UNBOUND        | NOP_SA_Z | 0,
  RASTER_COMBINE_OP_SCREEN     = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,

  // TODO: COMPOSITING OPERATOR FORMULAS, VERIFICATION.
  RASTER_COMBINE_OP_OVERLAY    = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,

  RASTER_COMBINE_OP_DARKEN     = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,
  RASTER_COMBINE_OP_LIGHTEN    = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,

  // TODO: COMPOSITING OPERATOR FORMULAS, VERIFICATION.
  RASTER_COMBINE_OP_COLOR_DODGE= DC | DA | SC | SA | 0              | NOP_SA_Z | 0,
  RASTER_COMBINE_OP_COLOR_BURN = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,
  RASTER_COMBINE_OP_SOFT_LIGHT = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,
  RASTER_COMBINE_OP_HARD_LIGHT = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,

  RASTER_COMBINE_OP_DIFFERENCE = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,
  RASTER_COMBINE_OP_EXCLUSION  = DC | DA | SC | SA | 0              | NOP_SA_Z | 0,
  RASTER_COMBINE_OP_INVERT     = DC | DA | 0  | SA | 0              | NOP_SA_Z | 0,
  RASTER_COMBINE_OP_INVERT_RGB = DC | DA | SC | SA | 0              | NOP_SA_Z | 0
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
extern FOG_API uint32_t _raster_combineProperties[COMPOSITE_COUNT];

// ============================================================================
// [Fog::Raster - Data - LinearBlur8]
// ============================================================================

//! @internal
extern FOG_NO_EXPORT const uint16_t _raster_blur_stack_8_mul[256];

//! @internal
extern FOG_NO_EXPORT const uint8_t _raster_blur_stack_8_shr[256];

// ============================================================================
// [Fog::Raster - Data - CompatibleFormat]
// ============================================================================

#include <Fog/Core/C++/PackByte.h>
struct RasterCompatibleFormat
{
  uint8_t dstFormat;
  uint8_t srcFormat;
  uint8_t vblitId;
  uint8_t reserved;
};
#include <Fog/Core/C++/PackRestore.h>

//! @internal
extern FOG_NO_EXPORT const RasterCompatibleFormat _raster_compatibleFormat[IMAGE_FORMAT_COUNT][IMAGE_FORMAT_COUNT];

// ============================================================================
// [Fog::Raster - Data - ModifiedOperator]
// ============================================================================

extern FOG_NO_EXPORT const uint8_t _raster_modifiedOperator[IMAGE_FORMAT_COUNT][COMPOSITE_COUNT][2];

// ============================================================================
// [Fog::RASTER_CORE]
// ============================================================================

enum RASTER_CORE
{
  // --------------------------------------------------------------------------
  // [Multithreading]
  // --------------------------------------------------------------------------

  // Minimum size to set multithreading on.
  RASTER_MIN_SIZE_THRESHOLD = 128 * 128,

  // Maximum number of threads which may be used by the raster engine.
  RASTER_MAX_THREADS_USED = 64,
  // Maximum number of threads which may be suggested for rendering by the
  // raster painter engine.
  RASTER_MAX_THREADS_SUGGESTED = 16,

  // Maximum commands to accumulate in the buffer.
#if defined(FOG_DEBUG_RASTER_SYNC)
  // For debugging it's better to have only minimal buffer size for the commands
  // and calculations.
  RASTER_MAX_COMMANDS = 32,
#else
  RASTER_MAX_COMMANDS = 1024,
#endif // FOG_DEBUG_RASTER_SYNC

  // Maximum calculations to accumulate in the buffer, at this time it must be
  // the same as the maximum count of commands. The reason is that each command
  // can use exactly one calculation so if you need serialize RASTER_MAX_COMMANDS
  // you need also RASTER_MAX_COMMANDS possible calculations.
  RASTER_MAX_CALCULATIONS = RASTER_MAX_COMMANDS,
};

// ============================================================================
// [Fog::RASTER_CALC_OPCODE]
// ============================================================================

enum RASTER_CALC_OPCODE
{
  RASTER_CALC_OPCODE_NULL = 0,

  RASTER_CALC_OPCODE_COUNT
};

// ============================================================================
// [Fog::RASTER_CALC_STATUS]
// ============================================================================

//! @internal
//!
//! @brief The status of raster calculation action (@c RasterCalc class).
enum RASTER_CALC_STATUS
{
  //! @brief The calculation is not finished yet, it might be processed at
  //! the moment.
  RASTER_CALC_STATUS_WAIT = 0,
  //! @brief The calculation is done and ready to use.
  RASTER_CALC_STATUS_DONE = 1,
  //! @brief The calculation evaluation produced no result, depending command
  //! should be skipped.
  RASTER_CALC_STATUS_SKIP = 2
};

// ============================================================================
// [Fog::RASTER_CMD_OPCODE]
// ============================================================================

enum RASTER_CMD_OPCODE
{
  RASTER_CMD_OPCODE_NULL = 0,
  RASTER_CMD_OPCODE_FILL_RECT,
  RASTER_CMD_OPCODE_BLIT_IMAGE,

  RASTER_CMD_OPCODE_COUNT
};

// ============================================================================
// [Fog::RASTER_INTEGRAL_TRANSFORM]
// ============================================================================

//! @internal
enum RASTER_INTEGRAL_TRANSFORM
{
  //! @brief Transform is not integral.
  RASTER_INTEGRAL_TRANSFORM_NONE = 0,

  //! @brief Transform is simple (identity or translation).
  RASTER_INTEGRAL_TRANSFORM_SIMPLE = 1,
  //! @brief Transform is scaling (translation + scaling).
  RASTER_INTEGRAL_TRANSFORM_SCALING = 2,
  //! @brief Transform is swap (translation + swap).
  RASTER_INTEGRAL_TRANSFORM_SWAP = 3
};

// ============================================================================
// [Fog::RASTER_STATE]
// ============================================================================

//! @internal
//!
//! @brief Painter state flags used by @c RasterPaintEngine::save() and
//! @c RasterPaintEngine::restore().
enum RASTER_STATE
{
  //! @brief Source.
  RASTER_STATE_SOURCE = 0x00000001,
  //! @brief Stroke parameters.
  RASTER_STATE_STROKE = 0x00000002,

  //! @brief The painter transformation matrix.
  RASTER_STATE_TRANSFORM = 0x00000004,
  //! @brief The painter clipping (clip region or mask).
  RASTER_STATE_CLIPPING = 0x00000008,

  //! @brief The painter filter.
  RASTER_STATE_FILTER = 0x00000010,

  //! @brief All states, used internally.
  RASTER_STATE_ALL =
    RASTER_STATE_SOURCE    |
    RASTER_STATE_STROKE    |
    RASTER_STATE_TRANSFORM |
    RASTER_STATE_CLIPPING  |
    RASTER_STATE_FILTER
};

// ============================================================================
// [Fog::RASTER_CLIP]
// ============================================================================

//! @internal
//!
//! @brief Raster clip mode.
enum RASTER_CLIP
{
  //! @brief The clip is a rectangle (@ref BoxI) or painting is completely
  //! disabled.
  RASTER_CLIP_BOX = 0,

  //! @brief The clip is a region (YX sorted array of @ref BoxI instances).
  RASTER_CLIP_REGION = 1,

  //! @brief The clip is a mask (internally created by @c RasterSpan instances).
  RASTER_CLIP_MASK = 2,

  //! @brief Count of clip modes.
  RASTER_CLIP_COUNT = 3
};

// ============================================================================
// [Fog::RASTER_CONTEXT_FLAGS]
// ============================================================================

//! @internal
//!
//! @brief Raster context flags (used by master context).
//!
//! NO_PAINT - flags that will be set if some internal state disables
//! painting, see flags and their meanings for details.
//!
//! DIRTY - Some members are dirty and need to be updated.
//!
//! PENDING - flags used to send specific commands to workers. All flags
//! are grouped together and when needed the command is sent. Commands
//! are generally not generated when it's not needed. For example if you save
//! clip state using save() and then restore() it and there weren't made
//! changes, then save and restore commands are not send to workers, because
//! they are not needed.
enum RASTER_MASTER_FLAGS
{
  // --------------------------------------------------------------------------
  // [No-Paint Flags]
  //
  // If any of the no-paint flags is set then painting command will be terminated
  // before the parameters are evaluated. It's one of fastest checking available
  // if raster painter engine.
  // --------------------------------------------------------------------------

  //! @brief Mask including all no-paint states and fatal-error.
  RASTER_NO_PAINT_ALL_FLAGS = 0x8000FFFF,

  //! @brief The meta clip-region doens't contain painting area.
  RASTER_NO_PAINT_META_REGION = 0x00000001,

  //! @brief The meta transform is invalid.
  RASTER_NO_PAINT_META_TRANSFORM = 0x00000002,

  //! @brief The user clip rect/region doesn't contain painting area.
  RASTER_NO_PAINT_USER_CLIP = 0x00000004,

  //! @brief The user clip mask doesn't contain painting area.
  RASTER_NO_PAINT_USER_MASK = 0x00000010,

  //! @brief The user transform is invalid.
  RASTER_NO_PAINT_USER_TRANSFORM = 0x00000008,

  //! @brief The compositing operator produces no painting onto the current layer.
  RASTER_NO_PAINT_COMPOSITING_OPERATOR = 0x00000020,

  //! @brief The source color/pattern is invalid.
  RASTER_NO_PAINT_SOURCE = 0x00000040,
  //! @brief The global opacity is zero or very close to zero (zero when
  //! converted to integer).
  RASTER_NO_PAINT_OPACITY = 0x00000080,

  //! @brief Some of the stroke parameters contains degenerated value (for
  //! example line-width set to zero), so, using stroke is no-paint.
  RASTER_NO_PAINT_STROKE = 0x00001000,

  RASTER_NO_PAINT_BASE_FLAGS   = RASTER_NO_PAINT_META_REGION          |
                                 RASTER_NO_PAINT_META_TRANSFORM       |
                                 RASTER_NO_PAINT_USER_CLIP            |
                                 RASTER_NO_PAINT_USER_MASK            |
                                 RASTER_NO_PAINT_USER_TRANSFORM       |
                                 RASTER_NO_PAINT_COMPOSITING_OPERATOR |
                                 RASTER_NO_PAINT_OPACITY              ,

  //! @brief Painting is disabled, because something bad happened, for example
  //! memory allocation error for core objects. This error can be cleared only
  //! by @c Painter::switchTo(), @c endLayer() methods.
  //!
  //! This is fatal error that can disable all painting until the paint engine
  //! is destroyed or reused.
  RASTER_NO_PAINT_FATAL = 0x80000000,

#if 0
  // --------------------------------------------------------------------------
  // [Pending Flags - TOP-TO-BOTTOM Order]
  //
  // The pending flags are evaluated from top-to-bottom order. The evaluation
  // of pending flags can result in no-paint flags so it's needed to check them
  // again after processed.
  // --------------------------------------------------------------------------

  //! @brief Mask that contains all pending states.
  RASTER_PENDING_ALL_FLAGS = 0x7FF00000,

  //! @brief Global transformation matrix was changed, it's needed to
  //! recalculate other members related to transformations before a final
  //! transformation matrix may be used.
  RASTER_PENDING_GLOBAL_TRANSFORM = 0x00100000,

  RASTER_PENDING_CLIP_RECT = 0x00200000,
  RASTER_PENDING_CLIP_REGION = 0x00400000,
  RASTER_PENDING_CLIP_STACK = 0x00800000

  // TODO: Finish

  //! @brief The mask has been marked for saving.
  //!
  //! This flag is used in situation that user called save(RASTER_STATE_CLIP)
  //RASTER_PENDING_MASK_SAVE = 0,
  //RASTER_PENDING_MASK_RESTORE = 0,

  // TODO: Remove...

  //! @brief Clipping was changed so the first paint call (that is likely to
  //! paint something) must ensure to call methods that will send the changes
  //! to the workers (making command). If not running in multithreaded mode
  //! then this flag shouldn't be set.
  RASTER_CONTEXT_PENDING_CLIP_REGION = 0x00010000,

  //! @brief Clip state need to be saved (send the command to the workers).
  RASTER_CONTEXT_PENDING_CLIP_SAVE = 0x00020000,

  //! @brief Clip mask initialization is pending.
  RASTER_CONTEXT_PENDING_CLIP_MASK_CREATED = 0x00040000,

  //! @brief Clip state need to be restored (send the command to the workers).
  RASTER_CONTEXT_PENDING_CLIP_MASK_DELETED = 0x00080000,
#endif
};

// ============================================================================
// [Fog::RASTER_MODE]
// ============================================================================

enum RASTER_MODE
{
  RASTER_MODE_ST = 0,
  RASTER_MODE_MT = 1,

  RASTER_MODE_COUNT = 2
};

// ============================================================================
// [Fog::RASTER_PRECISION]
// ============================================================================

enum RASTER_PRECISION
{
  RASTER_PRECISION_NONE = 0x0,
  RASTER_PRECISION_F    = 0x1,
  RASTER_PRECISION_D    = 0x2,
  RASTER_PRECISION_BOTH = 0x3
};

// ============================================================================
// [Fog::RASTER_SOURCE]
// ============================================================================

//! @internal
//!
//! @brief Raster source type.
enum RASTER_SOURCE
{
  RASTER_SOURCE_NONE = 0,
  RASTER_SOURCE_ARGB32 = 1,
  RASTER_SOURCE_COLOR = 2,
  RASTER_SOURCE_TEXTURE = 3,
  RASTER_SOURCE_GRADIENT = 4
};

// ============================================================================
// [Fog::RASTER_SPAN]
// ============================================================================

//! @internal
//!
//! @brief Type of @c RasterSpan.
enum RASTER_SPAN
{
  // --------------------------------------------------------------------------
  // NOTE: When changing these constants, please make sure that the span methods
  // like isConst()/isVariant() are also changed. There are some optimizations
  // which are based on order of these constants.
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [Const]
  // --------------------------------------------------------------------------

  //! @brief Span is a const-mask.
  RASTER_SPAN_C = 0,

  // --------------------------------------------------------------------------
  // [Variant]
  // --------------------------------------------------------------------------

  //! @brief Start of variant-alpha span types (may be used by asserts).
  //!
  //! @note This constant is only shadow to valid mask type, don't use this
  //! value in switch() {}.
  RASTER_SPAN_V_BEGIN = 1,

  //! @brief Span is a variable-alpha mask (8-bit, A8).
  //!
  //! @sa @c RASTER_SPAN_AX_EXTRA.
  RASTER_SPAN_A8_GLYPH = 1,

  //! @brief Span is a variable-alpha mask (target bit-depth, A8, A16 or A32).
  //!
  //! @sa @c RASTER_SPAN_AX_EXTRA.
  RASTER_SPAN_AX_GLYPH = 2,

  //! @brief Span is an extended variable-alpha mask (target bit-depth + 1 bit
  //! for exact scaling).
  RASTER_SPAN_AX_EXTRA = 3,

  //! @brief Span is a variable-argb mask (8-bit, PRGB32).
  RASTER_SPAN_ARGB32_GLYPH = 4,

  //! @brief Span is a variable-argb mask (target bit-depth, PRGB32 or PRGB64).
  RASTER_SPAN_ARGBXX_GLYPH = 5,

  //! @brief The count of span types.
  RASTER_SPAN_COUNT = 6
};

enum
{
  //! @brief Helper constant to determine how many constant pixels are
  //! profitable for pure CMask span instance. Minimum is 1, but recommended
  //! are 4 or more (it depends on the  available SIMD instructions per platform).
  RASTER_SPAN_C_THRESHOLD = 4
};

// ============================================================================
// [Fog::RASTER_WORKER_STATE]
// ============================================================================

//! @internal
//!
//! @brief Raster worker state.
enum RASTER_WORKER_STATE
{
  RASTER_WORKER_RUNNING,
  RASTER_WORKER_WAITING,
  RASTER_WORKER_WAITING_AS_MAIN,
  RASTER_WORKER_DONE
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERCONSTANTS_P_H
