// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Global/Private.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/G2d/Tools/Matrix.h>

namespace Fog {

// ============================================================================
// [Fog::Matrix - Global]
// ============================================================================

static Static<MatrixDataF> MatrixF_dEmpty;
static Static<MatrixDataD> MatrixD_dEmpty;

static Static<MatrixF> MatrixF_oEmpty;
static Static<MatrixD> MatrixD_oEmpty;

template<typename NumT>
FOG_STATIC_INLINE_T NumT_(MatrixData)* MatrixT_getDEmpty() { return NULL; }

template<>
FOG_STATIC_INLINE_T MatrixDataF* MatrixT_getDEmpty<float>() { return &MatrixF_dEmpty; }

template<>
FOG_STATIC_INLINE_T MatrixDataD* MatrixT_getDEmpty<double>() { return &MatrixD_dEmpty; }

// ============================================================================
// [Fog::Matrix - Construction / Destruction]
// ============================================================================

template<typename NumT>
static void FOG_CDECL MatrixT_ctor(NumT_(Matrix)* self)
{
  self->_d = MatrixT_getDEmpty<NumT>()->addRef();
}

template<typename NumT>
static void FOG_CDECL MatrixT_ctorCopy(NumT_(Matrix)* self, const NumT_(Matrix)* other)
{
  self->_d = other->_d->addRef();
}

template<typename NumT>
static void FOG_CDECL MatrixT_ctorCreate(NumT_(Matrix)* self, const SizeI* size, const NumT* data)
{
  NumT_(MatrixData)* d = NumI_(Matrix)::_dCreate(*size, data);

  if (FOG_IS_NULL(d))
    d = MatrixT_getDEmpty<NumT>()->addRef();

  self->_d = d;
}

template<typename NumT>
static void FOG_CDECL MatrixT_dtor(NumT_(Matrix)* self)
{
  self->_d->release();
}

// ============================================================================
// [Fog::Matrix - Sharing]
// ============================================================================

template<typename NumT>
static err_t FOG_CDECL MatrixT_detach(NumT_(Matrix)* self)
{
  NumT_(MatrixData)* d = self->_d;

  if (d->reference.get() == 1 || d->isEmpty())
    return ERR_OK;

  d = NumI_(Matrix)::_dCreate(d->size, d->data);
  if (FOG_IS_NULL(d))
    return ERR_RT_OUT_OF_MEMORY;

  atomicPtrXchg(&self->_d, d)->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::Matrix - Create]
// ============================================================================

template<typename NumT>
static err_t FOG_CDECL MatrixT_create(NumT_(Matrix)* self, const SizeI* size, const NumT* data)
{
  if (size->w <= 0 || size->h <= 0)
  {
    self->reset();

    if (size->w < 0 || size->h < 0)
      return ERR_RT_INVALID_ARGUMENT;
    else
      return ERR_OK;
  }

  NumT_(MatrixData)* d = self->_d;

  size_t oldArea = (size_t)d->size.w * (size_t)d->size.h;
  size_t newArea = (size_t)size->w * (size_t)size->h;

  // Arithmetic overflow.
  if (sizeof(size_t) == sizeof(int) && (newArea / size->h != size->w))
    return ERR_RT_OUT_OF_MEMORY;

  if (oldArea == newArea && d->reference.get() == 1)
  {
    d->size = *size;
    if (data)
      MemOps::copy(d->data, data, newArea * sizeof(float));
    return ERR_OK;
  }

  NumT_(MatrixData)* newd = NumI_(Matrix)::_dCreate(*size, data);
  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  atomicPtrXchg(&self->_d, newd)->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::Matrix - Rezize]
// ============================================================================

template<typename NumT>
static err_t FOG_CDECL MatrixT_resize(NumT_(Matrix)* self, const SizeI* size, NumT value)
{
  NumT_(MatrixData)* d = self->_d;

  if (d->size == *size)
    return ERR_OK;

  if (size->w < 0 || size->h < 0)
    return ERR_RT_INVALID_ARGUMENT;

  if (size->w == 0 || size->h == 0)
  {
    self->reset();
    return ERR_OK;
  }

  NumT_(MatrixData)* newd = NumI_(Matrix)::_dCreate(*size);
  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  int x, y;

  int copyw = Math::min(d->size.w, size->w);
  int copyh = Math::min(d->size.h, size->h);
  int zerow = size->w - copyw;

  NumT* nData = newd->data;
  const NumT* oRow = d->data;

  // Copy/Set matrix rows.
  for (y = copyh; y != 0; y--, oRow += d->size.w)
  {
    const NumT* oData = oRow;

    for (x = copyw; x; x--, nData++, oData++) nData[0] = oData[0];
    for (x = zerow; x; x--, nData++         ) nData[0] = value;
  }

  // Set remaining rows.
  for (y = (size->h - copyh) * size->w; y; y--, nData++)
  {
    nData[0] = value;
  }

  atomicPtrXchg(&self->_d, newd)->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::Matrix - Accessors]
// ============================================================================

template<typename NumT>
static NumT FOG_CDECL MatrixT_getCell(const NumT_(Matrix)* self, int x, int y)
{
  NumT_(MatrixData)* d = self->_d;

  if ((uint)x >= (uint)d->size.w ||
      (uint)y >= (uint)d->size.h)
  {
    return Math::getQNanT<NumT>();
  }

  return d->data[y * d->size.w + x];
}

template<typename NumT>
static err_t FOG_CDECL MatrixT_setCell(NumT_(Matrix)* self, int x, int y, NumT value)
{
  NumT_(MatrixData)* d = self->_d;

  if ((uint)x >= (uint)d->size.w ||
      (uint)y >= (uint)d->size.h)
  {
    return ERR_RT_INVALID_ARGUMENT;
  }

  if (d->reference.get() > 1)
  {
    FOG_RETURN_ON_ERROR(self->_detach());
    d = self->_d;
  }

  d->data[y * d->size.w + x] = value;
  return ERR_OK;
}

template<typename NumT>
static err_t FOG_CDECL MatrixT_fill(NumT_(Matrix)* self, const RectI* rect, NumT value)
{
  NumT_(MatrixData)* d = self->_d;

  int w = d->size.w;
  int h = d->size.h;

  int x0 = Math::max<int>(rect->getX0(), 0);
  int y0 = Math::max<int>(rect->getY0(), 0);

  int x1 = Math::min<int>(rect->getX1(), w);
  int y1 = Math::min<int>(rect->getY1(), h);

  if (x0 >= x1 || y0 >= y1)
    return ERR_RT_INVALID_ARGUMENT;

  if (d->reference.get() > 1)
  {
    FOG_RETURN_ON_ERROR(self->_detach());
    d = self->_d;
  }

  NumT* p = &d->data[y0 * w + x0];

  int fw = x1 - x0;
  int fh = y1 - y0;

  for (int y = fh; y; y--, p += w)
  {
    for (int x = 0; x < fw; x++) p[x] = value;
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Matrix - Reset]
// ============================================================================

template<typename NumT>
static void FOG_CDECL MatrixT_reset(NumT_(Matrix)* self)
{
  atomicPtrXchg(&self->_d, MatrixT_getDEmpty<NumT>()->addRef())->release();
}

// ============================================================================
// [Fog::Matrix - Copy]
// ============================================================================

template<typename NumT>
static void FOG_CDECL MatrixT_copy(NumT_(Matrix)* self, const NumT_(Matrix)* other)
{
  atomicPtrXchg(&self->_d, other->_d->addRef())->release();
}

// ============================================================================
// [Fog::Matrix - Equality]
// ============================================================================

template<typename NumT>
static bool FOG_CDECL MatrixT_eq(const NumT_(Matrix)* a, const NumT_(Matrix)* b)
{
  NumT_(MatrixData)* a_d = a->_d;
  NumT_(MatrixData)* b_d = b->_d;

  if (a_d == b_d)
    return true;

  if (a_d->size != b_d->size)
    return false;

  const NumT* a_data = a_d->data;
  const NumT* b_data = b_d->data;

  size_t area = (size_t)(uint)a_d->size.w * (size_t)(uint)b_d->size.h;
  for (size_t i = 0; i < area; i++)
  {
    if (a_data[i] != b_data[i])
      return false;
  }

  return true;
}

// ============================================================================
// [Fog::Matrix - MatrixData]
// ============================================================================

template<typename NumT>
static NumT_(MatrixData)* MatrixT_dCreate(const SizeI* size, const NumT* data)
{
  int w = size->w;
  int h = size->h;

  if (w <= 0 || h <= 0)
    return NULL;

  size_t area = (size_t)(uint)w * (size_t)(uint)h;
  if (area / (uint)h != w)
    return NULL;

  if (area > (SIZE_MAX - sizeof(NumT_(MatrixData))) / sizeof(NumT))
    return NULL;

  NumT_(MatrixData)* d = reinterpret_cast<NumT_(MatrixData)*>(
    MemMgr::alloc(NumI_(MatrixData)::getSizeOf(area)));

  if (FOG_IS_NULL(d))
    return NULL;

  d->reference.init(1);
  d->size.set(w, h);

  if (data != NULL)
    MemOps::copy(d->data, data, area * sizeof(NumT));

  return d;
}

template<typename NumT>
static void MatrixT_dFree(NumT_(MatrixData)* d)
{
  if ((d->vType & VAR_FLAG_STATIC) == 0)
    MemMgr::free(d);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Matrix_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  _api.matrixf.ctor = MatrixT_ctor<float>;
  _api.matrixf.ctorCopy = MatrixT_ctorCopy<float>;
  _api.matrixf.ctorCreate = MatrixT_ctorCreate<float>;
  _api.matrixf.dtor = MatrixT_dtor<float>;
  _api.matrixf.detach = MatrixT_detach<float>;
  _api.matrixf.create = MatrixT_create<float>;
  _api.matrixf.resize = MatrixT_resize<float>;
  _api.matrixf.getCell = MatrixT_getCell<float>;
  _api.matrixf.setCell = MatrixT_setCell<float>;
  _api.matrixf.fill = MatrixT_fill<float>;
  _api.matrixf.reset = MatrixT_reset<float>;
  _api.matrixf.copy = MatrixT_copy<float>;
  _api.matrixf.eq = MatrixT_eq<float>;

  _api.matrixf.dCreate = MatrixT_dCreate<float>;
  _api.matrixf.dFree = MatrixT_dFree<float>;

  _api.matrixd.ctor = MatrixT_ctor<double>;
  _api.matrixd.ctorCopy = MatrixT_ctorCopy<double>;
  _api.matrixd.ctorCreate = MatrixT_ctorCreate<double>;
  _api.matrixd.dtor = MatrixT_dtor<double>;
  _api.matrixd.detach = MatrixT_detach<double>;
  _api.matrixd.create = MatrixT_create<double>;
  _api.matrixd.resize = MatrixT_resize<double>;
  _api.matrixd.getCell = MatrixT_getCell<double>;
  _api.matrixd.setCell = MatrixT_setCell<double>;
  _api.matrixd.fill = MatrixT_fill<double>;
  _api.matrixd.reset = MatrixT_reset<double>;
  _api.matrixd.copy = MatrixT_copy<double>;
  _api.matrixd.eq = MatrixT_eq<double>;

  _api.matrixd.dCreate = MatrixT_dCreate<double>;
  _api.matrixd.dFree = MatrixT_dFree<double>;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  MatrixDataF* df = &MatrixF_dEmpty;
  MatrixDataD* dd = &MatrixD_dEmpty;

  df->reference.init(1);
  df->vType = VAR_TYPE_MATRIXF | VAR_FLAG_NONE;

  dd->reference.init(1);
  dd->vType = VAR_TYPE_MATRIXF | VAR_FLAG_NONE;

  _api.matrixf.oEmpty = MatrixF_oEmpty.initCustom1(df);
  _api.matrixd.oEmpty = MatrixD_oEmpty.initCustom1(dd);
}

} // Fog namespace
