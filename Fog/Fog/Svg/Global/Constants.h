// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_GLOBAL_CONSTANTS_H
#define _FOG_SVG_GLOBAL_CONSTANTS_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>

namespace Fog {

//! @addtogroup Fog_Svg_Constants
//! @{

// ============================================================================
// [Fog::SVG_ELEMENT_TYPE]
// ============================================================================

enum SVG_ELEMENT_TYPE
{
  SVG_ELEMENT_NONE = 0,

  SVG_ELEMENT_A,
  SVG_ELEMENT_CIRCLE,
  SVG_ELEMENT_CLIP_PATH,
  SVG_ELEMENT_DEFS,
  SVG_ELEMENT_DESC,
  SVG_ELEMENT_ELLIPSE,
  SVG_ELEMENT_G,
  SVG_ELEMENT_IMAGE,
  SVG_ELEMENT_LINE,
  SVG_ELEMENT_LINEAR_GRADIENT,
  SVG_ELEMENT_MARKER,
  SVG_ELEMENT_PATH,
  SVG_ELEMENT_PATTERN,
  SVG_ELEMENT_POLYGON,
  SVG_ELEMENT_POLYLINE,
  SVG_ELEMENT_RADIAL_GRADIENT,
  SVG_ELEMENT_RECT,
  SVG_ELEMENT_SOLID_COLOR,
  SVG_ELEMENT_STOP,
  SVG_ELEMENT_SVG,
  SVG_ELEMENT_SYMBOL,
  SVG_ELEMENT_TEXT,
  SVG_ELEMENT_TEXT_PATH,
  SVG_ELEMENT_TITLE,
  SVG_ELEMENT_TREF,
  SVG_ELEMENT_TSPAN,
  SVG_ELEMENT_USE,
  SVG_ELEMENT_VIEW
};

// ============================================================================
// [Fog::SVG_PAINT_TYPE]
// ============================================================================

enum SVG_PAINT_TYPE
{
  SVG_PAINT_FILL = 0,
  SVG_PAINT_STROKE = 1
};

// ============================================================================
// [Fog::SVG_STYLE_ID]
// ============================================================================

enum SVG_STYLE_ID
{
  // When adding value here, it's important to add value to Fog/Core/Tools/Strings.h
  // and Fog/Core/Tools/Strings.cpp. There are static strings for style names.
  SVG_STYLE_CLIP_PATH = 0,
  SVG_STYLE_CLIP_RULE,
  SVG_STYLE_ENABLE_BACKGROUND,
  SVG_STYLE_FILL,
  SVG_STYLE_FILL_OPACITY,
  SVG_STYLE_FILL_RULE,
  SVG_STYLE_FILTER,
  SVG_STYLE_FONT_FAMILY,
  SVG_STYLE_FONT_SIZE,
  SVG_STYLE_LETTER_SPACING,
  SVG_STYLE_MASK,
  SVG_STYLE_OPACITY,
  SVG_STYLE_STOP_COLOR,
  SVG_STYLE_STOP_OPACITY,
  SVG_STYLE_STROKE,
  SVG_STYLE_STROKE_DASH_ARRAY,
  SVG_STYLE_STROKE_DASH_OFFSET,
  SVG_STYLE_STROKE_LINE_CAP,
  SVG_STYLE_STROKE_LINE_JOIN,
  SVG_STYLE_STROKE_MITER_LIMIT,
  SVG_STYLE_STROKE_OPACITY,
  SVG_STYLE_STROKE_WIDTH,

  SVG_STYLE_INVALID
};

// ============================================================================
// [Fog::SVG_SOURCE_TYPE]
// ============================================================================

enum SVG_SOURCE_TYPE
{
  SVG_SOURCE_NONE = 0,
  SVG_SOURCE_COLOR = 1,
  SVG_SOURCE_URI = 2,

  SVG_SOURCE_INVALID
};

// ============================================================================
// [Fog::SVG_PATTERN_UNITS]
// ============================================================================

enum SVG_PATTERN_UNITS
{
  SVG_OBJECT_BOUNDING_BOX = 0,
  SVG_USER_SPACE_ON_USE = 1
};

// ============================================================================
// [Fog::SVG_LENGTH_ADJUST]
// ============================================================================

enum SVG_LENGTH_ADJUST
{
  SVG_LENGTH_ADJUST_SPACING = 0,
  SVG_LENGTH_ADJUST_SPACING_AND_GLYPHS = 1
};

// ============================================================================
// [Fog::SVG_VISITOR]
// ============================================================================

//! @brief Type of SVG visitor context.
enum SVG_VISITOR
{
  SVG_VISITOR_MEASURE = 0,
  SVG_VISITOR_RENDER = 1
};

// ============================================================================
// [Fog::ERR_SVG]
// ============================================================================

//! @brief Error codes used in Fog-Svg.
enum ERR_SVG_ENUM
{
  // Errors Range.
  ERR_SVG_START = 0x00011100,
  ERR_SVG_LAST  = 0x000111FF,

  // Svg Errors.

  //! @brief Invalid style name.
  ERR_SVG_INVALID_STYLE_NAME,
  //! @brief Invalid style value.
  ERR_SVG_INVALID_STYLE_VALUE,
  //! @brief Error set if svg embedded data are not in base64 format.
  ERR_SVG_INVALID_DATA_ENCODING = ERR_SVG_START
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_GLOBAL_CONSTANTS_H
