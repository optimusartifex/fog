// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Graphics/ImageContext.h>

namespace Fog {

// ============================================================================
// [Fog::ImageContext]
// ============================================================================

ImageContext::ImageContext()
{
}

ImageContext::~ImageContext()
{
}

void ImageContext::_clear()
{
  ImagePixels::_clear();
}

err_t ImageContext::_ready()
{
  return ERR_OK;
}

} // Fog namespace
