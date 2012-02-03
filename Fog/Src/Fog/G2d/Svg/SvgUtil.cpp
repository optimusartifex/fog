// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Tools/Algorithm.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Svg/SvgUtil.h>

namespace Fog {
namespace SvgUtil {

// ============================================================================
// [Fog::SvgUtil - Parse - Color]
// ============================================================================

FOG_API uint32_t parseColor(Color& dst, const StringW& src)
{
  const CharW* srcCur = src.getData();
  const CharW* srcEnd = srcCur + src.getLength();

  // Skip spaces.
  for (;;)
  {
    if (srcCur == srcEnd)
      return SVG_SOURCE_INVALID;

    if (!srcCur[0].isSpace()) break;
    srcCur++;
  }

  if (srcCur + 3 <= srcEnd && srcCur[0] == CharW('u') &&
                              srcCur[1] == CharW('r') &&
                              srcCur[2] == CharW('l'))
  {
    dst.reset();
    return SVG_SOURCE_URI;
  }

  if (srcCur + 4 <= srcEnd && srcCur[0] == CharW('n') &&
                              srcCur[1] == CharW('o') &&
                              srcCur[2] == CharW('n') &&
                              srcCur[3] == CharW('e'))
  {
    dst.reset();
    return SVG_SOURCE_NONE;
  }

  err_t err = dst.parse(StubW(srcCur, (size_t)(srcEnd - srcCur)), COLOR_NAME_CSS);
  if (err == ERR_OK)
    return SVG_SOURCE_COLOR;

  return SVG_SOURCE_INVALID;
}

FOG_API uint32_t parseColor(Argb32& dst, const StringW& src)
{
  Color color(UNINITIALIZED);
  uint32_t result = parseColor(color, src);

  dst = color.getArgb32();
  return result;
}

// ============================================================================
// [Fog::SvgUtil - Parse - Offset]
// ============================================================================

err_t parseOffset(float& dst, const StringW& src)
{
  size_t end;
  float d = 0.0;
  err_t err = src.parseReal(&d, CharW('.'), &end, NULL);

  if (err == ERR_OK)
  {
    // Parse '%'.
    if (end < src.getLength())
    {
      const CharW* srcCur = src.getData();
      const CharW* srcEnd = srcCur + src.getLength();

      srcCur += end;

      do {
        CharW c = srcCur[0];
        if (c.isSpace())
          continue;
        if (c == CharW('%'))
          d *= 0.01f;
        break;
      } while (++srcCur != srcEnd);
    }

    // Clamp value to 0.0 -> 1.0, inclusive.
    dst = Math::bound<float>(d, 0.0f, 1.0f);
  }

  return err;
}

// ============================================================================
// [Fog::SvgUtil - Parse - Opacity]
// ============================================================================

err_t parseOpacity(float& dst, const StringW& src)
{
  return parseOffset(dst, src);
}

// ============================================================================
// [Fog::SvgUtil - Parse - Transform]
// ============================================================================

err_t parseTransform(TransformF& dst, const StringW& src)
{
  err_t err = ERR_OK;

  const CharW* srcCur = src.getData();
  const CharW* srcEnd = srcCur + src.getLength();
  const CharW* functionName;
  size_t functionLen;

  float d[6];
  size_t d_count;

  dst.reset();

_Start:
  // Skip spaces.
  for (;;)
  {
    if (srcCur == srcEnd)
      goto _End;
    else if (srcCur->isSpace())
      srcCur++;
    else
      break;
  }

  // Parse function name.
  functionName = srcCur;
  for (;;)
  {
    if (srcCur == srcEnd)
      goto _End;
    else if (srcCur->isAsciiLetter())
      srcCur++;
    else
      break;
  }
  functionLen = (size_t)(srcCur - functionName);

  // Parse '('.
  if (srcCur[0] != CharW('('))
    goto _End;
  srcCur++;

  // Parse arguments.
  d_count = 0;
  for (;;)
  {
    if (srcCur == srcEnd) goto _End;

    // Parse number.
    size_t end;
    if (StringUtil::parseReal(&d[d_count++], srcCur, (size_t)(srcEnd - srcCur), CharW('.'), &end) != ERR_OK)
    {
      goto _End;
    }

    srcCur += end;

    // Skip ',' and move to position of the next digit.
    bool commaParsed = false;
    for (;;)
    {
      if (srcCur == srcEnd)
        goto _End;

      if (srcCur->isSpace())
      {
        srcCur++;
      }
      else if (srcCur[0] == CharW(','))
      {
        srcCur++;
        if (commaParsed)
          goto _End;
        commaParsed = true;
      }
      else if (srcCur[0] == CharW(')'))
      {
        srcCur++;
        if (commaParsed)
          goto _End;
        goto _Done;
      }
      else if (srcCur[0].isAsciiDigit() || srcCur[0] == CharW('-') || srcCur[0] == CharW('+'))
      {
        break;
      }
      else
      {
        goto _End;
      }
    }

    if (d_count == 6)
      goto _End;
  }
_Done:

  // matrix() function.
  if (functionLen == 6 && StringUtil::eq(functionName, "matrix", 6))
  {
    if (d_count != 6)
      goto _End;

    dst.transform(TransformF(d[0], d[1], d[2], d[3], d[4], d[5]), MATRIX_ORDER_PREPEND);
  }
  // translate() function.
  else if (functionLen == 9 && StringUtil::eq(functionName, "translate", 9))
  {
    if (d_count != 1 && d_count != 2) 
      goto _End;

    // If ty is not provided, it's assumed to be zero.
    if (d_count == 1) d[1] = 0.0f;
    dst.translate(PointF(d[0], d[1]), MATRIX_ORDER_PREPEND);
  }
  // scale() function.
  else if (functionLen == 5 && StringUtil::eq(functionName, "scale", 5))
  {
    if (d_count != 1 && d_count != 2)
      goto _End;

    // If sy is not provided, it's assumed to be equal to sx.
    if (d_count == 1) d[1] = d[0];
    dst.scale(PointF(d[0], d[1]), MATRIX_ORDER_PREPEND);
  }
  // rotate() function.
  else if (functionLen == 6 && StringUtil::eq(functionName, "rotate", 6))
  {
    if (d_count != 1 && d_count != 3)
      goto _End;

    float rotateDeg = Math::deg2rad(d[0]);

    if (d_count == 1)
    {
      dst.rotate(rotateDeg);
    }
    else
    {
      TransformF t;
      t.translate(PointF(d[1], d[2]));
      t.rotate(rotateDeg);
      t.translate(PointF(-d[1], -d[2]));
      dst.transform(t);
      //dst.rotate(rotateDeg, d[1], d[2]);
    }
  }
  // skewX() function.
  else if (functionLen == 5 && StringUtil::eq(functionName, "skewX", 5))
  {
    if (d_count != 1)
      goto _End;

    dst.skew(PointF(Math::deg2rad(d[0]), 0.0f), MATRIX_ORDER_PREPEND);
  }
  // skewY() function.
  else if (functionLen == 5 && StringUtil::eq(functionName, "skewY", 5))
  {
    if (d_count != 1)
      goto _End;

    dst.skew(PointF(0.0f, Math::deg2rad(d[0])), MATRIX_ORDER_PREPEND);
  }
  else
  {
    goto _End;
  }

  // Skip spaces.
  for (;;)
  {
    if (srcCur == srcEnd)
      break;

    if (srcCur->isSpace())
      srcCur++;
    else
      goto _Start;
  }

_End:
  return err;
}

// ============================================================================
// [Fog::SvgUtil - Parse - Coord]
// ============================================================================

// ${UNIT:BEGIN}
static const char svgUnitNames[] =
  "\0\0"
  "px"
  "pt"
  "pc"
  "in"
  "mm"
  "cm"
  "\0\0"
  "em"
  "ex";
// ${UNIT:END}

err_t parseCoord(CoordF& coord, const StringW& src)
{
  float d = 0.0f;
  uint32_t unit = UNIT_PX;

  size_t end;
  err_t err = src.parseReal(&d, CharW('.'), &end, NULL);

  if (err == ERR_OK)
  {
    if (end < src.getLength())
    {
      size_t end2 = src.indexOf(Range(end, DETECT_LENGTH), CharW(' '), CASE_SENSITIVE);
      StubW spec(src.getData() + end, (end2 == INVALID_INDEX ? src.getLength() : end2) - end);

      if (spec.getLength() == 1)
      {
        if (spec[0] == CharW('%'))
          unit = UNIT_PERCENTAGE;
      }
      else if (spec.getLength() == 2)
      {
        const char* units = svgUnitNames;
        for (uint32_t u = 0; u < UNIT_COUNT; u++, units += 2)
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

  if (unit == UNIT_PERCENTAGE)
    d *= 0.01f;

  coord.value = (float)d;
  coord.unit = unit;

  return err;
}

// ============================================================================
// [Fog::SvgUtil - Parse - ViewBox]
// ============================================================================

err_t parseViewBox(BoxF& box, const StringW& src)
{
  err_t err = ERR_OK;

  const CharW* srcCur = src.getData();
  const CharW* srcEnd = srcCur + src.getLength();

  float coords[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
  uint32_t position = 0;
  size_t numEnd;

  // Finished?
  if (srcCur == srcEnd) goto _Bail;

  // Parse coordinates.
  for (position = 0; position < 4; position++)
  {
    // Skip spaces.
    while (srcCur->isSpace())
    {
      if (++srcCur == srcEnd) goto _Bail;
    }

    // Parse number.
    err = StringUtil::parseReal(&coords[position], srcCur, (size_t)(srcEnd - srcCur), CharW('.'), &numEnd);
    if (FOG_IS_ERROR(err)) goto _Bail;

    srcCur += numEnd;
    if (srcCur == srcEnd) break;

    // Parse optional comma.
    if (*srcCur == CharW(','))
    {
      if (++srcCur == srcEnd) break;
    }
  }

_Bail:
  // TODO: Error reporting.

  box.setBox(coords[0], coords[1], coords[0] + coords[2], coords[1] + coords[3]);
  return err;
}

// ============================================================================
// [Fog::SvgUtil - Parse - Points]
// ============================================================================

err_t parsePoints(PathF& dst, const StringW& src, bool closePath)
{
  err_t err = ERR_OK;
  bool first = true;

  const CharW* srcCur = src.getData();
  const CharW* srcEnd = srcCur + src.getLength();

  float coords[2];
  uint32_t position = 0;

  // Clear the path.
  dst.clear();

  // Finished?
  if (srcCur == srcEnd) goto _Bail;

  // Parse coordinates.
  for (;;)
  {
    // Skip spaces.
    while (srcCur->isSpace())
    {
      if (++srcCur == srcEnd) goto _Bail;
    }

    // Parse number.
    size_t numEnd;
    err = StringUtil::parseReal(&coords[position], srcCur, (size_t)(srcEnd - srcCur), CharW('.'), &numEnd);
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

    srcCur += numEnd;
    if (srcCur == srcEnd) break;

    if (*srcCur == CharW(','))
    {
      if (++srcCur == srcEnd) break;
    }
  }

_Bail:
  if (closePath && !dst.isEmpty())
    dst.close();

  dst.squeeze();
  return err;
}

// ============================================================================
// [Fog::SvgUtil - Parse - Path]
// ============================================================================

err_t parsePath(PathF& dst, const StringW& src)
{
  err_t err = ERR_OK;

  const CharW* srcCur = src.getData();
  const CharW* srcEnd = srcCur + src.getLength();

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
    if (srcCur == srcEnd) goto _Bail;

    // Skip spaces.
    while (srcCur->isSpace())
    {
      if (++srcCur == srcEnd) goto _Bail;
    }

    // Parse command.
    if (!srcCur->isAsciiLetter())
    {
      if (command == 0) goto _Bail;
    }
    else
    {
      command = srcCur->getValue();
      if (++srcCur == srcEnd)
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
      while (srcCur->isSpace())
      {
        if (++srcCur == srcEnd) goto _Bail;
      }

      // Parse number.
      size_t numEnd;

      err = StringUtil::parseReal(&coords[position], srcCur, (size_t)(srcEnd - srcCur), CharW('.'), &numEnd);
      if (FOG_IS_ERROR(err)) goto _Bail;

      srcCur += numEnd;
      if (srcCur == srcEnd) break;

      if (*srcCur == CharW(','))
      {
        if (++srcCur == srcEnd) break;
      }
    }

    switch (command)
    {
      // ----------------------------------------------------------------------
      // [Arc-To]
      // ----------------------------------------------------------------------

      case 'a':
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
// [Fog::SvgUtil - Parse - CSSStyle]
// ============================================================================

err_t parseCSSStyle(const StringW& src, CSSStyleHandlerFunc func, void* ctx)
{
  // Parse all "name: value;" pairs.
  const CharW* strCur = src.getData();
  const CharW* strEnd = strCur + src.getLength();

  err_t err = ERR_OK;
  
  StringW styleName;
  StringW styleValue;

  for (;;)
  {
    if (strCur == strEnd)
      break;

    const CharW* styleNameBegin;
    const CharW* styleNameEnd;
    const CharW* styleValueBegin;
    const CharW* styleValueEnd;

    // Skip spaces.
    while (strCur->isSpace())
    {
      if (++strCur == strEnd)
        goto _Bail;
    }

    // Parse style name.
    styleNameBegin = strCur;
    while (*strCur != CharW(':') && !strCur->isSpace())
    {
      if (++strCur == strEnd)
        goto _Bail;
    }
    styleNameEnd = strCur;

    if (strCur->isSpace())
    {
      while (*strCur != CharW(':'))
      {
        if (++strCur == strEnd)
          goto _Bail;
      }
    }

    // Skip ':'.
    if (++strCur == strEnd)
      goto _Bail;

    // Skip spaces.
    while (strCur->isSpace())
    {
      if (++strCur == strEnd)
        goto _Bail;
    }

    // Parse style value.
    styleValueBegin = strCur;
    while (*strCur != CharW(';'))
    {
      if (++strCur == strEnd)
        break;
    }
    styleValueEnd = strCur;

    // Remove trailing spaces.
    //
    // We can't cause buffer underflow, because we already parsed ':' that's
    // not space.
    while (styleValueEnd[-1].isSpace())
      styleValueEnd--;

    // Skip ';'.
    if (strCur != strEnd)
      strCur++;

    // TODO: What about errors?
    err = styleName.set(StubW(styleNameBegin, size_t(styleNameEnd - styleNameBegin)));
    if (FOG_IS_ERROR(err))
      continue;

    err = styleValue.set(StubW(styleValueBegin, size_t(styleValueEnd - styleValueBegin)));
    if (FOG_IS_ERROR(err))
      continue;

    err = func(ctx, &styleName, &styleValue);
    if (FOG_IS_ERROR(err))
      continue;
  }

_Bail:
  return err;
}

// ============================================================================
// [Fog::SvgUtil - Serialize - Color]
// ============================================================================

err_t serializeColor(StringW& dst, const Color& color)
{
  switch (color.getModel())
  {
    case COLOR_MODEL_ARGB:
    case COLOR_MODEL_AHSV:
    case COLOR_MODEL_ACMYK:
    {
      Argb32 argb32 = color.getArgb32();

      if (argb32.getAlpha() == 0xFF)
      {
        FOG_RETURN_ON_ERROR( dst.append(CharW('#')) );
        FOG_RETURN_ON_ERROR( dst.appendInt(argb32.getPacked32() & 0x00FFFFFF, FormatInt(16, NO_FLAGS, 6)) );
      }
      else
      {
        FOG_RETURN_ON_ERROR(
          dst.format("rgba(%d, %d, %d, %f)",
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

err_t serializeColor(StringW& dst, const Argb32& argb32)
{
  return dst.appendInt(argb32.getPacked32() & 0x00FFFFFF, FormatInt(16, NO_FLAGS, 6));
}

// ============================================================================
// [Fog::SvgUtil - Serialize - Offset]
// ============================================================================

err_t serializeOffset(StringW& dst, float src)
{
  FOG_RETURN_ON_ERROR(dst.appendFormat("%g", src));
  
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgUtil - Serialize - Opacity]
// ============================================================================

err_t serializeOpacity(StringW& dst, float src)
{
  return serializeOffset(dst, src);
}

// ============================================================================
// [Fog::SvgUtil - Serialize - Coord]
// ============================================================================

err_t serializeCoord(StringW& dst, const CoordF& src)
{
  float val = src.value;

  if (src.unit == UNIT_PERCENTAGE) val *= 100.0f;
  FOG_RETURN_ON_ERROR(dst.appendReal(src.value));

  if (src.unit < UNIT_COUNT && svgUnitNames[src.unit * 2] != '\0')
    FOG_RETURN_ON_ERROR(dst.append(Ascii8(&svgUnitNames[src.unit * 2], 2)));
  else if (src.unit == UNIT_PERCENTAGE)
    FOG_RETURN_ON_ERROR(dst.append(CharW('%')));

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgUtil - Serialize - ViewBox]
// ============================================================================

err_t serializeViewBox(StringW& dst, const BoxF& src)
{
  if (src.isValid())
  {
    FOG_RETURN_ON_ERROR(dst.appendFormat("%g %g %g %g", src.x0, src.y0, src.x1, src.y1));
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgUtil - Serialize - Path]
// ============================================================================

err_t serializePath(StringW& dst, const PathF& src)
{
  size_t pathLength = src.getLength();
  size_t i = 0;
  
  const uint8_t* cmd = src.getCommands();
  const PointF* pts = src.getVertices();

  bool isFirst = true;

  while (i < pathLength)
  {
    if (!isFirst)
      dst.append(CharW(' '));
    else
      isFirst = false;
      
    switch (cmd[0])
    {
      case PATH_CMD_MOVE_TO:
        FOG_RETURN_ON_ERROR(dst.appendFormat("M%g,%g", 
          pts[0].x, pts[0].y));

        i++;
        cmd++;
        pts++;
        break;
      
      case PATH_CMD_LINE_TO:
        FOG_RETURN_ON_ERROR(dst.appendFormat("L%g,%g",
          pts[0].x, pts[0].y));

        i++;
        cmd++;
        pts++;
        break;
      
      case PATH_CMD_QUAD_TO:
        if (i + 2 > pathLength)
          return ERR_RT_INVALID_STATE;

        FOG_RETURN_ON_ERROR(dst.appendFormat("Q%g,%g %g,%g",
          pts[0].x, pts[0].y,
          pts[1].x, pts[1].y));

        i += 2;
        cmd += 2;
        pts += 2;
        break;
      
      case PATH_CMD_CUBIC_TO:
        if (i + 3 > pathLength)
          return ERR_RT_INVALID_STATE;

        FOG_RETURN_ON_ERROR(dst.appendFormat("C%g,%g %g,%g %g,%g",
          pts[0].x, pts[0].y,
          pts[1].x, pts[1].y,
          pts[2].x, pts[2].y));

        i += 3;
        cmd += 3;
        pts += 3;
        break;

      case PATH_CMD_CLOSE:
        FOG_RETURN_ON_ERROR(dst.append(CharW('Z')));
        i++;
        cmd++;
        pts++;
        break;

      default:
        return ERR_RT_INVALID_STATE;
    }
  }
  
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgUtil - Serialize - Transform]
// ============================================================================

err_t serializeTransform(StringW& dst, const TransformF& src)
{
  switch (src.getType())
  {
    case TRANSFORM_TYPE_IDENTITY:
      return ERR_OK;

    case TRANSFORM_TYPE_TRANSLATION:
      if (Math::isFuzzyZero(src._21))
        return dst.appendFormat("translate(%g)", src._20);
      else
        return dst.appendFormat("translate(%g %g)", src._20, src._21);

    case TRANSFORM_TYPE_SCALING:
      if (Math::isFuzzyZero(src._20) && Math::isFuzzyZero(src._21))
      {
        if (Math::isFuzzyEq(src._00, src._11))
          return dst.appendFormat("scale(%g)", src._00);
        else
          return dst.appendFormat("scale(%g %g)", src._00, src._11);
      }
      // ... Fall through ...

    default:
      return dst.appendFormat("matrix(%g %g %g %g %g %g)",
        src._00, src._01,
        src._10, src._11,
        src._20, src._21);
  }
}

} // SvgUtil namespace
} // Fog namespace
