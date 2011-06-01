// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SOURCE_COLOR_H
#define _FOG_G2D_SOURCE_COLOR_H

// [Dependencies]
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Memory/MemoryOps.h>
#include <Fog/Core/Global/Swap.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Stub.h>
#include <Fog/G2d/Face/Face_Raster_C.h>
#include <Fog/G2d/Global/Api.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Source/Acmyk.h>
#include <Fog/G2d/Source/Ahsl.h>
#include <Fog/G2d/Source/Ahsv.h>
#include <Fog/G2d/Source/Argb.h>
#include <Fog/G2d/Source/ColorUtil.h>

namespace Fog {

//! @addtogroup Fog_G2d_Source
//! @{

// ============================================================================
// [Fog::Color]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
//! @brief Color value using ARGB, AHSV or ACMYK format.
//!
//! @c Color is universal container for storing color value. It remembers the
//! color mode used to store the color and it can convert color to any other
//! supported models on-the-fly.
//!
//! @section Converting color to grey.
//!
//! The We are using this formula:
//!  0.212671 * R + 0.715160 * G + 0.072169 * B;
//! As:
//!  (13938 * R + 46868 * G + 4730 * B) / 65536
struct FOG_NO_EXPORT Color : public ColorBase
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Color()
  {
    reset();
  }

  explicit FOG_INLINE Color(_Uninitialized)
  {
  }

  FOG_INLINE Color(const Color& color)
  {
    setColor(color);
  }

  explicit FOG_INLINE Color(const ArgbBase32& argb32) { setArgb32(argb32); }
  explicit FOG_INLINE Color(const ArgbBase64& argb64) { setArgb64(argb64); }
  explicit FOG_INLINE Color(const ArgbBaseF& argbf) { setArgbF(argbf); }
  explicit FOG_INLINE Color(const AhsvBaseF& ahsvf) { setAhsvF(ahsvf); }
  explicit FOG_INLINE Color(const AhslBaseF& ahslf) { setAhslF(ahslf); }
  explicit FOG_INLINE Color(const AcmykBaseF& acmykf) { setAcmykF(acmykf); }

  // --------------------------------------------------------------------------
  // [IsOpaque]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isOpaque() const
  {
    return _data[0] == 1.0f;
  }

  FOG_INLINE bool isOpaque_ARGB32() const
  {
    return Face::p32ARGB32IsAlphaFF(_argb32.p32);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getModel() const
  {
    return _model;
  }

  FOG_INLINE void setModel(uint32_t model)
  {
    _g2d.color.setModel(*this, model);
  }

  FOG_INLINE uint32_t getHints() const
  {
    return _hints;
  }

  FOG_INLINE float getAlpha() const
  {
    return _data[0];
  }

  FOG_INLINE void setAlpha(float af)
  {
    Face::p32 a8;
    Face::f32CvtU8FromFX(a8, af);

    _data[0] = af;
    _argb32.a = (uint8_t)a8;
  }

  FOG_INLINE void setHints(uint32_t hints)
  {
    _hints = hints;
  }

  // --------------------------------------------------------------------------
  // [Conversion]
  // --------------------------------------------------------------------------

  FOG_INLINE Argb32 getArgb32() const
  {
    return Argb32(_argb32);
  }

  FOG_INLINE Argb64 getArgb64() const
  {
    Argb64 argb64;
    _g2d.color.convert[_COLOR_MODEL_ARGB64][_model](&argb64, _data);
    return argb64;
  }

  FOG_INLINE ArgbF getArgbF() const
  {
    ArgbF argbf;
    _g2d.color.convert[COLOR_MODEL_ARGB][_model](&argbf, _data);
    return argbf;
  }

  FOG_INLINE AhsvF getAhsvF() const
  {
    AhsvF ahsvf;
    _g2d.color.convert[COLOR_MODEL_AHSV][_model](&ahsvf, _data);
    return ahsvf;
  }

  FOG_INLINE AhslF getAhslF() const
  {
    AhslF ahslf;
    _g2d.color.convert[COLOR_MODEL_AHSL][_model](&ahslf, _data);
    return ahslf;
  }

  FOG_INLINE AcmykF getAcmykF() const
  {
    AcmykF acmykf;
    _g2d.color.convert[COLOR_MODEL_ACMYK][_model](&acmykf, _data);
    return acmykf;
  }

  FOG_INLINE void setArgb32(const ArgbBase32& argb32)
  {
    _g2d.color.setData(*this, _COLOR_MODEL_ARGB32, &argb32);
  }

  FOG_INLINE void setArgb64(const ArgbBase64& argb64)
  {
    _g2d.color.setData(*this, _COLOR_MODEL_ARGB64, &argb64);
  }

  FOG_INLINE void setArgbF(const ArgbBaseF& argbf)
  {
    _g2d.color.setData(*this, COLOR_MODEL_ARGB, &argbf);
  }

  FOG_INLINE void setAhsvF(const AhsvBaseF& ahsvf)
  {
    _g2d.color.setData(*this, COLOR_MODEL_AHSV, &ahsvf);
  }

  FOG_INLINE void setAhslF(const AhslBaseF& ahsl)
  {
    _g2d.color.setData(*this, COLOR_MODEL_AHSL, &ahsl);
  }

  FOG_INLINE void setAcmykF(const AcmykBaseF& acmykf)
  {
    _g2d.color.setData(*this, COLOR_MODEL_ACMYK, &acmykf);
  }

  FOG_INLINE void setColor(const ColorBase& color)
  {
    Memory::copy_t<Color>(this, reinterpret_cast<const Color*>(&color));
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    Memory::zero_t<Color>(this);
  }

  // --------------------------------------------------------------------------
  // [Consistency]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isValid() const
  {
    FOG_ASSERT(_model < COLOR_MODEL_COUNT);
    return _model != COLOR_MODEL_NONE;
  }

  // --------------------------------------------------------------------------
  // [Mix]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t mix(uint32_t mixOp, uint32_t alphaOp, const Color& secondary, float mask)
  {
    return _g2d.color.mix(*this, mixOp, alphaOp, secondary, mask);
  }

  // --------------------------------------------------------------------------
  // [Adjust]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t adjust(uint32_t adjustOp, float param)
  {
    return _g2d.color.adjust(*this, adjustOp, param);
  }

  // --------------------------------------------------------------------------
  // [Parse / Serialize]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t parse(const Stub8& str, uint32_t flags = COLOR_NAME_ANY)
  {
    return _g2d.color.parseA(*this, str, flags);
  }

  FOG_INLINE err_t parse(const Utf16& str, uint32_t flags = COLOR_NAME_ANY)
  {
    return _g2d.color.parseU(*this, str, flags);
  }

  FOG_INLINE err_t parse(const String& str, uint32_t flags = COLOR_NAME_ANY)
  {
    return _g2d.color.parseU(*this, Utf16(str.getData(), str.getLength()), flags);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Color& operator=(const ArgbBase32& argb32) { setArgb32(argb32); return *this; }
  FOG_INLINE Color& operator=(const ArgbBase64& argb64) { setArgb64(argb64); return *this; }
  FOG_INLINE Color& operator=(const ArgbBaseF& argbf) { setArgbF(argbf); return *this; }
  FOG_INLINE Color& operator=(const AhsvBaseF& ahsvf) { setAhsvF(ahsvf); return *this; }
  FOG_INLINE Color& operator=(const AcmykBaseF& acmykf) { setAcmykF(acmykf); return *this; }
  FOG_INLINE Color& operator=(const ColorBase& color) { setColor(color); return *this; }

  FOG_INLINE bool operator==(const Color& other) const { return Memory::eq_t<Color>(this, &other); }
  FOG_INLINE bool operator!=(const Color& other) const { return !operator==(other); }
};
#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Implemented-Later]
// ============================================================================

FOG_INLINE AhsvF Argb32::getAhsvF() const
{
  AhsvF ahsvf(UNINITIALIZED);
  _g2d.color.convert[COLOR_MODEL_AHSV][_COLOR_MODEL_ARGB32](&ahsvf, this);
  return ahsvf;
}

FOG_INLINE AhslF Argb32::getAhslF() const
{
  AhslF ahslf(UNINITIALIZED);
  _g2d.color.convert[COLOR_MODEL_AHSL][_COLOR_MODEL_ARGB32](&ahslf, this);
  return ahslf;
}

FOG_INLINE AcmykF Argb32::getAcmykF() const
{
  AcmykF acmykf(UNINITIALIZED);
  _g2d.color.convert[COLOR_MODEL_ACMYK][_COLOR_MODEL_ARGB32](&acmykf, this);
  return acmykf;
}

FOG_INLINE AhsvF Argb64::getAhsvF() const
{
  AhsvF ahsvf(UNINITIALIZED);
  _g2d.color.convert[COLOR_MODEL_AHSV][_COLOR_MODEL_ARGB64](&ahsvf, this);
  return ahsvf;
}

FOG_INLINE AhslF Argb64::getAhslF() const
{
  AhslF ahslf(UNINITIALIZED);
  _g2d.color.convert[COLOR_MODEL_AHSL][_COLOR_MODEL_ARGB64](&ahslf, this);
  return ahslf;
}

FOG_INLINE AcmykF Argb64::getAcmykF() const
{
  AcmykF acmykf(UNINITIALIZED);
  _g2d.color.convert[COLOR_MODEL_ACMYK][_COLOR_MODEL_ARGB64](&acmykf, this);
  return acmykf;
}

FOG_INLINE AhsvF ArgbF::getAhsvF() const
{
  AhsvF ahsvf(UNINITIALIZED);
  _g2d.color.convert[COLOR_MODEL_AHSV][COLOR_MODEL_ARGB](&ahsvf, this);
  return ahsvf;
}

FOG_INLINE AhslF ArgbF::getAhslF() const
{
  AhslF ahslf(UNINITIALIZED);
  _g2d.color.convert[COLOR_MODEL_AHSL][COLOR_MODEL_ARGB](&ahslf, this);
  return ahslf;
}

FOG_INLINE AcmykF ArgbF::getAcmykF() const
{
  AcmykF acmykf(UNINITIALIZED);
  _g2d.color.convert[COLOR_MODEL_ACMYK][COLOR_MODEL_ARGB](&acmykf, this);
  return acmykf;
}

FOG_INLINE AhslF AhsvF::getAhslF() const
{
  AhslF ahslf(UNINITIALIZED);
  _g2d.color.convert[COLOR_MODEL_AHSL][COLOR_MODEL_AHSV](&ahslf, this);
  return ahslf;
}

FOG_INLINE AcmykF AhsvF::getAcmykF() const
{
  AcmykF acmykf(UNINITIALIZED);
  _g2d.color.convert[COLOR_MODEL_AHSL][COLOR_MODEL_AHSV](&acmykf, this);
  return acmykf;
}

FOG_INLINE AhsvF AhslF::getAhsvF() const
{
  AhsvF ahsvf(UNINITIALIZED);
  _g2d.color.convert[COLOR_MODEL_AHSV][COLOR_MODEL_AHSL](&ahsvf, this);
  return ahsvf;
}

FOG_INLINE AcmykF AhslF::getAcmykF() const
{
  AcmykF acmykf(UNINITIALIZED);
  _g2d.color.convert[COLOR_MODEL_ACMYK][COLOR_MODEL_AHSL](&acmykf, this);
  return acmykf;
}

FOG_INLINE AhsvF AcmykF::getAhsvF() const
{
  AhsvF ahsvf(UNINITIALIZED);
  _g2d.color.convert[COLOR_MODEL_AHSV][COLOR_MODEL_ACMYK](&ahsvf, this);
  return ahsvf;
}

FOG_INLINE AhslF AcmykF::getAhslF() const
{
  AhslF ahslf(UNINITIALIZED);
  _g2d.color.convert[COLOR_MODEL_AHSL][COLOR_MODEL_ACMYK](&ahslf, this);
  return ahslf;
}

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::Color, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE(Fog::Color,
  a._model == b._model &&
  a._hints == b._hints &&
  a._argb32.u32 == b._argb32.u32 &&
  Math::feqv(a._data, b._data, 5)
)

// [Guard]
#endif // _FOG_G2D_SOURCE_COLOR_H
