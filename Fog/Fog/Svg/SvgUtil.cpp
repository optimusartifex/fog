// [Fog-Svg Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Math.h>
#include <Fog/Core/StringUtil.h>
#include <Fog/Graphics/Matrix.h>
#include <Fog/Svg/Constants.h>
#include <Fog/Svg/SvgUtil.h>

namespace Fog {
namespace SvgUtil {

// ============================================================================
// [Fog::SvgUtil - Color & Opacity]
// ============================================================================

#include <Fog/Core/Compiler/PackByte.h>
struct SvgNamedColor
{
  char name[22];
  uint32_t value;
};
#include <Fog/Core/Compiler/PackRestore.h>

#define __SVG_COLOR(R, G, B) \
  ( 0xFF000000U | (((uint32_t)R) << 16) | (((uint32_t)G) << 8) | ((uint32_t)B) )
static const SvgNamedColor svgNamedColors[] =
{
  { "aliceblue",             __SVG_COLOR(240,248,255) },
  { "antiquewhite",          __SVG_COLOR(250,235,215) },
  { "aqua",                  __SVG_COLOR(0  ,255,255) },
  { "aquamarine",            __SVG_COLOR(127,255,212) },
  { "azure",                 __SVG_COLOR(240,255,255) },
  { "beige",                 __SVG_COLOR(245,245,220) },
  { "bisque",                __SVG_COLOR(255,228,196) },
  { "black",                 __SVG_COLOR(0  ,0,  0  ) },
  { "blanchedalmond",        __SVG_COLOR(255,235,205) },
  { "blue",                  __SVG_COLOR(0  ,0  ,255) },
  { "blueviolet",            __SVG_COLOR(138,43 ,226) },
  { "brown",                 __SVG_COLOR(165,42 ,42) },
  { "burlywood",             __SVG_COLOR(222,184,135) },
  { "cadetblue",             __SVG_COLOR(95 ,158,160) },
  { "chartreuse",            __SVG_COLOR(127,255,0  ) },
  { "chocolate",             __SVG_COLOR(210,105,30 ) },
  { "coral",                 __SVG_COLOR(255,127,80 ) },
  { "cornflowerblue",        __SVG_COLOR(100,149,237) },
  { "cornsilk",              __SVG_COLOR(255,248,220) },
  { "crimson",               __SVG_COLOR(220,20 ,60 ) },
  { "cyan",                  __SVG_COLOR(0  ,255,255) },
  { "darkblue",              __SVG_COLOR(0  ,0  ,139) },
  { "darkcyan",              __SVG_COLOR(0  ,139,139) },
  { "darkgoldenrod",         __SVG_COLOR(184,134,11 ) },
  { "darkgray",              __SVG_COLOR(169,169,169) },
  { "darkgreen",             __SVG_COLOR(0  ,100,0  ) },
  { "darkgrey",              __SVG_COLOR(169,169,169) },
  { "darkkhaki",             __SVG_COLOR(189,183,107) },
  { "darkmagenta",           __SVG_COLOR(139,0  ,139) },
  { "darkolivegreen",        __SVG_COLOR(85 ,107,47 ) },
  { "darkorange",            __SVG_COLOR(255,140,0  ) },
  { "darkorchid",            __SVG_COLOR(153,50 ,204) },
  { "darkred",               __SVG_COLOR(139,0  ,0  ) },
  { "darksalmon",            __SVG_COLOR(233,150,122) },
  { "darkseagreen",          __SVG_COLOR(143,188,143) },
  { "darkslateblue",         __SVG_COLOR(72 ,61 ,139) },
  { "darkslategray",         __SVG_COLOR(47 ,79 ,79 ) },
  { "darkslategrey",         __SVG_COLOR(47 ,79 ,79 ) },
  { "darkturquoise",         __SVG_COLOR(0  ,206,209) },
  { "darkviolet",            __SVG_COLOR(148,0  ,211) },
  { "deeppink",              __SVG_COLOR(255,20 ,147) },
  { "deepskyblue",           __SVG_COLOR(0  ,191,255) },
  { "dimgray",               __SVG_COLOR(105,105,105) },
  { "dimgrey",               __SVG_COLOR(105,105,105) },
  { "dodgerblue",            __SVG_COLOR(30 ,144,255) },
  { "firebrick",             __SVG_COLOR(178,34 ,34 ) },
  { "floralwhite",           __SVG_COLOR(255,250,240) },
  { "forestgreen",           __SVG_COLOR(34 ,139,34 ) },
  { "fuchsia",               __SVG_COLOR(255,0  ,255) },
  { "gainsboro",             __SVG_COLOR(220,220,220) },
  { "ghostwhite",            __SVG_COLOR(248,248,255) },
  { "gold",                  __SVG_COLOR(255,215,0  ) },
  { "goldenrod",             __SVG_COLOR(218,165,32 ) },
  { "gray",                  __SVG_COLOR(128,128,128) },
  { "green",                 __SVG_COLOR(0  ,128,0  ) },
  { "greenyellow",           __SVG_COLOR(173,255,47 ) },
  { "grey",                  __SVG_COLOR(128,128,128) },
  { "honeydew",              __SVG_COLOR(240,255,240) },
  { "hotpink",               __SVG_COLOR(255,105,180) },
  { "indianred",             __SVG_COLOR(205,92 ,92 ) },
  { "indigo",                __SVG_COLOR(75 ,0  ,130) },
  { "ivory",                 __SVG_COLOR(255,255,240) },
  { "khaki",                 __SVG_COLOR(240,230,140) },
  { "lavender",              __SVG_COLOR(230,230,250) },
  { "lavenderblush",         __SVG_COLOR(255,240,245) },
  { "lawngreen",             __SVG_COLOR(124,252,0  ) },
  { "lemonchiffon",          __SVG_COLOR(255,250,205) },
  { "lightblue",             __SVG_COLOR(173,216,230) },
  { "lightcoral",            __SVG_COLOR(240,128,128) },
  { "lightcyan",             __SVG_COLOR(224,255,255) },
  { "lightgoldenrodyellow",  __SVG_COLOR(250,250,210) },
  { "lightgray",             __SVG_COLOR(211,211,211) },
  { "lightgreen",            __SVG_COLOR(144,238,144) },
  { "lightgrey",             __SVG_COLOR(211,211,211) },
  { "lightpink",             __SVG_COLOR(255,182,193) },
  { "lightsalmon",           __SVG_COLOR(255,160,122) },
  { "lightseagreen",         __SVG_COLOR(32 ,178,170) },
  { "lightskyblue",          __SVG_COLOR(135,206,250) },
  { "lightslategray",        __SVG_COLOR(119,136,153) },
  { "lightslategrey",        __SVG_COLOR(119,136,153) },
  { "lightsteelblue",        __SVG_COLOR(176,196,222) },
  { "lightyellow",           __SVG_COLOR(255,255,224) },
  { "lime",                  __SVG_COLOR(0  ,255,0  ) },
  { "limegreen",             __SVG_COLOR(50 ,205,50 ) },
  { "linen",                 __SVG_COLOR(250,240,230) },
  { "magenta",               __SVG_COLOR(255,0  ,255) },
  { "maroon",                __SVG_COLOR(128,0  ,0  ) },
  { "mediumaquamarine",      __SVG_COLOR(102,205,170) },
  { "mediumblue",            __SVG_COLOR(0  ,0  ,205) },
  { "mediumorchid",          __SVG_COLOR(186,85 ,211) },
  { "mediumpurple",          __SVG_COLOR(147,112,219) },
  { "mediumseagreen",        __SVG_COLOR(60 ,179,113) },
  { "mediumslateblue",       __SVG_COLOR(123,104,238) },
  { "mediumspringgreen",     __SVG_COLOR(0  ,250,154) },
  { "mediumturquoise",       __SVG_COLOR(72 ,209,204) },
  { "mediumvioletred",       __SVG_COLOR(199,21 ,133) },
  { "midnightblue",          __SVG_COLOR(25 ,25 ,112) },
  { "mintcream",             __SVG_COLOR(245,255,250) },
  { "mistyrose",             __SVG_COLOR(255,228,225) },
  { "moccasin",              __SVG_COLOR(255,228,181) },
  { "navajowhite",           __SVG_COLOR(255,222,173) },
  { "navy",                  __SVG_COLOR(0  ,0  ,128) },
  { "oldlace",               __SVG_COLOR(253,245,230) },
  { "olive",                 __SVG_COLOR(128,128,0  ) },
  { "olivedrab",             __SVG_COLOR(107,142,35 ) },
  { "orange",                __SVG_COLOR(255,165,0  ) },
  { "orangered",             __SVG_COLOR(255,69 ,0  ) },
  { "orchid",                __SVG_COLOR(218,112,214) },
  { "palegoldenrod",         __SVG_COLOR(238,232,170) },
  { "palegreen",             __SVG_COLOR(152,251,152) },
  { "paleturquoise",         __SVG_COLOR(175,238,238) },
  { "palevioletred",         __SVG_COLOR(219,112,147) },
  { "papayawhip",            __SVG_COLOR(255,239,213) },
  { "peachpuff",             __SVG_COLOR(255,218,185) },
  { "peru",                  __SVG_COLOR(205,133,63 ) },
  { "pink",                  __SVG_COLOR(255,192,203) },
  { "plum",                  __SVG_COLOR(221,160,221) },
  { "powderblue",            __SVG_COLOR(176,224,230) },
  { "purple",                __SVG_COLOR(128,0  ,128) },
  { "red",                   __SVG_COLOR(255,0  ,0  ) },
  { "rosybrown",             __SVG_COLOR(188,143,143) },
  { "royalblue",             __SVG_COLOR(65 ,105,225) },
  { "saddlebrown",           __SVG_COLOR(139,69 ,19 ) },
  { "salmon",                __SVG_COLOR(250,128,114) },
  { "sandybrown",            __SVG_COLOR(244,164,96 ) },
  { "seagreen",              __SVG_COLOR(46 ,139,87 ) },
  { "seashell",              __SVG_COLOR(255,245,238) },
  { "sienna",                __SVG_COLOR(160,82 ,45 ) },
  { "silver",                __SVG_COLOR(192,192,192) },
  { "skyblue",               __SVG_COLOR(135,206,235) },
  { "slateblue",             __SVG_COLOR(106,90 ,205) },
  { "slategray",             __SVG_COLOR(112,128,144) },
  { "slategrey",             __SVG_COLOR(112,128,144) },
  { "snow",                  __SVG_COLOR(255,250,250) },
  { "springgreen",           __SVG_COLOR(0  ,255,127) },
  { "steelblue",             __SVG_COLOR(70 ,130,180) },
  { "tan",                   __SVG_COLOR(210,180,140) },
  { "teal",                  __SVG_COLOR(0  ,128,128) },
  { "thistle",               __SVG_COLOR(216,191,216) },
  { "tomato",                __SVG_COLOR(255,99 ,71 ) },
  { "turquoise",             __SVG_COLOR(64 ,224,208) },
  { "violet",                __SVG_COLOR(238,130,238) },
  { "wheat",                 __SVG_COLOR(245,222,179) },
  { "white",                 __SVG_COLOR(255,255,255) },
  { "whitesmoke",            __SVG_COLOR(245,245,245) },
  { "yellow",                __SVG_COLOR(255,255,0  ) },
  { "yellowgreen",           __SVG_COLOR(154,205,50 ) }
};
#undef __SVG_COLOR

static int svgNamedColorCmp(const void* a, const void* b)
{
  return reinterpret_cast<const String*>(a)->compare(
    Ascii8((reinterpret_cast<const SvgNamedColor *>(b))->name));
}

static uint8_t singleHexToColorValue(Char c0)
{
  uint8_t i;
  if (c0.isAsciiDigit())
    i = (uint8_t)(c0.ch() - '0');
  else
    i = (uint8_t)(c0.toAsciiLower().ch() - 'a' + 10);
  return (i << 4) | i;
}

static uint8_t doubleHexToColorValue(Char c0, Char c1)
{
  uint8_t i;
  uint8_t j;

  if (c0.isAsciiDigit())
    i = (uint8_t)(c0.ch() - '0');
  else
    i = (uint8_t)(c0.toAsciiLower().ch() - 'a' + 10);

  if (c1.isAsciiDigit())
    j = (uint8_t)(c1.ch() - '0');
  else
    j = (uint8_t)(c1.toAsciiLower().ch() - 'a' + 10);
  return (i << 4) | j;
}

FOG_API int parseColor(const String& str, Argb* dst)
{
  int status = SVG_SOURCE_INVALID;
  Argb color(0xFF000000);

  const Char* strCur = str.getData();
  const Char* strEnd = strCur + str.getLength();

  while (strCur != strEnd && strCur->isSpace()) strCur++;
  if (strCur == strEnd) goto bail;

  // HTML color '#RGB' or '#RRGGBB'.
  if (*strCur == Char('#'))
  {
    const Char* begin = ++strCur;
    const Char* end;
    if (strCur == strEnd) goto bail;

    while (strCur != strEnd && strCur->isXDigit()) strCur++;
    end = strCur;

    sysuint_t len = (sysuint_t)(end - begin);
    if (len == 3)
    {
      color.setRed  (singleHexToColorValue(begin[0]));
      color.setGreen(singleHexToColorValue(begin[1]));
      color.setBlue (singleHexToColorValue(begin[2]));

      status = SVG_SOURCE_COLOR;
    }
    else if (len == 6)
    {
      color.setRed  (doubleHexToColorValue(begin[0], begin[1]));
      color.setGreen(doubleHexToColorValue(begin[2], begin[3]));
      color.setBlue (doubleHexToColorValue(begin[4], begin[5]));

      status = SVG_SOURCE_COLOR;
    }
  }
  // CSS color 'rgb(r, g, b)'.
  else if (*strCur == Char('r') && strCur + 2 < strEnd && StringUtil::eq(strCur, "rgb", 3))
  {
    strCur += 3;

    // Skip spaces.
    for (;;) {
      if (strCur == strEnd) goto bail;
      else if (strCur->isSpace()) strCur++;
      else break;
    }

    // Parse '('.
    if (*strCur != Char('(')) goto bail;
    if (++strCur == strEnd) goto bail;

    // Parse 0-255, 0-255, 0-255.
    uint8_t c[3];
    for (sysuint_t i = 0; i < 3; i++)
    {
      // Parse 8-bit number
      sysuint_t end;
      if (StringUtil::atou8(strCur, (sysuint_t)(strEnd - strCur), &c[i], 10, &end) != ERR_OK) goto bail;
      strCur += end;

      // Skip spaces and parse ','.
      if (i < 2)
      {
        for (;;) {
          if (strCur == strEnd) goto bail;

          if (strCur->isSpace())
          {
            strCur++;
          }
          else if (*strCur == Char(','))
          {
            if (++strCur == strEnd) goto bail;
            break;
          }
          else
          {
            goto bail;
          }
        }
      }
    }

    // Skip spaces and parse ')'
    for (;;) {
      if (strCur == strEnd) goto bail;

      if (strCur->isSpace())
      {
        strCur++;
      }
      else if (*strCur == Char(')'))
      {
        strCur++;
        break;
      }
      else
      {
        goto bail;
      }
    }

    color.setRed  (c[0]);
    color.setGreen(c[1]);
    color.setBlue (c[2]);

    status = SVG_SOURCE_COLOR;
  }
  else if (*strCur == Char('u') && strCur + 2 < strEnd && StringUtil::eq(strCur, "url", 3))
  {
    status = SVG_SOURCE_URI;
  }
  else if (*strCur == Char('n') && strCur + 3 < strEnd && StringUtil::eq(strCur, "none", 4))
  {
    status = SVG_SOURCE_NONE;
  }
  else
  {
    // Try to parse named color.
    const SvgNamedColor* match = reinterpret_cast<const SvgNamedColor*>(fog_bsearch(
      &str, svgNamedColors, FOG_ARRAY_SIZE(svgNamedColors),
      sizeof(SvgNamedColor), svgNamedColorCmp));

    if (match)
    {
      color = match->value;
      status = SVG_SOURCE_COLOR;
    }
  }

bail:
  *dst = color;
  return status;
}

err_t serializeColor(String& dst, Argb color)
{
  err_t err;

  if ((err = dst.append(Char('#'))) ||
      (err = dst.appendInt(color.getValue(), 16, FormatFlags(6, 6)))) return err;

  return err;
}

err_t parseOpacity(const String& str, float* dst)
{
  sysuint_t end;
  float d = 0.0;
  err_t err = str.atof(&d, NULL, &end);

  if (err == ERR_OK)
  {
    // Parse '%'.
    if (end < str.getLength())
    {
      const Char* strCur = str.getData();
      const Char* strEnd = strCur + str.getLength();

      strCur += end;

      do {
        Char c = *strCur;
        if (c.isSpace()) continue;

        if (c == Char('%'))
        {
          d *= 0.01f;
          return ERR_OK;
        }
        else
        {
          return ERR_OK;
        }
      } while (++strCur != strEnd);
    }

    // Clamp value to 0.0 -> 1.0 inclusive.
    *dst = Math::bound<float>(d, 0.0f, 1.0f);
  }

  return err;
}

// ============================================================================
// [Fog::SvgUtil - Matrix]
// ============================================================================

err_t parseMatrix(const String& str, DoubleMatrix* dst)
{
  err_t status = ERR_OK;

  const Char* strCur = str.getData();
  const Char* strEnd = strCur + str.getLength();
  const Char* functionName;
  sysuint_t functionLen;

  double d[6];
  sysuint_t d_count;

  DoubleMatrix w;

start:
  // Skip spaces.
  for (;;) {
    if (strCur == strEnd) goto end;
    else if (strCur->isSpace()) strCur++;
    else break;
  }

  // Parse function name.
  functionName = strCur;
  for (;;) {
    if (strCur == strEnd) goto end;
    else if (strCur->isAsciiAlpha()) strCur++;
    else break;
  }
  functionLen = (sysuint_t)(strCur - functionName);

  // Parse '('.
  if (strCur[0] != Char('(')) goto end;
  strCur++;

  // Parse arguments.
  d_count = 0;
  for (;;) {
    if (strCur == strEnd) goto end;

    // Parse number.
    sysuint_t end;
    if (StringUtil::atod(strCur, (sysuint_t)(strEnd - strCur), &d[d_count++], Char('.'), &end) != ERR_OK) goto end;

    strCur += end;

    // Skip ',' and move to position of the next digit.
    bool commaParsed = false;
    for (;;) {
      if (strCur == strEnd) goto end;

      if (strCur->isSpace())
      {
        strCur++;
      }
      else if (strCur[0] == Char(','))
      {
        strCur++;
        if (commaParsed) goto end;
        commaParsed = true;
      }
      else if (strCur[0] == Char(')'))
      {
        strCur++;
        if (commaParsed) goto end;
        else goto done;
      }
      else if (strCur[0].isAsciiDigit() || strCur[0] == Char('-') || strCur[0] == Char('+'))
      {
        break;
      }
      else goto end;
    }

    if (d_count == 6) goto end;
  }
done:

  // matrix() function.
  if (functionLen == 6 && StringUtil::eq(functionName, "matrix", 6))
  {
    if (d_count != 6) goto end;
    w.multiply(*reinterpret_cast<DoubleMatrix *>(d), MATRIX_PREPEND);
  }
  // translate() function.
  else if (functionLen == 9 && StringUtil::eq(functionName, "translate", 9))
  {
    if (d_count != 1 && d_count != 2) goto end;

    // If ty is not provided, it's assumed to be zero.
    if (d_count == 1) d[1] = 0.0;
    w.translate(d[0], d[1], MATRIX_PREPEND);
  }
  // scale() function.
  else if (functionLen == 5 && StringUtil::eq(functionName, "scale", 5))
  {
    if (d_count != 1 && d_count != 2) goto end;

    // If sy is not provided, it's assumed to be equal to sx.
    if (d_count == 1) d[1] = d[0];
    w.scale(d[0], d[1]);
  }
  // rotate() function.
  else if (functionLen == 6 && StringUtil::eq(functionName, "rotate", 6))
  {
    if (d_count != 1 && d_count != 3) goto end;

    if (d_count == 3) w.translate(d[1], d[2], MATRIX_PREPEND);
    w.rotate(Math::deg2rad(d[0]), MATRIX_PREPEND);
    if (d_count == 3) w.translate(-d[1], -d[2], MATRIX_PREPEND);
  }
  // skewX() function.
  else if (functionLen == 5 && StringUtil::eq(functionName, "skewX", 5))
  {
    if (d_count != 1) goto end;

    w.skew(Math::deg2rad(d[0]), 0.0, MATRIX_PREPEND);
  }
  // skewY() function.
  else if (functionLen == 5 && StringUtil::eq(functionName, "skewY", 5))
  {
    if (d_count != 1) goto end;

    w.skew(0.0, Math::deg2rad(d[0]), MATRIX_PREPEND);
  }
  else
  {
    goto end;
  }

  // Skip spaces.
  for (;;) {
    if (strCur == strEnd) break;
    else if (strCur->isSpace()) strCur++;
    else goto start;
  }

end:
  *dst = w;
  return status;
}

// ============================================================================
// [Fog::SvgUtil - Coordinates]
// ============================================================================

static const char svgUnitNames[] = "cmemexinmmpcptpx";

SvgCoord parseCoord(const String& str)
{
  double d = 0.0;
  uint32_t unit = SVG_UNIT_NONE;

  sysuint_t end;
  err_t err = str.atod(&d, NULL, &end);

  if (err == ERR_OK)
  {
    unit = SVG_UNIT_PX;

    if (end < str.getLength())
    {
      sysuint_t end2 = str.indexOf(Char(' '), CASE_SENSITIVE, Range(end));
      Utf16 spec(str.getData() + end, (end2 == INVALID_INDEX ? str.getLength() : end2) - end);

      if (spec.getLength() == 1)
      {
        if (spec[0] == Char('%'))
        {
          unit = SVG_UNIT_PERCENT;
        }
      }
      else if (spec.getLength() == 2)
      {
        const char* units = svgUnitNames;
        for (sysuint_t u = 1; u < SVG_UNIT_PERCENT; u++, units += 2)
        {
          if (spec.getData()[0] == units[0] && spec.getData()[1] == units[1])
          {
            unit = u;
            break;
          }
        }
      }
    }
  }

  if (unit == SVG_UNIT_PERCENT) d *= 0.01;
  return SvgCoord(d, unit);
}

err_t serializeCoord(String& dst, const SvgCoord& coord)
{
  err_t err;

  if ((err = dst.appendDouble(coord.value))) return err;

  if ((coord.unit < SVG_UNIT_INVALID || coord.unit != SVG_UNIT_NONE) &&
      (err = dst.append(Ascii8(&svgUnitNames[coord.unit * 2], 2)))) return err;

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgUtil - Paths]
// ============================================================================

DoublePath parsePoints(const String& str, bool close)
{
  DoublePath path;

  const Char* strCur = str.getData();
  const Char* strEnd = strCur + str.getLength();

  double coords[2];
  uint32_t position;

  bool first = true;

  // Finished?
  if (strCur == strEnd) goto bail;

  // Parse coordinates.
  position = 0;
  for (;;)
  {
    // Skip spaces.
    while (strCur->isSpace())
    {
      if (++strCur == strEnd) goto bail;
    }

    // Parse number.
    sysuint_t numEnd;
    err_t err = StringUtil::atod(strCur, (sysuint_t)(strEnd - strCur), &coords[position], Char('.'), &numEnd);
    if (err) goto bail;

    strCur += numEnd;
    if (strCur == strEnd) break;

    if (*strCur == Char(','))
    {
      if (++strCur == strEnd) break;
    }

    if (++position == 2)
    {
      if (first)
      {
        path.moveTo(coords[0], coords[1]);
        first = false;
      }
      else
      {
        path.lineTo(coords[0], coords[1]);
      }
      position = 0;
    }
  }

bail:
  if (close && !path.isEmpty()) path.closePolygon();
  path.squeeze();
  return path;
}

DoublePath parsePath(const String& str)
{
  DoublePath path;

  const Char* strCur = str.getData();
  const Char* strEnd = strCur + str.getLength();

  uint32_t command;
  uint32_t position;
  uint32_t count;
  double coords[10];

  path.reserve(32);

  for (;;)
  {
    // Finished?
    if (strCur == strEnd) goto bail;

    // Skip spaces.
    while (strCur->isSpace())
    {
      if (++strCur == strEnd) goto bail;
    }

    // Parse command.
    if (!strCur->isAsciiAlpha())
    {
      if (command == 0) goto bail;
    }
    else
    {
      command = strCur->ch();
      if (++strCur == strEnd) 
      {
        if (command == 'Z' || command == 'z') goto close;
        goto bail;
      }
    }

    switch (command)
    {
      case 'A':
      case 'a': count = 7; break;
      case 'C':
      case 'c': count = 6; break;
      case 'H':
      case 'h': count = 1; break;
      case 'L':
      case 'l': count = 2; break;
      case 'M':
      case 'm': count = 2; break;
      case 'Q':
      case 'q': count = 4; break;
      case 'S':
      case 's': count = 4; break;
      case 'T':
      case 't': count = 2; break;
      case 'V':
      case 'v': count = 1; break;
      case 'Z':
      case 'z': count = 0; break;

      default:
        // Unknown command.
        goto bail;
    }

    // Parse coordinates.
    for (position = 0; position < count; position++)
    {
      // Skip spaces.
      while (strCur->isSpace())
      {
        if (++strCur == strEnd) goto bail;
      }

      // Parse number.
      sysuint_t numEnd;
      err_t err = StringUtil::atod(strCur, (sysuint_t)(strEnd - strCur), &coords[position], Char('.'), &numEnd);
      if (err) goto bail;

      strCur += numEnd;
      if (strCur == strEnd) break;

      if (*strCur == Char(','))
      {
        if (++strCur == strEnd) break;
      }
    }

    switch (command)
    {
      case 'A':
        path.svgArcTo(
          coords[0], coords[1],
          coords[2],
          (bool)(int)coords[3],
          (bool)(int)coords[4],
          coords[5],
          coords[6]);
        break;
      case 'a':
        path.svgArcRel(
          coords[0], coords[1],
          coords[2],
          (bool)(int)coords[3],
          (bool)(int)coords[4],
          coords[5],
          coords[6]);
        break;

      case 'C':
        path.cubicTo(coords[0], coords[1], coords[2], coords[3], coords[4], coords[5]);
        break;
      case 'c':
        path.cubicRel(coords[0], coords[1], coords[2], coords[3], coords[4], coords[5]);
        break;

      case 'H':
        path.hlineTo(coords[0]);
        break;
      case 'h':
        path.hlineRel(coords[0]);
        break;

      case 'L':
        path.lineTo(coords[0], coords[1]);
        break;
      case 'l':
        path.lineRel(coords[0], coords[1]);
        break;

      case 'M':
        path.moveTo(coords[0], coords[1]);
        break;
      case 'm':
        path.moveRel(coords[0], coords[1]);
        break;

      case 'Q':
        path.curveTo(coords[0], coords[1], coords[2], coords[3]);
        break;
      case 'q':
        path.curveRel(coords[0], coords[1], coords[2], coords[3]);
        break;

      case 'S':
        path.cubicTo(coords[0], coords[1], coords[2], coords[3]);
        break;
      case 's':
        path.cubicRel(coords[0], coords[1], coords[2], coords[3]);
        break;

      case 'T':
        path.curveTo(coords[0], coords[1]);
        break;
      case 't':
        path.curveRel(coords[0], coords[1]);
        break;

      case 'V':
        path.vlineTo(coords[0]);
        break;
      case 'v':
        path.vlineRel(coords[0]);
        break;

      case 'Z': 
      case 'z':
close:
        path.closePolygon();
        break;
    }
  }

bail:
  path.squeeze();
  return path;
}

} // SvgUtil namespace
} // Fog namespace
