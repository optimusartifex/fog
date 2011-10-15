// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_LISTREAL_H
#define _FOG_CORE_TOOLS_LISTREAL_H

// [Dependencies]
#include <Fog/Core/Tools/List.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// This file contains implementation of List<float> and List<double>. Very
// interesting problem is the behavior of indexing and comparing of special
// numbers like Infinity and NaN. Instead of using FPU comparison instructions
// which never return true for NaN <-> NaN comparison, we use binary indexOf
// and compare methods.
//
// Topics:
//   - http://stackoverflow.com/questions/4816156/are-ieee-floats-valid-key-types-for-stdmap-and-stdset

// ===========================================================================
// [Fog::List<float>]
// ===========================================================================

template<>
struct List<float> : public ListImpl< float, TypeInfo<float>::TYPE, 1, 1 >
{
  typedef ListImpl< float, TypeInfo<float>::TYPE, 1, 1 > Impl;

  using ListImpl<float, TypeInfo<float>::TYPE, 0, 0>::setList;
  using ListImpl<float, TypeInfo<float>::TYPE, 0, 0>::concat;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE List()
  {
    fog_api.list_untyped_ctor(this);
  }

  FOG_INLINE List(const List& other)
  {
    fog_api.list_untyped_ctorCopy(this, &other);
  }

  FOG_INLINE List(const List& other, const Range& range)
  {
    _ctorSlice(other, range);
  }

  explicit FOG_INLINE List(ListUntypedData* d)
  {
    ListUntyped::_d = d;
  }

  FOG_INLINE ~List()
  {
    Impl::_dtor();
  }

  // --------------------------------------------------------------------------
  // [SetList]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setList(const List<double>& other);
  FOG_INLINE err_t setList(const List<double>& other, const Range& range);

  FOG_INLINE err_t setList(const double* data, size_t dataLength)
  {
    return fog_api.list_float_opDataD(this, CONTAINER_OP_REPLACE, data, dataLength);
  }

  // --------------------------------------------------------------------------
  // [Append / Concat]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t concat(const List<double>& other);
  FOG_INLINE err_t concat(const List<double>& other, const Range& range);

  FOG_INLINE err_t concat(const double* data, size_t dataLength)
  {
    return fog_api.list_float_opDataD(this, CONTAINER_OP_APPEND, data, dataLength);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE List<float>& operator=(const List<float>& other)
  {
    Impl::_copy(other);
    return *this;
  }

  FOG_INLINE List<float>& operator+=(const float& item)
  {
    Impl::append(item);
    return *this;
  }

  FOG_INLINE bool operator==(const List<float>& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const List<float>& other) const { return !eq(other); }

  FOG_INLINE const float& operator[](size_t index) const
  {
    FOG_ASSERT_X(index < ListUntyped::_d->length,
      "Fog::List<float>::operator[] - Index out of range.");

    return reinterpret_cast<const float*>(ListUntyped::_d->data)[index];
  }
};

// ===========================================================================
// [Fog::List<double>]
// ===========================================================================

template<>
struct List<double> : public ListImpl< double, TypeInfo<double>::TYPE, 1, 1 >
{
  typedef ListImpl< double, TypeInfo<double>::TYPE, 1, 1 >
    Impl;

  using ListImpl<double, TypeInfo<double>::TYPE, 0, 0>::setList;
  using ListImpl<double, TypeInfo<double>::TYPE, 0, 0>::concat;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE List()
  {
    fog_api.list_untyped_ctor(this);
  }

  FOG_INLINE List(const List& other)
  {
    fog_api.list_untyped_ctorCopy(this, &other);
  }

  FOG_INLINE List(const List& other, const Range& range)
  {
    _ctorSlice(other, range);
  }

  explicit FOG_INLINE List(ListUntypedData* d)
  {
    ListUntyped::_d = d;
  }

  FOG_INLINE ~List()
  {
    Impl::_dtor();
  }

  // --------------------------------------------------------------------------
  // [SetList]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setList(const List<float>& other)
  {
    return fog_api.list_double_opListF(this, CONTAINER_OP_REPLACE, &other, NULL);
  }

  FOG_INLINE err_t setList(const List<float>& other, const Range& range)
  {
    return fog_api.list_double_opListF(this, CONTAINER_OP_REPLACE, &other, &range);
  }

  FOG_INLINE err_t setList(const float* data, size_t dataLength)
  {
    return fog_api.list_double_opDataF(this, CONTAINER_OP_REPLACE, data, dataLength);
  }

  // --------------------------------------------------------------------------
  // [Append / Concat]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t concat(const List<float>& other)
  {
    return fog_api.list_double_opListF(this, CONTAINER_OP_APPEND, &other, NULL);
  }

  FOG_INLINE err_t concat(const List<float>& other, const Range& range)
  {
    return fog_api.list_double_opListF(this, CONTAINER_OP_APPEND, &other, &range);
  }

  FOG_INLINE err_t concat(const float* data, size_t dataLength)
  {
    return fog_api.list_double_opDataF(this, CONTAINER_OP_APPEND, data, dataLength);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE List<double>& operator=(const List<double>& other)
  {
    Impl::_copy(other);
    return *this;
  }

  FOG_INLINE List<double>& operator+=(const double& item)
  {
    Impl::append(item);
    return *this;
  }

  FOG_INLINE bool operator==(const List<double>& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const List<double>& other) const { return !eq(other); }

  FOG_INLINE const double& operator[](size_t index) const
  {
    FOG_ASSERT_X(index < ListUntyped::_d->length,
      "Fog::List<double>::operator[] - Index out of range.");

    return reinterpret_cast<const double*>(ListUntyped::_d->data)[index];
  }
};

// ============================================================================
// [Fog::List<float> - Implemented-Later]
// ============================================================================

FOG_INLINE err_t List<float>::setList(const List<double>& other)
{
  return fog_api.list_float_opListD(this, CONTAINER_OP_REPLACE, &other, NULL);
}

FOG_INLINE err_t List<float>::setList(const List<double>& other, const Range& range)
{
  return fog_api.list_float_opListD(this, CONTAINER_OP_REPLACE, &other, &range);
}

// --------------------------------------------------------------------------
// [Append / Concat]
// --------------------------------------------------------------------------

FOG_INLINE err_t List<float>::concat(const List<double>& other)
{
  return fog_api.list_float_opListD(this, CONTAINER_OP_APPEND, &other, NULL);
}

FOG_INLINE err_t List<float>::concat(const List<double>& other, const Range& range)
{
  return fog_api.list_float_opListD(this, CONTAINER_OP_APPEND, &other, &range);
}

// ============================================================================
// [Fog::List<double> - Implemented-Later]
// ============================================================================

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_LISTREAL_H
