// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_VARIANT_VAR_H
#define _FOG_CORE_VARIANT_VAR_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/Swap.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Core_Data
//! @{

// ============================================================================
// [Fog::ValueData]
// ============================================================================

//! @brief Value data.
struct FOG_API ValueData
{
  // [Construction / Destruction]

  ValueData(uint32_t type);
  virtual ~ValueData();

  static void* allogetData();
  static void freeData(void* data);

  // [Methods]

  virtual err_t clone(void* dst) const = 0;
  virtual err_t getInt32(int32_t* dst) const = 0;
  virtual err_t getInt64(int64_t* dst) const = 0;
  virtual err_t getDouble(double* dst) const = 0;
  virtual err_t getString(String* dst) const = 0;

  virtual err_t setValue(void* val) = 0;

  ValueData* ref() const;
  void deref();

  // [Members]

  //! @brief Reference count.
  mutable Atomic<size_t> refCount;

  //! @brief Type of Value.
  uint32_t type;
  //! @brief Value flags.
  uint32_t flags;

  //! @brief Inlined data of various types
  //!
  //! Internal implementation:
  //!   - TypeNull
  //!   - TypeInteger: i.i64
  //!   - TypeDouble: i.d
  //!   - TypeString: &raw[8] == Fog::String*
  union
  {
    uint8_t raw[sizeof(void*)];
    int64_t i64;
    double d;
  };
  void* ptr;

private:
  _FOG_CLASS_NO_COPY(ValueData)
};

// ============================================================================
// [Fog::Value]
// ============================================================================

//! @brief Value can hold any value (dynamic type).
struct FOG_API Value
{
  // [Construction / Destruction]

  Value();
  Value(const Value& other);
  explicit FOG_INLINE Value(ValueData* d) : _d(d) {}
  ~Value();

  Value& operator=(const Value& other);

  static Value null();
  static Value fromBool(bool val);
  static Value fromInt32(int32_t val);
  static Value fromInt64(int64_t val);
  static Value fromDouble(double val);
  static Value fromString(const String& val);

  // [Sharing]

  //! @copydoc Doxygen::Implicit::getReference().
  FOG_INLINE size_t getReference() const { return _d->refCount.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return getReference() == 1; }
  //! @copydoc Doxygen::Implicit::detach().
  err_t detach();

  // [Methods]

  uint32_t getType() const { return _d->type; }

  FOG_INLINE bool isNull() const { return _d->type == VALUE_TYPE_NULL; }
  FOG_INLINE bool isInteger() const { return _d->type == VALUE_TYPE_INTEGER; }
  FOG_INLINE bool isDouble() const { return _d->type == VALUE_TYPE_DOUBLE; }
  FOG_INLINE bool isString() const { return _d->type == VALUE_TYPE_STRING; }

  FOG_INLINE err_t getInt32(int32_t* dst) const { return _d->getInt32(dst); }
  FOG_INLINE err_t getInt64(int64_t* dst) const { return _d->getInt64(dst); }
  FOG_INLINE err_t getDouble(double* dst) const { return _d->getDouble(dst); }
  FOG_INLINE err_t getString(String* dst) const { return _d->getString(dst); }

  err_t reset();
  err_t setInt32(int32_t val);
  err_t setInt64(int64_t val);
  err_t setDouble(double val);
  err_t setString(const String& val);

  // [Static Data]

  static ValueData* _dnull;

  // [Members]

  _FOG_CLASS_D(ValueData)
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::Value, Fog::TYPEINFO_MOVABLE)

// ============================================================================
// [Fog::Swap]
// ============================================================================

_FOG_SWAP_D(Fog::Value)

// [Guard]
#endif // _FOG_CORE_VARIANT_VAR_H
