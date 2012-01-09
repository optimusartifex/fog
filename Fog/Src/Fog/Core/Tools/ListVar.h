// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_LISTVAR_H
#define _FOG_CORE_TOOLS_LISTVAR_H

// [Dependencies]
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/Var.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ===========================================================================
// [Fog::List<Var>]
// ===========================================================================

template<>
struct List<Var> : public ListUntyped
{
  // --------------------------------------------------------------------------
  // [Typedefs]
  // --------------------------------------------------------------------------

  //! @brief Item type.
  typedef Var Item;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE List()
  {
    fog_api.list_var_ctor(this);
  }

  FOG_INLINE List(const List<Var>& other)
  {
    fog_api.list_untyped_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE List(List<Var>&& other) { ListUntyped::_d = other._d; other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  FOG_INLINE List(const List<Var>& other, const Range& range)
  {
    fog_api.list_var_ctorSlice(this, &other, &range);
  }

  explicit FOG_INLINE List(ListUntypedData* d)
  {
    _d = d;
  }

  FOG_INLINE ~List()
  {
    fog_api.list_var_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  using ListUntyped::getReference;
  using ListUntyped::isDetached;

  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach()
  {
    return ListUntyped::isDetached() ? (err_t)ERR_OK : _detach();
  }

  FOG_INLINE err_t _detach()
  {
    return fog_api.list_var_detach(this);
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  using ListUntyped::getCapacity;
  using ListUntyped::getLength;
  using ListUntyped::isEmpty;

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return fog_api.list_var_reserve(this, capacity);
  }

  FOG_INLINE void squeeze()
  {
    fog_api.list_var_squeeze(this);
  }

  FOG_INLINE err_t _growLeft(size_t length)
  {
    return fog_api.list_var_growLeft(this, length);
  }

  FOG_INLINE err_t _growRight(size_t length)
  {
    return fog_api.list_var_growRight(this, length);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const Var* getData() const
  {
    return reinterpret_cast<const Var*>(_d->data);
  }

  FOG_INLINE Var* getDataX()
  {
    FOG_ASSERT_X(ListUntyped::isDetached(),
      "Fog::List<Fog::Var>::getDataX() - Not detached.");

    return reinterpret_cast<Var*>(_d->data);
  }

  FOG_INLINE const Var& getAt(size_t index) const
  {
    FOG_ASSERT_X(index < _d->length,
      "Fog::List<Fog::Var>::getAt() - Index out of range.");

    return reinterpret_cast<const Var*>(_d->data)[index];
  }

  FOG_INLINE err_t setAt(size_t index, const Var& item)
  {
    FOG_ASSERT_X(index < _d->length,
      "Fog::List<Fog::Var>::setAt() - Index out of range.");

    return fog_api.list_var_setAt(this, index, &item);
  }

  FOG_INLINE err_t setAtX(size_t index, const Var& item)
  {
    FOG_ASSERT_X(ListUntyped::isDetached(),
      "Fog::List<Fog::Var>::setAtX() - Not detached.");
    FOG_ASSERT_X(index < _d->length,
      "Fog::List<Fog::Var>::setAtX() - Index out of range.");

    reinterpret_cast<Var*>(_d->data)[index] = item;
    return ERR_OK;
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    fog_api.list_var_clear(this);
  }

  FOG_INLINE void reset()
  {
    fog_api.list_var_reset(this);
  }

  // --------------------------------------------------------------------------
  // [SetList]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setList(List<Var>& other)
  {
    return fog_api.list_var_opList(this, CONTAINER_OP_REPLACE, &other, NULL);
  }

  FOG_INLINE err_t setList(List<Var>& other, const Range& range)
  {
    return fog_api.list_var_opList(this, CONTAINER_OP_REPLACE, &other, &range);
  }

  // --------------------------------------------------------------------------
  // [Append / Concat]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t append(const Var& item)
  {
    return fog_api.list_var_append(this, &item);
  }

  FOG_INLINE err_t concat(const List<Var>& other)
  {
    return fog_api.list_var_opList(this, CONTAINER_OP_APPEND, &other, NULL);
  }

  FOG_INLINE err_t concat(const List<Var>& other, const Range& range)
  {
    return fog_api.list_var_opList(this, CONTAINER_OP_APPEND, &other, &range);
  }

  // --------------------------------------------------------------------------
  // [Prepend]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t prepend(const Var& item)
  {
    return fog_api.list_var_insert(this, 0, &item);
  }

  // --------------------------------------------------------------------------
  // [Insert]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t insert(size_t index, const Var& item)
  {
    return fog_api.list_var_insert(this, index, &item);
  }

  // --------------------------------------------------------------------------
  // [Remove]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t removeRange(const Range& range)
  {
    return fog_api.list_var_remove(this, &range);
  }

  FOG_INLINE err_t removeAt(size_t index)
  {
    return removeRange(Range(index, index + 1));
  }

  FOG_INLINE err_t removeFirst()
  {
    return removeRange(Range(0, 1));
  }

  FOG_INLINE err_t removeLast()
  {
    size_t length = _d->length;
    return removeRange(Range(length - 1, length));
  }

  // --------------------------------------------------------------------------
  // [Replace]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t replace(const Range& range, const List<Var>& src)
  {
    return fog_api.list_var_replace(this, &range, &src, NULL);
  }

  FOG_INLINE err_t replace(const Range& range, const List<Var>& src, const Range& srcRange)
  {
    return fog_api.list_var_replace(this, &range, &src, &srcRange);
  }

  // --------------------------------------------------------------------------
  // [Slice]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t slice(const Range& range)
  {
    return fog_api.list_var_slice(this, &range);
  }

  // --------------------------------------------------------------------------
  // [Contains]
  // --------------------------------------------------------------------------

  FOG_INLINE bool contains(const Var& item) const
  {
    return fog_api.list_var_indexOf(this, NULL, &item) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const Range& range, const Var& item) const
  {
    return fog_api.list_var_indexOf(this, &range, &item) != INVALID_INDEX;
  }

  // --------------------------------------------------------------------------
  // [IndexOf]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t indexOf(const Var& item) const
  {
    return fog_api.list_var_indexOf(this, NULL, &item);
  }

  FOG_INLINE size_t indexOf(const Range& range, const Var& item) const
  {
    return fog_api.list_var_indexOf(this, &range, &item);
  }

  // --------------------------------------------------------------------------
  // [LastIndexOf]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t lastIndexOf(const Var& item) const
  {
    return fog_api.list_var_lastIndexOf(this, NULL, &item);
  }

  FOG_INLINE size_t lastIndexOf(const Range& range, const Var& item) const
  {
    return fog_api.list_var_lastIndexOf(this, &range, &item);
  }

  // --------------------------------------------------------------------------
  // [Sort]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t sort(uint32_t sortOrder)
  {
    return fog_api.list_var_sort(this, sortOrder, Var::getCompareFunc());
  }

  FOG_INLINE err_t sort(uint32_t sortOrder, CompareFunc compareFunc)
  {
    return fog_api.list_var_sort(this, sortOrder, compareFunc);
  }

  FOG_INLINE err_t sort(uint32_t sortOrder, CompareExFunc compareFunc, const void* data)
  {
    return fog_api.list_var_sortEx(this, sortOrder, compareFunc, data);
  }

  // --------------------------------------------------------------------------
  // [SwapItems]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t swapItems(size_t index1, size_t index2)
  {
    FOG_ASSERT_X(index1 < _d->length,
      "Fog::List<Fog::Var>::swapItems() - Index1 out of range.");
    FOG_ASSERT_X(index2 < _d->length,
      "Fog::List<Fog::Var>::swapItems() - Index2 out of range.");

    return fog_api.list_var_swapItems(this, index1, index2);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const List<Var>& other) const
  {
    return fog_api.list_untyped_customEq(this, &other, sizeof(Var), Var::getEqFunc());
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE List<Var>& operator=(const List<Var>& other)
  {
    fog_api.list_var_opList(this, CONTAINER_OP_REPLACE, &other, NULL);
    return *this;
  }

  FOG_INLINE List<Var>& operator+=(const Var& item)
  {
    fog_api.list_var_append(this, &item);
    return *this;
  }

  FOG_INLINE bool operator==(const List<Var>& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const List<Var>& other) const { return !eq(other); }

  FOG_INLINE const Var& operator[](size_t index) const
  {
    FOG_ASSERT_X(index < _d->length,
      "Fog::List<Fog::Var>::operator[] - Index out of range.");

    return reinterpret_cast<const Var*>(_d->data)[index];
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const List<Var>* a, const List<Var>* b)
  {
    return fog_api.list_untyped_customEq(a, b, sizeof(Var), Var::getEqFunc());
  }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_LISTVAR_H
