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
#include <Fog/Core/Memory/Alloc.h>
#include <Fog/G2d/Rasterizer/Scanline_p.h>

namespace Fog {

// ============================================================================
// [Fog::Scanline - Construction / Destruction]
// ============================================================================

Scanline::Scanline(uint32_t spanSize, uint32_t maskUnit) :
  _maskData(NULL),
  _maskCurrent(NULL),
  _maskCapacity(0),
  _maskUnit(maskUnit),
  _spanAllocator(512),
  _spanLast(NULL),
  _spanSaved(NULL),
  _spanCurrent(NULL),
  _spanSize(spanSize)
{
  // Make sure that span size is based on the 'Span' class.
  FOG_ASSERT(spanSize >= sizeof(Span));

  // Initialize the '_spanFirst', the first 'invisible' span.
  _spanFirst._x0 = SPAN_INVALID_POSITION;
  _spanFirst._type = SPAN_C;
  _spanFirst._x1 = SPAN_INVALID_POSITION;
  _spanFirst._mask = NULL;
  _spanFirst._next = NULL;
}

Scanline::~Scanline()
{
  if (_maskData) Memory::free(_maskData);
}

// ============================================================================
// [Fog::Scanline - Private]
// ============================================================================

err_t Scanline::_prepare(int w)
{
  FOG_ASSERT(w > 0);
  w = Math::max<int>(w + SPAN_C_THRESHOLD, 512);

  // See prepare()
  FOG_ASSERT(_maskCapacity < (uint)w);

  if (_maskData) Memory::free(_maskData);

  _maskData = reinterpret_cast<uint8_t*>(Memory::alloc(w * _maskUnit));
  _maskCapacity = w * _maskUnit;

  if (FOG_UNLIKELY(_maskData == NULL))
  {
    _maskCapacity = 0;
    return ERR_RT_OUT_OF_MEMORY;
  }

  return ERR_OK;
}

err_t Scanline::_begin(int x0, int x1)
{
  FOG_ASSERT(x0 < x1);

  err_t err = _prepare(x1 - x0);
  if (FOG_IS_ERROR(err)) return err;

  // Duplicated, the same code is also in begin().
  _maskCurrent = _maskData;
  _spanCurrent = &_spanFirst;
  if (_spanLast) _spanLast->setNext(_spanSaved);

  // If we are called the first time, we also need to prepare some spans.
  if (_spanCurrent->getNext() == NULL) _growSpans();

  return ERR_OK;
}

Span* Scanline::_growSpans()
{
  // 10 spans is our grow limit. When painting antialiased shapes where are no
  // intersections we need usually 3 spans. When painting antialiased stroked
  // shapes then span we need usually 6 spans per scanline. 10 spans growth is
  // convenient for filling and stroking shapes that could be simple or complex.
  const uint GROW_BY = 10;
  Span* cur = _spanCurrent;

  for (uint i = 0; i < GROW_BY; i++)
  {
    Span8* span = reinterpret_cast<Span8*>(_spanAllocator.alloc(_spanSize));
    if (FOG_UNLIKELY(span == NULL)) goto _Fail;
    cur->setNext(span);
    cur = span;
  }

_Done:
  cur->setNext(NULL);
  return _spanCurrent->getNext();

_Fail:
  // Be silent if we failed, but some spans were allocated. We can fail in next
  // call to _growSpans(). For now we are complete.
  if (cur != _spanCurrent) goto _Done;

  // Okay, we really failed to allocate at least one new span instance. What
  // to do? Simply, make infinite chain from the last span so the allocation
  // will never fail again for this scanline. We allocated the space needed
  // for mask for the whole scanline before so we can't get out of bounds in
  // _maskCurrent pointer used by addValue().
  //
  // This infinite chain is checked and cleared by the @c close() methpd. It
  // will also report the memory allocation failure that occurred here.
  _spanCurrent->setNext(_spanCurrent);
  return _spanCurrent;
}

} // Fog namespace
