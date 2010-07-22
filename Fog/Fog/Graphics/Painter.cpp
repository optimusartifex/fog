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

#include <Fog/Graphics/PaintEngine/PaintEngine_Null_p.h>
#include <Fog/Graphics/PaintEngine/PaintEngine_Raster_Engine_p.h>

namespace Fog {

// ============================================================================
// [Fog::Painter]
// ============================================================================

static Static<NullPaintEngine> _nullPaintEngine;
PaintEngine* Painter::_dnull;

static FOG_INLINE bool Painter_isFormatSupported(uint32_t format)
{
  // This function is called after null image check, so the pixel format
  // can't be null (IMAGE_FORMAT_NULL) at this time.
  FOG_ASSERT(format < IMAGE_FORMAT_COUNT);

  if (format == IMAGE_FORMAT_I8) return false;

  return true;
}

static FOG_INLINE bool Painter_isRectValid(const Image& image, const IntRect& rect)
{
  int x1 = rect.x;
  int y1 = rect.y;
  int x2 = x1 + rect.w;
  int y2 = y1 + rect.h;

  if (x1 < 0 || y1 < 0 || x2 > image.getWidth() || y2 > image.getHeight() || x1 >= x2 || y1 >= y2) 
    return false;
  else
    return true;
}

Painter::Painter()
{
  _engine = _dnull;
}

Painter::Painter(Image& image, uint32_t initFlags)
{
  _engine = _dnull;
  begin(image, initFlags);
}

Painter::Painter(Image& image, const IntRect& rect, uint32_t initFlags)
{
  _engine = _dnull;
  begin(image, rect, initFlags);
}

Painter::~Painter()
{
  if (_engine != _dnull) fog_delete(_engine);
}

err_t Painter::begin(Image& image, uint32_t initFlags)
{
  // End painting.
  end();

  // Do basic checks.
  uint32_t format = image.getFormat();

  if (image.isEmpty()) return ERR_IMAGE_INVALID_SIZE;
  if (!Painter_isFormatSupported(format)) return ERR_PAINTER_UNSUPPORTED_FORMAT;

  // Create image buffer.
  FOG_RETURN_ON_ERROR(image.detach());

  ImageBuffer buffer;
  buffer.import(image._d);

  // Create paint engine.
  PaintEngine* d = fog_new RasterPaintEngine(buffer, image._d, initFlags);
  if (!d) return ERR_RT_OUT_OF_MEMORY;

  _engine = d;
  return ERR_OK;
}

err_t Painter::begin(Image& image, const IntRect& rect, uint32_t initFlags)
{
  // End painting.
  end();

  // Do basic checks.
  uint32_t format = image.getFormat();

  if (image.isEmpty()) return ERR_RT_INVALID_ARGUMENT;
  if (!Painter_isRectValid(image, rect)) return ERR_RT_INVALID_ARGUMENT;
  if (!Painter_isFormatSupported(format)) return ERR_PAINTER_UNSUPPORTED_FORMAT;

  // Create image buffer.
  FOG_RETURN_ON_ERROR(image.detach());

  ImageBuffer buffer;
  buffer.import(image._d, rect);

  // Create paint engine.
  PaintEngine* d = fog_new RasterPaintEngine(buffer, image._d, initFlags);
  if (!d) return ERR_RT_OUT_OF_MEMORY;

  _engine = d;
  return ERR_OK;
}

err_t Painter::begin(const ImageBuffer& buffer, uint32_t initFlags)
{
  // End painting.
  end();

  // Do basic checks.
  if (!buffer.isValid()) return ERR_RT_INVALID_ARGUMENT;
  if (!Painter_isFormatSupported(buffer.format)) return ERR_PAINTER_UNSUPPORTED_FORMAT;

  // Create paint engine.
  PaintEngine* d = fog_new RasterPaintEngine(buffer, NULL, initFlags);
  if (!d) return ERR_RT_OUT_OF_MEMORY;

  _engine = d;
  return ERR_OK;
}

void Painter::end()
{
  if (_engine == _dnull) return;

  fog_delete(_engine);
  _engine = _dnull;
}

err_t Painter::switchTo(Image& image)
{
  uint32_t engineType = _engine->getEngine();

  // We must ensure that engine is PAINT_ENGINE_RASTER_XXX.
  if (engineType != PAINT_ENGINE_RASTER_ST && engineType != PAINT_ENGINE_RASTER_MT)
    return begin(image);

  // Do basic checks.
  if (image.isEmpty()) return ERR_RT_INVALID_ARGUMENT;
  if (!Painter_isFormatSupported(image.getFormat())) return ERR_PAINTER_UNSUPPORTED_FORMAT;

  // Create image buffer and switch to it.
  ImageBuffer buffer;
  buffer.import(image._d);

  return reinterpret_cast<RasterPaintEngine*>(_engine)->switchTo(buffer, image._d);
}

err_t Painter::switchTo(Image& image, const IntRect& rect)
{
  uint32_t engineType = _engine->getEngine();

  // We must ensure that engine is PAINT_ENGINE_RASTER_XXX.
  if (engineType != PAINT_ENGINE_RASTER_ST && engineType != PAINT_ENGINE_RASTER_MT)
    return begin(image, rect);

  // Do basic checks.
  if (image.isEmpty()) return ERR_RT_INVALID_ARGUMENT;
  if (!Painter_isRectValid(image, rect)) return ERR_RT_INVALID_ARGUMENT;
  if (!Painter_isFormatSupported(image.getFormat())) return ERR_PAINTER_UNSUPPORTED_FORMAT;

  // Create image buffer and switch to it.
  ImageBuffer buffer;
  buffer.import(image._d, rect);

  return reinterpret_cast<RasterPaintEngine*>(_engine)->switchTo(buffer, image._d);
}

err_t Painter::switchTo(const ImageBuffer& buffer)
{
  uint32_t engineType = _engine->getEngine();

  // We must ensure that engine is PAINT_ENGINE_RASTER_XXX.
  if (engineType != PAINT_ENGINE_RASTER_ST && engineType != PAINT_ENGINE_RASTER_MT)
    return begin(buffer);

  // Do basic checks.
  if (!buffer.isValid()) return ERR_RT_INVALID_ARGUMENT;
  if (!Painter_isFormatSupported(buffer.format)) return ERR_PAINTER_UNSUPPORTED_FORMAT;

  // Switch to a given buffer.
  return reinterpret_cast<RasterPaintEngine*>(_engine)->switchTo(buffer, NULL);
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_painter_init(void)
{
  using namespace Fog;

  _nullPaintEngine.init();
  Painter::_dnull = _nullPaintEngine.instancep();

  return ERR_OK;
}

FOG_INIT_DECLARE void fog_painter_shutdown(void)
{
  using namespace Fog;

  _nullPaintEngine.destroy();
  Painter::_dnull = NULL;
}
