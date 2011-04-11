// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Core/Tools/Strings.h>

namespace Fog {

// ============================================================================
// [Fog::fog_strings]
// ============================================================================

static const char fog_strings_data[] =
{
  // [Core - Object]

  "id\0"
  "name\0"

  // [Core - Properties]

  // reserved...

  // [Graphics - Imaging - Stream Types]

  "ANI\0"
  "APNG\0"
  "BMP\0"
  "FLI\0"
  "FLC\0"
  "GIF\0"
  "ICO\0"
  "JPEG\0"
  "LBM\0"
  "MNG\0"
  "PCX\0"
  "PNG\0"
  "PNM\0"
  "TGA\0"
  "TIFF\0"
  "XBM\0"
  "XPM\0"

  // [Graphics - Imaging - Stream Extensions]

  "ani\0"
  "apng\0"
  "bmp\0"
  "fli\0"
  "flc\0"
  "gif\0"
  "ico\0"
  "jfi\0"
  "jfif\0"
  "jpg\0"
  "jpeg\0"
  "lbm\0"
  "mng\0"
  "pcx\0"
  "png\0"
  "pnm\0"
  "ras\0"
  "tga\0"
  "tif\0"
  "tiff\0"
  "xbm\0"
  "xpm\0"

  // [Graphics - Imaging - Device Properties]

  "width\0"
  "height\0"
  "depth\0"
  "planes\0"
  "actualFrame\0"
  "framesCount\0"
  "progress\0"
  "quality\0"
  "compression\0"
  "skipFileHeader\0"

  // [Xml]

  "unnamed\0"
  "id\0"
  "style\0"

  "#text\0"
  "#cdata\0"
  "#pi\0"
  "#comment\0"
  "#document\0"

  // [Svg]

  "none\0"
  "a\0"
  "circle\0"
  "clipPath\0"
  "defs\0"
  "ellipse\0"
  "g\0"
  "image\0"
  "line\0"
  "linearGradient\0"
  "marker\0"
  "mask\0"
  "path\0"
  "pattern\0"
  "polygon\0"
  "polyline\0"
  "radialGradient\0"
  "rect\0"
  "solidColor\0"
  "stop\0"
  "symbol\0"
  "svg\0"
  "text\0"
  "textPath\0"
  "tref\0"
  "tspan\0"
  "use\0"
  "view\0"

  "angle\0"
  "cx\0"
  "cy\0"
  "d\0"
  "dx\0"
  "dy\0"
  "fx\0"
  "fy\0"
  "gradientTransform\0"
  "gradientUnits\0"
  "height\0"
  "lengthAdjust\0"
  "offset\0"
  "points\0"
  "r\0"
  "rotate\0"
  "rx\0"
  "ry\0"
  "spreadMethod\0"
  "textLength\0"
  "transform\0"
  "width\0"
  "x\0"
  "x1\0"
  "x2\0"
  "xlink:href\0"
  "y\0"
  "y1\0"
  "y2\0"

  "clip-path\0"
  "clip-rule\0"
  "enable-background\0"
  "fill\0"
  "fill-opacity\0"
  "fill-rule\0"
  "filter\0"
  "font-family\0"
  "font-size\0"
  "letter-spacing\0"
  "mask\0"
  "opacity\0"
  "stop-color\0"
  "stop-opacity\0"
  "stroke\0"
  "stroke-dasharray\0"
  "stroke-dashoffset\0"
  "stroke-linecap\0"
  "stroke-linejoin\0"
  "stroke-miterlimit\0"
  "stroke-opacity\0"
  "stroke-width\0"

  // [Platform Specific]

#if defined(FOG_OS_WINDOWS)
  "USERPROFILE\0"
#endif // FOG_OS_WINDOWS
};

Fog::ManagedString::Cache* fog_strings;

// ============================================================================
// [Fog::Core - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _core_strings_init(void)
{
  fog_strings = ManagedString::createCache(
    fog_strings_data,
    FOG_ARRAY_SIZE(fog_strings_data),
    STR_COUNT,
    Ascii8("Fog")
  );
}

FOG_NO_EXPORT void _core_strings_fini(void)
{
}

} // Fog namespace
