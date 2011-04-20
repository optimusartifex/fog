// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/G2d/Global/Init_G2d_p.h>
#include <Fog/G2d/Imaging/ImageEffect.h>

namespace Fog {

// ============================================================================
// [Fog::ImageEffect - Construction / Destruction]
// ============================================================================

ImageEffect::ImageEffect()
{
  _d = _dnull->ref();
}

ImageEffect::ImageEffect(const ImageEffect& other) : 
  _d(other._d->ref())
{
}

ImageEffect::~ImageEffect()
{
  _d->deref();
}

// ============================================================================
// [Fog::ImageEffect - Effect]
// ============================================================================

err_t ImageEffect::_getEffect(uint32_t type, void* effect) const
{
  // TODO: Not implemented.
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t ImageEffect::_setEffect(uint32_t type, const void* effect)
{
  // TODO: Not implemented.
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t ImageEffect::setEffect(const ImageEffect& effect)
{
  atomicPtrXchg(&_d, effect._d->ref())->deref();
  return ERR_OK;
}

// ============================================================================
// [Fog::ImageEffect - Reset]
// ============================================================================

void ImageEffect::reset()
{
  if (_d == _dnull.instancep()) return;
  atomicPtrXchg(&_d, _dnull->ref())->deref();
}

// ============================================================================
// [Fog::ImageEffect - Statics]
// ============================================================================

Static<ImageEffectData> ImageEffect::_dnull;

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_imageeffect_init(void)
{
  ImageEffectData* d = ImageEffect::_dnull.instancep();

  d->refCount.init(1);
  d->destroy = NULL; // Never called.
  d->type = IMAGE_EFFECT_NONE;
}

FOG_NO_EXPORT void _g2d_imageeffect_fini(void)
{
  ImageEffectData* d = ImageEffect::_dnull.instancep();

  d->refCount.deref();
}

} // Fog namespace
