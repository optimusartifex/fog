// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Collection/Algorithms.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/Svg/Global/Constants.h>
#include <Fog/Svg/Tools/SvgUtil.h>

namespace Fog {
namespace SvgUtil {

// ============================================================================
// [Fog::SvgUtil - Parse - Color]
// ============================================================================

FOG_API uint32_t parseColor(Color& dst, const String& str)
{
  const Char* strCur = str.getData();
  const Char* strEnd = strCur + str.getLength();

  // Skip spaces.
  for (;;)
  {
    if (strCur == strEnd) return SVG_SOURCE_INVALID;

    if (!strCur[0].isSpace()) break;
    strCur++;
  }

  if (strCur + 3 <= strEnd && strCur[0] == Char('u') &&
                              strCur[1] == Char('r') &&
                              strCur[2] == Char('l'))
  {
    dst.reset();
    return SVG_SOURCE_URI;
  }
  if (strCur + 4 <= strEnd && strCur[0] == Char('n') &&
                              strCur[1] == Char('o') &&
                              strCur[2] == Char('n') &&
                              strCur[3] == Char('e'))
  {
    dst.reset();
    return SVG_SOURCE_NONE;
  }

  err_t err = dst.parse(Utf16(strCur, (sysuint_t)(strEnd - strCur)), COLOR_NAME_CSS);
  if (err == ERR_OK) return SVG_SOURCE_COLOR;

  return SVG_SOURCE_INVALID;
}

// ============================================================================
// [Fog::SvgUtil - Parse - Opacity]
// ============================================================================

err_t parseOpacity(float& dst, const String& str)
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
    dst = Math::bound<float>(d, 0.0f, 1.0f);
  }

  return err;
}

// ============================================================================
// [Fog::SvgUtil - Parse - Transform]
// ============================================================================

err_t parseTransform(TransformF& dst, const String& str)
{
  err_t err = ERR_OK;

  const Char* strCur = str.getData();
  const Char* strEnd = strCur + str.getLength();
  const Char* functionName;
  sysuint_t functionLen;

  float d[6];
  sysuint_t d_count;

  dst.reset();

_Start:
  // Skip spaces.
  for (;;)
  {
    if (strCur == strEnd) goto _End;
    else if (strCur->isSpace()) strCur++;
    else break;
  }

  // Parse function name.
  functionName = strCur;
  for (;;)
  {
    if (strCur == strEnd) goto _End;
    else if (strCur->isAsciiAlpha()) strCur++;
    else break;
  }
  functionLen = (sysuint_t)(strCur - functionName);

  // Parse '('.
  if (strCur[0] != Char('(')) goto _End;
  strCur++;

  // Parse arguments.
  d_count = 0;
  for (;;)
  {
    if (strCur == strEnd) goto _End;

    // Parse number.
    sysuint_t end;
    if (StringUtil::atof(strCur, (sysuint_t)(strEnd - strCur), &d[d_count++], Char('.'), &end) != ERR_OK)
    {
      goto _End;
    }

    strCur += end;

    // Skip ',' and move to position of the next digit.
    bool commaParsed = false;
    for (;;)
    {
      if (strCur == strEnd) goto _End;

      if (strCur->isSpace())
      {
        strCur++;
      }
      else if (strCur[0] == Char(','))
      {
        strCur++;
        if (commaParsed) goto _End;
        commaParsed = true;
      }
      else if (strCur[0] == Char(')'))
      {
        strCur++;
        if (commaParsed) goto _End;
        goto _Done;
      }
      else if (strCur[0].isAsciiDigit() || strCur[0] == Char('-') || strCur[0] == Char('+'))
      {
        break;
      }
      else
      {
        goto _End;
      }
    }

    if (d_count == 6) goto _End;
  }
_Done:

  // matrix() function.
  if (functionLen == 6 && StringUtil::eq(functionName, "matrix", 6))
  {
    if (d_count != 6) goto _End;

    dst.transform(TransformF(d[0], d[1], d[2], d[3], d[4], d[5]), MATRIX_ORDER_PREPEND);
  }
  // translate() function.
  else if (functionLen == 9 && StringUtil::eq(functionName, "translate", 9))
  {
    if (d_count != 1 && d_count != 2) goto _End;
    // If ty is not provided, it's assumed to be zero.
    if (d_count == 1) d[1] = 0.0f;
    dst.translate(PointF(d[0], d[1]), MATRIX_ORDER_PREPEND);
  }
  // scale() function.
  else if (functionLen == 5 && StringUtil::eq(functionName, "scale", 5))
  {
    if (d_count != 1 && d_count != 2) goto _End;
    // If sy is not provided, it's assumed to be equal to sx.
    if (d_count == 1) d[1] = d[0];
    dst.scale(PointF(d[0], d[1]), MATRIX_ORDER_PREPEND);
  }
  // rotate() function.
  else if (functionLen == 6 && StringUtil::eq(functionName, "rotate", 6))
  {
    if (d_count != 1 && d_count != 3) goto _End;
    if (d_count == 3) dst.translate(PointF(d[1], d[2]), MATRIX_ORDER_PREPEND);
    dst.rotate(Math::deg2rad(d[0]), MATRIX_ORDER_PREPEND);
    if (d_count == 3) dst.translate(PointF(-d[1], -d[2]), MATRIX_ORDER_PREPEND);
  }
  // skewX() function.
  else if (functionLen == 5 && StringUtil::eq(functionName, "skewX", 5))
  {
    if (d_count != 1) goto _End;
    dst.skew(PointF(Math::deg2rad(d[0]), 0.0f), MATRIX_ORDER_PREPEND);
  }
  // skewY() function.
  else if (functionLen == 5 && StringUtil::eq(functionName, "skewY", 5))
  {
    if (d_count != 1) goto _End;
    dst.skew(PointF(0.0f, Math::deg2rad(d[0])), MATRIX_ORDER_PREPEND);
  }
  else
  {
    goto _End;
  }

  // Skip spaces.
  for (;;)
  {
    if (strCur == strEnd) break;

    if (strCur->isSpace())
      strCur++;
    else
      goto _Start;
  }

_End:
  return err;
}

// ============================================================================
// [Fog::SvgUtil - Parse - Coord]
// ============================================================================

static const char svgUnitNames[] = "cmemexinmmpcptpx";

err_t parseCoord(SvgCoord& coord, const String& str)
{
  float d = 0.0f;
  uint32_t unit = SVG_UNIT_NONE;

  sysuint_t end;
  err_t err = str.atof(&d, NULL, &end);

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
        for (uint32_t u = 1; u < SVG_UNIT_PERCENT; u++, units += 2)
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

  if (unit == SVG_UNIT_PERCENT) d *= 0.01f;

  coord.value = (float)d;
  coord.unit = unit;
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgUtil - Parse - ViewBox]
// ============================================================================

err_t parseViewBox(BoxF& box, const String& str)
{
  err_t err = ERR_OK;

  const Char* strCur = str.getData();
  const Char* strEnd = strCur + str.getLength();

  float coords[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
  uint32_t position = 0;
  sysuint_t numEnd;

  // Finished?
  if (strCur == strEnd) goto _Bail;

  // Parse coordinates.
  for (position = 0; position < 4; position++)
  {
    // Skip spaces.
    while (strCur->isSpace())
    {
      if (++strCur == strEnd) goto _Bail;
    }

    // Parse number.
    err = StringUtil::atof(strCur, (sysuint_t)(strEnd - strCur), &coords[position], Char('.'), &numEnd);
    if (FOG_IS_ERROR(err)) goto _Bail;

    strCur += numEnd;
    if (strCur == strEnd) break;

    // Parse optional comma.
    if (*strCur == Char(','))
    {
      if (++strCur == strEnd) break;
    }
  }

_Bail:
  // TODO: Error reporting.

  box.setBox(coords[0], coords[1], coords[2], coords[3]);
  return err;
}

// ============================================================================
// [Fog::SvgUtil - Parse - Points]
// ============================================================================

err_t parsePoints(PathF& dst, const String& str, bool closePath)
{
  err_t err = ERR_OK;
  bool first = true;

  const Char* strCur = str.getData();
  const Char* strEnd = strCur + str.getLength();

  float coords[2];
  uint32_t position = 0;

  // Clear the path.
  dst.clear();

  // Finished?
  if (strCur == strEnd) goto _Bail;

  // Parse coordinates.
  for (;;)
  {
    // Skip spaces.
    while (strCur->isSpace())
    {
      if (++strCur == strEnd) goto _Bail;
    }

    // Parse number.
    sysuint_t numEnd;
    err = StringUtil::atof(strCur, (sysuint_t)(strEnd - strCur), &coords[position], Char('.'), &numEnd);
    if (FOG_IS_ERROR(err)) goto _Bail;

    if (++position == 2)
    {
      if (first)
      {
        err = dst.moveTo(PointF(coords[0], coords[1]));
        if (FOG_IS_ERROR(err)) goto _Bail;
        first = false;
      }
      else
      {
        err = dst.lineTo(PointF(coords[0], coords[1]));
        if (FOG_IS_ERROR(err)) goto _Bail;
      }
      position = 0;
    }

    strCur += numEnd;
    if (strCur == strEnd) break;

    if (*strCur == Char(','))
    {
      if (++strCur == strEnd) break;
    }
  }

_Bail:
  if (closePath && !dst.isEmpty()) dst.close();
  dst.squeeze();
  return err;
}

// ============================================================================
// [Fog::SvgUtil - Parse - Path]
// ============================================================================

err_t parsePath(PathF& dst, const String& str)
{
  err_t err = ERR_OK;

  const Char* strCur = str.getData();
  const Char* strEnd = strCur + str.getLength();

  uint32_t command = 0;
  uint32_t position;
  uint32_t count;
  float coords[10];

  // Clear the path.
  dst.clear();

  // Is his a first coordinate.
  PointF last(0.0f, 0.0f);
  PointF initial(0.0f, 0.0f);

  for (;;)
  {
    // Finished?
    if (strCur == strEnd) goto _Bail;

    // Skip spaces.
    while (strCur->isSpace())
    {
      if (++strCur == strEnd) goto _Bail;
    }

    // Parse command.
    if (!strCur->isAsciiAlpha())
    {
      if (command == 0) goto _Bail;
    }
    else
    {
      command = strCur->ch();
      if (++strCur == strEnd)
      {
        if (command == 'Z' || command == 'z') goto _Close;
        goto _Bail;
      }
    }

    switch (command)
    {
      case 'a': case 'A': count = 7; break;
      case 'c': case 'C': count = 6; break;
      case 'h': case 'H': count = 1; break;
      case 'l': case 'L': count = 2; break;
      case 'm': case 'M': count = 2; break;
      case 'q': case 'Q': count = 4; break;
      case 's': case 'S': count = 4; break;
      case 't': case 'T': count = 2; break;
      case 'v': case 'V': count = 1; break;
      case 'z': case 'Z': count = 0; break;

      // Unknown command.
      default: goto _Bail;
    }

    // Parse coordinates.
    for (position = 0; position < count; position++)
    {
      // Skip spaces.
      while (strCur->isSpace())
      {
        if (++strCur == strEnd) goto _Bail;
      }

      // Parse number.
      sysuint_t numEnd;

      err = StringUtil::atof(strCur, (sysuint_t)(strEnd - strCur), &coords[position], Char('.'), &numEnd);
      if (FOG_IS_ERROR(err)) goto _Bail;

      strCur += numEnd;
      if (strCur == strEnd) break;

      if (*strCur == Char(','))
      {
        if (++strCur == strEnd) break;
      }
    }

    switch (command)
    {
      // ----------------------------------------------------------------------
      // [Arc-To]
      // ----------------------------------------------------------------------

      case 'a':
        coords[0] += last.x; coords[1] += last.y;
        coords[5] += last.x; coords[6] += last.y;
        // ... Fall through ...

      case 'A':
        err = dst.svgArcTo(
          PointF(coords[0], coords[1]),
          Math::deg2rad(coords[2]),
          (bool)(int)coords[3],
          (bool)(int)coords[4],
          PointF(coords[5], coords[6]));

        last.x = coords[5];
        last.y = coords[6];
        break;

      // ----------------------------------------------------------------------
      // [Cubic-To]
      // ----------------------------------------------------------------------

      case 'c':
        coords[0] += last.x; coords[1] += last.y;
        coords[2] += last.x; coords[3] += last.y;
        coords[4] += last.x; coords[5] += last.y;
        // ... Fall through ...

      case 'C':
        err = dst.cubicTo(
          PointF(coords[0], coords[1]),
          PointF(coords[2], coords[3]),
          PointF(coords[4], coords[5]));

        last.x = coords[4];
        last.y = coords[5];
        break;

      // ----------------------------------------------------------------------
      // [Horizontal-Line-To]
      // ----------------------------------------------------------------------

      case 'h':
        coords[0] += last.x;
        // ... Fall through ...

      case 'H':
        err = dst.lineTo(PointF(coords[0], last.y));

        last.x = coords[0];
        break;

      // ----------------------------------------------------------------------
      // [Line-To]
      // ----------------------------------------------------------------------

      case 'l':
        coords[0] += last.x; coords[1] += last.y;
        // ... Fall through ...

      case 'L':
        err = dst.lineTo(PointF(coords[0], coords[1]));

        last.x = coords[0];
        last.y = coords[1];
        break;

      // ----------------------------------------------------------------------
      // [Move-To]
      // ----------------------------------------------------------------------

      case 'm':
        // Switch command to relative line-to 'l' to match the specification.
        command = 'l';

        coords[0] += last.x;
        coords[1] += last.y;
        goto _MoveTo;

      case 'M':
        // Switch command to absolute line-to 'L' to match the specification.
        command = 'L';
_MoveTo:
        err = dst.moveTo(PointF(coords[0], coords[1]));

        last.x = coords[0];
        last.y = coords[1];

        initial = last;
        break;

      // ----------------------------------------------------------------------
      // [Quad-To]
      // ----------------------------------------------------------------------

      case 'q':
        coords[0] += last.x; coords[1] += last.y;
        coords[2] += last.x; coords[3] += last.y;
        // ... Fall through ...

      case 'Q':
        err = dst.quadTo(PointF(coords[0], coords[1]), PointF(coords[2], coords[3]));

        last.x = coords[2];
        last.y = coords[3];
        break;

      // ----------------------------------------------------------------------
      // [Smooth-Cubic-To]
      // ----------------------------------------------------------------------

      case 's':
        coords[0] += last.x; coords[1] += last.y;
        coords[2] += last.x; coords[3] += last.y;
        // ... Fall through ...

      case 'S':
        err = dst.smoothCubicTo(PointF(coords[0], coords[1]), PointF(coords[2], coords[3]));

        last.x = coords[2];
        last.y = coords[3];
        break;

      // ----------------------------------------------------------------------
      // [Smooth-Quad-To]
      // ----------------------------------------------------------------------

      case 't':
        coords[0] += last.x; coords[1] += last.y;
        // ... Fall through ...

      case 'T':
        err = dst.smoothQuadTo(PointF(coords[0], coords[1]));

        last.x = coords[0];
        last.y = coords[1];
        break;

      // ----------------------------------------------------------------------
      // [Vertical-Line-To]
      // ----------------------------------------------------------------------

      case 'v':
        coords[0] += last.y;
        // ... Fall through ...

      case 'V':
        err = dst.lineTo(PointF(last.x, coords[0]));

        last.y = coords[0];
        break;

      // ----------------------------------------------------------------------
      // [Close]
      // ----------------------------------------------------------------------

      case 'Z':
      case 'z':
_Close:
        err = dst.close();

        // Clear the error which is caused by adding the PATH_CMD_CLOSE to the
        // path without vertex or more times.
        if (err == ERR_PATH_NO_VERTEX) err = ERR_OK;

        last = initial;
        break;
    }

    if (FOG_IS_ERROR(err)) goto _Bail;
  }

_Bail:
  dst.squeeze();
  return err;
}

// ============================================================================
// [Fog::SvgUtil - Serialize - Color]
// ============================================================================

err_t serializeColor(String& dst, const Color& color)
{
  switch (color.getModel())
  {
    case COLOR_MODEL_ARGB:
    case COLOR_MODEL_AHSV:
    case COLOR_MODEL_ACMYK:
    {
      Argb32 argb32 = color.getArgb32();

      if (argb32.getAlpha() != 0xFF)
      {
        FOG_RETURN_ON_ERROR( dst.append(Char('#')) );
        FOG_RETURN_ON_ERROR( dst.appendInt(argb32.getPacked32() & 0x00FFFFFF, 16, FormatFlags(6, 6)) );
      }
      else
      {
        FOG_RETURN_ON_ERROR(
          dst.format("rgb(%d, %d, %d, %f)",
            argb32.getRed(),
            argb32.getGreen(),
            argb32.getBlue(),
            color.getAlpha())
        );
      }
      return ERR_OK;
    }

    case COLOR_MODEL_AHSL:
    {
      if (color.isOpaque())
      {
        FOG_RETURN_ON_ERROR(
          dst.format("hsl(%d, %d, %d)",
            Math::iround(color._data[1] * 255.0f),
            Math::iround(color._data[2] * 100.0f),
            Math::iround(color._data[3] * 100.0f))
        );
      }
      else
      {
        FOG_RETURN_ON_ERROR(
          dst.format("hsla(%d, %d, %d, %f)",
            Math::iround(color._data[1] * 255.0f),
            Math::iround(color._data[2] * 100.0f),
            Math::iround(color._data[3] * 100.0f),
            color.getAlpha())
        );
      }
      return ERR_OK;
    }

    case COLOR_MODEL_NONE:
    default:
      return dst.append(Ascii8("none"));
  }
}

// ============================================================================
// [Fog::SvgUtil - Serialize - Coord]
// ============================================================================

err_t serializeCoord(String& dst, const SvgCoord& coord)
{
  FOG_RETURN_ON_ERROR( dst.appendDouble(coord.value) );

  if (coord.unit < SVG_UNIT_INVALID || coord.unit != SVG_UNIT_NONE)
    FOG_RETURN_ON_ERROR( dst.append(Ascii8(&svgUnitNames[coord.unit * 2], 2)) );

  return ERR_OK;
}

} // SvgUtil namespace
} // Fog namespace
