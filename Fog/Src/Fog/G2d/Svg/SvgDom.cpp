// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Core/Tools/Stream.h>
#include <Fog/G2d/Svg/SvgDocument.h>
#include <Fog/G2d/Svg/SvgDom_p.h>
#include <Fog/G2d/Svg/SvgElement.h>
#include <Fog/G2d/Svg/SvgRender.h>
#include <Fog/G2d/Svg/SvgUtil.h>
#include <Fog/G2d/Svg/SvgVisitor.h>

// Disable the MSC specific warning.
#if defined(FOG_CC_MSC)
# pragma warning(disable:4355) // 'this' used in base member initializer list.
#endif // FOG_CC_MSC

namespace Fog {

// TODO: List of unimplemented elements:
//   - <clipPath>
//   - <desc>
//   - <marker>
//   - <mask>
//   - <textPath>
//   - <title>
//   - <tref>

// ============================================================================
// [Fog::SvgHelpers]
// ============================================================================

// TODO: Rename to svgGetStyleId
static int svgStyleToId(const ManagedStringW& name)
{
  int i;

  for (i = 0; i < SVG_STYLE_INVALID; i++)
  {
    if (name == ManagedStringCacheW::get()->getString(i + STR_SVG_STYLE_NAMES))
      break;
  }

  return i;
}

static int svgGetEnumId(const StringW& value, const SvgEnumItem* items)
{
  while (items->name[0] != '\0')
  {
    if (value == Ascii8(items->name)) break;
    items++;
  }

  return items->value;
}

static StubW parseHtmlLinkId(const StringW& url)
{
  const CharW* idStr;
  const CharW* idEnd;
  const CharW* idMark;
  CharW c;

  if (url.getLength() < 2) goto bail;

  idStr = url.getData();
  idEnd = idStr + url.getLength();

  if (*idStr != CharW('#')) goto bail;
  idStr++;

  while (idStr->isSpace())
  {
    if (++idStr == idEnd) goto bail;
  }

  idMark = idStr;
  while ((c = *idStr).isAsciiNumlet() || c == CharW('-') || c == CharW('_'))
  {
    if (++idStr == idEnd) break;
  }
  return StubW(idMark, (size_t)(idStr - idMark));

bail:
  return StubW((const CharW*)NULL, 0);
}

static StubW parseCssLinkId(const StringW& url)
{
  const CharW* idStr;
  const CharW* idEnd;
  const CharW* idMark;

  CharW quot;

  if (url.getLength() < 7) goto _Bail;

  idStr = url.getData();
  idEnd = idStr + url.getLength();

  // Add url(
  idStr += 4;

  // Detect quot, if used.
  quot = idStr[0];
  if (quot == CharW('\"') || quot == CharW('\''))
  {
    idStr++;
    idEnd--;
  }
  else
  {
    quot = CharW(')');
  }

  // Invalid ID.
  if (idStr + 1 >= idEnd || idStr[0] != CharW('#'))
    goto _Bail;
  idStr++;

  while (idStr->isSpace())
  {
    if (++idStr == idEnd)
      goto _Bail;
  }

  idMark = idStr;
  while (idStr[0] != quot)
  {
    if (++idStr == idEnd)
      goto _Bail;
  }
  return StubW(idMark, (size_t)(idStr - idMark));

_Bail:
  return StubW((const CharW*)NULL, 0);
}

// ============================================================================
// [Fog::SvgEnumItem - Data]
// ============================================================================

static const SvgEnumItem svgEnum_fillRule[] =
{
  { "nonzero", FILL_RULE_NON_ZERO },
  { "evenodd", FILL_RULE_EVEN_ODD },
  { "", -1 }
};

static const SvgEnumItem svgEnum_gradientUnits[3] =
{
  { "userSpaceOnUse", SVG_USER_SPACE_ON_USE },
  { "objectBoundingBox", SVG_OBJECT_BOUNDING_BOX },
  { "", -1 }
};

static const SvgEnumItem svgEnum_lengthAdjust[3] =
{
  { "spacing", SVG_LENGTH_ADJUST_SPACING },
  { "spacingAndGlyphs", SVG_LENGTH_ADJUST_SPACING_AND_GLYPHS },
  { "", -1 }
};

static const SvgEnumItem svgEnum_spreadMethod[4] =
{
  { "pad", GRADIENT_SPREAD_PAD },
  { "reflect", GRADIENT_SPREAD_REFLECT },
  { "repeat", GRADIENT_SPREAD_REPEAT },
  { "", -1 }
};

static const SvgEnumItem svgEnum_strokeLineCap[] =
{
  { "butt", LINE_CAP_BUTT },
  { "round", LINE_CAP_ROUND },
  { "square", LINE_CAP_SQUARE },
  { "", -1 }
};

static const SvgEnumItem svgEnum_strokeLineJoin[] =
{
  { "miter", LINE_JOIN_MITER },
  { "round", LINE_JOIN_ROUND },
  { "bevel", LINE_JOIN_BEVEL },
  { "", -1 }
};

// ============================================================================
// [Fog::SvgEnumAttribute]
// ============================================================================

SvgEnumAttribute::SvgEnumAttribute(XmlElement* element, const ManagedStringW& name, int offset, const SvgEnumItem* items) :
  XmlAttribute(element, name, offset),
  _enumItems(items),
  _enumValue(-1)
{
}

SvgEnumAttribute::~SvgEnumAttribute()
{
}

err_t SvgEnumAttribute::setValue(const StringW& value)
{
  err_t err = _value.set(value);
  if (FOG_IS_ERROR(err)) return err;

  _enumValue = svgGetEnumId(value, _enumItems);
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgCoordAttribute]
// ============================================================================

SvgCoordAttribute::SvgCoordAttribute(XmlElement* element, const ManagedStringW& name, int offset) :
  XmlAttribute(element, name, offset)
{
  _coord.value = 0.0;
  _coord.unit = UNIT_PX;
}

SvgCoordAttribute::~SvgCoordAttribute()
{
}

err_t SvgCoordAttribute::setValue(const StringW& value)
{
  err_t err = _value.set(value);
  if (FOG_IS_ERROR(err)) return err;

  SvgUtil::parseCoord(_coord, value);

  if (_element) reinterpret_cast<SvgElement*>(_element)->_boundingBoxDirty = true;
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgImageLinkAttribute]
// ============================================================================

SvgImageLinkAttribute::SvgImageLinkAttribute(XmlElement* element, const ManagedStringW& name, int offset) :
  XmlAttribute(element, name, offset),
  _embedded(false)
{
}

SvgImageLinkAttribute::~SvgImageLinkAttribute()
{
}

StringW SvgImageLinkAttribute::getValue() const
{
  if (_embedded)
  {
    err_t err = ERR_OK;

    StringW dst;
    Stream memio;

    err |= dst.append(Ascii8("data:image/png;base64,"));
    err |= memio.openBuffer();

    _image.writeToStream(memio, StringW::fromAscii8("png"));
    err |= StringW::base64Encode(dst, CONTAINER_OP_APPEND, memio.getBuffer());

    if (FOG_IS_ERROR(err)) dst.reset();
    return dst;
  }
  else
  {
    return _value;
  }
}

err_t SvgImageLinkAttribute::setValue(const StringW& value)
{
  err_t err = ERR_OK;

  if (value.startsWith(Ascii8("data:")))
  {
    size_t semicolon = value.indexOf(CharW(';'));
    size_t separator = value.indexOf(CharW(','));

    if (semicolon != INVALID_INDEX && separator != INVALID_INDEX)
    {
      StringW type = value.substring(Range(5, semicolon));
      StringW extension;
      StringW encoding = value.substring(Range(semicolon + 1, separator));

      StringA memio;
      Stream stream;

      if (type == Ascii8("image/png"))
      {
        extension = FOG_STR_(IMAGE_EXT_png);
      }
      else if (type == Ascii8("image/jpeg"))
      {
        extension = FOG_STR_(IMAGE_EXT_jpeg);
      }
      else
      {
        // Standard talks only about PNG and JPEG, but we can use any attached
        // image that can be decoded by Fog-Imaging API.
      }

      if (encoding == Ascii8("base64"))
      {
        err |= StringA::base64Decode(memio, CONTAINER_OP_REPLACE, value.getData() + separator + 1, value.getLength() - separator - 1);
      }
      else
      {
        // Maybe in future something else will be supported by the SVG standard.
        return ERR_SVG_INVALID_DATA_ENCODING;
      }

      err |= stream.openBuffer(memio);
      err |= _image.readFromStream(stream, extension);

      if (FOG_IS_ERROR(err))
      {
        // Something evil happened. I don't know what to do now, because image
        // seems to be corrupted or unsupported.
        _value.set(value);
        _image.reset();
        _embedded = false;
      }
      else
      {
        _value.reset();
        _embedded = true;
      }
    }
  }

  err = _value.set(value);
  if (_element) reinterpret_cast<SvgElement*>(_element)->_boundingBoxDirty = true;
  return err;
}

// ============================================================================
// [Fog::SvgOffsetAttribute]
// ============================================================================

SvgOffsetAttribute::SvgOffsetAttribute(XmlElement* element, const ManagedStringW& name, int offset) :
  XmlAttribute(element, name, offset)
{
  _offset = 0.0;
}

SvgOffsetAttribute::~SvgOffsetAttribute()
{
}

err_t SvgOffsetAttribute::setValue(const StringW& value)
{
  err_t err = _value.set(value);
  if (FOG_IS_ERROR(err)) return err;

  size_t end;
  if (value.parseReal(&_offset, CharW('.'), &end, NULL) == ERR_OK)
  {
    if (end < value.getLength() && value.getAt(end) == CharW('%')) _offset *= 0.01f;
    _offset = Math::bound<float>(_offset, 0.0f, 1.0f);
  }
  else
  {
    _offset = 0.0f;
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgPathAttribute]
// ============================================================================

SvgPathAttribute::SvgPathAttribute(XmlElement* element, const ManagedStringW& name, int offset) :
  XmlAttribute(element, name, offset)
{
}

SvgPathAttribute::~SvgPathAttribute()
{
}

err_t SvgPathAttribute::setValue(const StringW& value)
{
  FOG_RETURN_ON_ERROR(_value.set(value));
  FOG_RETURN_ON_ERROR(SvgUtil::parsePath(_path, value));

  // Build path-info to optimize path operations.
  _path.buildPathInfo();

  if (_element != NULL)
    reinterpret_cast<SvgElement*>(_element)->_boundingBoxDirty = true;

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgPointsAttribute]
// ============================================================================

SvgPointsAttribute::SvgPointsAttribute(XmlElement* element, const ManagedStringW& name, bool closePath, int offset) :
  XmlAttribute(element, name, offset),
  _closePath(closePath)
{
}

SvgPointsAttribute::~SvgPointsAttribute()
{
}

err_t SvgPointsAttribute::setValue(const StringW& value)
{
  FOG_RETURN_ON_ERROR(_value.set(value));
  FOG_RETURN_ON_ERROR(SvgUtil::parsePoints(_path, value, _closePath));

  // Close path if used by closed object (SvgPolygon).
  if (!_path.isEmpty() && _closePath)
    _path.close();

  // Build path-info to optimize path operations.
  _path.buildPathInfo();

  if (_element != NULL)
    reinterpret_cast<SvgElement*>(_element)->_boundingBoxDirty = true;

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgTransformAttribute]
// ============================================================================

SvgTransformAttribute::SvgTransformAttribute(XmlElement* element, const ManagedStringW& name, int offset) :
    XmlAttribute(element, name, offset)
{
}

SvgTransformAttribute::~SvgTransformAttribute()
{
}

err_t SvgTransformAttribute::setValue(const StringW& value)
{
  err_t err = _value.set(value);
  if (FOG_IS_ERROR(err)) return err;

  _isValid = (SvgUtil::parseTransform(_transform, value) == ERR_OK);
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgViewBoxAttribute]
// ============================================================================

SvgViewBoxAttribute::SvgViewBoxAttribute(XmlElement* element, const ManagedStringW& name, int offset) :
  XmlAttribute(element, name, offset)
{
  _box.reset();
}

SvgViewBoxAttribute::~SvgViewBoxAttribute()
{
}

err_t SvgViewBoxAttribute::setValue(const StringW& value)
{
  err_t err = _value.set(value);
  if (FOG_IS_ERROR(err)) return err;

  SvgUtil::parseViewBox(_box, value);
  _isValid = _box.isValid();

  if (_element) reinterpret_cast<SvgElement*>(_element)->_boundingBoxDirty = true;
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgStyleAttribute]
// ============================================================================

SvgStyleAttribute::SvgStyleAttribute(XmlElement* element, const ManagedStringW& name, int offset) :
  XmlAttribute(element, name, offset),

  _mask(0),

  _clipRule(FILL_RULE_DEFAULT),
  _fillSource(SVG_SOURCE_NONE),
  _fillRule(FILL_RULE_DEFAULT),
  _strokeSource(SVG_SOURCE_NONE),

  _strokeLineCap(LINE_CAP_DEFAULT),
  _strokeLineJoin(LINE_JOIN_DEFAULT),
  _strokeDashOffsetUnit(UNIT_PX),
  _strokeMiterLimitUnit(UNIT_PX),

  _strokeWidthUnit(UNIT_PX),
  _fontSizeUnit(UNIT_PX),
  _letterSpacingUnit(UNIT_PX),
  _reserved_0(0),

  _fillColor(),
  _strokeColor(),
  _stopColor(),

  _opacity(0.0f),
  _fillOpacity(0.0f),
  _strokeOpacity(0.0f),
  _stopOpacity(0.0f),

  _strokeDashOffsetValue(0.0f),
  _strokeMiterLimitValue(0.0f),
  _strokeWidthValue(0.0f),
  _fontSizeValue(0.0f),
  _letterSpacingValue(0.0f)
{
}

SvgStyleAttribute::~SvgStyleAttribute()
{
}

StringW SvgStyleAttribute::getValue() const
{
  StringW result;
  result.reserve(128);

  int i;
  for (i = 0; i < SVG_STYLE_INVALID; i++)
  {
    if (_mask & (1 << i))
    {
      result.append(ManagedStringCacheW::get()->getString(i + STR_SVG_STYLE_NAMES));
      result.append(CharW(':'));
      result.append(getStyle(i));
      result.append(CharW(';'));
    }
  }

  return result;
}

err_t SvgStyleAttribute::setValue(const StringW& value)
{
  // Parse all "name: value;" pairs.
  const CharW* strCur = value.getData();
  const CharW* strEnd = strCur + value.getLength();

  ManagedStringW styleName;
  StringW styleValue;

  for (;;)
  {
    if (strCur == strEnd) break;

    const CharW* styleNameBegin;
    const CharW* styleNameEnd;
    const CharW* styleValueBegin;
    const CharW* styleValueEnd;

    err_t err;

    // Skip spaces.
    while (strCur->isSpace())
    {
      if (++strCur == strEnd) goto _Bail;
    }

    // Parse style name.
    styleNameBegin = strCur;
    while (*strCur != CharW(':') && !strCur->isSpace())
    {
      if (++strCur == strEnd) goto _Bail;
    }
    styleNameEnd = strCur;

    if (strCur->isSpace())
    {
      while (*strCur != CharW(':'))
      {
        if (++strCur == strEnd) goto _Bail;
      }
    }

    // Skip ':'.
    if (++strCur == strEnd) goto _Bail;

    // Skip spaces.
    while (strCur->isSpace())
    {
      if (++strCur == strEnd) goto _Bail;
    }

    // Parse style value.
    styleValueBegin = strCur;
    while (*strCur != CharW(';'))
    {
      if (++strCur == strEnd) break;
    }
    styleValueEnd = strCur;

    // Remove trailing spaces.
    //
    // We can't cause buffer underflow, because we already parsed ':' that's
    // not space.
    while (styleValueEnd[-1].isSpace()) styleValueEnd--;

    // Skip ';'.
    if (strCur != strEnd) strCur++;

    err = styleName.set(StubW(styleNameBegin, size_t(styleNameEnd - styleNameBegin)));
    if (FOG_IS_ERROR(err)) continue;

    err = styleValue.set(StubW(styleValueBegin, size_t(styleValueEnd - styleValueBegin)));
    if (FOG_IS_ERROR(err)) continue;

    reinterpret_cast<SvgElement*>(getElement())->setStyle(styleName, styleValue);
  }

_Bail:
  return ERR_OK;
}

StringW SvgStyleAttribute::getStyle(int styleId) const
{
  StringW result;

  // Don't process non-used style values.
  if ((_mask & (1 << styleId)) == 0) goto _End;

  switch (styleId)
  {
    case SVG_STYLE_CLIP_PATH:
      // SVG TODO:
      break;

    case SVG_STYLE_CLIP_RULE:
      FOG_ASSERT(_clipRule < FILL_RULE_COUNT);
      result.set(Ascii8(svgEnum_fillRule[_clipRule].name));
      break;

    case SVG_STYLE_ENABLE_BACKGROUND:
      // SVG TODO:
      break;

    case SVG_STYLE_FILL:
      switch (_fillSource)
      {
        case SVG_SOURCE_NONE:
          result.append(Ascii8("none"));
          break;
        case SVG_SOURCE_COLOR:
          SvgUtil::serializeColor(result, _fillColor);
          break;
        case SVG_SOURCE_URI:
          result = _fillUri;
          break;
      }
      break;

    case SVG_STYLE_FILL_OPACITY:
      result.setReal(_fillOpacity);
      break;

    case SVG_STYLE_FILL_RULE:
      FOG_ASSERT(_fillRule < FILL_RULE_COUNT);
      result.set(Ascii8(svgEnum_fillRule[_fillRule].name));
      break;

    case SVG_STYLE_FILTER:
      // SVG TODO:
      break;

    case SVG_STYLE_FONT_FAMILY:
      result.set(_fontFamily);
      break;

    case SVG_STYLE_FONT_SIZE:
      SvgUtil::serializeCoord(result, getFontSize());
      break;

    case SVG_STYLE_LETTER_SPACING:
      SvgUtil::serializeCoord(result, getLetterSpacing());
      break;

    case SVG_STYLE_MASK:
      // SVG TODO:
      break;

    case SVG_STYLE_OPACITY:
      result.setReal(_opacity);
      break;

    case SVG_STYLE_STOP_COLOR:
      SvgUtil::serializeColor(result, _stopColor);
      break;

    case SVG_STYLE_STOP_OPACITY:
      result.setReal(_stopOpacity);
      break;

    case SVG_STYLE_STROKE:
      switch (_strokeSource)
      {
        case SVG_SOURCE_NONE:
          result.append(Ascii8("none"));
          break;
        case SVG_SOURCE_COLOR:
          SvgUtil::serializeColor(result, _strokeColor);
          break;
        case SVG_SOURCE_URI:
          result = _strokeUri;
          break;
      }
      break;

    case SVG_STYLE_STROKE_DASH_ARRAY:
      // SVG TODO:
      break;

    case SVG_STYLE_STROKE_DASH_OFFSET:
      SvgUtil::serializeCoord(result, getStrokeDashOffset());
      break;

    case SVG_STYLE_STROKE_LINE_CAP:
      FOG_ASSERT(_strokeLineCap < LINE_CAP_COUNT);
      result.set(Ascii8(svgEnum_strokeLineCap[_strokeLineCap].name));
      break;

    case SVG_STYLE_STROKE_LINE_JOIN:
      FOG_ASSERT(_strokeLineJoin < LINE_JOIN_COUNT);
      result.set(Ascii8(svgEnum_strokeLineJoin[_strokeLineJoin].name));
      break;

    case SVG_STYLE_STROKE_MITER_LIMIT:
      SvgUtil::serializeCoord(result, getStrokeMiterLimit());
      break;

    case SVG_STYLE_STROKE_OPACITY:
      result.setReal(_strokeOpacity);
      break;

    case SVG_STYLE_STROKE_WIDTH:
      SvgUtil::serializeCoord(result, getStrokeWidth());
      break;

    default:
      break;
  }
_End:
  return result;
}

err_t SvgStyleAttribute::setStyle(int styleId, const StringW& value)
{
  err_t err = ERR_OK;
  int i;

  if (value.isEmpty())
  {
    _mask &= ~(1 << styleId);
    return ERR_OK;
  }

  switch (styleId)
  {
    case SVG_STYLE_CLIP_PATH:
    {
      // SVG TODO:
      err = ERR_RT_NOT_IMPLEMENTED;
      break;
    }

    case SVG_STYLE_CLIP_RULE:
    {
      i = svgGetEnumId(value, svgEnum_fillRule);
      if (i == -1)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      else
        _clipRule = (uint8_t)(uint)i;
      break;
    }

    case SVG_STYLE_ENABLE_BACKGROUND:
    {
      // SVG TODO:
      err = ERR_RT_NOT_IMPLEMENTED;
      break;
    }

    case SVG_STYLE_FILL:
    {
      _fillSource = (uint8_t)SvgUtil::parseColor(_fillColor, value);
      switch (_fillSource)
      {
        case SVG_SOURCE_NONE:
        case SVG_SOURCE_COLOR:
          break;
        case SVG_SOURCE_URI:
          _fillUri = value;
          break;
        case SVG_SOURCE_INVALID:
          err = ERR_SVG_INVALID_STYLE_VALUE;
          break;
      }
      break;
    }

    case SVG_STYLE_FILL_OPACITY:
    {
      err = SvgUtil::parseOpacity(_fillOpacity, value);
      break;
    }

    case SVG_STYLE_FILL_RULE:
    {
      i = svgGetEnumId(value, svgEnum_fillRule);
      if (i == -1)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      else
        _fillRule = (uint8_t)(uint)i;
      break;
    }

    case SVG_STYLE_FILTER:
    {
      // SVG TODO:
      err = ERR_RT_NOT_IMPLEMENTED;
      break;
    }

    case SVG_STYLE_FONT_FAMILY:
    {
      err = _value.set(value);
      break;
    }

    case SVG_STYLE_FONT_SIZE:
    {
      CoordF coord(UNINITIALIZED);
      if (SvgUtil::parseCoord(coord, value) != ERR_OK)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      setFontSize(coord);
      break;
    }

    case SVG_STYLE_LETTER_SPACING:
    {
      CoordF coord(UNINITIALIZED);
      if (SvgUtil::parseCoord(coord, value) != ERR_OK)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      setLetterSpacing(coord);
      break;
    }

    case SVG_STYLE_MASK:
    {
      err = ERR_RT_NOT_IMPLEMENTED;
      break;
    }

    case SVG_STYLE_OPACITY:
    {
      err = SvgUtil::parseOpacity(_opacity, value);
      break;
    }

    case SVG_STYLE_STOP_COLOR:
    {
      if (SvgUtil::parseColor(_stopColor, value) != SVG_SOURCE_COLOR)
      {
        err = ERR_SVG_INVALID_STYLE_VALUE;
      }
      break;
    }

    case SVG_STYLE_STOP_OPACITY:
    {
      err = SvgUtil::parseOpacity(_stopOpacity, value);
      break;
    }

    case SVG_STYLE_STROKE:
    {
      _strokeSource = (uint8_t)SvgUtil::parseColor(_strokeColor, value);
      switch (_strokeSource)
      {
        case SVG_SOURCE_NONE:
        case SVG_SOURCE_COLOR:
          break;
        case SVG_SOURCE_URI:
          _strokeUri = value;
          break;
        case SVG_SOURCE_INVALID:
          err = ERR_SVG_INVALID_STYLE_VALUE;
          break;
      }
      break;
    }

    case SVG_STYLE_STROKE_DASH_ARRAY:
    {
      //err = ERR_RT_NOT_IMPLEMENTED;
      break;
    }

    case SVG_STYLE_STROKE_DASH_OFFSET:
    {
      CoordF coord(UNINITIALIZED);
      if (SvgUtil::parseCoord(coord, value) != ERR_OK)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      setStrokeDashOffset(coord);
      break;
    }

    case SVG_STYLE_STROKE_LINE_CAP:
    {
      i = svgGetEnumId(value, svgEnum_strokeLineCap);
      if (i == -1)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      else
        _strokeLineCap = (uint8_t)(uint)i;
      break;
    }

    case SVG_STYLE_STROKE_LINE_JOIN:
    {
      i = svgGetEnumId(value, svgEnum_strokeLineJoin);
      if (i == -1)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      else
        _strokeLineJoin = (uint8_t)(uint)i;
      break;
    }

    case SVG_STYLE_STROKE_MITER_LIMIT:
    {
      CoordF coord(UNINITIALIZED);
      if (SvgUtil::parseCoord(coord, value) != ERR_OK)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      setStrokeMiterLimit(coord);
      break;
    }

    case SVG_STYLE_STROKE_OPACITY:
    {
      err = SvgUtil::parseOpacity(_strokeOpacity, value);
      break;
    }

    case SVG_STYLE_STROKE_WIDTH:
    {
      CoordF coord(UNINITIALIZED);
      if (SvgUtil::parseCoord(coord, value) != ERR_OK)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      setStrokeWidth(coord);
      break;
    }

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }

  if (err == ERR_OK)
    _mask |= (1 << styleId);
  else
    _mask &= ~(1 << styleId);

  return err;
}

// ============================================================================
// [Fog::SvgStyledElement]
// ============================================================================

SvgStyledElement::SvgStyledElement(const ManagedStringW& tagName, uint32_t svgType) :
  SvgElement(tagName, svgType),
  a_style    (this, FOG_STR_(XML_ATTRIBUTE_style), FOG_OFFSET_OF(SvgStyledElement, a_style)),
  a_transform(NULL, FOG_STR_(SVG_ATTRIBUTE_transform), FOG_OFFSET_OF(SvgStyledElement, a_transform))
{
  // Style attribute is always added as default and can't be removed.
  _attributes.append(&a_style);
}

SvgStyledElement::~SvgStyledElement()
{
  // Class that inherits us must destroy all attributes (a_style must be
  // removed from _attributes too).
  FOG_ASSERT(_attributes.isEmpty());
}

XmlAttribute* SvgStyledElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(XML_ATTRIBUTE_style)) return (XmlAttribute*)&a_style;
  if (name == FOG_STR_(SVG_ATTRIBUTE_transform)) return (XmlAttribute*)&a_transform;

  return base::_createAttribute(name);
}

err_t SvgStyledElement::_setAttribute(const ManagedStringW& name, const StringW& value)
{
  // Add css-style instead of attribute.
  int id = svgStyleToId(name);
  if ((uint)id < SVG_STYLE_INVALID)
  {
    // Do not return an error value here, because if Xml/Svg parser see error
    // then parsing is over. This means that there is probability we can't
    // render correctly some image, but we want to render what we can.
    a_style.setStyle(id, value);
    return ERR_OK;
  }

  return base::_setAttribute(name, value);
}

err_t SvgStyledElement::_removeAttribute(const ManagedStringW& name)
{
  if (name == FOG_STR_(XML_ATTRIBUTE_style)) return ERR_XML_ATTRIBUTE_CANT_BE_REMOVED;

  return base::_removeAttribute(name);
}

err_t SvgStyledElement::onPrepare(SvgVisitor* visitor, SvgGState* state) const
{
  // Apply transformations and setup styles defined by this element.
  uint32_t styleMask = a_style.getMask();
  bool isTransformed = a_transform.isAssigned() & a_transform.isValid();

  if (styleMask != 0 || isTransformed)
  {
    SvgDocument* doc = reinterpret_cast<SvgDocument*>(getDocument());

    if (isTransformed)
    {
      if (state && !state->hasState(SvgGState::SAVED_TRANSFORM))
        state->saveTransform();
      visitor->transform(a_transform.getTransform());
    }

    // Setup global parameters.
    if (styleMask & ((1 << SVG_STYLE_OPACITY)))
    {
      if (state) state->saveGlobal();
      visitor->setOpacity(a_style._opacity);
    }

    // Setup font parameters.
    if (styleMask & ((1 << SVG_STYLE_FONT_FAMILY) |
                     (1 << SVG_STYLE_FONT_SIZE  )))
    {
      if (state) state->saveFont();

      StringW family = visitor->_font.getFamily();
      float size = visitor->_font.getHeight();

      if (styleMask & (1 << SVG_STYLE_FONT_FAMILY))
      {
        family = a_style._fontFamily;
      }

      if (styleMask & (1 << SVG_STYLE_FONT_SIZE))
      {
        size = doc->_dpi.toDeviceSpace(
          a_style._fontSizeValue, a_style._fontSizeUnit);
      }

      visitor->_font.create(family, size, UNIT_NONE);
    }

    // Setup fill parameters.
    if (styleMask & ((1 << SVG_STYLE_FILL               ) |
                     (1 << SVG_STYLE_FILL_OPACITY       ) |
                     (1 << SVG_STYLE_FILL_RULE          )))
    {
      if (state) state->saveFill();

      if (styleMask & (1 << SVG_STYLE_FILL))
      {
        switch (a_style._fillSource)
        {
          case SVG_SOURCE_NONE:
          case SVG_SOURCE_INVALID:
          {
            visitor->setFillNone();
            break;
          }
          case SVG_SOURCE_COLOR:
          {
            visitor->setFillColor(a_style._fillColor);
            break;
          }
          case SVG_SOURCE_URI:
          {
            XmlElement* r = getDocument()->getElementById(parseCssLinkId(a_style._fillUri));
            if (r && r->isSvg())
            {
              reinterpret_cast<SvgElement*>(r)->onPattern(visitor, const_cast<SvgStyledElement*>(this), SVG_PAINT_FILL);
            }
            break;
          }
        }
      }

      if (styleMask & (1 << SVG_STYLE_FILL_OPACITY))
      {
        visitor->setFillOpacity(a_style._fillOpacity);
      }

      if (styleMask & (1 << SVG_STYLE_FILL_RULE))
      {
        visitor->setFillRule(a_style._fillRule);
      }
    }

    // Setup stroke parameters.
    if (styleMask & ((1 << SVG_STYLE_STROKE             ) |
                     (1 << SVG_STYLE_STROKE_DASH_ARRAY  ) |
                     (1 << SVG_STYLE_STROKE_DASH_OFFSET ) |
                     (1 << SVG_STYLE_STROKE_LINE_CAP    ) |
                     (1 << SVG_STYLE_STROKE_LINE_JOIN   ) |
                     (1 << SVG_STYLE_STROKE_MITER_LIMIT ) |
                     (1 << SVG_STYLE_STROKE_OPACITY     ) |
                     (1 << SVG_STYLE_STROKE_WIDTH       )))
    {
      if (state) state->saveStroke();

      if (styleMask & (1 << SVG_STYLE_STROKE))
      {
        switch (a_style._strokeSource)
        {
          case SVG_SOURCE_NONE:
          case SVG_SOURCE_INVALID:
          {
            visitor->setStrokeNone();
            break;
          }
          case SVG_SOURCE_COLOR:
          {
            visitor->setStrokeColor(a_style._strokeColor);
            break;
          }
          case SVG_SOURCE_URI:
          {
            XmlElement* r = getDocument()->getElementById(parseCssLinkId(a_style._strokeUri));
            if (r && r->isSvg())
            {
              reinterpret_cast<SvgElement*>(r)->onPattern(visitor, const_cast<SvgStyledElement*>(this), SVG_PAINT_STROKE);
            }
            break;
          }
        }
      }

      if (styleMask & (1 << SVG_STYLE_STROKE_LINE_CAP))
      {
        visitor->setLineCaps(a_style._strokeLineCap);
      }

      if (styleMask & (1 << SVG_STYLE_STROKE_LINE_JOIN))
      {
        visitor->setLineJoin(a_style._strokeLineJoin);
      }

      if (styleMask & (1 << SVG_STYLE_STROKE_MITER_LIMIT))
      {
        float miterLimit = doc->_dpi.toDeviceSpace(
          a_style._strokeMiterLimitValue, a_style._strokeMiterLimitUnit);
        visitor->setMiterLimit(miterLimit);
      }

      if (styleMask & (1 << SVG_STYLE_STROKE_OPACITY))
      {
        visitor->setStrokeOpacity(a_style._strokeOpacity);
      }

      if (styleMask & (1 << SVG_STYLE_STROKE_WIDTH))
      {
        float lineWidth = doc->_dpi.toDeviceSpace(
          a_style._strokeWidthValue, a_style._strokeWidthUnit);
        visitor->setLineWidth(lineWidth);
      }
    }
  }

  return ERR_OK;
}

StringW SvgStyledElement::getStyle(const StringW& name) const
{
  StringW result;
  ManagedStringW m_name(name, MANAGED_STRING_OPTION_LOOKUP);

  if (m_name.isEmpty())
    return result;

  int id = svgStyleToId(m_name);
  if ((uint)id >= SVG_STYLE_INVALID)
    return result;

  return a_style.getStyle(id);
}

err_t SvgStyledElement::setStyle(const StringW& name, const StringW& value)
{
  ManagedStringW m_name(name, MANAGED_STRING_OPTION_LOOKUP);

  if (m_name.isEmpty())
    return ERR_SVG_INVALID_STYLE_NAME;

  int id = svgStyleToId(m_name);
  if ((uint)id >= SVG_STYLE_INVALID)
    return ERR_SVG_INVALID_STYLE_NAME;

  return a_style.setStyle(id, value);
}

// ============================================================================
// [Fog::SvgRootElement]
// ============================================================================

SvgRootElement::SvgRootElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_svg), SVG_ELEMENT_SVG),
  a_x      (NULL, FOG_STR_(SVG_ATTRIBUTE_x      ), FOG_OFFSET_OF(SvgRootElement, a_x      )),
  a_y      (NULL, FOG_STR_(SVG_ATTRIBUTE_y      ), FOG_OFFSET_OF(SvgRootElement, a_y      )),
  a_width  (NULL, FOG_STR_(SVG_ATTRIBUTE_width  ), FOG_OFFSET_OF(SvgRootElement, a_width  )),
  a_height (NULL, FOG_STR_(SVG_ATTRIBUTE_height ), FOG_OFFSET_OF(SvgRootElement, a_height )),
  a_viewBox(NULL, FOG_STR_(SVG_ATTRIBUTE_viewBox), FOG_OFFSET_OF(SvgRootElement, a_viewBox))
{
}

SvgRootElement::~SvgRootElement()
{
  _removeAttributes();
}

XmlAttribute* SvgRootElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_x      )) return (XmlAttribute*)&a_x;
  if (name == FOG_STR_(SVG_ATTRIBUTE_y      )) return (XmlAttribute*)&a_y;
  if (name == FOG_STR_(SVG_ATTRIBUTE_width  )) return (XmlAttribute*)&a_width;
  if (name == FOG_STR_(SVG_ATTRIBUTE_height )) return (XmlAttribute*)&a_height;
  if (name == FOG_STR_(SVG_ATTRIBUTE_viewBox)) return (XmlAttribute*)&a_viewBox;

  return base::_createAttribute(name);
}

err_t SvgRootElement::onPrepare(SvgVisitor* visitor, SvgGState* state) const
{
  if (a_viewBox.isAssigned() && a_viewBox.isValid())
  {
    if (state) state->saveTransform();

    SizeF size = getRootSize();
    BoxF box = a_viewBox.getBox();

    TransformF tr(
      size.w / box.getWidth(), 0.0f,
      0.0f, size.h / box.getHeight(),
      -box.x0, -box.y0);
    visitor->transform(tr);
  }

  return base::onPrepare(visitor, state);
}

err_t SvgRootElement::onProcess(SvgVisitor* visitor) const
{
  if (!hasChildNodes()) return ERR_OK;
  return _visitContainer(visitor);
}

err_t SvgRootElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

SizeF SvgRootElement::getRootSize() const
{
  SizeF size(0.0f, 0.0f);

  // Width/Height of document are assigned.
  if (a_width.isAssigned() && a_height.isAssigned())
  {
    float w = a_width.getCoordComputed();
    float h = a_height.getCoordComputed();

    if (a_width.getCoordUnit() == UNIT_PERCENTAGE || a_height.getCoordUnit() == UNIT_PERCENTAGE)
    {
      if (a_viewBox.isAssigned() && a_viewBox.isValid())
      {
        w = a_viewBox.getBox().getWidth();
        h = a_viewBox.getBox().getHeight();
      }
      else
      {
        w = 0.0f;
        h = 0.0f;
      }
    }

    size.set(w, h);
  }

  // If size is not specified of value is invalid, initialize it to
  // safe values.
  if (size.w <= 0.0f || size.h <= 0.0f)
  {
    size.w = 128.0f;
    size.h = 128.0f;
  }

  return size;
}

// ============================================================================
// [Fog::SvgSolidColorElement]
// ============================================================================

SvgSolidColorElement::SvgSolidColorElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_solidColor), SVG_ELEMENT_SOLID_COLOR)
{
}

SvgSolidColorElement::~SvgSolidColorElement()
{
  _removeAttributes();
}

XmlAttribute* SvgSolidColorElement::_createAttribute(const ManagedStringW& name) const
{
  return base::_createAttribute(name);
}

err_t SvgSolidColorElement::onProcess(SvgVisitor* visitor) const
{
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgAbstractGradientElement]
// ============================================================================

SvgAbstractGradientElement::SvgAbstractGradientElement(const ManagedStringW& tagName, uint32_t svgType) :
  SvgStyledElement(tagName, svgType),
  a_spreadMethod     (NULL, FOG_STR_(SVG_ATTRIBUTE_spreadMethod     ), FOG_OFFSET_OF(SvgAbstractGradientElement, a_spreadMethod     ), svgEnum_spreadMethod),
  a_gradientUnits    (NULL, FOG_STR_(SVG_ATTRIBUTE_gradientUnits    ), FOG_OFFSET_OF(SvgAbstractGradientElement, a_gradientUnits    ), svgEnum_gradientUnits),
  a_gradientTransform(NULL, FOG_STR_(SVG_ATTRIBUTE_gradientTransform), FOG_OFFSET_OF(SvgAbstractGradientElement, a_gradientTransform))
{
}

SvgAbstractGradientElement::~SvgAbstractGradientElement()
{
  _removeAttributes();
}

XmlAttribute* SvgAbstractGradientElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_spreadMethod     )) return (XmlAttribute*)&a_spreadMethod;
  if (name == FOG_STR_(SVG_ATTRIBUTE_gradientUnits    )) return (XmlAttribute*)&a_gradientUnits;
  if (name == FOG_STR_(SVG_ATTRIBUTE_gradientTransform)) return (XmlAttribute*)&a_gradientTransform;

  return base::_createAttribute(name);
}

err_t SvgAbstractGradientElement::onProcess(SvgVisitor* visitor) const
{
  return ERR_OK;
}

void SvgAbstractGradientElement::_walkAndAddColorStops(XmlElement* root, GradientF& gradient)
{
  bool stopsParsed = false;
  XmlElement* stop;
  int depth = 0;

_Start:
  for (stop = root->getFirstChild(); stop; stop = stop->getNextSibling())
  {
    if (stop->isSvg() && reinterpret_cast<SvgElement*>(stop)->getSvgType() == SVG_ELEMENT_STOP)
    {
      SvgStopElement* _stop = reinterpret_cast<SvgStopElement*>(stop);

      if (_stop->a_offset.isAssigned() && _stop->a_style.hasStyle(SVG_STYLE_STOP_COLOR))
      {
        float offset = _stop->a_offset.getOffset();
        Color color(_stop->a_style._stopColor);

        if (_stop->a_style.hasStyle(SVG_STYLE_STOP_OPACITY))
        {
          color.setAlpha(_stop->a_style._stopOpacity);
        }

        gradient.addStop(offset, color);
        stopsParsed = true;
      }
    }
  }

  if (!stopsParsed)
  {
    XmlElement* e;
    StringW link = root->_getAttribute(FOG_STR_(SVG_ATTRIBUTE_xlink_href));

    if ((!link.isEmpty() && link.getAt(0) == CharW('#')) &&
        (e = root->getDocument()->getElementById(StubW(link.getData() + 1, link.getLength() - 1))))
    {
      root = e;
      if (++depth == 32) return;
      goto _Start;
    }
  }
}

// ============================================================================
// [Fog::SvgLinearGradientElement]
// ============================================================================

SvgLinearGradientElement::SvgLinearGradientElement() :
  SvgAbstractGradientElement(FOG_STR_(SVG_ELEMENT_linearGradient), SVG_ELEMENT_LINEAR_GRADIENT),
  a_x1(NULL, FOG_STR_(SVG_ATTRIBUTE_x1), FOG_OFFSET_OF(SvgLinearGradientElement, a_x1)),
  a_y1(NULL, FOG_STR_(SVG_ATTRIBUTE_y1), FOG_OFFSET_OF(SvgLinearGradientElement, a_y1)),
  a_x2(NULL, FOG_STR_(SVG_ATTRIBUTE_x2), FOG_OFFSET_OF(SvgLinearGradientElement, a_x2)),
  a_y2(NULL, FOG_STR_(SVG_ATTRIBUTE_y2), FOG_OFFSET_OF(SvgLinearGradientElement, a_y2))
{
}

SvgLinearGradientElement::~SvgLinearGradientElement()
{
  _removeAttributes();
}

XmlAttribute* SvgLinearGradientElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_x1)) return (XmlAttribute*)&a_x1;
  if (name == FOG_STR_(SVG_ATTRIBUTE_y1)) return (XmlAttribute*)&a_y1;
  if (name == FOG_STR_(SVG_ATTRIBUTE_x2)) return (XmlAttribute*)&a_x2;
  if (name == FOG_STR_(SVG_ATTRIBUTE_y2)) return (XmlAttribute*)&a_y2;

  return base::_createAttribute(name);
}

err_t SvgLinearGradientElement::onPattern(SvgVisitor* visitor, SvgElement* obj, uint32_t paintType) const
{
  LinearGradientF gradient;
  TransformF tr;

  // Set spread method.
  if (a_spreadMethod.isAssigned()) gradient.setGradientSpread(a_spreadMethod.getEnumValue());

  // Setup start and end points.
  if (!a_gradientUnits.isAssigned() || a_gradientUnits.getEnumValue() == SVG_OBJECT_BOUNDING_BOX)
  {
    // "objectBoundingBox".
    BoxF bbox;
    obj->getBoundingBox(bbox);

    float x1 = a_x1.isAssigned() ? a_x1.getCoordValue() : 0.0f;
    float y1 = a_y1.isAssigned() ? a_y1.getCoordValue() : 0.0f;
    float x2 = a_x2.isAssigned() ? a_x2.getCoordValue() : 1.0f;
    float y2 = a_y2.isAssigned() ? a_y2.getCoordValue() : 0.0f;

    tr._type = TRANSFORM_TYPE_SCALING;
    tr._00 = bbox.getWidth();
    tr._11 = bbox.getHeight();
    tr._20 = bbox.getX();
    tr._21 = bbox.getY();

    gradient.setStart(PointF(x1, y1));
    gradient.setEnd(PointF(x2, y2));
  }
  else
  {
    // "userSpaceOnUse".
    float x1 = a_x1.isAssigned() ? a_x1.getCoordComputed() : 0.0f;
    float y1 = a_y1.isAssigned() ? a_y1.getCoordComputed() : 0.0f;
    float x2 = a_x2.isAssigned() ? a_x2.getCoordComputed() : 1.0f;
    float y2 = a_y2.isAssigned() ? a_y2.getCoordComputed() : 0.0f;

    // TODO: Percentages to the current view-port.
    // if (!a_x1.isAssigned() || a_x1.getCoordUnit() == COORD_UNIT_PERCENT)
    // if (!a_y1.isAssigned() || a_y1.getCoordUnit() == COORD_UNIT_PERCENT)
    // if (!a_x2.isAssigned() || a_x2.getCoordUnit() == COORD_UNIT_PERCENT)
    // if (!a_y2.isAssigned() || a_y2.getCoordUnit() == COORD_UNIT_PERCENT)

    gradient.setStart(PointF(x1, y1));
    gradient.setEnd(PointF(x2, y2));
  }

  // Add color stops.
  _walkAndAddColorStops(const_cast<SvgLinearGradientElement*>(this), gradient);

  if (a_gradientTransform.isAssigned())
    tr.transform(a_gradientTransform.getTransform(), MATRIX_ORDER_APPEND);

  // Create Pattern instance.
  Pattern pattern(gradient, tr);

  if (paintType == SVG_PAINT_FILL)
    visitor->setFillPattern(pattern);
  else
    visitor->setStrokePattern(pattern);

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgRadialGradientElement]
// ============================================================================

SvgRadialGradientElement::SvgRadialGradientElement() :
  SvgAbstractGradientElement(FOG_STR_(SVG_ELEMENT_radialGradient), SVG_ELEMENT_RADIAL_GRADIENT),
  a_cx(NULL, FOG_STR_(SVG_ATTRIBUTE_cx), FOG_OFFSET_OF(SvgRadialGradientElement, a_cx)),
  a_cy(NULL, FOG_STR_(SVG_ATTRIBUTE_cy), FOG_OFFSET_OF(SvgRadialGradientElement, a_cy)),
  a_fx(NULL, FOG_STR_(SVG_ATTRIBUTE_fx), FOG_OFFSET_OF(SvgRadialGradientElement, a_fx)),
  a_fy(NULL, FOG_STR_(SVG_ATTRIBUTE_fy), FOG_OFFSET_OF(SvgRadialGradientElement, a_fx)),
  a_r (NULL, FOG_STR_(SVG_ATTRIBUTE_r ), FOG_OFFSET_OF(SvgRadialGradientElement, a_r ))
{
}

SvgRadialGradientElement::~SvgRadialGradientElement()
{
  _removeAttributes();
}

XmlAttribute* SvgRadialGradientElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_cx)) return (XmlAttribute*)&a_cx;
  if (name == FOG_STR_(SVG_ATTRIBUTE_cy)) return (XmlAttribute*)&a_cy;
  if (name == FOG_STR_(SVG_ATTRIBUTE_fx)) return (XmlAttribute*)&a_fx;
  if (name == FOG_STR_(SVG_ATTRIBUTE_fy)) return (XmlAttribute*)&a_fy;
  if (name == FOG_STR_(SVG_ATTRIBUTE_r )) return (XmlAttribute*)&a_r;

  return base::_createAttribute(name);
}

err_t SvgRadialGradientElement::onPattern(SvgVisitor* visitor, SvgElement* obj, uint32_t paintType) const
{
  // SVG TODO: Radial Gradient.

  RadialGradientF gradient;

  // Set spread method.
  if (a_spreadMethod.isAssigned()) gradient.setGradientSpread(a_spreadMethod.getEnumValue());

  // Setup start and end points.
  if (!a_gradientUnits.isAssigned() || a_gradientUnits.getEnumValue() == SVG_OBJECT_BOUNDING_BOX)
  {
    BoxF bbox;
    obj->getBoundingBox(bbox);

    float bw = bbox.getWidth();
    float bh = bbox.getHeight();

    float cx = a_cx.isAssigned() ? a_cx.getCoordValue() : 0.5f;
    float cy = a_cy.isAssigned() ? a_cy.getCoordValue() : 0.5f;
    float fx = a_fx.isAssigned() ? a_fx.getCoordValue() : cx;
    float fy = a_fy.isAssigned() ? a_fy.getCoordValue() : cy;
    float rx = a_r.isAssigned() ? a_r.getCoordValue() : 0.5f;
    float ry = rx;

    cx = bbox.x0 + bw * cx;
    cy = bbox.y0 + bh * cy;
    fx = bbox.x0 + bw * fx;
    fy = bbox.y0 + bh * fy;
    rx = bw * rx;
    ry = bh * ry;

    gradient.setCenter(PointF(cx, cy));
    gradient.setFocal(PointF(fx, fy));
    gradient.setRadius(PointF(rx, ry));
  }
  else
  {
    float cx = a_cx.isAssigned() ? a_cx.getCoordComputed() : 0.5f;
    float cy = a_cy.isAssigned() ? a_cy.getCoordComputed() : 0.5f;
    float fx = a_fx.isAssigned() ? a_fx.getCoordComputed() : cx;
    float fy = a_fy.isAssigned() ? a_fy.getCoordComputed() : cy;
    float rx = a_r.isAssigned() ? a_r.getCoordComputed() : 0.5f;
    float ry = rx;

    // TODO: Percentages to the current view-port.

    gradient.setCenter(PointF(cx, cy));
    gradient.setFocal(PointF(fx, fy));
    gradient.setRadius(PointF(rx, ry));
  }

  // Add color stops.
  _walkAndAddColorStops(const_cast<SvgRadialGradientElement*>(this), gradient);

  // Create Pattern instance.
  Pattern pattern(gradient, a_gradientTransform.isAssigned()
    ? a_gradientTransform.getTransform()
    : TransformF::identity()
  );

  if (paintType == SVG_PAINT_FILL)
    visitor->setFillPattern(pattern);
  else
    visitor->setStrokePattern(pattern);

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgPatternElement]
// ============================================================================

SvgPatternElement::SvgPatternElement() :
  SvgElement(FOG_STR_(SVG_ELEMENT_pattern), SVG_ELEMENT_PATTERN),
  a_x               (NULL, FOG_STR_(SVG_ATTRIBUTE_x               ), FOG_OFFSET_OF(SvgPatternElement, a_x               )),
  a_y               (NULL, FOG_STR_(SVG_ATTRIBUTE_y               ), FOG_OFFSET_OF(SvgPatternElement, a_y               )),
  a_width           (NULL, FOG_STR_(SVG_ATTRIBUTE_width           ), FOG_OFFSET_OF(SvgPatternElement, a_width           )),
  a_height          (NULL, FOG_STR_(SVG_ATTRIBUTE_height          ), FOG_OFFSET_OF(SvgPatternElement, a_height          )),
  a_patternUnits    (NULL, FOG_STR_(SVG_ATTRIBUTE_patternUnits    ), FOG_OFFSET_OF(SvgPatternElement, a_patternUnits    ), svgEnum_gradientUnits),
  a_patternTransform(NULL, FOG_STR_(SVG_ATTRIBUTE_patternTransform), FOG_OFFSET_OF(SvgPatternElement, a_patternTransform)),
  a_viewBox         (NULL, FOG_STR_(SVG_ATTRIBUTE_viewBox         ), FOG_OFFSET_OF(SvgPatternElement, a_viewBox         ))
{
}

SvgPatternElement::~SvgPatternElement()
{
  _removeAttributes();
}

XmlAttribute* SvgPatternElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_x               )) return (XmlAttribute*)&a_x;
  if (name == FOG_STR_(SVG_ATTRIBUTE_y               )) return (XmlAttribute*)&a_y;
  if (name == FOG_STR_(SVG_ATTRIBUTE_width           )) return (XmlAttribute*)&a_width;
  if (name == FOG_STR_(SVG_ATTRIBUTE_height          )) return (XmlAttribute*)&a_height;
  if (name == FOG_STR_(SVG_ATTRIBUTE_patternUnits    )) return (XmlAttribute*)&a_patternUnits;
  if (name == FOG_STR_(SVG_ATTRIBUTE_patternTransform)) return (XmlAttribute*)&a_patternTransform;
  if (name == FOG_STR_(SVG_ATTRIBUTE_viewBox         )) return (XmlAttribute*)&a_viewBox;

  return base::_createAttribute(name);
}

err_t SvgPatternElement::onProcess(SvgVisitor* visitor) const
{
  return ERR_OK;
}

err_t SvgPatternElement::onPattern(SvgVisitor* visitor, SvgElement* obj, uint32_t paintType) const
{
  Pattern pattern;
  FOG_RETURN_ON_ERROR(_createPattern(pattern, obj));

  if (paintType == SVG_PAINT_FILL)
    visitor->setFillPattern(pattern);
  else
    visitor->setStrokePattern(pattern);
  return ERR_OK;
}

err_t SvgPatternElement::_createPattern(Pattern& pattern, SvgElement* obj) const
{
  StringW link = _getAttribute(FOG_STR_(SVG_ATTRIBUTE_xlink_href));

  if (!link.isEmpty() && link.getAt(0) == CharW('#'))
  {
    XmlElement* e = getDocument()->getElementById(StubW(link.getData() + 1, link.getLength() - 1));
    if (e != NULL && e->isSvg() && reinterpret_cast<SvgElement*>(e)->getSvgType() == SVG_ELEMENT_PATTERN)
    {
      SvgPatternElement* pe = reinterpret_cast<SvgPatternElement*>(e);
      FOG_RETURN_ON_ERROR(pe->_createPattern(pattern, obj));
      goto _AssignTransform;
    }
  }

  // TODO: Object bounding box support.
  {
    int w = a_width.isAssigned()  ? (int)a_width.getCoordValue()  : 0;
    int h = a_height.isAssigned() ? (int)a_height.getCoordValue() : 0;
    if (w == 0 || h == 0) return ERR_IMAGE_INVALID_SIZE;

    Image image;
    FOG_RETURN_ON_ERROR(image.create(SizeI(w, h), IMAGE_FORMAT_PRGB32));

    Painter painter(image);
    painter.setSource(Argb32(0x00000000));
    painter.setCompositingOperator(COMPOSITE_SRC);
    painter.fillAll();

    if (a_viewBox.isAssigned() && a_viewBox.isValid())
    {
      const BoxF& box = a_viewBox.getBox();
      float bbw = box.getWidth();
      float bbh = box.getHeight();

      if (bbw > MATH_EPSILON_F && bbh > MATH_EPSILON_F)
      {
        TransformF tr(
          float(w) / box.getWidth(), 0.0f,
          0.0f, float(h) / box.getHeight(),
          -box.x0, -box.y0);
        painter.setTransform(tr);
      }
    }

    SvgRender render(&painter);
    _visitContainer(&render);
    painter.end();

    float tx = a_x.isAssigned() ? a_x.getCoordValue() : 0.0f;
    float ty = a_y.isAssigned() ? a_y.getCoordValue() : 0.0f;

    pattern.createTexture(Texture(image, TEXTURE_TILE_REPEAT));
    pattern.translate(PointF(tx, ty));
  }

_AssignTransform:
  if (a_patternTransform.isAssigned() && a_patternTransform.isValid())
    pattern.transform(a_patternTransform.getTransform());

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgStopElement]
// ============================================================================

SvgStopElement::SvgStopElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_stop), SVG_ELEMENT_STOP),
  a_offset(NULL, FOG_STR_(SVG_ATTRIBUTE_offset), FOG_OFFSET_OF(SvgStopElement, a_offset))
{
}

SvgStopElement::~SvgStopElement()
{
  _removeAttributes();
}

XmlAttribute* SvgStopElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_offset)) return (XmlAttribute*)&a_offset;

  return base::_createAttribute(name);
}

err_t SvgStopElement::onProcess(SvgVisitor* visitor) const
{
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgDefsElement]
// ============================================================================

SvgDefsElement::SvgDefsElement() :
  SvgElement(FOG_STR_(SVG_ELEMENT_defs), SVG_ELEMENT_DEFS)
{
}

SvgDefsElement::~SvgDefsElement()
{
  _removeAttributes();
}

err_t SvgDefsElement::onProcess(SvgVisitor* visitor) const
{
  // <defs> section is used only to define shared resources. Don't go inside.
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgGElement]
// ============================================================================

SvgGElement::SvgGElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_g), SVG_ELEMENT_G)
{
}

SvgGElement::~SvgGElement()
{
  _removeAttributes();
}

err_t SvgGElement::onProcess(SvgVisitor* visitor) const
{
  if (!hasChildNodes()) return ERR_OK;
  return _visitContainer(visitor);
}

err_t SvgGElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  if (!hasChildNodes())
  {
    box.reset();
    return ERR_GEOMETRY_NONE;
  }

  // TODO: It seems that there is nothing to do, because all render specific
  // options are related to a single SVG element, not to a group.
  // return _walkAndMergeBBox(this);

  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::SvgSymbolElement]
// ============================================================================

SvgSymbolElement::SvgSymbolElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_symbol), SVG_ELEMENT_SYMBOL)
{
}

SvgSymbolElement::~SvgSymbolElement()
{
  _removeAttributes();
}

err_t SvgSymbolElement::onProcess(SvgVisitor* visitor) const
{
  if (!hasChildNodes()) return ERR_OK;
  return _visitContainer(visitor);
}

// ============================================================================
// [Fog::SvgUseElement]
// ============================================================================

SvgUseElement::SvgUseElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_use), SVG_ELEMENT_USE),
  a_x(NULL, FOG_STR_(SVG_ATTRIBUTE_x), FOG_OFFSET_OF(SvgUseElement, a_x)),
  a_y(NULL, FOG_STR_(SVG_ATTRIBUTE_y), FOG_OFFSET_OF(SvgUseElement, a_y))
{
}

SvgUseElement::~SvgUseElement()
{
  _removeAttributes();
}

XmlAttribute* SvgUseElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_x)) return (XmlAttribute*)&a_x;
  if (name == FOG_STR_(SVG_ATTRIBUTE_y)) return (XmlAttribute*)&a_y;

  return base::_createAttribute(name);
}

err_t SvgUseElement::onPrepare(SvgVisitor* visitor, SvgGState* state) const
{
  float tx = 0.0f;
  float ty = 0.0f;

  if (a_x.isAssigned()) tx = a_x.getCoordComputed();
  if (a_y.isAssigned()) ty = a_y.getCoordComputed();

  if (tx != 0.0f || ty != 0.0f)
  {
    if (state && !state->hasState(SvgGState::SAVED_TRANSFORM)) state->saveTransform();
    visitor->translate(PointF(tx, ty));
  }

  return ERR_OK;
}

err_t SvgUseElement::onProcess(SvgVisitor* visitor) const
{
  err_t err = ERR_OK;

  StringW link = _getAttribute(FOG_STR_(SVG_ATTRIBUTE_xlink_href));
  XmlElement* ref = getDocument()->getElementById(parseHtmlLinkId(link));

  if (ref && ref->isSvgElement())
    err = visitor->onVisit(reinterpret_cast<SvgElement*>(ref));

  return err;
}

err_t SvgUseElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  box.reset();
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgViewElement]
// ============================================================================

SvgViewElement::SvgViewElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_svg), SVG_ELEMENT_VIEW),
  a_viewBox(NULL, FOG_STR_(SVG_ATTRIBUTE_viewBox), FOG_OFFSET_OF(SvgViewElement, a_viewBox))
{
}

SvgViewElement::~SvgViewElement()
{
  _removeAttributes();
}

XmlAttribute* SvgViewElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_viewBox)) return (XmlAttribute*)&a_viewBox;

  return base::_createAttribute(name);
}

err_t SvgViewElement::onProcess(SvgVisitor* visitor) const
{
  if (!hasChildNodes()) return ERR_OK;
  return _visitContainer(visitor);
}

err_t SvgViewElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::SvgAElement]
// ============================================================================

SvgAElement::SvgAElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_a), SVG_ELEMENT_A)
{
}

SvgAElement::~SvgAElement()
{
  _removeAttributes();
}

XmlAttribute* SvgAElement::_createAttribute(const ManagedStringW& name) const
{
  return base::_createAttribute(name);
}

err_t SvgAElement::onProcess(SvgVisitor* visitor) const
{
  return _visitContainer(visitor);
}

// ============================================================================
// [Fog::SvgCircleElement]
// ============================================================================

SvgCircleElement::SvgCircleElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_circle), SVG_ELEMENT_CIRCLE),
  a_cx(NULL, FOG_STR_(SVG_ATTRIBUTE_cx), FOG_OFFSET_OF(SvgCircleElement, a_cx)),
  a_cy(NULL, FOG_STR_(SVG_ATTRIBUTE_cy), FOG_OFFSET_OF(SvgCircleElement, a_cy)),
  a_r (NULL, FOG_STR_(SVG_ATTRIBUTE_r ), FOG_OFFSET_OF(SvgCircleElement, a_r ))
{
}

SvgCircleElement::~SvgCircleElement()
{
  _removeAttributes();
}

XmlAttribute* SvgCircleElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_cx)) return (XmlAttribute*)&a_cx;
  if (name == FOG_STR_(SVG_ATTRIBUTE_cy)) return (XmlAttribute*)&a_cy;
  if (name == FOG_STR_(SVG_ATTRIBUTE_r )) return (XmlAttribute*)&a_r;

  return base::_createAttribute(name);
}

err_t SvgCircleElement::onProcess(SvgVisitor* visitor) const
{
  if (a_r.isAssigned())
  {
    float cx = a_cx.isAssigned() ? a_cx.getCoordComputed() : 0.0f;
    float cy = a_cy.isAssigned() ? a_cy.getCoordComputed() : 0.0f;
    float r = Math::abs(a_r.getCoordComputed());

    if (r <= 0.0f) return ERR_OK;

    CircleF circle(cx, cy, r);
    return visitor->onShape((SvgElement*)this, ShapeF(&circle));
  }

  return ERR_OK;
}

err_t SvgCircleElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  if (a_r.isAssigned())
  {
    float cx = a_cx.isAssigned() ? a_cx.getCoordComputed() : 0.0f;
    float cy = a_cy.isAssigned() ? a_cy.getCoordComputed() : 0.0f;
    float r = Math::abs(a_r.getCoordComputed());

    if (r <= 0.0f) goto _Fail;
    return CircleF(PointF(cx, cy), r)._getBoundingBox(box, tr);
  }

_Fail:
  box.reset();
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgEllipseElement]
// ============================================================================

SvgEllipseElement::SvgEllipseElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_ellipse), SVG_ELEMENT_ELLIPSE),
  a_cx(NULL, FOG_STR_(SVG_ATTRIBUTE_cx), FOG_OFFSET_OF(SvgEllipseElement, a_cx)),
  a_cy(NULL, FOG_STR_(SVG_ATTRIBUTE_cy), FOG_OFFSET_OF(SvgEllipseElement, a_cy)),
  a_rx(NULL, FOG_STR_(SVG_ATTRIBUTE_rx), FOG_OFFSET_OF(SvgEllipseElement, a_rx)),
  a_ry(NULL, FOG_STR_(SVG_ATTRIBUTE_ry), FOG_OFFSET_OF(SvgEllipseElement, a_ry))
{
}

SvgEllipseElement::~SvgEllipseElement()
{
  _removeAttributes();
}

XmlAttribute* SvgEllipseElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_cx)) return (XmlAttribute*)&a_cx;
  if (name == FOG_STR_(SVG_ATTRIBUTE_cy)) return (XmlAttribute*)&a_cy;
  if (name == FOG_STR_(SVG_ATTRIBUTE_rx)) return (XmlAttribute*)&a_rx;
  if (name == FOG_STR_(SVG_ATTRIBUTE_ry)) return (XmlAttribute*)&a_ry;

  return base::_createAttribute(name);
}

err_t SvgEllipseElement::onProcess(SvgVisitor* visitor) const
{
  if (a_rx.isAssigned() && a_ry.isAssigned())
  {
    float cx = a_cx.isAssigned() ? a_cx.getCoordComputed() : 0.0f;
    float cy = a_cy.isAssigned() ? a_cy.getCoordComputed() : 0.0f;

    float rx = Math::abs(a_rx.getCoordComputed());
    float ry = Math::abs(a_ry.getCoordComputed());

    if (rx <= 0.0f || ry <= 0.0f) return ERR_OK;

    EllipseF ellipse(cx, cy, rx, ry);
    return visitor->onShape((SvgElement*)this, ShapeF(&ellipse));
  }

  return ERR_OK;
}

err_t SvgEllipseElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  if (a_rx.isAssigned() && a_ry.isAssigned())
  {
    float cx = a_cx.isAssigned() ? a_cx.getCoordComputed() : 0.0f;
    float cy = a_cy.isAssigned() ? a_cy.getCoordComputed() : 0.0f;

    float rx = a_rx.getCoordComputed();
    float ry = a_ry.getCoordComputed();

    if (rx <= 0.0f || ry <= 0.0f) goto _Fail;
    return EllipseF(PointF(cx, cy), PointF(rx, ry))._getBoundingBox(box, tr);
  }

_Fail:
  box.reset();
  return ERR_GEOMETRY_INVALID;
}

// ============================================================================
// [Fog::SvgImageElement]
// ============================================================================

SvgImageElement::SvgImageElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_image), SVG_ELEMENT_IMAGE),
  a_x      (NULL, FOG_STR_(SVG_ATTRIBUTE_x         ), FOG_OFFSET_OF(SvgImageElement, a_x     )),
  a_y      (NULL, FOG_STR_(SVG_ATTRIBUTE_y         ), FOG_OFFSET_OF(SvgImageElement, a_y     )),
  a_width  (NULL, FOG_STR_(SVG_ATTRIBUTE_width     ), FOG_OFFSET_OF(SvgImageElement, a_width )),
  a_height (NULL, FOG_STR_(SVG_ATTRIBUTE_height    ), FOG_OFFSET_OF(SvgImageElement, a_height)),
  a_href   (NULL, FOG_STR_(SVG_ATTRIBUTE_xlink_href), FOG_OFFSET_OF(SvgImageElement, a_href  ))
{
}

SvgImageElement::~SvgImageElement()
{
  _removeAttributes();
}

XmlAttribute* SvgImageElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_x         )) return (XmlAttribute*)&a_x;
  if (name == FOG_STR_(SVG_ATTRIBUTE_y         )) return (XmlAttribute*)&a_y;
  if (name == FOG_STR_(SVG_ATTRIBUTE_width     )) return (XmlAttribute*)&a_width;
  if (name == FOG_STR_(SVG_ATTRIBUTE_height    )) return (XmlAttribute*)&a_height;
  if (name == FOG_STR_(SVG_ATTRIBUTE_xlink_href)) return (XmlAttribute*)&a_href;

  return base::_createAttribute(name);
}

err_t SvgImageElement::onPrepare(SvgVisitor* visitor, SvgGState* state) const
{
  // There is only transformation attribute which can be applied to the image.
  if (a_transform.isAssigned() && a_transform.isValid())
  {
    if (state && !state->hasState(SvgGState::SAVED_TRANSFORM))
      state->saveTransform();
    visitor->transform(a_transform.getTransform());
  }

  return ERR_OK;
}

err_t SvgImageElement::onProcess(SvgVisitor* visitor) const
{
  if (!a_href._embedded || a_href._image.isEmpty()) return ERR_OK;

  float x = a_x.isAssigned() ? a_x.getCoordComputed() : 0.0f;
  float y = a_y.isAssigned() ? a_y.getCoordComputed() : 0.0f;

  return visitor->onImage((SvgElement*)this, PointF(x, y), a_href._image);
}

// ============================================================================
// [Fog::SvgLineElement]
// ============================================================================

SvgLineElement::SvgLineElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_line), SVG_ELEMENT_LINE),
  a_x1(NULL, FOG_STR_(SVG_ATTRIBUTE_x1), FOG_OFFSET_OF(SvgLineElement, a_x1)),
  a_y1(NULL, FOG_STR_(SVG_ATTRIBUTE_y1), FOG_OFFSET_OF(SvgLineElement, a_y1)),
  a_x2(NULL, FOG_STR_(SVG_ATTRIBUTE_x2), FOG_OFFSET_OF(SvgLineElement, a_x2)),
  a_y2(NULL, FOG_STR_(SVG_ATTRIBUTE_y2), FOG_OFFSET_OF(SvgLineElement, a_y2))
{
}

SvgLineElement::~SvgLineElement()
{
  _removeAttributes();
}

XmlAttribute* SvgLineElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_x1)) return (XmlAttribute*)&a_x1;
  if (name == FOG_STR_(SVG_ATTRIBUTE_y1)) return (XmlAttribute*)&a_y1;
  if (name == FOG_STR_(SVG_ATTRIBUTE_x2)) return (XmlAttribute*)&a_x2;
  if (name == FOG_STR_(SVG_ATTRIBUTE_y2)) return (XmlAttribute*)&a_y2;

  return base::_createAttribute(name);
}

err_t SvgLineElement::onProcess(SvgVisitor* visitor) const
{
  float x1 = a_x1.isAssigned() ? a_x1.getCoordComputed() : 0.0f;
  float y1 = a_y1.isAssigned() ? a_y1.getCoordComputed() : 0.0f;
  float x2 = a_x2.isAssigned() ? a_x2.getCoordComputed() : 0.0f;
  float y2 = a_y2.isAssigned() ? a_y2.getCoordComputed() : 0.0f;

  LineF line(x1, y1, x2, y2);
  return visitor->onShape((SvgElement*)this, ShapeF(&line));
}

err_t SvgLineElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  float x1 = a_x1.isAssigned() ? a_x1.getCoordComputed() : 0.0f;
  float y1 = a_y1.isAssigned() ? a_y1.getCoordComputed() : 0.0f;
  float x2 = a_x2.isAssigned() ? a_x2.getCoordComputed() : 0.0f;
  float y2 = a_y2.isAssigned() ? a_y2.getCoordComputed() : 0.0f;

  box.setBox(x1, y1, x2, y2);
  if (tr) tr->mapPoints(reinterpret_cast<PointF*>(&box), reinterpret_cast<PointF*>(&box), 2);

  if (box.x0 > box.x1) swap(box.x0, box.x1);
  if (box.y0 > box.y1) swap(box.y0, box.y1);
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgPathElement]
// ============================================================================

SvgPathElement::SvgPathElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_path), SVG_ELEMENT_PATH),
  a_d(NULL, FOG_STR_(SVG_ATTRIBUTE_d), FOG_OFFSET_OF(SvgPathElement, a_d))
{
}

SvgPathElement::~SvgPathElement()
{
  _removeAttributes();
}

XmlAttribute* SvgPathElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_d)) return (XmlAttribute*)&a_d;

  return base::_createAttribute(name);
}

err_t SvgPathElement::onProcess(SvgVisitor* visitor) const
{
  /*if (_unused)
  {
    printf("A");
  }*/

  if (!a_d.isAssigned()) return ERR_OK;

  const PathF& path = a_d.getPath();
  return visitor->onShape((SvgElement*)this, ShapeF(&path));
}

err_t SvgPathElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  if (a_d.isAssigned())
  {
    const PathF& path = a_d.getPath();
    return path._getBoundingBox(box, tr);
  }
  else
  {
    box.reset();
    return ERR_OK;
  }
}

// ============================================================================
// [Fog::SvgPolygonElement]
// ============================================================================

SvgPolygonElement::SvgPolygonElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_polygon), SVG_ELEMENT_POLYGON),
  a_points(NULL, FOG_STR_(SVG_ATTRIBUTE_points), true, FOG_OFFSET_OF(SvgPolygonElement, a_points))
{
}

SvgPolygonElement::~SvgPolygonElement()
{
  _removeAttributes();
}

XmlAttribute* SvgPolygonElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_points)) return (XmlAttribute*)&a_points;

  return base::_createAttribute(name);
}

err_t SvgPolygonElement::onProcess(SvgVisitor* visitor) const
{
  if (!a_points.isAssigned()) return ERR_OK;

  const PathF& path = a_points.getPath();
  return visitor->onShape((SvgElement*)this, ShapeF(&path));
}

err_t SvgPolygonElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  if (a_points.isAssigned())
  {
    const PathF& path = a_points.getPath();
    return path._getBoundingBox(box, tr);
  }
  else
  {
    box.reset();
    return ERR_OK;
  }
}

// ============================================================================
// [Fog::SvgPolylineElement]
// ============================================================================

SvgPolylineElement::SvgPolylineElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_polyline), SVG_ELEMENT_POLYLINE),
  a_points(NULL, FOG_STR_(SVG_ATTRIBUTE_points), false, FOG_OFFSET_OF(SvgPolylineElement, a_points))
{
}

SvgPolylineElement::~SvgPolylineElement()
{
  _removeAttributes();
}

XmlAttribute* SvgPolylineElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_points)) return (XmlAttribute*)&a_points;

  return base::_createAttribute(name);
}

err_t SvgPolylineElement::onProcess(SvgVisitor* visitor) const
{
  if (!a_points.isAssigned()) return ERR_OK;

  const PathF& path = a_points.getPath();
  return visitor->onShape((SvgElement*)this, ShapeF(&path));
}

err_t SvgPolylineElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  if (a_points.isAssigned())
  {
    const PathF& path = a_points.getPath();
    return path._getBoundingBox(box, tr);
  }
  else
  {
    box.reset();
    return ERR_OK;
  }
}

// ============================================================================
// [Fog::SvgRectElement]
// ============================================================================

SvgRectElement::SvgRectElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_rect), SVG_ELEMENT_RECT),
  a_x     (NULL, FOG_STR_(SVG_ATTRIBUTE_x     ), FOG_OFFSET_OF(SvgRectElement, a_x     )),
  a_y     (NULL, FOG_STR_(SVG_ATTRIBUTE_y     ), FOG_OFFSET_OF(SvgRectElement, a_y     )),
  a_width (NULL, FOG_STR_(SVG_ATTRIBUTE_width ), FOG_OFFSET_OF(SvgRectElement, a_width )),
  a_height(NULL, FOG_STR_(SVG_ATTRIBUTE_height), FOG_OFFSET_OF(SvgRectElement, a_height)),
  a_rx    (NULL, FOG_STR_(SVG_ATTRIBUTE_rx    ), FOG_OFFSET_OF(SvgRectElement, a_rx    )),
  a_ry    (NULL, FOG_STR_(SVG_ATTRIBUTE_ry    ), FOG_OFFSET_OF(SvgRectElement, a_ry    ))
{
}

SvgRectElement::~SvgRectElement()
{
  _removeAttributes();
}

XmlAttribute* SvgRectElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_x     )) return (XmlAttribute*)&a_x;
  if (name == FOG_STR_(SVG_ATTRIBUTE_y     )) return (XmlAttribute*)&a_y;
  if (name == FOG_STR_(SVG_ATTRIBUTE_width )) return (XmlAttribute*)&a_width;
  if (name == FOG_STR_(SVG_ATTRIBUTE_height)) return (XmlAttribute*)&a_height;
  if (name == FOG_STR_(SVG_ATTRIBUTE_rx    )) return (XmlAttribute*)&a_rx;
  if (name == FOG_STR_(SVG_ATTRIBUTE_ry    )) return (XmlAttribute*)&a_ry;

  return base::_createAttribute(name);
}

err_t SvgRectElement::onProcess(SvgVisitor* visitor) const
{
  if (a_width.isAssigned() && a_height.isAssigned())
  {
    float rw = a_width.getCoordComputed();
    float rh = a_height.getCoordComputed();
    if (rw <= 0.0f || rh <= 0.0f) return ERR_GEOMETRY_INVALID;

    float rx = a_x.isAssigned() ? a_x.getCoordComputed() : 0.0f;
    float ry = a_y.isAssigned() ? a_y.getCoordComputed() : 0.0f;

    float radx = a_rx.isAssigned() ? a_rx.getCoordComputed() : 0.0f;
    float rady = a_ry.isAssigned() ? a_ry.getCoordComputed() : 0.0f;

    if (!a_rx.isAssigned() && a_ry.isAssigned()) radx = rady;
    if (!a_ry.isAssigned() && a_rx.isAssigned()) rady = radx;

    if (radx <= float(0.0) || rady <= float(0.0))
    {
      RectF rect(rx, ry, rw, rh);
      return visitor->onShape((SvgElement*)this, ShapeF(&rect));
    }
    else
    {
      RoundF round(rx, ry, rw, rh, radx, rady);
      return visitor->onShape((SvgElement*)this, ShapeF(&round));
    }
  }

  return ERR_OK;
}

err_t SvgRectElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  if (a_width.isAssigned() && a_height.isAssigned())
  {
    float rw = a_width.getCoordComputed();
    float rh = a_height.getCoordComputed();
    if (rw < 0.0f || rh < 0.0f) goto _Fail;

    float rx = a_x.isAssigned() ? a_x.getCoordComputed() : 0.0f;
    float ry = a_y.isAssigned() ? a_y.getCoordComputed() : 0.0f;

    box.setRect(rx, ry, rw, rh);
    if (tr) tr->mapBox(box, box);

    return ERR_OK;
  }

_Fail:
  box.reset();
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgTextElement]
// ============================================================================

SvgTextElement::SvgTextElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_text), SVG_ELEMENT_TEXT),
  a_x           (NULL, FOG_STR_(SVG_ATTRIBUTE_x         ), FOG_OFFSET_OF(SvgTextElement, a_x           )),
  a_y           (NULL, FOG_STR_(SVG_ATTRIBUTE_y         ), FOG_OFFSET_OF(SvgTextElement, a_y           )),
  a_dx          (NULL, FOG_STR_(SVG_ATTRIBUTE_dx        ), FOG_OFFSET_OF(SvgTextElement, a_dx          )),
  a_dy          (NULL, FOG_STR_(SVG_ATTRIBUTE_dy        ), FOG_OFFSET_OF(SvgTextElement, a_dy          )),
  a_textLength  (NULL, FOG_STR_(SVG_ATTRIBUTE_textLength), FOG_OFFSET_OF(SvgTextElement, a_textLength  ))
{
}

SvgTextElement::~SvgTextElement()
{
  _removeAttributes();
}

XmlAttribute* SvgTextElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_x           )) return (XmlAttribute*)&a_x;
  if (name == FOG_STR_(SVG_ATTRIBUTE_y           )) return (XmlAttribute*)&a_y;
  if (name == FOG_STR_(SVG_ATTRIBUTE_dx          )) return (XmlAttribute*)&a_dx;
  if (name == FOG_STR_(SVG_ATTRIBUTE_dy          )) return (XmlAttribute*)&a_dy;
  if (name == FOG_STR_(SVG_ATTRIBUTE_textLength  )) return (XmlAttribute*)&a_textLength;

  return base::_createAttribute(name);
}

err_t SvgTextElement::onPrepare(SvgVisitor* visitor, SvgGState* state) const
{
  base::onPrepare(visitor, state);
  if (state && !state->hasState(SvgGState::SAVED_GLOBAL)) state->saveGlobal();

  float x = a_x.isAssigned() ? a_x.getCoordComputed() : 0.0f;
  float y = a_y.isAssigned() ? a_y.getCoordComputed() : 0.0f;
  visitor->_textCursor.set(x, y);

  return ERR_OK;
}

err_t SvgTextElement::onProcess(SvgVisitor* visitor) const
{
  err_t err = ERR_OK;
  XmlElement* e;

  float x = visitor->_textCursor.x;
  float y = visitor->_textCursor.y;

  for (e = getFirstChild(); e; e = e->getNextSibling())
  {
    if (e->isSvgElement() && reinterpret_cast<SvgElement*>(e)->getVisible())
    {
      err = visitor->onVisit(reinterpret_cast<SvgElement*>(e));
      if (FOG_IS_ERROR(err)) break;
    }

    if (e->isText())
    {
      StringW text = e->getTextContent();
      text.simplify();

      // TODO: Not optimal, just initial support for text rendering.
      PathF path;
      visitor->_font.getTextOutline(path, CONTAINER_OP_APPEND, PointF(x, y), text);

      err = visitor->onShape((SvgElement*)this, ShapeF(&path));
      if (FOG_IS_ERROR(err)) break;
    }
  }

  return err;
}

err_t SvgTextElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  // TODO: SVGText bounding box.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::SvgTSpanElement]
// ============================================================================

SvgTSpanElement::SvgTSpanElement() :
  SvgStyledElement(FOG_STR_(SVG_ELEMENT_text), SVG_ELEMENT_TSPAN),
  a_x           (NULL, FOG_STR_(SVG_ATTRIBUTE_x           ), FOG_OFFSET_OF(SvgTSpanElement, a_x           )),
  a_y           (NULL, FOG_STR_(SVG_ATTRIBUTE_y           ), FOG_OFFSET_OF(SvgTSpanElement, a_y           )),
  a_dx          (NULL, FOG_STR_(SVG_ATTRIBUTE_dx          ), FOG_OFFSET_OF(SvgTSpanElement, a_dx          )),
  a_dy          (NULL, FOG_STR_(SVG_ATTRIBUTE_dy          ), FOG_OFFSET_OF(SvgTSpanElement, a_dy          )),
  a_textLength  (NULL, FOG_STR_(SVG_ATTRIBUTE_textLength  ), FOG_OFFSET_OF(SvgTSpanElement, a_textLength  )),
  a_lengthAdjust(NULL, FOG_STR_(SVG_ATTRIBUTE_lengthAdjust), FOG_OFFSET_OF(SvgTSpanElement, a_lengthAdjust), svgEnum_lengthAdjust)
{
}

SvgTSpanElement::~SvgTSpanElement()
{
  _removeAttributes();
}

XmlAttribute* SvgTSpanElement::_createAttribute(const ManagedStringW& name) const
{
  if (name == FOG_STR_(SVG_ATTRIBUTE_x         )) return (XmlAttribute*)&a_x;
  if (name == FOG_STR_(SVG_ATTRIBUTE_y         )) return (XmlAttribute*)&a_y;
  if (name == FOG_STR_(SVG_ATTRIBUTE_dx        )) return (XmlAttribute*)&a_dx;
  if (name == FOG_STR_(SVG_ATTRIBUTE_dy        )) return (XmlAttribute*)&a_dy;
  if (name == FOG_STR_(SVG_ATTRIBUTE_textLength)) return (XmlAttribute*)&a_textLength;
  if (name == FOG_STR_(SVG_ATTRIBUTE_lengthAdjust)) return (XmlAttribute*)&a_lengthAdjust;

  return base::_createAttribute(name);
}

err_t SvgTSpanElement::onPrepare(SvgVisitor* visitor, SvgGState* state) const
{
  base::onPrepare(visitor, state);
  if (state && !state->hasState(SvgGState::SAVED_GLOBAL)) state->saveGlobal();

  float x = a_x.isAssigned() ? a_x.getCoordComputed() : 0.0f;
  float y = a_y.isAssigned() ? a_y.getCoordComputed() : 0.0f;
  visitor->_textCursor.set(x, y);

  return ERR_OK;
}

err_t SvgTSpanElement::onProcess(SvgVisitor* visitor) const
{
  err_t err = ERR_OK;

  float x = visitor->_textCursor.x;
  float y = visitor->_textCursor.y;

  if (a_dx.isAssigned()) x += a_dx.getCoordComputed();
  if (a_dy.isAssigned()) y += a_dy.getCoordComputed();

  StringW text = getTextContent();
  text.simplify();

  // TODO: Not optimal, just initial support for text rendering.
  PathF path;

  err = visitor->_font.getTextOutline(path, CONTAINER_OP_APPEND, PointF(x, y), text);
  if (FOG_IS_ERROR(err)) return err;

  err = visitor->onShape((SvgElement*)this, ShapeF(&path));
  visitor->_textCursor.set(x, y);
  return err;
}

} // Fog namespace
