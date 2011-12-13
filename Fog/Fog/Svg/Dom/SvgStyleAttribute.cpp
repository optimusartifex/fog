// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Svg/Dom/SvgElement.h>
#include <Fog/Svg/Dom/SvgEnumItem_p.h>
#include <Fog/Svg/Dom/SvgStyleAttribute_p.h>
#include <Fog/Svg/Tools/SvgUtil.h>

namespace Fog {

// ============================================================================
// [Fog::SvgEnumItem - Data]
// ============================================================================

static const SvgEnumItem svgEnum_fillRule[] =
{
  { "nonzero", FILL_RULE_NON_ZERO },
  { "evenodd", FILL_RULE_EVEN_ODD },
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

} // Fog namespace
