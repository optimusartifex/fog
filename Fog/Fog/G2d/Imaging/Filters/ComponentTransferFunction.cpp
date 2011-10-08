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
#include <Fog/G2d/Imaging/Filters/ComponentTransfer.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {

// ============================================================================
// [Fog::ComponentTransferFunction - Global]
// ============================================================================

static Static<ComponentTransferFunctionData> ComponentTransferFunction_dIdentity;
static Static<ComponentTransferFunction> ComponentTransferFunction_oIdentity;

// ============================================================================
// [Fog::ComponentTransferFunction - Construction / Destruction]
// ============================================================================

static void FOG_CDECL ComponentTransferFunction_ctor(ComponentTransferFunction* self)
{
  self->_d = ComponentTransferFunction_dIdentity->addRef();
}

static void FOG_CDECL ComponentTransferFunction_ctorCopy(ComponentTransferFunction* self, const ComponentTransferFunction* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL ComponentTransferFunction_dtor(ComponentTransferFunction* self)
{
  self->_d->release();
}

// ============================================================================
// [Fog::ComponentTransferFunction - Accessors]
// ============================================================================

static bool FOG_CDECL ComponentTransferFunction_resultsInIdentity(const ComponentTransferFunction* self)
{
  const ComponentTransferFunctionData* d = self->_d;

  switch (d->functionType)
  {
    // C' = C
    case COMPONENT_TRANSFER_FUNCTION_IDENTITY:
    {
      return true;
    }

    // k/n <= C < (k+1)/n
    // C' = v[k] + (C - k/n)*n * (v[k+1] - v[k])
    case COMPONENT_TRANSFER_FUNCTION_TABLE:
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
    case COMPONENT_TRANSFER_FUNCTION_DISCRETE:
    {
      return false;
    }

    // C' = slope * C + intercept
    case COMPONENT_TRANSFER_FUNCTION_LINEAR:
    {
      return Math::isFuzzyOne (d->linear().getSlope()    ) &&
             Math::isFuzzyZero(d->linear().getIntercept()) ;
    }

    // C' = amplitude * pow(C, exponent) + offset
    case COMPONENT_TRANSFER_FUNCTION_GAMMA:
    {
      return Math::isFuzzyOne (d->gamma().getAmplitude() ) &&
             Math::isFuzzyOne (d->gamma().getExponent()  ) &&
             Math::isFuzzyZero(d->gamma().getOffset()    ) ;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

static err_t FOG_CDECL ComponentTransferFunction_getData(const ComponentTransferFunction* self, uint32_t functionType, void* functionData)
{
  const ComponentTransferFunctionData* d = self->_d;

  if (functionType >= COMPONENT_TRANSFER_FUNCTION_COUNT)
    return ERR_RT_INVALID_ARGUMENT;

  if (functionType != d->functionType)
    return ERR_RT_INVALID_STATE;

  switch (functionType)
  {
    case COMPONENT_TRANSFER_FUNCTION_IDENTITY:
      // There is no identity data, we just return ERR_OK to inform that 
      // data and function-type match. We really shouldn't get into this case.
      return ERR_OK;

    case COMPONENT_TRANSFER_FUNCTION_TABLE:
    case COMPONENT_TRANSFER_FUNCTION_DISCRETE:
      return reinterpret_cast< List<float>* >(functionData)->setList(d->table());

    case COMPONENT_TRANSFER_FUNCTION_LINEAR:
      *reinterpret_cast<ComponentTransferFunctionLinear*>(functionData) = d->linear();
      return ERR_OK;

    case COMPONENT_TRANSFER_FUNCTION_GAMMA:
      *reinterpret_cast<ComponentTransferFunctionGamma*>(functionData) = d->gamma();
      return ERR_OK;

    default:
      return ERR_RT_INVALID_STATE;
  }
}

static err_t FOG_CDECL ComponentTransferFunction_setData(ComponentTransferFunction* self, uint32_t functionType, const void* functionData)
{
  if (functionType >= COMPONENT_TRANSFER_FUNCTION_COUNT)
    return ERR_RT_INVALID_ARGUMENT;

  if (functionType == COMPONENT_TRANSFER_FUNCTION_IDENTITY)
  {
    _api.componenttransferfunction_reset(self);
    return ERR_OK;
  }

  ComponentTransferFunctionData* d = self->_d;

  if (d->reference.get() != 1)
  {
    ComponentTransferFunctionData* newd = _api.componenttransferfunction_dCreate(functionType, functionData);
    if (FOG_IS_NULL(newd))
      return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&self->_d, newd)->release();
    return ERR_OK;
  }
  else
  {
    switch (d->functionType)
    {
      case COMPONENT_TRANSFER_FUNCTION_TABLE:
      case COMPONENT_TRANSFER_FUNCTION_DISCRETE:
        d->table.destroy();
        break;
    }

    d->functionType = functionType;
    switch (functionType)
    {
      case COMPONENT_TRANSFER_FUNCTION_TABLE:
      case COMPONENT_TRANSFER_FUNCTION_DISCRETE:
        d->table.initCustom1(*reinterpret_cast< const List<float>* >(functionData));
        break;

      case COMPONENT_TRANSFER_FUNCTION_LINEAR:
        d->linear.initCustom1(*reinterpret_cast< const ComponentTransferFunctionLinear* >(functionData));
        break;

      case COMPONENT_TRANSFER_FUNCTION_GAMMA:
        d->gamma.initCustom1(*reinterpret_cast< const ComponentTransferFunctionGamma* >(functionData));
        break;

      default:
        FOG_ASSERT_NOT_REACHED();
    }

    return ERR_OK;
  }
}

// ============================================================================
// [Fog::ComponentTransferFunction - Reset]
// ============================================================================

static void FOG_CDECL ComponentTransferFunction_reset(ComponentTransferFunction* self)
{
  atomicPtrXchg(&self->_d, ComponentTransferFunction_dIdentity->addRef())->release();
}

// ============================================================================
// [Fog::ComponentTransferFunction - Copy]
// ============================================================================

static err_t FOG_CDECL ComponentTransferFunction_copy(ComponentTransferFunction* self, const ComponentTransferFunction* other)
{
  atomicPtrXchg(&self->_d, other->_d->addRef())->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::ComponentTransferFunction - Eq]
// ============================================================================

static bool FOG_CDECL ComponentTransferFunction_eq(const ComponentTransferFunction* a, const ComponentTransferFunction* b)
{
  const ComponentTransferFunctionData* a_d = a->_d;
  const ComponentTransferFunctionData* b_d = b->_d;

  if (a_d == b_d)
    return true;

  uint32_t functionType = a_d->functionType;
  if (functionType != b_d->functionType)
    return false;

  switch (functionType)
  {
    case COMPONENT_TRANSFER_FUNCTION_IDENTITY:
      return true;

    case COMPONENT_TRANSFER_FUNCTION_TABLE:
    case COMPONENT_TRANSFER_FUNCTION_DISCRETE:
      return a_d->table() == b_d->table();

    case COMPONENT_TRANSFER_FUNCTION_LINEAR:
      return a_d->linear() == b_d->linear();

    case COMPONENT_TRANSFER_FUNCTION_GAMMA:
      return a_d->gamma() == b_d->gamma();

    default:
      return false;
  }
}

// ============================================================================
// [Fog::ComponentTransferFunction - ComponentTransferFunctionData]
// ============================================================================

static ComponentTransferFunctionData* FOG_CDECL ComponentTransferFunction_dCreate(uint32_t functionType, const void* functionData)
{
  if (functionType == COMPONENT_TRANSFER_FUNCTION_IDENTITY)
    return ComponentTransferFunction_dIdentity->addRef();

  ComponentTransferFunctionData* d = reinterpret_cast<ComponentTransferFunctionData*>(
    MemMgr::alloc(sizeof(ComponentTransferFunctionData)));

  if (FOG_IS_NULL(d))
    return NULL;

  d->reference.init(1);
  d->vType = 0; // TODO: Var
  d->functionType = functionType;

  switch (functionType)
  {
    case COMPONENT_TRANSFER_FUNCTION_TABLE:
    case COMPONENT_TRANSFER_FUNCTION_DISCRETE:
      d->table.initCustom1(*reinterpret_cast< const List<float>* >(functionData));
      break;

    case COMPONENT_TRANSFER_FUNCTION_LINEAR:
      d->linear.initCustom1(*reinterpret_cast< const ComponentTransferFunctionLinear* >(functionData));
      break;

    case COMPONENT_TRANSFER_FUNCTION_GAMMA:
      d->gamma.initCustom1(*reinterpret_cast< const ComponentTransferFunctionGamma* >(functionData));
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return d;
}

static void FOG_CDECL ComponentTransferFunction_dFree(ComponentTransferFunctionData* d)
{
  switch (d->functionType)
  {
    case COMPONENT_TRANSFER_FUNCTION_TABLE:
    case COMPONENT_TRANSFER_FUNCTION_DISCRETE:
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

FOG_NO_EXPORT void ComponentTransferFunction_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  _api.componenttransferfunction_ctor = ComponentTransferFunction_ctor;
  _api.componenttransferfunction_ctorCopy = ComponentTransferFunction_ctorCopy;
  _api.componenttransferfunction_dtor = ComponentTransferFunction_dtor;

  _api.componenttransferfunction_resultsInIdentity = ComponentTransferFunction_resultsInIdentity;
  _api.componenttransferfunction_getData = ComponentTransferFunction_getData;
  _api.componenttransferfunction_setData = ComponentTransferFunction_setData;
  _api.componenttransferfunction_reset = ComponentTransferFunction_reset;

  _api.componenttransferfunction_copy = ComponentTransferFunction_copy;
  _api.componenttransferfunction_eq = ComponentTransferFunction_eq;

  _api.componenttransferfunction_dCreate = ComponentTransferFunction_dCreate;
  _api.componenttransferfunction_dFree = ComponentTransferFunction_dFree;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  ComponentTransferFunctionData* d = &ComponentTransferFunction_dIdentity;

  d->reference.init(1);
  d->vType = 0; // TODO: Var
  d->functionType = COMPONENT_TRANSFER_FUNCTION_IDENTITY;

  _api.componenttransferfunction_oIdentity = ComponentTransferFunction_oIdentity.initCustom1(d);
}

} // Fog namespace
