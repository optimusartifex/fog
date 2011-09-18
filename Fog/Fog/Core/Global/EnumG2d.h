// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_ENUMG2D_H
#define _FOG_CORE_GLOBAL_ENUMG2D_H

// [Dependencies]
#include <Fog/Core/Global/EnumCore.h>

namespace Fog {

//! @addtogroup Fog_Core_Global
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
// [Fog::AXIS]
// ============================================================================

//! @brief Axis.
enum AXIS
{
  //! @brief X axis.
  AXIS_X = 0x1,
  //! @brief Y axis.
  AXIS_Y = 0x2,
  //! @brief Z axis.
  AXIS_Z = 0x4
};

// ============================================================================
// [Fog::CLIP_OP]
// ============================================================================

//! @brief Clip operation used by @c Painter::clip() and PaintEngine::clip()
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
// [Fog::COLOR_ADJUST_OP]
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
// [Fog::COLOR_MODEL]
// ============================================================================

//! @brief Color model used by @c Color class.
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

//! @brief Contains flags which might be used by @c Color.parse().
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
  COLOR_MIX_ALPHA_PRESERVE = 0,

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
// [Fog::DITHER_TYPE]
// ============================================================================

enum DITHER_TYPE
{
  DITHER_TYPE_NONE = 0,
  DITHER_TYPE_PATTERN = 1
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
// [Fog::FONT_ALIGN_MODE]
// ============================================================================

enum FONT_ALIGN_MODE
{
  FONT_ALIGN_MODE_NONE = 0x00,
  FONT_ALIGN_MODE_X = 0x01,
  FONT_ALIGN_MODE_Y = 0x02,
  FONT_ALIGN_MODE_XY = 0x03,
  FONT_ALIGN_MODE_HEIGHT = 0x04,

  FONT_ALIGN_MODE_DETECT = 0xFF
};

// ============================================================================
// [Fog::FONT_DATA]
// ============================================================================

enum FONT_DATA
{
  //! @brief The font properties were resolved - the height is in pixels and
  //! all properties were successfully detected (hinting).
  //!
  //! Physical font can be used by the layout manager and the result matches
  //! the device pixels and other requirements to layout the font-glyphs
  //! correctly (for example using quantized hinting and kerning).
  FONT_DATA_IS_PHYSICAL = 0x01,

  //! @brief The size of font-face is aligned.
  FONT_DATA_IS_ALIGNED = 0x02,

  //! @brief The font properties contains custom letter-spacing.
  FONT_DATA_HAS_LETTER_SPACING = 0x04,

  //! @brief The font properties contains custom word-spacing.
  FONT_DATA_HAS_WORD_SPACING = 0x08,

  //! @brief The font properties contains user transform.
  FONT_DATA_HAS_TRANSFORM = 0x10
};

// ============================================================================
// [Fog::FONT_DECORATION]
// ============================================================================

enum FONT_DECORATION
{
  FONT_DECORATION_NONE = 0x00,
  FONT_DECORATION_UNDERLINE = 0x01,
  FONT_DECORATION_STRIKE_THROUGH = 0x02
};

// ============================================================================
// [Fog::FONT_FEATURE]
// ============================================================================

enum FONT_FEATURE
{
  //! @brief Font contains raster-based glyphs.
  //!
  //! @note May be combined with @c FONT_FEATURE_OUTLINE.
  FONT_FEATURE_RASTER = 0x00000001,

  //! @brief Font contains outlined glyphs.
  //!
  //! @note May be combined with @c FONT_FEATURE_RASTER.
  FONT_FEATURE_OUTLINE = 0x00000002,

  //! @brief Font supports kerning.
  FONT_FEATURE_KERNING = 0x00000004,

  //! @brief Font supports hinting.
  FONT_FEATURE_HINTING = 0x00000008,

  //! @brief Font supports LCD quality of rendering.
  FONT_FEATURE_LCD_QUALITY = 0x00000010
};

// ============================================================================
// [Fog::FONT_FACE]
// ============================================================================

//! @brief Type of font-face.
enum FONT_FACE
{
  //! @brief Null font-face (dummy face that is used if something failed or on-demand).
  FONT_FACE_NULL = 0,
  //! @brief Windows font-face (HFONT instance).
  FONT_FACE_WINDOWS = 1,
  //! @brief Mac font-face.
  FONT_FACE_MAC = 2,
  //! @brief Freetype font-face (FT_Face).
  FONT_FACE_FREETYPE = 3,

  //! @brief Count of font-faces.
  FONT_FACE_COUNT = 4
};

// ============================================================================
// [Fog::FONT_FAMILY]
// ============================================================================

//! @brief Standard font family IDs (defined by CSS).
enum FONT_FAMILY
{
  //! @brief "serif" font.
  //!
  //! Glyphs of serif fonts, as the term is used in CSS, tend to have finishing
  //! strokes, flared or tapering ends, or have actual serifed endings (including
  //! slab serifs). Serif fonts are typically proportionately-spaced. They often
  //! display a greater variation between thick and thin strokes than fonts from
  //! the "sans-serif" generic font family. CSS uses the term "serif" to apply to
  //! a font for any script, although other names may be more familiar for
  //! particular scripts, such as Mincho (Japanese), Sung or Song (Chinese),
  //! Totum or Kodig (Korean). Any font that is so described may be used to
  //! represent the generic "serif" family.
  //!
  //! Latin fonts
  //!   - Times New Roman,
  //!   - Bodoni, Garamond,
  //!   - Minion Web,
  //!   - ITC Stone Serif,
  //!   - MS Georgia,
  //!   - Bitstream Cyberbit.
  //!
  //! Greek fonts
  //!   - Bitstream Cyberbit.
  //!
  //! Cyrillic fonts
  //!   - Adobe Minion Cyrillic,
  //!   - Excelsior Cyrillic Upright,
  //!   - Monotype Albion 70,
  //!   - Bitstream Cyberbit,
  //!   - ER Bukinist.
  //!
  //! Hebrew fonts
  //!   - New Peninim,
  //!   - Raanana,
  //!   - Bitstream Cyberbit.
  //!
  //! Japanese fonts
  //!   - Ryumin Light-KL,
  //!   - Kyokasho ICA,
  //!   - Futo Min A101.
  //!
  //! Arabic fonts
  //!   - Bitstream Cyberbit.
  FONT_FAMILY_SERIF = 0,

  //! @brief "sans-serif" font.
  //!
  //! Glyphs in sans-serif fonts, as the term is used in CSS, tend to have
  //! stroke endings that are plain - with little or no flaring, cross stroke,
  //! or other ornamentation. Sans-serif fonts are typically proportionately-
  //! spaced. They often have little variation between thick and thin strokes,
  //! compared to fonts from the "serif" family. CSS uses the term 'sans-serif'
  //! to apply to a font for any script, although other names may be more
  //! familiar for particular scripts, such as Gothic (Japanese), Kai (Chinese),
  //! or Pathang (Korean). Any font that is so described may be used to
  //! represent the generic ?sans-serif" family.
  //!
  //! Latin fonts:
  //!   - MS Trebuchet,
  //!   - ITC Avant Garde Gothic,
  //!   - MS Arial,
  //!   - MS Verdana,
  //!   - Univers,
  //!   - Futura,
  //!   - ITC Stone Sans,
  //!   - Gill Sans,
  //!   - Akzidenz Grotesk,
  //!   - Helvetica.
  //!
  //! Greek fonts
  //!   - Attika,
  //!   - Typiko New Era,
  //!   - MS Tahoma,
  //!   - Monotype Gill Sans 571,
  //!   - Helvetica Greek.
  //!
  //! Cyrillic fonts
  //!   - Helvetica Cyrillic,
  //!   - ER Univers,
  //!   - Lucida Sans Unicode,
  //!   - Bastion.
  //!
  //! Hebrew fonts
  //!   - Arial Hebrew,
  //!   - MS Tahoma.
  //!
  //! Japanese fonts
  //!   - Shin Go,
  //!   - Heisei Kaku Gothic W5.
  //!
  //! Arabic fonts
  //!   - MS Tahoma.
  FONT_FAMILY_SANS_SERIF = 1,

  //! @brief "cursive" font.
  //!
  //! Glyphs in cursive fonts, as the term is used in CSS, generally have either
  //! joining strokes or other cursive characteristics beyond those of italic
  //! typefaces. The glyphs are partially or completely connected, and the
  //! result looks more like handwritten pen or brush writing than printed
  //! letterwork. Fonts for some scripts, such as Arabic, are almost always
  //! cursive. CSS uses the term 'cursive' to apply to a font for any script,
  //! although other names such as Chancery, Brush, Swing and Script are also
  //! used in font names.
  //!
  //! Latin fonts:
  //!   - Caflisch Script,
  //!   - Adobe Poetica,
  //!   - Sanvito,
  //!   - Ex Ponto,
  //!   - Snell Roundhand,
  //!   - Zapf-Chancery.
  //!
  //! Cyrillic fonts:
  //!   - ER Architekt.
  //!
  //! Hebrew fonts:
  //!   - Corsiva.
  //!
  //! Arabic fonts:
  //!   - DecoType Naskh,
  //!   - Monotype Urdu 507.
  FONT_FAMILY_CUSRIVE = 2,

  //! @brief "fantasy" font.
  //!
  //! Fantasy fonts, as used in CSS, are primarily decorative while still
  //! containing representations of characters (as opposed to Pi or Picture
  //! fonts, which do not represent characters).
  //!
  //! Latin fonts:
  //! - Alpha Geometrique,
  //! - Critter,
  //! - Cottonwood,
  //! - FB Reactor,
  //! - Studz.
  FONT_FAMILY_FANTASY = 3,

  //! @brief "monospace" font.
  //!
  //! The sole criterion of a monospace font is that all glyphs have the same
  //! fixed width. (This can make some scripts, such as Arabic, look most
  //! peculiar.) The effect is similar to a manual typewriter, and is often
  //! used to set samples of computer code.
  //!
  //! Latin fonts:
  //!   - Courier,
  //!   - Courier New,
  //!   - Lucida Console,
  //!   - Monaco.
  //!
  //! Greek fonts:
  //!   - MS Courier New,
  //!   - Everson Mono.
  //!
  //! Cyrillic fonts:
  //!   - ER Kurier, Everson Mono.
  //!
  //! Japanese fonts:
  //!   - Osaka Monospaced.
  FONT_FAMILY_MONOSPACE = 4,

  //! @brief Count of font-family IDs.
  FONT_FAMILY_COUNT = 5,

  //! @brief Unknown font (not categorized family).
  FONT_FAMILY_UNKNOWN = 0xFF
};

// ============================================================================
// [Fog::FONT_HINTING]
// ============================================================================

//! @brief Font-hinting mode.
enum FONT_HINTING
{
  //! @brief Font-hinting is disabled.
  FONT_HINTING_DISABLED = 0,
  //! @brief Font-hinting is enabled.
  FONT_HINTING_ENABLED = 1,

  //! @brief Detect font-hinting.
  FONT_HINTING_DETECT = 0xFF
};

// ============================================================================
// [Fog::FONT_KERNING]
// ============================================================================

//! @brief Font-kerning mode.
enum FONT_KERNING
{
  //! @brief Disable the use of kerning.
  FONT_KERNING_DISABLED = 0,
  //! @brief Enable the use of kerning (default).
  FONT_KERNING_ENABLED = 1,

  //! @brief Detect font-kerning.
  FONT_KERNING_DETECT = 0xFF
};

// ============================================================================
// [Fog::FONT_ORDER]
// ============================================================================

//! @brief Font-provider order.
enum FONT_ORDER
{
  //! @brief Prepend the provider (add it as the first, taking the highest
  //! relevance).
  FONT_ORDER_FIRST = 0,

  //! @brief Append the provider (add it as the last item, taking the lowest
  //! relevance).
  FONT_ORDER_LAST = 1
};

// ============================================================================
// [Fog::FONT_PROVIDER]
// ============================================================================

//! @brief Font-provider IDs.
enum FONT_PROVIDER
{
  //! @brief Null font-provider (only for compatibility with @c FONT_FACE, never created).
  FONT_PROVIDER_NULL = 0,
  //! @brief Windows font-provider.
  FONT_PROVIDER_WINDOWS = 1,
  //! @brief Mac font-provider.
  FONT_PROVIDER_MAC = 2,
  //! @brief Freetype font-provider which use fontconfig.
  FONT_PROVIDER_FT_FONTCONFIG = 3,
  //! @brief Freetype font-provider which use own, minimalist provider.
  FONT_PROVIDER_FT_MINIMALIST = 4,
  //! @brief Freetype font-provider (including fontconfig support if available).
  //! @brief Custom font-provider (SVG/CSS/Others...).
  FONT_PROVIDER_CUSTOM = 5,

  //! @brief Count of font-providers.
  FONT_PROVIDER_COUNT = 6
};

// ============================================================================
// [Fog::FONT_QUALITY]
// ============================================================================

//! @brief Font quality.
enum FONT_QUALITY
{
  //! @brief No antialiasing.
  FONT_QUALITY_ALIASED = 0,
  //! @brief Grey antialiasing.
  FONT_QUALITY_GREY = 1,
  //! @brief LCD subpixel antialiasing (only enabled for the LCD output device).
  FONT_QUALITY_LCD = 2,

  //! @brief Use default font quality.
  FONT_QUALITY_DETECT = 0xFF,
  //! @brief Count of font-quality options.
  FONT_QUALITY_COUNT = 3
};

// ============================================================================
// [Fog::FONT_SPACING_MODE]
// ============================================================================

//! @brief Font spacing mode.
enum FONT_SPACING_MODE
{
  //! @brief Spacing is percentage (in Fog 0.0 to 1.0, inclusive) of the glyph
  //! spacing.
  FONT_SPACING_MODE_PERCENTAGE = 0,

  //! @brief Spacing is absolute, in font units.
  FONT_SPACING_MODE_ABSOLUTE = 1,

  //! @brief Count of font spacing modes.
  FONT_SPACING_MODE_COUNT = 2
};

// ============================================================================
// [Fog::FONT_STYLE]
// ============================================================================

//! @brief Font style.
enum FONT_STYLE
{
  //! @brief Normal style.
  FONT_STYLE_NORMAL = 0,

  //! @brief Oblique (slanted, sloped) style.
  //!
  //! Form of type that slants slightly to the right, using shearing transform
  //! of original glyph-set.
  FONT_STYLE_OBLIQUE = 1,

  //! @brief Italic style.
  //!
  //! Form of type that slants slightly to the right, using different glyph-set.
  //! If glyph-set for italic is not available, the @c FONT_STYLE_OBLIQUE is
  //! used instead.
  FONT_STYLE_ITALIC = 2,

  //! @brief Count of font styles.
  FONT_STYLE_COUNT = 3
};

// ============================================================================
// [Fog::FONT_VARIANT]
// ============================================================================

//! @brief Font variant.
enum FONT_VARIANT
{
  //! @brief Normal font-variant.
  FONT_VARIANT_NORMAL = 0,
  //! @brief Small letters are capitalized, but their size is lowered.
  FONT_VARIANT_SMALL_CAPS = 1,

  FONT_VARIANT_COUNT = 2
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
// [Fog::GRADIENT_SPREAD]
// ============================================================================

//! @brief Spread type.
enum GRADIENT_SPREAD
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
// [Fog::IMAGE_CODEC]
// ============================================================================

//! @brief Type of @c ImageCodec class.
enum IMAGE_CODEC
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
//! @note The memory representation of nearly all pixel formats depends on the
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
//! - No difference: AA (8-bit alpha value, indices to palette are also valid).
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
//! @note The terminology used for position is the same as in the @c Fog-Face
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
// [Fog::IMAGE_STREAM]
// ============================================================================

//! @brief Image file type.
enum IMAGE_STREAM
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

  IMAGE_QUALITY_DEFAULT = IMAGE_QUALITY_BILINEAR,
  IMAGE_QUALITY_COUNT = 5
};

// ============================================================================
// [Fog::LCD_ORDER]
// ============================================================================

//! @brief Order of RGB components of LCD display.
enum LCD_ORDER
{
  //! @brief Order is unknown or the device is not the LCD display.
  LCD_ORDER_NONE = 0,
  //! @brief Horizontal R-G-B order.
  LCD_ORDER_HRGB = 1,
  //! @brief Horizontal B-G-R order.
  LCD_ORDER_HBGR = 2,
  //! @brief Vertical R-G-B order.
  LCD_ORDER_VRGB = 3,
  //! @brief Vertical B-G-R order.
  LCD_ORDER_VBGR = 4,

  //! @brief Count of LCD order options.
  LCD_ORDER_COUNT = 5
};

// ============================================================================
// [Fog::LINE_CAP]
// ============================================================================

//! @brief Line-cap.
enum LINE_CAP
{
  //! @brief Butt line-cap (default).
  LINE_CAP_BUTT = 0,
  //! @brief Square line-cap.
  LINE_CAP_SQUARE = 1,
  //! @brief Round line-cap.
  LINE_CAP_ROUND = 2,
  //! @brief Reversed round line-cap.
  LINE_CAP_ROUND_REVERSE = 3,
  //! @brief Triangle line-cap.
  LINE_CAP_TRIANGLE = 4,
  //! @brief Reversed triangle line-cap.
  LINE_CAP_TRIANGLE_REVERSE = 5,

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
  //! @brief Miter line-join (default).
  LINE_JOIN_MITER = 0,
  //! @brief Round line-join.
  LINE_JOIN_ROUND = 1,
  //! @brief Bevel line-join.
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
  MASK_OP_REPLACE = 0,
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
  //! method is efficient, because the painter can mark an image region as
  //! transparent and then use that hint to perform fast source-copy
  //! compositing on that region, instead of doing regular composition.
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
  //! @brief Map user coordinates to device coordinates.
  PAINTER_MAP_USER_TO_DEVICE = 0,
  //! @brief Map device coordinates to user coordinates.
  PAINTER_MAP_DEVICE_TO_USER = 1,

  //! @brief Count of map operations.
  PAINTER_MAP_COUNT = 2
};

// ============================================================================
// [Fog::PAINT_DEVICE
// ============================================================================

//! @brief Type of the paint-device.
enum PAINT_DEVICE
{
  //! @brief Null paint-device (not initialized or invalid).
  PAINT_DEVICE_NULL = 0,
  //! @brief Same as @c PAINT_DEVICE_NULL.
  PAINT_DEVICE_UNKNOWN = 0,

  //! @brief Raster paint-device.
  PAINT_DEVICE_RASTER = 1,

  //! @brief Count of paint-device IDs.
  PAINT_DEVICE_COUNT = 2
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
  PAINTER_PARAMETER_DEVICE_I = 4,

  // --------------------------------------------------------------------------
  // [Multithreading]
  // --------------------------------------------------------------------------

  PAINTER_PARAMETER_MULTITHREADED_I = 5,
  PAINTER_PARAMETER_MAX_THREADS_I = 6,

  // --------------------------------------------------------------------------
  // [Paint Params]
  // --------------------------------------------------------------------------

  PAINTER_PARAMETER_PAINT_PARAMS_F = 7,
  PAINTER_PARAMETER_PAINT_PARAMS_D = 8,

  // --------------------------------------------------------------------------
  // [Paint Hints]
  // --------------------------------------------------------------------------

  PAINTER_PARAMETER_PAINT_HINTS = 9,

  //! @brief Compositing operator.
  PAINTER_PARAMETER_COMPOSITING_OPERATOR_I = 10,

  //! @brief Render quality, see @c RENDER_QUALITY.
  PAINTER_PARAMETER_RENDER_QUALITY_I = 11,

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
// [Fog::PATH_CLIPPER_MEASURE]
// ============================================================================

enum PATH_CLIPPER_MEASURE
{
  //! @brief Source path is bounded to the clip-box (not needed to clip).
  PATH_CLIPPER_MEASURE_BOUNDED = 0,
  //! @brief Source path is not bounded to the clip-box (needed to clip).
  PATH_CLIPPER_MEASURE_UNBOUNDED = 1,
  //! @brief Source path is invalid.
  PATH_CLIPPER_MEASURE_INVALID = 2
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
// [Fog::PATH_FLAG]
// ============================================================================

enum PATH_FLAG
{
  PATH_FLAG_DIRTY_BBOX  = VAR_FLAG_RESERVED_1,
  PATH_FLAG_DIRTY_CMD   = VAR_FLAG_RESERVED_2,
  PATH_FLAG_DIRTY_INFO  = VAR_FLAG_RESERVED_3,

  PATH_FLAG_HAS_BBOX    = VAR_FLAG_RESERVED_4,
  PATH_FLAG_HAS_QBEZIER = VAR_FLAG_RESERVED_5,
  PATH_FLAG_HAS_CBEZIER = VAR_FLAG_RESERVED_6,

  PATH_FLAG_MASK        = PATH_FLAG_DIRTY_BBOX  |
                          PATH_FLAG_DIRTY_CMD   |
                          PATH_FLAG_DIRTY_INFO  |
                          PATH_FLAG_HAS_BBOX    |
                          PATH_FLAG_HAS_QBEZIER |
                          PATH_FLAG_HAS_CBEZIER
};

// ============================================================================
// [Fog::PATH_FLATTEN]
// ============================================================================

//! @brief Path flattening option.
enum PATH_FLATTEN
{
  //! @brief Flattening is disabled.
  //!
  //!
  PATH_FLATTEN_DISABLED = 0,

  //! @brief Flatten all cubic Bezier curves to quadratic Bezier curves.
  //!
  //! Resulting path may contain lines or quadratic Bezier curves. This option
  //! is designed for backends which only support quadratic bezier curves, so
  //! all cubics are flattened by Stroker/Clipper.
  PATH_FLATTEN_QBEZIER = 1,

  //! @brief Flatten all cubic and quadratic Bezier curves to lines.
  //!
  //! Resulting path may contain only lines. This option is usable for
  //! backends which support only line-segments which can form a triangles,
  //! trapezoids, or polygons.
  PATH_FLATTEN_LINE = 2,

  //! @brief Count of path flattening options.
  PATH_FLATTEN_COUNT = 3
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
// [Fog::PIXEL_ARGB32_POS]
// ============================================================================

enum PIXEL_ARGB32_POS
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  PIXEL_ARGB32_POS_A = 3,
  PIXEL_ARGB32_POS_R = 2,
  PIXEL_ARGB32_POS_G = 1,
  PIXEL_ARGB32_POS_B = 0
#else // FOG_BYTE_ORDER == FOG_BIG_ENDIAN
  PIXEL_ARGB32_POS_A = 0,
  PIXEL_ARGB32_POS_R = 1,
  PIXEL_ARGB32_POS_G = 2,
  PIXEL_ARGB32_POS_B = 3
#endif // FOG_BYTE_ORDER
};

// ============================================================================
// [Fog::PIXEL_ARGB32_MASK]
// ============================================================================

static const uint32_t PIXEL_ARGB32_MASK_A = 0xFF000000U;
static const uint32_t PIXEL_ARGB32_MASK_R = 0x00FF0000U;
static const uint32_t PIXEL_ARGB32_MASK_G = 0x0000FF00U;
static const uint32_t PIXEL_ARGB32_MASK_B = 0x000000FFU;

// ============================================================================
// [Fog::PIXEL_ARGB32_SHIFT]
// ============================================================================

enum PIXEL_ARGB32_SHIFT
{
  PIXEL_ARGB32_SHIFT_A = 24U,
  PIXEL_ARGB32_SHIFT_R = 16U,
  PIXEL_ARGB32_SHIFT_G =  8U,
  PIXEL_ARGB32_SHIFT_B =  0U
};

// ============================================================================
// [Fog::PIXEL_ARGB64_POS]
// ============================================================================

enum PIXEL_ARGB64_POS
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  PIXEL_ARGB64_POS_A = 3,
  PIXEL_ARGB64_POS_R = 2,
  PIXEL_ARGB64_POS_G = 1,
  PIXEL_ARGB64_POS_B = 0
#else // FOG_BYTE_ORDER == FOG_BIG_ENDIAN
  PIXEL_ARGB64_POS_A = 0,
  PIXEL_ARGB64_POS_R = 1,
  PIXEL_ARGB64_POS_G = 2,
  PIXEL_ARGB64_POS_B = 3
#endif // FOG_BYTE_ORDER
};

// ============================================================================
// [Fog::PIXEL_ARGB64_MASK]
// ============================================================================

static const uint64_t PIXEL_ARGB64_MASK_A = FOG_UINT64_C(0xFFFF000000000000);
static const uint64_t PIXEL_ARGB64_MASK_R = FOG_UINT64_C(0x0000FFFF00000000);
static const uint64_t PIXEL_ARGB64_MASK_G = FOG_UINT64_C(0x00000000FFFF0000);
static const uint64_t PIXEL_ARGB64_MASK_B = FOG_UINT64_C(0x000000000000FFFF);

// ============================================================================
// [Fog::PIXEL_ARGB64_SHIFT]
// ============================================================================

enum PIXEL_ARGB64_SHIFT
{
  PIXEL_ARGB64_SHIFT_A = 48U,
  PIXEL_ARGB64_SHIFT_R = 32U,
  PIXEL_ARGB64_SHIFT_G = 16U,
  PIXEL_ARGB64_SHIFT_B =  0U
};

// ============================================================================
// [Fog::PIXEL_RGB24_POS]
// ============================================================================

enum PIXEL_RGB24_POS
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  PIXEL_RGB24_POS_R = 2,
  PIXEL_RGB24_POS_G = 1,
  PIXEL_RGB24_POS_B = 0
#else // FOG_BYTE_ORDER == FOG_BIG_ENDIAN
  PIXEL_RGB24_POS_R = 0,
  PIXEL_RGB24_POS_G = 1,
  PIXEL_RGB24_POS_B = 2
#endif // FOG_BYTE_ORDER
};

// ============================================================================
// [Fog::PIXEL_RGB48_POS]
// ============================================================================

enum PIXEL_RGB48_POS
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  PIXEL_RGB48_POS_R = 2,
  PIXEL_RGB48_POS_G = 1,
  PIXEL_RGB48_POS_B = 0
#else // FOG_BYTE_ORDER == FOG_BIG_ENDIAN
  PIXEL_RGB48_POS_R = 0,
  PIXEL_RGB48_POS_G = 1,
  PIXEL_RGB48_POS_B = 2
#endif // FOG_BYTE_ORDER
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

//! @brief Region combining operators.
enum REGION_OP
{
  //! @brief Replacement (B).
  REGION_OP_REPLACE = 0,
  //! @brief Intersection (A & B).
  REGION_OP_INTERSECT = 1,
  //! @brief Union (A + B).
  REGION_OP_UNION = 2,
  //! @brief Xor (A ^ B).
  REGION_OP_XOR = 3,
  //! @brief Subtraction (A - B).
  REGION_OP_SUBTRACT = 4,
  //! @brief Reverse subtraction (B - A).
  REGION_OP_SUBTRACT_REV = 5,

  //! @brief Count of region operators.
  REGION_OP_COUNT = 6
};

// ============================================================================
// [Fog::REGION_TYPE]
// ============================================================================

//! @brief Type of @c Region.
enum REGION_TYPE
{
  // NOTE: Never change value of REGION_TYPE_EMPTY and REGION_TYPE_RECT
  // constants, see Region::getType() method in Region.cpp file.

  //! @brief Region is empty.
  REGION_TYPE_EMPTY = 0,
  //! @brief Region has only one rectangle (rectangular).
  REGION_TYPE_RECT = 1,
  //! @brief Region has more YX sorted rectangles.
  REGION_TYPE_COMPLEX = 2,
  //! @brief Region is infinite (special region type).
  REGION_TYPE_INFINITE = 3
};

// ============================================================================
// [Fog::RENDER_QUALITY]
// ============================================================================

//! @brief Render quality.
enum RENDER_QUALITY
{
  //! @brief Aliased (disabled antialiasing).
  RENDER_QUALITY_ALIASED = 0,

  //! @brief Use at least 4 shades of grey for antialiasing.
  //!
  //! This is only recommendation for paint-engine. It can use higher render
  //! quality if this option is not supported.
  RENDER_QUALITY_GREY_4 = 1,

  //! @brief Use at least 8 shades of grey for antialiasing.
  //!
  //! This is only recommendation for paint-engine. It can use higher or lower
  //! render quality if this option is not supported.
  RENDER_QUALITY_GREY_8 = 2,

  //! @brief Use at least 16 shades of grey for antialiasing.
  //!
  //! This is only recommendation for paint-engine. It can use higher or lower
  //! render quality if this option is not supported.
  RENDER_QUALITY_GREY_16 = 3,

  //! @brief Use at least 32 shades of grey for antialiasing.
  //!
  //! This is only recommendation for paint-engine. It can use higher or lower
  //! render quality if this option is not supported.
  RENDER_QUALITY_GREY_32 = 4,

  //! @brief Use at least 64 shades of grey for antialiasing.
  //!
  //! This is only recommendation for paint-engine. It can use higher or lower
  //! render quality if this option is not supported.
  RENDER_QUALITY_GREY_64 = 5,

  //! @brief Use at least 256 shades of grey for antialiasing when rendering
  //! for 8-bit target and 65536 shades of grey when rendering for 16-bit
  //! target.
  //!
  //! This is only recommendation for paint-engine. It can use higher or lower
  //! render quality if this option is not supported.
  RENDER_QUALITY_GREY_HQ = 6,

  //! @brief LCD subpixel antialiasing.
  //!
  //! This type of antialiasing is usually only implemented for font rendering.
  RENDER_QUALITY_LCD = 7,

  //! @brief Count of render quality settings (for error checking).
  RENDER_QUALITY_COUNT = 8,

  //! @brief Default rendering quality (synonym to @c RENDER_QUALITY_GREY_16).
  RENDER_QUALITY_DEFAULT = RENDER_QUALITY_GREY_16
};

// ============================================================================
// [Fog::SHAPE_TYPE]
// ============================================================================

// TODO: Remove

//! @brief Type of primitive shape, see @c ShapeF and @c ShapeD classes.
enum SHAPE_TYPE
{
  SHAPE_TYPE_NONE = 0,

  // --------------------------------------------------------------------------
  // [Unclosed]
  // --------------------------------------------------------------------------

  SHAPE_TYPE_LINE = 1,
  SHAPE_TYPE_QBEZIER = 2,
  SHAPE_TYPE_CBEZIER = 3,
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
  SHAPE_TYPE_TRIANGLE = 11,

  // --------------------------------------------------------------------------
  // [Count]
  // --------------------------------------------------------------------------

  SHAPE_TYPE_COUNT = 12
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
// [Fog::UNIT]
// ============================================================================

//! @brief Coordinate units that can be used by the @c Dpi and @c Font classes.
//!
//! Coordinate units can be used to create display independent graphics, keeping
//! the coordinates in device independent units and translating them into
//! device pixel by Fog-G2d engine.
enum UNIT
{
  //! @brief No unit (compatible to @c UNIT_PX).
  UNIT_NONE = 0,
  //! @brief Pixel.
  UNIT_PX,

  //! @brief Point, 1 [pt] == 1/72 [in].
  UNIT_PT,
  //! @brief Pica, 1 [pc] == 12 [pt].
  UNIT_PC,

  //! @brief Inch, 1 [in] == 2.54 [cm].
  UNIT_IN,

  //! @brief Millimeter.
  UNIT_MM,
  //! @brief Centimeter.
  UNIT_CM,

  //! @brief Used for coordinates which depends on the object bounding box.
  UNIT_PERCENTAGE,

  //! @brief The font-size of the relevant font (see @c Font).
  UNIT_EM,
  //! @brief The x-height of the relevant font (see @c Font).
  UNIT_EX,

  //! @brief Count of coord units.
  UNIT_COUNT
};






// TODO: Unfinished.






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

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_GLOBAL_ENUMG2D_H
