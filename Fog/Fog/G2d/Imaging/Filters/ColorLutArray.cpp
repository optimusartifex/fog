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
#include <Fog/G2d/Imaging/Filters/ColorLutArray.h>
#include <Fog/G2d/Imaging/Filters/ComponentTransferFunction.h>

namespace Fog {

// ============================================================================
// [Fog::ColorLutArray - Global]
// ============================================================================

static Static<ColorLutArrayData> ColorLutArray_dIdentity;
static Static<ColorLutArray> ColorLutArray_oIdentity;

// ============================================================================
// [Fog::ColorLutArray - Construction / Destruction]
// ============================================================================

static void FOG_CDECL ColorLutArray_ctor(ColorLutArray* self)
{
  self->_d = ColorLutArray_dIdentity->addRef();
}

static void FOG_CDECL ColorLutArray_ctorCopy(ColorLutArray* self, const ColorLutArray* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL ColorLutArray_dtor(ColorLutArray* self)
{
  self->_d->release();
}

// ============================================================================
// [Fog::ColorLutArray - Sharing]
// ============================================================================

static err_t FOG_CDECL ColorLutArray_detach(ColorLutArray* self)
{
  ColorLutArrayData* d;

  if (d->reference.get() == 1)
    return ERR_OK;

  ColorLutArrayData* newd = _api.colorlutarray_dCreate();

  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  MemOps::copy(newd->data, d->data, 256);

  atomicPtrXchg(&self->_d, newd)->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::ColorLutArray - Accessors]
// ============================================================================

static err_t FOG_CDECL ColorLutArray_setAt(ColorLutArray* self, size_t index, uint8_t value)
{
  FOG_ASSERT(index < 256);

  FOG_RETURN_ON_ERROR(self->detach());

  self->_d->data[index] = value;
  return ERR_OK;
}

static err_t FOG_CDECL ColorLutArray_setFromFunction(ColorLutArray* self, const ComponentTransferFunction* func)
{
  if (func->resultsInIdentity())
  {
    self->reset();
    return ERR_OK;
  }

  ColorLutArrayData* d = self->_d;
  if (d->reference.get() != 1)
  {
    d = _api.colorlutarray_dCreate();
    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&self->_d, d)->release();
  }

  const ComponentTransferFunctionData* fd = func->_d;
  uint8_t* data = d->data;

  switch (fd->functionType)
  {
    // C' = C
    case COMPONENT_TRANSFER_FUNCTION_IDENTITY:
    {
      // Should be catched by ComponentTransferFunction::resultsInIdentity().
      FOG_ASSERT_NOT_REACHED();
    }

    // k/n <= C < (k+1)/n
    // C' = v[k] + (C - k/n)*n * (v[k+1] - v[k])
    case COMPONENT_TRANSFER_FUNCTION_TABLE:
    {
      size_t tableLength = fd->table().getLength();
      const float* tableData = fd->table().getData();

      // Should be catched by ComponentTransferFunction::resultsInIdentity().
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
    case COMPONENT_TRANSFER_FUNCTION_DISCRETE:
    {
      size_t tableLength = fd->table().getLength();
      const float* tableData = fd->table().getData();

      // Should be catched by ComponentTransferFunction::resultsInIdentity().
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
    case COMPONENT_TRANSFER_FUNCTION_LINEAR:
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
    case COMPONENT_TRANSFER_FUNCTION_GAMMA:
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
// [Fog::ColorLutArray - Reset]
// ============================================================================

static void FOG_CDECL ColorLutArray_reset(ColorLutArray* self)
{
  atomicPtrXchg(&self->_d, ColorLutArray_dIdentity->addRef())->release();
}

// ============================================================================
// [Fog::ColorLutArray - Copy]
// ============================================================================

static err_t FOG_CDECL ColorLutArray_copy(ColorLutArray* self, const ColorLutArray* other)
{
  atomicPtrXchg(&self->_d, other->_d->addRef())->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::ColorLutArray - Eq]
// ============================================================================

static bool FOG_CDECL ColorLutArray_eq(const ColorLutArray* a, const ColorLutArray* b)
{
  return MemOps::eq(a->_d->data, b->_d->data, 256);
}

// ============================================================================
// [Fog::ColorLutArray - ColorLutArrayData]
// ============================================================================

static ColorLutArrayData* FOG_CDECL ColorLutArray_dCreate(void)
{
  ColorLutArrayData* d = reinterpret_cast<ColorLutArrayData*>(MemMgr::alloc(sizeof(ColorLutArrayData)));

  if (FOG_IS_NULL(d))
    return NULL;

  d->reference.init(1);

  return d;
}

static void FOG_CDECL ColorLutArray_dFree(ColorLutArrayData* d)
{
  MemMgr::free(d);
}

// ============================================================================
// [Fog::ColorLutArray - Helpers]
// ============================================================================

static void FOG_CDECL ColorLutArray_setIdentity(uint8_t* _data)
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

static bool FOG_CDECL ColorLutArray_isIdentity(const uint8_t* _data)
{
  if (_data == ColorLutArray_dIdentity->data)
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

FOG_NO_EXPORT void ColorLutArray_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  _api.colorlutarray_ctor = ColorLutArray_ctor;
  _api.colorlutarray_ctorCopy = ColorLutArray_ctorCopy;
  _api.colorlutarray_dtor = ColorLutArray_dtor;

  _api.colorlutarray_setAt = ColorLutArray_setAt;
  _api.colorlutarray_setFromFunction = ColorLutArray_setFromFunction;
  _api.colorlutarray_reset = ColorLutArray_reset;
  _api.colorlutarray_copy = ColorLutArray_copy;
  _api.colorlutarray_eq = ColorLutArray_eq;

  _api.colorlutarray_dCreate = ColorLutArray_dCreate;
  _api.colorlutarray_dFree = ColorLutArray_dFree;

  _api.colorlutarray_setIdentity = ColorLutArray_setIdentity;
  _api.colorlutarray_isIdentity = ColorLutArray_isIdentity;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  ColorLutArrayData* d = &ColorLutArray_dIdentity;

  d->reference.init(1);
  ColorLutArray_setIdentity(d->data);

  _api.colorlutarray_oIdentity = ColorLutArray_oIdentity.initCustom1(d);
}

} // Fog namespace
