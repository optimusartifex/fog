// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_OTCORE_H
#define _FOG_G2D_TEXT_OTCORE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/BSwap.h>

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct OT_BYTE;
struct OT_CHAR;
struct OT_USHORT;
struct OT_SHORT;
struct OT_ULONG;
struct OT_LONG;
struct OT_Index;

struct OT_Font;
struct OT_Table;

struct OT_CMap;

// ============================================================================
// [TypeDefs]
// ============================================================================

typedef OT_SHORT OT_FWORD;
typedef OT_USHORT OT_UFWORD;

typedef OT_SHORT OT_FIXED2x14;
typedef OT_LONG OT_FIXED16x16;

typedef OT_USHORT OT_SOffset;
typedef OT_ULONG OT_LOffset;

typedef OT_USHORT OT_GlyphID;
typedef OT_ULONG OT_FixedVersion;
typedef OT_ULONG OT_Tag;

typedef void (FOG_CDECL* OT_TableFreeFunc)(OT_Table* ctx);

// ============================================================================
// [FOG_OT_TAG]
// ============================================================================

#define FOG_OT_TAG(_C0_, _C1_, _C2_, _C3_) ( \
  ((uint32_t)(_C0_) << 24) | \
  ((uint32_t)(_C1_) << 16) | \
  ((uint32_t)(_C2_) <<  8) | \
  ((uint32_t)(_C3_)      ) )

// ============================================================================
// [Fog::OT_Api]
// ============================================================================

struct FOG_NO_EXPORT OT_Api
{
  // --------------------------------------------------------------------------
  // [OT_CheckSum]
  // --------------------------------------------------------------------------

  uint32_t (FOG_CDECL* ot_checksum_calc)(const OT_ULONG* pTable, uint32_t length);
};

} // Fog namespace

// ============================================================================
// [fog_ot_api]
// ============================================================================

//! @brief Fog-CAPI interface.
FOG_CVAR_EXTERN Fog::OT_Api fog_ot_api;

namespace Fog {

// [Byte-Pack]
#include <Fog/Core/C++/PackByte.h>

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::OT_BYTE]
// ============================================================================

// NOTE: These types should be considered as primitive types. Do not use 
// constructors, destructors, and copy-operators. The types are stored in big
// endian byte-order to respect the byte-order of TrueType and OpenType fonts.

//! @brief OpenType BYTE (8-bit unsigned integer).
struct FOG_NO_EXPORT OT_BYTE
{
  typedef uint8_t Type;

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE Type getValue() const { return _b[0]; }
  FOG_INLINE void setValue(Type value) { _b[0] = value; }

  FOG_INLINE operator Type() const { return getValue(); }
  FOG_INLINE OT_BYTE& operator=(Type value) { setValue(value); return *this; }

  FOG_INLINE bool operator==(const Type& other) const { return _b[0] == other; }
  FOG_INLINE bool operator!=(const Type& other) const { return _b[0] != other; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint8_t _b[1];
};

// ============================================================================
// [Fog::OT_CHAR]
// ============================================================================

//! @brief OpenType CHAR (8-bit signed integer).
struct FOG_NO_EXPORT OT_CHAR
{
  typedef int8_t Type;

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE Type getValue() const { return _b[0]; }
  FOG_INLINE void setValue(Type value) { _b[0] = value; }

  FOG_INLINE operator Type() const { return getValue(); }
  FOG_INLINE OT_CHAR& operator=(Type value) { setValue(value); return *this; }

  FOG_INLINE bool operator==(const Type& other) const { return _b[0] == other; }
  FOG_INLINE bool operator!=(const Type& other) const { return _b[0] != other; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int8_t _b[1];
};

// ============================================================================
// [Fog::OT_USHORT]
// ============================================================================

struct FOG_NO_EXPORT OT_USHORT
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
  FOG_INLINE OT_USHORT& operator=(Type value) { setValueU(value); return *this; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint8_t _b[2];
};

// ============================================================================
// [Fog::OT_SHORT]
// ============================================================================

struct FOG_NO_EXPORT OT_SHORT
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
  FOG_INLINE OT_SHORT& operator=(Type value) { setValueU(value); return *this; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint8_t _b[2];
};

// ============================================================================
// [Fog::OT_ULONG]
// ============================================================================

struct FOG_NO_EXPORT OT_ULONG
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
  FOG_INLINE OT_ULONG& operator=(Type value) { setValueU(value); return *this; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint8_t _b[4];
};

// ============================================================================
// [Fog::OT_LONG]
// ============================================================================

struct FOG_NO_EXPORT OT_LONG
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
  FOG_INLINE OT_LONG& operator=(Type value) { setValueU(value); return *this; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint8_t _b[4];
};

// ============================================================================
// [Fog::OT_Index]
// ============================================================================

struct FOG_NO_EXPORT OT_Index : public OT_USHORT
{
  enum { NOT_FOUND = 0xFFFF };
};

// ============================================================================
// [Fog::OT_CheckSum]
// ============================================================================

struct FOG_NO_EXPORT OT_CheckSum : public OT_ULONG
{
  static FOG_INLINE uint32_t calc(const OT_ULONG* pTable, uint32_t size)
  {
    return fog_ot_api.ot_checksum_calc(pTable, size);
  }
};

// ============================================================================
// [Fog::OT_DateTime]
// ============================================================================

struct FOG_NO_EXPORT OT_DateTime
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint64_t asUInt64() const
  {
    return (static_cast<uint64_t>(hi) << 32) + static_cast<uint64_t>(lo);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  OT_LONG hi;
  OT_ULONG lo;
};

//! @}

// [Byte-Pack]
#include <Fog/Core/C++/PackRestore.h>

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_OTCORE_H
