// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/G2d/Painting/RasterScanline_p.h>

namespace Fog {

// ============================================================================
// [Fog::RasterScanline - Construction / Destruction]
// ============================================================================

RasterScanline::RasterScanline() :
  _mask(NULL),
  _maskCapacity(0),
  _spanAllocator(1000)
{
  // Initialize the '_fakeSpan'.
  _fakeSpan._x0 = RASTER_SPAN_INVALID_X;
  _fakeSpan._type = RASTER_SPAN_C;
  _fakeSpan._x1 = RASTER_SPAN_INVALID_X;
  _fakeSpan._mask = NULL;
  _fakeSpan._next = NULL;
}

RasterScanline::~RasterScanline()
{
  if (_mask != NULL)
    MemMgr::free(_mask);
}

// ============================================================================
// [Fog::Scanline - Private]
// ============================================================================

err_t RasterScanline::_prepare(size_t maxSize)
{
  FOG_ASSERT(maxSize > 0);

  // See prepare()
  FOG_ASSERT(_maskCapacity < (uint)maxSize);

  // Align.
  maxSize = (maxSize + 511) & ~(size_t)511;

  // Free old mask storage.
  if (_mask != NULL)
    MemMgr::free(_mask);

  // Alloc new mask storage.
  _mask = reinterpret_cast<uint8_t*>(MemMgr::alloc(maxSize));
  _maskCapacity = 0;

  if (FOG_UNLIKELY(_mask == NULL))
    return ERR_RT_OUT_OF_MEMORY;

  // Everything seems fine.
  _maskCapacity = maxSize;
  return ERR_OK;
}

} // Fog namespace
