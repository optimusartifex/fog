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
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/G2d/Imaging/Filters/FeComponentTransfer.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {

// ============================================================================
// [Fog::FeComponentFunction - Global]
// ============================================================================

static Static<FeComponentFunctionData> FeComponentFunction_dIdentity;
static Static<FeComponentFunction> FeComponentFunction_oIdentity;

// ============================================================================
// [Fog::FeComponentFunction - Construction / Destruction]
// ============================================================================

static void FOG_CDECL FeComponentFunction_ctor(FeComponentFunction* self)
{
  self->_d = FeComponentFunction_dIdentity->addRef();
}

static void FOG_CDECL FeComponentFunction_ctorCopy(FeComponentFunction* self, const FeComponentFunction* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL FeComponentFunction_dtor(FeComponentFunction* self)
{
  FeComponentFunctionData* d = self->_d;

  if (d != NULL)
    d->release();
}

// ============================================================================
// [Fog::FeComponentFunction - Accessors]
// ============================================================================

static bool FOG_CDECL FeComponentFunction_resultsInIdentity(const FeComponentFunction* self)
{
  const FeComponentFunctionData* d = self->_d;

  switch (d->functionType)
  {
    // C' = C
    case FE_COMPONENT_FUNCTION_IDENTITY:
    {
      return true;
    }

    // k/n <= C < (k+1)/n
    // C' = v[k] + (C - k/n)*n * (v[k+1] - v[k])
    case FE_COMPONENT_FUNCTION_TABLE:
    {
      size_t tableLength = d->table().getLength();
      const float* tableData = d->table().getData();

      // No items means identity.
      if (tableLength == 0)
        return true;

      // Constant fill.
      if (tableLength == 1)
        return false;

      // Check whether the table is in the closed interval [0, 1].
      if (!Math::isFuzzyZero(tableData[0]) || !Math::isFuzzyOne(tableData[1]))
        return false;

      tableLength--;

      // Use double to minimize FPU errors.
      double step = 1.0 / double((ssize_t)tableLength);
      double pos = step;

      for (size_t i = 1; i < tableLength; i++)
      {
        if (!Math::isFuzzyEq(tableData[i], float(pos)))
          return false;
        pos += step;
      }

      return true;
    }

    // k/n <= C < (k+1)/n
    // C' = v[k]
    case FE_COMPONENT_FUNCTION_DISCRETE:
    {
      return false;
    }

    // C' = slope * C + intercept
    case FE_COMPONENT_FUNCTION_LINEAR:
    {
      return Math::isFuzzyOne (d->linear().getSlope()    ) &&
             Math::isFuzzyZero(d->linear().getIntercept()) ;
    }

    // C' = amplitude * pow(C, exponent) + offset
    case FE_COMPONENT_FUNCTION_GAMMA:
    {
      return Math::isFuzzyOne (d->gamma().getAmplitude() ) &&
             Math::isFuzzyOne (d->gamma().getExponent()  ) &&
             Math::isFuzzyZero(d->gamma().getOffset()    ) ;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return false;
}

static err_t FOG_CDECL FeComponentFunction_getData(const FeComponentFunction* self, uint32_t functionType, void* functionData)
{
  const FeComponentFunctionData* d = self->_d;

  if (functionType >= FE_COMPONENT_FUNCTION_COUNT)
    return ERR_RT_INVALID_ARGUMENT;

  if (functionType != d->functionType)
    return ERR_RT_INVALID_STATE;

  switch (functionType)
  {
    case FE_COMPONENT_FUNCTION_IDENTITY:
      // There is no identity data, we just return ERR_OK to inform that
      // data and function-type match. We really shouldn't get into this case.
      return ERR_OK;

    case FE_COMPONENT_FUNCTION_TABLE:
    case FE_COMPONENT_FUNCTION_DISCRETE:
      return reinterpret_cast< List<float>* >(functionData)->setList(d->table());

    case FE_COMPONENT_FUNCTION_LINEAR:
      *reinterpret_cast<FeComponentFunctionLinear*>(functionData) = d->linear();
      return ERR_OK;

    case FE_COMPONENT_FUNCTION_GAMMA:
      *reinterpret_cast<FeComponentFunctionGamma*>(functionData) = d->gamma();
      return ERR_OK;

    default:
      return ERR_RT_INVALID_STATE;
  }
}

static err_t FOG_CDECL FeComponentFunction_setData(FeComponentFunction* self, uint32_t functionType, const void* functionData)
{
  if (functionType >= FE_COMPONENT_FUNCTION_COUNT)
    return ERR_RT_INVALID_ARGUMENT;

  if (functionType == FE_COMPONENT_FUNCTION_IDENTITY)
  {
    fog_api.fecomponentfunction_reset(self);
    return ERR_OK;
  }

  FeComponentFunctionData* d = self->_d;

  if (d->reference.get() != 1)
  {
    FeComponentFunctionData* newd = fog_api.fecomponentfunction_dCreate(functionType, functionData);
    if (FOG_IS_NULL(newd))
      return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&self->_d, newd)->release();
    return ERR_OK;
  }
  else
  {
    switch (d->functionType)
    {
      case FE_COMPONENT_FUNCTION_TABLE:
      case FE_COMPONENT_FUNCTION_DISCRETE:
        d->table.destroy();
        break;
    }

    d->functionType = functionType;
    switch (functionType)
    {
      case FE_COMPONENT_FUNCTION_TABLE:
      case FE_COMPONENT_FUNCTION_DISCRETE:
        d->table.initCustom1(*reinterpret_cast< const List<float>* >(functionData));
        break;

      case FE_COMPONENT_FUNCTION_LINEAR:
        d->linear.initCustom1(*reinterpret_cast< const FeComponentFunctionLinear* >(functionData));
        break;

      case FE_COMPONENT_FUNCTION_GAMMA:
        d->gamma.initCustom1(*reinterpret_cast< const FeComponentFunctionGamma* >(functionData));
        break;

      default:
        FOG_ASSERT_NOT_REACHED();
    }

    return ERR_OK;
  }
}

// ============================================================================
// [Fog::FeComponentFunction - Reset]
// ============================================================================

static void FOG_CDECL FeComponentFunction_reset(FeComponentFunction* self)
{
  atomicPtrXchg(&self->_d, FeComponentFunction_dIdentity->addRef())->release();
}

// ============================================================================
// [Fog::FeComponentFunction - Copy]
// ============================================================================

static err_t FOG_CDECL FeComponentFunction_copy(FeComponentFunction* self, const FeComponentFunction* other)
{
  atomicPtrXchg(&self->_d, other->_d->addRef())->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::FeComponentFunction - Eq]
// ============================================================================

static bool FOG_CDECL FeComponentFunction_eq(const FeComponentFunction* a, const FeComponentFunction* b)
{
  const FeComponentFunctionData* a_d = a->_d;
  const FeComponentFunctionData* b_d = b->_d;

  if (a_d == b_d)
    return true;

  uint32_t functionType = a_d->functionType;
  if (functionType != b_d->functionType)
    return false;

  switch (functionType)
  {
    case FE_COMPONENT_FUNCTION_IDENTITY:
      return true;

    case FE_COMPONENT_FUNCTION_TABLE:
    case FE_COMPONENT_FUNCTION_DISCRETE:
      return a_d->table() == b_d->table();

    case FE_COMPONENT_FUNCTION_LINEAR:
      return a_d->linear() == b_d->linear();

    case FE_COMPONENT_FUNCTION_GAMMA:
      return a_d->gamma() == b_d->gamma();

    default:
      return false;
  }
}

// ============================================================================
// [Fog::FeComponentFunction - FeComponentFunctionData]
// ============================================================================

static FeComponentFunctionData* FOG_CDECL FeComponentFunction_dCreate(uint32_t functionType, const void* functionData)
{
  if (functionType == FE_COMPONENT_FUNCTION_IDENTITY)
    return FeComponentFunction_dIdentity->addRef();

  FeComponentFunctionData* d = reinterpret_cast<FeComponentFunctionData*>(
    MemMgr::alloc(sizeof(FeComponentFunctionData)));

  if (FOG_IS_NULL(d))
    return NULL;

  d->reference.init(1);
  d->vType = 0; // TODO: Var
  d->functionType = functionType;

  switch (functionType)
  {
    case FE_COMPONENT_FUNCTION_TABLE:
    case FE_COMPONENT_FUNCTION_DISCRETE:
      d->table.initCustom1(*reinterpret_cast< const List<float>* >(functionData));
      break;

    case FE_COMPONENT_FUNCTION_LINEAR:
      d->linear.initCustom1(*reinterpret_cast< const FeComponentFunctionLinear* >(functionData));
      break;

    case FE_COMPONENT_FUNCTION_GAMMA:
      d->gamma.initCustom1(*reinterpret_cast< const FeComponentFunctionGamma* >(functionData));
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return d;
}

static void FOG_CDECL FeComponentFunction_dFree(FeComponentFunctionData* d)
{
  switch (d->functionType)
  {
    case FE_COMPONENT_FUNCTION_TABLE:
    case FE_COMPONENT_FUNCTION_DISCRETE:
      d->table.destroy();
      break;

    default:
      break;
  }

  MemMgr::free(d);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void FeComponentFunction_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.fecomponentfunction_ctor = FeComponentFunction_ctor;
  fog_api.fecomponentfunction_ctorCopy = FeComponentFunction_ctorCopy;
  fog_api.fecomponentfunction_dtor = FeComponentFunction_dtor;

  fog_api.fecomponentfunction_resultsInIdentity = FeComponentFunction_resultsInIdentity;
  fog_api.fecomponentfunction_getData = FeComponentFunction_getData;
  fog_api.fecomponentfunction_setData = FeComponentFunction_setData;
  fog_api.fecomponentfunction_reset = FeComponentFunction_reset;

  fog_api.fecomponentfunction_copy = FeComponentFunction_copy;
  fog_api.fecomponentfunction_eq = FeComponentFunction_eq;

  fog_api.fecomponentfunction_dCreate = FeComponentFunction_dCreate;
  fog_api.fecomponentfunction_dFree = FeComponentFunction_dFree;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  FeComponentFunctionData* d = &FeComponentFunction_dIdentity;

  d->reference.init(1);
  d->vType = 0; // TODO: Var
  d->functionType = FE_COMPONENT_FUNCTION_IDENTITY;

  fog_api.fecomponentfunction_oIdentity = FeComponentFunction_oIdentity.initCustom1(d);
}

} // Fog namespace
