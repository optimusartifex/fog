// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Global/Init_p.h>
#include <Fog/G2d/Tools/Matrix.h>

namespace Fog {

// ============================================================================
// [Fog::MatrixF - Construction / Destruction]
// ============================================================================

MatrixF::MatrixF() :
  _d(_dnull->ref())
{
}

MatrixF::MatrixF(const MatrixF& other) :
  _d(other._d->ref())
{
}

MatrixF::MatrixF(const SizeI& size, const float* data) :
  _d(_dalloc(size))
{
  if (FOG_UNLIKELY(_d == NULL))
  {
    _d = _dnull->ref();
    return;
  }

  if (data)
  {
    Memory::copy(_d->data, data, _d->size.w * _d->size.h * sizeof(float));
  }
}

MatrixF::~MatrixF()
{
  _d->deref();
}

// ============================================================================
// [Fog::MatrixF - Methods]
// ============================================================================

err_t MatrixF::_detach()
{
  if (isDetached() || isEmpty()) return ERR_OK;

  MatrixDataF* newd = _dalloc(_d->size);
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  Memory::copy(newd->data, _d->data, _d->size.w * _d->size.h * sizeof(float));

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

err_t MatrixF::create(const SizeI& size, const float* data)
{
  if (FOG_UNLIKELY(!size.isValid()))
  {
    reset();
    return (size.w == 0 && size.h == 0)
      ? (err_t)ERR_OK
      : (err_t)ERR_RT_INVALID_ARGUMENT;
  }

  if ((_d->size.w * _d->size.h) == (size.w * size.h) && isDetached())
  {
    if (data) Memory::copy(_d->data, data, size.w * size.h * sizeof(float));
    _d->size = size;
    return ERR_OK;
  }

  MatrixDataF* newd = _dalloc(size);
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  if (data) Memory::copy(newd->data, data, size.w * size.h * sizeof(float));

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

err_t MatrixF::resize(const SizeI& size, float value)
{
  if (FOG_UNLIKELY(!size.isValid()))
  {
    reset();
    return (size.w == 0 && size.h == 0)
      ? (err_t)ERR_OK
      : (err_t)ERR_RT_INVALID_ARGUMENT;
  }

  if (_d->size == size) return ERR_OK;

  MatrixDataF* newd = _dalloc(size);
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  int x, y;

  int copyw = Math::min(_d->size.w, size.w);
  int copyh = Math::min(_d->size.h, size.h);
  int zerow = size.w - copyw;

  float* dstCur = newd->data;
  float* srcRow = _d->data;

  // Copy/Set matrix rows.
  for (y = copyh; y != 0; y--, srcRow += _d->size.w)
  {
    float* srcCur = srcRow;

    for (x = copyw; x; x--, dstCur++, srcCur++) dstCur[0] = srcCur[0];
    for (x = zerow; x; x--, dstCur++          ) dstCur[0] = value;
  }

  // Clear created rows.
  for (y = (size.h - copyh) * size.w; y; y--, dstCur++)
  {
    dstCur[0] = value;
  }

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

void MatrixF::reset()
{
  atomicPtrXchg(&_d, _dnull->ref())->deref();
}

float MatrixF::getCell(int x, int y) const
{
  MatrixDataF* d = _d;
  if ((uint)x >= (uint)d->size.w || (uint)y >= (uint)_d->size.h) return 0.0f;

  return d->data[y * d->size.w + x];
}

err_t MatrixF::setCell(int x, int y, float val)
{
  MatrixDataF* d = _d;
  if ((uint)x >= (uint)d->size.w || (uint)y >= (uint)d->size.h) return ERR_RT_INVALID_ARGUMENT;

  FOG_RETURN_ON_ERROR(detach());
  d = _d;

  d->data[y * d->size.w + x] = val;
  return ERR_OK;
}

err_t MatrixF::fill(const RectI& rect, float val)
{
  int x0 = Math::max<int>(rect.getX0(), 0);
  int y0 = Math::max<int>(rect.getY0(), 0);

  int x1 = Math::min<int>(rect.getX1(), getWidth());
  int y1 = Math::min<int>(rect.getY1(), getHeight());

  if (x0 >= x1 || y0 >= y1) return ERR_RT_INVALID_ARGUMENT;

  FOG_RETURN_ON_ERROR(detach());

  int width = _d->size.w;
  float* p = &_d->data[y0 * width + x0];
  
  int x, y;
  int fillw = x1 - x0;
  int fillh = y1 - y0;

  for (y = fillh; y; y--, p += width)
  {
    for (x = 0; x < fillw; x++) p[x] = val;
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::MatrixF - Operator Overload]
// ============================================================================

MatrixF& MatrixF::operator=(const MatrixF& other)
{
  atomicPtrXchg(&_d, other._d->ref())->deref();
  return *this;
}

// ============================================================================
// [Fog::MatrixF - Statics]
// ============================================================================

Static<MatrixDataF> MatrixF::_dnull;

MatrixDataF* MatrixF::_dalloc(const SizeI& size)
{
  if (!size.isValid()) return _dnull->ref();

  MatrixDataF* d = reinterpret_cast<MatrixDataF*>(
    Memory::alloc(sizeof(MatrixDataF) - sizeof(float) + (size.w * size.h) * sizeof(float)));
  if (FOG_IS_NULL(d)) return NULL;

  d->refCount.init(1);
  d->size = size;
  return d;
}

void MatrixF::_dcopy(
  MatrixDataF* dst, int dstX, int dstY, 
  MatrixDataF* src, int srcX, int srcY, int w, int h)
{
  int x, y;

  float* dstCur = dst->data + dstY * dst->size.w + dstX;
  float* srcCur = src->data + srcY * src->size.w + srcX;

  for (y = h; y; y--, dstCur += dst->size.w, srcCur += src->size.w)
  {
    for (x = 0; x < w; x++) dstCur[x] = srcCur[x];
  }
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_genericmatrix_init(void)
{
  MatrixF::_dnull->refCount.init(1);
  MatrixF::_dnull->size.reset();
}

FOG_NO_EXPORT void _g2d_genericmatrix_fini(void)
{
  MatrixF::_dnull->refCount.dec();
}

} // Fog namespace
