// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Painting/RasterPaintContext_p.h>

namespace Fog {

// ============================================================================
// [Fog::RasterContext - Construction / Destruction]
// ============================================================================

RasterContext::RasterContext() :
  engine(NULL),
  precision(0xFFFFFFFF),
  clipType(RASTER_CLIP_BOX),
  clipBoxI(0, 0, 0, 0)
{
  scope.reset();
  layer.reset();

  paintHints.packed = 0;
  rasterHints.packed = 0;

  solid.prgb32.u32 = 0xFF000000;
  solid.prgb64.u64 = FOG_UINT64_C(0xFFFF000000000000);
  pc = (RasterPattern*)(size_t)0x1;

  closure.ditherOrigin.reset();
  closure.palette = NULL;
  closure.data = NULL;
}

RasterContext::~RasterContext()
{
  _initPrecision(0xFFFFFFFF);
}

// ============================================================================
// [Fog::RasterContext - Init / Reset]
// ============================================================================

err_t RasterContext::_initByMaster(const RasterContext& master)
{
  // If mask was created we can't copy the data from master context into thread's
  // own. The RasterEngine must check for this condition before and must return
  // @c ERR_PAINTER_NOT_ALLOWED error.
  //FOG_ASSERT(mask == NULL && master.mask == NULL);

  layer = master.layer;

  clipType = master.clipType;
  clipRegion = master.clipRegion;
  clipBoxI = master.clipBoxI;

  paintHints = master.paintHints;
  rasterHints = master.rasterHints;

  closure = master.closure;
  solid.prgb64 = master.solid.prgb64;
  solid.prgb32 = master.solid.prgb32;
  pc = (RasterPattern*)(size_t)0x1;

  return _initPrecision(master.precision);
}

err_t RasterContext::_initPrecision(uint32_t precision)
{
  if (this->precision != precision)
  {
    // Destroy resources using an old precision.
    switch (this->precision)
    {
      case IMAGE_PRECISION_BYTE:
        boxRasterizer8.destroy();
        pathRasterizer8.destroy();
        scanline8.destroy();
        break;

      case IMAGE_PRECISION_WORD:
        // TODO: 16-bit image processing.
        // boxRasterizer16.destroy();
        // pathRasterizer16.destroy();
        // scanline16.destroy();
        break;

      default:
        break;
    }

    // Create resources using the new precision.
    this->precision = precision;

    switch (this->precision)
    {
      case IMAGE_PRECISION_BYTE:
        fullOpacity.u = 0x100;
        fullOpacity.f = float(0x100);
        boxRasterizer8.init();
        pathRasterizer8.init();
        scanline8.init();
        break;

      case IMAGE_PRECISION_WORD:
        fullOpacity.u = 0x10000;
        fullOpacity.f = float(0x10000);
        // TODO: 16-bit image processing.
        // boxRasterizer16.init();
        // pathRasterizer16.init();
        // scanline16.init();
        break;

      default:
        break;
    }
  }

  uint32_t pcBpl = layer.size.w;
  switch (this->precision)
  {
    case IMAGE_PRECISION_BYTE:
      pcBpl *= 4;
      break;

    case IMAGE_PRECISION_WORD:
      pcBpl *= 8;
      // TODO: 16-bit image processing.
      break;

    default:
      pcBpl = 0;
      return ERR_OK;
  }

  uint8_t* pcBuf = reinterpret_cast<uint8_t*>(buffer.alloc(pcBpl));
  return pcBuf != NULL ? (err_t)ERR_OK : (err_t)ERR_RT_OUT_OF_MEMORY;
}

void RasterContext::_reset()
{
  // TODO: Not used.
}

} // Fog namespace
