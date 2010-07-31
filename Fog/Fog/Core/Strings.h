// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_STRINGS_H
#define _FOG_CORE_STRINGS_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/ManagedString.h>

namespace Fog {

//! @addtogroup Fog_Core_String
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
  // [Core - Object]

  STR_CORE_id = 0,
  STR_CORE_name,

  // [Core - Properties]

  // reserved...

  // [Graphics - Image File Types]

  STR_GRAPHICS_ANI,
  STR_GRAPHICS_APNG,
  STR_GRAPHICS_BMP,
  STR_GRAPHICS_FLI,
  STR_GRAPHICS_FLC,
  STR_GRAPHICS_GIF,
  STR_GRAPHICS_ICO,
  STR_GRAPHICS_JPEG,
  STR_GRAPHICS_LBM,
  STR_GRAPHICS_MNG,
  STR_GRAPHICS_PCX,
  STR_GRAPHICS_PNG,
  STR_GRAPHICS_PNM,
  STR_GRAPHICS_TGA,
  STR_GRAPHICS_TIFF,
  STR_GRAPHICS_XBM,
  STR_GRAPHICS_XPM,

  // [Graphics - Image File Extensions]

  STR_GRAPHICS_ani,
  STR_GRAPHICS_apng,
  STR_GRAPHICS_bmp,
  STR_GRAPHICS_fli,
  STR_GRAPHICS_flc,
  STR_GRAPHICS_gif,
  STR_GRAPHICS_ico,
  STR_GRAPHICS_jfi,
  STR_GRAPHICS_jfif,
  STR_GRAPHICS_jpg,
  STR_GRAPHICS_jpeg,
  STR_GRAPHICS_lbm,
  STR_GRAPHICS_mng,
  STR_GRAPHICS_pcx,
  STR_GRAPHICS_png,
  STR_GRAPHICS_pnm,
  STR_GRAPHICS_ras,
  STR_GRAPHICS_tga,
  STR_GRAPHICS_tif,
  STR_GRAPHICS_tiff,
  STR_GRAPHICS_xbm,
  STR_GRAPHICS_xpm,

  // [Graphics - ImageIO Properties]

  STR_GRAPHICS_width,
  STR_GRAPHICS_height,
  STR_GRAPHICS_depth,
  STR_GRAPHICS_planes,
  STR_GRAPHICS_actualFrame,
  STR_GRAPHICS_framesCount,
  STR_GRAPHICS_progress,
  STR_GRAPHICS_quality,
  STR_GRAPHICS_compression,
  STR_GRAPHICS_skipFileHeader,

  // [Xml]

  STR_XML_unnamed,
  STR_XML_id,
  STR_XML_style,

  STR_XML__text,
  STR_XML__cdata,
  STR_XML__pi,
  STR_XML__comment,
  STR_XML__document,

  // [Svg - Elements]

  STR_SVG_none,
  STR_SVG_circle,
  STR_SVG_defs,
  STR_SVG_ellipse,
  STR_SVG_g,
  STR_SVG_image,
  STR_SVG_line,
  STR_SVG_linearGradient,
  STR_SVG_path,
  STR_SVG_pattern,
  STR_SVG_polygon,
  STR_SVG_polyline,
  STR_SVG_radialGradient,
  STR_SVG_rect,
  STR_SVG_solidColor,
  STR_SVG_stop,
  STR_SVG_svg,
  STR_SVG_text,
  STR_SVG_textPath,
  STR_SVG_tref,
  STR_SVG_tspan,
  STR_SVG_use,

  // [Svg - Attributes]

  STR_SVG_cx,
  STR_SVG_cy,
  STR_SVG_d,
  STR_SVG_fx,
  STR_SVG_fy,
  STR_SVG_gradientTransform,
  STR_SVG_gradientUnits,
  STR_SVG_height,
  STR_SVG_offset,
  STR_SVG_points,
  STR_SVG_r,
  STR_SVG_rx,
  STR_SVG_ry,
  STR_SVG_spreadMethod,
  STR_SVG_transform,
  STR_SVG_width,
  STR_SVG_x,
  STR_SVG_x1,
  STR_SVG_x2,
  STR_SVG_xlink_href,
  STR_SVG_y,
  STR_SVG_y1,
  STR_SVG_y2,

  // [Svg - Attributes / Styles]

  STR_SVG_STYLE_NAMES,
  STR_SVG_clip_path = STR_SVG_STYLE_NAMES,
  STR_SVG_clip_rule,
  STR_SVG_enable_background,
  STR_SVG_fill,
  STR_SVG_fill_opacity,
  STR_SVG_fill_rule,
  STR_SVG_filter,
  STR_SVG_font_family,
  STR_SVG_font_size,
  STR_SVG_letter_spacing,
  STR_SVG_mask,
  STR_SVG_opacity,
  STR_SVG_stop_color,
  STR_SVG_stop_opacity,
  STR_SVG_stroke,
  STR_SVG_stroke_dasharray,
  STR_SVG_stroke_dashoffset,
  STR_SVG_stroke_linecap,
  STR_SVG_stroke_linejoin,
  STR_SVG_stroke_miterlimit,
  STR_SVG_stroke_opacity,
  STR_SVG_stroke_width,

  // [Platform Specific]
#if defined(FOG_OS_WINDOWS)
  STR_PLATFORM_USERPROFILE,
#endif // FOG_OS_WINDOWS

  // [Count]

  STR_COUNT
};

extern FOG_API ManagedString::Cache* fog_strings;

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_STRINGS_H
