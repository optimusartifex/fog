// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_COLLECTION_LIST_H
#define _FOG_CORE_COLLECTION_LIST_H

// [Dependencies]
#include <Fog/Core/Collection/Algorithms.h>
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/SequenceInfo.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Global/Swap.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Range.h>

namespace Fog {

//! @addtogroup Fog_Core_Collection
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

template<typename T>
struct List;

// ===========================================================================
// [Fog::ListData]
// ===========================================================================

//! @brief Data used in @c List<> container.
struct FOG_NO_EXPORT ListData
{
  // [Reference Count]

  FOG_INLINE ListData* ref() const
  {
    refCount.inc();
    return const_cast<ListData*>(this);
  }

  // [Data Access]

  //! @brief Get mutable void* data pointer.
  FOG_INLINE char* pstart() { return reinterpret_cast<char*>(this) + sizeof(ListData); }

  //! @brief Get const void* data pointer.
  FOG_INLINE const char* pstart() const { return reinterpret_cast<const char*>(this) + sizeof(ListData); }

  // [Members]

  //! @brief Reference count.
  mutable Atomic<sysuint_t> refCount;

  //! @brief Capacity.
  sysuint_t capacity;

  //! @brief Elements in list.
  sysuint_t length;

  //! @brief Start index in data.
  //!
  //! In Fog::List<T>::Data is startIndex always equal to 0.
  //!
  //! In Fog::List<T>::Data is startIndex variable.
  sysuint_t startIndex;

  //! @brief Compatibility with ListBase::UntypedData::endIndex
  //!
  //! In Fog::List<T>::Data is endIndex always equal to 'length'.
  //!
  //! In Fog::List<T>::Data is endIndex variable, but never lower than
  //! 'startIndex'. Also endIndex - startIndex is equal to length and
  //! endIndex is never larger than capacity.
  sysuint_t endIndex;

  //! @brief Pointer to data (adjusted by @c startIndex * sizeof(T)).
  char* p;
};

// ===========================================================================
// [Fog::ListUntyped]
// ===========================================================================

//! @brief Abstract @c List<T> container, do not use directly.
struct FOG_NO_EXPORT ListUntyped
{
  _FOG_CLASS_D(ListData)
};

// ============================================================================
// [Fog::ListPrivate]
// ============================================================================

struct FOG_API ListPrivate_
{
  // [Primitive Types]

  static err_t p_detach(ListUntyped* self, sysuint_t typeSize);
  static err_t p_reserve(ListUntyped* self, sysuint_t typeSize, sysuint_t to);
  static err_t p_reserve(ListUntyped* self, sysuint_t typeSize, sysuint_t left, sysuint_t right);
  static err_t p_resize(ListUntyped* self, sysuint_t typeSize, sysuint_t to);
  static err_t p_reserveoptimal(ListUntyped* self, sysuint_t typeSize, sysuint_t leftBy, sysuint_t rightBy);

  static void p_squeeze(ListUntyped* self, sysuint_t typeSize);
  static void p_reset(ListUntyped* self);
  static void p_clear(ListUntyped* self);

  static err_t p_assign(ListUntyped* self, sysuint_t typeSize, const void* data, sysuint_t length);
  static err_t p_append(ListUntyped* self, sysuint_t typeSize, const void* data, sysuint_t length);
  static err_t p_insert(ListUntyped* self, sysuint_t typeSize, sysuint_t index, const void* data, sysuint_t length);
  static err_t p_remove(ListUntyped* self, sysuint_t typeSize, const Range& range);
  static err_t p_replace(ListUntyped* self, sysuint_t typeSize, const Range& range, const void* data, sysuint_t length);

  static void p_deref(ListData* d);

  // [Class Types]

  static err_t c_detach(ListUntyped* self, const SequenceInfoVTable* vtable);
  static err_t c_reserve(ListUntyped* self, const SequenceInfoVTable* vtable, sysuint_t to);
  static err_t c_reserve(ListUntyped* self, const SequenceInfoVTable* vtable, sysuint_t left, sysuint_t right);
  static err_t c_resize(ListUntyped* self, const SequenceInfoVTable* vtable, sysuint_t to);
  static err_t c_reserveoptimal(ListUntyped* self, const SequenceInfoVTable* vtable, sysuint_t left, sysuint_t right);

  static void c_squeeze(ListUntyped* self, const SequenceInfoVTable* vtable);
  static void c_reset(ListUntyped* self, const SequenceInfoVTable* vtable);
  static void c_clear(ListUntyped* self, const SequenceInfoVTable* vtable);

  static err_t c_assign(ListUntyped* self, const SequenceInfoVTable* vtable, const void* data, sysuint_t length);
  static err_t c_append(ListUntyped* self, const SequenceInfoVTable* vtable, const void* data, sysuint_t length);
  static err_t c_insert(ListUntyped* self, const SequenceInfoVTable* vtable, sysuint_t index, const void* data, sysuint_t length);
  static err_t c_remove(ListUntyped* self, const SequenceInfoVTable* vtable, const Range& range);
  static err_t c_replace(ListUntyped* self, const SequenceInfoVTable* vtable, const Range& range, const void* data, sysuint_t length);

  static void c_deref(ListData* d, const SequenceInfoVTable* vtable);

  // [Data]

  static ListData* d_alloc(sysuint_t typeSize, sysuint_t capacity);
  static sysuint_t d_getSize(sysuint_t typeSize, sysuint_t capacity);

  static Static<ListData> _dnull;
};

// 0 = PrimitiveType
// 1 = MoveableType
// 2 = ClassType
template<typename T, uint __TypeInfo__>
struct FOG_NO_EXPORT ListPrivate_T {};

template<typename T>
struct FOG_NO_EXPORT ListPrivate_T<T, 0> : public ListPrivate_
{
  typedef bool (*ElementEqFn)(const T* a, const T* b);
  typedef int (*ElementCompareFn)(const T* a, const T* b);

  // List<T> functions
  static FOG_INLINE err_t detach(List<T>* self)
  { return p_detach(reinterpret_cast<ListUntyped*>(self), sizeof(T)); }

  static FOG_INLINE err_t reserve(List<T>* self, sysuint_t to)
  { return p_reserve(reinterpret_cast<ListUntyped*>(self), sizeof(T), to); }

  static FOG_INLINE err_t resize(List<T>* self, sysuint_t to)
  { return p_resize(reinterpret_cast<ListUntyped*>(self), sizeof(T), to); }

  static FOG_INLINE void squeeze(List<T>* self)
  { p_squeeze(reinterpret_cast<ListUntyped*>(self), sizeof(T)); }

  static FOG_INLINE void clear(List<T>* self)
  { p_clear(reinterpret_cast<ListUntyped*>(self)); }

  static FOG_INLINE void reset(List<T>* self)
  { p_reset(reinterpret_cast<ListUntyped*>(self)); }

  static FOG_INLINE void deref(ListData* d)
  { p_deref(d); }

  static FOG_INLINE err_t reserve(List<T>* self, sysuint_t left, sysuint_t right)
  { return p_reserve(reinterpret_cast<ListUntyped*>(self), sizeof(T), left, right); }

  static FOG_INLINE err_t assign(List<T>* self, const T* data, sysuint_t length)
  { return p_assign(reinterpret_cast<ListUntyped*>(self), sizeof(T), (const void*)data, length); }

  static FOG_INLINE err_t append(List<T>* self, const T* data, sysuint_t length)
  { return p_append(reinterpret_cast<ListUntyped*>(self), sizeof(T), (const void*)data, length); }

  static FOG_INLINE err_t insert(List<T>* self, sysuint_t index, const T* data, sysuint_t length)
  { return p_insert(reinterpret_cast<ListUntyped*>(self), sizeof(T), index, (const void*)data, length); }

  static FOG_INLINE err_t remove(List<T>* self, const Range& range)
  { return p_remove(reinterpret_cast<ListUntyped*>(self), sizeof(T), range); }

  static FOG_INLINE err_t replace(List<T>* self, const Range& range, const T* data, sysuint_t length)
  { return p_replace(reinterpret_cast<ListUntyped*>(self), sizeof(T), range, (const void*)data, length); }
};

template<typename T>
struct FOG_NO_EXPORT ListPrivate_T<T, 1> : public ListPrivate_
{
  typedef bool (*ElementEqFn)(const T* a, const T* b);
  typedef int (*ElementCompareFn)(const T* a, const T* b);

  // List<T> functions
  static FOG_INLINE err_t detach(List<T>* self)
  { return c_detach(reinterpret_cast<ListUntyped*>(self), &SequenceInfo<T>::seqvtable); }

  static FOG_INLINE err_t reserve(List<T>* self, sysuint_t to)
  { return c_reserve(reinterpret_cast<ListUntyped*>(self), &SequenceInfo<T>::seqvtable, to); }

  static FOG_INLINE err_t resize(List<T>* self, sysuint_t to)
  { return c_resize(reinterpret_cast<ListUntyped*>(self), &SequenceInfo<T>::seqvtable, to); }

  static FOG_INLINE void squeeze(List<T>* self)
  { c_squeeze(reinterpret_cast<ListUntyped*>(self), &SequenceInfo<T>::seqvtable); }

  static FOG_INLINE void clear(List<T>* self)
  { c_clear(reinterpret_cast<ListUntyped*>(self), &SequenceInfo<T>::seqvtable); }

  static FOG_INLINE void reset(List<T>* self)
  { c_reset(reinterpret_cast<ListUntyped*>(self), &SequenceInfo<T>::seqvtable); }

  static FOG_INLINE void deref(ListData* d)
  { c_deref(d, &SequenceInfo<T>::seqvtable); }

  static FOG_INLINE err_t reserve(List<T>* self, sysuint_t left, sysuint_t right)
  { return c_reserve(reinterpret_cast<ListUntyped*>(self), SequenceInfo<T>::seqvtable, left, right); }

  static FOG_INLINE err_t assign(List<T>* self, const T* data, sysuint_t length)
  { return c_assign(reinterpret_cast<ListUntyped*>(self), &SequenceInfo<T>::seqvtable, (const void*)data, length); }

  static FOG_INLINE err_t append(List<T>* self, const T* data, sysuint_t length)
  { return c_append(reinterpret_cast<ListUntyped*>(self), &SequenceInfo<T>::seqvtable, (const void*)data, length); }

  static FOG_INLINE err_t insert(List<T>* self, sysuint_t index, const T* data, sysuint_t length)
  { return c_insert(reinterpret_cast<ListUntyped*>(self), &SequenceInfo<T>::seqvtable, index, (const void*)data, length); }

  static FOG_INLINE err_t remove(List<T>* self, const Range& range)
  { return c_remove(reinterpret_cast<ListUntyped*>(self), &SequenceInfo<T>::seqvtable, range); }

  static FOG_INLINE err_t replace(List<T>* self, const Range& range, const T* data, sysuint_t length)
  { return c_replace(reinterpret_cast<ListUntyped*>(self), &SequenceInfo<T>::seqvtable, range, (const void*)data, length); }
};

template<typename T>
struct FOG_NO_EXPORT ListPrivate_T<T, 2> : public ListPrivate_T<T, 1> {};

template<typename T>
struct FOG_NO_EXPORT ListPrivate : public ListPrivate_T<T, TypeInfo<T>::TYPE> {};

// ===========================================================================
// [Fog::List<T>]
// ===========================================================================

//! @brief List is container template similar to std::vector<> or std::deque<>.
//!
//! This implementation is based on implicit sharing and copy-on-write.
//! Another great feature is that code generated by compiler should be
//! smaller than code needed for std::vector<> or std::deque<>, because
//! Fog::List uses some tricks to do that (exported symbols for basic types
//! in library and intelligent template design for primitive and moveable types.
//!
//! Use List<T> as an universal list container.
template<typename T>
struct FOG_NO_EXPORT List
{
  // --------------------------------------------------------------------------
  // [Value Type]
  // --------------------------------------------------------------------------

  //! @brief Type of value (template T type).
  typedef T ValueType;

  // --------------------------------------------------------------------------
  // [Function Prototypes]
  // --------------------------------------------------------------------------

  typedef bool (*ElementEqFn)(const T* a, const T* b);
  typedef int (*ElementCompareFn)(const T* a, const T* b);

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE List() : _d(ListPrivate_::_dnull->ref()) {}
  FOG_INLINE List(const List& other) : _d(other._d->ref()) {}

  explicit FOG_INLINE List(ListData* d) : _d(d) {}

  FOG_INLINE ~List() { ListPrivate<T>::deref(_d); }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getRefCount().
  FOG_INLINE sysuint_t getRefCount() const { return _d->refCount.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return getRefCount() == 1; }
  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach() { return isDetached() ? (err_t)ERR_OK : ListPrivate<T>::detach(this); }

  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::isNull().
  FOG_INLINE bool isNull() const { return _d == ListPrivate_::_dnull.instancep(); }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  FOG_INLINE sysuint_t getLength() const { return _d->length; }
  FOG_INLINE sysuint_t getCapacity() const { return _d->capacity; }

  FOG_INLINE bool isEmpty() const { return _d->length == 0; }

  FOG_INLINE err_t reserve(sysuint_t to)
  { return ListPrivate<T>::reserve(this, to); }

  FOG_INLINE void reserve(sysuint_t left, sysuint_t right)
  { return ListPrivate<T>::reserve(this, left, right); }

  FOG_INLINE err_t resize(sysuint_t to)
  { return ListPrivate<T>::resize(this, to); }

  FOG_INLINE void squeeze()
  { ListPrivate<T>::squeeze(this); }

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  FOG_INLINE const T* getData() const
  {
    return reinterpret_cast<const T*>(_d->p);
  }

  FOG_INLINE T* getDataX()
  {
    FOG_ASSERT_X(isDetached(), "Fog::List<T>::getDataX() - Called on non-detached object.");
    return reinterpret_cast<T*>(_d->p);
  }

  FOG_INLINE const T& at(sysuint_t index) const
  {
    FOG_ASSERT_X(index < _d->length, "Fog::List<T>::at() - Index out of range.");
    return reinterpret_cast<const T*>(_d->p)[index];
  }

  // --------------------------------------------------------------------------
  // [Manipulation]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t assign(const T& element)
  { return ListPrivate<T>::assign(this, &element, 1); }

  FOG_INLINE err_t assign(const T* elements, sysuint_t length)
  { return ListPrivate<T>::assign(this, elements, length); }

  FOG_INLINE err_t assign(const List<T>& other)
  { ListPrivate<T>::deref(atomicPtrXchg(&_d, other._d->ref())); return ERR_OK; }

  FOG_INLINE err_t set(sysuint_t index, const T& element)
  {
    FOG_ASSERT_X(index < _d->length, "Fog::List<T>::set() - Index out of range.");

    err_t err = detach();
    if (err) return err;

    reinterpret_cast<T*>(_d->p)[index] = element;
    return ERR_OK;
  }

  FOG_INLINE err_t append(const T& element)
  { return ListPrivate<T>::append(this, &element, 1); }

  FOG_INLINE err_t append(const T* data, sysuint_t length)
  { return ListPrivate<T>::append(this, data, length); }

  FOG_INLINE err_t append(const List<T>& other)
  { List<T> t(other); return append(t.getData(), t.getLength()); }

  FOG_INLINE err_t prepend(const T& element)
  { return ListPrivate<T>::insert(this, 0, &element, 1); }

  FOG_INLINE err_t prepend(const T* data, sysuint_t length)
  { return ListPrivate<T>::insert(this, 0, data, length); }

  FOG_INLINE err_t prepend(const List<T>& other)
  { List<T> t(other); return prepend(t.getData(), t.getLength()); }

  FOG_INLINE err_t insert(sysuint_t index, const T& element)
  { return ListPrivate<T>::insert(this, index, &element, 1); }

  FOG_INLINE err_t insert(sysuint_t index, const T* data, sysuint_t length)
  { return ListPrivate<T>::insert(this, index, data, length); }

  FOG_INLINE err_t insert(sysuint_t index, const List<T>& other)
  { List<T> t(other); return insert(index, t.getData(), t.getLength()); }

  FOG_INLINE err_t remove(const T& element)
  { return removeAt(indexOf(element)); }

  FOG_INLINE err_t removeAt(sysuint_t index)
  { return ListPrivate<T>::remove(this, Range(index, index + 1)); }

  FOG_INLINE err_t removeAt(const Range& range)
  { return ListPrivate<T>::remove(this, range); }

  FOG_INLINE bool removeFirst()
  { return removeAt(0, 1) == 1; }

  FOG_INLINE bool removeLast()
  { sysuint_t len = getLength(); return removeAt(Range(len - 1, len)) == 1; }

  FOG_INLINE err_t replace(sysuint_t index, sysuint_t range, const T* data, sysuint_t length)
  { return ListPrivate<T>::replace(this, index, range, data, length); }

  FOG_INLINE err_t replace(sysuint_t index, sysuint_t range, const List<T>& other)
  { List<T> t(other); return replace(index, range, t.getData(), t.getLength()); }

  T take(sysuint_t index)
  {
    FOG_ASSERT_X(index < _d->length, "Fog::List<T>::takeAt() - Index out of range.");

    T result = reinterpret_cast<T*>(_d->p)[index];
    removeAt(index);
    return result;
  }

  FOG_INLINE T takeFirst() { return take(0); }
  FOG_INLINE T takeLast() { return take(getLength() - 1); }

  FOG_INLINE T top() const { return at(getLength() - 1); }
  FOG_INLINE void pop() { sysuint_t len = getLength(); removeAt(Range(len - 1, len)); }

  FOG_INLINE void clear()
  { ListPrivate<T>::clear(this); }

  FOG_INLINE void reset()
  { ListPrivate<T>::reset(this); }

  //! @brief Return first index of element @a element in list, otherwise
  //! INVALID_INDEX.
  FOG_NO_INLINE sysuint_t indexOf(const T& element) const
  {
    const ListData* d = _d;
    sysuint_t i, length = d->length;

    const T* cur = reinterpret_cast<const T*>(d->p);
    for (i = 0; i < length; i++, cur++)
    {
      if (cur[0] == element) return i;
    }

    return INVALID_INDEX;
  }

  //! @overload.
  FOG_NO_INLINE sysuint_t indexOf(const T& element, const Range& range_) const
  {
    const ListData* d = _d;
    sysuint_t i, length = d->length;

    sysuint_t rstart = range_.getStart();
    sysuint_t rend = Math::min(range_.getEnd(), length);
    if (rstart >= rend) return INVALID_INDEX;

    const T* cur = reinterpret_cast<const T*>(d->p) + rstart;
    for (i = rstart; i < rend; i++, cur++)
    {
      if (cur[0] == element) return i;
    }

    return INVALID_INDEX;
  }

  //! @brief Return last index of element @a element in list, otherwise INVALID_INDEX.
  FOG_NO_INLINE sysuint_t lastIndexOf(const T& element) const
  {
    const ListData* d = _d;

    sysuint_t i = d->length - 1;
    const T* cur = reinterpret_cast<const T*>(d->p) + i;

    for (; i != INVALID_INDEX; i--, cur--)
    {
      if (cur[0] == element) return i;
    }
    return INVALID_INDEX;
  }

  //! @overload.
  FOG_NO_INLINE sysuint_t lastIndexOf(const T& element, const Range& range_) const
  {
    const ListData* d = _d;
    sysuint_t i, length = d->length;

    sysuint_t rstart = range_.getStart();
    sysuint_t rend = Math::min(range_.getEnd(), length);
    if (rstart >= rend) return INVALID_INDEX;

    rstart--;
    rend--;
    const T* cur = reinterpret_cast<const T*>(d->p) + rend;

    for (;;)
    {
      if (cur[0] == element) return i;
      if (i == rstart) break;
      i--; cur--;
    }

    return INVALID_INDEX;
  }

  //! @brief Get whether list contains element @a element.
  FOG_INLINE bool contains(const T& element) const
  { return indexOf(element) != INVALID_INDEX; }

  //! @overload
  FOG_INLINE bool contains(const T& element, const Range& range) const
  { return indexOf(element, range) != INVALID_INDEX; }

  FOG_NO_INLINE err_t swap(sysuint_t index1, sysuint_t index2)
  {
    FOG_ASSERT_X(index1 < _d->length, "Fog::List<T>::swap - Index1 out of range.");
    FOG_ASSERT_X(index2 < _d->length, "Fog::List<T>::swap - Index2 out of range.");

    FOG_RETURN_ON_ERROR(detach());

    ListData* d = _d;

    T* p = reinterpret_cast<T*>(d->p);
    T t = p[index1];
    p[index1] = p[index2];
    p[index2] = t;

    return ERR_OK;
  }

  FOG_NO_INLINE err_t sort()
  {
    return sort(TypeCmp<T>::compare);
  }

  FOG_NO_INLINE err_t sort(ElementCompareFn compar)
  {
    FOG_RETURN_ON_ERROR(detach());

    ListData* d = _d;
    Algorithms::qsort(d->p, d->length, sizeof(T), (Algorithms::CompareFn)compar);
    return ERR_OK;
  }

  FOG_NO_INLINE err_t reverse()
  {
    FOG_RETURN_ON_ERROR(detach());

    ListData* d = _d;
    sysuint_t len = d->length;

    T* a = reinterpret_cast<T*>(d->p);
    T* b = reinterpret_cast<T*>(d->p) + (len - 1);

    while (a < b)
    {
      T t(*a);
      *a = *b;
      *b = a;

      a++;
      b--;
    }

    return ERR_OK;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE List<T>& operator=(const List<T>& other)
  { assign(other); return *this; }

  FOG_INLINE List<T>& operator+=(const T& element)
  { append(element); return *this; }

  FOG_INLINE List<T>& operator+=(const List<T>& other)
  { append(other); return *this; }

  FOG_INLINE const T& operator[](sysuint_t index) const
  { return at(index); }

  // --------------------------------------------------------------------------
  // [Iterators]
  // --------------------------------------------------------------------------

  //! @brief Read only iterator.
  struct FOG_NO_EXPORT ConstIterator
  {
    const List<T>* _owner;
    const T* _beg;
    const T* _cur;
    const T* _end;
    bool _isValid;

    //! @brief Creates the new list iterator.
    FOG_INLINE ConstIterator(const List<T>& owner) { _owner = &owner; init(); }
    //! @brief Creates the new list iterator.
    FOG_INLINE ConstIterator(const List<T>* owner) { _owner = owner; init(); }
    //! @brief Destroys the list iterator.
    FOG_INLINE ~ConstIterator() {}

    //! @brief Go to the start of list.
    FOG_INLINE ConstIterator& toStart() { _cur = _beg; _isValid = (_cur <= _end); return *this; }
    //! @brief Go to the end of list.
    FOG_INLINE ConstIterator& toEnd() { _cur = _end; _isValid = (_cur >= _beg); return *this; }
    //! @brief Go to the specified index.
    FOG_INLINE ConstIterator& toIndex(sysuint_t index) { _cur = _beg + index; _isValid = (index < _owner->_d->length); return *this; }

    //! @brief Go to the next element.
    FOG_INLINE ConstIterator& toNext() { _cur++; _isValid = (_cur <= _end); return *this; }
    //! @brief Go to the next element.
    FOG_INLINE ConstIterator& toPrevious() { _cur--; _isValid = (_cur >= _beg); return *this; }

    //! @brief Returns @c true if the iterator is at the start of list.
    FOG_INLINE bool atStart() { return _cur == _beg; }
    //! @brief Returns @c true if the iterator is at the end of list.
    FOG_INLINE bool atEnd() { return _cur == _end; }
    //! @brief Returns @c true if the iterator is at the specified index @c index.
    FOG_INLINE bool atIndex(sysuint_t index) { return _cur == _beg + index; }

    //! @brief Returns @c true if iterator position is valid in both sides.
    FOG_INLINE bool isValid() const { return _isValid; }

    //! @brief Returns current index.
    FOG_INLINE sysuint_t index() const { return (sysuint_t)(_cur - _beg); }

    //! @brief Returns current element.
    FOG_INLINE const T& value() const { return *_cur; }

    // [Operator Overload]

    //! @brief Standard operator that is like @c value().
    FOG_INLINE const T& operator*() const { return *_cur; }

    FOG_INLINE ConstIterator& operator++() { _cur++; _isValid = (_cur <= _end); return *this; }
    FOG_INLINE ConstIterator& operator--() { _cur--; _isValid = (_cur >= _beg); return *this; }
    FOG_INLINE ConstIterator& operator+=(int n) { _cur += n; _isValid = (_cur <= _end); return *this; }
    FOG_INLINE ConstIterator& operator-=(int n) { _cur -= n; _isValid = (_cur >= _beg); return *this; }

    ConstIterator& init(sysuint_t i = 0)
    {
      ListData* d = _owner->_d;

      _beg = reinterpret_cast<const T*>(d->p);
      _cur = _beg + i;
      _end = _beg + d->length - 1;

      _isValid = (_cur >= _beg && _cur <= _end);

      return *this;
    }
  };

  //! @brief Read / Write iterator.
  struct FOG_NO_EXPORT MutableIterator
  {
    List<T>* _owner;
    T* _beg;
    T* _cur;
    T* _end;
    bool _isValid;

    //! @brief Creates the new list iterator.
    FOG_INLINE MutableIterator(List<T>& owner) { _owner = &owner; _owner->detach(); init(); }
    //! @brief Creates the new list iterator.
    FOG_INLINE MutableIterator(List<T>* owner) { _owner = owner; _owner->detach(); init(); }
    //! @brief Destroys the list iterator.
    FOG_INLINE ~MutableIterator() {}

    //! @brief Go to the start of list.
    FOG_INLINE MutableIterator& toStart() { _cur = _beg; _isValid = (_cur <= _end); return *this; }
    //! @brief Go to the end of list.
    FOG_INLINE MutableIterator& toEnd() { _cur = _end; _isValid = (_cur >= _beg); return *this; }
    //! @brief Go to the specified index.
    FOG_INLINE MutableIterator& toIndex(sysuint_t index) { _cur = _beg + index; _isValid = (index < _owner->_d->length); return *this; }

    //! @brief Go to the next element.
    FOG_INLINE MutableIterator& toNext() { _cur++; _isValid = (_cur <= _end); return *this; }
    //! @brief Go to the next element.
    FOG_INLINE MutableIterator& toPrevious() { _cur--; _isValid = (_cur <= _end); return *this; }

    //! @brief Returns @c true if the iterator is at the start of list.
    FOG_INLINE bool atStart() { return _cur == _beg; }
    //! @brief Returns @c true if the iterator is at the end of list.
    FOG_INLINE bool atEnd() { return _cur == _end; }
    //! @brief Returns @c true if the iterator is at the specified index @c index.
    FOG_INLINE bool atIndex(sysuint_t index) { return _cur == (_beg + index); }

    //! @brief Returns @c true if iterator position is valid in both sides.
    FOG_INLINE bool isValid() const { return _isValid; }

    //! @brief Returns current index.
    FOG_INLINE sysuint_t index() const { return (sysuint_t)(_cur - _beg); }

    //! @brief Returns current element.
    FOG_INLINE T& value() const { return *_cur; }

    // [Operator Overload]

    //! @brief Standard operator that is like @c value().
    FOG_INLINE const T& operator*() const { return *_cur; }

    FOG_INLINE MutableIterator& operator++() { _cur++; _isValid = (_cur <= _end); return *this; }
    FOG_INLINE MutableIterator& operator--() { _cur--; _isValid = (_cur <= _beg); return *this; }
    FOG_INLINE MutableIterator& operator+=(int n) { _cur += n; _isValid = (_cur <= _end); return *this; }
    FOG_INLINE MutableIterator& operator-=(int n) { _cur -= n; _isValid = (_cur >= _beg); return *this; }

    // [Manipulators]

    MutableIterator& remove()
    {
      FOG_ASSERT_X(isValid(), "Fog::List<T>::MutableIterator::remove() - Iterator out of range");

      sysuint_t i = index();
      _owner->remove(i);
      return init(i);
    }

    T take()
    {
      FOG_ASSERT_X(isValid(), "Fog::List<T>::MutableIterator::take() - Iterator out of range");

      sysuint_t i = index();
      T element = _owner->takeAt(i);
      init(i);

      return element;
    }

    MutableIterator& init(sysuint_t i = 0)
    {
      ListData* d = _owner->_d;

      _beg = reinterpret_cast<T*>(d->p);
      _cur = _beg + i;
      _end = _beg + d->length - 1;

      _isValid = (_cur >= _beg && _cur <= _end);

      return *this;
    }
  };

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(ListData)
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE_T1(Fog::List, typename, T, Fog::TYPEINFO_MOVABLE)

// ============================================================================
// [Fog::Swap]
// ============================================================================

_FOG_SWAP_D_TEMPLATE1(Fog::List, typename, T)

// [Guard]
#endif // _FOG_CORE_COLLECTION_LIST_H
