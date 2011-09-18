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
    _api.list.simple.ctorSlice(this, sizeof(ItemT), &other, &range);
  }

  FOG_INLINE void _dtor()
  {
    _api.list.simple.dtor(this);
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
    return _api.list.simple.detach(this, sizeof(ItemT));
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  using ListUntyped::getCapacity;
  using ListUntyped::getLength;
  using ListUntyped::isEmpty;

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return _api.list.simple.reserve(this, sizeof(ItemT), capacity);
  }

  FOG_INLINE void squeeze()
  {
    _api.list.simple.squeeze(this, sizeof(ItemT));
  }

  FOG_INLINE err_t _growLeft(size_t length)
  {
    return _api.list.simple.growLeft(this, sizeof(ItemT), length);
  }

  FOG_INLINE err_t _growRight(size_t length)
  {
    return _api.list.simple.growRight(this, sizeof(ItemT), length);
  }

  FOG_INLINE ItemT* _prepare(uint32_t cntOp, size_t length)
  {
    return reinterpret_cast<ItemT*>(_api.list.simple.prepare(this, sizeof(ItemT), cntOp, length));
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
      "Fog::List<?>::getDataX() - Called on non-detached object.");

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
      return _api.list.simple.setAt(this, sizeof(ItemT), index, &item);
    else
      return _api.list.simple.setAt_4x(this, sizeof(ItemT), index, &item);
  }

  FOG_INLINE err_t setAtX(size_t index, const ItemT& item)
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::List<?>::setAtX() - Called on non-detached object.");
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
    _api.list.simple.clear(this);
  }

  FOG_INLINE void reset()
  {
    _api.list.simple.reset(this);
  }

  // --------------------------------------------------------------------------
  // [SetList]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setList(const List<ItemT>& other)
  {
    _api.list.simple.copy(this, &other);
    return ERR_OK;
  }

  FOG_INLINE err_t setList(const List<ItemT>& other, const Range& range)
  {
    return _api.list.simple.opList(this, sizeof(ItemT), CONTAINER_OP_REPLACE, &other, &range);
  }

  FOG_INLINE err_t setList(const ItemT* data, size_t dataLength)
  {
    return _api.list.simple.opData(this, sizeof(ItemT), CONTAINER_OP_REPLACE, data, dataLength);
  }

  // --------------------------------------------------------------------------
  // [Append / Concat]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t append(const ItemT& item)
  {
    if (sizeof(ItemT) % 4)
      return _api.list.simple.appendItem(this, sizeof(ItemT), &item);
    else
      return _api.list.simple.appendItem_4x(this, sizeof(ItemT), &item);
  }

  FOG_INLINE err_t concat(const List<ItemT>& other)
  {
    return _api.list.simple.opList(this, sizeof(ItemT), CONTAINER_OP_APPEND, &other, NULL);
  }

  FOG_INLINE err_t concat(const List<ItemT>& other, const Range& range)
  {
    return _api.list.simple.opList(this, sizeof(ItemT), CONTAINER_OP_APPEND, &other, &range);
  }

  FOG_INLINE err_t concat(const ItemT* data, size_t dataLength)
  {
    return _api.list.simple.opData(this, sizeof(ItemT), CONTAINER_OP_APPEND, data, dataLength);
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
      return _api.list.simple.insertItem(this, sizeof(ItemT), index, &item);
    else
      return _api.list.simple.insertItem_4x(this, sizeof(ItemT), index, &item);
  }

  // --------------------------------------------------------------------------
  // [Remove]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t removeRange(const Range& range)
  {
    return _api.list.simple.remove(this, sizeof(ItemT), &range);
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
    return _api.list.simple.replace(this, sizeof(ItemT), &range, &src, NULL);
  }

  FOG_INLINE err_t replace(const Range& range, const List<ItemT>& src, const Range& srcRange)
  {
    return _api.list.simple.replace(this, sizeof(ItemT), &range, &src, &srcRange);
  }

  // --------------------------------------------------------------------------
  // [Slice]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t slice(const Range& range)
  {
    return _api.list.simple.slice(this, sizeof(ItemT), &range);
  }

  // --------------------------------------------------------------------------
  // [Sort]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t sort(uint32_t sortOrder, CompareFunc compareFunc)
  {
    return _api.list.simple.sort(this, sizeof(ItemT), sortOrder, compareFunc);
  }

  FOG_INLINE err_t sort(uint32_t sortOrder, CompareExFunc compareFunc, const void* data)
  {
    return _api.list.simple.sortEx(this, sizeof(ItemT), sortOrder, compareFunc, data);
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
      return _api.list.simple.swapItems(this, sizeof(ItemT), index1, index2);
    else
      return _api.list.simple.swapItems_4x(this, sizeof(ItemT), index1, index2);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE void _copy(const List<ItemT>& other)
  {
    _api.list.simple.copy(this, &other);
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
    return _api.list.simple.sort(this, sizeof(ItemT), sortOrder, TypeFunc<ItemT>::getCompareFunc());
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
      return _api.list.untyped.indexOf_4B(this, NULL, &item);
    else if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 8)
      return _api.list.untyped.indexOf_8B(this, NULL, &item);
    else if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 16)
      return _api.list.untyped.indexOf_16B(this, NULL, &item);
    else
      return _indexOf(item);
  }

  //! @overload.
  FOG_INLINE size_t indexOf(const Range& range_, const ItemT& item) const
  {
    if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 4)
      return _api.list.untyped.indexOf_4B(this, &range_, &item);
    else if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 8)
      return _api.list.untyped.indexOf_8B(this, &range_, &item);
    else if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 16)
      return _api.list.untyped.indexOf_16B(this, &range_, &item);
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
      return _api.list.untyped.lastIndexOf_4B(this, NULL, &item);
    else if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 8)
      return _api.list.untyped.lastIndexOf_8B(this, NULL, &item);
    else if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 16)
      return _api.list.untyped.lastIndexOf_16B(this, NULL, &item);
    else
      return _lastIndexOf(item);
  }

  //! @overload.
  FOG_NO_INLINE size_t lastIndexOf(const Range& range_, const ItemT& item) const
  {
    if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 4)
      return _api.list.untyped.lastIndexOf_4B(this, &range_, &item);
    else if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 8)
      return _api.list.untyped.lastIndexOf_8B(this, &range_, &item);
    else if (TypeInfo<ItemT>::BIN_EQ && sizeof(ItemT) == 16)
      return _api.list.untyped.lastIndexOf_16B(this, &range_, &item);
    else
      return _lastIndexOf(range_, item);
  }
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
    _api.list.unknown.ctorSlice(this, ListVTable<ItemT>::getVTable(), &other, &range);
  }

  FOG_INLINE void _dtor()
  {
    _api.list.unknown.dtor(this, ListVTable<ItemT>::getVTable());
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
    return _api.list.unknown.detach(this, ListVTable<ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  using ListUntyped::getCapacity;
  using ListUntyped::getLength;
  using ListUntyped::isEmpty;

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return _api.list.unknown.reserve(this, ListVTable<ItemT>::getVTable(), capacity);
  }

  FOG_INLINE void squeeze()
  {
    _api.list.unknown.squeeze(this, ListVTable<ItemT>::getVTable());
  }

  FOG_INLINE err_t _growLeft(size_t length)
  {
    return _api.list.unknown.growLeft(this, ListVTable<ItemT>::getVTable(), length);
  }

  FOG_INLINE err_t _growRight(size_t length)
  {
    return _api.list.unknown.growRight(this, ListVTable<ItemT>::getVTable(), length);
  }

  FOG_INLINE ItemT* _prepare(uint32_t cntOp, size_t length)
  {
    return reinterpret_cast<ItemT*>(_api.list.unknown.prepare(this, ListVTable<ItemT>::getVTable(), cntOp, length));
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
      "Fog::List<?>::getDataX() - Called on non-detached object.");

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
      "Fog::List<?>::setAtX() - Called on non-detached object.");
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
    _api.list.unknown.clear(this, ListVTable<ItemT>::getVTable());
  }

  FOG_INLINE void reset()
  {
    _api.list.unknown.reset(this, ListVTable<ItemT>::getVTable());
  }

  // --------------------------------------------------------------------------
  // [SetList]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setList(const List<ItemT>& other)
  {
    _api.list.unknown.copy(this, ListVTable<ItemT>::getVTable(), &other);
    return ERR_OK;
  }

  FOG_INLINE err_t setList(const List<ItemT>& other, const Range& range)
  {
    return _api.list.unknown.opList(this, ListVTable<ItemT>::getVTable(), CONTAINER_OP_REPLACE, &other, &range);
  }

  FOG_INLINE err_t setList(const ItemT* data, size_t dataLength)
  {
    return _api.list.unknown.opData(this, ListVTable<ItemT>::getVTable(), CONTAINER_OP_REPLACE, data, dataLength);
  }

  // --------------------------------------------------------------------------
  // [Append / Concat]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t append(const ItemT& item)
  {
    ItemT* p = reinterpret_cast<ItemT*>(_api.list.unknown.prepareAppendItem(this, ListVTable<ItemT>::getVTable()));

    if (FOG_IS_NULL(p))
      return ERR_RT_OUT_OF_MEMORY;

    fog_new_p(p) ItemT(item);
    return ERR_OK;
  }

  FOG_INLINE err_t concat(const List<ItemT>& other)
  {
    return _api.list.unknown.opList(this, ListVTable<ItemT>::getVTable(), CONTAINER_OP_APPEND, &other, NULL);
  }

  FOG_INLINE err_t concat(const List<ItemT>& other, const Range& range)
  {
    return _api.list.unknown.opList(this, ListVTable<ItemT>::getVTable(), CONTAINER_OP_APPEND, &other, &range);
  }

  FOG_INLINE err_t concat(const ItemT* data, size_t dataLength)
  {
    return _api.list.unknown.opData(this, ListVTable<ItemT>::getVTable(), CONTAINER_OP_APPEND, data, dataLength);
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
    ItemT* p = reinterpret_cast<ItemT*>(_api.list.unknown.prepareInsertItem(this, ListVTable<ItemT>::getVTable(), index));

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
    return _api.list.unknown.remove(this, ListVTable<ItemT>::getVTable(), &range);
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
    return _api.list.unknown.replace(this, ListVTable<ItemT>::getVTable(), &range, &src, NULL);
  }

  FOG_INLINE err_t replace(const Range& range, const List<ItemT>& src, const Range& srcRange)
  {
    return _api.list.unknown.replace(this, ListVTable<ItemT>::getVTable(), &range, &src, &srcRange);
  }

  // --------------------------------------------------------------------------
  // [Slice]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t slice(const Range& range)
  {
    return _api.list.unknown.slice(this, ListVTable<ItemT>::getVTable(), &range);
  }

  // --------------------------------------------------------------------------
  // [Sort]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t sort(uint32_t sortOrder, CompareFunc compareFunc)
  {
    return _api.list.unknown.sort(this, ListVTable<ItemT>::getVTable(), sortOrder, compareFunc);
  }

  FOG_INLINE err_t sort(uint32_t sortOrder, CompareExFunc compareFunc, const void* data)
  {
    return _api.list.unknown.sortEx(this, ListVTable<ItemT>::getVTable(), sortOrder, compareFunc, data);
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
    _api.list.unknown.copy(this, ListVTable<ItemT>::getVTable(), &other);
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
    return _api.list.simple.sort(this, sizeof(ItemT), sortOrder, TypeFunc<ItemT>::getCompareFunc());
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
    _api.list.untyped.ctor(this);
  }

  FOG_INLINE List(const List& other)
  {
    _api.list.untyped.ctorCopy(this, &other);
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
