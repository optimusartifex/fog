// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Graphics/ImageFilterEngine.h>

namespace Fog {

// ============================================================================
// [Fog::ImageFilterEngine - Construction / Destruction]
// ============================================================================

ImageFilterEngine::ImageFilterEngine(uint32_t type) :
  type(type),
  characteristics(0)
{
  refCount.init(1);
}

ImageFilterEngine::~ImageFilterEngine()
{
}

// ============================================================================
// [Fog::ImageFilterEngine - Interface - Basics]
// ============================================================================

bool ImageFilterEngine::isNop() const
{
  return false;
}

err_t ImageFilterEngine::getExtendedRect(IntRect& rect) const
{
  // Default is no extents.
  FOG_UNUSED(rect);
  return ERR_OK;
}

// ============================================================================
// [Fog::ImageFilterEngine - Interface - Context]
// ============================================================================

const void* ImageFilterEngine::getContext() const
{
  return NULL;
}

void ImageFilterEngine::releaseContext(const void* context) const
{
  FOG_UNUSED(context);
}

// ============================================================================
// [Fog::ImageFilterEngine - Interface - Filtering]
// ============================================================================

ColorFilterFn ImageFilterEngine::getColorFilterFn(uint32_t format) const
{
  FOG_UNUSED(format);
  return NULL;
}

ImageFilterFn ImageFilterEngine::getImageFilterFn(uint32_t format, uint32_t processing) const
{
  FOG_UNUSED(format);
  FOG_UNUSED(processing);
  return NULL;
}

} // Fog namespace
