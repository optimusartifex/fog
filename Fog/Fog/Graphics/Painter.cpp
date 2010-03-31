// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Painter.h>
#include <Fog/Graphics/PaintEngine.h>

#include <Fog/Graphics/PaintEngine/Null_p.h>
#include <Fog/Graphics/PaintEngine/Raster_p.h>

namespace Fog {

// ============================================================================
// [Fog::Painter]
// ============================================================================

static Static<NullPaintEngine> _nullPaintEngine;
PaintEngine* Painter::sharedNull;

Painter::Painter()
{
  _engine = sharedNull;
}

Painter::Painter(Image& image, uint32_t initFlags)
{
  _engine = sharedNull;
  begin(image, initFlags);
}

Painter::~Painter()
{
  if (_engine != sharedNull) delete _engine;
}

err_t Painter::begin(Image& image, uint32_t initFlags)
{
  end();

  if (image.isEmpty()) return ERR_IMAGE_INVALID_SIZE;

  // If image is not empty, the format can't be Null.
  int format = image.getFormat();
  if (format == PIXEL_FORMAT_I8) return ERR_IMAGE_UNSUPPORTED_FORMAT;

  FOG_RETURN_ON_ERROR(image.detach());

  Image::Data* image_d = image._d;
  ImageBuffer buffer;

  buffer.width = image_d->width;
  buffer.height = image_d->height;
  buffer.format = image_d->format;
  buffer.stride = image_d->stride;
  buffer.data = image_d->first;

  PaintEngine* d = new(std::nothrow) RasterPaintEngine(buffer, initFlags);
  if (!d) return ERR_RT_OUT_OF_MEMORY;

  _engine = d;
  return ERR_OK;
}

err_t Painter::begin(const ImageBuffer& buffer, uint32_t initFlags)
{
  end();

  // Check for invalid arguments.
  if (buffer.data == NULL) return ERR_RT_INVALID_ARGUMENT;

  // Never initialize painter for zero image.
  if (buffer.width <= 0 || buffer.height <= 0) return ERR_IMAGE_INVALID_SIZE;

  // Check for valid image format.
  if ((uint)buffer.format >= PIXEL_FORMAT_COUNT) return ERR_RT_INVALID_ARGUMENT;
  if ((uint)buffer.format == PIXEL_FORMAT_I8) return ERR_IMAGE_UNSUPPORTED_FORMAT;

  PaintEngine* d = new(std::nothrow) RasterPaintEngine(buffer, initFlags);
  if (!d) return ERR_RT_OUT_OF_MEMORY;

  _engine = d;
  return ERR_OK;
}

void Painter::end()
{
  if (_engine != sharedNull) delete _engine;
  _engine = sharedNull;
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_painter_init(void)
{
  using namespace Fog;

  _nullPaintEngine.init();
  Painter::sharedNull = _nullPaintEngine.instancep();

  return ERR_OK;
}

FOG_INIT_DECLARE void fog_painter_shutdown(void)
{
  using namespace Fog;

  _nullPaintEngine.destroy();
  Painter::sharedNull = NULL;
}
