// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_LISTSTRING_H
#define _FOG_CORE_TOOLS_LISTSTRING_H

// [Dependencies]
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ===========================================================================
// [Fog::List<StringA>]
// ===========================================================================

template<>
struct List<StringA> : public ListUntyped
{
  // --------------------------------------------------------------------------
  // [Typedefs]
  // --------------------------------------------------------------------------

  //! @brief Item type.
  typedef StringA Item;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE List()
  {
    _api.list.stringa.ctor(this);
  }

  FOG_INLINE List(const List& other)
  {
    _api.list.untyped.ctorCopy(this, &other);
  }

  FOG_INLINE List(const List& other, const Range& range)
  {
    _api.list.stringa.ctorSlice(this, &other, &range);
  }

  explicit FOG_INLINE List(ListUntypedData* d)
  {
    _d = d;
  }

  FOG_INLINE ~List()
  {
    _api.list.stringa.dtor(this);
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
    return _api.list.stringa.detach(this);
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  using ListUntyped::getCapacity;
  using ListUntyped::getLength;
  using ListUntyped::isEmpty;

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return _api.list.stringa.reserve(this, capacity);
  }

  FOG_INLINE void squeeze()
  {
    _api.list.stringa.squeeze(this);
  }

  FOG_INLINE err_t _growLeft(size_t length)
  {
    return _api.list.stringa.growLeft(this, length);
  }

  FOG_INLINE err_t _growRight(size_t length)
  {
    return _api.list.stringa.growRight(this, length);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const StringA* getData() const
  {
    return reinterpret_cast<const StringA*>(_d->data);
  }

  FOG_INLINE StringA* getDataX()
  {
    FOG_ASSERT_X(ListUntyped::isDetached(),
      "Fog::List<Fog::StringA>::getDataX() - Not detached.");

    return reinterpret_cast<StringA*>(_d->data);
  }

  FOG_INLINE const StringA& getAt(size_t index) const
  {
    FOG_ASSERT_X(index < _d->length,
      "Fog::List<Fog::StringA>::getAt() - Index out of range.");

    return reinterpret_cast<const StringA*>(_d->data)[index];
  }

  FOG_INLINE err_t setAt(size_t index, const StubA& item)
  {
    FOG_ASSERT_X(index < _d->length,
      "Fog::List<Fog::StringA>::setAt() - Index out of range.");

    return _api.list.stringa.setAtStubA(this, index, &item);
  }

  FOG_INLINE err_t setAt(size_t index, const StringA& item)
  {
    FOG_ASSERT_X(index < _d->length,
      "Fog::List<Fog::StringA>::setAt() - Index out of range.");

    return _api.list.stringa.setAtStringA(this, index, &item);
  }

  FOG_INLINE err_t setAtX(size_t index, const StubA& item)
  {
    FOG_ASSERT_X(ListUntyped::isDetached(),
      "Fog::List<Fog::StringA>::setAtX() - Not detached.");
    FOG_ASSERT_X(index < _d->length,
      "Fog::List<Fog::StringA>::setAtX() - Index out of range.");

    return reinterpret_cast<StringA*>(_d->data)[index].set(item);
  }

  FOG_INLINE err_t setAtX(size_t index, const StringA& item)
  {
    FOG_ASSERT_X(ListUntyped::isDetached(),
      "Fog::List<Fog::StringA>::setAtX() - Not detached.");
    FOG_ASSERT_X(index < _d->length,
      "Fog::List<Fog::StringA>::setAtX() - Index out of range.");

    return reinterpret_cast<StringA*>(_d->data)[index].set(item);
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    _api.list.stringa.clear(this);
  }

  FOG_INLINE void reset()
  {
    _api.list.stringa.reset(this);
  }

  // --------------------------------------------------------------------------
  // [SetList]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setList(List<StringA>& other)
  {
    return _api.list.stringa.opList(this, CONTAINER_OP_REPLACE, &other, NULL);
  }

  FOG_INLINE err_t setList(List<StringA>& other, const Range& range)
  {
    return _api.list.stringa.opList(this, CONTAINER_OP_REPLACE, &other, &range);
  }

  // --------------------------------------------------------------------------
  // [Append / Concat]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t append(const StubA& item)
  {
    return _api.list.stringa.appendStubA(this, &item);
  }

  FOG_INLINE err_t append(const StringA& item)
  {
    return _api.list.stringa.appendStringA(this, &item);
  }

  FOG_INLINE err_t concat(const List<StringA>& other)
  {
    return _api.list.stringa.opList(this, CONTAINER_OP_APPEND, &other, NULL);
  }

  FOG_INLINE err_t concat(const List<StringA>& other, const Range& range)
  {
    return _api.list.stringa.opList(this, CONTAINER_OP_APPEND, &other, &range);
  }

  // --------------------------------------------------------------------------
  // [Prepend]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t prepend(const StubA& item)
  {
    return _api.list.stringa.insertStubA(this, 0, &item);
  }

  FOG_INLINE err_t prepend(const StringA& item)
  {
    return _api.list.stringa.insertStringA(this, 0, &item);
  }

  // --------------------------------------------------------------------------
  // [Insert]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t insert(size_t index, const StubA& item)
  {
    return _api.list.stringa.insertStubA(this, index, &item);
  }

  FOG_INLINE err_t insert(size_t index, const StringA& item)
  {
    return _api.list.stringa.insertStringA(this, index, &item);
  }

  // --------------------------------------------------------------------------
  // [Remove]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t removeRange(const Range& range)
  {
    return _api.list.stringa.remove(this, &range);
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

  FOG_INLINE err_t replace(const Range& range, const List<StringA>& src)
  {
    return _api.list.stringa.replace(this, &range, &src, NULL);
  }

  FOG_INLINE err_t replace(const Range& range, const List<StringA>& src, const Range& srcRange)
  {
    return _api.list.stringa.replace(this, &range, &src, &srcRange);
  }

  // --------------------------------------------------------------------------
  // [Slice]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t slice(const Range& range)
  {
    return _api.list.stringa.slice(this, &range);
  }

  // --------------------------------------------------------------------------
  // [Contains]
  // --------------------------------------------------------------------------

  FOG_INLINE bool contains(const StubA& item) const
  {
    return _api.list.stringa.indexOfStubA(this, NULL, &item) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const StringA& item) const
  {
    return _api.list.stringa.indexOfStringA(this, NULL, &item) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const Range& range, const StubA& item) const
  {
    return _api.list.stringa.indexOfStubA(this, &range, &item) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const Range& range, const StringA& item) const
  {
    return _api.list.stringa.indexOfStringA(this, &range, &item) != INVALID_INDEX;
  }

  // --------------------------------------------------------------------------
  // [IndexOf]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t indexOf(const StubA& item) const
  {
    return _api.list.stringa.indexOfStubA(this, NULL, &item);
  }

  FOG_INLINE size_t indexOf(const StringA& item) const
  {
    return _api.list.stringa.indexOfStringA(this, NULL, &item);
  }

  FOG_INLINE size_t indexOf(const Range& range, const StubA& item) const
  {
    return _api.list.stringa.indexOfStubA(this, &range, &item);
  }

  FOG_INLINE size_t indexOf(const Range& range, const StringA& item) const
  {
    return _api.list.stringa.indexOfStringA(this, &range, &item);
  }

  // --------------------------------------------------------------------------
  // [LastIndexOf]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t lastIndexOf(const StubA& item) const
  {
    return _api.list.stringa.lastIndexOfStubA(this, NULL, &item);
  }

  FOG_INLINE size_t lastIndexOf(const StringA& item) const
  {
    return _api.list.stringa.lastIndexOfStringA(this, NULL, &item);
  }

  FOG_INLINE size_t lastIndexOf(const Range& range, const StubA& item) const
  {
    return _api.list.stringa.lastIndexOfStubA(this, &range, &item);
  }

  FOG_INLINE size_t lastIndexOf(const Range& range, const StringA& item) const
  {
    return _api.list.stringa.lastIndexOfStringA(this, &range, &item);
  }

  // --------------------------------------------------------------------------
  // [Sort]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t sort(uint32_t sortOrder)
  {
    return _api.list.stringa.sort(this, sortOrder, StringA::getCompareFunc());
  }

  FOG_INLINE err_t sort(uint32_t sortOrder, CompareFunc compareFunc)
  {
    return _api.list.stringa.sort(this, sortOrder, compareFunc);
  }

  FOG_INLINE err_t sort(uint32_t sortOrder, CompareExFunc compareFunc, const void* data)
  {
    return _api.list.stringa.sortEx(this, sortOrder, compareFunc, data);
  }

  // --------------------------------------------------------------------------
  // [SwapItems]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t swapItems(size_t index1, size_t index2)
  {
    FOG_ASSERT_X(index1 < _d->length,
      "Fog::List<Fog::StringA>::swapItems() - Index1 out of range.");
    FOG_ASSERT_X(index2 < _d->length,
      "Fog::List<Fog::StringA>::swapItems() - Index2 out of range.");

    return _api.list.stringa.swapItems(this, index1, index2);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const List<StringA>& other) const
  {
    return _api.list.untyped.customEq(this, &other, sizeof(StringA), StringA::getEqFunc());
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE List<StringA>& operator=(const List<StringA>& other)
  {
    _api.list.stringa.opList(this, CONTAINER_OP_REPLACE, &other, NULL);
    return *this;
  }

  FOG_INLINE List<StringA>& operator+=(const StubA& item)
  {
    _api.list.stringa.appendStubA(this, &item);
    return *this;
  }

  FOG_INLINE List<StringA>& operator+=(const StringA& item)
  {
    _api.list.stringa.appendStringA(this, &item);
    return *this;
  }

  FOG_INLINE bool operator==(const List<StringA>& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const List<StringA>& other) const { return !eq(other); }

  FOG_INLINE const StringA& operator[](size_t index) const
  {
    FOG_ASSERT_X(index < _d->length,
      "Fog::List<Fog::StringA>::operator[] - Index out of range.");

    return reinterpret_cast<const StringA*>(_d->data)[index];
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const List<StringA>* a, const List<StringA>* b)
  {
    return _api.list.untyped.customEq(a, b, sizeof(StringA), StringA::getEqFunc());
  }
};

// ===========================================================================
// [Fog::List<StringW>]
// ===========================================================================

template<>
struct List<StringW> : public ListUntyped
{
  // --------------------------------------------------------------------------
  // [Typedefs]
  // --------------------------------------------------------------------------

  //! @brief Item type.
  typedef StringW Item;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE List()
  {
    _api.list.stringw.ctor(this);
  }

  FOG_INLINE List(const List<StringW>& other)
  {
    _api.list.untyped.ctorCopy(this, &other);
  }

  FOG_INLINE List(const List<StringW>& other, const Range& range)
  {
    _api.list.stringw.ctorSlice(this, &other, &range);
  }

  explicit FOG_INLINE List(ListUntypedData* d)
  {
    _d = d;
  }

  FOG_INLINE ~List()
  {
    _api.list.stringw.dtor(this);
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
    return _api.list.stringw.detach(this);
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  using ListUntyped::getCapacity;
  using ListUntyped::getLength;
  using ListUntyped::isEmpty;

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return _api.list.stringw.reserve(this, capacity);
  }

  FOG_INLINE void squeeze()
  {
    _api.list.stringw.squeeze(this);
  }

  FOG_INLINE err_t _growLeft(size_t length)
  {
    return _api.list.stringw.growLeft(this, length);
  }

  FOG_INLINE err_t _growRight(size_t length)
  {
    return _api.list.stringw.growRight(this, length);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const StringW* getData() const
  {
    return reinterpret_cast<const StringW*>(_d->data);
  }

  FOG_INLINE StringW* getDataX()
  {
    FOG_ASSERT_X(ListUntyped::isDetached(),
      "Fog::List<Fog::StringW>::getDataX() - Not detached.");

    return reinterpret_cast<StringW*>(_d->data);
  }

  FOG_INLINE const StringW& getAt(size_t index) const
  {
    FOG_ASSERT_X(index < _d->length,
      "Fog::List<Fog::StringW>::getAt() - Index out of range.");

    return reinterpret_cast<const StringW*>(_d->data)[index];
  }

  FOG_INLINE err_t setAt(size_t index, const Ascii8& item)
  {
    FOG_ASSERT_X(index < _d->length,
      "Fog::List<Fog::StringW>::setAt() - Index out of range.");

    return _api.list.stringw.setAtStubA(this, index, &item);
  }

  FOG_INLINE err_t setAt(size_t index, const StubW& item)
  {
    FOG_ASSERT_X(index < _d->length,
      "Fog::List<Fog::StringW>::setAt() - Index out of range.");

    return _api.list.stringw.setAtStubW(this, index, &item);
  }

  FOG_INLINE err_t setAt(size_t index, const StringW& item)
  {
    FOG_ASSERT_X(index < _d->length,
      "Fog::List<Fog::StringW>::setAt() - Index out of range.");

    return _api.list.stringw.setAtStringW(this, index, &item);
  }

  FOG_INLINE err_t setAtX(size_t index, const Ascii8& item)
  {
    FOG_ASSERT_X(ListUntyped::isDetached(),
      "Fog::List<Fog::StringW>::setAtX() - Not detached.");
    FOG_ASSERT_X(index < _d->length,
      "Fog::List<Fog::StringW>::setAtX() - Index out of range.");

    return reinterpret_cast<StringW*>(_d->data)[index].set(item);
  }

  FOG_INLINE err_t setAtX(size_t index, const StubW& item)
  {
    FOG_ASSERT_X(ListUntyped::isDetached(),
      "Fog::List<Fog::StringW>::setAtX() - Not detached.");
    FOG_ASSERT_X(index < _d->length,
      "Fog::List<Fog::StringW>::setAtX() - Index out of range.");

    return reinterpret_cast<StringW*>(_d->data)[index].set(item);
  }

  FOG_INLINE err_t setAtX(size_t index, const StringW& item)
  {
    FOG_ASSERT_X(ListUntyped::isDetached(),
      "Fog::List<Fog::StringW>::setAtX() - Not detached.");
    FOG_ASSERT_X(index < _d->length,
      "Fog::List<Fog::StringW>::setAtX() - Index out of range.");

    return reinterpret_cast<StringW*>(_d->data)[index].set(item);
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    _api.list.stringw.clear(this);
  }

  FOG_INLINE void reset()
  {
    _api.list.stringw.reset(this);
  }

  // --------------------------------------------------------------------------
  // [SetList]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setList(List<StringW>& other)
  {
    return _api.list.stringw.opList(this, CONTAINER_OP_REPLACE, &other, NULL);
  }

  FOG_INLINE err_t setList(List<StringW>& other, const Range& range)
  {
    return _api.list.stringw.opList(this, CONTAINER_OP_REPLACE, &other, &range);
  }

  // --------------------------------------------------------------------------
  // [Append / Concat]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t append(const Ascii8& item)
  {
    return _api.list.stringw.appendStubA(this, &item);
  }

  FOG_INLINE err_t append(const StubW& item)
  {
    return _api.list.stringw.appendStubW(this, &item);
  }

  FOG_INLINE err_t append(const StringW& item)
  {
    return _api.list.stringw.appendStringW(this, &item);
  }

  FOG_INLINE err_t concat(const List<StringW>& other)
  {
    return _api.list.stringw.opList(this, CONTAINER_OP_APPEND, &other, NULL);
  }

  FOG_INLINE err_t concat(const List<StringW>& other, const Range& range)
  {
    return _api.list.stringw.opList(this, CONTAINER_OP_APPEND, &other, &range);
  }

  // --------------------------------------------------------------------------
  // [Prepend]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t prepend(const Ascii8& item)
  {
    return _api.list.stringw.insertStubA(this, 0, &item);
  }

  FOG_INLINE err_t prepend(const StubW& item)
  {
    return _api.list.stringw.insertStubW(this, 0, &item);
  }

  FOG_INLINE err_t prepend(const StringW& item)
  {
    return _api.list.stringw.insertStringW(this, 0, &item);
  }

  // --------------------------------------------------------------------------
  // [Insert]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t insert(size_t index, const Ascii8& item)
  {
    return _api.list.stringw.insertStubA(this, index, &item);
  }

  FOG_INLINE err_t insert(size_t index, const StubW& item)
  {
    return _api.list.stringw.insertStubW(this, index, &item);
  }

  FOG_INLINE err_t insert(size_t index, const StringW& item)
  {
    return _api.list.stringw.insertStringW(this, index, &item);
  }

  // --------------------------------------------------------------------------
  // [Remove]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t removeRange(const Range& range)
  {
    return _api.list.stringw.remove(this, &range);
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

  FOG_INLINE err_t replace(const Range& range, const List<StringW>& src)
  {
    return _api.list.stringw.replace(this, &range, &src, NULL);
  }

  FOG_INLINE err_t replace(const Range& range, const List<StringW>& src, const Range& srcRange)
  {
    return _api.list.stringw.replace(this, &range, &src, &srcRange);
  }

  // --------------------------------------------------------------------------
  // [Slice]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t slice(const Range& range)
  {
    return _api.list.stringw.slice(this, &range);
  }

  // --------------------------------------------------------------------------
  // [Contains]
  // --------------------------------------------------------------------------

  FOG_INLINE bool contains(const Ascii8& item) const
  {
    return _api.list.stringw.indexOfStubA(this, NULL, &item) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const StubW& item) const
  {
    return _api.list.stringw.indexOfStubW(this, NULL, &item) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const StringW& item) const
  {
    return _api.list.stringw.indexOfStringW(this, NULL, &item) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const Range& range, const Ascii8& item) const
  {
    return _api.list.stringw.indexOfStubA(this, &range, &item) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const Range& range, const StubW& item) const
  {
    return _api.list.stringw.indexOfStubW(this, &range, &item) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const Range& range, const StringW& item) const
  {
    return _api.list.stringw.indexOfStringW(this, &range, &item) != INVALID_INDEX;
  }

  // --------------------------------------------------------------------------
  // [IndexOf]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t indexOf(const Ascii8& item) const
  {
    return _api.list.stringw.indexOfStubA(this, NULL, &item);
  }

  FOG_INLINE size_t indexOf(const StubW& item) const
  {
    return _api.list.stringw.indexOfStubW(this, NULL, &item);
  }

  FOG_INLINE size_t indexOf(const StringW& item) const
  {
    return _api.list.stringw.indexOfStringW(this, NULL, &item);
  }

  FOG_INLINE size_t indexOf(const Range& range, const Ascii8& item) const
  {
    return _api.list.stringw.indexOfStubA(this, &range, &item);
  }

  FOG_INLINE size_t indexOf(const Range& range, const StubW& item) const
  {
    return _api.list.stringw.indexOfStubW(this, &range, &item);
  }

  FOG_INLINE size_t indexOf(const Range& range, const StringW& item) const
  {
    return _api.list.stringw.indexOfStringW(this, &range, &item);
  }

  // --------------------------------------------------------------------------
  // [LastIndexOf]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t lastIndexOf(const Ascii8& item) const
  {
    return _api.list.stringw.lastIndexOfStubA(this, NULL, &item);
  }

  FOG_INLINE size_t lastIndexOf(const StubW& item) const
  {
    return _api.list.stringw.lastIndexOfStubW(this, NULL, &item);
  }

  FOG_INLINE size_t lastIndexOf(const StringW& item) const
  {
    return _api.list.stringw.lastIndexOfStringW(this, NULL, &item);
  }

  FOG_INLINE size_t lastIndexOf(const Range& range, const Ascii8& item) const
  {
    return _api.list.stringw.lastIndexOfStubA(this, &range, &item);
  }

  FOG_INLINE size_t lastIndexOf(const Range& range, const StubW& item) const
  {
    return _api.list.stringw.lastIndexOfStubW(this, &range, &item);
  }

  FOG_INLINE size_t lastIndexOf(const Range& range, const StringW& item) const
  {
    return _api.list.stringw.lastIndexOfStringW(this, &range, &item);
  }

  // --------------------------------------------------------------------------
  // [Sort]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t sort(uint32_t sortOrder)
  {
    return _api.list.stringw.sort(this, sortOrder, StringW::getCompareFunc());
  }

  FOG_INLINE err_t sort(uint32_t sortOrder, CompareFunc compareFunc)
  {
    return _api.list.stringw.sort(this, sortOrder, compareFunc);
  }

  FOG_INLINE err_t sort(uint32_t sortOrder, CompareExFunc compareFunc, const void* data)
  {
    return _api.list.stringw.sortEx(this, sortOrder, compareFunc, data);
  }

  // --------------------------------------------------------------------------
  // [SwapItems]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t swapItems(size_t index1, size_t index2)
  {
    FOG_ASSERT_X(index1 < _d->length,
      "Fog::List<Fog::StringW>::swapItems() - Index1 out of range.");
    FOG_ASSERT_X(index2 < _d->length,
      "Fog::List<Fog::StringW>::swapItems() - Index2 out of range.");

    return _api.list.stringw.swapItems(this, index1, index2);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const List<StringW>& other) const
  {
    return _api.list.untyped.customEq(this, &other, sizeof(StringW), StringW::getEqFunc());
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE List<StringW>& operator=(const List<StringW>& other)
  {
    _api.list.stringw.opList(this, CONTAINER_OP_REPLACE, &other, NULL);
    return *this;
  }

  FOG_INLINE List<StringW>& operator+=(const Ascii8& item)
  {
    _api.list.stringw.appendStubA(this, &item);
    return *this;
  }

  FOG_INLINE List<StringW>& operator+=(const StubW& item)
  {
    _api.list.stringw.appendStubW(this, &item);
    return *this;
  }

  FOG_INLINE List<StringW>& operator+=(const StringW& item)
  {
    _api.list.stringw.appendStringW(this, &item);
    return *this;
  }

  FOG_INLINE bool operator==(const List<StringW>& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const List<StringW>& other) const { return !eq(other); }

  FOG_INLINE const StringW& operator[](size_t index) const
  {
    FOG_ASSERT_X(index < _d->length,
      "Fog::List<Fog::StringW>::operator[] - Index out of range.");

    return reinterpret_cast<const StringW*>(_d->data)[index];
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const List<StringW>* a, const List<StringW>* b)
  {
    return _api.list.untyped.customEq(a, b, sizeof(StringW), StringW::getEqFunc());
  }
};

// ============================================================================
// [Fog::StringA - Implemented-Later]
// ============================================================================

FOG_INLINE List<StringA> StringA::split(char sep, uint32_t splitBehavior, uint32_t cs)
{
  List<StringA> t;
  _api.stringa.splitChar(&t, CONTAINER_OP_APPEND, this, NULL, sep, splitBehavior, cs);
  return t;
}

FOG_INLINE List<StringA> StringA::split(const StringA& sep, uint32_t splitBehavior, uint32_t cs)
{
  List<StringA> t;
  _api.stringa.splitStringA(&t, CONTAINER_OP_APPEND, this, NULL, &sep, splitBehavior, cs);
  return t;
}

FOG_INLINE List<StringA> StringA::split(const RegExpA& re, uint32_t splitBehavior)
{
  List<StringA> t;
  _api.stringa.splitRegExpA(&t, CONTAINER_OP_APPEND, this, NULL, &re, splitBehavior);
  return t;
}

FOG_INLINE List<StringA> StringA::split(const Range& range, char sep, uint32_t splitBehavior, uint32_t cs)
{
  List<StringA> t;
  _api.stringa.splitChar(&t, CONTAINER_OP_APPEND, this, &range, sep, splitBehavior, cs);
  return t;
}

FOG_INLINE List<StringA> StringA::split(const Range& range, const StringA& sep, uint32_t splitBehavior, uint32_t cs)
{
  List<StringA> t;
  _api.stringa.splitStringA(&t, CONTAINER_OP_APPEND, this, &range, &sep, splitBehavior, cs);
  return t;
}

FOG_INLINE List<StringA> StringA::split(const Range& range, const RegExpA& re, uint32_t splitBehavior)
{
  List<StringA> t;
  _api.stringa.splitRegExpA(&t, CONTAINER_OP_APPEND, this, &range, &re, splitBehavior);
  return t;
}

// ============================================================================
// [Fog::StringW - Implemented-Later]
// ============================================================================

FOG_INLINE List<StringW> StringW::split(CharW sep, uint32_t splitBehavior, uint32_t cs)
{
  List<StringW> t;
  _api.stringw.splitChar(&t, CONTAINER_OP_APPEND, this, NULL, sep._value, splitBehavior, cs);
  return t;
}

FOG_INLINE List<StringW> StringW::split(const StringW& sep, uint32_t splitBehavior, uint32_t cs)
{
  List<StringW> t;
  _api.stringw.splitStringW(&t, CONTAINER_OP_APPEND, this, NULL, &sep, splitBehavior, cs);
  return t;
}

FOG_INLINE List<StringW> StringW::split(const RegExpW& re, uint32_t splitBehavior)
{
  List<StringW> t;
  _api.stringw.splitRegExpW(&t, CONTAINER_OP_APPEND, this, NULL, &re, splitBehavior);
  return t;
}

FOG_INLINE List<StringW> StringW::split(const Range& range, CharW sep, uint32_t splitBehavior, uint32_t cs)
{
  List<StringW> t;
  _api.stringw.splitChar(&t, CONTAINER_OP_APPEND, this, &range, sep._value, splitBehavior, cs);
  return t;
}

FOG_INLINE List<StringW> StringW::split(const Range& range, const StringW& sep, uint32_t splitBehavior, uint32_t cs)
{
  List<StringW> t;
  _api.stringw.splitStringW(&t, CONTAINER_OP_APPEND, this, &range, &sep, splitBehavior, cs);
  return t;
}

FOG_INLINE List<StringW> StringW::split(const Range& range, const RegExpW& re, uint32_t splitBehavior)
{
  List<StringW> t;
  _api.stringw.splitRegExpW(&t, CONTAINER_OP_APPEND, this, &range, &re, splitBehavior);
  return t;
}

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_LISTSTRING_H
