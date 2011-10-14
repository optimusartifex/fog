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
#include <Fog/Core/Tools/Stream.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Svg/Dom/SvgElement.h>
#include <Fog/Svg/Dom/SvgImageLinkAttribute_p.h>

namespace Fog {

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

} // Fog namespace
