// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_BITARRAY_H
#define _FOG_CORE_BITARRAY_H

// [Dependencies]
#include <Fog/Build/Build.h>
#include <Fog/Core/Assert.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/TypeInfo.h>

//! @addtogroup Fog_Core
//! @{

namespace Fog {

// ============================================================================
// [Fog::BitArray]
// ============================================================================

struct FOG_API BitArray
{
  struct FOG_API Data
  {
    // [Ref / Deref]

    FOG_INLINE Data* ref() const
    {
      refCount.inc();
      return const_cast<Data*>(this);
    }

    void deref();

    static Data* create(sysuint_t capacity);
    static Data* create(sysuint_t capacity, const uint8_t* data, sysuint_t bitoffset, sysuint_t count);
    static Data* createFor(const uint8_t* data, sysuint_t bitoffset, sysuint_t count);

    static Data* copy(const Data* other);

    // [Members]

    mutable Atomic<sysuint_t> refCount;
    sysuint_t capacity;
    sysuint_t length;
    uint8_t data[4];
  };

  static Static<Data> _dnull;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  BitArray();
  BitArray(const BitArray& other);
  FOG_INLINE explicit BitArray(Data* d) : _d(d) {}
  ~BitArray();

  // --------------------------------------------------------------------------
  // [Implicit Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::refCount().
  FOG_INLINE sysuint_t refCount() const { return _d->refCount.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return refCount() == 1; }
  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach() { return !isDetached() ? _detach() : (err_t)ERR_OK; }
  //! @copydoc Doxygen::Implicit::_detach().
  err_t _detach();
  //! @copydoc Doxygen::Implicit::free().
  void free();

  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::isNull().
  FOG_INLINE bool isNull() const { return _d == _dnull.instancep(); }

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  //! @brief Returns const pointer to bit array data (bits).
  FOG_INLINE const uint8_t* getData() const
  {
    return _d->data;
  }

  //! @brief Returns mutable pointer to bit array data (bits).
  //! 
  //! To ensure that data can be overwritten, @c detach()
  //! is called internally.

  FOG_INLINE uint8_t* getMData()
  {
    return detach() == ERR_OK ? _d->data : NULL;
  }

  FOG_INLINE uint8_t* getXData()
  {
    FOG_ASSERT_X(isDetached(), "Fog::BitArray::getXData() - Non detached data.");
    return _d->data;
  }

  FOG_INLINE bool at(sysuint_t index) const
  {
    FOG_ASSERT_X(index < _d->length, "Fog::BitArray::at() - Index out of range.");
    return (bool)(_d->data[index >> 3] & (1 << (index & 7)));
  }

  //! @brief Returns count of allocated bits (capacity).
  FOG_INLINE sysuint_t getCapacity() const { return _d->capacity; }
  //! @brief Returns count of used bits (length).
  FOG_INLINE sysuint_t getLength() const { return _d->length; }
  //! @brief Returns true if bit array is empty.
  FOG_INLINE bool isEmpty() const { return _d->length == 0; }

  err_t reserve(sysuint_t to);
  err_t resize(sysuint_t to, uint32_t fill = 0);
  err_t grow(sysuint_t by);

  void squeeze();
  void clear();

  // --------------------------------------------------------------------------
  // [Single Bit Manipulation]
  // --------------------------------------------------------------------------

  err_t setBit(sysuint_t index);
  err_t setBit(sysuint_t index, uint32_t bit);
  err_t resetBit(sysuint_t index);
  err_t invertBit(sysuint_t index);

  // --------------------------------------------------------------------------
  // [Multiple Bits Manipulation]
  // --------------------------------------------------------------------------

  err_t fill(uint32_t bit);
  err_t fillAt(sysuint_t index, sysuint_t count, uint32_t bit);
  err_t invert();
  err_t invertAt(sysuint_t index, sysuint_t count);

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  err_t set(const BitArray& other);
  err_t setDeep(const BitArray& other);

  // --------------------------------------------------------------------------
  // [Append]
  // --------------------------------------------------------------------------

  err_t append(uint32_t bit);
  err_t append(uint32_t bit, sysuint_t count);
  err_t append(const BitArray& other);

  // --------------------------------------------------------------------------
  // [Prepend]
  // --------------------------------------------------------------------------

  err_t prepend(uint32_t bit);
  err_t prepend(uint32_t bit, sysuint_t count);
  err_t prepend(const BitArray& other);

  // --------------------------------------------------------------------------
  // [Insert]
  // --------------------------------------------------------------------------

  err_t insert(sysuint_t index, uint32_t bit);
  err_t insert(sysuint_t index, uint32_t bit, sysuint_t count);
  err_t insert(sysuint_t index, const BitArray& other);

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE BitArray& operator=(const BitArray& other) { set(other); return *this; }
  FOG_INLINE BitArray& operator+=(const BitArray& other) { append(other); return *this; }

  // --------------------------------------------------------------------------
  // [Comparing]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const BitArray& other) const { return eq(this, &other); }
  FOG_INLINE int compare(const BitArray& other) const { return compare(this, &other); }

  static bool eq(const BitArray* a, const BitArray* b);
  static int compare(const BitArray* a, const BitArray* b);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_DECLARE_D(Data)
};

} // Fog namespace

static FOG_INLINE const Fog::BitArray operator+(const Fog::BitArray& a, const Fog::BitArray& b) { Fog::BitArray t(a); t += b; return t; }
/*
static FOG_INLINE const Fog::BitArray operator|(const Fog::BitArray& a, const Fog::BitArray& b) { Fog::BitArray t(*this); t |= b; return t; }
static FOG_INLINE const Fog::BitArray operator&(const Fog::BitArray& a, const Fog::BitArray& b) { Fog::BitArray t(*this); t &= b; return t; }
static FOG_INLINE const Fog::BitArray operator^(const Fog::BitArray& a, const Fog::BitArray& b) { Fog::BitArray t(*this); t ^= b; return t; }
*/
static FOG_INLINE bool operator==(const Fog::BitArray& a, const Fog::BitArray& b) { return  a.eq(b); }
static FOG_INLINE bool operator!=(const Fog::BitArray& a, const Fog::BitArray& b) { return !a.eq(b); }
static FOG_INLINE bool operator<=(const Fog::BitArray& a, const Fog::BitArray& b) { return  a.compare(b) <= 0; }
static FOG_INLINE bool operator< (const Fog::BitArray& a, const Fog::BitArray& b) { return  a.compare(b) <  0; }
static FOG_INLINE bool operator>=(const Fog::BitArray& a, const Fog::BitArray& b) { return  a.compare(b) >= 0; }
static FOG_INLINE bool operator> (const Fog::BitArray& a, const Fog::BitArray& b) { return  a.compare(b) >  0; }

//! @}

// [Guard]
#endif // _FOG_CORE_BITARRAY_H
