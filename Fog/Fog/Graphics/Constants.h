// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_CONSTANTS_H
#define _FOG_GRAPHICS_CONSTANTS_H

// [Dependencies]
#include <Fog/Build/Build.h>

namespace Fog {

// ============================================================================
// [Fog::ANTI_ALIASING_TYPE]
// ============================================================================

//! @brief Anti-aliasing type.
enum ANTI_ALIASING_TYPE
{
  //! @brief No anti-aliasing.
  ANTI_ALIASING_NONE = 0,
  //! @brief Smooth anti-aliasing (default).
  ANTI_ALIASING_SMOOTH = 1,

  //! @brief Count of anti-aliasing types (for error checking).
  ANTI_ALIASING_COUNT = 2
};

// ============================================================================
// [Fog::COLOR_CHANNEL_TYPE]
// ============================================================================

//! @brief Argb color channels.
enum COLOR_CHANNEL_TYPE
{
  COLOR_CHANNEL_RED = 0x1,
  COLOR_CHANNEL_GREEN = 0x2,
  COLOR_CHANNEL_BLUE = 0x4,
  COLOR_CHANNEL_ALPHA = 0x8,
  COLOR_CHANNEL_RGB = COLOR_CHANNEL_RED | COLOR_CHANNEL_GREEN | COLOR_CHANNEL_BLUE,
  COLOR_CHANNEL_ARGB = COLOR_CHANNEL_RGB | COLOR_CHANNEL_ALPHA,

  COLOR_CHANNEL_COUNT = COLOR_CHANNEL_ARGB + 1
};

// ============================================================================
// [Fog::DPI_VALUE_TYPE]
// ============================================================================

//! @brief Type of values that can be used together by the @c Dpi class.
enum DPI_VALUE_TYPE
{
  DPI_VALUE_CM = 0,
  DPI_VALUE_EM,
  DPI_VALUE_EX,
  DPI_VALUE_IN,
  DPI_VALUE_MM,
  DPI_VALUE_PC,
  DPI_VALUE_PT,
  DPI_VALUE_PX,

  //! @brief Count of DPI value units.
  DPI_VALUE_COUNT
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
// [Fog::FONT_WEIGHT_TYPE]
// ============================================================================

enum FONT_WEIGHT_TYPE
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

  FONT_WEIGHT_THIN = FONT_WEIGHT_100,
  FONT_WEIGHT_EXTRALIGHT = FONT_WEIGHT_200,
  FONT_WEIGHT_LIGHT = FONT_WEIGHT_300,
  FONT_WEIGHT_NORMAL = FONT_WEIGHT_400,
  FONT_WEIGHT_MEDIUM = FONT_WEIGHT_500,
  FONT_WEIGHT_SEMIBOLD = FONT_WEIGHT_600,
  FONT_WEIGHT_BOLD = FONT_WEIGHT_700,
  FONT_WEIGHT_EXTRABOLD = FONT_WEIGHT_800,
  FONT_WEIGHT_BLACK = FONT_WEIGHT_900
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
// [Fog::OPERATOR_TYPE]
// ============================================================================

//! @brief Image compositing operator.
//!
//! @note Many values and formulas are from antigrain and from SVG specification
//! that can be found here: 
//! - http://www.w3.org/TR/2004/WD-SVG12-20041027/rendering.html
enum OPERATOR_TYPE
{
  // --------------------------------------------------------------------------
  // [SRC]
  // --------------------------------------------------------------------------

  //! @brief The source is copied to the destination.
  //!
  //! The source pixel is copied to destination pixel. If destination pixel 
  //! format not supports alpha channel, the source alpha value is ignored. 
  //! If there is alpha channel mask the composition is done using LERP 
  //! operator.
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SRC):
  //!   Dca' = Sca.Da + Sca.(1 - Da)
  //!        = Sca
  //!   Da'  = Sa.Da + Sa.(1 - Da)
  //!        = Sa
  //!
  //!   Msk:
  //!
  //!   Dca' = Sca.m + Dca.(1 - m)
  //!   Da'  = Sa.m + Da.(1 - m)
  //!
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (SRC):
  //!   Dca' = Sc
  //!   Da'  = 1
  //!
  //!   Msk:
  //!
  //!   Dca' = Sc.m + Dca.(1 - m)
  //!   Da'  = 1.m + Da.(1 - m)
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (SRC):
  //!   Dc'  = Sca
  //!
  //!   Msk:
  //!
  //!   Dc'  = Sca.m + Dc.(1 - m)
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (SRC):
  //!   Dc'  = Sc
  //!
  //!   Msk:
  //!
  //!   Dc'  = Sc.m + Dc.(1 - m)
  //!
  //! Formulas for A(dst), A(src) colorspaces (SRC):
  //!   Da'  = Sa
  //!
  //!   Msk:
  //!
  //!   Da'  = Sa.m + Da.(1 - m)
  OPERATOR_SRC = 0,

  // --------------------------------------------------------------------------
  // [DST]
  // --------------------------------------------------------------------------

  //! @brief The destination is left untouched.
  //!
  //! Destination pixels remains unchanged.
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (NOP):
  //!   Dca' = Dca
  //!   Da'  = Da
  //!
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (NOP):
  //!   Dca' = Dca
  //!   Da'  = Da
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (NOP):
  //!   Dc'  = Dc
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (NOP):
  //!   Dc'  = Dc
  //!
  //! Formulas for A(dst), A(src) colorspaces (NOP):
  //!   Da'  = Da
  OPERATOR_DST,

  // --------------------------------------------------------------------------
  // [SRC_OVER]
  // --------------------------------------------------------------------------

  //! @brief The source is composited over the destination.
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SRC_OVER):
  //!   Dca' = Sca.Da + Sca.(1 - Da) + Dca.(1 - Sa)
  //!        = Sca + Dca.(1 - Sa)
  //!   Da'  = Sa.Da + Sa.(1 - Da) + Da.(1 - Sa)
  //!        = Sa + Da.(1 - Sa)
  //!        = Sa + Da - Sa.Da
  //!
  //!   Msk:
  //!
  //!   Dca' = (Sca + Dca.(1 - Sa)).m + Dca.(1 - m)
  //!        = Sca.m + Dca.(1 - Sa.m)
  //!   Da'  = (Sa + Da.(1 - Sa)).m + Da .(1 - m)
  //!        = Sa.m + Da.(1 - Sa.m)
  //!
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (SRC):
  //!   Dca' = Sc
  //!   Da'  = 1
  //!
  //!   Msk:
  //!
  //!   Dca' = Sc.m + Dca.(1 - m)
  //!   Da'  = 1.m + Da.(1 - m)
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (SRC_OVER):
  //!   Dc'  = Sca + Dc.(1 - Sa)
  //!
  //!   Msk:
  //!
  //!   Da'  = Sca.m + Dc.(1 - Sa.m)
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (SRC):
  //!   Dc'  = Sc
  //!
  //!   Msk:
  //!
  //!   Da'  = Sc.m + Dc.(1 - m)
  //!
  //! Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  //!   Da'  = Da + Sa.(1 - Da)
  //!
  //!   Msk:
  //!
  //!   Da'  = Da + Sa.m.(1 - Da)
  //!
  //! If the source is full opaque (Sa == 1.0 or there is no alpha), it 
  //! replaces the destination.
  OPERATOR_SRC_OVER,

  // --------------------------------------------------------------------------
  // [DST_OVER]
  // --------------------------------------------------------------------------

  //! @brief The destination is composited over the source and the result 
  //! replaces the destination. 
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (DST_OVER):
  //!   Dca' = Dca.Sa + Sca.(1 - Da) + Dca.(1 - Sa)
  //!        = Dca + Sca.(1 - Da)
  //!   Da'  = Da.Sa + Sa.(1 - Da) + Da.(1 - Sa)
  //!        = Da + Sa.(1 - Da)
  //!        = Da + Sa - Da.Sa
  //!
  //!   Msk:
  //!
  //!   Dca' = (Dca + Sca.(1 - Da)).m + Dca.(1 - m)   => Dca + Sca.(1 - Da).m
  //!   Da'  = (Da  + Sa .(1 - Da)).m + Da .(1 - m)   => Da  + Sa .(1 - Da).m
  //!
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (DST_OVER):
  //!   Dca' = Dca + Sc.(1 - Da)
  //!   Da'  = 1
  //!
  //!   Msk:
  //!
  //!   Dca' = Dca + Sc.m.(1 - Da)
  //!   Da'  = Da + m.(1 - Da)
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (NOP):
  //!   Dc'  = Dc
  //!
  //!   Msk:
  //!
  //!   Dc ' = Dc
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (NOP):
  //!   Dc'  = Dc
  //!
  //!   Msk:
  //!
  //!   Dc ' = Dc
  //!
  //! Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  //!   Da'  = Da + Sa.(1 - Da) (standard formula used many times)
  //!
  //!   Msk:
  //!
  //!   Da'  = Da + Sa.m.(1 - Da)
  //!
  //! If the destination is full opaque (Da == 1.0 or there is no aplha), there
  //! is no change.
  OPERATOR_DST_OVER,

  // --------------------------------------------------------------------------
  // [SRC_IN]
  // --------------------------------------------------------------------------

  //! @brief The part of the source lying inside of the destination replaces
  //! the destination. 
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SRC_IN):
  //!   Dca' = Sca.Da
  //!   Da'  = Sa.Da
  //!
  //!   Msk:
  //!
  //!   Dca' = Sca.Da.m + Dca.(1 - m)
  //!   Da'  = Sa .Da.m + Da .(1 - m)
  //! 
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (SRC_IN):
  //!   Dca' = Sc.Da
  //!   Da'  = Da
  //!
  //!   Msk:
  //!
  //!   Dca' = Sc.Da.m + Dca.(1 - m)
  //!   Da'  = 1 .Da.m + Da .(1 - m)
  //!        = Da
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (SRC):
  //!   Dc'  = Sca
  //!
  //!   Msk:
  //!
  //!   Dc'  = (Sca.m + Dc.(1 - m))
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (SRC):
  //!   Dc'  = Sc
  //!
  //!   Msk:
  //!
  //!   Dc'  = (Sc.m + Dc.(1 - m))
  //!
  //! Formulas for A(dst), A(src) colorspaces (MUL):
  //!   Da'  = Sa.Da
  //!
  //!   Msk:
  //!
  //!   Da'  = Sa.Da.m + Da.(1 - m)
  //!        = Da.(Sa.m + 1 - m)
  OPERATOR_SRC_IN,

  // --------------------------------------------------------------------------
  // [DST_IN]
  // --------------------------------------------------------------------------

  //! @brief The part of the destination lying inside of the source replaces
  //! the destination. 
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (DST_IN):
  //!   Dca' = Dca.Sa
  //!   Da'  = Da.Sa
  //!
  //!   Msk:
  //!
  //!   Dca' = Dca.Sa.m + Dca.(1 - m)
  //!   Da'  = Da.Sa.m + Da.(1 - m)
  //!
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (NOP):
  //!   Dca' = Dca
  //!   Da'  = Da
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (DST_IN):
  //!   Dc'  = Dc.Sa
  //!
  //!   Msk:
  //!
  //!   Dc'  = Dc.Sa.m + Dc.(m - 1)
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (NOP):
  //!   Dc'  = Dc
  //!
  //! Formulas for A(dst), A(src) colorspaces (MUL):
  //!   Da'  = Sa.Da
  //!
  //!   Msk:
  //!
  //!   Da'  = Sa.Da.m + Da.(1 - m)
  //!        = Da.(Sa.m + 1 - m)
  OPERATOR_DST_IN,

  // --------------------------------------------------------------------------
  // [SRC_OUT]
  // --------------------------------------------------------------------------

  //! @brief The part of the source lying outside of the destination replaces
  //! the destination. 
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SRC_OUT):
  //!   Dca' = Sca.(1 - Da)
  //!   Da'  = Sa.(1 - Da)
  //!
  //!   Msk:
  //!
  //!   Dca' = Sca.(1 - Da).m + Dca.(1 - m)
  //!   Da'  = Sa.(1 - Da).m + Da.(1 - m)
  //!
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (SRC_OUT):
  //!   Dca' = Sc.(1 - Da)
  //!   Da'  = (1 - Da)
  //!
  //!   Msk:
  //!
  //!   Dca' = Sc.(1 - Da).m + Dca.(1 - m)
  //!   Da'  = (1 - Da).m + Da.(m - 1)
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (CLEAR):
  //!   Dc'  = 0
  //!
  //!   Msk:
  //!
  //!   Dc'  = Dca.(1 - m)
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (CLEAR):
  //!   Dc'  = 0
  //!
  //!   Msk:
  //!
  //!   Dc'  = Dc.(1 - m)
  //!
  //! Formulas for A(dst), A(src) colorspaces (SRC_OUT):
  //!   Da'  = Sa.(1 - Da)
  //!
  //!   Msk:
  //!
  //!   Da'  = Sa.(1 - Da).m + Da.(1 - m)
  OPERATOR_SRC_OUT,

  // --------------------------------------------------------------------------
  // [DST_OUT]
  // --------------------------------------------------------------------------

  //! @brief The part of the destination lying outside of the source replaces
  //! the destination. 
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (DST_OUT):
  //!   Dca' = Dca.(1 - Sa)
  //!   Da'  = Da.(1 - Sa)
  //!
  //!   Msk:
  //!
  //!   Dca' = Dca.(1 - Sa).m + Dca.(1 - m)
  //!        = Dca.(1 - Sa.m)
  //!   Da'  = Da.(1 - Sa).m + Da.(1 - m)
  //!        = Da.(1 - Sa.m)
  //!
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (CLEAR):
  //!   Dca' = 0
  //!   Da'  = 0
  //!
  //!   Msk:
  //!
  //!   Dca' = Dca.(m - 1)
  //!   Da'  = Da.(m - 1)
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (DST_OUT):
  //!   Dc'  = Dc.(1 - Sa)
  //!
  //!   Msk:
  //!
  //!   Dc'  = Dc.(1 - Sa.m)
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (CLEAR):
  //!   Dc'  = 0
  //!
  //!   Msk:
  //!
  //!   Dc'  = Dc.(m - 1)
  //!   Da'  = Da.(m - 1)
  //!
  //! Formulas for A(dst), A(src) colorspaces (DST_OUT):
  //!   Da'  = Da.(1 - Sa)
  OPERATOR_DST_OUT,

  // --------------------------------------------------------------------------
  // [SRC_ATOP]
  // --------------------------------------------------------------------------

  //! @brief The part of the source lying inside of the destination is
  //! composited onto the destination. 
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SRC_ATOP):
  //!   Dca' = Sca.Da + Dca.(1 - Sa)
  //!   Da'  = Sa.Da + Da.(1 - Sa) 
  //!        = Sa.Da + Da - Da.Sa
  //!        = Da
  //!
  //!   Msk:
  //!
  //!   Dca' = (Sca.Da + Dca.(1 - Sa)).m + Dca.(1 - m)
  //!        = Sca.Da.m + Dca.(1 - Sa).m + Dca.(1 - m)
  //!        = Sca.Da.m + Dca.(1 - Sa.m)
  //!   Da'  = Sa.Da.m + Da.(1 - Sa.m)
  //!        = Da
  //!
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (SRC_IN):
  //!   Dca' = Sc.Da
  //!   Da'  = Da
  //!
  //!   Msk:
  //!
  //!   Dca' = (Sc.Da + Dca.(1 - Sa)).m - Dca.(1 - m)
  //!        = Sc.Da.m + Dca.(1 - Sa).m - Dca.(1 - m)
  //!        = Sc.Da.m + Dca.(1 - Sa.m)
  //!   Da'  = (Da + Da.(1 - 1)).m - Da.(1 - m)
  //!        = Da.m + Da.(1 - m)
  //!        = Da
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (SRC_OVER):
  //!   Dc'  = Sca + Dc.(1 - Sa)
  //!
  //!   Msk:
  //!
  //!   Dc'  = Sca.m + Dc.(1 - Sa.m)
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (SRC):
  //!   Dc'  = Sc
  //!
  //!   Msk:
  //!
  //!   Dc'  = Sc.m + Dc.(1 - m)
  //!
  //! Formulas for A(dst), A(src) colorspaces (NOP):
  //!   Da'  = Da
  OPERATOR_SRC_ATOP,

  // --------------------------------------------------------------------------
  // [DST_ATOP]
  // --------------------------------------------------------------------------

  //! @brief The part of the destination lying inside of the source is 
  //! composited over the source and replaces the destination. 
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (DST_ATOP):
  //!   Dca' = Dca.Sa + Sca.(1 - Da)
  //!   Da'  = Da.Sa + Sa.(1 - Da)
  //!        = Sa.(Da + 1 - Da)
  //!        = Sa
  //!
  //!   Msk:
  //!
  //!   Dca' = (Dca.Sa + Sca.(1 - Da)).m + Dca.(1 - m)
  //!   Da'  = (Da.Sa + Sa.(1 - Da)).m + Da.(1 - m)
  //!
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (DST_OVER):
  //!   Dca' = Dca + Sc.(1 - Da)
  //!   Da'  = 1
  //!
  //!   Msk:
  //!
  //!   Dca' = Dca + Sc.m.(1 - Da)
  //!   Da'  = Da + m.(1 - Da)
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (DST_IN):
  //!   Dc'  = Dc.Sa
  //!
  //!   Msk:
  //!
  //!   Dca' = Dc.Sa.m + Dc.(1 - m)
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (NOP):
  //!   Dc'  = Dc
  //!
  //! Formulas for A(dst), A(src) colorspaces (SRC):
  //!   Da'  = Sa
  //!
  //!   Msk:
  //!
  //!   Da'  = Sa.m + Da.(1 - m)
  OPERATOR_DST_ATOP,

  // --------------------------------------------------------------------------
  // [XOR]
  // --------------------------------------------------------------------------

  //! @brief The part of the source that lies outside of the destination is 
  //! combined with the part of the destination that lies outside of the source.
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (XOR):
  //!   Dca' = Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa.(1 - Da) + Da.(1 - Sa)
  //!        = Sa + Da - 2.Sa.Da
  //!
  //!   Msk:
  //!
  //!   Dca' = Sca.m.(1 - Da) + Dca.(1 - Sa.m)
  //!   Da'  = Sa.m.(1 - Da) + Da.(1 - Sa.m)
  //!
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (SRC_OUT):
  //!   Dca' = Sc.(1 - Da)
  //!   Da'  = (1 - Da)
  //!
  //!   Msk:
  //!
  //!   Dca' = Sc.m.(1 - Da) + Dca.(1 - m)
  //!   Da'  = 1.m.(1 - Da) + Da.(1 - m)
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (DST_OUT):
  //!   Dc'  = Dc.(1 - Sa)
  //!
  //!   Msk:
  //!
  //!   Dc'  = Dc.(1 - Sa.m)
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (CLEAR):
  //!   Dc'  = 0
  //!
  //! Formulas for A(dst), A(src) colorspaces (XOR):
  //!   Da'  = Sa.(1 - Da) + Da.(1 - Sa)
  OPERATOR_XOR,

  // --------------------------------------------------------------------------
  // [CLEAR]
  // --------------------------------------------------------------------------

  //! @brief Clear the destination not using the source.
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (CLEAR):
  //!   Dca' = 0
  //!   Da'  = 0
  //!
  //!   Msk:
  //!
  //!   Dca' = Dca.(1 - m)
  //!   Da'  = Da .(1 - m)
  //!
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (CLEAR):
  //!   Dca' = 0
  //!   Da'  = 0
  //!
  //!   Msk:
  //!
  //!   Dca' = Dca.(1 - m)
  //!   Da'  = Da .(1 - m)
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (CLEAR):
  //!   Dc'  = 0
  //!
  //!   Msk:
  //!
  //!   Dc'  = Dc.(1 - m)
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (CLEAR):
  //!   Dc'  = 0
  //!
  //!   Msk:
  //!
  //!   Dc'  = Dc.(1 - m)
  //!
  //! Formulas for A(dst), A(src) colorspaces (CLEAR):
  //!   Da'  = 0
  //!
  //!   Msk:
  //!
  //!   Da'  = Da.(1 - m)
  OPERATOR_CLEAR,

  // --------------------------------------------------------------------------
  // [ADD]
  // --------------------------------------------------------------------------

  //! @brief The source is added to the destination and replaces the destination.
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (ADD):
  //!   Dca' = Sca.Da + Dca.Sa + Sca.(1 - Da) + Dca.(1 - Sa)
  //!        = Dca + Sca
  //!   Da'  = Sa.Da + Da.Sa + Sa.(1 - Da) + Da.(1 - Sa)
  //!        = Da + Sa
  //!
  //!   Msk:
  //!
  //!   Dca' = Sca.m + Dca.m + Dca.(1 - m)
  //!        = Dca + Sca.m
  //!   Da'  = Sa.m + Da.m + Da.(1 - m)
  //!        = Da + Sa.m
  //!
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (ADD):
  //!   Dca' = Dca + Sc
  //!   Da'  = 1
  //!
  //!   Msk:
  //!
  //!   Dca' = Dca + Sc.m
  //!   Da'  = Da + m
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (ADD):
  //!   Dc'  = Dc + Sca
  //!
  //!   Msk:
  //!
  //!   Dc'  = Dc + Sca.m
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (ADD):
  //!   Dc'  = Dc + Sc
  //!
  //!   Msk:
  //!
  //!   Dc'  = Dc + Sc.m
  //!
  //! Formulas for A(dst), A(src) colorspaces (ADD):
  //!   Da'  = Da + Sa
  //!
  //!   Msk:
  //!
  //!   Da'  = Da + Sa.m
  OPERATOR_ADD,

  // --------------------------------------------------------------------------
  // [SUBTRACT]
  // --------------------------------------------------------------------------

  //! @brief The source is subtracted from the destination and replaces 
  //! the destination.
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SUBTRACT):
  //!   Dca' = Dca - Sca
  //!   Da'  = 1 - (1 - Sa).(1 - Da)
  //!        = Sa + Da - Sa.Da
  //!
  //!   Msk:
  //!
  //!   Dca' = Dca.m - Sca.m + Dca.(1 - m)
  //!        = Dca - Sca.m
  //!   Da'  = Da + Sa.m - Sa.m.Da
  //!        = Da + Sa.m - (1 - Da)
  //!
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (SUBTRACT):
  //!   Dca' = Dca - Sc
  //!   Da'  = 1
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (SUBTRACT):
  //!   Dc'  = Dc - Sca
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (SUBTRACT):
  //!   Dc'  = Dc - Sc
  //!
  //! Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  //!   Da'  = Da + Sa.(1 - Da) (standard formula used many times)
  //!
  //!   Msk:
  //!
  //!   Da'  = Da + Sa.m.(1 - Da)
  OPERATOR_SUBTRACT,

  // --------------------------------------------------------------------------
  // [MULTIPLY]
  // --------------------------------------------------------------------------

  //! @brief The source is multiplied by the destination and replaces 
  //! the destination.
  //!
  //! The resultant color is always at least as dark as either of the two 
  //! constituent colors. Multiplying any color with black produces black.
  //! Multiplying any color with white leaves the original color unchanged.
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (MULTIPLY):
  //!   Dca' = Sca.Dca + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa.Da + Sa.(1 - Da) + Da.(1 - Sa)
  //!
  //!   Msk:
  //!
  //!   Dca' = (Dca.(Sca + 1 - Sa) + Sca.(1 - Da)).m + Dca.(1 - m)
  //!          (Dca.(Sca + 1 - Sa).m + Sca.(1 - Da).m + Dca.(1 - m)
  //!
  //!   Da'  = (Da.(Sa + 1 - Sa) + Sa.(1 - Da)).m + Da.(1 - m)
  //!
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (MULTIPLY):
  //!   Dca' = Sc.Dca + Sc.(1 - Da)
  //!   Da'  = 1
  //!
  //!   Msk:
  //!
  //!   Dca' = (Sc.Dca + Sc.(1 - Da)).m + Dca.(1 - m)
  //!   Da'  = m + Da.(1 - m)
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (MULTIPLY):
  //!   Dc'  = Sca.Dc + Dc.(1 - Sa)
  //!
  //!   Msk:
  //!
  //!   Dc'  = Dc + Dc.(Sca.m - Sa.m)
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (MULTIPLY):
  //!   Dc'  = Sc.Dc
  //!
  //!   Msk:
  //!
  //!   Dc'  = (Sc.Dc.m) + Dc.(1 - m)
  //!        = Dc.(Sc.m + 1 - m)
  //!
  //! Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  //!   Da'  = Da + Sa.(1 - Da) (standard formula used many times)
  //!
  //!   Msk:
  //!
  //!   Da'  = Da + Sa.m.(1 - Da)
  OPERATOR_MULTIPLY,

  // --------------------------------------------------------------------------
  // [SCREEN]
  // --------------------------------------------------------------------------

  //! @brief The source and destination are complemented and then multiplied 
  //! and then replace the destination.
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SCREEN):
  //!   Dca' = Sca + Dca.(1 - Sca)
  //!   Da'  = Sa + Da.(1 - Sa)
  //!
  //!   Msk:
  //!
  //!   Dca' = Sca.m + Dca.(1 - Sca.m)
  //!   Da'  = Sa.m + Da.(1 - Sa.m)
  //!
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (SCREEN):
  //!   Dca' = Sc + Dca.(1 - Sc)
  //!   Da'  = 1
  //!
  //!   Msk:
  //!
  //!   Dca' = Sc.m + Dca.(1 - Sc.m)
  //!   Da'  = m + Da.(1 - m)
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (SCREEN):
  //!   Dc'  = Sca + Dc.(1 - Sca)
  //!
  //!   Msk:
  //!
  //!   Dca' = Sca.m + Dc.(1 - Sca.m)
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (SCREEN):
  //!   Dc'  = Sc + Dc.(1 - Sc)
  //!
  //!   Msk:
  //!
  //!   Dc'  = Sc.m + Dc.(1 - Sc.m)
  //!
  //! Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  //!   Da'  = Da + Sa.(1 - Da) (standard formula used many times)
  //!
  //!   Msk:
  //!
  //!   Da'  = Da + Sa.m.(1 - Da)
  OPERATOR_SCREEN,

  // --------------------------------------------------------------------------
  // [DARKEN]
  // --------------------------------------------------------------------------

  //! @brief Selects the darker of the destination and source colors. The 
  //! destination is replaced with the source when the source is darker,
  //! otherwise it is left unchanged.
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (DARKEN):
  //!   Dca' = min(Sca.Da, Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = min(Sa.Da, Da.Sa) + Sa.(1 - Da) + Da.(1 - Sa)
  //!        = Sa.Da + Sa - Sa.Da + Da - Sa.Da
  //!        = Sa + Da - Sa.Da
  //!
  //!   ALTERNATIVE: if (Sca.Da < Dca.Sa) Src-Over() else Dst-Over()
  //!
  //!   Msk:
  //!
  //!   Dca' = (min(Sca.Da, Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa)).m + Dca.(1 - m)
  //!          min(Sca.Da.m, Dca.Sa.m) + Sca.m.(1 - Da) +  Dca.(1 - Sa.m)
  //!   Da'  = (min(Sa.Da, Da.Sa) + Sa.(1 - Da) + Da.(1 - Sa)).m + Da.(1 - m)
  //!          min(Sa.Da.m, Da.Sa.m) + Sa.m.(1 - Da) +  Da.(1 - Sa.m)
  //!
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (DARKEN):
  //!   Dca' = min(Sc.Da, Dca) + Sc.(1 - Da)
  //!   Da'  = 1
  //!
  //!   Msk:
  //!
  //!   Dca' = min(Da.m, Dca.m) + Sc.m.(1 - Da) +  Dca.(1 - m)
  //!   Da'  = min(Da.m, Da.m) + 1.m.(1 - Da) +  Da.(1 - m)
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (DARKEN):
  //!   Dc'  = min(Sca, Dc.Sa) + Dc.(1 - Sa)
  //!
  //!   Msk:
  //!
  //!   Dc'  = min(Sca.m, Dc.Sa.m) + Dc.(1 - Sa.m)
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (DARKEN):
  //!   Dc'  = min(Sc, Dc)
  //!
  //!   Msk:
  //!
  //!   Dc'  = (min(Sc, Dc)).m + Dc.(1 - m)
  //!
  //! Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  //!   Da'  = Da + Sa.(1 - Da) (standard formula used many times)
  //!
  //!   Msk:
  //!
  //!   Da'  = Da + Sa.m.(1 - Da)
  OPERATOR_DARKEN,

  // --------------------------------------------------------------------------
  // [LIGHTEN]
  // --------------------------------------------------------------------------

  //! @brief Selects the lighter of the destination and source colors. The
  //! destination is replaced with the source when the source is lighter, 
  //! otherwise it is left unchanged.
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (LIGHTEN):
  //!   Dca' = max(Sca.Da, Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = max(Sa.Da, Da.Sa) + Sa.(1 - Da) + Da.(1 - Sa)
  //!        = Sa.Da + Sa - Sa.Da + Da - Sa.Da
  //!        = Sa + Da - Sa.Da
  //!
  //!   ALTERNATIVE: if (Sca.Da > Dca.Sa) Src-Over() else Dst-Over()
  //!
  //!   Msk:
  //!
  //!   Dca' = (max(Sca.Da, Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa)).m + Dca.(1 - m)
  //!          max(Sca.Da.m, Dca.Sa.m) + Sca.m.(1 - Da) +  Dca.(1 - Sa.m)
  //!   Da'  = (max(Sa.Da, Da.Sa) + Sa.(1 - Da) + Da.(1 - Sa)).m + Da.(1 - m)
  //!          max(Sa.Da.m, Da.Sa.m) + Sa.m.(1 - Da) +  Da.(1 - Sa.m)
  //!
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (LIGHTEN):
  //!   Dca' = max(Sc.Da, Dca) + Sc.(1 - Da)
  //!   Da'  = 1
  //!
  //!   Msk:
  //!
  //!   Dca' = max(Da.m, Dca.m) + Sc.m.(1 - Da) +  Dca.(1 - m)
  //!   Da'  = max(Da.m, Da.m) + 1.m.(1 - Da) +  Da.(1 - m)
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (LIGHTEN):
  //!   Dc'  = max(Sca, Dc.Sa) + Dc.(1 - Sa)
  //!
  //!   Msk:
  //!
  //!   Dc'  = max(Sca.m, Dc.Sa.m) + Dc.(1 - Sa.m)
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (LIGHTEN):
  //!   Dc'  = max(Sc, Dc)
  //!
  //!   Msk:
  //!
  //!   Dc'  = (max(Sc, Dc)).m + Dc.(1 - m)
  //!
  //! Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  //!   Da'  = Da + Sa.(1 - Da) (standard formula used many times)
  //!
  //!   Msk:
  //!
  //!   Da'  = Da + Sa.m.(1 - Da)
  OPERATOR_LIGHTEN,

  // --------------------------------------------------------------------------
  // [DIFFERENCE]
  // --------------------------------------------------------------------------

  //! @brief Subtracts the darker of the two constituent colors from the 
  //! lighter. Painting with white inverts the destination color. Painting
  //! with black produces no change.
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (DIFFERENCE):
  //!   Dca' = abs(Dca.Sa - Sca.Da) + Sca.(1 - Da) + Dca.(1 - Sa)
  //!        = Dca + Sca - 2.min(Sca.Da, Dca.Sa)
  //!   Da'  = abs(Da.Sa - Sa.Da) + Sa.(1 - Da) + Da.(1 - Sa)
  //!        = Sa + Da - min(Sa.Da, Da.Sa)
  //!        = Sa + Da - Sa.Da
  //!
  //!   Msk:
  //!
  //!   Dca' = (abs(Dca.Sa - Sca.Da) + Sca.(1 - Da) + Dca.(1 - Sa)).m + Dca.(1 - m)
  //!        = (Sca + Dca - 2.min(Sca.Da, Dca.Sa)).m + Dca.(1 - m)
  //!        = Dca + (Sca - 2.min(Sca.Da, Dca.Sa)).m
  //!        = Dca + Sca.m - 2.min(Sca.Da.m, Dca.Sa.m)
  //!   Da'  = (Sa + Da - min(Sa.Da, Da.Sa)).m + Da.(1 - m)
  //!        = Da + (Sa - min(Sa.Da, Da.Sa)).m
  //!        = Da + Sa.m - min(Sa.Da.m, Da.Sa.m)
  //!
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (DIFFERENCE):
  //!   Dca' = abs(Dca.Sa - Sc.Da) + Sca.(1 - Da) + Dca
  //!        = Dca + Sc - 2.min(Sc.Da, Dca.1)
  //!   Da'  = abs(Da.1 - 1.Da) + 1.(1 - Da) + Da
  //!        = 1 + Da - min(1.Da, Da.1)
  //!        = 1
  //!
  //!   Msk:
  //!
  //!   Dca' = (abs(Dca.Sa - Sca.Da) + Sca.(1 - Da) + Dca.(1 - Sa)).m + Dca.(1 - m)
  //!        = (Sca + Dca - 2.min(Sca.Da, Dca.Sa)).m + Dca.(1 - m)
  //!        = Dca + (Sca - 2.min(Sca.Da, Dca.Sa)).m
  //!        = Dca + Sca.m - 2.min(Sca.Da.m, Dca.Sa.m)
  //!   Da'  = (abs(Da.Sa - Sa.Da) + Sa.(1 - Da) + Da.(1 - Sa)).m + Da.(1 - m)
  //!        = (Sa + Da - 2.min(Sa.Da, Da.Sa)).m + Da.(1 - m)
  //!        = Da + (Sa - 2.min(Sa.Da, Da.Sa)).m
  //!        = Da + Sa.m - 2.min(Sa.Da.m, Da.Sa.m)
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (DIFFERENCE):
  //!   Dc'  = abs(Dc.Sa - Sca) + Dc.(1 - Sa)
  //!        = Sca + Dc - 2.min(Sca, Dc.Sa)
  //!
  //!   Msk:
  //!
  //!   Dc'  = (abs(Dc.Sa - Sca) + Dc.(1 - Sa)).m + Dc.(1 - m)
  //!        = abs(Dc.Sa.m - Sca.m) + Dc.(1 - Sa.m)
  //!   Dc'  = (Sca + Dc - 2.min(Sca, Dc.Sa)).m + Dc.(1 - m)
  //!        = Sca.m + Dc - 2.min(Sca.m, Dc.Sa.m)
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (DIFFERENCE):
  //!   Dc'  = Dc + Sc - 2.min(Sc, Dc)
  //!        = abs(Dc - Sc)
  //!
  //!   Msk:
  //!
  //!   Dc'  = abs(Dc - Sc).m + Dc.(1 - m)
  //!        = abs(Dc.m - Sc.m) + Dc.(1 - m)
  //!   Dc'  = (Sc + Dc - 2.min(Sc, Dc)).m + Dc.(1 - m)
  //!        = Sc.m + Dc - 2.min(Sc.m, Dc.m)
  //!
  //! Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  //!   Da'  = Da + Sa.(1 - Da) (standard formula used many times)
  //!
  //!   Msk:
  //!
  //!   Da'  = Da + Sa.m.(1 - Da)
  OPERATOR_DIFFERENCE,

  // --------------------------------------------------------------------------
  // [EXCLUSION]
  // --------------------------------------------------------------------------

  //! @brief Produces an effect similar to that of 'difference', but appears
  //! as lower contrast. Painting with white inverts the destination color.
  //! Painting with black produces no change.
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (EXCLUSION):
  //!   Dca' = Sca.Da + Dca - 2.Sca.Dca
  //!   Da'  = Sa + Da - Sa.Da
  //!
  //!   Msk:
  //!
  //!   Dca' = Sca.m.Da + Dca - 2.Sca.m.Dca
  //!   Da'  = Sa.m + Da - Sa.m.Da
  //!
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (EXCLUSION):
  //!   Dca' = Sc.Da + Dca - 2.Sc.Dca
  //!   Da'  = 1
  //!
  //!   Msk:
  //!
  //!   Dca' = Sc.m.Da + Dca - 2.Sc.m.Dca
  //!   Da'  = m + Da - m.Da
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (EXCLUSION):
  //!   Dc'  = Sca + Dc - 2.Sca.Dc
  //!
  //!   Msk:
  //!
  //!   Dc'  = Sca.m + Dc - 2.Sca.m.Dc
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (EXCLUSION):
  //!   Dc'  = Sc + Dc - 2.Sc.Dc
  //!
  //!   Msk:
  //!
  //!   Dc'  = Sc.m.Da + Dc - 2.Sc.m.Dc
  //!
  //! Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  //!   Da'  = Da + Sa.(1 - Da) (standard formula used many times)
  //!
  //!   Msk:
  //!
  //!   Da'  = Da + Sa.m.(1 - Da)
  OPERATOR_EXCLUSION,

  // --------------------------------------------------------------------------
  // [INVERT]
  // --------------------------------------------------------------------------

  //! @brief Invert.
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (INVERT):
  //!   Dca' = (Da - Dca) * Sa + Dca.(1 - Sa)
  //!   Da'  = (1) * Sa + Da.(1 - Sa)
  //!
  //!   Msk:
  //!
  //!   Dca' = ((Da - Dca) * Sa + Dca.(1 - Sa)).m + Dca.(1 - m)
  //!        = (Da - Dca) * Sa.m + Dca.(1 - Sa.m)
  //!   Da'  = (1) * Sa.m + Da.(1 - Sa.m)
  //!
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (INVERT):
  //!   Dca' = (Da - Dca)
  //!   Da'  = 1
  //!
  //!   Msk:
  //!
  //!   Dca' = (Da - Dca).m + Dca.(1 - m)
  //!          Da.m - Dca.m + Dca.(1 - m)
  //!   Da'  = m - Da.(1 - m)
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (INVERT):
  //!   Dc'  = (1 - Dc) * Sa + Dc.(1 - Sa)
  //!
  //!   Msk:
  //!
  //!   Dc'  = (1 - Dc).Sa.m + Dc.(1 - Sa.m)
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (INVERT):
  //!   Dc'  = 1 - Dc
  //!
  //!   Msk:
  //!
  //!   Dc'  = (1 - Dc).m + Dc.(1 - m)
  //!
  //! Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  //!   Da'  = Da + Sa.(1 - Da) (standard formula used many times)
  //!
  //!   Msk:
  //!
  //!   Da'  = Da + Sa.m.(1 - Da)
  OPERATOR_INVERT,

  // --------------------------------------------------------------------------
  // [INVERT_RGB]
  // --------------------------------------------------------------------------

  //! @brief Invert RGB.
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (INVERT_RGB):
  //!   Dca' = (Da - Dca) * Sca + Dca.(1 - Sa)
  //!   Da'  = (1) * Sa + Da.(1 - Sa)
  //!
  //!   Msk:
  //!
  //!   Dca' = (Da - Dca) * Sca.m + Dca.(1 - Sa.m)
  //!   Da'  = (1) * Sa.m + Da.(1 - Sa.m)
  //!
  //! Formulas for PRGB(dst), XRGB(src) colorspaces (INVERT_RGB):
  //!   Dca' = (Da - Dca) * Sc
  //!   Da'  = 1
  //!
  //!   Msk:
  //!
  //!   Dca' = (Da - Dca) * Sc.m + Dca.(1 - Sa.m)
  //!   Da'  = (1) * Sa.m + Da.(1 - Sa.m)
  //!
  //! Formulas for XRGB(dst), PRGB(src) colorspaces (INVERT_RGB):
  //!   Dc'  = (1 - Dc) * Sca + Dc.(1 - Sa)
  //!
  //!   Msk:
  //!
  //!   Dc'  = (1 - Dc) * Sca.m + Dc.(1 - Sa.m)
  //!
  //! Formulas for XRGB(dst), XRGB(src) colorspaces (INVERT_RGB):
  //!   Dc'  = (1 - Dc) * Sc
  //!
  //!   Msk:
  //!
  //!   Dca' = (1 - Dc) * Sc.m + Dc.(1 - Sa.m)
  //!
  //! Formulas for A(dst), A(src) colorspaces (SRC_OVER):
  //!   Da'  = Da + Sa.(1 - Da) (standard formula used many times)
  //!
  //!   Msk:
  //!
  //!   Da'  = Da + Sa.m.(1 - Da)
  OPERATOR_INVERT_RGB,

  // --------------------------------------------------------------------------
  // [...]
  // --------------------------------------------------------------------------

  //! @brief Count of compositing operators (for error checking).
  OPERATOR_COUNT
};

// ============================================================================
// [Fog::PIXEL_FORMAT]
// ============================================================================

//! @brief Pixel format.
//!
//! @note All pixel formats are CPU endian dependent. So @c ARGB32 pixels
//! will be stored differenly in memory on machines with different endianness.
//!
//! @c PIXEL_FORMAT_ARGB32, @c PIXEL_FORMAT_PRGB32:
//! - Little endian: BBGGRRAA
//! - Big endian   : AARRGGBB
//! @c PIXEL_FORMAT_XRGB32:
//! - Little endian: BBGGRRXX
//! - Big endian   : XXRRGGBB
//! @c PIXEL_FORMAT_A8:
//! - no difference: AA (8 bit alpha value)
//! @c PIXEL_FORMAT_I8:
//! - no difference: II (8 bit index value to palette)
//!
//! @sa @c PIXEL_FORMAT_EXTENDED.
enum PIXEL_FORMAT
{
  //! @brief 32-bit RGBA, premultiplied.
  PIXEL_FORMAT_PRGB32 = 0,

  //! @brief 32-bit RGBA, non-premultiplied.
  //!
  //! Eequivalent to @c Argb.
  PIXEL_FORMAT_ARGB32 = 1,

  //! @brief 32-bit XRGB, no alpha.
  //!
  //! equivalent to @c Argb, where alpha is set to 255).
  PIXEL_FORMAT_XRGB32 = 2,

  //! @brief 8-bit alpha channel only.
  PIXEL_FORMAT_A8 = 3,

  //! @brief 8-bit indexed pixel format.
  PIXEL_FORMAT_I8 = 4,

  //! @brief Count of pixel formats.
  PIXEL_FORMAT_COUNT = 5,

  //! @brief Null pixel format (used only by empty images).
  PIXEL_FORMAT_NULL = 0xFF
};

// ============================================================================
// [Fog::DIB_FORMAT]
// ============================================================================

//! @brief Device independent pixel formats that can be used by Image::getDib()
//! and Image::setDib() methods. These formats can't be used by @c Image itself.
enum DIB_FORMAT
{
  //! @brief 32-bit RGBA, premultiplied (compatible to @c PIXEL_FORMAT_PRGB32)
  DIB_FORMAT_PRGB32_NATIVE = 0,

  //! @brief 32-bit RGBA, non-premultiplied (compatible to @c PIXEL_FORMAT_ARGB32).
  //!
  //! Eequivalent to @c Argb.
  DIB_FORMAT_ARGB32_NATIVE = 1,

  //! @brief 32-bit XRGB, no alpha (compatible to @c PIXEL_FORMAT_XRGB32).
  //!
  //! equivalent to @c Argb, where alpha is set to 255).
  DIB_FORMAT_XRGB32_NATIVE = 2,

  //! @brief 8-bit alpha channel (compatible to @c PIXEL_FORMAT_A8).
  DIB_FORMAT_A8 = 3,

  //! @brief 8-bit indexed pixel format (compatible to @c PIXEL_FORMAT_I8).
  DIB_FORMAT_I8 = 4,

  //! @brief 32-bit RGBA, premultiplied, byteswapped.
  DIB_FORMAT_PRGB32_SWAPPED = 5,

  //! @brief 32-bit RGBA, non-premultiplied, byteswapped.
  DIB_FORMAT_ARGB32_SWAPPED = 6,

  //! @brief 32-bit XRGB, no alpha, byteswapped.
  DIB_FORMAT_XRGB32_SWAPPED = 7,

  //! @brief 24-bit RGB in native endian format.
  //!
  //! If your target machine is little endian, individual pixels in buffer are
  //! encoded in BGRBGRBGR... form, while on big endian machines the order is
  //! reversed: RGBRGBRGB.
  DIB_FORMAT_RGB24_NATIVE = 8,

  //! @brief 24-bit RGB in byte-swapped endian format.
  DIB_FORMAT_RGB24_SWAPPED = 9,

  //! @brief 16-bit RGB in 5-6-5 form.
  DIB_FORMAT_RGB16_565_NATIVE = 10,

  //! @brief 16-bit RGB in 5-6-5 form, but byteswapped.
  //!
  //! see @c DIB_FORMAT_RGB16_565_NATIVE.
  DIB_FORMAT_RGB16_565_SWAPPED = 11,

  //! @brief 16-bit RGB in 5-5-5 form, one bit is unused and ignored (it's set
  //! to zero by fog DIB routines).
  DIB_FORMAT_RGB16_555_NATIVE = 12,

  //! @brief 16-bit RGB in 5-5-5 form, but byteswapped.
  //!
  //! see @c DIB_FORMAT_RGB16_555_NATIVE.
  DIB_FORMAT_RGB16_555_SWAPPED = 13,

  //! @brief 8-bit greyscale, no alpha.
  DIB_FORMAT_GREY8 = 14,

  //! @brief Count of DIB formats.
  DIB_FORMAT_COUNT = 15,

  // Convenience formats.

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
# define __DIB_FORMAT_SWITCH(name, le, be) name = le
#else
# define __DIB_FORMAT_SWITCH(name, le, be) name = le
#endif // FOG_BYTE_ORDER

  __DIB_FORMAT_SWITCH(DIB_FORMAT_PRGB32_LE, DIB_FORMAT_PRGB32_NATIVE, DIB_FORMAT_PRGB32_SWAPPED),
  __DIB_FORMAT_SWITCH(DIB_FORMAT_ARGB32_LE, DIB_FORMAT_ARGB32_NATIVE, DIB_FORMAT_ARGB32_SWAPPED),
  __DIB_FORMAT_SWITCH(DIB_FORMAT_XRGB32_LE, DIB_FORMAT_XRGB32_NATIVE, DIB_FORMAT_XRGB32_SWAPPED),

  __DIB_FORMAT_SWITCH(DIB_FORMAT_PRGB32_BE, DIB_FORMAT_PRGB32_SWAPPED, DIB_FORMAT_PRGB32_NATIVE),
  __DIB_FORMAT_SWITCH(DIB_FORMAT_ARGB32_BE, DIB_FORMAT_ARGB32_SWAPPED, DIB_FORMAT_ARGB32_NATIVE),
  __DIB_FORMAT_SWITCH(DIB_FORMAT_XRGB32_BE, DIB_FORMAT_XRGB32_SWAPPED, DIB_FORMAT_XRGB32_NATIVE),

  __DIB_FORMAT_SWITCH(DIB_FORMAT_RGB24_LE, DIB_FORMAT_RGB24_NATIVE, DIB_FORMAT_RGB24_SWAPPED),
  __DIB_FORMAT_SWITCH(DIB_FORMAT_RGB24_BE, DIB_FORMAT_RGB24_SWAPPED, DIB_FORMAT_RGB24_NATIVE),

  __DIB_FORMAT_SWITCH(DIB_FORMAT_RGB16_555_LE, DIB_FORMAT_RGB16_555_NATIVE, DIB_FORMAT_RGB16_555_SWAPPED),
  __DIB_FORMAT_SWITCH(DIB_FORMAT_RGB16_565_LE, DIB_FORMAT_RGB16_565_NATIVE, DIB_FORMAT_RGB16_565_SWAPPED),

  __DIB_FORMAT_SWITCH(DIB_FORMAT_RGB16_555_BE, DIB_FORMAT_RGB16_555_SWAPPED, DIB_FORMAT_RGB16_555_NATIVE),
  __DIB_FORMAT_SWITCH(DIB_FORMAT_RGB16_565_BE, DIB_FORMAT_RGB16_565_SWAPPED, DIB_FORMAT_RGB16_565_NATIVE)

#undef __DIB_FORMAT_SWITCH
};

// ============================================================================
// [Fog::ARGB32_MASK]
// ============================================================================

static const uint32_t ARGB32_RMASK = 0x00FF0000U;
static const uint32_t ARGB32_GMASK = 0x0000FF00U;
static const uint32_t ARGB32_BMASK = 0x000000FFU;
static const uint32_t ARGB32_AMASK = 0xFF000000U;

// ============================================================================
// [Fog::ARGB32_SHIFT]
// ============================================================================

enum ARGB32_SHIFT
{
  ARGB32_RSHIFT = 16U,
  ARGB32_GSHIFT =  8U,
  ARGB32_BSHIFT =  0U,
  ARGB32_ASHIFT = 24U
};

// ============================================================================
// [Fog::ARGB32_BYTEPOS]
// ============================================================================

enum ARGB32_BYTEPOS
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  ARGB32_RBYTE = 2,
  ARGB32_GBYTE = 1,
  ARGB32_BBYTE = 0,
  ARGB32_ABYTE = 3
#else // FOG_BYTE_ORDER == FOG_BIG_ENDIAN
  ARGB32_RBYTE = 1,
  ARGB32_GBYTE = 2,
  ARGB32_BBYTE = 3,
  ARGB32_ABYTE = 0
#endif // FOG_BYTE_ORDER
};

// ============================================================================
// [Fog::IMAGE_LIMITS]
// ============================================================================

//! @brief Image limits.
enum IMAGE_LIMITS
{
  //! @brief Maximum image width (in pixels).
  IMAGE_MAX_WIDTH = 16777215,
  //! @brief Maximum image height (in pixels).
  IMAGE_MAX_HEIGHT = 16777215
};

// ============================================================================
// [Fog::IMAGE_ADOPT]
// ============================================================================

//! @brief Image adopt flags.
enum IMAGE_ADOPT
{
  //! @brief Standard adopt behavior
  IMAGE_ADOPT_DEFAULT = 0x0,
  //! @brief Adopted image will be read-only.
  IMAGE_ATOPT_READ_ONLY = 0x1,
  //! @brief Adopted image data are from bottom-to-top (Windows DIBs).
  IMAGE_ADOPT_REVERSED = 0x2
};

// ============================================================================
// [Fog::IMAGE_MIRROR_MODE]
// ============================================================================

//! @brief Mirror modes used together with @c Image::mirror().
enum IMAGE_MIRROR_MODE
{
  IMAGE_MIRROR_NONE       = 0x0,
  IMAGE_MIRROR_HORIZONTAL = 0x1,
  IMAGE_MIRROR_VERTICAL   = 0x2,
  IMAGE_MIRROR_BOTH       = 0x3
};

// ============================================================================
// [Fog::IMAGE_ROTATE_MODE]
// ============================================================================

//! @brief Rotate modes used together with @c Image::rotate() methods.
enum IMAGE_ROTATE_MODE
{
  IMAGE_ROTATE_0   = 0x0,
  IMAGE_ROTATE_90  = 0x1,
  IMAGE_ROTATE_180 = 0x2,
  IMAGE_ROTATE_270 = 0x3
};

// ============================================================================
// [Fog::IMAGE_FILTER_TYPE]
// ============================================================================

//! @brief Type of image filter, see @c ImageFilter and @c ColorFilter classes.
enum IMAGE_FILTER_TYPE
{
  //! @brief Image filter is null (NOP).
  IMAGE_FILTER_TYPE_NONE = 0,

  // [ColorFilter filters]

  //! @brief @c ColorLut image filter.
  IMAGE_FILTER_TYPE_COLORLUT = 1,
  //! @brief @c ColorMatrix image filter.
  IMAGE_FILTER_TYPE_COLORMATRIX = 2,

  // [ImageFilter filters]

  //! @brief Image filter is box blur.
  IMAGE_FILTER_TYPE_BLUR = 3,
  //! @brief Image filter is convolution.
  IMAGE_FILTER_TYPE_CONVOLUTION = 4
};

// ============================================================================
// [Fog::IMAGE_FILTER_CHAR]
// ============================================================================

//! @brief Characteristics of image filter.
//!
//! Characteristics can be used to improve performance of filters by @c Painter.
enum IMAGE_FILTER_CHAR
{
  //! @brief Image filter does only color transformations.
  //!
  //! This flag must set all color filter, because it's very useful hint that
  //! enables very good code optimizations inside @c Painter and @c Image
  //! classes.
  IMAGE_FILTER_CHAR_COLOR_TRANSFORM = 0x0001,

  //! @brief Image filter can extend image boundary (blur and convolution
  //! filters).
  IMAGE_FILTER_CHAR_CAN_EXTEND = 0x0002,

  //! @brief Image filter constains standard processing mechanism - one pass.
  IMAGE_FILTER_CHAR_ENTIRE_PROCESSING = 0x0004,

  //! @brief When doing entire processing the destination and source buffers
  //! can be shared (dst and src pointers can point to same location).
  IMAGE_FILTER_CHAR_ENTIRE_MEM_EQUAL = 0x0008,

  //! @brief Image filter does vertical processing of image.
  //!
  //! This bit is set for all blur/convolution filters. Performance of filter
  //! is usually degraded, because filter processing function needs to access
  //!  pixels in different scanlines (cache misses, etc...).
  //!
  //! @note Vertical processing can be combined with horizontal processing and
  //! painter tries to make this combination efficient.
  IMAGE_FILTER_CHAR_VERT_PROCESSING = 0x0010,

  //! @brief When doing vertical processing the destination and source buffers
  //! can be shared (dst and src pointers can point to same location).
  IMAGE_FILTER_CHAR_VERT_MEM_EQUAL = 0x0020,

  //! @brief Image filter does horizontal processing of image.
  //!
  //! If filter needs only horizontal (no IMAGE_FILTER_VERT_PROCESSING bit is
  //! set) then processing it can be very efficient in multithreaded painter
  //! engine.
  IMAGE_FILTER_CHAR_HORZ_PROCESSING = 0x0040,

  //! @brief When doing vertical processing the destination and source buffers
  //! can be shared (dst and src pointers can point to same location).
  IMAGE_FILTER_CHAR_HORZ_MEM_EQUAL = 0x0080,

  //! @brief Contains both, @c IMAGE_FILTER_VERT_PROCESSING and
  //! @c IMAGE_FILTER_HORZ_PROCESSING flags.
  IMAGE_FILTER_CHAR_HV_PROCESSING =
    IMAGE_FILTER_CHAR_VERT_PROCESSING |
    IMAGE_FILTER_CHAR_HORZ_PROCESSING ,

  //! @brief Image filter supports @c PIXEL_FORMAT_PRGB32.
  //!
  //! If filters not supports this format the image data must be first converted
  //! to @c PIXEL_FORMAT_ARGB32, processed and then converted back.
  IMAGE_FILTER_CHAR_SUPPORTS_PRGB32 = 0x0100,

  //! @brief Image filter supports @c PIXEL_FORMAT_ARGB32.
  //!
  //! @note This flag should be always set (or at least 
  //! @c IMAGE_FILTER_FORMAT_PRGB32)!
  IMAGE_FILTER_CHAR_SUPPORTS_ARGB32 = 0x0200,

  //! @brief Image filter supports @c PIXEL_FORMAT_XRGB32.
  //!
  //! @note This flag should be always set!
  IMAGE_FILTER_CHAR_SUPPORTS_XRGB32 = 0x0400,

  //! @brief Image filter supports @c PIXEL_FORMAT_A8.
  //!
  //! @note This flag should be always set!
  IMAGE_FILTER_CHAR_SUPPORTS_A8 = 0x0800,

  //! @brief Image filter supports alpha-channel promotion. This means that
  //! source image without alpha-channel can be converted to image with
  //! alpha-channel.
  //!
  //! This operation is supported by all blur-filters (and should be supported
  //! generally by all filters that extend image boundary).
  IMAGE_FILTER_CHAR_PROMOTE_ALPHA = 0x1000
};

// ============================================================================
// [Fog::IMAGE_FILTER_BLUR_TYPE]
// ============================================================================

//! @brief Type of blur, see @c ImageFilter.
enum IMAGE_FILTER_BLUR_TYPE
{
  //! @brief Box blur type.
  //!
  //! Box blur is very bad looking blur, but it's fastest blur implemented
  //! in Fog library. Fog small radius it's quite good looking one. Box blur
  //! result looks very agressive.
  IMAGE_FILTER_BLUR_BOX = 0,

  //! @brief Linear blur (the default one).
  //!
  //! Linear blur provides very good looking blur with optimal performance.
  IMAGE_FILTER_BLUR_LINEAR = 1,

  //! @brief Gaussian blur type.
  //!
  //! Gaussian blur uses gaussian function to setup convolution matrix. It's
  //! slowest blur in Fog library, but the quality is excellent.
  IMAGE_FILTER_BLUR_GAUSSIAN = 2,

  //! @brief Count of blur types (for error checking).
  IMAGE_FILTER_BLUR_COUNT
};

//! @brief Maximum blur radius.
static const float IMAGE_FILTER_BLUR_MAX_RADIUS = 255.0f;

// ============================================================================
// [Fog::IMAGE_FILTER_EXTEND_TYPE]
// ============================================================================

//! @brief Border extend mode used in image filtering (convolution and blurs).
enum IMAGE_FILTER_EXTEND_TYPE
{
  //! @brief Borders are extended using pad.
  IMAGE_FILTER_EXTEND_PAD = 0,

  //! @brief Borders are extended using repead.
  IMAGE_FILTER_EXTEND_REPEAT = 1,

  //! @brief Borders are extended using reflect.
  IMAGE_FILTER_EXTEND_REFLECT = 2,

  //! @brief Borders are extended by custom single color.
  IMAGE_FILTER_EXTEND_COLOR = 3,

  //! @brief Count of border extend types (for error checking).
  IMAGE_FILTER_EXTEND_COUNT = 4
};

// ============================================================================
// [Fog::IMAGE_FILTER_ALPHA_MODE]
// ============================================================================

enum IMAGE_FILTER_ALPHA_MODE
{
  IMAGE_FILTER_ALPHA_NORMAL,
  IMAGE_FILTER_ALPHA_INCLUDE
};

// ============================================================================
// [Fog::IMAGE_IO_DEVICE_TYPE]
// ============================================================================

//! @brief Type of @c ImageIO::BaseDevice class.
enum IMAGE_IO_DEVICE_TYPE
{
  //! @brief None, null codec or non-initialized (shouldn't be used in public code).
  IMAGE_IO_DEVICE_NONE = 0x0,
  //! @brief Image IO Encoder.
  IMAGE_IO_DEVICE_ENCODER = 0x1,
  //! @brief Image IO Decoder.
  IMAGE_IO_DEVICE_DECODER = 0x2,
  //! @brief Image IO Decoder and Encoder.
  IMAGE_IO_DEVICE_BOTH = 0x3
};

// ============================================================================
// [Fog::IMAGE_IO_FILE_TYPE]
// ============================================================================

//! @brief Image file type.
enum IMAGE_IO_FILE_TYPE
{
  IMAGE_IO_FILE_NONE = 0,

  IMAGE_IO_FILE_ANI,
  IMAGE_IO_FILE_APNG,
  IMAGE_IO_FILE_BMP,
  IMAGE_IO_FILE_FLI,
  IMAGE_IO_FILE_FLC,
  IMAGE_IO_FILE_GIF,
  IMAGE_IO_FILE_ICO,
  IMAGE_IO_FILE_JPEG,
  IMAGE_IO_FILE_LBM,
  IMAGE_IO_FILE_MNG,
  IMAGE_IO_FILE_PCX,
  IMAGE_IO_FILE_PNG,
  IMAGE_IO_FILE_PNM,
  IMAGE_IO_FILE_TGA,
  IMAGE_IO_FILE_TIFF,
  IMAGE_IO_FILE_XBM,
  IMAGE_IO_FILE_XPM,

  IMAGE_IO_FILE_CUSTOM = 65536
};

// ============================================================================
// [Fog::INTERPOLATION_TYPE]
// ============================================================================

//! @brief Scale filter that can be using with @c Image::scale() or set in
//! painter.
enum INTERPOLATION_TYPE
{
  INTERPOLATION_NEAREST = 0,
  INTERPOLATION_SMOOTH = 1,

  INTERPOLATION_INVALID = 2
};

// ============================================================================
// [Fog::MATRIX_DATA]
// ============================================================================

//! @brief Matrix data offsets.
enum MATRIX_DATA
{
  //! @brief Scale X offset.
  MATRIX_SX = 0,
  //! @brief Scale Y offset.
  MATRIX_SY = 3,
  //! @brief Shear X offset.
  MATRIX_SHX = 2,
  //! @brief Shear Y offset.
  MATRIX_SHY = 1,
  //! @brief Translate X offset.
  MATRIX_TX = 4,
  //! @brief Translate Y offset.
  MATRIX_TY = 5
};

// ============================================================================
// [Fog::MATRIX_ORDER]
// ============================================================================

//! @brief Matrix multiply ordering.
enum MATRIX_ORDER
{
  MATRIX_PREPEND = 0,
  MATRIX_APPEND = 1
};

// ============================================================================
// [Fog::MATRIX_TYPE]
// ============================================================================

//! @brief Matrix type bit masks.
enum MATRIX_TYPE
{
  MATRIX_TYPE_IDENTITY = 0x00,
  MATRIX_TYPE_TRANSLATE = 0x01,
  MATRIX_TYPE_SCALE = 0x02,
  MATRIX_TYPE_SHEAR = 0x04
};

// ============================================================================
// [Fog::Fill Constants]
// ============================================================================

//! @brief Fill mode.
enum FILL_MODE
{
  //! @brief Fill using non-zero rule.
  FILL_NON_ZERO = 0,
  //! @brief Fill using even-odd rule.
  FILL_EVEN_ODD = 1,
  //! @brief Initial (default) fill rule for painter and rasterizer.
  FILL_DEFAULT = FILL_EVEN_ODD,

  //! @brief Used to catch invalid arguments.
  FILL_MODE_COUNT = 2
};

// ============================================================================
// [Fog::Stroke Constants]
// ============================================================================

static const double LINE_WIDTH_DEFAULT = 1.0;
static const double MITER_LIMIT_DEFAULT = 4.0;
static const double INNER_LIMIT_DEFAULT = 1.01;
static const double DASH_OFFSET_DEFAULT = 0.0;

//! @brief Line cap.
enum LINE_CAP
{
  LINE_CAP_BUTT = 0,
  LINE_CAP_SQUARE = 1,
  LINE_CAP_ROUND = 2,
  LINE_CAP_ROUND_REVERT = 3,
  LINE_CAP_TRIANGLE = 4,
  LINE_CAP_TRIANGLE_REVERT = 5,
  LINE_CAP_DEFAULT = LINE_CAP_BUTT,

  //! @brief Used to catch invalid arguments.
  LINE_CAP_COUNT = 6
};

//! @brief Line join.
enum LINE_JOIN
{
  LINE_JOIN_MITER = 0,
  LINE_JOIN_ROUND = 1,
  LINE_JOIN_BEVEL = 2,
  LINE_JOIN_MITER_REVERT = 3,
  LINE_JOIN_MITER_ROUND = 4,
  LINE_JOIN_DEFAULT = LINE_JOIN_MITER,

  //! @brief Used to catch invalid arguments.
  LINE_JOIN_COUNT = 5
};

//! @brief Inner join.
enum INNER_JOIN
{
  INNER_JOIN_MITER = 0,
  INNER_JOIN_BEVEL = 1,
  INNER_JOIN_ROUND = 2,
  INNER_JOIN_JAG = 3,
  INNER_JOIN_DEFAULT = INNER_JOIN_MITER,

  //! @brief Used to catch invalid arguments.
  INNER_JOIN_COUNT = 4
};

// ============================================================================
// [Fog::PAINTER_SOURCE_TYPE]
// ============================================================================

//! @brief Type of source assigned in @c Painter or @c PaintEngine.
enum PAINTER_SOURCE_TYPE
{
  //! @brief Painter source is ARGB color, see @c Painter::setSource(Argb argb).
  PAINTER_SOURCE_ARGB = 0,
  //! @brief Painter source is pattern color, see @c Painter::setPattern(const Pattern& pattern).
  PAINTER_SOURCE_PATTERN = 1,
  //! @brief Painter source is color filter, see @c Painter::setSource(const ColorFilter& colorFilter).
  PAINTER_SOURCE_COLOR_FILTER = 2
};

// ============================================================================
// [Fog::PAINTER_ENGINE]
// ============================================================================

//! @brief Type of painter engine.
enum PAINTER_ENGINE
{
  //! @brief Null painter engine (painter is not initialized or invalid).
  PAINTER_ENGINE_NULL = 0,

  //! @brief Singlethreaded raster painter engine.
  PAINTER_ENGINE_RASTER_ST = 1,

  //! @brief Multithreaded raster painter engine.
  PAINTER_ENGINE_RASTER_MT = 2
};

// ============================================================================
// [Fog::PAINTER_INIT]
// ============================================================================

//! @brief Painter initialization flags.
enum PAINTER_INIT_FLAGS
{
  //! @brief Initialize multithreading if it makes sense.
  //! 
  //! If this option is true, painter first check if image size is not too 
  //! small (painting to small images are singlethreaded by default). Then
  //! CPU detection is used to check if machine contains more CPU units.
  PAINTER_INIT_MT = 0x0001
};

// ============================================================================
// [Fog::PAINTER_HINT]
// ============================================================================

//! @brief Painter hint.
enum PAINTER_HINT
{
  //! @brief No hint.
  PAINTER_HINT_NONE = 0,

  //! @brief Anti-aliasing type (quality), see @c ANTI_ALIASING_TYPE.
  PAINTER_HINT_ANTIALIASING_QUALITY = 1,

  //! @brief Image interpolation type (quality), see @c INTERPOLATION_TYPE.
  PAINTER_HINT_IMAGE_INTERPOLATION = 2,

  //! @brief Gradient interpolation type (quality), see @c INTERPOLATION_TYPE.
  PAINTER_HINT_GRADIENT_INTERPOLATION = 3,

  //! @brief Whether to render text only using path outlines (vectors).
  //!
  //! If font do not support vector data then bitmaps are used. This flag makes
  //! sense mainly if you are using true-type fonts and text transformations.
  PAINTER_HINT_OUTLINE_TEXT = 4
};

// ============================================================================
// [Fog::PAINTER_FLUSH_FLAGS]
// ============================================================================

//! @brief Painter flush flags.
enum PAINTER_FLUSH_FLAGS
{
  //! @brief Flush all painter commands and wait for completition. Use this
  //! command if you want to access painter data after the flush() function
  //! call.
  PAINTER_FLUSH_SYNC = 0x0001
};

// ============================================================================
// [Fog::PATH_CMD_DATA]
// ============================================================================

//! @brief Command data type and flags.
enum PATH_CMD_DATA
{
  PATH_CMD_STOP = 0x00,
  PATH_CMD_MOVE_TO = 0x01,
  PATH_CMD_LINE_TO = 0x02,
  PATH_CMD_CURVE_3 = 0x04,
  PATH_CMD_CURVE_4 = 0x06,
  PATH_CMD_END = 0x08,

  PATH_CMD_FLAG_NONE = 0,
  PATH_CMD_FLAG_CLOSE = 0x10,
  PATH_CMD_FLAG_CW = 0x20,
  PATH_CMD_FLAG_CCW = 0x40,

  PATH_CMD_TYPE_MASK = 0x0F,
  PATH_CMD_VERTEX_MASK = 0x07,
  PATH_CMD_CURVE_MASK = 0x04,
  PATH_CMD_FLAG_MASK = 0xF0
};

// ============================================================================
// [Fog::PATH_DIRECTION]
// ============================================================================

//! @brief Path or path object direction.
enum PATH_DIRECTION
{
  //! @brief Clockwise direction.
  PATH_DIRECTION_CW = 1,
  //! @brief Counter-clockwise direction.
  PATH_DIRECTION_CCW = -1
};

// ============================================================================
// [Fog::PATTERN_TYPE]
// ============================================================================

//! @brief Type of pattern in the @c Pattern instance.
enum PATTERN_TYPE
{
  //! @brief Null pattern (nothing will be paint using this pattern).
  PATTERN_NULL = 0x0,
  //! @brief Solid color pattern.
  PATTERN_SOLID = 0x1,
  //! @brief Texture pattern (@c Image).
  PATTERN_TEXTURE = 0x2,

  //! @brief Mask for all gradient patterns.
  PATTERN_GRADIENT_MASK = 0x10,

  //! @brief Linear gradient pattern.
  PATTERN_LINEAR_GRADIENT = PATTERN_GRADIENT_MASK | 0x0,
  //! @brief Radial gradient pattern.
  PATTERN_RADIAL_GRADIENT = PATTERN_GRADIENT_MASK | 0x1,
  //! @brief Conical gradient pattern.
  PATTERN_CONICAL_GRADIENT = PATTERN_GRADIENT_MASK | 0x2
};

// ============================================================================
// [Fog::REGION_TYPE]
// ============================================================================

//! @brief Type of @c Region.
enum REGION_TYPE
{
  //! @brief Region is empty.
  REGION_TYPE_EMPTY = 0,
  //! @brief Region has only one rectangle (rectangular).
  REGION_TYPE_SIMPLE = 1,
  //! @brief Region has more YX sorted rectangles.
  REGION_TYPE_COMPLEX = 2
};

// ============================================================================
// [Fog::REGION_OP]
// ============================================================================

//! @brief Region operators.
enum REGION_OP
{
  //! @brief Copy.*/
  REGION_OP_COPY = 0,
  //! @brief Union (OR)
  REGION_OP_UNITE = 1,
  //! @brief Intersection (AND).
  REGION_OP_INTERSECT = 2,
  //! @brief eXclusive or (XOR).
  REGION_OP_EOR = 3,
  //! @brief eXclusive or (XOR).
  REGION_OP_XOR = 3,
  //! @brief Subtraction (Difference).
  REGION_OP_SUBTRACT = 4,

  //! @brief Count of region operators.
  REGION_OP_COUNT
};

// ============================================================================
// [Fog::REGION_HITTEST]
// ============================================================================

//! @brief Region hit-test result.
enum REGION_HITTEST
{
  //! @brief Object isn't in region (point, rectangle or another region).
  REGION_HITTEST_OUT = 0,
  //! @brief Object is in region (point, rectangle or another region).
  REGION_HITTEST_IN = 1,
  //! @brief Object is partially in region (point, rectangle or another region).
  REGION_HITTEST_PART = 2
};

// ============================================================================
// [Fog::SPREAD_TYPE]
// ============================================================================

//! @brief Spread of @c Pattern.
enum SPREAD_TYPE
{
  //! @brief No spread (area outside the pattern definition is transparent).
  SPREAD_NONE = 0,
  //! @brief Pad spread (area outside the pattern continues by border color).
  SPREAD_PAD = 1,
  //! @brief Releat spread (pattern is repeated).
  SPREAD_REPEAT = 2,
  //! @brief Reflect spread (pattern is reflected and then repeated).
  SPREAD_REFLECT = 3,

  //! @brief Count of pattern spreads.
  SPREAD_COUNT = 4
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
// [Fog::ERR_GRAPHICS]
// ============================================================================

//! @brief Error codes used in Fog-Graphics.
enum ERR_GRAPHICS_ENUM
{
  // [Errors Range]
  ERR_GRAPHICS_START = 0x00011100,
  ERR_GRAPHICS_LAST  = 0x000111FF,

  // Path Errors.

  ERR_PATH_INVALID = ERR_GRAPHICS_START,

  // Image, ImageFilter, ImageIO and Painter Errors.

  ERR_IMAGE_INVALID_SIZE,
  ERR_IMAGE_INVALID_FORMAT,

  ERR_IMAGE_UNSUPPORTED_FORMAT,

  ERR_IMAGEIO_INTERNAL_ERROR,

  ERR_IMAGEIO_NO_DECODER,
  ERR_IMAGEIO_NO_ENCODER,

  ERR_IMAGEIO_UNSUPPORTED_FORMAT,
  ERR_IMAGEIO_TERMINATED,
  ERR_IMAGEIO_TRUNCATED,

  ERR_IMAGEIO_MIME_NOT_MATCH,
  ERR_IMAGEIO_MALFORMED_HEADER,
  ERR_IMAGEIO_MALFORMED_RLE,
  ERR_IMAGEIO_NO_MORE_FRAMES,

  ERR_IMAGEIO_LIBJPEG_NOT_LOADED,
  ERR_IMAGEIO_LIBJPEG_ERROR,

  ERR_IMAGEIO_LIBPNG_NOT_LOADED,
  ERR_IMAGEIO_LIBPNG_ERROR,

  ERR_IMAGEIO_GDIPLUS_NOT_LOADED,
  ERR_IMAGEIO_GDIPLUS_ERROR,

  // Font Errors.

  ERR_FONT_NOT_MATCHED,
  ERR_FONT_INVALID_FACE,
  ERR_FONT_INVALID_DATA,
  ERR_FONT_CANT_LOAD_DEFAULT_FACE,
  ERR_FONT_CANT_GET_OUTLINE,

  ERR_FONT_FONTCONFIG_NOT_LOADED,
  ERR_FONT_FONTCONFIG_INIT_FAILED,

  ERR_FONT_FREETYPE_NOT_LOADED,
  ERR_FONT_FREETYPE_INIT_FAILED
};

extern FOG_API uint32_t OperatorCharacteristics[OPERATOR_COUNT];

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_CONSTANTS_H
