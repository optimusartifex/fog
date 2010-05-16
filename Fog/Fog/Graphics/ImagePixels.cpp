// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Graphics/ImagePixels.h>

namespace Fog {

// ============================================================================
// [Fog::ImagePixels]
// ============================================================================

ImagePixels::ImagePixels() :
  _size(0, 0),
  _format(IMAGE_FORMAT_NULL),
  _stride(0),
  _data(NULL)
{
  _relatedImage->_d = NULL;
}

ImagePixels::~ImagePixels()
{
  if (_relatedImage->_d != NULL) _relatedImage->unlockPixels(*this);
}

void ImagePixels::_clear()
{
  _size.clear();
  _format = IMAGE_FORMAT_NULL;
  _stride = 0;
  _data = NULL;
  _relatedImage->_d = NULL;
}

err_t ImagePixels::_ready()
{
  return ERR_OK;
}

} // Fog namespace
