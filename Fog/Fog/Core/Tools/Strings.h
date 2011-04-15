// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_STRINGS_H
#define _FOG_CORE_TOOLS_STRINGS_H

// [Dependencies]
#include <Fog/Core/Tools/ManagedString.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct StringCache;

// ============================================================================
// [Fog::fog_strings]
// ============================================================================

//! @brief Cached strings IDs used in @c fog_strings array.
enum STR_ID
{
  // --------------------------------------------------------------------------
  // [Fog/Core - Object]
  // --------------------------------------------------------------------------

  STR_CORE_OBJECT_id = 0,
  STR_CORE_OBJECT_name,

  // --------------------------------------------------------------------------
  // [Fog/Core - Properties]
  // --------------------------------------------------------------------------

  // reserved...

  // --------------------------------------------------------------------------
  // [Fog/G2d/Imaging - Stream Types]
  // --------------------------------------------------------------------------

  STR_G2D_STREAM_ANI,
  STR_G2D_STREAM_APNG,
  STR_G2D_STREAM_BMP,
  STR_G2D_STREAM_FLI,
  STR_G2D_STREAM_FLC,
  STR_G2D_STREAM_GIF,
  STR_G2D_STREAM_ICO,
  STR_G2D_STREAM_JPEG,
  STR_G2D_STREAM_LBM,
  STR_G2D_STREAM_MNG,
  STR_G2D_STREAM_PCX,
  STR_G2D_STREAM_PNG,
  STR_G2D_STREAM_PNM,
  STR_G2D_STREAM_TGA,
  STR_G2D_STREAM_TIFF,
  STR_G2D_STREAM_XBM,
  STR_G2D_STREAM_XPM,

  // --------------------------------------------------------------------------
  // [Fog/G2d/Imaging - Stream Extensions]
  // --------------------------------------------------------------------------

  STR_G2D_EXTENSION_ani,
  STR_G2D_EXTENSION_apng,
  STR_G2D_EXTENSION_bmp,
  STR_G2D_EXTENSION_fli,
  STR_G2D_EXTENSION_flc,
  STR_G2D_EXTENSION_gif,
  STR_G2D_EXTENSION_ico,
  STR_G2D_EXTENSION_jfi,
  STR_G2D_EXTENSION_jfif,
  STR_G2D_EXTENSION_jpg,
  STR_G2D_EXTENSION_jpeg,
  STR_G2D_EXTENSION_lbm,
  STR_G2D_EXTENSION_mng,
  STR_G2D_EXTENSION_pcx,
  STR_G2D_EXTENSION_png,
  STR_G2D_EXTENSION_pnm,
  STR_G2D_EXTENSION_ras,
  STR_G2D_EXTENSION_tga,
  STR_G2D_EXTENSION_tif,
  STR_G2D_EXTENSION_tiff,
  STR_G2D_EXTENSION_xbm,
  STR_G2D_EXTENSION_xpm,

  // --------------------------------------------------------------------------
  // [Fog/G2d/Imaging - Device Properties]
  // --------------------------------------------------------------------------

  STR_G2D_CODEC_width,
  STR_G2D_CODEC_height,
  STR_G2D_CODEC_depth,
  STR_G2D_CODEC_planes,
  STR_G2D_CODEC_actualFrame,
  STR_G2D_CODEC_framesCount,
  STR_G2D_CODEC_progress,
  STR_G2D_CODEC_quality,
  STR_G2D_CODEC_compression,
  STR_G2D_CODEC_skipFileHeader,

  // --------------------------------------------------------------------------
  // [Fog/Xml]
  // --------------------------------------------------------------------------

  STR_XML_unnamed,
  STR_XML_ATTRIBUTE_id,
  STR_XML_ATTRIBUTE_style,

  STR_XML__text,
  STR_XML__cdata,
  STR_XML__pi,
  STR_XML__comment,
  STR_XML__document,

  // --------------------------------------------------------------------------
  // [Fog/Svg - Elements]
  // --------------------------------------------------------------------------

  STR_SVG_ELEMENT_none,
  STR_SVG_ELEMENT_a,
  STR_SVG_ELEMENT_circle,
  STR_SVG_ELEMENT_clipPath,
  STR_SVG_ELEMENT_defs,
  STR_SVG_ELEMENT_ellipse,
  STR_SVG_ELEMENT_g,
  STR_SVG_ELEMENT_image,
  STR_SVG_ELEMENT_line,
  STR_SVG_ELEMENT_linearGradient,
  STR_SVG_ELEMENT_marker,
  STR_SVG_ELEMENT_mask,
  STR_SVG_ELEMENT_path,
  STR_SVG_ELEMENT_pattern,
  STR_SVG_ELEMENT_polygon,
  STR_SVG_ELEMENT_polyline,
  STR_SVG_ELEMENT_radialGradient,
  STR_SVG_ELEMENT_rect,
  STR_SVG_ELEMENT_solidColor,
  STR_SVG_ELEMENT_stop,
  STR_SVG_ELEMENT_svg,
  STR_SVG_ELEMENT_symbol,
  STR_SVG_ELEMENT_text,
  STR_SVG_ELEMENT_textPath,
  STR_SVG_ELEMENT_tref,
  STR_SVG_ELEMENT_tspan,
  STR_SVG_ELEMENT_use,
  STR_SVG_ELEMENT_view,

  // --------------------------------------------------------------------------
  // [Fog/Svg - Attributes]
  // --------------------------------------------------------------------------

  STR_SVG_ATTRIBUTE_angle,
  STR_SVG_ATTRIBUTE_cx,
  STR_SVG_ATTRIBUTE_cy,
  STR_SVG_ATTRIBUTE_d,
  STR_SVG_ATTRIBUTE_dx,
  STR_SVG_ATTRIBUTE_dy,
  STR_SVG_ATTRIBUTE_fx,
  STR_SVG_ATTRIBUTE_fy,
  STR_SVG_ATTRIBUTE_gradientTransform,
  STR_SVG_ATTRIBUTE_gradientUnits,
  STR_SVG_ATTRIBUTE_height,
  STR_SVG_ATTRIBUTE_lengthAdjust,
  STR_SVG_ATTRIBUTE_offset,
  STR_SVG_ATTRIBUTE_patternUnits,
  STR_SVG_ATTRIBUTE_points,
  STR_SVG_ATTRIBUTE_r,
  STR_SVG_ATTRIBUTE_rotate,
  STR_SVG_ATTRIBUTE_rx,
  STR_SVG_ATTRIBUTE_ry,
  STR_SVG_ATTRIBUTE_spreadMethod,
  STR_SVG_ATTRIBUTE_textLength,
  STR_SVG_ATTRIBUTE_transform,
  STR_SVG_ATTRIBUTE_width,
  STR_SVG_ATTRIBUTE_x,
  STR_SVG_ATTRIBUTE_x1,
  STR_SVG_ATTRIBUTE_x2,
  STR_SVG_ATTRIBUTE_xlink_href,
  STR_SVG_ATTRIBUTE_y,
  STR_SVG_ATTRIBUTE_y1,
  STR_SVG_ATTRIBUTE_y2,

  // --------------------------------------------------------------------------
  // [Fog/Svg - Attributes / Styles]
  // --------------------------------------------------------------------------

  STR_SVG_STYLE_NAMES,
  STR_SVG_STYLE_clip_path = STR_SVG_STYLE_NAMES,
  STR_SVG_STYLE_clip_rule,
  STR_SVG_STYLE_enable_background,
  STR_SVG_STYLE_fill,
  STR_SVG_STYLE_fill_opacity,
  STR_SVG_STYLE_fill_rule,
  STR_SVG_STYLE_filter,
  STR_SVG_STYLE_font_family,
  STR_SVG_STYLE_font_size,
  STR_SVG_STYLE_letter_spacing,
  STR_SVG_STYLE_mask,
  STR_SVG_STYLE_opacity,
  STR_SVG_STYLE_stop_color,
  STR_SVG_STYLE_stop_opacity,
  STR_SVG_STYLE_stroke,
  STR_SVG_STYLE_stroke_dasharray,
  STR_SVG_STYLE_stroke_dashoffset,
  STR_SVG_STYLE_stroke_linecap,
  STR_SVG_STYLE_stroke_linejoin,
  STR_SVG_STYLE_stroke_miterlimit,
  STR_SVG_STYLE_stroke_opacity,
  STR_SVG_STYLE_stroke_width,

  // --------------------------------------------------------------------------
  // [Platform Specific]
  // --------------------------------------------------------------------------

#if defined(FOG_OS_WINDOWS)
  STR_PLATFORM_USERPROFILE,
#endif // FOG_OS_WINDOWS

  // --------------------------------------------------------------------------
  // [...]
  // --------------------------------------------------------------------------

  STR_COUNT
};

extern FOG_API ManagedString::Cache* fog_strings;

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_STRINGS_H
