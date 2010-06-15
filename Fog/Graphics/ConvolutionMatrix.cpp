// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Graphics/ConvolutionMatrix.h>
#include <Fog/Graphics/Geometry.h>

namespace Fog {

// ============================================================================
// [Fog::ConvolutionMatrixI]
// ============================================================================

Static<IntConvolutionMatrix::Data> IntConvolutionMatrix::_dnull;

IntConvolutionMatrix::IntConvolutionMatrix() :
  _d(_dnull->ref())
{
}

IntConvolutionMatrix::IntConvolutionMatrix(const IntConvolutionMatrix& other) :
  _d(other._d->ref())
{
}

IntConvolutionMatrix::~IntConvolutionMatrix()
{
  _d->deref();
}

IntConvolutionMatrix IntConvolutionMatrix::fromData(int w, int h, const ValueType* data)
{
  Data* d = NULL;

  if (w > 0 && h > 0 && (d = Data::alloc(w, h)))
    Memory::copy(d->m, data, (sysuint_t)w * (sysuint_t)h * (sysuint_t)sizeof(ValueType));
  else
    d = _dnull->ref();

  return IntConvolutionMatrix(d);
}

err_t IntConvolutionMatrix::_detach()
{
  if (_d->refCount.get() == 1) return ERR_OK;

  Data* newd = Data::alloc(_d->width, _d->height);
  if (!newd) return ERR_RT_OUT_OF_MEMORY;

  Data::copy(newd, 0, 0, _d, 0, 0, _d->width, _d->height);
  return ERR_OK;
}

err_t IntConvolutionMatrix::setData(int width, int height, const ValueType* data)
{
  if (_d->refCount.get() == 1 && (
      (_d->width == width && _d->height == height) ||
      (_d->width == height && _d->height == width)))
  {
    _d->width = width;
    _d->height = height;
    Memory::copy(_d->m, data, (sysuint_t)width * (sysuint_t)height * (sysuint_t)sizeof(ValueType));
    return ERR_OK;
  }

  Data* newd = Data::alloc(width, height);
  if (!newd) return ERR_RT_OUT_OF_MEMORY;

  Memory::copy(newd->m, data, (sysuint_t)width * (sysuint_t)height * (sysuint_t)sizeof(ValueType));
  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

err_t IntConvolutionMatrix::create(int width, int height)
{
  if (_d->width == width && _d->height == height) return ERR_OK;

  Data* newd = Data::alloc(width, height);
  if (!newd) return ERR_RT_OUT_OF_MEMORY;

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

err_t IntConvolutionMatrix::extend(int width, int height, ValueType value)
{
  if (_d->width == width && _d->height == height) return ERR_OK;

  Data* newd = Data::alloc(width, height);
  if (!newd) return ERR_RT_OUT_OF_MEMORY;

  int copyw = Math::min(_d->width, width);
  int copyh = Math::min(_d->height, height);
  int zerow = width - copyw;

  int x, y;

  ValueType *dstCur = newd->m;
  ValueType* srcM = _d->m;

  for (y = copyh; y; y--, srcM += _d->width)
  {
    ValueType *srcCur = srcM;

    for (x = copyw; x; x--, dstCur++, srcCur++) *dstCur++ = *srcCur++;
    for (x = zerow; x; x--) *dstCur++ = value;
  }

  for (y = copyh; y; y--)
  {
    for (x = width; x; x--) *dstCur++ = value;
  }

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

int IntConvolutionMatrix::getCell(int x, int y) const
{
  Data* d = _d;
  if ((uint)x >= (uint)d->width || (uint)y >= (uint)_d->height) return 0;

  return d->m[y * d->width + x];
}

err_t IntConvolutionMatrix::setCell(int x, int y, int val)
{
  if ((uint)x >= (uint)getWidth() || (uint)y >= (uint)getHeight()) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  Data* d = _d;
  d->m[y * d->width + x] = val;

  return ERR_OK;
}

err_t IntConvolutionMatrix::fill(const IntRect& rect, ValueType value)
{
  int x1 = Math::max<int>(rect.getX1(), 0);
  int y1 = Math::max<int>(rect.getY1(), 0);

  int x2 = Math::min<int>(rect.getX2(), getWidth());
  int y2 = Math::min<int>(rect.getY2(), getHeight());

  if (x1 >= x2 || y1 >= y2) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  int width = getWidth();
  ValueType* cur = &_d->m[y1 * width + x1];
  
  int w = x2 - x1;
  int h = y2 - y1;

  for (int y = h; y; y--, cur += width)
  {
    ValueType* p = cur;
    for (int x = w; x; x--, p++) *p = value;
  }

  return ERR_OK;
}

IntConvolutionMatrix& IntConvolutionMatrix::operator=(const IntConvolutionMatrix& other)
{
  atomicPtrXchg(&_d, other._d->ref())->deref();
  return *this;
}

IntConvolutionMatrix::ValueType* IntConvolutionMatrix::operator[](int y)
{
  Data* d = _d;
  if ((uint)y >= (uint)_d->height) return NULL;

  if (FOG_UNLIKELY(!isDetached()))
  {
    if (_detach() != ERR_OK) return NULL;
    d = _d;
  }

  return d->m + y * d->width;
}

// ============================================================================
// [Fog::ConvolutionMatrixI::Data]
// ============================================================================

IntConvolutionMatrix::Data* IntConvolutionMatrix::Data::alloc(int w, int h)
{
  if (w == 0 || h == 0) return _dnull->ref();

  Data* d = (Data*)Memory::alloc(sizeof(Data) + (w * h) * sizeof(ValueType));
  if (!d) return NULL;

  d->refCount.init(1);
  d->width = w;
  d->height = h;
  return d;
}

void IntConvolutionMatrix::Data::copy(Data* dst, int dstX, int dstY, Data* src, int srcX, int srcY, int w, int h)
{
  ValueType* dstM = dst->m + dstY * dst->width + dstX;
  ValueType* srcM = src->m + srcY * src->width + srcX;

  for (int y = h; y; y--, dstM += dst->width, srcM += src->width)
  {
    ValueType* dstCur = dstM;
    ValueType* srcCur = srcM;

    for (int x = w; x; x--, dstCur++, srcCur++) *dstCur++ = *srcCur++;
  }
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_convolutionmatrix_init(void)
{
  using namespace Fog;

  IntConvolutionMatrix::_dnull.init();
  return ERR_OK;
}

FOG_INIT_DECLARE void fog_convolutionmatrix_shutdown(void)
{
  using namespace Fog;

  IntConvolutionMatrix::_dnull.destroy();
}
