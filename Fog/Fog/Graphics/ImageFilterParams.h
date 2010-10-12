// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_IMAGEFILTERPARAMS_H
#define _FOG_GRAPHICS_IMAGEFILTERPARAMS_H

// [Dependencies]
#include <Fog/Core/List.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/ConvolutionMatrix.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Effects
//! @{

// ============================================================================
// [Fog::ImageBlurParams]
// ============================================================================

struct FOG_HIDDEN BlurParams
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE BlurParams() :
    blur(IMAGE_FILTER_BLUR_LINEAR),
    hRadius(1.0),
    vRadius(1.0),
    borderExtend(IMAGE_FILTER_EXTEND_PAD),
    borderColor(0x00000000)
  {
  }

  FOG_INLINE BlurParams(
    uint32_t blur,
    float hRadius,
    float vRadius,
    uint32_t borderExtend = IMAGE_FILTER_EXTEND_PAD,
    uint32_t borderColor = 0x00000000)
    :
    blur(blur),
    hRadius(hRadius),
    vRadius(vRadius),
    borderExtend(borderExtend),
    borderColor(borderColor)
  {
  }

  FOG_INLINE ~BlurParams()
  {
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Blur type, see @c IMAGE_FILTER_BLUR_TYPE.
  uint32_t blur;

  //! @brief Horizontal blur radius.
  float hRadius;
  //! @brief Vertical blur radius.
  float vRadius;

  //! @brief Border extend mode, see @c IMAGE_FILTER_EXTEND_TYPE.
  uint32_t borderExtend;
  //! @brief Border color (non-premultiplied).
  uint32_t borderColor;
};

// ============================================================================
// [Fog::FloatSymmetricConvolveParams]
// ============================================================================

struct FOG_HIDDEN FloatSymmetricConvolveParams
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FloatSymmetricConvolveParams() :
    borderExtend(IMAGE_FILTER_EXTEND_PAD),
    borderColor(0x00000000)
  {
  }

  FOG_INLINE FloatSymmetricConvolveParams(const FloatSymmetricConvolveParams& other) :
    hMatrix(other.hMatrix),
    vMatrix(other.vMatrix),
    borderExtend(other.borderExtend),
    borderColor(other.borderColor)
  {
  }

  FOG_INLINE FloatSymmetricConvolveParams(
    const List<float>& hMatrix,
    const List<float>& vMatrix,
    uint32_t borderExtend = IMAGE_FILTER_EXTEND_PAD,
    uint32_t borderColor = 0x00000000)
    :
    hMatrix(hMatrix),
    vMatrix(vMatrix),
    borderExtend(IMAGE_FILTER_EXTEND_PAD),
    borderColor(0x00000000)
  {
  }

  FOG_INLINE ~FloatSymmetricConvolveParams()
  {
  }

  FOG_NO_INLINE FloatSymmetricConvolveParams& operator=(const FloatSymmetricConvolveParams& other)
  {
    hMatrix = other.hMatrix;
    vMatrix = other.vMatrix;
    borderExtend = other.borderExtend;
    borderColor = other.borderColor;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  List<float> hMatrix;
  List<float> vMatrix;

  //! @brief Border extend mode, see @c IMAGE_FILTER_EXTEND_TYPE.
  uint32_t borderExtend;
  //! @brief Border color (non-premultiplied).
  uint32_t borderColor;
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::BlurParams, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::FloatSymmetricConvolveParams, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_GRAPHICS_IMAGEFILTERPARAMS_H
