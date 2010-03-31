// [Fog-Graphics Library - Public API]
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

//! @addtogroup Fog_Graphics
//! @{

namespace Fog {

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
// [Fog::SymmetricConvolveParamsF]
// ============================================================================

struct FOG_HIDDEN SymmetricConvolveParamsF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE SymmetricConvolveParamsF() :
    borderExtend(IMAGE_FILTER_EXTEND_PAD),
    borderColor(0x00000000)
  {
  }

  FOG_INLINE SymmetricConvolveParamsF(const SymmetricConvolveParamsF& other) :
    hMatrix(other.hMatrix),
    vMatrix(other.vMatrix),
    borderExtend(other.borderExtend),
    borderColor(other.borderColor)
  {
  }

  FOG_INLINE SymmetricConvolveParamsF(
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

  FOG_INLINE ~SymmetricConvolveParamsF()
  {
  }

  FOG_NO_INLINE SymmetricConvolveParamsF& operator=(const SymmetricConvolveParamsF& other)
  {
    hMatrix = other.hMatrix;
    vMatrix = other.vMatrix;
    borderExtend = other.borderExtend;
    borderColor = other.borderColor;
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

} // Fog namespace

//! @}

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::BlurParams, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::SymmetricConvolveParamsF, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_GRAPHICS_IMAGEFILTERPARAMS_H
