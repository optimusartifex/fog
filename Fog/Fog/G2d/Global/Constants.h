// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GLOBAL_CONSTANTS_H
#define _FOG_G2D_GLOBAL_CONSTANTS_H

// [Dependencies]
#include <Fog/Core/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_G2d_Global
//! @{

// ============================================================================
// [Fog::ALPHA_DISTRIBUTION]
// ============================================================================

//! @brief Distribution of alpha channel values in an image or in a color table.
enum ALPHA_DISTRIBUTION
{
  //! @brief Alpha values are 0.
  ALPHA_DISTRIBUTION_ZERO = 0,
  //! @brief Alpha values are 1.
  ALPHA_DISTRIBUTION_FULL = 1,
  //! @brief Alpha values are 0 (transparent) or 1 (opaque).
  ALPHA_DISTRIBUTION_ZERO_OR_FULL = 2,
  //! @brief Alpha values are variant.
  ALPHA_DISTRIBUTION_VARIANT = 3,

  //! @brief Count of alpha distribution types.
  ALPHA_DISTRIBUTION_COUNT = 4
};

// ============================================================================
// [Fog::ANTIALIASING_QUALITY]
// ============================================================================

//! @brief Anti-aliasing quality.
enum ANTIALIASING_QUALITY
{
  //! @brief No anti-aliasing.
  ANTIALIASING_QUALITY_NONE = 0,

  //! @brief Low quality anti-aliasing.
  ANTIALIASING_QUALITY_LOW = 1,
  //! @brief Middle quality anti-aliasing.
  ANTIALIASING_QUALITY_NORMAL = 2,
  //! @brief High quality anti-aliasing.
  ANTIALIASING_QUALITY_HIGH = 3,

  //! @brief Vertically oriented LCD anti-aliasing (RGB order).
  ANTIALIASING_QUALITY_LCD_VRGB = 4,

  //! @brief Vertically oriented LCD anti-aliasing (BGR order).
  ANTIALIASING_QUALITY_LCD_VBGR = 5,

  //! @brief Horizontally oriented LCD anti-aliasing (RGB order).
  ANTIALIASING_QUALITY_LCD_HRGB = 6,

  //! @brief Horizontally oriented LCD anti-aliasing (BGR order).
  ANTIALIASING_QUALITY_LCD_HBGR = 7,

  //! @brief Count of anti-aliasing quality settings (for error checking).
  ANTIALIASING_QUALITY_COUNT = 8,

  //! @brief Default anti-aliasing quality (synonym to @c ANTIALIASING_QUALITY_NORMAL)
  ANTIALIASING_QUALITY_DEFAULT = ANTIALIASING_QUALITY_NORMAL
};

// ============================================================================
// [Fog::ARGB32_POS]
// ============================================================================

enum ARGB32_POS
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  ARGB32_ABYTE = 3,
  ARGB32_RBYTE = 2,
  ARGB32_GBYTE = 1,
  ARGB32_BBYTE = 0
#else // FOG_BYTE_ORDER == FOG_BIG_ENDIAN
  ARGB32_ABYTE = 0,
  ARGB32_RBYTE = 1,
  ARGB32_GBYTE = 2,
  ARGB32_BBYTE = 3
#endif // FOG_BYTE_ORDER
};

// ============================================================================
// [Fog::ARGB32_MASK]
// ============================================================================

static const uint32_t ARGB32_AMASK = 0xFF000000U;
static const uint32_t ARGB32_RMASK = 0x00FF0000U;
static const uint32_t ARGB32_GMASK = 0x0000FF00U;
static const uint32_t ARGB32_BMASK = 0x000000FFU;

// ============================================================================
// [Fog::ARGB32_SHIFT]
// ============================================================================

enum ARGB32_SHIFT
{
  ARGB32_ASHIFT = 24U,
  ARGB32_RSHIFT = 16U,
  ARGB32_GSHIFT =  8U,
  ARGB32_BSHIFT =  0U
};

// ============================================================================
// [Fog::ARGB64_POS]
// ============================================================================

enum ARGB64_POS
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  ARGB64_AWORD = 3,
  ARGB64_RWORD = 2,
  ARGB64_GWORD = 1,
  ARGB64_BWORD = 0
#else // FOG_BYTE_ORDER == FOG_BIG_ENDIAN
  ARGB64_AWORD = 0,
  ARGB64_RWORD = 1,
  ARGB64_GWORD = 2,
  ARGB64_BWORD = 3
#endif // FOG_BYTE_ORDER
};

// ============================================================================
// [Fog::ARGB64_MASK]
// ============================================================================

static const uint64_t ARGB64_AMASK = FOG_UINT64_C(0xFFFF000000000000);
static const uint64_t ARGB64_RMASK = FOG_UINT64_C(0x0000FFFF00000000);
static const uint64_t ARGB64_GMASK = FOG_UINT64_C(0x00000000FFFF0000);
static const uint64_t ARGB64_BMASK = FOG_UINT64_C(0x000000000000FFFF);

// ============================================================================
// [Fog::ARGB64_SHIFT]
// ============================================================================

enum ARGB64_SHIFT
{
  ARGB64_ASHIFT = 48U,
  ARGB64_RSHIFT = 32U,
  ARGB64_GSHIFT = 16U,
  ARGB64_BSHIFT =  0U
};

// ============================================================================
// [Fog::RGB24_POS]
// ============================================================================

enum RGB24_POS
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  RGB24_RBYTE = 2,
  RGB24_GBYTE = 1,
  RGB24_BBYTE = 0
#else // FOG_BYTE_ORDER == FOG_BIG_ENDIAN
  RGB24_RBYTE = 0,
  RGB24_GBYTE = 1,
  RGB24_BBYTE = 2
#endif // FOG_BYTE_ORDER
};

// ============================================================================
// [Fog::RGB48_POS]
// ============================================================================

enum RGB48_POS
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  RGB48_RWORD = 2,
  RGB48_GWORD = 1,
  RGB48_BWORD = 0
#else // FOG_BYTE_ORDER == FOG_BIG_ENDIAN
  RGB48_RWORD = 0,
  RGB48_GWORD = 1,
  RGB48_BWORD = 2
#endif // FOG_BYTE_ORDER
};

// ============================================================================
// [Fog::AXIS_FLAGS]
// ============================================================================

//! @brief Axis.
enum AXIS_FLAGS
{
  //! @brief X axis.
  AXIS_X = 0x1,
  //! @brief Y axis.
  AXIS_Y = 0x2,
  //! @brief Z axis.
  AXIS_Z = 0x4
};

// ============================================================================
// [Fog::CLIP_SHIFT]
// ============================================================================

enum CLIP_SHIFT
{
  CLIP_SHIFT_X0 = 2,
  CLIP_SHIFT_Y0 = 3,
  CLIP_SHIFT_X1 = 0,
  CLIP_SHIFT_Y1 = 1
};

// ============================================================================
// [Fog::CLIP_SIDE]
// ============================================================================

/*
  CLIPPED_X0 = 0x4,
  CLIPPED_X1 = 0x1,
  CLIPPED_Y0 = 0x8,
  CLIPPED_Y1 = 0x2,
  CLIPPED_X  = CLIPPED_X0 | CLIPPED_X1,
  CLIPPED_Y  = CLIPPED_Y0 | CLIPPED_Y1
*/

enum CLIP_SIDE
{
  CLIP_SIDE_NONE   = 0x00,

  CLIP_SIDE_X0     = 1 << CLIP_SHIFT_X0,
  CLIP_SIDE_Y0     = 1 << CLIP_SHIFT_Y0,
  CLIP_SIDE_X1     = 1 << CLIP_SHIFT_X1,
  CLIP_SIDE_Y1     = 1 << CLIP_SHIFT_Y1,

  CLIP_SIDE_X      = CLIP_SIDE_X0 | CLIP_SIDE_X1,
  CLIP_SIDE_Y      = CLIP_SIDE_Y0 | CLIP_SIDE_Y1,

  CLIP_SIDE_LEFT   = CLIP_SIDE_X0,
  CLIP_SIDE_TOP    = CLIP_SIDE_Y0,
  CLIP_SIDE_RIGHT  = CLIP_SIDE_X1,
  CLIP_SIDE_BOTTOM = CLIP_SIDE_Y1,

  CLIP_SIDE_LT     = CLIP_SIDE_LEFT | CLIP_SIDE_TOP,
  CLIP_SIDE_RT     = CLIP_SIDE_RIGHT | CLIP_SIDE_TOP,

  CLIP_SIDE_LB     = CLIP_SIDE_LEFT | CLIP_SIDE_BOTTOM,
  CLIP_SIDE_RB     = CLIP_SIDE_RIGHT | CLIP_SIDE_BOTTOM
};

// ============================================================================
// [Fog::CLIPPER_INIT]
// ============================================================================

enum CLIPPER_INIT
{
  CLIPPER_INIT_ALREADY_CLIPPED,
  CLIPPER_INIT_NOT_CLIPPED,
  CLIPPER_INIT_INVALID_PATH
};

// ============================================================================
// [Fog::COMPOSITE_OP]
// ============================================================================

//! @brief The compositing operator.
enum COMPOSITE_OP
{
  //! @brief The source is copied to the destination.
  //!
  //! The source pixel is copied to destination pixel. If destination pixel
  //! format not supports alpha channel, the source alpha value is ignored.
  //! If there is alpha channel mask the composition is done using LERP
  //! operator.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SRC):
  //!   Dca' = Sca.Da + Sca.(1 - Da)
  //!        = Sca
  //!   Da'  = Sa.Da + Sa.(1 - Da)
  //!        = Sa
  //! @endverbatim
  COMPOSITE_SRC = 0,

  //! @brief The source is composited over the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SRC_OVER):
  //!   Dca' = Sca.Da + Sca.(1 - Da) + Dca.(1 - Sa)
  //!        = Sca + Dca.(1 - Sa)
  //!   Da'  = Sa.Da + Sa.(1 - Da) + Da.(1 - Sa)
  //!        = Sa + Da.(1 - Sa)
  //!        = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_SRC_OVER = 1,

  //! @brief The part of the source lying inside of the destination replaces
  //! the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SRC_IN):
  //!   Dca' = Sca.Da
  //!   Da'  = Sa.Da
  //! @endverbatim
  COMPOSITE_SRC_IN = 2,

  //! @brief The part of the source lying outside of the destination replaces
  //! the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SRC_OUT):
  //!   Dca' = Sca.(1 - Da)
  //!   Da'  = Sa.(1 - Da)
  //! @endverbatim
  COMPOSITE_SRC_OUT = 3,

  //! @brief The part of the source lying inside of the destination is
  //! composited onto the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SRC_ATOP):
  //!   Dca' = Sca.Da + Dca.(1 - Sa)
  //!   Da'  = Sa.Da + Da.(1 - Sa)
  //!        = Sa.Da + Da - Da.Sa
  //!        = Da
  //! @endverbatim
  COMPOSITE_SRC_ATOP = 4,

  //! @brief The destination is left untouched.
  //!
  //! Destination pixels remains unchanged.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (NOP):
  //!   Dca' = Dca
  //!   Da'  = Da
  //! @endverbatim
  COMPOSITE_DST = 5,

  //! @brief The destination is composited over the source and the result
  //! replaces the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (DST_OVER):
  //!   Dca' = Dca.Sa + Sca.(1 - Da) + Dca.(1 - Sa)
  //!        = Dca + Sca.(1 - Da)
  //!   Da'  = Da.Sa + Sa.(1 - Da) + Da.(1 - Sa)
  //!        = Da + Sa.(1 - Da)
  //!        = Da + Sa - Da.Sa
  //! @endverbatim
  COMPOSITE_DST_OVER = 6,

  //! @brief The part of the destination lying inside of the source replaces
  //! the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (DST_IN):
  //!   Dca' = Dca.Sa
  //!   Da'  = Da.Sa
  //! @endverbatim
  COMPOSITE_DST_IN = 7,

  //! @brief The part of the destination lying outside of the source replaces
  //! the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (DST_OUT):
  //!   Dca' = Dca.(1 - Sa)
  //!   Da'  = Da.(1 - Sa)
  //! @endverbatim
  COMPOSITE_DST_OUT = 8,

  //! @brief The part of the destination lying inside of the source is
  //! composited over the source and replaces the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (DST_ATOP):
  //!   Dca' = Dca.Sa + Sca.(1 - Da)
  //!   Da'  = Da.Sa + Sa.(1 - Da)
  //!        = Sa.(Da + 1 - Da)
  //!        = Sa
  //! @endverbatim
  COMPOSITE_DST_ATOP = 9,

  //! @brief The part of the source that lies outside of the destination is
  //! combined with the part of the destination that lies outside of the source.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (XOR):
  //!   Dca' = Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa.(1 - Da) + Da.(1 - Sa)
  //!        = Sa + Da - 2.Sa.Da
  //! @endverbatim
  COMPOSITE_XOR = 10,

  //! @brief Clear the destination not using the source.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (CLEAR):
  //!   Dca' = 0
  //!   Da'  = 0
  //! @endverbatim
  COMPOSITE_CLEAR = 11,

  //! @brief The source is added to the destination and replaces the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (ADD):
  //!   Dca' = Sca.Da + Dca.Sa + Sca.(1 - Da) + Dca.(1 - Sa)
  //!        = Dca + Sca
  //!   Da'  = Sa.Da + Da.Sa + Sa.(1 - Da) + Da.(1 - Sa)
  //!        = Da + Sa
  //! @endverbatim
  COMPOSITE_ADD = 12,

  //! @brief The source is subtracted from the destination and replaces
  //! the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SUBTRACT):
  //!   Dca' = Dca - Sca
  //!   Da'  = 1 - (1 - Sa).(1 - Da)
  //!        = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_SUBTRACT = 13,

  //! @brief The source is multiplied by the destination and replaces
  //! the destination.
  //!
  //! The resultant color is always at least as dark as either of the two
  //! constituent colors. Multiplying any color with black produces black.
  //! Multiplying any color with white leaves the original color unchanged.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (MULTIPLY):
  //!   Dca' = Sca.Dca + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa.Da + Sa.(1 - Da) + Da.(1 - Sa)
  //! @endverbatim
  COMPOSITE_MULTIPLY = 14,

  //! @brief The source and destination are complemented and then multiplied
  //! and then replace the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SCREEN):
  //!   Dca' = Sca + Dca.(1 - Sca)
  //!   Da'  = Sa + Da.(1 - Sa)
  //! @endverbatim
  COMPOSITE_SCREEN = 15,

  //! @brief Multiplies or screens the colors, dependent on the destination
  //! color.
  //!
  //! Source colors overlay the destination whilst preserving its highlights
  //! and shadows. The destination color is not replaced, but is mixed with
  //! the source color to reflect the lightness or darkness of the destination.
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (OVERLAY):
  //!
  //! @verbatim
  //! Tca = Sca.(1 - Da) + Dca.(1 - Sa)
  //! Ta  = Sa .(1 - Da) + Da .(1 - Sa)
  //!
  //! if (2.Dca < Da)
  //!   Dca' = 2.Sca.Dca + Tca
  //!   Da'  = Sa.Da     + Ta
  //!        = Sa + Da - Sa.Da
  //! else
  //!   Dca' = Sa.Da - 2.(Da - Dca).(Sa - Sca) + Tca
  //!   Da'  = Sa.Da - 2.(Da - Da ).(Sa - Sa ) + Ta
  //!        = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_OVERLAY = 16,

  //! @brief Selects the darker of the destination and source colors.
  //!
  //! The destination is replaced with the source when the source is
  //! darker, otherwise it is left unchanged.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (DARKEN):
  //!   Dca' = min(Sca.Da, Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = min(Sa.Da, Da.Sa) + Sa.(1 - Da) + Da.(1 - Sa)
  //!        = Sa.Da + Sa - Sa.Da + Da - Sa.Da
  //!        = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_DARKEN = 17,

  //! @brief Selects the lighter of the destination and source colors.
  //!
  //! The destination is replaced with the source when the source is
  //! lighter, otherwise it is left unchanged.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (LIGHTEN):
  //!   Dca' = max(Sca.Da, Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = max(Sa.Da, Da.Sa) + Sa.(1 - Da) + Da.(1 - Sa)
  //!        = Sa.Da + Sa - Sa.Da + Da - Sa.Da
  //!        = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_LIGHTEN = 18,

  //! @brief Brightens the destination color to reflect the source color.
  //!
  //! Painting with black produces no change.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (COLOR_DODGE):
  //!
  //! if (Sca.Da + Dca.Sa >= Sa.Da)
  //!   Dca' = Sa.Da + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa + Da - Sa.Da
  //! else
  //!   Dca' = Dca.Sa / (1 - Sca / Sa) + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Dca' = [Dca.(Sa - Sca.(1 - Sa)) + Sca((Sa - Sca).(1 - Da))] / [Sa - Sca]
  //!   Da'  = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_COLOR_DODGE = 19,

  //! @brief Darkens the destination color to reflect the source color.
  //! Painting with white produces no change.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (COLOR_BURN):
  //!
  //! if (Sca.Da + Dca.Sa <= Sa.Da)
  //!   Dca' = Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa + Da - Sa.Da
  //! else
  //!   Dca' = Sa.(Sca.Da + Dca.Sa - Sa.Da)/Sca + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_COLOR_BURN = 20,

  //! @brief Darkens or lightens the colors, dependent on the source color value.
  //!
  //! If the source color is lighter than 0.5, the destination is lightened. If
  //! the source color is darker than 0.5, the destination is darkened, as if
  //! it were burned in. The degree of darkening or lightening is proportional
  //! to the difference between the source color and 0.5. If it is equal to 0.5,
  //! the destination is unchanged. Painting with pure black or white produces
  //! a distinctly darker or lighter area, but does not result in pure black or
  //! white.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SOFT_LIGHT):
  //!
  //! if (2.Sca <= Sa)
  //!   Dca' = Dca.Sa - (Sa.Da - 2.Sca.Da).Dca.Sa.(Sa.Da - Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa + Da - Sa.Da
  //! else if (4.Dca <= Da)
  //!   Dca' = Dca.Sa + (2.Sca.Da - Sa.Da).((((16.Dsa.Sa - 12).Dsa.Sa + 4).Dsa.Da) - Dsa.Da) + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa + Da - Sa.Da
  //! else
  //!   Dca' = Dca.Sa + (2.Sca.Da - Sa.Da).((Dca.Sa)^0.5 - Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_SOFT_LIGHT = 21,

  //! @brief Multiplies or screens the colors, dependent on the source color value.
  //!
  //! If the source color is lighter than 0.5, the destination is lightened as
  //! if it were screened. If the source color is darker than 0.5, the destination
  //! is darkened, as if it were multiplied. The degree of lightening or darkening
  //! is proportional to the difference between the source color and 0.5. If it is
  //! equal to 0.5 the destination is unchanged. Painting with pure black or white
  //! produces black or white.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (HARD_LIGHT):
  //!
  //! if (2.Sca <= Sa)
  //!   Dca' = 2.Sca.Dca + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa + Da - Sa.Da
  //! else
  //!   Dca' = Sa.Da - 2.(Da - Dca).(Sa - Sca) + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_HARD_LIGHT = 22,

  //! @brief Subtracts the darker of the two constituent colors from the
  //! lighter.
  //!
  //! Painting with white inverts the destination color. Painting with
  //! black produces no change.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (DIFFERENCE):
  //!   Dca' = abs(Dca.Sa - Sca.Da) + Sca.(1 - Da) + Dca.(1 - Sa)
  //!        = Dca + Sca - 2.min(Sca.Da, Dca.Sa)
  //!   Da'  = abs(Da.Sa - Sa.Da) + Sa.(1 - Da) + Da.(1 - Sa)
  //!        = Sa + Da - min(Sa.Da, Da.Sa)
  //!        = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_DIFFERENCE = 23,

  //! @brief Produces an effect similar to that of 'difference', but appears
  //! as lower contrast.
  //!
  //! Painting with white inverts the destination color. Painting with black
  //! produces no change.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (EXCLUSION):
  //!   Dca' = Sca.Da + Dca - 2.Sca.Dca
  //!   Da'  = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_EXCLUSION = 24,

  //! @brief Invert.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (INVERT):
  //!   Dca' = (Da - Dca) * Sa + Dca.(1 - Sa)
  //!   Da'  = (1) * Sa + Da.(1 - Sa)
  //! @endverbatim
  COMPOSITE_INVERT = 25,

  //! @brief Invert RGB.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (INVERT_RGB):
  //!   Dca' = (Da - Dca) * Sca + Dca.(1 - Sa)
  //!   Da'  = (1) * Sa + Da.(1 - Sa)
  //! @endverbatim
  COMPOSITE_INVERT_RGB = 26,

  //! @brief Count of compositing operators (for error checking).
  COMPOSITE_COUNT = 27
};

// ============================================================================
// [Fog::CLIP_OP]
// ============================================================================

//! @brief Clip operation used by @c Painter::clip() and PainterEngine::clip()
//! methods.
enum CLIP_OP
{
  //! @brief Replace the current clipping area by the filled shape.
  CLIP_OP_REPLACE = 0x0,
  //! @brief Intersect the current clipping area by the filled shape.
  CLIP_OP_INTERSECT = 0x1,

  //! @brief Stroke instead of fill (invalid when clipping to masks).
  CLIP_OP_STROKE = 0x2,

  //! @brief Count of clip operators.
  CLIP_OP_COUNT = 4
};

// ============================================================================
// [Fog::COLOR_ADJUST]
// ============================================================================

enum COLOR_ADJUST_OP
{
  COLOR_ADJUST_OP_NONE = 0,
  COLOR_ADJUST_OP_LIGHTEN = 1,
  COLOR_ADJUST_OP_DARKEN = 2,
  COLOR_ADJUST_OP_HUE = 3,

  COLOR_ADJUST_OP_COUNT = 4
};

// ============================================================================
// [Fog::COLOR_MODEL]
// ============================================================================

enum COLOR_MODEL
{
  // --------------------------------------------------------------------------
  // [Public]
  // --------------------------------------------------------------------------

  COLOR_MODEL_NONE = 0,
  COLOR_MODEL_ARGB = 1,
  COLOR_MODEL_AHSV = 2,
  COLOR_MODEL_AHSL = 3,
  COLOR_MODEL_ACMYK = 4,

  COLOR_MODEL_COUNT = 5,

  // --------------------------------------------------------------------------
  // [Private]
  // --------------------------------------------------------------------------

  // Used internally by the Fog::Color conversion classes, reserved one color
  // model for the future binary compatibility.

  //! @internal
  _COLOR_MODEL_ARGB32 = 6,
  //! @internal
  _COLOR_MODEL_ARGB64 = 7,
  //! @internal
  _COLOR_MODEL_COUNT = 8
};

// ============================================================================
// [Fog::COLOR_NAME]
// ============================================================================

enum COLOR_NAME
{
  //! @brief Color in the CSS-RGB form "rgb(R, G, B)".
  COLOR_NAME_CSS_RGB = 0x00000001,
  //! @brief Color in the CSS-RGBA form "rgba(R, G, B, A)".
  COLOR_NAME_CSS_RGBA = 0x00000002,
  //! @brief Color in the CSS-HSL form "hsl(H, S, L)".
  COLOR_NAME_CSS_HSL = 0x00000004,
  //! @brief Color in the CSS-HSLA form "hsla(H, S, L, A)".
  COLOR_NAME_CSS_HSLA = 0x00000008,

  //! @brief Color in the CSS-HEX form "#RRGGBB" or "#RGB".
  COLOR_NAME_CSS_HEX = 0x00000010,

  //! @brief Color that matches the CSS color keywords.
  COLOR_NAME_CSS_KEYWORD = 0x00000020,
  //! @brief Color that matches the 'transparent' keyword.
  COLOR_NAME_CSS_TRANSPARENT = 0x00000040,

  //! @brief Strict mode.
  //!
  //! Strict mode does following:
  //!   - Error is returned if the given string starts with white-space
  //!   - Error is returned if the given string not ends with the color
  //!     keyword or data (continues).
  //!   - Error is returned for an non-css compliant color combination,
  //!     for example rgb(255, 100%, 100%) is invalid color string,
  //!     because percent notation is mixed with integer notation.
  COLOR_NAME_STRICT = 0x00001000,
  //! @brief Ignore case of the color string.
  COLOR_NAME_IGNORE_CASE = 0x00002000,

  //! @brief Use all rules to parse the color in CSS format.
  COLOR_NAME_CSS =
    COLOR_NAME_CSS_RGB         |
    COLOR_NAME_CSS_RGBA        |
    COLOR_NAME_CSS_HSL         |
    COLOR_NAME_CSS_HSLA        |
    COLOR_NAME_CSS_HEX         |
    COLOR_NAME_CSS_KEYWORD     |
    COLOR_NAME_CSS_TRANSPARENT ,

  //! @brief Use all rules to parse the color in ANY format.
  COLOR_NAME_ANY =
    COLOR_NAME_CSS             |
    COLOR_NAME_IGNORE_CASE
};

// ============================================================================
// [Fog::COLOR_MIX_ALPHA]
// ============================================================================

enum COLOR_MIX_ALPHA
{
  //! @brief Preserve the destination alpha and do not use the source alpha.
  COLOR_MIX_ALPHA_IGNORE = 0,

  //! @brief Preserve the destination alpha and multiply the source alpha with
  //! the mask.
  COLOR_MIX_ALPHA_MASK = 1,

  //! @brief Treat the alpha as a RGB component, using the same mix function.
  COLOR_MIX_ALPHA_COMPONENT = 2
};

// ============================================================================
// [Fog::COLOR_MIX_OP]
// ============================================================================

enum COLOR_MIX_OP
{
  COLOR_MIX_OP_NONE = 0,
  COLOR_MIX_OP_BLEND = 1,
  COLOR_MIX_OP_ADD = 2,
  COLOR_MIX_OP_SUBTRACT = 3,
  COLOR_MIX_OP_MULTIPLY = 4,
  COLOR_MIX_OP_DIVIDE = 5,
  COLOR_MIX_OP_SCREEN = 6,
  COLOR_MIX_OP_OVERLAY = 7,
  COLOR_MIX_OP_DARKEN = 8,
  COLOR_MIX_OP_LIGHTEN = 9,
  COLOR_MIX_OP_DODGE = 10,
  COLOR_MIX_OP_BURN = 11,
  COLOR_MIX_OP_DIFFERENCE = 12,

  COLOR_MIX_OP_COUNT = 13
};

// ============================================================================
// [Fog::COLOR_CHANNEL]
// ============================================================================

//! @brief Argb32 color channels.
enum COLOR_CHANNEL
{
  COLOR_CHANNEL_NONE  = 0x0,

  COLOR_CHANNEL_ALPHA = 0x1,
  COLOR_CHANNEL_RED   = 0x2,
  COLOR_CHANNEL_GREEN = 0x4,
  COLOR_CHANNEL_BLUE  = 0x8,

  COLOR_CHANNEL_RGB   = COLOR_CHANNEL_RED | COLOR_CHANNEL_GREEN | COLOR_CHANNEL_BLUE,
  COLOR_CHANNEL_ARGB  = COLOR_CHANNEL_RGB | COLOR_CHANNEL_ALPHA,

  COLOR_CHANNEL_COUNT = COLOR_CHANNEL_ARGB + 1
};

// ============================================================================
// [Fog::COORD_UNIT]
// ============================================================================

//! @brief Coordinate units that can be used by the @c Dpi and @c Font classes.
//!
//! Coordinate units can be used to create display independent graphics, keeping
//! the coordinates in device independent units and translating them into 
//! device pixel by Fog-G2d engine.
enum COORD_UNIT
{
  //! @brief No unit (compatible to @c COORD_UNIT_PX).
  COORD_UNIT_NONE = 0,
  //! @brief Pixel.
  COORD_UNIT_PX,

  //! @brief Point, 1 [pt] == 1/72 [in].
  COORD_UNIT_PT,
  //! @brief Pica, 1 [pc] == 12 [pt].
  COORD_UNIT_PC,

  //! @brief Inch, 1 [in] == 2.54 [cm].
  COORD_UNIT_IN,

  //! @brief Millimeter.
  COORD_UNIT_MM,
  //! @brief Centimeter.
  COORD_UNIT_CM,

  //! @brief Used for coordinates which depends to object bounding box.
  COORD_UNIT_PERCENT,

  //! @brief The font-size of the relevant font (see @c Font).
  COORD_UNIT_EM,
  //! @brief The x-height of the relevant font (see @c Font).
  COORD_UNIT_EX,

  //! @brief Count of coord units.
  COORD_UNIT_COUNT
};

// ============================================================================
// [Fog::DITHER_TYPE]
// ============================================================================

enum DITHER_TYPE
{
  DITHER_TYPE_NONE = 0,
  DITHER_TYPE_PATTERN = 1
};

// ============================================================================
// [Fog::FONT_FACE_TYPE]
// ============================================================================

//! @brief Type of font face (which engine created the font face instance).
enum FONT_FACE_TYPE
{
  //! @brief None face (this is dummy face that does nothing, created in case
  //! that something failed).
  FONT_FACE_NONE = 0,
  //! @brief Windows font face - HFONT.
  FONT_FACE_WINDOWS = 1,
  //! @brief FreeType font face - FtFace.
  FONT_FACE_FREETYPE = 2,
  //! @brief Mac font face.
  FONT_FACE_MAC = 3
};

// ============================================================================
// [Fog::FONT_FACE_FLAGS]
// ============================================================================

//! @brief Font face flags.
enum FONT_FACE_FLAGS
{
  //! @brief Font face is cached. This means that font glyphs and outlines
  //! can be stored in cache for later reuse.
  FONT_FACE_CACHED = 0x00000001,

  //! @brief Font face contains glyphs (raster images) that should be use to
  //! render the font on raster devices if not using transformations / scaling.
  //!
  //! @note Each font face supports glyphs.
  FONT_FACE_GLYPHS = 0x00000002,

  //! @brief Font face contains outlines (vectors). Text can be converted to
  //! path and then rendered.
  FONT_FACE_OUTLINES = 0x00000004,

  //! @brief Font contains kerning.
  FONT_FACE_KERNING = 0x00000008,

  //! @brief Whether the font matrics were quantized.
  FONT_FACE_QUANTIZED_METRICS = 0x00000010
};

// ============================================================================
// [Fog::FONT_STYLE_TYPE]
// ============================================================================

enum FONT_STYLE_TYPE
{
  FONT_STYLE_NORMAL = 0,
  FONT_STYLE_ITALIC = 1,
  FONT_STYLE_OBLIQUE = 2
};

// ============================================================================
// [Fog::FONT_WEIGHT]
// ============================================================================

enum FONT_WEIGHT
{
  FONT_WEIGHT_100 = 1,
  FONT_WEIGHT_200 = 2,
  FONT_WEIGHT_300 = 3,
  FONT_WEIGHT_400 = 4,
  FONT_WEIGHT_500 = 5,
  FONT_WEIGHT_600 = 6,
  FONT_WEIGHT_700 = 7,
  FONT_WEIGHT_800 = 8,
  FONT_WEIGHT_900 = 9,

  FONT_WEIGHT_THIN       = FONT_WEIGHT_100,
  FONT_WEIGHT_EXTRALIGHT = FONT_WEIGHT_200,
  FONT_WEIGHT_LIGHT      = FONT_WEIGHT_300,
  FONT_WEIGHT_NORMAL     = FONT_WEIGHT_400,
  FONT_WEIGHT_MEDIUM     = FONT_WEIGHT_500,
  FONT_WEIGHT_SEMIBOLD   = FONT_WEIGHT_600,
  FONT_WEIGHT_BOLD       = FONT_WEIGHT_700,
  FONT_WEIGHT_EXTRABOLD  = FONT_WEIGHT_800,
  FONT_WEIGHT_BLACK      = FONT_WEIGHT_900
};

// ============================================================================
// [Fog::FONT_DECORATION_FLAGS]
// ============================================================================

enum FONT_DECORATION_FLAGS
{
  FONT_DECORATION_NONE = 0x00,
  FONT_DECORATION_UNDERLINE = 0x01,
  FONT_DECORATION_STRIKE_THROUGH = 0x02
};

// ============================================================================
// [Fog::FONT_KERNING_TYPE]
// ============================================================================

enum FONT_KERNING_TYPE
{
  FONT_KERNING_NONE = 0x00,
  FONT_KERNING_ENABLED = 0x01,
  FONT_KERNING_DEFAULT = 0xFF
};

// ============================================================================
// [Fog::FONT_HINTING_TYPE]
// ============================================================================

enum FONT_HINTING_TYPE
{
  FONT_HINTING_NONE = 0x00,
  FONT_HINTING_ENABLED = 0x01,
  FONT_HINTING_METRICS = 0x02,
  FONT_HINTING_DEFAULT = 0xFF
};

// ============================================================================
// [Fog::FILL_RULE]
// ============================================================================

//! @brief Fill rule.
enum FILL_RULE
{
  //! @brief Fill using non-zero rule.
  FILL_RULE_NON_ZERO = 0,
  //! @brief Fill using even-odd rule.
  FILL_RULE_EVEN_ODD = 1,

  //! @brief Default fill-rule.
  FILL_RULE_DEFAULT = FILL_RULE_EVEN_ODD,
  //! @brief Used to catch invalid arguments.
  FILL_RULE_COUNT = 2
};

// ============================================================================
// [Fog::GEOMETRIC_PRECISION]
// ============================================================================

//! @brief The geometic precision hint.
enum GEOMETRIC_PRECISION
{
  //! @brief Standard geometric precision (default).
  //!
  //! Standard geometric precision uses single-precision or double-precision
  //! floating points. The usage of single-precision floating points is
  //! determined by input parameters. For example if shape that is being
  //! painted is given in single-precision floating point vectors, then
  //! single-precision floating point operations will be used to do the
  //! transformations, clipping and final painting.
  GEOMETRIC_PRECISION_NORMAL = 0,

  //! @brief High geometric precision.
  //!
  //! High geometric precision uses always double-precision floating points
  //! regardless of input arguments.
  //!
  //! @note High geometric precision may be very expensive when running on CPU
  //! without double-precision floating point unit.
  GEOMETRIC_PRECISION_HIGH = 1,

  GEOMETRIC_PRECISION_DEFAULT = GEOMETRIC_PRECISION_NORMAL,
  GEOMETRIC_PRECISION_COUNT = 2
};

// ============================================================================
// [Fog::GRADIENT_QUALITY]
// ============================================================================

//! @brief The color interpolation type, used by @c Painter.
enum GRADIENT_QUALITY
{
  //! @brief Normal interpolation quality (good for the most purposes).
  GRADIENT_QUALITY_NORMAL = 0,
  //! @brief High interpolation quality (slower, but more precise).
  GRADIENT_QUALITY_HIGH = 1,

  //! @brief Default interpolation quality, @c GRADIENT_QUALITY_NORMAL.
  GRADIENT_QUALITY_DEFAULT = GRADIENT_QUALITY_NORMAL,
  //! @brief Count of interpolation quality options.
  GRADIENT_QUALITY_COUNT = 2
};

// ============================================================================
// [Fog::GRADIENT_TYPE]
// ============================================================================

enum GRADIENT_TYPE
{
  //! @brief Linear gradient type.
  GRADIENT_TYPE_LINEAR = 0,
  //! @brief Radial gradient type.
  GRADIENT_TYPE_RADIAL = 1,
  //! @brief Conical gradient type.
  GRADIENT_TYPE_CONICAL = 2,
  //! @brief Rectangular gradient type.
  GRADIENT_TYPE_RECTANGULAR = 3,

  //! @brief Invalid gradient type, used internally.
  GRADIENT_TYPE_INVALID = 4,
  //! @brief Count of gradient types.
  GRADIENT_TYPE_COUNT = 4
};

// ============================================================================
// [Fog::IMAGE_ADOPT]
// ============================================================================

//! @brief Image adopt flags.
enum IMAGE_ADOPT
{
  //! @brief Standard adopt behavior
  IMAGE_ADOPT_DEFAULT = 0x00,
  //! @brief Adopted image will be read-only.
  IMAGE_ATOPT_READ_ONLY = 0x01,
  //! @brief Adopted image data are from bottom-to-top.
  //!
  //! Useful flag for Windows-DIB adoption.
  IMAGE_ADOPT_REVERSED = 0x02
};

// ============================================================================
// [Fog::IMAGE_BUFFER]
// ============================================================================

//! @brief Image type.
enum IMAGE_BUFFER
{
  //! @brief The image is a platform independent memory buffer.
  //!
  //! @note This is the default image type.
  IMAGE_BUFFER_MEMORY = 0,

  //! @brief The image is Windows-Dib (DIBSECTION).
  //!
  //! @note This is Windows-only image type.
  IMAGE_BUFFER_WIN_DIB = 1,

  //! @brief Count of image types.
  IMAGE_BUFFER_COUNT = 2,

  //! @brief Ignore image type (used by some functions inside @c Image).
  IMAGE_BUFFER_IGNORE = 0xFF
};

// ============================================================================
// [Fog::IMAGE_COMPONENT]
// ============================================================================

//! @brief Image components.
enum IMAGE_COMPONENT
{
  //! @brief No components.
  IMAGE_COMPONENT_NONE = 0x00000000,

  //! @brief Component mask.
  IMAGE_COMPONENT_MASK = 0x00000003,

  //! @brief Alpha component available.
  //!
  //! Related pixel formats:
  //!   - @c IMAGE_FORMAT_A8.
  //!   - @c IMAGE_FORMAT_A16.
  IMAGE_COMPONENT_ALPHA = 0x00000001,

  //! @brief RGB components available.
  //!
  //! Related pixel formats:
  //!   - @c IMAGE_FORMAT_XRGB32.
  //!   - @c IMAGE_FORMAT_RGB24.
  //!   - @c IMAGE_FORMAT_RGB48.
  IMAGE_COMPONENT_RGB = 0x00000002,

  //! @brief Image contains ARGB entities (RGB and ALPHA).
  //!
  //! Related pixel formats:
  //!   - @c IMAGE_FORMAT_PRGB32.
  //!   - @c IMAGE_FORMAT_PRGB64.
  IMAGE_COMPONENT_ARGB = IMAGE_COMPONENT_ALPHA | IMAGE_COMPONENT_RGB
};

// ============================================================================
// [Fog::IMAGE_FD_FLAGS]
// ============================================================================

enum IMAGE_FD_FLAGS
{
  IMAGE_FD_NONE,

  IMAGE_FD_IS_PREMULTIPLIED = 0x01,
  IMAGE_FD_IS_BYTESWAPPED = 0x02,
  IMAGE_FD_FILL_UNUSED_BITS = 0x04
};

// ============================================================================
// [Fog::IMAGE_FORMAT]
// ============================================================================

//! @brief Pixel format.
//!
//! @note The memory representation of nearly all pixel formats depends to the
//! CPU endianness.
//!
//! @c IMAGE_FORMAT_PRGB32, @c IMAGE_FORMAT_XRGB32:
//! - Memory       : 00|01|02|03
//! - Little endian: BB|GG|RR|AA
//! - Big endian   : AA|RR|GG|BB
//!
//! @c IMAGE_FORMAT_RGB24:
//! - Memory       : 00|01|02
//! - Little endian: BB|GG|RR
//! - Big endian   : RR|GG|BB
//!
//! @c IMAGE_FORMAT_A8:
//! - No difference: AA (8-bit alpha value, indexes to palette are also valid).
//!
//! @c IMAGE_FORMAT_I8:
//! - No difference: II (8-bit index value to palette)
//!
//! @c IMAGE_FORMAT_PRGB64
//! - Memory       : 00|01|02|03|04|05|06|07
//! - Little endian: B0|B1|G0|G1|R0|R1|A0|A1
//! - Big endian   : A1|A0|R1|R0|G1|G0|B1|B0
//!
//! @c IMAGE_FORMAT_RGB48
//! - Memory       : 00|01|02|03|04|05
//! - Little endian: B0|B1|G0|G1|R0|R1
//! - Big endian   : R1|R0|G1|G0|B1|B0
//!
//! @c IMAGE_FORMAT_A16
//! - Memory       : 00|01|
//! - Little endian: A0|A1|
//! - Big endian   : A1|A0|
//!
//! @note The terminology used for positions is the same as in the Fog-Face
//! framework:
//!
//! @verbatim
//! - Byte0: (Value >>  0) & 0xFF
//! - Byte1: (Value >>  8) & 0xFF
//! - Byte2: (Value >> 16) & 0xFF
//! - Byte3: (Value >> 24) & 0xFF
//! @endverbatim
enum IMAGE_FORMAT
{
  // --------------------------------------------------------------------------
  // NOTE: When changing order, removing or adding image format please match
  // ${IMAGE_FORMAT:BEGIN} -> ${IMAGE_FORMAT:END} and
  // fix the code that depends on image-format order and their meaning.
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [8-bit per component or less]
  // --------------------------------------------------------------------------

  //! @brief 32-bit ARGB (8 bits per component), premultiplied.
  IMAGE_FORMAT_PRGB32 = 0,
  //! @brief 32-bit RGB (8 bits per component).
  IMAGE_FORMAT_XRGB32 = 1,
  //! @brief 24-bit RGB (8 bits per component).
  IMAGE_FORMAT_RGB24 = 2,

  //! @brief 8-bit ALPHA.
  IMAGE_FORMAT_A8 = 3,
  //! @brief 8-bit INDEXED (RGB32 palette).
  IMAGE_FORMAT_I8 = 4,

  // --------------------------------------------------------------------------
  // [16-bit per component]
  // --------------------------------------------------------------------------

  //! @brief 64-bit ARGB (16 bits per component), premultiplied.
  IMAGE_FORMAT_PRGB64 = 5,
  //! @brief 48-bit RGB (16 bits per component).
  IMAGE_FORMAT_RGB48 = 6,
  //! @brief 16-bit ALPHA.
  IMAGE_FORMAT_A16 = 7,

  // --------------------------------------------------------------------------
  // [...]
  // --------------------------------------------------------------------------

  //! @brief Count of image formats.
  IMAGE_FORMAT_COUNT = 8,
  //! @brief Null image format (used only by empty images).
  IMAGE_FORMAT_NULL = IMAGE_FORMAT_COUNT,
  //! @brief Ignore image format (used by some functions inside the @c Image).
  IMAGE_FORMAT_IGNORE = IMAGE_FORMAT_COUNT
};

// ============================================================================
// [Fog::IMAGE_LIMITS]
// ============================================================================

//! @brief Image limits.
enum IMAGE_LIMITS
{
  //! @brief Maximum image width (65536 in pixels).
  IMAGE_MAX_WIDTH = 0x10000,
  //! @brief Maximum image height (65536 in pixels).
  IMAGE_MAX_HEIGHT = 0x10000
};

// ============================================================================
// [Fog::IMAGE_MIRROR_MODE]
// ============================================================================

//! @brief Mirror modes used together with @c Image::mirror().
enum IMAGE_MIRROR_MODE
{
  IMAGE_MIRROR_NONE       = 0x00,
  IMAGE_MIRROR_HORIZONTAL = 0x01,
  IMAGE_MIRROR_VERTICAL   = 0x02,
  IMAGE_MIRROR_BOTH       = 0x03
};

// ============================================================================
// [Fog::IMAGE_PRECISION]
// ============================================================================

enum IMAGE_PRECISION
{
  IMAGE_PRECISION_BYTE = 0,
  IMAGE_PRECISION_WORD = 1,

  IMAGE_PRECISION_COUNT = 2
};

// ============================================================================
// [Fog::IMAGE_ROTATE_MODE]
// ============================================================================

//! @brief Rotate modes used together with @c Image::rotate() methods.
enum IMAGE_ROTATE_MODE
{
  IMAGE_ROTATE_0   = 0x00,
  IMAGE_ROTATE_90  = 0x01,
  IMAGE_ROTATE_180 = 0x02,
  IMAGE_ROTATE_270 = 0x03
};

// ============================================================================
// [Fog::IMAGE_DATA_FLAGS]
// ============================================================================

//! @brief @c Image / @c ImageData flags.
enum IMAGE_DATA_FLAGS
{
  // --------------------------------------------------------------------------
  // [Core Flags]
  // --------------------------------------------------------------------------

  //! @brief Image data was adopted.
  IMAGE_DATA_STATIC = CONTAINER_DATA_STATIC,

  //! @brief Image is read-only. Fog must create a copy when write operation
  //! is performed (creating @c Painter instance, locking pixels, etc...)
  IMAGE_DATA_READ_ONLY = CONTAINER_DATA_READ_ONLY,

  // --------------------------------------------------------------------------
  // [Extended Flags]
  // --------------------------------------------------------------------------

  //! @brief Image is reversed (from bottom-to-top).
  //!
  //! @note This flag should be set only when adopting other image or
  //! DIBSECTION (Windows only). Fog itselt shouldn't create reversed images.
  IMAGE_DATA_REVERSED = 0x10
};








// TODO TODO TODO




// ============================================================================
// [Fog::IMAGE_EFFECT]
// ============================================================================

//! @brief Type of image filter, see @c ImageFxFilter and @c ColorFilter classes.
enum IMAGE_EFFECT
{
  //! @brief Image effect is none (COPY).
  IMAGE_EFFECT_NONE = 0,

  //! @brief @c Color adjust image effect.
  IMAGE_EFFECT_COLOR_ADJUST = 1,
  //! @brief @c Color LUT image effect.
  IMAGE_EFFECT_COLOR_LUT = 2,
  //! @brief @c Color matrix image effect.
  IMAGE_EFFECT_COLOR_MATRIX = 3,

  //! @brief Image filter is box blur.
  IMAGE_EFFECT_BLUR = 4,
  //! @brief Image filter is convolution (using convolution matrix).
  IMAGE_EFFECT_CONVOLVE_MATRIX = 5,
  //! @brief Image filter is convolution (using convolution vectors - horizontal and vertical).
  IMAGE_EFFECT_CONVOLVE_VECTOR = 6,

  // TODO: Image effects.
  IMAGE_EFFECT_EMBOSS = 7,
  IMAGE_EFFECT_MORPHOLOGY = 8,
  IMAGE_EFFECT_SHARPEN = 9,
  IMAGE_EFFECT_SHADOW = 10,

  IMAGE_EFFECT_COUNT = 11
};

// ============================================================================
// [Fog::IMAGE_EFFECT_CHAR]
// ============================================================================

//! @brief Characteristics of image filter.
//!
//! Characteristics can be used to improve performance of filters by @c Painter.
enum IMAGE_EFFECT_CHAR
{
  //! @brief Image filter does only color transformations.
  //!
  //! This flag must set all color filter, because it's very useful hint that
  //! enables very good code optimizations inside @c Painter and @c Image
  //! classes.
  IMAGE_EFFECT_CHAR_COLOR_TRANSFORM = 0x0001,

  //! @brief Image filter can extend image boundary (blur and convolution
  //! filters).
  IMAGE_EFFECT_CHAR_CAN_EXTEND = 0x0002,

  //! @brief Image filter constains standard processing mechanism - one pass.
  IMAGE_EFFECT_CHAR_ENTIRE_PROCESSING = 0x0004,

  //! @brief When doing entire processing the destination and source buffers
  //! can be shared (dst and src pointers can point to same location).
  IMAGE_EFFECT_CHAR_ENTIRE_MEM_EQUAL = 0x0008,

  //! @brief Image filter does vertical processing of image.
  //!
  //! This bit is set for all blur/convolution filters. Performance of filter
  //! is usually degraded, because filter processing function needs to access
  //!  pixels in different scanlines (cache misses, etc...).
  //!
  //! @note Vertical processing can be combined with horizontal processing and
  //! painter tries to make this combination efficient.
  IMAGE_EFFECT_CHAR_VERT_PROCESSING = 0x0010,

  //! @brief When doing vertical processing the destination and source buffers
  //! can be shared (dst and src pointers can point to same location).
  IMAGE_EFFECT_CHAR_VERT_MEM_EQUAL = 0x0020,

  //! @brief Image filter does horizontal processing of image.
  //!
  //! If filter needs only horizontal (no IMAGE_EFFECT_VERT_PROCESSING bit is
  //! set) then processing it can be very efficient in multithreaded painter
  //! engine.
  IMAGE_EFFECT_CHAR_HORZ_PROCESSING = 0x0040,

  //! @brief When doing vertical processing the destination and source buffers
  //! can be shared (dst and src pointers can point to same location).
  IMAGE_EFFECT_CHAR_HORZ_MEM_EQUAL = 0x0080,

  //! @brief Contains both, @c IMAGE_EFFECT_VERT_PROCESSING and
  //! @c IMAGE_EFFECT_HORZ_PROCESSING flags.
  IMAGE_EFFECT_CHAR_HV_PROCESSING =
    IMAGE_EFFECT_CHAR_VERT_PROCESSING |
    IMAGE_EFFECT_CHAR_HORZ_PROCESSING ,

  //! @brief Image filter supports @c IMAGE_FORMAT_PRGB32.
  IMAGE_EFFECT_CHAR_SUPPORTS_PRGB32 = 0x0100,

  //! @brief Image filter supports @c IMAGE_FORMAT_XRGB32.
  //!
  //! @note This flag should be always set!
  IMAGE_EFFECT_CHAR_SUPPORTS_XRGB32 = 0x0400,

  //! @brief Image filter supports @c IMAGE_FORMAT_A8.
  //!
  //! @note This flag should be always set!
  IMAGE_EFFECT_CHAR_SUPPORTS_A8 = 0x0800,

  //! @brief Image filter supports alpha-channel promotion. This means that
  //! source image without alpha-channel can be converted to image with
  //! alpha-channel.
  //!
  //! This operation is supported by all blur-filters (and should be supported
  //! generally by all filters that extend image boundary).
  IMAGE_EFFECT_CHAR_PROMOTE_ALPHA = 0x1000
};

// ============================================================================
// [Fog::BLUR_FX_TYPE]
// ============================================================================

//! @brief Type of blur, see @c ImageFxFilter.
enum BLUR_FX_TYPE
{
  //! @brief The linear-blur effect (default).
  //!
  //! The Linear-blur effect quality is between box-blur and gaussian-blur.
  //! The result and performance of this effect is optimal for the most
  //! operations.
  BLUR_FX_LINEAR = 0,

  //! @brief The box-blur effect.
  //!
  //! The box-blur effect is low-level quality blur, but very efficient.
  BLUR_FX_BOX = 1,

  //! @brief The gaussian-blur type.
  //!
  //! The gaussian-blur effect is high-quality blur, but computation intensive
  //! (poor performance).
  BLUR_FX_GAUSSIAN = 2,

  //! @brief The default blur type.
  BLUR_FX_DEFAULT = BLUR_FX_LINEAR,

  //! @brief Count of blur effects.
  BLUR_FX_COUNT = 3
};

//! @brief Maximum blur radius.
static const float BLUR_FX_MAX_RADIUS = 255.0f;

// ============================================================================
// [Fog::IMAGE_EFFECT_ALPHA_MODE]
// ============================================================================

enum IMAGE_EFFECT_ALPHA_MODE
{
  IMAGE_EFFECT_ALPHA_NORMAL,
  IMAGE_EFFECT_ALPHA_INCLUDE
};
























// ============================================================================
// [Fog::BORDER_EXTEND_TYPE]
// ============================================================================

//! @brief Border extend mode used by image effects (convolution and blurs).
enum BORDER_EXTEND_TYPE
{
  //! @brief Borders are extended by a color.
  BORDER_EXTEND_COLOR = 0,

  //! @brief Borders are extended using pad.
  BORDER_EXTEND_PAD = 1,

  //! @brief Borders are extended using repead.
  BORDER_EXTEND_REPEAT = 2,

  //! @brief Borders are extended using reflect.
  BORDER_EXTEND_REFLECT = 3,

  //! @brief Default border extend type.
  BORDER_EXTEND_DEFAULT = BORDER_EXTEND_COLOR,

  //! @brief Count of border extend types (for error checking).
  BORDER_EXTEND_COUNT = 4
};

// ============================================================================
// [Fog::IMAGE_CODEC_TYPE]
// ============================================================================

//! @brief Type of @c ImageCodec class.
enum IMAGE_CODEC_TYPE
{
  //! @brief None, null codec or non-initialized (shouldn't be used in public code).
  IMAGE_CODEC_NONE = 0x0,
  //! @brief Image encoder.
  IMAGE_CODEC_ENCODER = 0x1,
  //! @brief Image decoder.
  IMAGE_CODEC_DECODER = 0x2,
  //! @brief Image decoder and encoder.
  IMAGE_CODEC_BOTH = 0x3
};

// ============================================================================
// [Fog::IMAGE_STREAM_TYPE]
// ============================================================================

//! @brief Image file type.
enum IMAGE_STREAM_TYPE
{
  IMAGE_STREAM_NONE = 0,

  IMAGE_STREAM_ANI,
  IMAGE_STREAM_APNG,
  IMAGE_STREAM_BMP,
  IMAGE_STREAM_FLI,
  IMAGE_STREAM_FLC,
  IMAGE_STREAM_GIF,
  IMAGE_STREAM_ICO,
  IMAGE_STREAM_JPEG,
  IMAGE_STREAM_LBM,
  IMAGE_STREAM_MNG,
  IMAGE_STREAM_PCX,
  IMAGE_STREAM_PNG,
  IMAGE_STREAM_PNM,
  IMAGE_STREAM_TGA,
  IMAGE_STREAM_TIFF,
  IMAGE_STREAM_XBM,
  IMAGE_STREAM_XPM,

  IMAGE_STREAM_CUSTOM = 65536
};

// ============================================================================
// [Fog::IMAGE_QUALITY]
// ============================================================================

//! @brief Image interpolation type, used by @c Painter or @c Image::scale().
enum IMAGE_QUALITY
{
  IMAGE_QUALITY_NEAREST = 0,
  IMAGE_QUALITY_BILINEAR = 1,
  IMAGE_QUALITY_BILINEAR_HQ = 2,
  IMAGE_QUALITY_BICUBIC = 3,
  IMAGE_QUALITY_BICUBIC_HQ = 4,

  IMAGE_QUALITY_COUNT = 5,
  IMAGE_QUALITY_DEFAULT = IMAGE_QUALITY_BILINEAR
};

// ============================================================================
// [Fog::LINE_CAP]
// ============================================================================

//! @brief Line cap.
enum LINE_CAP
{
  LINE_CAP_BUTT = 0,
  LINE_CAP_SQUARE = 1,
  LINE_CAP_ROUND = 2,
  LINE_CAP_ROUND_REVERT = 3,
  LINE_CAP_TRIANGLE = 4,
  LINE_CAP_TRIANGLE_REVERT = 5,

  //! @brief Default line-cap type.
  LINE_CAP_DEFAULT = LINE_CAP_BUTT,
  //! @brief Used to catch invalid arguments.
  LINE_CAP_COUNT = 6
};

// ============================================================================
// [Fog::LINE_INTERSECTION]
// ============================================================================

//! @brief Line intersection result.
enum LINE_INTERSECTION
{
  LINE_INTERSECTION_NONE = 0,
  LINE_INTERSECTION_BOUNDED = 1,
  LINE_INTERSECTION_UNBOUNDED = 2
};

// ============================================================================
// [Fog::LINE_JOIN]
// ============================================================================

//! @brief Line join.
enum LINE_JOIN
{
  LINE_JOIN_MITER = 0,
  LINE_JOIN_ROUND = 1,
  LINE_JOIN_BEVEL = 2,
  LINE_JOIN_MITER_REVERT = 3,
  LINE_JOIN_MITER_ROUND = 4,

  //! @brief Default line-join type.
  LINE_JOIN_DEFAULT = LINE_JOIN_MITER,
  //! @brief Used to catch invalid arguments.
  LINE_JOIN_COUNT = 5
};

// ============================================================================
// [Fog::MASK_OP]
// ============================================================================

enum MASK_OP
{
  //! @brief Copy the source to the destination (SRC).
  MASK_OP_SRC = 0,
  //! @brief Intersect the source with the destination (MULTIPLY).
  MASK_OP_INTERSECT = 1,

  //! @brief Count of mask operators.
  MASK_OP_COUNT = 2
};

// ============================================================================
// [Fog::MATRIX_ORDER]
// ============================================================================

//! @brief Matrix multiply ordering.
enum MATRIX_ORDER
{
  //! @brief The second matrix which is multiplied with the primary matrix is
  //! on the left (default for all graphics / color matrix operations).
  MATRIX_ORDER_PREPEND = 0,

  //! @brief The second matrix which is multiplied with the primary matrix is
  //! on the right.
  MATRIX_ORDER_APPEND = 1
};

// ============================================================================
// [Fog::PAINTER_FLUSH]
// ============================================================================

//! @brief Painter flush flags.
enum PAINTER_FLUSH
{
  //! @brief Flush all painter commands and wait for completition. Use this
  //! command if you want to access image data after the @c flush() call.
  PAINTER_FLUSH_SYNC = 0x00000001
};

// ============================================================================
// [Fog::PAINTER_INIT]
// ============================================================================

//! @brief Painter initialization flags.
enum PAINTER_INIT
{
  //! @brief Crear the content of the painter to transparent color if target
  //! buffer contains alpha channel or to black (if target buffer not contains
  //! alpha channel).
  //!
  //! Using this flag can lead to very optimized painting and it's generally
  //! faster than setting painter to @c COMPOSITE_SRC and clearing the content
  //! by using @c Painter::fillAll() or @c Painter::fillRect() methods. This
  //! method is efficient, because painter will mark image region as transparent
  //! and then use that information to perform copy operator to the
  //! span that hasn't been initialized yet.
  PAINTER_INIT_CLEAR = 0x00000001,

  //! @brief Initialize multithreading if it makes sense.
  //!
  //! If this option is true, painter first check if image size is not too
  //! small (painting to small images are singlethreaded by default). Then
  //! CPU detection is used to check if machine contains more CPUs or cores.
  PAINTER_INIT_MT = 0x00000002
};

// ============================================================================
// [Fog::PAINTER_MAP]
// ============================================================================

//! @brief Painter point options.
enum PAINTER_MAP
{
  PAINTER_MAP_WORLD_TO_SCREEN = 0,
  PAINTER_MAP_SCREEN_TO_WORLD = 1,

  //! @brief Count of map options.
  PAINTER_MAP_COUNT = 2
};

// ============================================================================
// [Fog::PAINTER_TYPE]
// ============================================================================

//! @brief The type of painter.
enum PAINTER_TYPE
{
  //! @brief The null painter type (painter is not initialized or invalid).
  PAINTER_TYPE_NULL = 0,

  //! @brief The raster painter type.
  PAINTER_TYPE_RASTER = 1
};

// ============================================================================
// [Fog::PAINTER_PARAMETER]
// ============================================================================

enum PAINTER_PARAMETER
{
  // --------------------------------------------------------------------------
  // [Engine]
  // --------------------------------------------------------------------------

  PAINTER_PARAMETER_SIZE_I = 0,
  PAINTER_PARAMETER_SIZE_F = 1,
  PAINTER_PARAMETER_SIZE_D = 2,

  PAINTER_PARAMETER_FORMAT_I = 3,
  PAINTER_PARAMETER_ENGINE_I = 4,

  // --------------------------------------------------------------------------
  // [Multithreading]
  // --------------------------------------------------------------------------

  PAINTER_PARAMETER_MULTITHREADED_I = 5,
  PAINTER_PARAMETER_MAX_THREADS_I = 6,

  // --------------------------------------------------------------------------
  // [Paint Params]
  // --------------------------------------------------------------------------

  PAINTER_PARAMETER_PARAMS_F = 7,
  PAINTER_PARAMETER_PARAMS_D = 8,

  // --------------------------------------------------------------------------
  // [Paint Hints]
  // --------------------------------------------------------------------------

  PAINTER_PARAMETER_PAINT_HINTS = 9,

  //! @brief Compositing operator.
  PAINTER_PARAMETER_COMPOSITING_OPERATOR_I = 10,

  //! @brief Antialiasing quality, see @c ANTIALIASING_QUALITY.
  PAINTER_PARAMETER_ANTIALIASING_QUALITY_I = 11,

  //! @brief Image interpolation quality, see @c IMAGE_QUALITY.
  PAINTER_PARAMETER_IMAGE_QUALITY_I = 12,

  //! @brief Gradient interpolation quality, see @c GRADIENT_QUALITY.
  PAINTER_PARAMETER_GRADIENT_QUALITY_I = 13,

  //! @brief Whether to render text using path-outlines only.
  PAINTER_PARAMETER_OUTLINED_TEXT_I = 14,

  //! @brief Whether to maximize the performance of rendering lines, possibly
  //! degrading their visual quality.
  //!
  //! @note The quality of rendered lines must be related to antialiasing quality.
  //! This means that using fast-lines flag never turns antialiasing off, only
  //! the mask computation can be simplified.
  PAINTER_PARAMETER_FAST_LINE_I = 15,

  //! @brief Whether to maximize the geometric precision of vector coordinates
  //! and transformations.
  PAINTER_PARAMETER_GEOMETRIC_PRECISION_I = 16,

  // --------------------------------------------------------------------------
  // [Paint Opacity]
  // --------------------------------------------------------------------------

  PAINTER_PARAMETER_OPACITY_F = 17,
  PAINTER_PARAMETER_OPACITY_D = 18,

  // --------------------------------------------------------------------------
  // [Fill Params]
  // --------------------------------------------------------------------------

  PAINTER_PARAMETER_FILL_RULE_I = 19,

  // --------------------------------------------------------------------------
  // [Stroke Params]
  // --------------------------------------------------------------------------

  PAINTER_PARAMETER_STROKE_PARAMS_F = 20,
  PAINTER_PARAMETER_STROKE_PARAMS_D = 21,

  PAINTER_PARAMETER_LINE_WIDTH_F = 22,
  PAINTER_PARAMETER_LINE_WIDTH_D = 23,

  PAINTER_PARAMETER_LINE_JOIN_I = 24,
  PAINTER_PARAMETER_START_CAP_I = 25,
  PAINTER_PARAMETER_END_CAP_I = 26,
  PAINTER_PARAMETER_LINE_CAPS_I = 27,

  PAINTER_PARAMETER_MITER_LIMIT_F = 28,
  PAINTER_PARAMETER_MITER_LIMIT_D = 29,

  PAINTER_PARAMETER_DASH_OFFSET_F = 30,
  PAINTER_PARAMETER_DASH_OFFSET_D = 31,

  PAINTER_PARAMETER_DASH_LIST_F = 32,
  PAINTER_PARAMETER_DASH_LIST_D = 33,

  // --------------------------------------------------------------------------
  // [...]
  // --------------------------------------------------------------------------

  //! @brief Count of painter parameters.
  PAINTER_PARAMETER_COUNT = 34
};

// ============================================================================
// [Fog::PAINTER_SOURCE]
// ============================================================================

enum PAINTER_SOURCE
{
  PAINTER_SOURCE_TEXTURE_F = 0,
  PAINTER_SOURCE_TEXTURE_D = 1,
  PAINTER_SOURCE_GRADIENT_F = 2,
  PAINTER_SOURCE_GRADIENT_D = 3,
  PAINTER_SOURCE_COUNT = 4
};

// ============================================================================
// [Fog::PATH_CMD]
// ============================================================================

//! @brief Path commands.
enum PATH_CMD
{
  //! @brief Move-to command.
  PATH_CMD_MOVE_TO = 0,
  //! @brief Line-to command.
  PATH_CMD_LINE_TO = 1,
  //! @brief Quad-to command.
  PATH_CMD_QUAD_TO = 2,
  //! @brief Cubic-to command.
  PATH_CMD_CUBIC_TO = 3,
  // TODO: Not implemented yet!
  //
  //! @brief Command used to distunguish between the additional data needed for
  //! @c PATH_CMD_QUAD_TO and @c PATH_CMD_CUBIC_TO.
  PATH_CMD_DATA = 4,
  //! @brief Close command.
  PATH_CMD_CLOSE = 5
};

// ============================================================================
// [Fog::PATH_DATA]
// ============================================================================

enum PATH_DATA
{
  PATH_DATA_STATIC = CONTAINER_DATA_STATIC,

  PATH_DATA_DIRTY_BOUNDING_BOX = 0x0010,
  PATH_DATA_DIRTY_CMD = 0x0020,

  PATH_DATA_HAS_BOUNDING_BOX = 0x0040,
  PATH_DATA_HAS_QUAD_CMD = 0x0080,
  PATH_DATA_HAS_CUBIC_CMD = 0x0100,

  PATH_DATA_OWN_FLAGS = 
    PATH_DATA_DIRTY_BOUNDING_BOX | 
    PATH_DATA_DIRTY_CMD          |
    PATH_DATA_HAS_BOUNDING_BOX   |
    PATH_DATA_HAS_QUAD_CMD       |
    PATH_DATA_HAS_CUBIC_CMD
};

// ============================================================================
// [Fog::PATH_DIRECTION]
// ============================================================================

//! @brief Sub-path direction.
enum PATH_DIRECTION
{
  //! @brief No direction specified.
  PATH_DIRECTION_NONE = 0,
  //! @brief Clockwise direction.
  PATH_DIRECTION_CW = 1,
  //! @brief Counter-clockwise direction.
  PATH_DIRECTION_CCW = 2
};

// ============================================================================
// [Fog::PATTERN_TYPE]
// ============================================================================

//! @brief Type of pattern in the @c PatternF or @c PatternD instance.
enum PATTERN_TYPE
{
  //! @brief Null pattern (nothing will be paint using this pattern).
  PATTERN_TYPE_NONE = 0,
  //! @brief Solid color pattern.
  PATTERN_TYPE_COLOR = 1,
  //! @brief Texture pattern (@c Texture).
  PATTERN_TYPE_TEXTURE = 2,
  //! @brief Gradient pattern (@c GradientF or @c GradientD).
  PATTERN_TYPE_GRADIENT = 3
};

// ============================================================================
// [Fog::REGION_HIT_TEST]
// ============================================================================

//! @brief Region hit-test result.
enum REGION_HIT_TEST
{
  //! @brief Object isn't in region (point, rectangle or another region).
  REGION_HIT_OUT = 0,
  //! @brief Object is in region (point, rectangle or another region).
  REGION_HIT_IN = 1,
  //! @brief Object is partially in region (point, rectangle or another region).
  REGION_HIT_PART = 2
};

// ============================================================================
// [Fog::REGION_OP]
// ============================================================================

//! @brief Region or clip combining operators.
enum REGION_OP
{
  //! @brief Replace (COPY).
  REGION_OP_REPLACE = 0,
  //! @brief Intersection (AND).
  REGION_OP_INTERSECT = 1,
  //! @brief Union (OR)
  REGION_OP_UNION = 2,
  //! @brief eXclusive or (XOR).
  REGION_OP_XOR = 3,
  //! @brief Subtraction (DIFF).
  REGION_OP_SUBTRACT = 4,

  //! @brief Count of region operators.
  REGION_OP_COUNT
};

// ============================================================================
// [Fog::REGION_TYPE]
// ============================================================================

//! @brief Type of @c Region.
enum REGION_TYPE
{
  // NOTE: Never change value of REGION_TYPE_EMPTY and REGION_TYPE_SIMPLE
  // constants, see Region::getType() method in Region.cpp file.

  //! @brief Region is empty.
  REGION_TYPE_EMPTY = 0,
  //! @brief Region has only one rectangle (rectangular).
  REGION_TYPE_SIMPLE = 1,
  //! @brief Region has more YX sorted rectangles.
  REGION_TYPE_COMPLEX = 2,
  //! @brief Region is infinite (special region type).
  REGION_TYPE_INFINITE = 3
};

// ============================================================================
// [Fog::SHAPE_TYPE]
// ============================================================================

//! @brief Type of primitive shape, see @c ShapeF and @c ShapeD classes.
enum SHAPE_TYPE
{
  SHAPE_TYPE_NONE = 0,

  // --------------------------------------------------------------------------
  // [Unclosed]
  // --------------------------------------------------------------------------

  SHAPE_TYPE_LINE = 1,
  SHAPE_TYPE_QUAD = 2,
  SHAPE_TYPE_CUBIC = 3,
  SHAPE_TYPE_ARC = 4,

  // --------------------------------------------------------------------------
  // [Closed]
  // --------------------------------------------------------------------------

  SHAPE_TYPE_RECT = 5,
  SHAPE_TYPE_ROUND = 6,
  SHAPE_TYPE_CIRCLE = 7,
  SHAPE_TYPE_ELLIPSE = 8,
  SHAPE_TYPE_CHORD = 9,
  SHAPE_TYPE_PIE = 10,

  // --------------------------------------------------------------------------
  // [Count]
  // --------------------------------------------------------------------------

  SHAPE_TYPE_COUNT = 11
};

// ============================================================================
// [Fog::SPREAD]
// ============================================================================

//! @brief Spread type.
enum SPREAD
{
  //! @brief Pad spread (area outside the shape continues using border color).
  GRADIENT_SPREAD_PAD = 0,
  //! @brief Releat spread (pattern is repeated).
  GRADIENT_SPREAD_REPEAT = 1,
  //! @brief Reflect spread (pattern is reflected and then repeated).
  GRADIENT_SPREAD_REFLECT = 2,

  //! @brief Default spread.
  GRADIENT_SPREAD_DEFAULT = GRADIENT_SPREAD_PAD,
  //! @brief Count of spread types.
  GRADIENT_SPREAD_COUNT = 3
};

// ============================================================================
// [Fog::SPAN]
// ============================================================================

//! @brief Type of @c Span.
enum SPAN
{
  // --------------------------------------------------------------------------
  // NOTE: When changing these constants, please make sure that the span methods
  // like isConst()/isVariant() are also changed. There are some optimizations
  // which are based on these values.
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [Const]
  // --------------------------------------------------------------------------

  //! @brief Span is a const-mask.
  SPAN_C = 0,

  // --------------------------------------------------------------------------
  // [Variant]
  // --------------------------------------------------------------------------

  //! @brief Start of variant-alpha span types (may be used by asserts).
  //!
  //! @note This constant is only shadow to valid mask type, don't use this
  //! value in switch() {}.
  SPAN_V_BEGIN = 1,

  //! @brief Span is a variable-alpha mask (8-bit, A8).
  //!
  //! @sa @c SPAN_AX_EXTRA.
  SPAN_A8_GLYPH = 1,

  //! @brief Span is a variable-alpha mask (target bit-depth, A8, A16 or A32).
  //!
  //! @sa @c SPAN_AX_EXTRA.
  SPAN_AX_GLYPH = 2,

  //! @brief Span is an extended variable-alpha mask (target bit-depth + 1 bit
  //! for exact scaling).
  SPAN_AX_EXTRA = 3,

  //! @brief Span is a variable-argb mask (8-bit, PRGB32).
  SPAN_ARGB32_GLYPH = 4,

  //! @brief Span is a variable-argb mask (target bit-depth, PRGB32 or PRGB64).
  SPAN_ARGBXX_GLYPH = 5,

  //! @brief The count of span types.
  SPAN_COUNT = 6
};

// ============================================================================
// [Fog::TEXT_ALIGN]
// ============================================================================

//! @brief Text alignment
enum TEXT_ALIGN
{
  TEXT_ALIGN_LEFT        = 0x01,
  TEXT_ALIGN_RIGHT       = 0x02,
  TEXT_ALIGN_HCENTER     = 0x03,
  TEXT_ALIGN_HMASK       = 0x03,

  TEXT_ALIGN_TOP         = 0x10,
  TEXT_ALIGN_BOTTOM      = 0x20,
  TEXT_ALIGN_VCENTER     = 0x30,
  TEXT_ALIGN_VMASK       = 0x30,

  TEXT_ALIGN_CENTER      = TEXT_ALIGN_VCENTER | TEXT_ALIGN_HCENTER
};

// ============================================================================
// [Fog::TEXT_HINT]
// ============================================================================

enum TEXT_HINT
{
  TEXT_HINT_PRECISE = 0,
  TEXT_HINT_ALIGNED_VERTICAL = 1,
  TEXT_HINT_ALIGNED_BOTH = 2
};

// ============================================================================
// [Fog::TEXTURE_TILE]
// ============================================================================

//! @brief Texture tiling mode (see @c Texture).
enum TEXTURE_TILE
{
  TEXTURE_TILE_PAD = 0,
  TEXTURE_TILE_REPEAT = 1,
  TEXTURE_TILE_REFLECT = 2,
  TEXTURE_TILE_CLAMP = 3,

  TEXTURE_TILE_DEFAULT = TEXTURE_TILE_REPEAT,
  TEXTURE_TILE_COUNT = 4
};

// ============================================================================
// [Fog::TRANSFORM_CREATE]
// ============================================================================

//! @brief Type of matrix to create.
enum TRANSFORM_CREATE
{
  TRANSFORM_CREATE_IDENTITY = 0,
  TRANSFORM_CREATE_TRANSLATION = 1,
  TRANSFORM_CREATE_SCALING = 2,
  TRANSFORM_CREATE_ROTATION = 3,
  TRANSFORM_CREATE_SKEWING = 4,
  TRANSFORM_CREATE_LINE_SEGMENT = 5,
  TRANSFORM_CREATE_REFLECTION_U = 6,
  TRANSFORM_CREATE_REFLECTION_XY = 7,
  TRANSFORM_CREATE_REFLECTION_UNIT = 8,
  TRANSFORM_CREATE_PARALLELOGRAM = 9,
  TRANSFORM_CREATE_QUAD_TO_QUAD = 10,

  TRANSFORM_CREATE_COUNT = 11
};

// ============================================================================
// [Fog::TRANSFORM_OP]
// ============================================================================

//! @brief Type of transform operation.
enum TRANSFORM_OP
{
  //! @brief Translate matrix.
  TRANSFORM_OP_TRANSLATE = 0,
  //! @brief Scale matrix.
  TRANSFORM_OP_SCALE = 1,
  //! @brief Rotate matrix.
  TRANSFORM_OP_ROTATE = 2,
  //! @brief Rotate matrix (about a point).
  TRANSFORM_OP_ROTATE_PT = 3,
  //! @brief Skew matrix.
  TRANSFORM_OP_SKEW = 4,
  //! @brief Flip matrix.
  TRANSFORM_OP_FLIP = 5,
  //! @brief Multiply with other matrix.
  TRANSFORM_OP_MULTIPLY = 6,
  //! @brief Multiply with other matrix, but invert it before multiplication.
  TRANSFORM_OP_MULTIPLY_INV = 7,

  //! @brief Count of matrix transform operations.
  TRANSFORM_OP_COUNT = 8
};

// ============================================================================
// [Fog::TRANSFORM_TYPE]
// ============================================================================

//! @brief Type of transform.
enum TRANSFORM_TYPE
{
  //! @brief Transform type is identity (all zeros, 1 at diagonals).
  TRANSFORM_TYPE_IDENTITY = 0,
  //! @brief Transform type is translation (_20, _21 elements are used).
  TRANSFORM_TYPE_TRANSLATION = 1,
  //! @brief Transform type is scaling (_00, _11, _20, _21 elements are used).
  TRANSFORM_TYPE_SCALING = 2,
  //! @brief Transform type is swap (_01, _10, _20, _21 elements are used).
  TRANSFORM_TYPE_SWAP = 3,
  //! @brief Transform type is rotation (affine part is used).
  TRANSFORM_TYPE_ROTATION = 4,
  //! @brief Transform type is affine.
  TRANSFORM_TYPE_AFFINE = 5,
  //! @brief Transform type is projection.
  TRANSFORM_TYPE_PROJECTION = 6,
  //! @brief Transform type is degenerate (same as projection, but degenerated).
  TRANSFORM_TYPE_DEGENERATE = 7,

  //! @brief Count of transform types (for asserts, ...).
  TRANSFORM_TYPE_COUNT = 8,

  //! @brief Matrix is dirty.
  TRANSFORM_TYPE_DIRTY = 0x8
};

// ============================================================================
// [Fog::ERR_GRAPHICS]
// ============================================================================

//! @brief Error codes used in Fog-Graphics.
enum ERR_GRAPHICS_ENUM
{
  // --------------------------------------------------------------------------
  // [Errors Range]
  // --------------------------------------------------------------------------

  // TODO: Increase this range.

  //! @brief First error code that can be used by Fog-Graphics library.
  ERR_GRAPHICS_FIRST = 0x00011100,
  //! @brief Last error code that can be used by Fog-Graphics library.
  ERR_GRAPHICS_LAST  = 0x000111FF,

  // --------------------------------------------------------------------------
  // [Imaging]
  // --------------------------------------------------------------------------

  //! @brief Image size is invalid or zero.
  ERR_IMAGE_INVALID_SIZE = ERR_GRAPHICS_FIRST,

  //! @brief Image format is invalid.
  ERR_IMAGE_INVALID_FORMAT,

  // TODO: Remove
  ERR_IMAGE_UNSUPPORTED_FORMAT,

  ERR_IMAGEIO_INTERNAL_ERROR,

  ERR_IMAGE_NO_DECODER,
  ERR_IMAGE_NO_ENCODER,

  ERR_IMAGEIO_UNSUPPORTED_FORMAT,
  ERR_IMAGE_TERMINATED,
  ERR_IMAGE_TRUNCATED,

  ERR_IMAGE_MIME_NOT_MATCH,
  ERR_IMAGE_MALFORMED_HEADER,
  ERR_IMAGE_MALFORMED_RLE,
  ERR_IMAGE_NO_MORE_FRAMES,

  ERR_IMAGE_LIBJPEG_NOT_LOADED,
  ERR_IMAGE_LIBJPEG_ERROR,

  ERR_IMAGE_LIBPNG_NOT_LOADED,
  ERR_IMAGE_LIBPNG_ERROR,

  //! @brief Failed to load Gdi+ library (Windows).
  ERR_IMAGE_GDIPLUS_NOT_LOADED,

  //! @brief Call to Gdi+ library failed and resulting error can't be
  //! translated to the Fog one. This is a generic error.
  ERR_IMAGE_GDIPLUS_ERROR,

  // --------------------------------------------------------------------------
  // [Font]
  // --------------------------------------------------------------------------

  ERR_FONT_NOT_MATCHED,
  ERR_FONT_INVALID_FACE,
  ERR_FONT_INVALID_DATA,
  ERR_FONT_CANT_LOAD_DEFAULT_FACE,
  ERR_FONT_CANT_GET_OUTLINE,

  ERR_FONT_FONTCONFIG_NOT_LOADED,
  ERR_FONT_FONTCONFIG_INIT_FAILED,

  ERR_FONT_FREETYPE_NOT_LOADED,
  ERR_FONT_FREETYPE_INIT_FAILED,

  // --------------------------------------------------------------------------
  // [Geometry]
  // --------------------------------------------------------------------------

  //! @brief No shape to work with.
  //!
  //! This may happen when some method is requested on an empty path or shape
  //! that is @c SHAPE_TYPE_NONE. If this error is returned then the output
  //! of the functions shouldn't be considered as correct. For example the
  //! bounding-box of shape which is invalid will be [0, 0, 0, 0], but it
  //! shouldn't be used/merged or analyzed, because the shape doesn't exist.
  ERR_GEOMETRY_NONE,

  //! @brief Invalid shape.
  //!
  //! This may happen if @c PathF or @c PathD contains invalid data (although
  //! this might be considered as the run-time error) or if some basic shape
  //! is incorrectly given (for example if @c RectF or @c RectD with or height
  //! is negative).
  ERR_GEOMETRY_INVALID,
  
  //! @brief The transform to be used is degenerated.
  //!
  //! The degenerated transform can't be used in geometry, because the result
  //! is simply nothing - for example rectange might degenerate to rectangle
  //! with zero width or height and it won't be painted. To prevent errors
  //! caused by degenerate transform use @c TransformF::isDegenerate() and
  //! @c TransformD::isDegenerate().
  //!
  //! @note This error is always related to trasnform.
  ERR_GEOMETRY_DEGENERATE,

  // --------------------------------------------------------------------------
  // [Painter]
  // --------------------------------------------------------------------------

  //! @brief Image format is not supported by the paint engine.
  //!
  //! The error can be misleading, but it can be returned by @c Painter::begin()
  //! where the image argument points to image that is using @c IMAGE_FORMAT_I8.
  //!
  //! Painter can be used only to paint into supported pixel formats (all RGB,
  //! ARGB and ALPHA-only pixel formats are supported).
  ERR_PAINTER_UNSUPPORTED_FORMAT,

  //! @brief Error that can be returned by painter if it's illegal to change
  //! paint engine at this time.
  //!
  //! This can happen if multithreaded or hardware accelerated painter is used
  //! and you want to switch to a different paint engine.
  //!
  //! @sa @c Painter::setEngine().
  ERR_PAINTER_NOT_ALLOWED,

  //! @brief There is not state to restore, might be caused by
  //! @ref Painter::restore().
  ERR_PAINTER_NO_STATE,

  // --------------------------------------------------------------------------
  // [Path]
  // --------------------------------------------------------------------------

  // TODO: Merge with ERR_GEOMETRY section.

  //! @brief It is required that a previous path command is a vertex,
  //!
  //! To add @c PATH_CMD_LINE_TO, @c PATH_CMD_QUAD_TO, @c PATH_CMD_CUBIC_TO or
  //! @c PATH_CMD_CLOSE the previous command must be a vertex. The only command
  //! that do not require such condition is @c PATH_CMD_MOVE_TO.
  ERR_PATH_NO_VERTEX,

  //! @brief The relative command can't be added, because the previous command
  //! is not a vertex.
  ERR_PATH_NO_RELATIVE,

  //! @brief Can't stroke path that contains only one vertex (probably
  //! @c PATH_CMD_MOVE_TO).
  ERR_PATH_CANT_STROKE,

  // --------------------------------------------------------------------------
  // [Region]
  // --------------------------------------------------------------------------

  //! @brief Can't do binary algebra using infinite region as a source or
  //! destination.
  //!
  //! The algebra is possible, but resulting region could be very large and
  //! all other operations can be insecure. This can happen if you want to
  //! XOR simple or complex region by infinite one.
  ERR_REGION_INFINITE
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_GLOBAL_CONSTANTS_H
