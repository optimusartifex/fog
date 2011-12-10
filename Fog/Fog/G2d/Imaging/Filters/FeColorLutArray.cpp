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
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Imaging/Filters/FeColorLutArray.h>
#include <Fog/G2d/Imaging/Filters/FeComponentFunction.h>

namespace Fog {

// ============================================================================
// [Fog::FeColorLutArray - Global]
// ============================================================================

static Static<FeColorLutArrayData> FeColorLutArray_dIdentity;
static Static<FeColorLutArray> FeColorLutArray_oIdentity;

// ============================================================================
// [Fog::FeColorLutArray - Construction / Destruction]
// ============================================================================

static void FOG_CDECL FeColorLutArray_ctor(FeColorLutArray* self)
{
  self->_d = FeColorLutArray_dIdentity->addRef();
}

static void FOG_CDECL FeColorLutArray_ctorCopy(FeColorLutArray* self, const FeColorLutArray* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL FeColorLutArray_dtor(FeColorLutArray* self)
{
  FeColorLutArrayData* d = self->_d;

  if (d != NULL)
    d->release();
}

// ============================================================================
// [Fog::FeColorLutArray - Sharing]
// ============================================================================

static err_t FOG_CDECL FeColorLutArray_detach(FeColorLutArray* self)
{
  FeColorLutArrayData* d = self->_d;

  if (d->reference.get() == 1)
    return ERR_OK;

  FeColorLutArrayData* newd = fog_api.fecolorlutarray_dCreate();

  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  MemOps::copy(newd->data, d->data, 256);

  atomicPtrXchg(&self->_d, newd)->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::FeColorLutArray - Accessors]
// ============================================================================

static err_t FOG_CDECL FeColorLutArray_setAt(FeColorLutArray* self, size_t index, uint8_t value)
{
  FOG_ASSERT(index < 256);

  FOG_RETURN_ON_ERROR(self->detach());

  self->_d->data[index] = value;
  return ERR_OK;
}

static err_t FOG_CDECL FeColorLutArray_setFromComponentFunction(FeColorLutArray* self, const FeComponentFunction* func)
{
  if (func->resultsInIdentity())
  {
    self->reset();
    return ERR_OK;
  }

  FeColorLutArrayData* d = self->_d;
  if (d->reference.get() != 1)
  {
    d = fog_api.fecolorlutarray_dCreate();
    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&self->_d, d)->release();
  }

  const FeComponentFunctionData* fd = func->_d;
  uint8_t* data = d->data;

  switch (fd->functionType)
  {
    // C' = C
    case FE_COMPONENT_FUNCTION_IDENTITY:
    {
      // Should be catched by FeComponentFunction::resultsInIdentity().
      FOG_ASSERT_NOT_REACHED();
    }

    // k/n <= C < (k+1)/n
    // C' = v[k] + (C - k/n)*n * (v[k+1] - v[k])
    case FE_COMPONENT_FUNCTION_TABLE:
    {
      size_t tableLength = fd->table().getLength();
      const float* tableData = fd->table().getData();

      // Should be catched by FeComponentFunction::resultsInIdentity().
      FOG_ASSERT(tableLength != 0);

      // Constant fill.
      if (tableLength == 1)
      {
        MemOps::set(data, (uint8_t)Math::uroundToByte255(Math::bound(tableData[0], 0.0f, 1.0f)), 256);
        return ERR_OK;
      }

      tableLength--;

      float kFrom;
      float kTo = tableData[0];

      size_t iFrom;
      size_t iTo = 0;

      for (size_t k = 0; k < tableLength; k++)
      {
        kFrom = kTo;
        kTo = tableData[k + 1];

        iFrom = iTo;
        iTo = ((k + 1) * 255U) / tableLength;

        if (iFrom != iTo)
        {
          float pos = kFrom;
          float step = (kTo - kFrom) / float((ssize_t)(iTo - iFrom));

          do {
            data[iFrom] = (uint8_t)Math::uroundToByte255(Math::bound(pos, 0.0f, 1.0f));
            pos += step;
          } while (++iFrom < iTo);
        }

        data[iTo] = (uint8_t)Math::uroundToByte255(Math::bound(kTo, 0.0f, 1.0f));
      }

      break;
    }

    // k/n <= C < (k+1)/n
    // C' = v[k]
    case FE_COMPONENT_FUNCTION_DISCRETE:
    {
      size_t tableLength = fd->table().getLength();
      const float* tableData = fd->table().getData();

      // Should be catched by FeComponentFunction::resultsInIdentity().
      FOG_ASSERT(tableLength != 0);

      // Constant fill.
      if (tableLength == 1)
      {
        MemOps::set(data, (uint8_t)Math::uroundToByte255(Math::bound(tableData[0], 0.0f, 1.0f)), 256);
        return ERR_OK;
      }

      tableLength--;

      size_t iFrom;
      size_t iTo = 0;

      for (size_t k = 0; k < tableLength; k++)
      {
        uint8_t value = (uint8_t)Math::uroundToByte255(Math::bound(tableData[k], 0.0f, 1.0f));

        iFrom = iTo;
        iTo = ((k + 1) * 255U) / tableLength;

        while (iFrom <= iTo)
        {
          data[iFrom] = value;
          iFrom++;
        }
      }

      break;
    }

    // C' = slope * C + intercept
    case FE_COMPONENT_FUNCTION_LINEAR:
    {
      float slope = fd->linear().getSlope();
      float intercept = fd->linear().getIntercept();

      float c = 0.0f;
      float step = float(MATH_1_DIV_255);

      for (uint32_t i = 0; i < 256; i++, c += step)
      {
        float value = Math::bound(slope * c + intercept, 0.0f, 1.0f);
        data[i] = (uint8_t)Math::uroundToByte255(value);
      }

      break;
    }

    // C' = amplitude * pow(C, exponent) + offset
    case FE_COMPONENT_FUNCTION_GAMMA:
    {
      float amplitude = fd->gamma().getAmplitude();
      float exponent = fd->gamma().getExponent();
      float offset = fd->gamma().getOffset();

      float c = 0.0f;
      float step = float(MATH_1_DIV_255);

      for (uint32_t i = 0; i < 256; i++, c += step)
      {
        float value = Math::bound(amplitude * Math::pow(c, exponent) + offset, 0.0f, 1.0f);
        data[i] = (uint8_t)Math::uroundToByte255(value);
      }

      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::FeColorLutArray - Reset]
// ============================================================================

static void FOG_CDECL FeColorLutArray_reset(FeColorLutArray* self)
{
  atomicPtrXchg(&self->_d, FeColorLutArray_dIdentity->addRef())->release();
}

// ============================================================================
// [Fog::FeColorLutArray - Copy]
// ============================================================================

static err_t FOG_CDECL FeColorLutArray_copy(FeColorLutArray* self, const FeColorLutArray* other)
{
  atomicPtrXchg(&self->_d, other->_d->addRef())->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::FeColorLutArray - Eq]
// ============================================================================

static bool FOG_CDECL FeColorLutArray_eq(const FeColorLutArray* a, const FeColorLutArray* b)
{
  return MemOps::eq(a->_d->data, b->_d->data, 256);
}

// ============================================================================
// [Fog::FeColorLutArray - FeColorLutArrayData]
// ============================================================================

static FeColorLutArrayData* FOG_CDECL FeColorLutArray_dCreate(void)
{
  FeColorLutArrayData* d = reinterpret_cast<FeColorLutArrayData*>(MemMgr::alloc(sizeof(FeColorLutArrayData)));

  if (FOG_IS_NULL(d))
    return NULL;

  d->reference.init(1);

  return d;
}

static void FOG_CDECL FeColorLutArray_dFree(FeColorLutArrayData* d)
{
  MemMgr::free(d);
}

// ============================================================================
// [Fog::FeColorLutArray - Helpers]
// ============================================================================

static void FOG_CDECL FeColorLutArray_setIdentity(uint8_t* _data)
{
  if (sizeof(size_t) == 4 || ((size_t)_data & 0x7) != 0)
  {
    uint32_t* data = reinterpret_cast<uint32_t*>(_data);
    uint32_t c = FOG_MAKE_UINT32_SEQ(0x00, 0x01, 0x02, 0x03);

    for (uint32_t i = 0; i < 256 / 4; i++)
    {
      data[i] = c;
      c += 0x04040404U;
    }
  }
  else
  {
    uint64_t* data = reinterpret_cast<uint64_t*>(_data);
    uint64_t c = FOG_MAKE_UINT64_SEQ(0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07);

    for (uint32_t i = 0; i < 256 / 8; i++)
    {
      data[i] = c;
      c += FOG_UINT64_C(0x0808080808080808);
    }
  }
}

static bool FOG_CDECL FeColorLutArray_isIdentity(const uint8_t* _data)
{
  if (_data == FeColorLutArray_dIdentity->data)
    return true;

  if (sizeof(size_t) == 4 || ((size_t)_data & 0x7) != 0)
  {
    const uint32_t* data = reinterpret_cast<const uint32_t*>(_data);
    uint32_t c = FOG_MAKE_UINT32_SEQ(0x00, 0x01, 0x02, 0x03);

    for (uint32_t i = 0; i < 256 / 4; i++)
    {
      if (data[i] != c)
        return false;
      c += 0x04040404U;
    }
  }
  else
  {
    const uint64_t* data = reinterpret_cast<const uint64_t*>(_data);
    uint64_t c = FOG_MAKE_UINT64_SEQ(0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07);

    for (uint32_t i = 0; i < 256 / 8; i++)
    {
      if (data[i] != c)
        return false;
      c += FOG_UINT64_C(0x0808080808080808);
    }
  }

  return true;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void FeColorLutArray_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.fecolorlutarray_ctor = FeColorLutArray_ctor;
  fog_api.fecolorlutarray_ctorCopy = FeColorLutArray_ctorCopy;
  fog_api.fecolorlutarray_dtor = FeColorLutArray_dtor;
  
  fog_api.fecolorlutarray_detach = FeColorLutArray_detach;
  fog_api.fecolorlutarray_setAt = FeColorLutArray_setAt;
  fog_api.fecolorlutarray_setFromComponentFunction = FeColorLutArray_setFromComponentFunction;
  fog_api.fecolorlutarray_reset = FeColorLutArray_reset;
  fog_api.fecolorlutarray_copy = FeColorLutArray_copy;
  fog_api.fecolorlutarray_eq = FeColorLutArray_eq;

  fog_api.fecolorlutarray_dCreate = FeColorLutArray_dCreate;
  fog_api.fecolorlutarray_dFree = FeColorLutArray_dFree;

  fog_api.fecolorlutarray_setIdentity = FeColorLutArray_setIdentity;
  fog_api.fecolorlutarray_isIdentity = FeColorLutArray_isIdentity;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  FeColorLutArrayData* d = &FeColorLutArray_dIdentity;

  d->reference.init(1);
  FeColorLutArray_setIdentity(d->data);

  fog_api.fecolorlutarray_oIdentity = FeColorLutArray_oIdentity.initCustom1(d);
}

} // Fog namespace
