// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_LIST_H
#define _FOG_CORE_TOOLS_LIST_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Tools/Algorithm.h>
#include <Fog/Core/Tools/ListUntyped.h>
#include <Fog/Core/Tools/Range.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::ListVTable<ItemT>]
// ============================================================================

template<typename ItemT>
struct ListVTable
{
  static const ListUntypedVTable vtable;
  static FOG_INLINE const ListUntypedVTable* getVTable() { return &vtable; }

  static void FOG_CDECL _ctor(void* _dst, const void* _src, size_t length)
  {
    ItemT* dst = reinterpret_cast<ItemT*>(_dst);
    const ItemT* src = reinterpret_cast<const ItemT*>(_src);

    for (size_t i = length; i; i--, dst++, src++)
      fog_new_p(dst) ItemT(*src);
  }

  static void FOG_CDECL _dtor(void* _dst, size_t length)
  {
    ItemT* dst = reinterpret_cast<ItemT*>(_dst);

    for (size_t i = length; i; i--, dst++)
      dst->~ItemT();
  }
};

template<typename ItemT>
const ListUntypedVTable ListVTable<ItemT>::vtable =
{
  sizeof(ItemT),

  ListVTable<ItemT>::_ctor,
  ListVTable<ItemT>::_dtor
};

// ============================================================================
// [Fog::ListImpl<>]
// ============================================================================

template<typename ItemT, int Category, int Indexable, int Comparable>
struct ListImpl {};

// ============================================================================
// [Fog::ListImpl<> - Simple]
// ============================================================================

template<typename ItemT>
struct ListImpl<ItemT, TYPE_CATEGORY_SIMPLE, 0, 0> : public ListUntyped
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE void _ctorSlice(const List<ItemT>& other, const Range& range)
  {
    fog_api.list_simple_ctorSlice(this, sizeof(ItemT), &other, &range);
  }

  FOG_INLINE void _dtor()
  {
    fog_api.list_simple_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  using ListUntyped::getReference;
  using ListUntyped::isDetached;

  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach()
  {
    return isDetached() ? (err_t)ERR_OK : _detach();
  }

  FOG_INLINE err_t _detach()
  {
    return fog_api.list_simple_detach(this, sizeof(ItemT));
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  using ListUntyped::getCapacity;
  using ListUntyped::getLength;
  using ListUntyped::isEmpty;

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return fog_api.list_simple_reserve(this, sizeof(ItemT), capacity);
  }

  FOG_INLINE void squeeze()
  {
    fog_api.list_simple_squeeze(this, sizeof(ItemT));
  }

  FOG_INLINE err_t _growLeft(size_t length)
  {
    return fog_api.list_simple_growLeft(this, sizeof(ItemT), length);
  }

  FOG_INLINE err_t _growRight(size_t length)
  {
    return fog_api.list_simple_growRight(this, sizeof(ItemT), length);
  }

  FOG_INLINE ItemT* _prepare(uint32_t cntOp, size_t length)
  {
    return reinterpret_cast<ItemT*>(fog_api.list_simple_prepare(this, sizeof(ItemT), cntOp, length));
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const ItemT* getData() const
  {
    return reinterpret_cast<const ItemT*>(ListUntyped::_d->data);
  }

  FOG_INLINE ItemT* getDataX()
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::List<?>::getDataX() - Not detached.");

    return reinterpret_cast<ItemT*>(ListUntyped::_d->data);
  }

  FOG_INLINE const ItemT& getAt(size_t index) const
  {
    FOG_ASSERT_X(index < ListUntyped::_d->length,
      "Fog::List<?>::getAt() - Index out of range.");

    return reinterpret_cast<const ItemT*>(ListUntyped::_d->data)[index];
  }

  FOG_INLINE const ItemT& getFirst() const
  {
    FOG_ASSERT_X(ListUntyped::_d->length > 0,
      "Fog::List<?>::getFirst() - List is empty.");

    return reinterpret_cast<const ItemT*>(ListUntyped::_d->data)[0];
  }

  FOG_INLINE const ItemT& getLast() const
  {
    FOG_ASSERT_X(ListUntyped::_d->length > 0,
      "Fog::List<?>::getLast() - List is empty.");

    ListUntypedData* d = ListUntyped::_d;
    return reinterpret_cast<const ItemT*>(d->data)[d->length - 1];
  }

  FOG_INLINE err_t setAt(size_t index, const ItemT& item)
  {
    FOG_ASSERT_X(index < ListUntyped::_d->length,
      "Fog::List<?>::setAt() - Index out of range.");

    if (sizeof(ItemT) % 4)
      return fog_api.list_simple_setAt(this, sizeof(ItemT), index, &item);
    else
      return fog_api.list_simple_setAt_4x(this, sizeof(ItemT), index, &item);
  }

  FOG_INLINE err_t setAtX(size_t index, const ItemT& item)
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::List<?>::setAtX() - Not detached.");
    FOG_ASSERT_X(index < ListUntyped::_d->length,
      "Fog::List<?>::setAtX() - Index out of range.");

    reinterpret_cast<ItemT*>(ListUntyped::_d->data)[index] = item;
    return ERR_OK;
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    fog_api.list_simple_clear(this);
  }

  FOG_INLINE void reset()
  {
    fog_api.list_simple_reset(this);
  }

  // --------------------------------------------------------------------------
  // [SetList]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setList(const List<ItemT>& other)
  {
    fog_api.list_simple_copy(this, &other);
    return ERR_OK;
  }

  FOG_INLINE err_t setList(const List<ItemT>& other, const Range& range)
  {
    return fog_api.list_simple_opList(this, sizeof(ItemT), CONTAINER_OP_REPLACE, &other, &range);
  }

  FOG_INLINE err_t setList(const ItemT* data, size_t dataLength)
  {
    return fog_api.list_simple_opData(this, sizeof(ItemT), CONTAINER_OP_REPLACE, data, dataLength);
  }

  // --------------------------------------------------------------------------
  // [Append / Concat]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t append(const ItemT& item)
  {
    if (sizeof(ItemT) % 4)
      return fog_api.list_simple_appendItem(this, sizeof(ItemT), &item);
    else
      return fog_api.list_simple_appendItem_4x(this, sizeof(ItemT), &item);
  }

  FOG_INLINE err_t concat(const List<ItemT>& other)
  {
    return fog_api.list_simple_opList(this, sizeof(ItemT), CONTAINER_OP_APPEND, &other, NULL);
  }

  FOG_INLINE err_t concat(const List<ItemT>& other, const Range& range)
  {
    return fog_api.list_simple_opList(this, sizeof(ItemT), CONTAINER_OP_APPEND, &other, &range);
  }

  FOG_INLINE err_t concat(const ItemT* data, size_t dataLength)
  {
    return fog_api.list_simple_opData(this, sizeof(ItemT), CONTAINER_OP_APPEND, data, dataLength);
  }

  // --------------------------------------------------------------------------
  // [Prepend]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t prepend(const ItemT& item)
  {
    return insert(0, item);
  }

  // --------------------------------------------------------------------------
  // [Insert]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t insert(size_t index, const ItemT& item)
  {
    if (sizeof(ItemT) % 4)
      return fog_api.list_simple_insertItem(this, sizeof(ItemT), index, &item);
    else
      return fog_api.list_simple_insertItem_4x(this, sizeof(ItemT), index, &item);
  }

  // --------------------------------------------------------------------------
  // [Remove]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t removeRange(const Range& range)
  {
    return fog_api.list_simple_remove(this, sizeof(ItemT), &range);
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
    size_t length = ListUntyped::_d->length;
    return removeRange(Range(length - 1, length));
  }

  // --------------------------------------------------------------------------
  // [Replace]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t replace(const Range& range, const List<ItemT>& src)
  {
    return fog_api.list_simple_replace(this, sizeof(ItemT), &range, &src, NULL);
  }

  FOG_INLINE err_t replace(const Range& range, const List<ItemT>& src, const Range& srcRange)
  {
    return fog_api.list_simple_replace(this, sizeof(ItemT), &range, &src, &srcRange);
  }

  // --------------------------------------------------------------------------
  // [Slice]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t slice(const Range& range)
  {
    return fog_api.list_simple_slice(this, sizeof(ItemT), &range);
  }

  // --------------------------------------------------------------------------
  // [Sort]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t sort(uint32_t sortOrder, CompareFunc compareFunc)
  {
    return fog_api.list_simple_sort(this, sizeof(ItemT), sortOrder, compareFunc);
  }

  FOG_INLINE err_t sort(uint32_t sortOrder, CompareExFunc compareFunc, const void* data)
  {
    return fog_api.list_simple_sortEx(this, sizeof(ItemT), sortOrder, compareFunc, data);
  }

  // --------------------------------------------------------------------------
  // [SwapItems]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t swapItems(size_t index1, size_t index2)
  {
    FOG_ASSERT_X(index1 < ListUntyped::_d->length,
      "Fog::List<?>::swapItems() - Index1 out of range.");
    FOG_ASSERT_X(index2 < ListUntyped::_d->length,
    "Fog::List<?>::swapItems() - Index2 out of range.");

    if (sizeof(ItemT) % 4)
      return fog_api.list_simple_swapItems(this, sizeof(ItemT), index1, index2);
    else
      return fog_api.list_simple_swapItems_4x(this, sizeof(ItemT), index1, index2);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE void _copy(const List<ItemT>& other)
  {
    fog_api.list_simple_copy(this, &other);
  }
};

template<typename ItemT>
struct ListImpl<ItemT, TYPE_CATEGORY_SIMPLE, 0, 1> : public ListImpl<ItemT, TYPE_CATEGORY_SIMPLE, 0, 0>
{
  // --------------------------------------------------------------------------
  // [Sort]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t sort(uint32_t sortOrder)
  {
    return fog_api.list_simple_sort(this, sizeof(ItemT), sortOrder, TypeFunc<ItemT>::getCompareFunc());
  }
};

template<typename ItemT, int Comparable>
struct ListImpl<ItemT, TYPE_CATEGORY_SIMPLE, 1, Comparable> : public ListImpl<ItemT, TYPE_CATEGORY_SIMPLE, 0, Comparable>
{
  // --------------------------------------------------------------------------
  // [Contains]
  // --------------------------------------------------------------------------

  FOG_INLINE bool contains(const ItemT& item) const
  {
    return indexOf(item) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const Range& range, const ItemT& item) const
  {
    return indexOf(range, item) != INVALID_INDEX;
  }

  // --------------------------------------------------------------------------
  // [IndexOf]
  // --------------------------------------------------------------------------

  FOG_NO_INLINE size_t _indexOf(const ItemT& item) const
  {
    const ListUntypedData* d = ListUntyped::_d;
    size_t length = d->length;

    const ItemT* p = reinterpret_cast<const ItemT*>(d->data);
    for (size_t i = 0; i < length; i++, p++)
    {
      if (p[0] == item)
        return i;
    }
    return INVALID_INDEX;
  }

  FOG_NO_INLINE size_t _indexOf(const Range& range_, const ItemT& item) const
  {
    const ListUntypedData* d = ListUntyped::_d;
    size_t length = d->length;

    size_t rStart = range_.getStart();
    size_t rEnd = Math::min(range_.getEnd(), length);

    if (rStart >= rEnd)
      return INVALID_INDEX;

    const ItemT* p = reinterpret_cast<const ItemT*>(d->data) + rStart;
    for (size_t i = rStart; i < rEnd; i++, p++)
    {
      if (p[0] == item)
        return i;
    }
    return INVALID_INDEX;
  }

  //! @brief Get the first index of the @a item or @c INVALID_INDEX.
  FOG_INLINE size_t indexOf(const ItemT& item) const
  {
    if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 4)
      return fog_api.list_untyped_indexOf_4B(this, NULL, &item);
    else if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 8)
      return fog_api.list_untyped_indexOf_8B(this, NULL, &item);
    else if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 16)
      return fog_api.list_untyped_indexOf_16B(this, NULL, &item);
    else
      return _indexOf(item);
  }

  //! @overload.
  FOG_INLINE size_t indexOf(const Range& range_, const ItemT& item) const
  {
    if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 4)
      return fog_api.list_untyped_indexOf_4B(this, &range_, &item);
    else if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 8)
      return fog_api.list_untyped_indexOf_8B(this, &range_, &item);
    else if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 16)
      return fog_api.list_untyped_indexOf_16B(this, &range_, &item);
    else
      return _indexOf(range_, item);
  }

  // --------------------------------------------------------------------------
  // [LastIndexOf]
  // --------------------------------------------------------------------------

  FOG_NO_INLINE size_t _lastIndexOf(const ItemT& item) const
  {
    const ListUntypedData* d = ListUntyped::_d;
    size_t i = d->length - 1;

    const ItemT* p = reinterpret_cast<const ItemT*>(d->data) + i;
    for (; i != INVALID_INDEX; i--, p--)
    {
      if (p[0] == item)
        return i;
    }
    return INVALID_INDEX;
  }

  FOG_NO_INLINE size_t _lastIndexOf(const Range& range_, const ItemT& item) const
  {
    const ListUntypedData* d = ListUntyped::_d;
    size_t i, length = d->length;

    size_t rStart = range_.getStart();
    size_t rEnd = Math::min(range_.getEnd(), length);

    if (rStart >= rEnd)
      return INVALID_INDEX;

    i = rEnd - 1;
    const ItemT* p = reinterpret_cast<const ItemT*>(d->data) + i;
    for (;;)
    {
      if (p[0] == item)
        return i;
      if (i == rStart)
        break;
      i--;
      p--;
    }

    return INVALID_INDEX;
  }

  //! @brief Get the last index of the @a item or @c INVALID_INDEX.
  FOG_INLINE size_t lastIndexOf(const ItemT& item) const
  {
    if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 4)
      return fog_api.list_untyped_lastIndexOf_4B(this, NULL, &item);
    else if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 8)
      return fog_api.list_untyped_lastIndexOf_8B(this, NULL, &item);
    else if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 16)
      return fog_api.list_untyped_lastIndexOf_16B(this, NULL, &item);
    else
      return _lastIndexOf(item);
  }

  //! @overload.
  FOG_NO_INLINE size_t lastIndexOf(const Range& range_, const ItemT& item) const
  {
    if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 4)
      return fog_api.list_untyped_lastIndexOf_4B(this, &range_, &item);
    else if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 8)
      return fog_api.list_untyped_lastIndexOf_8B(this, &range_, &item);
    else if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 16)
      return fog_api.list_untyped_lastIndexOf_16B(this, &range_, &item);
    else
      return _lastIndexOf(range_, item);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const List<ItemT>& other) const
  {
    if (TypeInfo<ItemT>::BIN_EQ)
      return fog_api.list_untyped_binaryEq(this, &other, sizeof(ItemT));
    else
      return fog_api.list_untyped_customEq(this, &other, sizeof(ItemT), TypeFunc<ItemT>::getEqFunc());
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE bool operator==(const List<ItemT>& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const List<ItemT>& other) const { return !eq(other); }
};

// ============================================================================
// [Fog::ListImpl<> - Movable]
// ============================================================================

template<typename ItemT>
struct ListImpl<ItemT, TYPE_CATEGORY_MOVABLE, 0, 0> : public ListUntyped
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE void _ctorSlice(const List<ItemT>& other, const Range& range)
  {
    fog_api.list_unknown_ctorSlice(this, ListVTable<ItemT>::getVTable(), &other, &range);
  }

  FOG_INLINE void _dtor()
  {
    fog_api.list_unknown_dtor(this, ListVTable<ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  using ListUntyped::getReference;
  using ListUntyped::isDetached;

  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach()
  {
    return isDetached() ? (err_t)ERR_OK : _detach();
  }

  FOG_INLINE err_t _detach()
  {
    return fog_api.list_unknown_detach(this, ListVTable<ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  using ListUntyped::getCapacity;
  using ListUntyped::getLength;
  using ListUntyped::isEmpty;

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return fog_api.list_unknown_reserve(this, ListVTable<ItemT>::getVTable(), capacity);
  }

  FOG_INLINE void squeeze()
  {
    fog_api.list_unknown_squeeze(this, ListVTable<ItemT>::getVTable());
  }

  FOG_INLINE err_t _growLeft(size_t length)
  {
    return fog_api.list_unknown_growLeft(this, ListVTable<ItemT>::getVTable(), length);
  }

  FOG_INLINE err_t _growRight(size_t length)
  {
    return fog_api.list_unknown_growRight(this, ListVTable<ItemT>::getVTable(), length);
  }

  FOG_INLINE ItemT* _prepare(uint32_t cntOp, size_t length)
  {
    return reinterpret_cast<ItemT*>(fog_api.list_unknown_prepare(this, ListVTable<ItemT>::getVTable(), cntOp, length));
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const ItemT* getData() const
  {
    return reinterpret_cast<const ItemT*>(ListUntyped::_d->data);
  }

  FOG_INLINE ItemT* getDataX()
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::List<?>::getDataX() - Not detached.");

    return reinterpret_cast<ItemT*>(ListUntyped::_d->data);
  }

  FOG_INLINE const ItemT& getAt(size_t index) const
  {
    FOG_ASSERT_X(index < ListUntyped::_d->length,
      "Fog::List<?>::getAt() - Index out of range.");

    return reinterpret_cast<const ItemT*>(ListUntyped::_d->data)[index];
  }

  FOG_INLINE const ItemT& getFirst() const
  {
    FOG_ASSERT_X(ListUntyped::_d->length > 0,
      "Fog::List<?>::getFirst() - List is empty.");

    return reinterpret_cast<const ItemT*>(ListUntyped::_d->data)[0];
  }

  FOG_INLINE const ItemT& getLast() const
  {
    FOG_ASSERT_X(ListUntyped::_d->length > 0,
      "Fog::List<?>::getLast() - List is empty.");

    ListUntypedData* d = ListUntyped::_d;
    return reinterpret_cast<const ItemT*>(d->data)[d->length - 1];
  }

  FOG_INLINE err_t setAt(size_t index, const ItemT& item)
  {
    FOG_ASSERT_X(index < ListUntyped::_d->length,
      "Fog::List<?>::setAt() - Index out of range.");

    FOG_RETURN_ON_ERROR(detach());

    reinterpret_cast<ItemT*>(ListUntyped::_d->data)[index] = item;
    return ERR_OK;
  }

  FOG_INLINE err_t setAtX(size_t index, const ItemT& item)
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::List<?>::setAtX() - Not detached.");
    FOG_ASSERT_X(index < ListUntyped::_d->length,
      "Fog::List<?>::setAtX() - Index out of range.");

    reinterpret_cast<ItemT*>(ListUntyped::_d->data)[index] = item;
    return ERR_OK;
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    fog_api.list_unknown_clear(this, ListVTable<ItemT>::getVTable());
  }

  FOG_INLINE void reset()
  {
    fog_api.list_unknown_reset(this, ListVTable<ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [SetList]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setList(const List<ItemT>& other)
  {
    fog_api.list_unknown_copy(this, ListVTable<ItemT>::getVTable(), &other);
    return ERR_OK;
  }

  FOG_INLINE err_t setList(const List<ItemT>& other, const Range& range)
  {
    return fog_api.list_unknown_opList(this, ListVTable<ItemT>::getVTable(), CONTAINER_OP_REPLACE, &other, &range);
  }

  FOG_INLINE err_t setList(const ItemT* data, size_t dataLength)
  {
    return fog_api.list_unknown_opData(this, ListVTable<ItemT>::getVTable(), CONTAINER_OP_REPLACE, data, dataLength);
  }

  // --------------------------------------------------------------------------
  // [Append / Concat]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t append(const ItemT& item)
  {
    ItemT* p = reinterpret_cast<ItemT*>(fog_api.list_unknown_prepareAppendItem(this, ListVTable<ItemT>::getVTable()));

    if (FOG_IS_NULL(p))
      return ERR_RT_OUT_OF_MEMORY;

    fog_new_p(p) ItemT(item);
    return ERR_OK;
  }

  FOG_INLINE err_t concat(const List<ItemT>& other)
  {
    return fog_api.list_unknown_opList(this, ListVTable<ItemT>::getVTable(), CONTAINER_OP_APPEND, &other, NULL);
  }

  FOG_INLINE err_t concat(const List<ItemT>& other, const Range& range)
  {
    return fog_api.list_unknown_opList(this, ListVTable<ItemT>::getVTable(), CONTAINER_OP_APPEND, &other, &range);
  }

  FOG_INLINE err_t concat(const ItemT* data, size_t dataLength)
  {
    return fog_api.list_unknown_opData(this, ListVTable<ItemT>::getVTable(), CONTAINER_OP_APPEND, data, dataLength);
  }

  // --------------------------------------------------------------------------
  // [Prepend]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t prepend(const ItemT& item)
  {
    return insert(0, item);
  }

  // --------------------------------------------------------------------------
  // [Insert]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t insert(size_t index, const ItemT& item)
  {
    ItemT* p = reinterpret_cast<ItemT*>(fog_api.list_unknown_prepareInsertItem(this, ListVTable<ItemT>::getVTable(), index));

    if (FOG_IS_NULL(p))
      return ERR_RT_OUT_OF_MEMORY;

    fog_new_p(p) ItemT(item);
    return ERR_OK;
  }

  // --------------------------------------------------------------------------
  // [Remove]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t removeRange(const Range& range)
  {
    return fog_api.list_unknown_remove(this, ListVTable<ItemT>::getVTable(), &range);
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
    size_t length = ListUntyped::_d->length;
    return removeRange(Range(length - 1, length));
  }

  // --------------------------------------------------------------------------
  // [Replace]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t replace(const Range& range, const List<ItemT>& src)
  {
    return fog_api.list_unknown_replace(this, ListVTable<ItemT>::getVTable(), &range, &src, NULL);
  }

  FOG_INLINE err_t replace(const Range& range, const List<ItemT>& src, const Range& srcRange)
  {
    return fog_api.list_unknown_replace(this, ListVTable<ItemT>::getVTable(), &range, &src, &srcRange);
  }

  // --------------------------------------------------------------------------
  // [Slice]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t slice(const Range& range)
  {
    return fog_api.list_unknown_slice(this, ListVTable<ItemT>::getVTable(), &range);
  }

  // --------------------------------------------------------------------------
  // [Sort]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t sort(uint32_t sortOrder, CompareFunc compareFunc)
  {
    return fog_api.list_unknown_sort(this, ListVTable<ItemT>::getVTable(), sortOrder, compareFunc);
  }

  FOG_INLINE err_t sort(uint32_t sortOrder, CompareExFunc compareFunc, const void* data)
  {
    return fog_api.list_unknown_sortEx(this, ListVTable<ItemT>::getVTable(), sortOrder, compareFunc, data);
  }

  // --------------------------------------------------------------------------
  // [SwapItems]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t swapItems(size_t index1, size_t index2)
  {
    FOG_ASSERT_X(index1 < ListUntyped::_d->length,
      "Fog::List<?>::swapItems() - Index1 out of range.");
    FOG_ASSERT_X(index2 < ListUntyped::_d->length,
    "Fog::List<?>::swapItems() - Index2 out of range.");

    FOG_RETURN_ON_ERROR(detach());

    ItemT* p = reinterpret_cast<ItemT*>(ListUntyped::_d->data);
    swap<ItemT>(p[index1], p[index2]);
    return ERR_OK;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE void _copy(const List<ItemT>& other)
  {
    fog_api.list_unknown_copy(this, ListVTable<ItemT>::getVTable(), &other);
  }
};

template<typename ItemT>
struct ListImpl<ItemT, TYPE_CATEGORY_MOVABLE, 0, 1> : public ListImpl<ItemT, TYPE_CATEGORY_MOVABLE, 0, 0>
{
  // --------------------------------------------------------------------------
  // [Sort]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t sort(uint32_t sortOrder)
  {
    return fog_api.list_simple_sort(this, sizeof(ItemT), sortOrder, TypeFunc<ItemT>::getCompareFunc());
  }
};

template<typename ItemT, int Comparable>
struct ListImpl<ItemT, TYPE_CATEGORY_MOVABLE, 1, Comparable> : public ListImpl<ItemT, TYPE_CATEGORY_MOVABLE, 0, Comparable>
{
  // --------------------------------------------------------------------------
  // [Contains]
  // --------------------------------------------------------------------------

  FOG_INLINE bool contains(const ItemT& item) const
  {
    return indexOf(item) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const Range& range, const ItemT& item) const
  {
    return indexOf(range, item) != INVALID_INDEX;
  }

  // --------------------------------------------------------------------------
  // [IndexOf]
  // --------------------------------------------------------------------------

  //! @brief Get the first index of the @a item or @c INVALID_INDEX.
  FOG_NO_INLINE size_t indexOf(const ItemT& item) const
  {
    const ListUntypedData* d = ListUntyped::_d;
    size_t length = d->length;

    const ItemT* p = reinterpret_cast<const ItemT*>(d->data);
    for (size_t i = 0; i < length; i++, p++)
    {
      if (p[0] == item)
        return i;
    }
    return INVALID_INDEX;
  }

  //! @overload.
  FOG_NO_INLINE size_t indexOf(const Range& range_, const ItemT& item) const
  {
    const ListUntypedData* d = ListUntyped::_d;
    size_t length = d->length;

    size_t rStart = range_.getStart();
    size_t rEnd = Math::min(range_.getEnd(), length);

    if (rStart >= rEnd)
      return INVALID_INDEX;

    const ItemT* p = reinterpret_cast<const ItemT*>(d->data) + rStart;
    for (size_t i = rStart; i < rEnd; i++, p++)
    {
      if (p[0] == item)
        return i;
    }
    return INVALID_INDEX;
  }

  // --------------------------------------------------------------------------
  // [LastIndexOf]
  // --------------------------------------------------------------------------

  //! @brief Get the last index of the @a item or @c INVALID_INDEX.
  FOG_NO_INLINE size_t lastIndexOf(const ItemT& item) const
  {
    const ListUntypedData* d = ListUntyped::_d;
    size_t i = d->length - 1;

    const ItemT* p = reinterpret_cast<const ItemT*>(d->data) + i;
    for (; i != INVALID_INDEX; i--, p--)
    {
      if (p[0] == item)
        return i;
    }
    return INVALID_INDEX;
  }

  //! @overload.
  FOG_NO_INLINE size_t lastIndexOf(const Range& range_, const ItemT& item) const
  {
    const ListUntypedData* d = ListUntyped::_d;
    size_t i, length = d->length;

    size_t rStart = range_.getStart();
    size_t rEnd = Math::min(range_.getEnd(), length);

    if (rStart >= rEnd)
      return INVALID_INDEX;

    i = rEnd - 1;
    const ItemT* p = reinterpret_cast<const ItemT*>(d->data) + i;
    for (;;)
    {
      if (p[0] == item)
        return i;
      if (i == rStart)
        break;
      i--;
      p--;
    }

    return INVALID_INDEX;
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const List<ItemT>& other) const
  {
    if (TypeInfo<ItemT>::BIN_EQ)
      return fog_api.list_untyped_binaryEq(this, &other, sizeof(ItemT));
    else
      return fog_api.list_untyped_customEq(this, &other, sizeof(ItemT), TypeFunc<ItemT>::getEqFunc());
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE bool operator==(const List<ItemT>& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const List<ItemT>& other) const { return !eq(other); }
};

// ============================================================================
// [Fog::List<>]
// ============================================================================

template<typename ItemT>
struct List : public ListImpl< ItemT, TypeInfo<ItemT>::TYPE, !TypeInfo<ItemT>::NO_EQ, !TypeInfo<ItemT>::NO_CMP >
{
  typedef ListImpl< ItemT, TypeInfo<ItemT>::TYPE, !TypeInfo<ItemT>::NO_EQ, !TypeInfo<ItemT>::NO_CMP > Impl;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE List()
  {
    fog_api.list_untyped_ctor(this);
  }

  FOG_INLINE List(const List<ItemT>& other)
  {
    fog_api.list_untyped_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE List(List<ItemT>&& other) { ListUntyped::_d = other._d; other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  FOG_INLINE List(const List<ItemT>& other, const Range& range)
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
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE List<ItemT>& operator=(const List<ItemT>& other)
  {
    Impl::_copy(other);
    return *this;
  }

  FOG_INLINE List<ItemT>& operator+=(const ItemT& item)
  {
    Impl::append(item);
    return *this;
  }

  FOG_INLINE const ItemT& operator[](size_t index) const
  {
    FOG_ASSERT_X(index < ListUntyped::_d->length,
      "Fog::List<?>::operator[] - Index out of range.");

    return reinterpret_cast<const ItemT*>(ListUntyped::_d->data)[index];
  }
};

// ============================================================================
// [Fog::ListIterator<>]
// ============================================================================

template<typename ItemT>
struct ListIterator
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create a new List<ItemT> iterator.
  FOG_INLINE ListIterator(const List<ItemT>& container) { start(&container); }

  //! @brief Destroy the List<ItemT> iterator.
  FOG_INLINE ~ListIterator() {}

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getIndex() const
  {
    return (size_t)(_p - reinterpret_cast<ItemT*>(_container->_d->data));
  }

  FOG_INLINE const ItemT& getItem() const
  {
    FOG_ASSERT_X(isValid(),
      "Fog::ListIterator<?>::getItem() - Iterator is not valid.");

    return *_p;
  }

  FOG_INLINE bool isValid() const
  {
    return _p != _end;
  }

  // --------------------------------------------------------------------------
  // [Start / Next]
  // --------------------------------------------------------------------------

  FOG_INLINE bool start(const List<ItemT>* container)
  {
    _container = container;
    return start();
  }

  FOG_INLINE bool start()
  {
    ListUntypedData* d = _container->_d;

    _p = reinterpret_cast<const ItemT*>(d->data);
    _end = _p + d->length;

    return isValid();
  }

  FOG_INLINE bool next()
  {
    FOG_ASSERT_X(_p != _end,
      "Fog::ListIterator<?>::next() - Iterator already at the end.");

    _p++;
    return isValid();
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Operator* that acts like @c getItem().
  FOG_INLINE const ItemT& operator*() const
  {
    FOG_ASSERT_X(isValid(),
      "Fog::ListIterator<?>::operator*() - Iterator is not valid.");

    return *_p;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  const List<ItemT>* _container;
  const ItemT* _p;
  const ItemT* _end;
};

// ============================================================================
// [Fog::ListReverseIterator<>]
// ============================================================================

template<typename ItemT>
struct ListReverseIterator
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create a new List<ItemT> iterator.
  FOG_INLINE ListReverseIterator(const List<ItemT>& container) { start(&container); }

  //! @brief Destroy the List<ItemT> iterator.
  FOG_INLINE ~ListReverseIterator() {}

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getIndex() const
  {
    return (size_t)(_p - reinterpret_cast<ItemT*>(_container->_d->data));
  }

  FOG_INLINE const ItemT& getItem() const
  {
    FOG_ASSERT_X(isValid(),
      "Fog::ListReverseIterator<?>::getItem() - Iterator is not valid.");

    return *_p;
  }

  FOG_INLINE bool isValid() const
  {
    return _p != _end;
  }

  // --------------------------------------------------------------------------
  // [Start / Next]
  // --------------------------------------------------------------------------

  FOG_INLINE bool start(const List<ItemT>* container)
  {
    _container = container;
    return start();
  }

  FOG_INLINE bool start()
  {
    ListUntypedData* d = _container->_d;

    _end = reinterpret_cast<const ItemT*>(d->data) - 1;
    _p = _end + d->length;

    return isValid();
  }

  FOG_INLINE bool next()
  {
    FOG_ASSERT_X(_p != _end,
      "Fog::ListReverseIterator<?>::next() - Iterator already at the end.");

    _p--;
    return isValid();
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Operator* that acts like @c getItem().
  FOG_INLINE const ItemT& operator*() const
  {
    FOG_ASSERT_X(isValid(),
      "Fog::ListReverseIterator<?>::operator*() - Iterator is not valid.");

    return *_p;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  const List<ItemT>* _container;
  const ItemT* _p;
  const ItemT* _end;
};

//! @}

} // Fog namespace

// ============================================================================
// [Include Specialized Containers]
// ============================================================================

#include <Fog/Core/Tools/ListReal.h>

#if defined(_FOG_CORE_TOOLS_STRING_H)
# include <Fog/Core/Tools/ListString.h>
#endif // _FOG_CORE_TOOLS_STRING_H

#if defined(_FOG_CORE_TOOLS_VAR_H)
# include <Fog/Core/Tools/ListVar.h>
#endif // _FOG_CORE_TOOLS_VAR_H

// [Guard]
#endif // _FOG_CORE_TOOLS_LIST_H
