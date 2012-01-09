// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_VAR_H
#define _FOG_CORE_TOOLS_VAR_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/ListUntyped.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/VarId.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::VarData]
// ============================================================================

struct FOG_NO_EXPORT VarData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE VarData* addRef() const
  {
    return fog_api.var_dAddRef(const_cast<VarData*>(this));
  }

  FOG_INLINE void release()
  {
    return fog_api.var_dRelease(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint8_t* getData()
  {
    return reinterpret_cast<uint8_t*>(this) + sizeof(VarData);
  }

  FOG_INLINE const uint8_t* getData() const
  {
    return reinterpret_cast<const uint8_t*>(this) + sizeof(VarData);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Unknown member.
  union
  {
    //! @brief VarData can be class with virtual members. In such case the ptr
    //! is an address to the class vtable.
    void* ptr;
    //! @brief VarData can be implicitly shared data instance. In such case the
    //! size is a reference count.
    size_t reference;
  } unknown;

  //! @brief Variable type and flags.
  uint32_t vType;
#if FOG_ARCH_BITS >= 64
  //! @brief Reserved.
  uint32_t padding_0_32;
#endif // FOG_ARCH_BITS
};

// ============================================================================
// [Fog::VarSimpleData]
// ============================================================================

struct VarSimpleData : public VarData
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  template<typename Type>
  FOG_INLINE const Type* getDataT() const { return reinterpret_cast<const Type*>(data); }

  template<typename Type>
  FOG_INLINE Type* getDataT() const { return reinterpret_cast<Type*>(data); }

  template<typename Type>
  FOG_INLINE const Type& getInstanceT() { return *reinterpret_cast<const Type*>(data); }

  template<typename Type>
  FOG_INLINE Type& getInstanceT() const { return *reinterpret_cast<Type*>(data); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint8_t data[64 - sizeof(VarData)];
};

// ============================================================================
// [Fog::Var]
// ============================================================================

//! @brief Var can hold any value (dynamic type).
struct FOG_NO_EXPORT Var
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Var()
  {
    fog_api.var_ctor(this);
  }

  FOG_INLINE Var(const Var& other)
  {
    fog_api.var_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE Var(Var&& other) : _d(other._d) { other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  FOG_INLINE Var(uint32_t vType, const void* vData)
  {
    fog_api.var_ctorType(this, vType, vData);
  }

  explicit FOG_INLINE Var(VarData* d) : _d(d) {}
  explicit FOG_INLINE Var(_Uninitialized) {}

  FOG_INLINE ~Var()
  {
    fog_api.var_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const
  {
    return fog_api.var_getReference(this);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    return fog_api.var_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Variable Type / Flags]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getVarType() const { return _d->vType & VAR_TYPE_MASK; }
  FOG_INLINE uint32_t getVarFlags() const { return _d->vType & VAR_FLAG_MASK; }

  FOG_INLINE bool isNull() const { return _d->vType == VAR_TYPE_NULL; }

  FOG_INLINE bool isInteger() const { return Math::isBounded<uint32_t>(getVarType(), _VAR_TYPE_INTEGER_START, _VAR_TYPE_INTEGER_END); }
  FOG_INLINE bool isReal() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_FLOAT, VAR_TYPE_DOUBLE); }
  FOG_INLINE bool isNumber() const { return Math::isBounded<uint32_t>(getVarType(), _VAR_TYPE_NUMBER_START, _VAR_TYPE_NUMBER_END);  }
  FOG_INLINE bool isString() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_STRINGA, VAR_TYPE_STRINGW); }

  FOG_INLINE bool isList() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_LIST_STRINGA, VAR_TYPE_LIST_VAR); }
  FOG_INLINE bool isHashA() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_HASH_STRINGA_STRINGA, VAR_TYPE_HASH_STRINGA_VAR); }
  FOG_INLINE bool isHashW() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_HASH_STRINGW_STRINGW, VAR_TYPE_HASH_STRINGW_VAR); }

  FOG_INLINE bool isRegExp() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_REGEXPA, VAR_TYPE_REGEXPW); }
  FOG_INLINE bool isLocale() const { return getVarType() == VAR_TYPE_LOCALE; }
  FOG_INLINE bool isDate() const { return getVarType() == VAR_TYPE_DATE; }
  FOG_INLINE bool isTime() const { return getVarType() == VAR_TYPE_TIME; }

  FOG_INLINE bool isGeometry() const { return Math::isBounded<uint32_t>(getVarType(), _VAR_TYPE_GEOMETRY_START, _VAR_TYPE_GEOMETRY_END); }
  FOG_INLINE bool isPoint() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_POINTI, VAR_TYPE_POINTD); }
  FOG_INLINE bool isSize() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_SIZEI, VAR_TYPE_SIZED); }
  FOG_INLINE bool isBox() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_BOXI, VAR_TYPE_BOXD); }
  FOG_INLINE bool isRect() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_RECTI, VAR_TYPE_RECTD); }
  FOG_INLINE bool isLine() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_LINEF, VAR_TYPE_LINED); }
  FOG_INLINE bool isQBezier() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_QBEZIERF, VAR_TYPE_QBEZIERD); }
  FOG_INLINE bool isCBezier() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_CBEZIERF, VAR_TYPE_CBEZIERD); }
  FOG_INLINE bool isTriangle() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_TRIANGLEF, VAR_TYPE_TRIANGLED); }
  FOG_INLINE bool isRound() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_ROUNDF, VAR_TYPE_ROUNDD); }
  FOG_INLINE bool isCircle() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_CIRCLEF, VAR_TYPE_CIRCLED); }
  FOG_INLINE bool isEllipse() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_ELLIPSEF, VAR_TYPE_ELLIPSED); }
  FOG_INLINE bool isArc() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_ARCF, VAR_TYPE_ARCD); }
  FOG_INLINE bool isChord() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_CHORDF, VAR_TYPE_CHORDD); }
  FOG_INLINE bool isPie() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_PIEF, VAR_TYPE_PIED); }
  FOG_INLINE bool isPolygon() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_POLYGONF, VAR_TYPE_POLYGOND); }
  FOG_INLINE bool isPath() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_PATHF, VAR_TYPE_PATHD); }
  FOG_INLINE bool isRegion() const { return getVarType() == VAR_TYPE_REGION; }

  FOG_INLINE bool isTransform() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_TRANSFORMF, VAR_TYPE_TRANSFORMD); }
  FOG_INLINE bool isMatrix() const { return Math::isBounded<uint32_t>(getVarType(), VAR_TYPE_MATRIXF, VAR_TYPE_MATRIXD); }

  FOG_INLINE bool isColor() const { return getVarType() == VAR_TYPE_COLOR; }
  FOG_INLINE bool isPattern() const { return Math::isBounded<uint32_t>(getVarType(), _VAR_TYPE_PATTERN_START, _VAR_TYPE_PATTERN_END); }

  FOG_INLINE bool isColorStop() const { return getVarType() == VAR_TYPE_COLOR_STOP; }
  FOG_INLINE bool isColorStopList() const { return getVarType() == VAR_TYPE_COLOR_STOP_LIST; }

  FOG_INLINE bool isImage() const { return getVarType() == VAR_TYPE_IMAGE; }
  FOG_INLINE bool isImagePalette() const { return getVarType() == VAR_TYPE_IMAGE_PALETTE; }
  FOG_INLINE bool isImageFilter() const { return getVarType() == VAR_TYPE_IMAGE_FILTER; }

  FOG_INLINE bool isFont() const { return getVarType() == VAR_TYPE_FONT; }

  FOG_INLINE bool isObjectRef() const { return getVarType() == VAR_TYPE_OBJECT_REF; }

  // --------------------------------------------------------------------------
  // [Accessors - Int]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t getI8(int8_t& dst) const
  {
    int32_t t;
    err_t err = fog_api.var_getI32Bound(this, &t, INT8_MIN, INT8_MAX);

    dst =  (int8_t)t;
    return err;
  }

  FOG_INLINE err_t getI8(int8_t& dst, int8_t min, int8_t max) const
  {
    int32_t t;
    err_t err = fog_api.var_getI32Bound(this, &t, min, max);

    dst =  (int8_t)t;
    return err;
  }

  FOG_INLINE err_t getU8(uint8_t& dst) const
  {
    uint32_t t;
    err_t err = fog_api.var_getU32Bound(this, &t, 0, UINT8_MAX);

    dst =  (uint8_t)t;
    return err;
  }

  FOG_INLINE err_t getU8(uint8_t& dst, uint8_t min, uint8_t max) const
  {
    uint32_t t;
    err_t err = fog_api.var_getU32Bound(this, &t, min, max);

    dst =  (uint8_t)t;
    return err;
  }

  FOG_INLINE err_t getI16(int16_t& dst) const
  {
    int32_t t;
    err_t err = fog_api.var_getI32Bound(this, &t, INT16_MIN, INT16_MAX);

    dst =  (int16_t)t;
    return err;
  }

  FOG_INLINE err_t getI16(int16_t& dst, int16_t min, int16_t max) const
  {
    int32_t t;
    err_t err = fog_api.var_getI32Bound(this, &t, min, max);

    dst =  (int16_t)t;
    return err;
  }

  FOG_INLINE err_t getU16(uint16_t& dst) const
  {
    uint32_t t;
    err_t err = fog_api.var_getU32Bound(this, &t, 0, UINT16_MAX);

    dst =  (uint16_t)t;
    return err;
  }

  FOG_INLINE err_t getU16(uint16_t& dst, uint16_t min, uint16_t max) const
  {
    uint32_t t;
    err_t err = fog_api.var_getU32Bound(this, &t, min, max);

    dst =  (uint16_t)t;
    return err;
  }

  FOG_INLINE err_t getI32(int32_t& dst) const
  {
    return fog_api.var_getI32(this, &dst);
  }

  FOG_INLINE err_t getI32(int32_t& dst, int32_t min, int32_t max) const
  {
    return fog_api.var_getI32Bound(this, &dst, min, max);
  }

  FOG_INLINE err_t getU32(uint32_t& dst) const
  {
    return fog_api.var_getU32(this, &dst);
  }

  FOG_INLINE err_t getU32(uint32_t& dst, uint32_t min, uint32_t max) const
  {
    return fog_api.var_getU32Bound(this, &dst, min, max);
  }

  FOG_INLINE err_t getI64(int64_t& dst) const
  {
    return fog_api.var_getI64(this, &dst);
  }

  FOG_INLINE err_t getI64(int64_t& dst, int64_t min, int64_t max) const
  {
    return fog_api.var_getI64Bound(this, &dst, min, max);
  }

  FOG_INLINE err_t getU64(uint64_t& dst) const
  {
    return fog_api.var_getU64(this, &dst);
  }

  FOG_INLINE err_t getU64(uint64_t& dst, uint64_t min, uint64_t max) const
  {
    return fog_api.var_getU64Bound(this, &dst, min, max);
  }

  FOG_INLINE err_t getSSizeT(size_t& dst) const
  {
    if (sizeof(size_t) == sizeof(int32_t))
      return fog_api.var_getI32(this, reinterpret_cast<int32_t*>(&dst));
    else
      return fog_api.var_getI64(this, reinterpret_cast<int64_t*>(&dst));
  }

  FOG_INLINE err_t getSSizeT(size_t& dst, size_t min, size_t max) const
  {
    if (sizeof(size_t) == sizeof(int32_t))
      return fog_api.var_getI32Bound(this, reinterpret_cast<int32_t*>(&dst), (int32_t)min, (int32_t)max);
    else
      return fog_api.var_getI64Bound(this, reinterpret_cast<int64_t*>(&dst), (int64_t)min, (int64_t)max);
  }

  FOG_INLINE err_t getSizeT(size_t& dst) const
  {
    if (sizeof(size_t) == sizeof(uint32_t))
      return fog_api.var_getU32(this, reinterpret_cast<uint32_t*>(&dst));
    else
      return fog_api.var_getU64(this, reinterpret_cast<uint64_t*>(&dst));
  }

  FOG_INLINE err_t getSizeT(size_t& dst, size_t min, size_t max) const
  {
    if (sizeof(size_t) == sizeof(uint32_t))
      return fog_api.var_getU32Bound(this, reinterpret_cast<uint32_t*>(&dst), (uint32_t)min, (uint32_t)max);
    else
      return fog_api.var_getU64Bound(this, reinterpret_cast<uint64_t*>(&dst), (uint64_t)min, (uint64_t)max);
  }

  FOG_INLINE err_t getLong(long& dst) const
  {
    if (sizeof(long) == sizeof(int32_t))
      return fog_api.var_getI32(this, reinterpret_cast<int32_t*>(&dst));
    else
      return fog_api.var_getI64(this, reinterpret_cast<int64_t*>(&dst));
  }

  FOG_INLINE err_t getLong(long& dst, size_t min, size_t max) const
  {
    if (sizeof(long) == sizeof(int32_t))
      return fog_api.var_getI32Bound(this, reinterpret_cast<int32_t*>(&dst), (int32_t)min, (int32_t)max);
    else
      return fog_api.var_getI64Bound(this, reinterpret_cast<int64_t*>(&dst), (int64_t)min, (int64_t)max);
  }

  FOG_INLINE err_t getULong(ulong& dst) const
  {
    if (sizeof(ulong) == sizeof(uint32_t))
      return fog_api.var_getU32(this, reinterpret_cast<uint32_t*>(&dst));
    else
      return fog_api.var_getU64(this, reinterpret_cast<uint64_t*>(&dst));
  }

  FOG_INLINE err_t getULong(ulong& dst, ulong min, ulong max) const
  {
    if (sizeof(ulong) == sizeof(uint32_t))
      return fog_api.var_getU32Bound(this, reinterpret_cast<uint32_t*>(&dst), (uint32_t)min, (uint32_t)max);
    else
      return fog_api.var_getU64Bound(this, reinterpret_cast<uint64_t*>(&dst), (uint64_t)min, (uint64_t)max);
  }

  FOG_INLINE err_t getInt(int8_t& dst) const { return getI8(dst); }
  FOG_INLINE err_t getInt(uint8_t& dst) const { return getU8(dst); }
  FOG_INLINE err_t getInt(int16_t& dst) const { return getI16(dst); }
  FOG_INLINE err_t getInt(uint16_t& dst) const { return getU16(dst); }
  FOG_INLINE err_t getInt(int32_t& dst) const { return getI32(dst); }
  FOG_INLINE err_t getInt(uint32_t& dst) const { return getU32(dst); }
  FOG_INLINE err_t getInt(int64_t& dst) const { return getI64(dst); }
  FOG_INLINE err_t getInt(uint64_t& dst) const { return getU64(dst); }

  FOG_INLINE err_t getInt(int8_t& dst, int8_t min, int8_t max) const { return getI8(dst, min, max); }
  FOG_INLINE err_t getInt(uint8_t& dst, uint8_t min, uint8_t max) const { return getU8(dst, min, max); }
  FOG_INLINE err_t getInt(int16_t& dst, int16_t min, int16_t max) const { return getI16(dst, min, max); }
  FOG_INLINE err_t getInt(uint16_t& dst, uint16_t min, uint16_t max) const { return getU16(dst, min, max); }
  FOG_INLINE err_t getInt(int32_t& dst, int32_t min, int32_t max) const { return getI32(dst, min, max); }
  FOG_INLINE err_t getInt(uint32_t& dst, uint32_t min, uint32_t max) const { return getU32(dst, min, max); }
  FOG_INLINE err_t getInt(int64_t& dst, int64_t min, int64_t max) const { return getI64(dst, min, max); }
  FOG_INLINE err_t getInt(uint64_t& dst, uint64_t min, uint64_t max) const { return getU64(dst, min, max); }

  FOG_INLINE err_t setI8(int8_t src) { int32_t t = src; return fog_api.var_setType(this, VAR_TYPE_INT32, &t); }
  FOG_INLINE err_t setU8(uint8_t src) { uint32_t t = src; return fog_api.var_setType(this, VAR_TYPE_UINT32, &t); }
  FOG_INLINE err_t setI16(int16_t src) { int32_t t = src; return fog_api.var_setType(this, VAR_TYPE_INT32, &t); }
  FOG_INLINE err_t setU16(uint16_t src) { uint32_t t = src; return fog_api.var_setType(this, VAR_TYPE_UINT32, &t); }

  FOG_INLINE err_t setI32(int32_t src) { return fog_api.var_setType(this, VAR_TYPE_INT32, &src); }
  FOG_INLINE err_t setU32(uint32_t src) { return fog_api.var_setType(this, VAR_TYPE_UINT32, &src); }
  FOG_INLINE err_t setI64(int64_t src) { return fog_api.var_setType(this, VAR_TYPE_INT64, &src); }
  FOG_INLINE err_t setU64(uint64_t src) { return fog_api.var_setType(this, VAR_TYPE_UINT64, &src); }

  FOG_INLINE err_t setSSizeT(size_t src)
  {
    return fog_api.var_setType(this, sizeof(ssize_t) == sizeof(int64_t) ? VAR_TYPE_INT64 : VAR_TYPE_INT32, &src);
  }

  FOG_INLINE err_t setSizeT(size_t src)
  {
    return fog_api.var_setType(this, sizeof(size_t) == sizeof(uint64_t) ? VAR_TYPE_UINT64 : VAR_TYPE_UINT32, &src);
  }

  FOG_INLINE err_t setLong(long src)
  {
    return fog_api.var_setType(this, sizeof(long) == sizeof(int64_t) ? VAR_TYPE_INT64 : VAR_TYPE_INT32, &src);
  }

  FOG_INLINE err_t setULong(ulong src)
  {
    return fog_api.var_setType(this, sizeof(ulong) == sizeof(uint64_t) ? VAR_TYPE_INT64 : VAR_TYPE_INT32, &src);
  }

  FOG_INLINE err_t setInt(int8_t src) { return setI8(src); }
  FOG_INLINE err_t setInt(uint8_t src) { return setU8(src); }
  FOG_INLINE err_t setInt(int16_t src) { return setI16(src); }
  FOG_INLINE err_t setInt(uint16_t src) { return setU16(src); }
  FOG_INLINE err_t setInt(int32_t src) { return setI32(src); }
  FOG_INLINE err_t setInt(uint32_t src) { return setU32(src); }
  FOG_INLINE err_t setInt(int64_t src) { return setI64(src); }
  FOG_INLINE err_t setInt(uint64_t src) { return setU64(src); }

  // --------------------------------------------------------------------------
  // [Accessors - Real]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t getFloat(float& dst) const { return fog_api.var_getFloat(this, &dst); }
  FOG_INLINE err_t getDouble(double& dst) const { return fog_api.var_getDouble(this, &dst); }

  FOG_INLINE err_t getFloat(float& dst, float min, float max) const { return fog_api.var_getFloatBound(this, &dst, min, max); }
  FOG_INLINE err_t getDouble(double& dst, double min, double max) const { return fog_api.var_getDoubleBound(this, &dst, min, max); }

  FOG_INLINE err_t getReal(float& dst) const { return getFloat(dst); }
  FOG_INLINE err_t getReal(double& dst) const { return getDouble(dst); }

  FOG_INLINE err_t getReal(float& dst, float min, float max) const { return getFloat(dst, min, max); }
  FOG_INLINE err_t getReal(double& dst, double min, double max) const { return getDouble(dst, min, max); }

  FOG_INLINE err_t setFloat(float src) { return fog_api.var_setType(this, VAR_TYPE_FLOAT, &src); }
  FOG_INLINE err_t setDouble(double src) { return fog_api.var_setType(this, VAR_TYPE_DOUBLE, &src); }

  FOG_INLINE err_t setReal(float src) { return setFloat(src); }
  FOG_INLINE err_t setReal(double src) { return setDouble(src); }

  // --------------------------------------------------------------------------
  // [Accessors - String]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t getString(StringA& dst) const { return fog_api.var_getType(this, VAR_TYPE_STRINGA, &dst); }
  FOG_INLINE err_t getString(StringW& dst) const { return fog_api.var_getType(this, VAR_TYPE_STRINGW, &dst); }

  FOG_INLINE err_t setString(const StringA& src) { return fog_api.var_setType(this, VAR_TYPE_STRINGA, &src); }
  FOG_INLINE err_t setString(const StringW& src) { return fog_api.var_setType(this, VAR_TYPE_STRINGW, &src); }
  FOG_INLINE err_t setString(const ManagedStringW& src) { return fog_api.var_setType(this, VAR_TYPE_STRINGW, &src); }

  // --------------------------------------------------------------------------
  // [Accessors - Var]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setVar(const Var& other)
  {
    return fog_api.var_copy(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Var& operator=(const Var& other)
  {
    fog_api.var_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const Var& other) const { return  fog_api.var_eq(this, &other); }
  FOG_INLINE bool operator!=(const Var& other) const { return !fog_api.var_eq(this, &other); }

  // --------------------------------------------------------------------------
  // [Statics - Instance]
  // --------------------------------------------------------------------------

  static FOG_INLINE const Var& null() { return *fog_api.var_oNull; }

  // --------------------------------------------------------------------------
  // [Statics - From]
  // --------------------------------------------------------------------------

  static FOG_INLINE Var fromType(uint32_t vType, const void* vData) { return Var(vType, vData); }

  static FOG_INLINE Var fromBool(bool val) { return Var(VAR_TYPE_BOOL, &val); }
  static FOG_INLINE Var fromChar(CharW val) { return Var(VAR_TYPE_CHAR, &val); }

#if defined(FOG_CC_HAS_NATIVE_CHAR_TYPE)
  static FOG_INLINE Var fromChar(char val) { return fromChar(CharW((unsigned char)val)); }
#endif // FOG_CC_HAS_NATIVE_CHAR_TYPE

  static FOG_INLINE Var fromChar(signed char val) { return fromChar(CharW((unsigned char)val)); }
  static FOG_INLINE Var fromChar(unsigned char val) { return fromChar(CharW(val)); }
  static FOG_INLINE Var fromChar(signed short val) { return fromChar(CharW((unsigned short)val)); }
  static FOG_INLINE Var fromChar(unsigned short val) { return fromChar(CharW(val)); }

  static FOG_INLINE Var fromI32(int32_t val) { return Var(VAR_TYPE_INT32, &val); }
  static FOG_INLINE Var fromU32(uint32_t val) { return Var(VAR_TYPE_UINT32, &val); }
  static FOG_INLINE Var fromI64(int64_t val) { return Var(VAR_TYPE_INT64, &val); }
  static FOG_INLINE Var fromU64(uint64_t val) { return Var(VAR_TYPE_UINT64, &val); }
  static FOG_INLINE Var fromFloat(float val) { return Var(VAR_TYPE_FLOAT, &val); }
  static FOG_INLINE Var fromDouble(double val) { return Var(VAR_TYPE_DOUBLE, &val); }

#if defined(FOG_CC_HAS_NATIVE_CHAR_TYPE)
  static FOG_INLINE Var fromInteger(char c) { return fromI32(c); }
#endif // FOG_CC_HAS_NATIVE_CHAR_TYPE
  static FOG_INLINE Var fromInteger(signed char c) { return fromI32(c); }
  static FOG_INLINE Var fromInteger(unsigned char c) { return fromU32(c); }

  static FOG_INLINE Var fromInteger(short c) { return fromI32(c); }
  static FOG_INLINE Var fromInteger(unsigned short c) { return fromU32(c); }

  static FOG_INLINE Var fromInteger(int c) { return fromI32(c); }
  static FOG_INLINE Var fromInteger(unsigned int c) { return fromU32(c); }

  static FOG_INLINE Var fromInteger(int64_t c) { return fromI64(c); }
  static FOG_INLINE Var fromInteger(uint64_t c) { return fromU64(c); }

  static FOG_INLINE Var fromReal(float val) { return fromFloat(val); }
  static FOG_INLINE Var fromReal(double val) { return fromDouble(val); }

  static FOG_INLINE Var fromStringA(const StringA& val) { return Var(VAR_TYPE_STRINGA, &val); }
  static FOG_INLINE Var fromStringW(const StringW& val) { return Var(VAR_TYPE_STRINGW, &val); }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const Var* a, const Var* b)
  {
    return fog_api.var_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.var_eq;
  }

  // --------------------------------------------------------------------------
  // [Compare]
  // --------------------------------------------------------------------------

  static FOG_INLINE int compare(const Var* a, const Var* b)
  {
    return fog_api.var_compare(a, b);
  }

  static FOG_INLINE CompareFunc getCompareFunc()
  {
    return (CompareFunc)fog_api.var_compare;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(VarData)
};

//! @}

} // Fog namespace

// ============================================================================
// [Include Specialized Containers]
// ============================================================================

#if defined(_FOG_CORE_TOOLS_LIST_H)
# include <Fog/Core/Tools/ListVar.h>
#endif // _FOG_CORE_TOOLS_LIST_H

// [Guard]
#endif // _FOG_CORE_TOOLS_VAR_H
