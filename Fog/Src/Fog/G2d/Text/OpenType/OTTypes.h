// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_OTTYPES_H
#define _FOG_G2D_TEXT_OTTYPES_H

// [Dependencies]
#include <Fog/Core/Memory/BSwap.h>
#include <Fog/G2d/Text/OpenType/OTApi.h>

namespace Fog {

// [Byte-Pack]
#include <Fog/Core/C++/PackByte.h>

//! @addtogroup Fog_G2d_Text_OpenType
//! @{

// ============================================================================
// [Defs]
// ============================================================================

#define FOG_OT_INT8(_Ptr_)   ((OTInt8*  )(_Ptr_))
#define FOG_OT_UINT8(_Ptr_)  ((OTUInt8* )(_Ptr_))
#define FOG_OT_INT16(_Ptr_)  ((OTInt16* )(_Ptr_))
#define FOG_OT_UINT16(_Ptr_) ((OTUInt16*)(_Ptr_))
#define FOG_OT_UINT24(_Ptr_) ((OTUInt24*)(_Ptr_))
#define FOG_OT_INT32(_Ptr_)  ((OTInt32* )(_Ptr_))
#define FOG_OT_UINT32(_Ptr_) ((OTUInt32*)(_Ptr_))

// ============================================================================
// [Fog::OTInt8]
// ============================================================================

//! @brief OpenType CHAR (8-bit signed integer).
struct FOG_NO_EXPORT OTInt8
{
  typedef int8_t Type;

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE Type getValue() const { return _b[0]; }
  FOG_INLINE void setValue(Type value) { _b[0] = value; }

  FOG_INLINE operator Type() const { return getValue(); }
  FOG_INLINE OTInt8& operator=(Type value) { setValue(value); return *this; }

  FOG_INLINE bool operator==(const Type& other) const { return _b[0] == other; }
  FOG_INLINE bool operator!=(const Type& other) const { return _b[0] != other; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int8_t _b[1];
};

// ============================================================================
// [Fog::OTUInt8]
// ============================================================================

// NOTE: These types should be considered as primitive types. Do not use 
// constructors, destructors, and copy-operators. The types are stored in big
// endian byte-order to respect the byte-order of TrueType and OpenType fonts.

//! @brief OpenType BYTE (8-bit unsigned integer).
struct FOG_NO_EXPORT OTUInt8
{
  typedef uint8_t Type;

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE Type getValue() const { return _b[0]; }
  FOG_INLINE void setValue(Type value) { _b[0] = value; }

  FOG_INLINE operator Type() const { return getValue(); }
  FOG_INLINE OTUInt8& operator=(Type value) { setValue(value); return *this; }

  FOG_INLINE bool operator==(const Type& other) const { return _b[0] == other; }
  FOG_INLINE bool operator!=(const Type& other) const { return _b[0] != other; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint8_t _b[1];
};

// ============================================================================
// [Fog::OTInt16]
// ============================================================================

struct FOG_NO_EXPORT OTInt16
{
  typedef int16_t Type;

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE Type getValueA() const
  {
    FOG_ASSERT_ALIGNED(_b, 2);
    return MemOps::bswap16be(reinterpret_cast<const int16_t*>(_b)[0]);
  }

  FOG_INLINE void setValueA(Type value)
  {
    FOG_ASSERT_ALIGNED(_b, 2);
    reinterpret_cast<int16_t*>(_b)[0] = MemOps::bswap16be(value);
  }

  FOG_INLINE Type getRawA() const
  {
    FOG_ASSERT_ALIGNED(_b, 2);
    return reinterpret_cast<const int16_t*>(_b)[0];
  }
  FOG_INLINE void setRawA(Type value)
  {
    FOG_ASSERT_ALIGNED(_b, 2);
    reinterpret_cast<int16_t*>(_b)[0] = value;
  }

#if defined(FOG_ARCH_UNALIGNED_ACCESS_16)
  FOG_INLINE Type getValueU() const
  {
    return MemOps::bswap16be(reinterpret_cast<const int16_t*>(_b)[0]);
  }
  FOG_INLINE void setValueU(Type value)
  {
    reinterpret_cast<int16_t*>(_b)[0] = MemOps::bswap16be(value);
  }

  FOG_INLINE Type getRawU() const
  {
    return reinterpret_cast<const int16_t*>(_b)[0];
  }

  FOG_INLINE void setRawU(Type value)
  {
    reinterpret_cast<int16_t*>(_b)[0] = value;
  }
#else
  FOG_INLINE Type getValueU() const
  {
    return static_cast<Type>(
      (static_cast<uint16_t>(_b[0]) << 8) + static_cast<uint16_t>(_b[1]));
  }

  FOG_INLINE void setValueU(Type value)
  {
    _b[0] = static_cast<uint8_t>(static_cast<uint16_t>(value) << 8);
    _b[1] = static_cast<uint8_t>(static_cast<uint16_t>(value)     );
  }

# if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  FOG_INLINE Type getRawU() const
  {
    return static_cast<Type>(
      static_cast<uint16_t>(_b[0]) + (static_cast<uint16_t>(_b[1]) << 8));
  }

  FOG_INLINE void setRawU(Type value)
  {
    _b[0] = static_cast<uint8_t>(static_cast<uint16_t>(value)     );
    _b[1] = static_cast<uint8_t>(static_cast<uint16_t>(value) << 8);
  }
# else
  FOG_INLINE Type getRawU() const { return getValueU(); }
  FOG_INLINE void setRawU(Type value) { setValueU(value); }
# endif
#endif

  FOG_INLINE operator Type() const { return getValueU(); }
  FOG_INLINE OTInt16& operator=(Type value) { setValueU(value); return *this; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint8_t _b[2];
};

// ============================================================================
// [Fog::OTUInt16]
// ============================================================================

struct FOG_NO_EXPORT OTUInt16
{
  typedef uint16_t Type;

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE Type getValueA() const
  {
    FOG_ASSERT_ALIGNED(_b, 2);
    return MemOps::bswap16be(reinterpret_cast<const uint16_t*>(_b)[0]);
  }

  FOG_INLINE void setValueA(Type value)
  {
    FOG_ASSERT_ALIGNED(_b, 2);
    reinterpret_cast<uint16_t*>(_b)[0] = MemOps::bswap16be(value);
  }

  FOG_INLINE Type getRawA() const
  {
    FOG_ASSERT_ALIGNED(_b, 2);
    return reinterpret_cast<const uint16_t*>(_b)[0];
  }

  FOG_INLINE void setRawA(Type value)
  {
    FOG_ASSERT_ALIGNED(_b, 2);
    reinterpret_cast<uint16_t*>(_b)[0] = value;
  }

#if defined(FOG_ARCH_UNALIGNED_ACCESS_16)
  FOG_INLINE Type getValueU() const
  {
    return MemOps::bswap16be(reinterpret_cast<const uint16_t*>(_b)[0]);
  }

  FOG_INLINE void setValueU(Type value)
  {
    reinterpret_cast<uint16_t*>(_b)[0] = MemOps::bswap16be(value);
  }

  FOG_INLINE Type getRawU() const
  {
    return reinterpret_cast<const uint16_t*>(_b)[0];
  }

  FOG_INLINE void setRawU(Type value)
  {
    reinterpret_cast<uint16_t*>(_b)[0] = value;
  }
#else
  FOG_INLINE Type getValueU() const
  {
    return (static_cast<uint16_t>(_b[0]) << 8) + static_cast<uint16_t>(_b[1]);
  }

  FOG_INLINE void setValueU(Type value)
  {
    _b[0] = static_cast<uint8_t>(value << 8);
    _b[1] = static_cast<uint8_t>(value     );
  }

# if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  FOG_INLINE Type getRawU() const
  {
    return static_cast<uint16_t>(_b[0]) + (static_cast<uint16_t>(_b[1]) << 8);
  }

  FOG_INLINE void setRawU(Type value)
  {
    _b[0] = static_cast<uint8_t>(value     );
    _b[1] = static_cast<uint8_t>(value << 8);
  }
# else
  FOG_INLINE Type getRawU() const { return getValueU(); }
  FOG_INLINE void setRawU(Type value) { setValueU(value); }
# endif
#endif

  FOG_INLINE operator Type() const { return getValueU(); }
  FOG_INLINE OTUInt16& operator=(Type value) { setValueU(value); return *this; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint8_t _b[2];
};

// ============================================================================
// [Fog::OTUInt24]
// ============================================================================

struct FOG_NO_EXPORT OTUInt24
{
  typedef uint32_t Type;

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE Type getValue() const
  {
    return (static_cast<uint32_t>(_b[0]) << 16) + 
           (static_cast<uint32_t>(_b[1]) <<  8) +
           (static_cast<uint32_t>(_b[2])      ) ;
  }

  FOG_INLINE void setValue(Type value)
  {
    FOG_ASSERT(value <= 0x00FFFFFF);
    _b[0] = static_cast<uint8_t>(value >> 16);
    _b[1] = static_cast<uint8_t>(value >>  8);
    _b[2] = static_cast<uint8_t>(value      );
  }

  FOG_INLINE operator Type() const { return getValue(); }
  FOG_INLINE OTUInt24& operator=(Type value) { setValue(value); return *this; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint8_t _b[3];
};

// ============================================================================
// [Fog::OTInt32]
// ============================================================================

struct FOG_NO_EXPORT OTInt32
{
  typedef int16_t Type;

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE Type getValueA() const
  {
    FOG_ASSERT_ALIGNED(_b, 4);
    return MemOps::bswap32be(reinterpret_cast<const int32_t*>(_b)[0]);
  }

  FOG_INLINE void setValueA(Type value)
  {
    FOG_ASSERT_ALIGNED(_b, 4);
    reinterpret_cast<int32_t*>(_b)[0] = MemOps::bswap32be(value);
  }

  FOG_INLINE Type getRawA() const
  {
    FOG_ASSERT_ALIGNED(_b, 4);
    return reinterpret_cast<const int32_t*>(_b)[0];
  }

  FOG_INLINE void setRawA(Type value)
  {
    FOG_ASSERT_ALIGNED(_b, 4);
    reinterpret_cast<int32_t*>(_b)[0] = value;
  }

#if defined(FOG_ARCH_UNALIGNED_ACCESS_16)
  FOG_INLINE Type getValueU() const
  {
    return MemOps::bswap32be(reinterpret_cast<const int32_t*>(_b)[0]);
  }

  FOG_INLINE void setValueU(Type value)
  {
    reinterpret_cast<int32_t*>(_b)[0] = MemOps::bswap32be(value);
  }

  FOG_INLINE Type getRawU() const
  {
    return reinterpret_cast<const int32_t*>(_b)[0];
  }

  FOG_INLINE void setRawU(Type value)
  {
    reinterpret_cast<int32_t*>(_b)[0] = value;
  }
#else
  FOG_INLINE Type getValueU() const
  {
    return static_cast<Type>(
      (static_cast<uint32_t>(_b[0]) << 24) + 
      (static_cast<uint32_t>(_b[1]) << 16) +
      (static_cast<uint32_t>(_b[2]) <<  8) +
      (static_cast<uint32_t>(_b[3])      ) );
  }

  FOG_INLINE void setValueU(Type value)
  {
    _b[0] = static_cast<uint8_t>(static_cast<uint32_t>(value) << 24);
    _b[1] = static_cast<uint8_t>(static_cast<uint32_t>(value) << 16);
    _b[2] = static_cast<uint8_t>(static_cast<uint32_t>(value) <<  8);
    _b[3] = static_cast<uint8_t>(static_cast<uint32_t>(value)      );
  }

# if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  FOG_INLINE Type getRawU() const
  {
    return static_cast<Type>(
      (static_cast<uint32_t>(_b[0])      ) + 
      (static_cast<uint32_t>(_b[1]) <<  8) +
      (static_cast<uint32_t>(_b[2]) << 16) +
      (static_cast<uint32_t>(_b[3]) << 24) );
  }

  FOG_INLINE void setRawU(Type value)
  {
    _b[0] = static_cast<uint8_t>(static_cast<uint32_t>(value)      );
    _b[1] = static_cast<uint8_t>(static_cast<uint32_t>(value) <<  8);
    _b[2] = static_cast<uint8_t>(static_cast<uint32_t>(value) << 16);
    _b[3] = static_cast<uint8_t>(static_cast<uint32_t>(value) << 24);
  }
# else
  FOG_INLINE Type getRawU() const { return getValueU(); }
  FOG_INLINE void setRawU(Type value) { setValueU(value); }
# endif
#endif

  FOG_INLINE operator Type() const { return getValueU(); }
  FOG_INLINE OTInt32& operator=(Type value) { setValueU(value); return *this; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint8_t _b[4];
};

// ============================================================================
// [Fog::OTUInt32]
// ============================================================================

struct FOG_NO_EXPORT OTUInt32
{
  typedef uint32_t Type;

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE Type getValueA() const
  {
    FOG_ASSERT_ALIGNED(_b, 4);
    return MemOps::bswap32be(reinterpret_cast<const uint32_t*>(_b)[0]);
  }

  FOG_INLINE void setValueA(Type value)
  {
    FOG_ASSERT_ALIGNED(_b, 4);
    reinterpret_cast<uint32_t*>(_b)[0] = MemOps::bswap32be(value);
  }

  FOG_INLINE Type getRawA() const
  {
    FOG_ASSERT_ALIGNED(_b, 4);
    return reinterpret_cast<const uint32_t*>(_b)[0];
  }

  FOG_INLINE void setRawA(Type value)
  {
    FOG_ASSERT_ALIGNED(_b, 4);
    reinterpret_cast<uint32_t*>(_b)[0] = value;
  }

#if defined(FOG_ARCH_UNALIGNED_ACCESS_16)
  FOG_INLINE Type getValueU() const
  {
    return MemOps::bswap32be(reinterpret_cast<const uint32_t*>(_b)[0]);
  }

  FOG_INLINE void setValueU(Type value)
  {
    reinterpret_cast<uint32_t*>(_b)[0] = MemOps::bswap32be(value);
  }

  FOG_INLINE Type getRawU() const
  {
    return reinterpret_cast<const uint32_t*>(_b)[0];
  }

  FOG_INLINE void setRawU(Type value)
  {
    reinterpret_cast<uint32_t*>(_b)[0] = value;
  }
#else
  FOG_INLINE Type getValueU() const
  {
    return (static_cast<uint32_t>(_b[0]) << 24) + 
           (static_cast<uint32_t>(_b[1]) << 16) +
           (static_cast<uint32_t>(_b[2]) <<  8) +
           (static_cast<uint32_t>(_b[3])      ) ;
  }

  FOG_INLINE void setValueU(Type value)
  {
    _b[0] = static_cast<uint8_t>(value << 24);
    _b[1] = static_cast<uint8_t>(value << 16);
    _b[2] = static_cast<uint8_t>(value <<  8);
    _b[3] = static_cast<uint8_t>(value      );
  }

# if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  FOG_INLINE Type getRawU() const
  {
    return (static_cast<uint32_t>(_b[0])      ) + 
           (static_cast<uint32_t>(_b[1]) <<  8) +
           (static_cast<uint32_t>(_b[2]) << 16) +
           (static_cast<uint32_t>(_b[3]) << 24) ;
  }
  FOG_INLINE void setRawU(Type value)
  {
    _b[0] = static_cast<uint8_t>(value      );
    _b[1] = static_cast<uint8_t>(value <<  8);
    _b[2] = static_cast<uint8_t>(value << 16);
    _b[3] = static_cast<uint8_t>(value << 24);
  }
# else
  FOG_INLINE Type getRawU() const { return getValueU(); }
  FOG_INLINE void setRawU(Type value) { setValueU(value); }
# endif
#endif

  FOG_INLINE operator Type() const { return getValueU(); }
  FOG_INLINE OTUInt32& operator=(Type value) { setValueU(value); return *this; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint8_t _b[4];
};

// ============================================================================
// [Fog::OTIndex]
// ============================================================================

struct FOG_NO_EXPORT OTIndex : public OTUInt16
{
  // --------------------------------------------------------------------------
  // [Defs]
  // --------------------------------------------------------------------------

  enum { NOT_FOUND = 0xFFFF };
};

// ============================================================================
// [Fog::OTCheckSum]
// ============================================================================

struct FOG_NO_EXPORT OTCheckSum : public OTUInt32
{
  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE uint32_t calc(const OTUInt32* pTable, uint32_t size)
  {
    return fog_ot_api.otchecksum_calc(pTable, size);
  }
};

// ============================================================================
// [Fog::OTDateTime]
// ============================================================================

struct FOG_NO_EXPORT OTDateTime
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint64_t asUInt64() const
  {
    return (static_cast<uint64_t>(hi.getValueU()) << 32) + static_cast<uint64_t>(lo.getValueU());
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  OTInt32 hi;
  OTUInt32 lo;
};

// ============================================================================
// [Fog::OTTable]
// ============================================================================

//! @brief TrueType or OpenType table loaded from font file or collection.
struct FOG_NO_EXPORT OTTable
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE OTFace* getFace() const { return _face; }
  FOG_INLINE OTTable* getNext() const { return _next; }

  FOG_INLINE uint32_t getTag() const { return _tag; }

  FOG_INLINE uint8_t* getData() const { return _data; }
  FOG_INLINE uint32_t getDataLength() const { return _dataLength; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Name of tag the data belongs to.
  uint32_t _tag;
  //! @brief Length of @c _data.
  uint32_t _dataLength;
  //! @brief 'sfnt' data.
  uint8_t* _data;

  //! @brief OTFace which owns the table.
  OTFace* _face;
  //! @brief Next table in @ref OTFace.
  OTTable* _next;

  //! @brief Release all associated data with the table.
  OTTableDestroyFunc _destroy;
};

//! @}

// [Byte-Pack]
#include <Fog/Core/C++/PackRestore.h>

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_OTTYPES_H
