// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Painting/RasterConstants_p.h>

namespace Fog {

// ============================================================================
// [Fog::Raster - Data - CombineProperties]
// ============================================================================

// ${COMPOSITE_OP:BEGIN}
uint32_t _raster_combineProperties[COMPOSITE_COUNT] =
{
  /* 00: SRC         -> */ RASTER_COMBINE_OP_SRC,
  /* 01: SRC_OVER    -> */ RASTER_COMBINE_OP_SRC_OVER,
  /* 02: SRC_IN      -> */ RASTER_COMBINE_OP_SRC_IN,
  /* 03: SRC_OUT     -> */ RASTER_COMBINE_OP_SRC_OUT,
  /* 04: SRC_ATOP    -> */ RASTER_COMBINE_OP_SRC_ATOP,
  /* 05: DST         -> */ RASTER_COMBINE_OP_DST,
  /* 06: DST_OVER    -> */ RASTER_COMBINE_OP_DST_OVER,
  /* 07: DST_IN      -> */ RASTER_COMBINE_OP_DST_IN,
  /* 08: DST_OUT     -> */ RASTER_COMBINE_OP_DST_OUT,
  /* 09: DST_ATOP    -> */ RASTER_COMBINE_OP_DST_ATOP,
  /* 10: XOR         -> */ RASTER_COMBINE_OP_XOR,
  /* 11: CLEAR       -> */ RASTER_COMBINE_OP_CLEAR,
  /* 12: ADD         -> */ RASTER_COMBINE_OP_ADD,
  /* 13: SUBTRACT    -> */ RASTER_COMBINE_OP_SUBTRACT,
  /* 14: MULTIPLY    -> */ RASTER_COMBINE_OP_MULTIPLY,
  /* 15: SCREEN      -> */ RASTER_COMBINE_OP_SCREEN,
  /* 16: OVERLAY     -> */ RASTER_COMBINE_OP_OVERLAY,
  /* 17: DARKEN      -> */ RASTER_COMBINE_OP_DARKEN,
  /* 18: LIGHTEN     -> */ RASTER_COMBINE_OP_LIGHTEN,
  /* 19: COLOR_DODGE -> */ RASTER_COMBINE_OP_COLOR_DODGE,
  /* 20: COLOR_BURN  -> */ RASTER_COMBINE_OP_COLOR_BURN,
  /* 21: SOFT_LIGHT  -> */ RASTER_COMBINE_OP_SOFT_LIGHT,
  /* 22: HARD_LIGHT  -> */ RASTER_COMBINE_OP_HARD_LIGHT,
  /* 23: DIFFERENCE  -> */ RASTER_COMBINE_OP_DIFFERENCE,
  /* 24: EXCLUSION   -> */ RASTER_COMBINE_OP_EXCLUSION,
  /* 25: INVERT      -> */ RASTER_COMBINE_OP_INVERT,
  /* 26: INVERT_RGB  -> */ RASTER_COMBINE_OP_INVERT_RGB
};
// ${COMPOSITE_OP:END}

// ============================================================================
// [Fog::Raster - Data - LinearBlur8]
// ============================================================================

const uint16_t _raster_linear_blur8_mul[255] =
{
  512, 512, 456, 512, 328, 456, 335, 512, 405, 328, 271, 456, 388, 335, 292, 512,
  454, 405, 364, 328, 298, 271, 496, 456, 420, 388, 360, 335, 312, 292, 273, 512,
  482, 454, 428, 405, 383, 364, 345, 328, 312, 298, 284, 271, 259, 496, 475, 456,
  437, 420, 404, 388, 374, 360, 347, 335, 323, 312, 302, 292, 282, 273, 265, 512,
  497, 482, 468, 454, 441, 428, 417, 405, 394, 383, 373, 364, 354, 345, 337, 328,
  320, 312, 305, 298, 291, 284, 278, 271, 265, 259, 507, 496, 485, 475, 465, 456,
  446, 437, 428, 420, 412, 404, 396, 388, 381, 374, 367, 360, 354, 347, 341, 335,
  329, 323, 318, 312, 307, 302, 297, 292, 287, 282, 278, 273, 269, 265, 261, 512,
  505, 497, 489, 482, 475, 468, 461, 454, 447, 441, 435, 428, 422, 417, 411, 405,
  399, 394, 389, 383, 378, 373, 368, 364, 359, 354, 350, 345, 341, 337, 332, 328,
  324, 320, 316, 312, 309, 305, 301, 298, 294, 291, 287, 284, 281, 278, 274, 271,
  268, 265, 262, 259, 257, 507, 501, 496, 491, 485, 480, 475, 470, 465, 460, 456,
  451, 446, 442, 437, 433, 428, 424, 420, 416, 412, 408, 404, 400, 396, 392, 388,
  385, 381, 377, 374, 370, 367, 363, 360, 357, 354, 350, 347, 344, 341, 338, 335,
  332, 329, 326, 323, 320, 318, 315, 312, 310, 307, 304, 302, 299, 297, 294, 292,
  289, 287, 285, 282, 280, 278, 275, 273, 271, 269, 267, 265, 263, 261, 259
};

const uint8_t _raster_linear_blur8_shr[255] =
{
  9 , 11, 12, 13, 13, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17,
  17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19,
  19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20,
  20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21,
  21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
  21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22,
  22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
  22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23,
  23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
  23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
  23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
  23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24
};

// ============================================================================
// [Fog::Raster - Data - CompatibleFormat]
// ============================================================================

// ${IMAGE_FORMAT:BEGIN}
const RasterCompatibleFormat _g2d_render_compatibleFormat[IMAGE_FORMAT_COUNT][IMAGE_FORMAT_COUNT] =
{
  // --------------------------------------------------------------------------
  // [PRGB32]
  // --------------------------------------------------------------------------

  {
    /* PRGB32    <- PRGB32    */ { IMAGE_FORMAT_PRGB32   , IMAGE_FORMAT_PRGB32   , RASTER_VBLIT_PRGB32_VS_PRGB32, 0 },
    /* PRGB32    <- XRGB32    */ { IMAGE_FORMAT_PRGB32   , IMAGE_FORMAT_XRGB32   , RASTER_VBLIT_PRGB32_VS_XRGB32, 0 },
    /* PRGB32    <- RGB24     */ { IMAGE_FORMAT_PRGB32   , IMAGE_FORMAT_RGB24    , RASTER_VBLIT_PRGB32_VS_RGB24 , 0 },
    /* PRGB32    <- A8        */ { IMAGE_FORMAT_PRGB32   , IMAGE_FORMAT_PRGB32   , RASTER_VBLIT_PRGB32_VS_PRGB32, 0 },
    /* PRGB32    <- I8        */ { IMAGE_FORMAT_PRGB32   , IMAGE_FORMAT_PRGB32   , RASTER_VBLIT_PRGB32_VS_PRGB32, 0 },
    /* PRGB32    <- PRGB64    */ { IMAGE_FORMAT_PRGB32   , IMAGE_FORMAT_PRGB32   , RASTER_VBLIT_PRGB32_VS_PRGB32, 0 },
    /* PRGB32    <- RGB48     */ { IMAGE_FORMAT_PRGB32   , IMAGE_FORMAT_XRGB32   , RASTER_VBLIT_PRGB32_VS_XRGB32, 0 },
    /* PRGB32    <- A16       */ { IMAGE_FORMAT_PRGB32   , IMAGE_FORMAT_PRGB32   , RASTER_VBLIT_PRGB32_VS_PRGB32, 0 }
  },

  // --------------------------------------------------------------------------
  // [XRGB32]
  // --------------------------------------------------------------------------

  {
    /* XRGB32    <- PRGB32    */ { IMAGE_FORMAT_XRGB32   , IMAGE_FORMAT_PRGB32   , RASTER_VBLIT_PRGB32_VS_PRGB32, 0 },
    /* XRGB32    <- XRGB32    */ { IMAGE_FORMAT_XRGB32   , IMAGE_FORMAT_XRGB32   , RASTER_VBLIT_PRGB32_VS_XRGB32, 0 },
    /* XRGB32    <- RGB24     */ { IMAGE_FORMAT_XRGB32   , IMAGE_FORMAT_RGB24    , RASTER_VBLIT_PRGB32_VS_RGB24 , 0 },
    /* XRGB32    <- A8        */ { IMAGE_FORMAT_XRGB32   , IMAGE_FORMAT_PRGB32   , RASTER_VBLIT_PRGB32_VS_PRGB32, 0 },
    /* XRGB32    <- I8        */ { IMAGE_FORMAT_XRGB32   , IMAGE_FORMAT_PRGB32   , RASTER_VBLIT_PRGB32_VS_PRGB32, 0 },
    /* XRGB32    <- PRGB64    */ { IMAGE_FORMAT_XRGB32   , IMAGE_FORMAT_PRGB32   , RASTER_VBLIT_PRGB32_VS_PRGB32, 0 },
    /* XRGB32    <- RGB48     */ { IMAGE_FORMAT_XRGB32   , IMAGE_FORMAT_XRGB32   , RASTER_VBLIT_PRGB32_VS_XRGB32, 0 },
    /* XRGB32    <- A16       */ { IMAGE_FORMAT_XRGB32   , IMAGE_FORMAT_PRGB32   , RASTER_VBLIT_PRGB32_VS_PRGB32, 0 }
  },

  // --------------------------------------------------------------------------
  // [RGB24]
  // --------------------------------------------------------------------------

  {
    /* RGB24     <- PRGB32    */ { IMAGE_FORMAT_RGB24    , IMAGE_FORMAT_PRGB32   , RASTER_VBLIT_PRGB32_VS_PRGB32, 0 },
    /* RGB24     <- XRGB32    */ { IMAGE_FORMAT_RGB24    , IMAGE_FORMAT_XRGB32   , RASTER_VBLIT_PRGB32_VS_XRGB32, 0 },
    /* RGB24     <- RGB24     */ { IMAGE_FORMAT_RGB24    , IMAGE_FORMAT_RGB24    , RASTER_VBLIT_PRGB32_VS_RGB24 , 0 },
    /* RGB24     <- A8        */ { IMAGE_FORMAT_RGB24    , IMAGE_FORMAT_PRGB32   , RASTER_VBLIT_PRGB32_VS_PRGB32, 0 },
    /* RGB24     <- I8        */ { IMAGE_FORMAT_RGB24    , IMAGE_FORMAT_PRGB32   , RASTER_VBLIT_PRGB32_VS_PRGB32, 0 },
    /* RGB24     <- PRGB64    */ { IMAGE_FORMAT_RGB24    , IMAGE_FORMAT_PRGB32   , RASTER_VBLIT_PRGB32_VS_PRGB32, 0 },
    /* RGB24     <- RGB48     */ { IMAGE_FORMAT_RGB24    , IMAGE_FORMAT_XRGB32   , RASTER_VBLIT_PRGB32_VS_XRGB32, 0 },
    /* RGB24     <- A16       */ { IMAGE_FORMAT_RGB24    , IMAGE_FORMAT_PRGB32   , RASTER_VBLIT_PRGB32_VS_PRGB32, 0 }
  },

  // --------------------------------------------------------------------------
  // [A8]
  // --------------------------------------------------------------------------

  {
    /* A8        <- PRGB32    */ { IMAGE_FORMAT_A8       , IMAGE_FORMAT_A8       , RASTER_VBLIT_A8_VS_A8        , 0 },
    /* A8        <- XRGB32    */ { IMAGE_FORMAT_A8       , IMAGE_FORMAT_A8       , RASTER_VBLIT_A8_VS_A8        , 0 },
    /* A8        <- RGB24     */ { IMAGE_FORMAT_A8       , IMAGE_FORMAT_A8       , RASTER_VBLIT_A8_VS_A8        , 0 },
    /* A8        <- A8        */ { IMAGE_FORMAT_A8       , IMAGE_FORMAT_A8       , RASTER_VBLIT_A8_VS_A8        , 0 },
    /* A8        <- I8        */ { IMAGE_FORMAT_A8       , IMAGE_FORMAT_A8       , RASTER_VBLIT_A8_VS_A8        , 0 },
    /* A8        <- PRGB64    */ { IMAGE_FORMAT_A8       , IMAGE_FORMAT_A8       , RASTER_VBLIT_A8_VS_A8        , 0 },
    /* A8        <- RGB48     */ { IMAGE_FORMAT_A8       , IMAGE_FORMAT_A8       , RASTER_VBLIT_A8_VS_A8        , 0 },
    /* A8        <- A16       */ { IMAGE_FORMAT_A8       , IMAGE_FORMAT_A8       , RASTER_VBLIT_A8_VS_A8        , 0 }
  },

  // --------------------------------------------------------------------------
  // [I8]
  // --------------------------------------------------------------------------

  {
    /* I8        <- PRGB32    */ { IMAGE_FORMAT_NULL     , IMAGE_FORMAT_NULL     , RASTER_VBLIT_INVALID         , 0 },
    /* I8        <- XRGB32    */ { IMAGE_FORMAT_NULL     , IMAGE_FORMAT_NULL     , RASTER_VBLIT_INVALID         , 0 },
    /* I8        <- RGB24     */ { IMAGE_FORMAT_NULL     , IMAGE_FORMAT_NULL     , RASTER_VBLIT_INVALID         , 0 },
    /* I8        <- A8        */ { IMAGE_FORMAT_NULL     , IMAGE_FORMAT_NULL     , RASTER_VBLIT_INVALID         , 0 },
    /* I8        <- I8        */ { IMAGE_FORMAT_NULL     , IMAGE_FORMAT_NULL     , RASTER_VBLIT_INVALID         , 0 },
    /* I8        <- PRGB64    */ { IMAGE_FORMAT_NULL     , IMAGE_FORMAT_NULL     , RASTER_VBLIT_INVALID         , 0 },
    /* I8        <- RGB48     */ { IMAGE_FORMAT_NULL     , IMAGE_FORMAT_NULL     , RASTER_VBLIT_INVALID         , 0 },
    /* I8        <- A16       */ { IMAGE_FORMAT_NULL     , IMAGE_FORMAT_NULL     , RASTER_VBLIT_INVALID         , 0 }
  },

  // --------------------------------------------------------------------------
  // [PRGB64]
  // --------------------------------------------------------------------------

  {
    /* PRGB64    <- PRGB32    */ { IMAGE_FORMAT_PRGB64   , IMAGE_FORMAT_PRGB32   , RASTER_VBLIT_PRGB64_VS_PRGB32, 0 },
    /* PRGB64    <- XRGB32    */ { IMAGE_FORMAT_PRGB64   , IMAGE_FORMAT_RGB48    , RASTER_VBLIT_PRGB64_VS_RGB48 , 0 },
    /* PRGB64    <- RGB24     */ { IMAGE_FORMAT_PRGB64   , IMAGE_FORMAT_RGB48    , RASTER_VBLIT_PRGB64_VS_RGB48 , 0 },
    /* PRGB64    <- A8        */ { IMAGE_FORMAT_PRGB64   , IMAGE_FORMAT_PRGB32   , RASTER_VBLIT_PRGB64_VS_PRGB32, 0 },
    /* PRGB64    <- I8        */ { IMAGE_FORMAT_PRGB64   , IMAGE_FORMAT_PRGB32   , RASTER_VBLIT_PRGB64_VS_PRGB32, 0 },
    /* PRGB64    <- PRGB64    */ { IMAGE_FORMAT_PRGB64   , IMAGE_FORMAT_PRGB64   , RASTER_VBLIT_PRGB64_VS_PRGB64, 0 },
    /* PRGB64    <- RGB48     */ { IMAGE_FORMAT_PRGB64   , IMAGE_FORMAT_PRGB64   , RASTER_VBLIT_PRGB64_VS_RGB48 , 0 },
    /* PRGB64    <- A16       */ { IMAGE_FORMAT_PRGB64   , IMAGE_FORMAT_PRGB64   , RASTER_VBLIT_PRGB64_VS_PRGB64, 0 }
  },

  // --------------------------------------------------------------------------
  // [RGB48]
  // --------------------------------------------------------------------------

  {
    /* RGB48     <- PRGB32    */ { IMAGE_FORMAT_RGB48    , IMAGE_FORMAT_PRGB32   , RASTER_VBLIT_PRGB64_VS_PRGB32 , 0 },
    /* RGB48     <- XRGB32    */ { IMAGE_FORMAT_RGB48    , IMAGE_FORMAT_RGB48    , RASTER_VBLIT_PRGB64_VS_RGB48  , 0 },
    /* RGB48     <- RGB24     */ { IMAGE_FORMAT_RGB48    , IMAGE_FORMAT_RGB48    , RASTER_VBLIT_PRGB64_VS_RGB48  , 0 },
    /* RGB48     <- A8        */ { IMAGE_FORMAT_RGB48    , IMAGE_FORMAT_PRGB32   , RASTER_VBLIT_PRGB64_VS_PRGB32 , 0 },
    /* RGB48     <- I8        */ { IMAGE_FORMAT_RGB48    , IMAGE_FORMAT_PRGB32   , RASTER_VBLIT_PRGB64_VS_PRGB32 , 0 },
    /* RGB48     <- PRGB64    */ { IMAGE_FORMAT_RGB48    , IMAGE_FORMAT_PRGB64   , RASTER_VBLIT_PRGB64_VS_PRGB64 , 0 },
    /* RGB48     <- RGB48     */ { IMAGE_FORMAT_RGB48    , IMAGE_FORMAT_PRGB64   , RASTER_VBLIT_PRGB64_VS_RGB48  , 0 },
    /* RGB48     <- A16       */ { IMAGE_FORMAT_RGB48    , IMAGE_FORMAT_PRGB64   , RASTER_VBLIT_PRGB64_VS_PRGB64 , 0 }
  },

  // --------------------------------------------------------------------------
  // [A16]
  // --------------------------------------------------------------------------

  {
    /* A16       <- PRGB32    */ { IMAGE_FORMAT_A16      , IMAGE_FORMAT_A8       , RASTER_VBLIT_A16_VS_A8        , 0 },
    /* A16       <- XRGB32    */ { IMAGE_FORMAT_A16      , IMAGE_FORMAT_A8       , RASTER_VBLIT_A16_VS_A8        , 0 },
    /* A16       <- RGB24     */ { IMAGE_FORMAT_A16      , IMAGE_FORMAT_A8       , RASTER_VBLIT_A16_VS_A8        , 0 },
    /* A16       <- A8        */ { IMAGE_FORMAT_A16      , IMAGE_FORMAT_A8       , RASTER_VBLIT_A16_VS_A8        , 0 },
    /* A16       <- I8        */ { IMAGE_FORMAT_A16      , IMAGE_FORMAT_A8       , RASTER_VBLIT_A16_VS_A8        , 0 },
    /* A16       <- PRGB64    */ { IMAGE_FORMAT_A16      , IMAGE_FORMAT_A16      , RASTER_VBLIT_A16_VS_A16       , 0 },
    /* A16       <- RGB48     */ { IMAGE_FORMAT_A16      , IMAGE_FORMAT_A16      , RASTER_VBLIT_A16_VS_A16       , 0 },
    /* A16       <- A16       */ { IMAGE_FORMAT_A16      , IMAGE_FORMAT_A16      , RASTER_VBLIT_A16_VS_A16       , 0 }
  }
};
// ${IMAGE_FORMAT:END}

} // Fog namespace
