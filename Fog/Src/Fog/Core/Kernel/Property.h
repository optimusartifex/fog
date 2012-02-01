// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_KERNEL_PROPERTY_H
#define _FOG_CORE_KERNEL_PROPERTY_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/InternedString.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Core_Kernel
//! @{

// ============================================================================
// [Fog::PropertyEnum]
// ============================================================================

//! @brief Enumeration name and value.
struct FOG_NO_EXPORT PropertyEnum
{
  char name[28];
  int32_t value;
};

// ============================================================================
// [Fog::PropertyInfo]
// ============================================================================

//! @brief Structure that contains information about an event.
//!
//! Used together with Fog::Object event model.
struct FOG_NO_EXPORT PropertyInfo
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  PropertyInfo() :
    _name(),
    _index(INVALID_INDEX),
    _type(0),
    _flags(NO_FLAGS)
  {
  }

  PropertyInfo(const PropertyInfo& other) :
    _name(other._name),
    _index(other._index),
    _type(other._type),
    _flags(other._flags)
  {
  }

  PropertyInfo(const InternedStringW& name, size_t index, uint32_t type, uint32_t flags) :
    _name(name),
    _index(index),
    _type(type),
    _flags(flags)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const InternedStringW& getName() const { return _name; }
  FOG_INLINE void setName(const InternedStringW& name) { _name = name; }

  FOG_INLINE size_t getIndex() const { return _index; }
  FOG_INLINE void setIndex(size_t index) { _index = index; }

  FOG_INLINE uint32_t getType() const { return _type; }
  FOG_INLINE void setType(uint32_t type) { _type = type; }

  FOG_INLINE uint32_t getFlags() const { return _flags; }
  FOG_INLINE void setFlags(uint32_t flags) { _flags = flags; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
_name.reset();
_index = INVALID_INDEX;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE PropertyInfo& operator=(const PropertyInfo& other)
  {
    _name = other._name;
    _index = other._index;
    _type = other._type;
    _flags = other._flags;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Property name.
  InternedStringW _name;
  //! @brief Property index.
  size_t _index;
  //! @brief Property type.
  uint32_t _type;
  //! @brief Property flags.
  uint32_t _flags;
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::PropertyIO]
// ============================================================================

//! @internal
//!
//! @brief CoreObj attribute parser / serializer wrapper.
template<typename PropertyT>
struct _Fog_PropertyIO {};

// ============================================================================
// [Fog::PropertyIO - Integer]
// ============================================================================

template<>
struct _Fog_PropertyIO<int8_t>
{
  FOG_INLINE err_t parse(int8_t& dst, const Fog::StringW& src) { return src.parseI8(&dst); }
  FOG_INLINE err_t serialize(Fog::StringW& dst, const int8_t& src) { return dst.setInt(src); }
};

template<>
struct _Fog_PropertyIO<uint8_t>
{
  FOG_INLINE err_t parse(uint8_t& dst, const Fog::StringW& src) { return src.parseU8(&dst); }
  FOG_INLINE err_t serialize(Fog::StringW& dst, const uint8_t& src) { return dst.setInt(src); }
};

template<>
struct _Fog_PropertyIO<int16_t>
{
  FOG_INLINE err_t parse(int16_t& dst, const Fog::StringW& src) { return src.parseI16(&dst); }
  FOG_INLINE err_t serialize(Fog::StringW& dst, const int16_t& src) { return dst.setInt(src); }
};

template<>
struct _Fog_PropertyIO<uint16_t>
{
  FOG_INLINE err_t parse(uint16_t& dst, const Fog::StringW& src) { return src.parseU16(&dst); }
  FOG_INLINE err_t serialize(Fog::StringW& dst, const uint16_t& src) { return dst.setInt(src); }
};

template<>
struct _Fog_PropertyIO<int32_t>
{
  FOG_INLINE err_t parse(int32_t& dst, const Fog::StringW& src) { return src.parseI32(&dst); }
  FOG_INLINE err_t serialize(Fog::StringW& dst, const int32_t& src) { return dst.setInt(src); }
};

template<>
struct _Fog_PropertyIO<uint32_t>
{
  FOG_INLINE err_t parse(uint32_t& dst, const Fog::StringW& src) { return src.parseU32(&dst); }
  FOG_INLINE err_t serialize(Fog::StringW& dst, const uint32_t& src) { return dst.setInt(src); }
};

template<>
struct _Fog_PropertyIO<int64_t>
{
  FOG_INLINE err_t parse(int64_t& dst, const Fog::StringW& src) { return src.parseI64(&dst); }
  FOG_INLINE err_t serialize(Fog::StringW& dst, const int64_t& src) { return dst.setInt(src); }
};

template<>
struct _Fog_PropertyIO<uint64_t>
{
  FOG_INLINE err_t parse(uint64_t& dst, const Fog::StringW& src) { return src.parseU64(&dst); }
  FOG_INLINE err_t serialize(Fog::StringW& dst, const uint64_t& src) { return dst.setInt(src); }
};

// ============================================================================
// [Fog::PropertyIO - Real]
// ============================================================================

template<>
struct _Fog_PropertyIO<float>
{
  FOG_INLINE err_t parse(float& dst, const Fog::StringW& src) { return src.parseReal(&dst); }
  FOG_INLINE err_t serialize(Fog::StringW& dst, const float& src) { return dst.setReal(src); }
};

template<>
struct _Fog_PropertyIO<double>
{
  FOG_INLINE err_t parse(double& dst, const Fog::StringW& src) { return src.parseReal(&dst); }
  FOG_INLINE err_t serialize(Fog::StringW& dst, const double& src) { return dst.setReal(src); }
};

// ============================================================================
// [Fog::PropertyIO - StringW]
// ============================================================================

template<>
struct _Fog_PropertyIO<Fog::StringW>
{
  FOG_INLINE err_t parse(Fog::StringW& dst, const Fog::StringW& src) { return dst.set(src); }
  FOG_INLINE err_t serialize(Fog::StringW& dst, const Fog::StringW& src) { return dst.set(src); }
};

// [Guard]
#endif // _FOG_CORE_KERNEL_PROPERTY_H
