// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Constants.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Scanline_p.h>

namespace Fog {

Scanline8::Scanline8(uint32_t spanSize) :
  _maskData(NULL),
  _maskCurrent(NULL),
  _maskCapacity(0),
  _spanLast(NULL),
  _spanSaved(NULL),
  _spanCurrent(NULL),
  _spanAllocator(500),
  _spanSize(spanSize)
{
  // Make sure that span size is valid.
  FOG_ASSERT(spanSize >= sizeof(Span8));

  // Initialize _spanFirst, our first 'invisible' span.
  _spanFirst._x1 = INVALID_SPAN;
  _spanFirst._type = SPAN_TYPE_CMASK;
  _spanFirst._x2 = INVALID_SPAN;
  _spanFirst._mask = NULL;
  _spanFirst._next = NULL;
}

Scanline8::~Scanline8()
{
  if (_maskData) Memory::free(_maskData);
}

err_t Scanline8::_prepareScanline(int w)
{
  FOG_ASSERT(w > 0);
  w = Math::max<int>(w + SPAN_CMASK_LENGTH_THRESHOLD, 512);

  // See prepareScanline()
  FOG_ASSERT(_maskCapacity < (uint)w);

  if (_maskData) Memory::free(_maskData);

  _maskData = reinterpret_cast<uint8_t*>(Memory::alloc(w));
  _maskCapacity = w;

  if (FOG_UNLIKELY(_maskData == NULL))
  {
    _maskCapacity = 0;
    return ERR_RT_OUT_OF_MEMORY;
  }

  return ERR_OK;
}

err_t Scanline8::_newScanline(int x1, int x2)
{
  FOG_ASSERT(x1 < x2);

  err_t err = _prepareScanline(x2 - x1);
  if (err != ERR_OK) return err;

  // Duplicated, the same code is also in newScanline().
  _maskCurrent = _maskData;
  _spanCurrent = &_spanFirst;
  if (_spanLast) _spanLast->setNext(_spanSaved);

  // If we are called the first time, we also need to prepare some spans.
  if (_spanCurrent->getNext() == NULL) _growSpans();

  return ERR_OK;
}

Span8* Scanline8::_growSpans()
{
  // 10 spans is our grow limit. When painting antialiased shapes where are no
  // intersections we need usually 3 spans. When painting antialiased stroked
  // shapes then span we need usually 6 spans per scanline. 10 spans growth is
  // convenient for filling and stroking shapes that could be simple or complex.
  const sysuint_t GROW_BY = 10;

  Span8* cur = _spanCurrent;

  for (sysuint_t i = 0; i < GROW_BY; i++)
  {
    Span8* span = reinterpret_cast<Span8*>(_spanAllocator.alloc(_spanSize));
    if (FOG_UNLIKELY(span == NULL)) goto failed;
    cur->setNext(span);
    cur = span;
  }

okay:
  cur->setNext(NULL);
  return _spanCurrent->getNext();

failed:
  // Be silent if we failed, but some spans were allocated. We can fail in next
  // call to _growSpans(). For now we are complete.
  if (cur != _spanCurrent) goto okay;

  // Okay, we really failed to allocate at least one new span instance. What
  // to do? Simply, make infinite chain from the last span so the allocation
  // will never fail again for this scanline. We allocated the space needed
  // for mask for the whole scanline before so we can't get out of bounds in
  // _maskCurrent pointer used by addValue().
  //
  // This infinite chain is checked and cleared by endScanline(). The 
  // endScanline() method will also report the memory allocation failure
  // that occurred here.
  _spanCurrent->setNext(_spanCurrent);
  return _spanCurrent;
}

} // Fog namespace
