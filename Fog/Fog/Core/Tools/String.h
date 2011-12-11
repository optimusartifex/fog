// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_STRING_H
#define _FOG_CORE_TOOLS_STRING_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/ListUntyped.h>
#include <Fog/Core/Tools/Range.h>
#include <Fog/Core/Tools/Stub.h>
#include <Fog/Core/Tools/TextCodec.h>

// [Dependencies - C]
#include <wchar.h>

//! @addtogroup Fog_Core_Tools
//! @{

namespace Fog {

// ============================================================================
// [Fog::FormatInt]
// ============================================================================

struct FOG_NO_EXPORT FormatInt
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FormatInt(uint32_t base = 10, uint32_t flags = NO_FLAGS, size_t precision = NO_PRECISION, size_t width = NO_WIDTH) :
    _base(base),
    _flags(flags),
    _precision(precision),
    _width(width)
  {
  }

  FOG_INLINE FormatInt(const FormatInt& other)
  {
    MemOps::copy_t<FormatInt>(this, &other);
  }

  explicit FOG_INLINE FormatInt(_Uninitialized)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getBase() const { return _base; }
  FOG_INLINE uint32_t getFlags() const { return _flags; }
  FOG_INLINE size_t getPrecision() const { return _precision; }
  FOG_INLINE size_t getWidth() const { return _width; }

  FOG_INLINE void setBase(uint32_t base) { _base = base; }
  FOG_INLINE void setFlags(uint32_t flags) { _flags = flags; }
  FOG_INLINE void setPrecision(size_t precision) { _precision = precision; }
  FOG_INLINE void setWidth(size_t width) { _width = width; }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FormatInt& operator=(const FormatInt& other)
  {
    MemOps::copy_t<FormatInt>(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FormatInt& other) const { return  MemOps::eq_t<FormatInt>(this, &other); }
  FOG_INLINE bool operator!=(const FormatInt& other) const { return !MemOps::eq_t<FormatInt>(this, &other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _base;
  uint32_t _flags;
  size_t _precision;
  size_t _width;
};

// ============================================================================
// [Fog::FormatReal]
// ============================================================================

struct FOG_NO_EXPORT FormatReal
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FormatReal(uint32_t form = DF_SIGNIFICANT_DIGITS, uint32_t flags = NO_FLAGS, size_t precision = NO_PRECISION, size_t width = NO_WIDTH) :
    _form(form),
    _flags(flags),
    _precision(precision),
    _width(width)
  {
  }

  FOG_INLINE FormatReal(const FormatReal& other)
  {
    MemOps::copy_t<FormatReal>(this, &other);
  }

  explicit FOG_INLINE FormatReal(_Uninitialized)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getForm() const { return _form; }
  FOG_INLINE uint32_t getFlags() const { return _flags; }
  FOG_INLINE size_t getPrecision() const { return _precision; }
  FOG_INLINE size_t getWidth() const { return _width; }

  FOG_INLINE void setForm(uint32_t form) { _form = form; }
  FOG_INLINE void setFlags(uint32_t flags) { _flags = flags; }
  FOG_INLINE void setPrecision(size_t precision) { _precision = precision; }
  FOG_INLINE void setWidth(size_t width) { _width = width; }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FormatReal& operator=(const FormatReal& other)
  {
    MemOps::copy_t<FormatReal>(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FormatReal& other) const { return  MemOps::eq_t<FormatReal>(this, &other); }
  FOG_INLINE bool operator!=(const FormatReal& other) const { return !MemOps::eq_t<FormatReal>(this, &other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _form;
  uint32_t _flags;
  size_t _precision;
  size_t _width;
};

// ============================================================================
// [Fog::StringDataA]
// ============================================================================

#include <Fog/Core/C++/PackByte.h>
struct FOG_NO_EXPORT StringDataA
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE StringDataA* addRef() const
  {
    reference.inc();
    return const_cast<StringDataA*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      fog_api.stringa_dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE size_t getSizeOf(size_t capacity)
  {
    return sizeof(StringDataA) + sizeof(char) * capacity;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // ${VAR:BEGIN}
  //
  // This data-object is binary compatible with the VarData header in the second
  // form called - "implicitly shared container". The members must be binary
  // compatible with the header below:
  //
  // +==============+============+============================================+
  // | Size         | Name       | Description / Purpose                      |
  // +==============+============+============================================+
  // | size_t       | reference  | Atomic reference count, can be managed by  |
  // |              |            | VarData without calling container specific |
  // |              |            | methods.                                   |
  // +--------------+------------+--------------------------------------------+
  // | uint32_t     | vType      | Variable type and flags.                   |
  // +--------------+------------+--------------------------------------------+
  // | uint32_t     | padding0_32| Not used by the Var. This member is only   |
  // |              |            | defined for 64-bit compilation to pad      |
  // |              |            | other members!                             |
  // +--------------+------------+--------------------------------------------+
  // | size_t       | capacity   | Capacity of the container (items).         |
  // +--------------+------------+--------------------------------------------+
  // | size_t       | length     | Length of the container (items).           |
  // +==============+============+============================================+
  //
  // ${VAR:END}

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Variable type and flags.
  uint32_t vType;

#if FOG_ARCH_BITS >= 64
  //! @brief Hash code.
  uint32_t hashCode;
#endif // FOG_ARCH_BITS >= 64

  //! @brief Capacity.
  size_t capacity;
  //! @brief Length.
  size_t length;

#if FOG_ARCH_BITS < 64
  //! @brief Hash code.
  uint32_t hashCode;
#endif // FOG_ARCH_BITS < 64

  char data[4];
};
#include <Fog/Core/C++/PackRestore.h>

// ============================================================================
// [Fog::StringDataW]
// ============================================================================

#include <Fog/Core/C++/PackByte.h>
struct FOG_NO_EXPORT StringDataW
{
  FOG_INLINE StringDataW* addRef() const
  {
    reference.inc();
    return const_cast<StringDataW*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      fog_api.stringw_dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE size_t getSizeOf(size_t capacity)
  {
    return sizeof(StringDataW) - sizeof(CharW) + sizeof(CharW) * capacity;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // ${VAR:BEGIN}
  //
  // This data-object is binary compatible with the VarData header in the second
  // form called - "implicitly shared container". The members must be binary
  // compatible with the header below:
  //
  // +==============+============+============================================+
  // | Size         | Name       | Description / Purpose                      |
  // +==============+============+============================================+
  // | size_t       | reference  | Atomic reference count, can be managed by  |
  // |              |            | VarData without calling container specific |
  // |              |            | methods.                                   |
  // +--------------+------------+--------------------------------------------+
  // | uint32_t     | vType      | Variable type and flags.                   |
  // +--------------+------------+--------------------------------------------+
  // | uint32_t     | padding0_32| Not used by the Var. This member is only   |
  // |              |            | defined for 64-bit compilation to pad      |
  // |              |            | other members!                             |
  // +--------------+------------+--------------------------------------------+
  // | size_t       | capacity   | Capacity of the container (items).         |
  // +--------------+------------+--------------------------------------------+
  // | size_t       | length     | Length of the container (items).           |
  // +==============+============+============================================+
  //
  // ${VAR:END}

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Variable type and flags.
  uint32_t vType;

#if FOG_ARCH_BITS >= 64
  //! @brief Hash code.
  uint32_t hashCode;
#endif // FOG_ARCH_BITS >= 64

  //! @brief Capacity.
  size_t capacity;
  //! @brief Length.
  size_t length;

#if FOG_ARCH_BITS < 64
  //! @brief Hash code.
  uint32_t hashCode;
#endif // FOG_ARCH_BITS < 64

  CharW data[2];
};
#include <Fog/Core/C++/PackRestore.h>

// ============================================================================
// [Fog::StringA]
// ============================================================================

//! @brief String (ansi).
struct FOG_NO_EXPORT StringA
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE StringA()
  {
    fog_api.stringa_ctor(this);
  }

  FOG_INLINE StringA(const StringA& other)
  {
    fog_api.stringa_ctorCopyA(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE StringA(StringA&& other) : _d(other._d) { other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  FOG_INLINE StringA(const StringA& other, const Range& range)
  {
    fog_api.stringa_ctorSubstr(this, &other, &range);
  }

  explicit FOG_INLINE StringA(const StubA& str)
  {
    fog_api.stringa_ctorStubA(this, &str);
  }

  explicit FOG_INLINE StringA(const char* str, size_t length = DETECT_LENGTH)
  {
    StubA stub(str, length);
    fog_api.stringa_ctorStubA(this, &stub);
  }

  FOG_INLINE StringA(const StringA& a, const StringA& b)
  {
    fog_api.stringa_ctorStringAStringA(this, &a, &b);
  }

  FOG_INLINE StringA(const StringA& a, const StubA& b)
  {
    fog_api.stringa_ctorStringAStubA(this, &a, &b);
  }

  FOG_INLINE StringA(const StringA& a, const char* b)
  {
    StubA sb(b);
    fog_api.stringa_ctorStringAStubA(this, &a, &sb);
  }

  FOG_INLINE StringA(const StubA& a, const StringA& b)
  {
    fog_api.stringa_ctorStubAStringA(this, &a, &b);
  }

  FOG_INLINE StringA(const StubA& a, const StubA& b)
  {
    fog_api.stringa_ctorStubAStubA(this, &a, &b);
  }

  FOG_INLINE StringA(const char* a, const char* b)
  {
    StubA sa(a);
    StubA sb(b);
    fog_api.stringa_ctorStubAStubA(this, &sa, &sb);
  }

  explicit FOG_INLINE StringA(_Uninitialized)
  {
  }

  explicit FOG_INLINE StringA(StringDataA* d) : _d(d)
  {
  }

  FOG_INLINE ~StringA()
  {
    fog_api.stringa_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getReference().
  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return getReference() == 1; }

  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach()
  {
    return isDetached() ? (err_t)ERR_OK : fog_api.stringa_detach(this);
  }

  FOG_INLINE err_t _detach()
  {
    return fog_api.stringa_detach(this);
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  //! @brief Get string capacity.
  FOG_INLINE size_t getCapacity() const { return _d->capacity; }
  //! @brief Get string length.
  FOG_INLINE size_t getLength() const { return _d->length; }
  //! @brief Get whether the string is empty.
  FOG_INLINE bool isEmpty() const { return _d->length == 0; }

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return fog_api.stringa_reserve(this, capacity);
  }

  FOG_INLINE err_t resize(size_t length)
  {
    return fog_api.stringa_resize(this, length);
  }

  FOG_INLINE err_t truncate(size_t length)
  {
    return fog_api.stringa_truncate(this, length);
  }

  FOG_INLINE void squeeze()
  {
    fog_api.stringa_squeeze(this);
  }

  FOG_INLINE char* _prepare(uint32_t cntOp, size_t length)
  {
    return fog_api.stringa_prepare(this, cntOp, length);
  }

  FOG_INLINE char* _add(size_t length)
  {
    return fog_api.stringa_add(this, length);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get a const pointer to the string data.
  FOG_INLINE const char* getData() const
  {
    return _d->data;
  }

  FOG_INLINE char* getDataX()
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::StringA::getDataX() - Not detached.");

    return _d->data;
  }

  FOG_INLINE char getAt(size_t index) const
  {
    FOG_ASSERT_X(index < getLength(),
      "Fog::StringA::getAt() - Index out of range.");

    return _d->data[index];
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    return fog_api.stringa_clear(this);
  }

  FOG_INLINE void reset()
  {
    return fog_api.stringa_reset(this);
  }

  // --------------------------------------------------------------------------
  // [HashCode]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getHashCode() const
  {
    return fog_api.stringa_getHashCode(this);
  }

  // --------------------------------------------------------------------------
  // [SetChar / SetChars]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setChar(char ch)
  {
    return fog_api.stringa_opFill(this, CONTAINER_OP_REPLACE, ch, 1);
  }

  FOG_INLINE err_t setChars(char ch, size_t length)
  {
    return fog_api.stringa_opFill(this, CONTAINER_OP_REPLACE, ch, length);
  }

  // --------------------------------------------------------------------------
  // [SetString]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t _set(const StubA& stub) { return fog_api.stringa_setStubA(this, &stub); }
  FOG_INLINE err_t _set(const StringA& other) { return fog_api.stringa_setStringA(this, &other); }

  FOG_INLINE err_t set(const StubA& stub)
  {
    return fog_api.stringa_setStubA(this, &stub);
  }

  FOG_INLINE err_t set(const StringA& other)
  {
    return fog_api.stringa_setStringA(this, &other);
  }

  FOG_INLINE err_t set(const char* str, size_t length = DETECT_LENGTH)
  {
    StubA stubA(str, length);
    return fog_api.stringa_setStubA(this, &stubA);
  }

  FOG_INLINE err_t setDeep(const StringA& other)
  {
    return fog_api.stringa_setDeep(this, &other);
  }

  FOG_INLINE err_t setAndNormalizeSlashes(const StringA& other, uint32_t slashForm)
  {
    return fog_api.stringa_opNormalizeSlashesA(this, CONTAINER_OP_REPLACE, &other, NULL, slashForm);
  }

  FOG_INLINE err_t setAndNormalizeSlashes(const StringA& other, const Range& range, uint32_t slashForm)
  {
    return fog_api.stringa_opNormalizeSlashesA(this, CONTAINER_OP_REPLACE, &other, &range, slashForm);
  }

  // --------------------------------------------------------------------------
  // [SetBool]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setBool(bool b) { return fog_api.stringa_opBool(this, CONTAINER_OP_REPLACE, b); }

  // --------------------------------------------------------------------------
  // [SetInt]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setInt(int8_t n) { return fog_api.stringa_opI32(this, CONTAINER_OP_REPLACE, n); }
  FOG_INLINE err_t setInt(uint8_t n) { return fog_api.stringa_opU32(this, CONTAINER_OP_REPLACE, n); }
  FOG_INLINE err_t setInt(int16_t n) { return fog_api.stringa_opI32(this, CONTAINER_OP_REPLACE, n); }
  FOG_INLINE err_t setInt(uint16_t n) { return fog_api.stringa_opU32(this, CONTAINER_OP_REPLACE, n); }
  FOG_INLINE err_t setInt(int32_t n) { return fog_api.stringa_opI32(this, CONTAINER_OP_REPLACE, n); }
  FOG_INLINE err_t setInt(uint32_t n) { return fog_api.stringa_opU32(this, CONTAINER_OP_REPLACE, n); }

  FOG_INLINE err_t setInt(int64_t n) { return fog_api.stringa_opI64(this, CONTAINER_OP_REPLACE, n); }
  FOG_INLINE err_t setInt(uint64_t n) { return fog_api.stringa_opU64(this, CONTAINER_OP_REPLACE, n); }

  FOG_INLINE err_t setInt(int8_t n, const FormatInt& fmt) { return fog_api.stringa_opI32Ex(this, CONTAINER_OP_REPLACE, n, &fmt); }
  FOG_INLINE err_t setInt(uint8_t n, const FormatInt& fmt) { return fog_api.stringa_opU32Ex(this, CONTAINER_OP_REPLACE, n, &fmt); }
  FOG_INLINE err_t setInt(int16_t n, const FormatInt& fmt) { return fog_api.stringa_opI32Ex(this, CONTAINER_OP_REPLACE, n, &fmt); }
  FOG_INLINE err_t setInt(uint16_t n, const FormatInt& fmt) { return fog_api.stringa_opU32Ex(this, CONTAINER_OP_REPLACE, n, &fmt); }
  FOG_INLINE err_t setInt(int32_t n, const FormatInt& fmt) { return fog_api.stringa_opI32Ex(this, CONTAINER_OP_REPLACE, n, &fmt); }
  FOG_INLINE err_t setInt(uint32_t n, const FormatInt& fmt) { return fog_api.stringa_opU32Ex(this, CONTAINER_OP_REPLACE, n, &fmt); }

  FOG_INLINE err_t setInt(int64_t n, const FormatInt& fmt) { return fog_api.stringa_opI64Ex(this, CONTAINER_OP_REPLACE, n, &fmt); }
  FOG_INLINE err_t setInt(uint64_t n, const FormatInt& fmt) { return fog_api.stringa_opU64Ex(this, CONTAINER_OP_REPLACE, n, &fmt); }

  // --------------------------------------------------------------------------
  // [SetReal]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setReal(float d)
  {
    return fog_api.stringa_opDouble(this, CONTAINER_OP_REPLACE, d);
  }

  FOG_INLINE err_t setReal(float d, const FormatReal& fmt)
  {
    return fog_api.stringa_opDoubleEx(this, CONTAINER_OP_REPLACE, d, &fmt);
  }

  FOG_INLINE err_t setReal(double d)
  {
    return fog_api.stringa_opDouble(this, CONTAINER_OP_REPLACE, d);
  }

  FOG_INLINE err_t setReal(double d, const FormatReal& fmt)
  {
    return fog_api.stringa_opDoubleEx(this, CONTAINER_OP_REPLACE, d, &fmt);
  }

  // --------------------------------------------------------------------------
  // [Format]
  // --------------------------------------------------------------------------

  FOG_NO_INLINE err_t format(const char* fmt, ...)
  {
    StubA fmtA(fmt, DETECT_LENGTH);

    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringa_opVFormatStubA(this, CONTAINER_OP_REPLACE, &fmtA, NULL, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t format_c(const char* fmt, const TextCodec& tc, ...)
  {
    StubA fmtA(fmt, DETECT_LENGTH);

    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringa_opVFormatStubA(this, CONTAINER_OP_REPLACE, &fmtA, &tc, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t format(const StubA& fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringa_opVFormatStubA(this, CONTAINER_OP_REPLACE, &fmt, NULL, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t format_c(const StubA& fmt, const TextCodec& tc, ...)
  {
    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringa_opVFormatStubA(this, CONTAINER_OP_REPLACE, &fmt, &tc, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t format(const StringA& fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringa_opVFormatStringA(this, CONTAINER_OP_REPLACE, &fmt, NULL, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t format_c(const StringA& fmt, const TextCodec& tc, ...)
  {
    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringa_opVFormatStringA(this, CONTAINER_OP_REPLACE, &fmt, &tc, ap);
    va_end(ap);

    return err;
  }

  // --------------------------------------------------------------------------
  // [vFormat]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t vFormat(const char* fmt, va_list ap)
  {
    StubA fmtA(fmt, DETECT_LENGTH);
    return fog_api.stringa_opVFormatStubA(this, CONTAINER_OP_REPLACE, &fmtA, NULL, ap);
  }

  FOG_INLINE err_t vFormat_c(const char* fmt, const TextCodec& tc, va_list ap)
  {
    StubA fmtA(fmt, DETECT_LENGTH);
    return fog_api.stringa_opVFormatStubA(this, CONTAINER_OP_REPLACE, &fmtA, &tc, ap);
  }

  FOG_INLINE err_t vFormat(const StubA& fmt, va_list ap)
  {
    return fog_api.stringa_opVFormatStubA(this, CONTAINER_OP_REPLACE, &fmt, NULL, ap);
  }

  FOG_INLINE err_t vFormat_c(const StubA& fmt, const TextCodec& tc, va_list ap)
  {
    return fog_api.stringa_opVFormatStubA(this, CONTAINER_OP_REPLACE, &fmt, &tc, ap);
  }

  FOG_INLINE err_t vFormat(const StringA& fmt, va_list ap)
  {
    return fog_api.stringa_opVFormatStringA(this, CONTAINER_OP_REPLACE, &fmt, NULL, ap);
  }

  FOG_INLINE err_t vFormat_c(const StringA& fmt, const TextCodec& tc, va_list ap)
  {
    return fog_api.stringa_opVFormatStringA(this, CONTAINER_OP_REPLACE, &fmt, &tc, ap);
  }

  // --------------------------------------------------------------------------
  // [ZFormat]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t zFormat(const char* fmt, char lex, const List<StringA>& args)
  {
    StubA fmtA(fmt, DETECT_LENGTH);
    ListUntypedData* args_d = reinterpret_cast<const ListUntyped*>(&args)->_d;

    return fog_api.stringa_opZFormatStubA(this, CONTAINER_OP_REPLACE, &fmtA, lex,
      reinterpret_cast<const StringA*>(args_d->data), args_d->length);
  }

  FOG_INLINE err_t zFormat(const char* fmt, char lex, const StringA* args, size_t argsLength)
  {
    StubA fmtA(fmt, DETECT_LENGTH);
    return fog_api.stringa_opZFormatStubA(this, CONTAINER_OP_REPLACE, &fmtA, lex,
      args, argsLength);
  }

  FOG_INLINE err_t zFormat(const StubA& fmt, char lex, const List<StringA>& args)
  {
    ListUntypedData* args_d = reinterpret_cast<const ListUntyped*>(&args)->_d;

    return fog_api.stringa_opZFormatStubA(this, CONTAINER_OP_REPLACE, &fmt, lex,
      reinterpret_cast<const StringA*>(args_d->data), args_d->length);
  }

  FOG_INLINE err_t zFormat(const StubA& fmt, char lex, const StringA* args, size_t argsLength)
  {
    return fog_api.stringa_opZFormatStubA(this, CONTAINER_OP_REPLACE, &fmt, lex,
      args, argsLength);
  }

  FOG_INLINE err_t zFormat(const StringA& fmt, char lex, const List<StringA>& args)
  {
    ListUntypedData* args_d = reinterpret_cast<const ListUntyped*>(&args)->_d;

    return fog_api.stringa_opZFormatStringA(this, CONTAINER_OP_REPLACE, &fmt, lex,
      reinterpret_cast<const StringA*>(args_d->data), args_d->length);
  }

  FOG_INLINE err_t zFormat(const StringA& fmt, char lex, const StringA* args, size_t argsLength)
  {
    return fog_api.stringa_opZFormatStringA(this, CONTAINER_OP_REPLACE, &fmt, lex,
      args, argsLength);
  }

  // --------------------------------------------------------------------------
  // [NormalizeSlashes]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t normalizeSlashes(uint32_t slashForm)
  {
    return fog_api.stringa_opNormalizeSlashesA(this, CONTAINER_OP_REPLACE, this, NULL, slashForm);
  }

  // --------------------------------------------------------------------------
  // [Append]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t _append(const StubA& stub)
  {
    return fog_api.stringa_appendStubA(this, &stub);
  }

  FOG_INLINE err_t _append(const StringA& other)
  {
    return fog_api.stringa_appendStringA(this, &other);
  }

  FOG_INLINE err_t append(char ch)
  {
    return fog_api.stringa_opFill(this, CONTAINER_OP_APPEND, ch, 1);
  }

  FOG_INLINE err_t append(char ch, size_t length)
  {
    return fog_api.stringa_opFill(this, CONTAINER_OP_APPEND, ch, length);
  }

  FOG_INLINE err_t append(const char* str, size_t length = DETECT_LENGTH)
  {
    StubA stub(str, length);
    return fog_api.stringa_appendStubA(this, &stub);
  }

  FOG_INLINE err_t append(const StubA& stub)
  {
    return fog_api.stringa_appendStubA(this, &stub);
  }

  FOG_INLINE err_t append(const StringA& other)
  {
    return fog_api.stringa_appendStringA(this, &other);
  }

  FOG_INLINE err_t appendAndNormalizeSlashes(const StringA& other, uint32_t slashForm)
  {
    return fog_api.stringa_opNormalizeSlashesA(this, CONTAINER_OP_APPEND, &other, NULL, slashForm);
  }

  FOG_INLINE err_t appendAndNormalizeSlashes(const StringA& other, const Range& range, uint32_t slashForm)
  {
    return fog_api.stringa_opNormalizeSlashesA(this, CONTAINER_OP_APPEND, &other, &range, slashForm);
  }

  // --------------------------------------------------------------------------
  // [AppendBool]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t appendBool(bool b)
  {
    return fog_api.stringa_opBool(this, CONTAINER_OP_APPEND, b);
  }

  // --------------------------------------------------------------------------
  // [AppendInt]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t appendInt(int8_t n) { return fog_api.stringa_opI32(this, CONTAINER_OP_APPEND, n); }
  FOG_INLINE err_t appendInt(uint8_t n) { return fog_api.stringa_opU32(this, CONTAINER_OP_APPEND, n); }
  FOG_INLINE err_t appendInt(int16_t n) { return fog_api.stringa_opI32(this, CONTAINER_OP_APPEND, n); }
  FOG_INLINE err_t appendInt(uint16_t n) { return fog_api.stringa_opU32(this, CONTAINER_OP_APPEND, n); }
  FOG_INLINE err_t appendInt(int32_t n) { return fog_api.stringa_opI32(this, CONTAINER_OP_APPEND, n); }
  FOG_INLINE err_t appendInt(uint32_t n) { return fog_api.stringa_opU32(this, CONTAINER_OP_APPEND, n); }
  FOG_INLINE err_t appendInt(int64_t n) { return fog_api.stringa_opI64(this, CONTAINER_OP_APPEND, n); }
  FOG_INLINE err_t appendInt(uint64_t n) { return fog_api.stringa_opU64(this, CONTAINER_OP_APPEND, n); }

  FOG_INLINE err_t appendInt(int8_t n, const FormatInt& fmt) { return fog_api.stringa_opI32Ex(this, CONTAINER_OP_APPEND, n, &fmt); }
  FOG_INLINE err_t appendInt(uint8_t n, const FormatInt& fmt) { return fog_api.stringa_opU32Ex(this, CONTAINER_OP_APPEND, n, &fmt); }
  FOG_INLINE err_t appendInt(int16_t n, const FormatInt& fmt) { return fog_api.stringa_opI32Ex(this, CONTAINER_OP_APPEND, n, &fmt); }
  FOG_INLINE err_t appendInt(uint16_t n, const FormatInt& fmt) { return fog_api.stringa_opU32Ex(this, CONTAINER_OP_APPEND, n, &fmt); }
  FOG_INLINE err_t appendInt(int32_t n, const FormatInt& fmt) { return fog_api.stringa_opI32Ex(this, CONTAINER_OP_APPEND, n, &fmt); }
  FOG_INLINE err_t appendInt(uint32_t n, const FormatInt& fmt) { return fog_api.stringa_opU32Ex(this, CONTAINER_OP_APPEND, n, &fmt); }
  FOG_INLINE err_t appendInt(int64_t n, const FormatInt& fmt) { return fog_api.stringa_opI64Ex(this, CONTAINER_OP_APPEND, n, &fmt); }
  FOG_INLINE err_t appendInt(uint64_t n, const FormatInt& fmt) { return fog_api.stringa_opU64Ex(this, CONTAINER_OP_APPEND, n, &fmt); }

  // --------------------------------------------------------------------------
  // [AppendReal]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t appendReal(float d)
  {
    return fog_api.stringa_opDouble(this, CONTAINER_OP_APPEND, d);
  }

  FOG_INLINE err_t appendReal(float d, const FormatReal& fmt)
  {
    return fog_api.stringa_opDoubleEx(this, CONTAINER_OP_APPEND, d, &fmt);
  }

  FOG_INLINE err_t appendReal(double d)
  {
    return fog_api.stringa_opDouble(this, CONTAINER_OP_APPEND, d);
  }

  FOG_INLINE err_t appendReal(double d, const FormatReal& fmt)
  {
    return fog_api.stringa_opDoubleEx(this, CONTAINER_OP_APPEND, d, &fmt);
  }

  // --------------------------------------------------------------------------
  // [AppendFormat]
  // --------------------------------------------------------------------------

  FOG_NO_INLINE err_t appendFormat(const char* fmt, ...)
  {
    StubA fmtA(fmt, DETECT_LENGTH);

    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringa_opVFormatStubA(this, CONTAINER_OP_APPEND, &fmtA, NULL, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t appendFormat_c(const char* fmt, const TextCodec& tc, ...)
  {
    StubA fmtA(fmt, DETECT_LENGTH);

    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringa_opVFormatStubA(this, CONTAINER_OP_APPEND, &fmtA, &tc, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t appendFormat(const StubA& fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringa_opVFormatStubA(this, CONTAINER_OP_APPEND, &fmt, NULL, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t appendFormat_c(const StubA& fmt, const TextCodec& tc, ...)
  {
    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringa_opVFormatStubA(this, CONTAINER_OP_APPEND, &fmt, &tc, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t appendFormat(const StringA& fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringa_opVFormatStringA(this, CONTAINER_OP_APPEND, &fmt, NULL, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t appendFormat_c(const StringA& fmt, const TextCodec& tc, ...)
  {
    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringa_opVFormatStringA(this, CONTAINER_OP_APPEND, &fmt, &tc, ap);
    va_end(ap);

    return err;
  }

  // --------------------------------------------------------------------------
  // [AppendVFormat]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t appendVFormat(const char* fmt, va_list ap)
  {
    StubA fmtA(fmt, DETECT_LENGTH);
    return fog_api.stringa_opVFormatStubA(this, CONTAINER_OP_APPEND, &fmtA, NULL, ap);
  }

  FOG_INLINE err_t appendVFormat_c(const char* fmt, const TextCodec& tc, va_list ap)
  {
    StubA fmtA(fmt, DETECT_LENGTH);
    return fog_api.stringa_opVFormatStubA(this, CONTAINER_OP_APPEND, &fmtA, &tc, ap);
  }

  FOG_INLINE err_t appendVFormat(const StubA& fmt, va_list ap)
  {
    return fog_api.stringa_opVFormatStubA(this, CONTAINER_OP_APPEND, &fmt, NULL, ap);
  }

  FOG_INLINE err_t appendVFormat_c(const StubA& fmt, const TextCodec& tc, va_list ap)
  {
    return fog_api.stringa_opVFormatStubA(this, CONTAINER_OP_APPEND, &fmt, &tc, ap);
  }

  FOG_INLINE err_t appendVFormat(const StringA& fmt, va_list ap)
  {
    return fog_api.stringa_opVFormatStringA(this, CONTAINER_OP_APPEND, &fmt, NULL, ap);
  }

  FOG_INLINE err_t appendVFormat_c(const StringA& fmt, const TextCodec& tc, va_list ap)
  {
    return fog_api.stringa_opVFormatStringA(this, CONTAINER_OP_APPEND, &fmt, &tc, ap);
  }

  // --------------------------------------------------------------------------
  // [AppendZFormat]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t appendZFormat(const char* fmt, char lex, const List<StringA>& args)
  {
    StubA fmtA(fmt, DETECT_LENGTH);
    ListUntypedData* args_d = reinterpret_cast<const ListUntyped*>(&args)->_d;

    return fog_api.stringa_opZFormatStubA(this, CONTAINER_OP_APPEND, &fmtA, lex,
      reinterpret_cast<const StringA*>(args_d->data), args_d->length);
  }

  FOG_INLINE err_t appendZFormat(const char* fmt, char lex, const StringA* args, size_t argsLength)
  {
    StubA fmtA(fmt, DETECT_LENGTH);
    return fog_api.stringa_opZFormatStubA(this, CONTAINER_OP_APPEND, &fmtA, lex,
      args, argsLength);
  }

  FOG_INLINE err_t appendZFormat(const StubA& fmt, char lex, const List<StringA>& args)
  {
    ListUntypedData* args_d = reinterpret_cast<const ListUntyped*>(&args)->_d;

    return fog_api.stringa_opZFormatStubA(this, CONTAINER_OP_APPEND, &fmt, lex,
      reinterpret_cast<const StringA*>(args_d->data), args_d->length);
  }

  FOG_INLINE err_t appendZFormat(const StubA& fmt, char lex, const StringA* args, size_t argsLength)
  {
    return fog_api.stringa_opZFormatStubA(this, CONTAINER_OP_APPEND, &fmt, lex,
      args, argsLength);
  }

  FOG_INLINE err_t appendZFormat(const StringA& fmt, char lex, const List<StringA>& args)
  {
    ListUntypedData* args_d = reinterpret_cast<const ListUntyped*>(&args)->_d;

    return fog_api.stringa_opZFormatStringA(this, CONTAINER_OP_APPEND, &fmt, lex,
      reinterpret_cast<const StringA*>(args_d->data), args_d->length);
  }

  FOG_INLINE err_t appendZFormat(const StringA& fmt, char lex, const StringA* args, size_t argsLength)
  {
    return fog_api.stringa_opZFormatStringA(this, CONTAINER_OP_APPEND, &fmt, lex,
      args, argsLength);
  }

  // --------------------------------------------------------------------------
  // [Prepend]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t prepend(char ch)
  {
    return fog_api.stringa_prependChars(this, ch, 1);
  }

  FOG_INLINE err_t prepend(char ch, size_t length)
  {
    return fog_api.stringa_prependChars(this, ch, length);
  }

  FOG_INLINE err_t prepend(const char* str, size_t length = DETECT_LENGTH)
  {
    StubA stub(str, length);
    return fog_api.stringa_prependStubA(this, &stub);
  }

  FOG_INLINE err_t prepend(const StubA& stub)
  {
    return fog_api.stringa_prependStubA(this, &stub);
  }

  FOG_INLINE err_t prepend(const StringA& other)
  {
    return fog_api.stringa_prependStringA(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Insert]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t insert(size_t index, char ch)
  {
    return fog_api.stringa_insertChars(this, index, ch, 1);
  }

  FOG_INLINE err_t insert(size_t index, char ch, size_t length)
  {
    return fog_api.stringa_insertChars(this, index, ch, length);
  }

  FOG_INLINE err_t insert(size_t index, const char* str, size_t length = DETECT_LENGTH)
  {
    StubA stub(str, length);
    return fog_api.stringa_insertStubA(this, index, &stub);
  }

  FOG_INLINE err_t insert(size_t index, const StubA& stub)
  {
    return fog_api.stringa_insertStubA(this, index, &stub);
  }

  FOG_INLINE err_t insert(size_t index, const StringA& other)
  {
    return fog_api.stringa_insertStringA(this, index, &other);
  }

  // --------------------------------------------------------------------------
  // [Remove]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t remove(const Range& range)
  {
    return fog_api.stringa_removeRange(this, &range);
  }

  FOG_INLINE err_t remove(const Range* range, size_t rangeLength)
  {
    return fog_api.stringa_removeRangeList(this, range, rangeLength);
  }

  FOG_INLINE err_t remove(char ch, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringa_removeChar(this, NULL, ch, cs);
  }

  FOG_INLINE err_t remove(const StubA& stub, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringa_removeStubA(this, NULL, &stub, cs);
  }

  FOG_INLINE err_t remove(const StringA& other, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringa_removeStringA(this, NULL, &other, cs);
  }

  FOG_INLINE err_t remove(const RegExpA& re)
  {
    return fog_api.stringa_removeRegExpA(this, NULL, &re);
  }

  FOG_INLINE err_t remove(const Range& range, char ch, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringa_removeChar(this, &range, ch, cs);
  }

  FOG_INLINE err_t remove(const Range& range, const StubA& stub, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringa_removeStubA(this, &range, &stub, cs);
  }

  FOG_INLINE err_t remove(const Range& range, const StringA& other, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringa_removeStringA(this, &range, &other, cs);
  }

  FOG_INLINE err_t remove(const Range& range, const RegExpA& re)
  {
    return fog_api.stringa_removeRegExpA(this, &range, &re);
  }

  // --------------------------------------------------------------------------
  // [Replace]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t replace(const Range& range, const StubA& replacement)
  {
    return fog_api.stringa_replaceRangeStubA(this, &range, &replacement);
  }

  FOG_INLINE err_t replace(const Range& range, const StringA& replacement)
  {
    return fog_api.stringa_replaceRangeStringA(this, &range, &replacement);
  }

  FOG_INLINE err_t replace(const Range* range, size_t rangeLength, const StubA& replacement)
  {
    return fog_api.stringa_replaceRangeListStubA(this, range, rangeLength, &replacement);
  }

  FOG_INLINE err_t replace(const Range* range, size_t rangeLength, const StringA& replacement)
  {
    return fog_api.stringa_replaceRangeListStringA(this, range, rangeLength, &replacement);
  }

  FOG_INLINE err_t replace(char before, char after, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringa_replaceChar(this, NULL, before, after, cs);
  }

  FOG_INLINE err_t replace(const StringA& pattern, const StringA& replacement, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringa_replaceStringA(this, NULL, &pattern, &replacement, cs);
  }

  FOG_INLINE err_t replace(const RegExpA& re, const StringA& replacement)
  {
    return fog_api.stringa_replaceRegExpA(this, NULL, &re, &replacement);
  }

  FOG_INLINE err_t replace(const Range& range, char before, char after, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringa_replaceChar(this, &range, before, after, cs);
  }

  FOG_INLINE err_t replace(const Range& range, const StringA& pattern, const StringA& replacement, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringa_replaceStringA(this, &range, &pattern, &replacement, cs);
  }

  FOG_INLINE err_t replace(const Range& range, const RegExpA& re, const StringA& replacement)
  {
    return fog_api.stringa_replaceRegExpA(this, &range, &re, &replacement);
  }

  // --------------------------------------------------------------------------
  // [Lower / Upper]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t lower() { return fog_api.stringa_lower(this, NULL); }
  FOG_INLINE err_t upper() { return fog_api.stringa_upper(this, NULL); }

  FOG_INLINE err_t lower(const Range& range) { return fog_api.stringa_lower(this, &range); }
  FOG_INLINE err_t upper(const Range& range) { return fog_api.stringa_upper(this, &range); }

  FOG_INLINE StringA lowered() const
  {
    StringA result(*this);
    fog_api.stringa_lower(&result, NULL);
    return result;
  }

  FOG_INLINE StringA uppered() const
  {
    StringA result(*this);
    fog_api.stringa_upper(&result, NULL);
    return result;
  }

  // --------------------------------------------------------------------------
  // [Trim / Simplify]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t trim()
  {
    return fog_api.stringa_trim(this);
  }

  FOG_INLINE err_t simplify()
  {
    return fog_api.stringa_simplify(this);
  }

  FOG_INLINE StringA trimmed() const
  {
    StringA result(*this);
    fog_api.stringa_trim(&result);
    return result;
  }

  FOG_INLINE StringA simplified() const
  {
    StringA result(*this);
    fog_api.stringa_simplify(&result);
    return result;
  }

  // --------------------------------------------------------------------------
  // [Justify]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t justify(size_t n, char ch, uint32_t flags)
  {
    return fog_api.stringa_justify(this, n, ch, flags);
  }

  FOG_INLINE StringA justified(size_t n, char ch, uint32_t flags) const
  {
    StringA result(*this);
    fog_api.stringa_justify(&result, n, ch, flags);
    return result;
  }

  // --------------------------------------------------------------------------
  // [Split]
  // --------------------------------------------------------------------------

  // Implemented-Later.
  FOG_INLINE List<StringA> split(char sep, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY, uint32_t cs = CASE_SENSITIVE);
  FOG_INLINE List<StringA> split(const StringA& sep, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY, uint32_t cs = CASE_SENSITIVE);
  FOG_INLINE List<StringA> split(const RegExpA& re, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY);

  FOG_INLINE List<StringA> split(const Range& range, char sep, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY, uint32_t cs = CASE_SENSITIVE);
  FOG_INLINE List<StringA> split(const Range& range, const StringA& sep, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY, uint32_t cs = CASE_SENSITIVE);
  FOG_INLINE List<StringA> split(const Range& range, const RegExpA& re, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY);

  FOG_INLINE err_t splitTo(List<StringA>& dst, uint32_t cntOp, char sep, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringa_splitChar(&dst, cntOp, this, NULL, sep, splitBehavior, cs);
  }

  FOG_INLINE err_t splitTo(List<StringA>& dst, uint32_t cntOp, const StringA& sep, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringa_splitStringA(&dst, cntOp, this, NULL, &sep, splitBehavior, cs);
  }

  FOG_INLINE err_t splitTo(List<StringA>& dst, uint32_t cntOp, const RegExpA& re, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY)
  {
    return fog_api.stringa_splitRegExpA(&dst, cntOp, this, NULL, &re, splitBehavior);
  }

  FOG_INLINE err_t splitTo(List<StringA>& dst, uint32_t cntOp, const Range& range, char sep, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringa_splitChar(&dst, cntOp, this, &range, sep, splitBehavior, cs);
  }

  FOG_INLINE err_t splitTo(List<StringA>& dst, uint32_t cntOp, const Range& range, const StringA& sep, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringa_splitStringA(&dst, cntOp, this, &range, &sep, splitBehavior, cs);
  }

  FOG_INLINE err_t splitTo(List<StringA>& dst, uint32_t cntOp, const Range& range, const RegExpA& re, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY)
  {
    return fog_api.stringa_splitRegExpA(&dst, cntOp, this, &range, &re, splitBehavior);
  }

  // --------------------------------------------------------------------------
  // [Slice]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t slice(size_t start, size_t end)
  {
    Range range(start, end);
    return fog_api.stringa_slice(this, &range);
  }

  FOG_INLINE err_t slice(const Range& range)
  {
    return fog_api.stringa_slice(this, &range);
  }

  // --------------------------------------------------------------------------
  // [Substring]
  // --------------------------------------------------------------------------

  FOG_INLINE StringA substring(const Range& range) const
  {
    StringA result((StringDataA*)NULL);
    fog_api.stringa_ctorSubstr(&result, this, &range);
    return result;
  }

  // --------------------------------------------------------------------------
  // [ParseBool]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t parseBool(bool* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringa_parseBool(this, dst, base, pEnd, pFlags);
  }

  // --------------------------------------------------------------------------
  // [ParseInt]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t parseI8(int8_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringa_parseI8(this, dst, base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseU8(uint8_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringa_parseU8(this, dst, base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseI16(int16_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringa_parseI16(this, dst, base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseU16(uint16_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringa_parseU16(this, dst, base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseI32(int32_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringa_parseI32(this, dst, base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseU32(uint32_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringa_parseU32(this, dst, base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseI64(int64_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringa_parseI64(this, dst, base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseU64(uint64_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringa_parseU64(this, dst, base, pEnd, pFlags);
  }

  // --------------------------------------------------------------------------
  // [ParseLong]
  // --------------------------------------------------------------------------

#if FOG_SIZEOF_LONG == 32
  FOG_INLINE err_t parseLong(long* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringa_parseI32(this, reinterpret_cast<int32_t*>(dst), base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseULong(ulong* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringa_parseU32(this, reinterpret_cast<uint32_t*>(dst), base, pEnd, pFlags);
  }
#else
  FOG_INLINE err_t parseLong(long* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringa_parseI64(this, reinterpret_cast<int64_t*>(dst), base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseULong(ulong* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringa_parseU64(this, reinterpret_cast<uint64_t*>(dst), base, pEnd, pFlags);
  }
#endif

  // --------------------------------------------------------------------------
  // [ParseSizeT]
  // --------------------------------------------------------------------------

#if FOG_ARCH_BITS == 32
  FOG_INLINE err_t parseSSizeT(ssize_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringa_parseI32(this, reinterpret_cast<int32_t*>(dst), base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseSizeT(size_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringa_parseU32(this, reinterpret_cast<uint32_t*>(dst), base, pEnd, pFlags);
  }
#else
  FOG_INLINE err_t parseSSizeT(ssize_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringa_parseI64(this, reinterpret_cast<int64_t*>(dst), base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseSizeT(size_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringa_parseU64(this, reinterpret_cast<uint64_t*>(dst), base, pEnd, pFlags);
  }
#endif

  // --------------------------------------------------------------------------
  // [ParseReal]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t parseReal(float* dst, char decimalPoint = '.', size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringa_parseFloat(this, dst, decimalPoint, pEnd, pFlags);
  }

  FOG_INLINE err_t parseReal(double* dst, char decimalPoint = '.', size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringa_parseDouble(this, dst, decimalPoint, pEnd, pFlags);
  }

  // --------------------------------------------------------------------------
  // [Contains]
  // --------------------------------------------------------------------------

  FOG_INLINE bool contains(char ch, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_indexOfChar(this, NULL, ch, cs) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const StubA& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_indexOfStubA(this, NULL, &pattern, cs) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const StringA& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_indexOfStringA(this, NULL, &pattern, cs) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const RegExpA& re) const
  {
    return fog_api.stringa_indexOfRegExpA(this, NULL, &re) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const Range& range, char ch, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_indexOfChar(this, &range, ch, cs) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const Range& range, const StubA& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_indexOfStubA(this, &range, &pattern, cs) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const Range& range, const StringA& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_indexOfStringA(this, &range, &pattern, cs) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const Range& range, const RegExpA& re) const
  {
    return fog_api.stringa_indexOfRegExpA(this, &range, &re) != INVALID_INDEX;
  }

  // --------------------------------------------------------------------------
  // [CountOf]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t countOf(char ch, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_countOfChar(this, NULL, ch, cs);
  }

  FOG_INLINE size_t countOf(const StubA& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_countOfStubA(this, NULL, &pattern, cs);
  }

  FOG_INLINE size_t countOf(const StringA& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_countOfStringA(this, NULL, &pattern, cs);
  }

  FOG_INLINE size_t countOf(const RegExpA& re) const
  {
    return fog_api.stringa_countOfRegExpA(this, NULL, &re);
  }

  FOG_INLINE size_t countOf(const Range& range, char ch, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_countOfChar(this, &range, ch, cs);
  }

  FOG_INLINE size_t countOf(const Range& range, const StubA& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_countOfStubA(this, &range, &pattern, cs);
  }

  FOG_INLINE size_t countOf(const Range& range, const StringA& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_countOfStringA(this, &range, &pattern, cs);
  }

  FOG_INLINE size_t countOf(const Range& range, const RegExpA& re) const
  {
    return fog_api.stringa_countOfRegExpA(this, &range, &re);
  }

  // --------------------------------------------------------------------------
  // [IndexOf]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t indexOf(char ch, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_indexOfChar(this, NULL, ch, cs);
  }

  FOG_INLINE size_t indexOf(const StubA& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_indexOfStubA(this, NULL, &pattern, cs);
  }

  FOG_INLINE size_t indexOf(const StringA& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_indexOfStringA(this, NULL, &pattern, cs);
  }

  FOG_INLINE size_t indexOf(const RegExpA& re) const
  {
    return fog_api.stringa_indexOfRegExpA(this, NULL, &re);
  }

  FOG_INLINE size_t indexOf(const Range& range, char ch, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_indexOfChar(this, &range, ch, cs);
  }

  FOG_INLINE size_t indexOf(const Range& range, const StubA& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_indexOfStubA(this, &range, &pattern, cs);
  }

  FOG_INLINE size_t indexOf(const Range& range, const StringA& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_indexOfStringA(this, &range, &pattern, cs);
  }

  FOG_INLINE size_t indexOf(const Range& range, const RegExpA& re) const
  {
    return fog_api.stringa_indexOfRegExpA(this, &range, &re);
  }

  FOG_INLINE size_t indexOfAny(const char* charArray, size_t charLength, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_indexOfAnyCharA(this, NULL, charArray, charLength, cs);
  }

  FOG_INLINE size_t indexOfAny(const Range& range, const char* charArray, size_t charLength, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_indexOfAnyCharA(this, &range, charArray, charLength, cs);
  }

  // --------------------------------------------------------------------------
  // [LastIndexOf]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t lastIndexOf(char ch, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_lastIndexOfChar(this, NULL, ch, cs);
  }

  FOG_INLINE size_t lastIndexOf(const StubA& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_lastIndexOfStubA(this, NULL, &pattern, cs);
  }

  FOG_INLINE size_t lastIndexOf(const StringA& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_lastIndexOfStringA(this, NULL, &pattern, cs);
  }

  FOG_INLINE size_t lastIndexOf(const RegExpA& re) const
  {
    return fog_api.stringa_lastIndexOfRegExpA(this, NULL, &re);
  }

  FOG_INLINE size_t lastIndexOf(const Range& range, char ch, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_lastIndexOfChar(this, &range, ch, cs);
  }

  FOG_INLINE size_t lastIndexOf(const Range& range, const StubA& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_lastIndexOfStubA(this, &range, &pattern, cs);
  }

  FOG_INLINE size_t lastIndexOf(const Range& range, const StringA& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_lastIndexOfStringA(this, &range, &pattern, cs);
  }

  FOG_INLINE size_t lastIndexOf(const Range& range, const RegExpA& re) const
  {
    return fog_api.stringa_lastIndexOfRegExpA(this, &range, &re);
  }

  FOG_INLINE size_t lastIndexOfAny(const char* charArray, size_t charLength, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_lastIndexOfAnyCharA(this, NULL, charArray, charLength, cs);
  }

  FOG_INLINE size_t lastIndexOfAny(const Range& range, const char* charArray, size_t charLength, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_lastIndexOfAnyCharA(this, &range, charArray, charLength, cs);
  }

  // --------------------------------------------------------------------------
  // [StartsWith]
  // --------------------------------------------------------------------------

  FOG_INLINE bool startsWith(char ch, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_startsWithChar(this, ch, cs);
  }

  FOG_INLINE bool startsWith(const char* str, uint32_t cs = CASE_SENSITIVE) const
  {
    StubA stub(str, DETECT_LENGTH);
    return fog_api.stringa_startsWithStubA(this, &stub, cs);
  }

  FOG_INLINE bool startsWith(const StubA& stub, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_startsWithStubA(this, &stub, cs);
  }

  FOG_INLINE bool startsWith(const StringA& other, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_startsWithStringA(this, &other, cs);
  }

  FOG_INLINE bool startsWith(const RegExpA& re) const
  {
    return fog_api.stringa_startsWithRegExpA(this, &re);
  }

  // --------------------------------------------------------------------------
  // [EndsWith]
  // --------------------------------------------------------------------------

  FOG_INLINE bool endsWith(char ch, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_endsWithChar(this, ch, cs);
  }

  FOG_INLINE bool endsWith(const char* str, uint32_t cs = CASE_SENSITIVE) const
  {
    StubA stub(str, DETECT_LENGTH);
    return fog_api.stringa_endsWithStubA(this, &stub, cs);
  }

  FOG_INLINE bool endsWith(const StubA& stub, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_endsWithStubA(this, &stub, cs);
  }

  FOG_INLINE bool endsWith(const StringA& other, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringa_endsWithStringA(this, &other, cs);
  }

  FOG_INLINE bool endsWith(const RegExpA& re) const
  {
    return fog_api.stringa_endsWithRegExpA(this, &re);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const char* str) const
  {
    return eq(StubA(str));
  }

  FOG_INLINE bool eq(const char* str, uint32_t cs) const
  {
    return eq(StubA(str), cs);
  }

  FOG_INLINE bool eq(const StubA& stub) const
  {
    return fog_api.stringa_eqStubA(this, &stub);
  }

  FOG_INLINE bool eq(const StubA& stub, uint32_t cs) const
  {
    return fog_api.stringa_eqStubExA(this, &stub, cs);
  }

  FOG_INLINE bool eq(const StringA& other) const
  {
    return fog_api.stringa_eqStringA(this, &other);
  }

  FOG_INLINE bool eq(const StringA& other, uint32_t cs) const
  {
    return fog_api.stringa_eqStringExA(this, &other, cs);
  }

  // --------------------------------------------------------------------------
  // [Comparison]
  // --------------------------------------------------------------------------

  FOG_INLINE int compare(const char* str) const
  {
    return compare(StubA(str));
  }

  FOG_INLINE int compare(const char* str, uint32_t cs) const
  {
    return compare(StubA(str), cs);
  }

  FOG_INLINE int compare(const StubA& stub) const
  {
    return fog_api.stringa_compareStubA(this, &stub);
  }

  FOG_INLINE int compare(const StubA& stub, uint32_t cs) const
  {
    return fog_api.stringa_compareStubExA(this, &stub, cs);
  }

  FOG_INLINE int compare(const StringA& other) const
  {
    return fog_api.stringa_compareStringA(this, &other);
  }

  FOG_INLINE int compare(const StringA& other, uint32_t cs) const
  {
    return fog_api.stringa_compareStringExA(this, &other, cs);
  }

  // --------------------------------------------------------------------------
  // [Modified]
  // --------------------------------------------------------------------------

  FOG_INLINE void _modified()
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::StringA::_modified() - Not detached.");
    FOG_ASSERT_X(_d->data[_d->length] == 0,
      "Fog::StringA::_modified() - Null terminator corrupted.");

    _d->hashCode = 0;
  }

  FOG_INLINE void _modified(char* end)
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::StringA::_modified() - Not detached.");
    FOG_ASSERT_X(end <= _d->data + _d->capacity,
      "Fog::StringA::_modified() - Buffer overflow.");

    *end = 0;

    _d->hashCode = 0;
    _d->length = (size_t)(end - _d->data);
  }

  // --------------------------------------------------------------------------
  // [RValue]
  // --------------------------------------------------------------------------

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE StringDataA* _acquireData()
  {
    StringDataA* d = _d;
    _d = NULL;
    return d;
  }
#endif // FOG_CC_HAS_RVALUE

  // --------------------------------------------------------------------------
  // [Utf8 Support]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t validateUtf8(size_t* invalid = NULL) const
  {
    return fog_api.stringa_validateUtf8(this, invalid);
  }

  FOG_INLINE err_t getUcsLength(size_t* ucsLength) const
  {
    return fog_api.stringa_getUcsLength(this, ucsLength);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE StringA& operator=(char ch) { setChar(ch); return *this; }
  FOG_INLINE StringA& operator=(const char* str) { set(str); return *this; }
  FOG_INLINE StringA& operator=(const StubA& str) { set(str); return *this; }
  FOG_INLINE StringA& operator=(const StringA& other) { set(other); return *this; }

  FOG_INLINE StringA& operator+=(char ch) { append(ch); return *this; }
  FOG_INLINE StringA& operator+=(const char* str) { append(str); return *this; }
  FOG_INLINE StringA& operator+=(const StubA& str) { append(str); return *this; }
  FOG_INLINE StringA& operator+=(const StringA& other) { append(other); return *this; }

  FOG_INLINE char operator[](size_t index) const
  {
    FOG_ASSERT_X(index < getLength(),
      "Fog::StringA::operator[] - Index out of range.");

    return _d->data[index];
  }

  // --------------------------------------------------------------------------
  // [Statics - From]
  // --------------------------------------------------------------------------

  static FOG_INLINE StringA fromData(const char* data, size_t length = DETECT_LENGTH) { return StringA(StubA(data, length)); }
  static FOG_INLINE StringA fromData(const StubA& data) { return StringA(data); }

  static FOG_INLINE StringA fromOther(const StringA& s) { return StringA(s); }
  static FOG_INLINE StringA fromOther(const StringA& s, const Range& range) { return StringA(s, range); }

  static FOG_INLINE StringA fromTwo(const char* str1, const char* str2) { return StringA(str1, str2); }
  static FOG_INLINE StringA fromTwo(const StubA& str1, const StubA& str2) { return StringA(str1, str2); }
  static FOG_INLINE StringA fromTwo(const StringA& str1, const StringA& str2) { return StringA(str1, str2); }

  static FOG_INLINE StringA fromBool(bool b) { StringA t; t.setBool(b); return t; }

  static FOG_INLINE StringA fromInt(int8_t n) { StringA t(UNINITIALIZED); fog_api.stringa_ctorU32(&t, (uint8_t)n, false); return t; }
  static FOG_INLINE StringA fromInt(uint8_t n) { StringA t(UNINITIALIZED); fog_api.stringa_ctorU32(&t, (uint8_t)n, true); return t; }
  static FOG_INLINE StringA fromInt(int16_t n) { StringA t(UNINITIALIZED); fog_api.stringa_ctorU32(&t, (uint16_t)n, false); return t; }
  static FOG_INLINE StringA fromInt(uint16_t n) { StringA t(UNINITIALIZED); fog_api.stringa_ctorU32(&t, (uint16_t)n, true); return t; }
  static FOG_INLINE StringA fromInt(int32_t n) { StringA t(UNINITIALIZED); fog_api.stringa_ctorU32(&t, n, false); return t; }
  static FOG_INLINE StringA fromInt(uint32_t n) { StringA t(UNINITIALIZED); fog_api.stringa_ctorU32(&t, n, true); return t; }
  static FOG_INLINE StringA fromInt(int64_t n) { StringA t(UNINITIALIZED); fog_api.stringa_ctorU64(&t, n, false); return t; }
  static FOG_INLINE StringA fromInt(uint64_t n) { StringA t(UNINITIALIZED); fog_api.stringa_ctorU64(&t, n, true); return t; }

  static FOG_INLINE StringA fromReal(float d) { StringA t(UNINITIALIZED); fog_api.stringa_ctorDouble(&t, d); return t; }
  static FOG_INLINE StringA fromReal(double d) { StringA t(UNINITIALIZED); fog_api.stringa_ctorDouble(&t, d); return t; }

  // --------------------------------------------------------------------------
  // [Statics - Join]
  // --------------------------------------------------------------------------

  static FOG_INLINE StringA join(const List<StringA>& list, char separator)
  {
    StringA result;
    ListUntypedData* list_d = reinterpret_cast<const ListUntyped*>(&list)->_d;

    fog_api.stringa_joinChar(&result,
      reinterpret_cast<const StringA*>(list_d->data), list_d->length,
      separator);

    return result;
  }

  static FOG_INLINE StringA join(const List<StringA>& list, const StringA& separator)
  {
    StringA result;
    ListUntypedData* list_d = reinterpret_cast<const ListUntyped*>(&list)->_d;

    fog_api.stringa_joinStringA(&result,
      reinterpret_cast<const StringA*>(list_d->data), list_d->length,
      &separator);

    return result;
  }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const StringA* a, const StringA* b)
  {
    return fog_api.stringa_eqStringA(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.stringa_eqStringA;
  }

  // --------------------------------------------------------------------------
  // [Statics - Compare]
  // --------------------------------------------------------------------------

  static FOG_INLINE int compare(const StringA* a, const StringA* b)
  {
    return fog_api.stringa_compareStringA(a, b);
  }

  static FOG_INLINE CompareFunc getCompareFunc()
  {
    return (CompareFunc)fog_api.stringa_compareStringA;
  }

  // --------------------------------------------------------------------------
  // [Statics - Hex]
  // --------------------------------------------------------------------------

  static FOG_INLINE err_t hexDecode(StringA& dst, uint32_t cntOp, const StringA& src)
  {
    return fog_api.stringa_hexDecode(&dst, cntOp, &src);
  }

  static FOG_INLINE err_t hexEncode(StringA& dst, uint32_t cntOp, const StringA& src, uint32_t textCase = TEXT_CASE_UPPER)
  {
    return fog_api.stringa_hexEncode(&dst, cntOp, &src, textCase);
  }

  // --------------------------------------------------------------------------
  // [Statics - Base64]
  // --------------------------------------------------------------------------

  static FOG_INLINE err_t base64Decode(StringA& dst, uint32_t cntOp, const StringA& src)
  {
    return fog_api.stringa_base64DecodeStringA(&dst, cntOp, &src);
  }

  static FOG_INLINE err_t base64Decode(StringA& dst, uint32_t cntOp, const StringW& src)
  {
    return fog_api.stringa_base64DecodeStringW(&dst, cntOp, &src);
  }

  static FOG_INLINE err_t base64Decode(StringA& dst, uint32_t cntOp, const char* src, size_t srcLength)
  {
    return fog_api.stringa_base64DecodeDataA(&dst, cntOp, src, srcLength);
  }

  static FOG_INLINE err_t base64Decode(StringA& dst, uint32_t cntOp, const CharW* src, size_t srcLength)
  {
    return fog_api.stringa_base64DecodeDataW(&dst, cntOp, src, srcLength);
  }

  static FOG_INLINE err_t base64Encode(StringA& dst, uint32_t cntOp, const StringA& src)
  {
    return fog_api.stringa_base64EncodeStringA(&dst, cntOp, &src);
  }

  static FOG_INLINE err_t base64Encode(StringA& dst, uint32_t cntOp, const char* src, size_t srcLength)
  {
    return fog_api.stringa_base64EncodeDataA(&dst, cntOp, src, srcLength);
  }

  // --------------------------------------------------------------------------
  // [Statics - StringDataA]
  // --------------------------------------------------------------------------

  static FOG_INLINE StringDataA* _dCreate(size_t capacity)
  {
    return fog_api.stringa_dCreate(capacity);
  }

  static FOG_INLINE StringDataA* _dCreate(size_t capacity, const StubA& stub)
  {
    return fog_api.stringa_dCreateStubA(capacity, &stub);
  }

  static FOG_INLINE StringDataA* _dAdopt(void* address, size_t capacity)
  {
    return fog_api.stringa_dAdopt(address, capacity);
  }

  static FOG_INLINE StringDataA* _dAdopt(void* address, size_t capacity, const StubA& stub)
  {
    return fog_api.stringa_dAdoptStubA(address, capacity, &stub);
  }

  static FOG_INLINE StringDataA* _dRealloc(StringDataA* d, size_t capacity)
  {
    return fog_api.stringa_dRealloc(d, capacity);
  }

  static FOG_INLINE void _dFree(StringDataA* d)
  {
    fog_api.stringa_dFree(d);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(StringDataA)
};

// ============================================================================
// [Fog::StringW]
// ============================================================================

//! @brief String (unicode).
struct FOG_NO_EXPORT StringW
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE StringW()
  {
    fog_api.stringw_ctor(this);
  }

  FOG_INLINE StringW(const StringW& other)
  {
    fog_api.stringw_ctorCopyW(this, &other);
  }

  explicit FOG_INLINE StringW(const Ascii8& stub)
  {
    fog_api.stringw_ctorStubA(this, &stub);
  }

  explicit FOG_INLINE StringW(const Local8& stub)
  {
    fog_api.stringw_ctorCodec(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_LOCAL]);
  }

  explicit FOG_INLINE StringW(const Utf8& stub)
  {
    fog_api.stringw_ctorCodec(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_UTF8]);
  }

  explicit FOG_INLINE StringW(const StubW& stub)
  {
    fog_api.stringw_ctorStubW(this, &stub);
  }

  explicit FOG_INLINE StringW(const CharW* str, size_t length = DETECT_LENGTH)
  {
    StubW stub(str, length);
    fog_api.stringw_ctorStubW(this, &stub);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE StringW(StringW&& other) : _d(other._d) { other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  FOG_INLINE StringW(const StringW& other, const Range& range)
  {
    fog_api.stringw_ctorSubstr(this, &other, &range);
  }

  FOG_INLINE StringW(const StringW& a, const StringW& b)
  {
    fog_api.stringw_ctorStringWStringW(this, &a, &b);
  }

  FOG_INLINE StringW(const StringW& a, const StubW& b)
  {
    fog_api.stringw_ctorStringWStubW(this, &a, &b);
  }

  FOG_INLINE StringW(const StringW& a, const Ascii8& b)
  {
    fog_api.stringw_ctorStringWStubA(this, &a, &b);
  }

  FOG_INLINE StringW(const StubW& a, const StringW& b)
  {
    fog_api.stringw_ctorStubWStringW(this, &a, &b);
  }

  FOG_INLINE StringW(const StubW& a, const StubW& b)
  {
    fog_api.stringw_ctorStubWStubW(this, &a, &b);
  }

  FOG_INLINE StringW(const CharW* a, const CharW* b)
  {
    StubW sa(a);
    StubW sb(b);

    fog_api.stringw_ctorStubWStubW(this, &sa, &sb);
  }

  FOG_INLINE StringW(const Ascii8& a, const StringW& b)
  {
    fog_api.stringw_ctorStubAStringW(this, &a, &b);
  }

  FOG_INLINE StringW(const Ascii8& a, const Ascii8& b)
  {
    fog_api.stringw_ctorStubAStubA(this, &a, &b);
  }

  explicit FOG_INLINE StringW(_Uninitialized)
  {
  }

  explicit FOG_INLINE StringW(StringDataW* d) : _d(d)
  {
  }

  FOG_INLINE ~StringW()
  {
    fog_api.stringw_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getReference().
  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return getReference() == 1; }

  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach()
  {
    return isDetached() ? (err_t)ERR_OK : fog_api.stringw_detach(this);
  }

  FOG_INLINE err_t _detach()
  {
    return fog_api.stringw_detach(this);
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  //! @brief Get string capacity.
  FOG_INLINE size_t getCapacity() const { return _d->capacity; }
  //! @brief Get string length.
  FOG_INLINE size_t getLength() const { return _d->length; }
  //! @brief Get whether the string is empty.
  FOG_INLINE bool isEmpty() const { return _d->length == 0; }

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return fog_api.stringw_reserve(this, capacity);
  }

  FOG_INLINE err_t resize(size_t length)
  {
    return fog_api.stringw_resize(this, length);
  }

  FOG_INLINE err_t truncate(size_t length)
  {
    return fog_api.stringw_truncate(this, length);
  }

  FOG_INLINE void squeeze()
  {
    fog_api.stringw_squeeze(this);
  }

  FOG_INLINE CharW* _prepare(uint32_t cntOp, size_t length)
  {
    return fog_api.stringw_prepare(this, cntOp, length);
  }

  FOG_INLINE CharW* _add(size_t length)
  {
    return fog_api.stringw_add(this, length);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get a const pointer to the string data.
  FOG_INLINE const CharW* getData() const
  {
    return _d->data;
  }

  FOG_INLINE CharW* getDataX()
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::StringW::getDataX() - Not detached.");

    return _d->data;
  }

  FOG_INLINE CharW getAt(size_t index) const
  {
    FOG_ASSERT_X(index < getLength(),
      "Fog::StringW::getAt() - Index out of range.");

    return _d->data[index];
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    return fog_api.stringw_clear(this);
  }

  FOG_INLINE void reset()
  {
    return fog_api.stringw_reset(this);
  }

  // --------------------------------------------------------------------------
  // [HashCode]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getHashCode() const
  {
    return fog_api.stringw_getHashCode(this);
  }

  // --------------------------------------------------------------------------
  // [SetChar / SetChars]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setChar(CharW ch)
  {
    return fog_api.stringw_opFill(this, CONTAINER_OP_REPLACE, ch, 1);
  }

  FOG_INLINE err_t setChars(CharW ch, size_t length)
  {
    return fog_api.stringw_opFill(this, CONTAINER_OP_REPLACE, ch, length);
  }

  // --------------------------------------------------------------------------
  // [SetString]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t _set(const StubA& stub) { return fog_api.stringw_setStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_ASCII]); }
  FOG_INLINE err_t _set(const StringA& other) { return fog_api.stringw_setStringA(this, &other, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_ASCII]); }
  FOG_INLINE err_t _set(const StubW& stub) { return fog_api.stringw_setStubW(this, &stub); }
  FOG_INLINE err_t _set(const StringW& other) { return fog_api.stringw_setStringW(this, &other); }

  FOG_INLINE err_t set(const Ascii8& stub)
  {
    return fog_api.stringw_setStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_ASCII]);
  }

  FOG_INLINE err_t set(const Local8& stub)
  {
    return fog_api.stringw_setStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_LOCAL]);
  }

  FOG_INLINE err_t set(const Utf8& stub)
  {
    return fog_api.stringw_setStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_UTF8]);
  }

  FOG_INLINE err_t set(const StubA& stub, const TextCodec& tc)
  {
    return fog_api.stringw_setStubA(this, &stub, &tc);
  }

  FOG_INLINE err_t set(const StubW& stub)
  {
    return fog_api.stringw_setStubW(this, &stub);
  }

  FOG_INLINE err_t set(const StringA& other, const TextCodec& tc)
  {
    return fog_api.stringw_setStringA(this, &other, &tc);
  }

  FOG_INLINE err_t set(const CharW* str, size_t length = DETECT_LENGTH)
  {
    StubW stubW(str, length);
    return fog_api.stringw_setStubW(this, &stubW);
  }

  FOG_INLINE err_t set(const StringW& other)
  {
    return fog_api.stringw_setStringW(this, &other);
  }

  FOG_INLINE err_t set(const StringW& other, const Range& range)
  {
    return fog_api.stringw_setStringExW(this, &other, &range);
  }

  FOG_INLINE err_t setAscii8(const char* str, size_t length = DETECT_LENGTH)
  {
    StubA stub(str, length);
    return fog_api.stringw_setStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_ASCII]);
  }

  FOG_INLINE err_t setAscii8(const StubA& stub)
  {
    return fog_api.stringw_setStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_ASCII]);
  }

  FOG_INLINE err_t setLocal8(const char* str, size_t length = DETECT_LENGTH)
  {
    StubA stub(str, length);
    return fog_api.stringw_setStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_LOCAL]);
  }

  FOG_INLINE err_t setLocal8(const StubA& stub)
  {
    return fog_api.stringw_setStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_LOCAL]);
  }

  FOG_INLINE err_t setUtf8(const char* str, size_t length = DETECT_LENGTH)
  {
    StubA stub(str, length);
    return fog_api.stringw_setStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_UTF8]);
  }

  FOG_INLINE err_t setUtf8(const StubA& stub)
  {
    return fog_api.stringw_setStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_UTF8]);
  }

  FOG_INLINE err_t setUtf16(const uint16_t* str, size_t length = DETECT_LENGTH)
  {
    StubW stub(str, length);
    return fog_api.stringw_setStubW(this, &stub);
  }

  FOG_INLINE err_t setUtf16(const StubW& stub)
  {
    return fog_api.stringw_setStubW(this, &stub);
  }

  FOG_INLINE err_t setUtf32(const uint32_t* str, size_t length = DETECT_LENGTH)
  {
    StubA stub(reinterpret_cast<const char*>(str), length);
    return fog_api.stringw_setStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_UTF32]);
  }

  FOG_INLINE err_t setWChar(const wchar_t* str, size_t length = DETECT_LENGTH)
  {
    if (sizeof(wchar_t) == 2)
      return setUtf16(reinterpret_cast<const uint16_t*>(str), length);
    else
      return setUtf32(reinterpret_cast<const uint32_t*>(str), length);
  }

  FOG_INLINE err_t setDeep(const StringW& other)
  {
    return fog_api.stringw_setDeep(this, &other);
  }

  FOG_INLINE err_t setAndNormalizeSlashes(const StringW& other, uint32_t slashForm)
  {
    return fog_api.stringw_opNormalizeSlashesW(this, CONTAINER_OP_REPLACE, &other, NULL, slashForm);
  }

  FOG_INLINE err_t setAndNormalizeSlashes(const StringW& other, const Range& range, uint32_t slashForm)
  {
    return fog_api.stringw_opNormalizeSlashesW(this, CONTAINER_OP_REPLACE, &other, &range, slashForm);
  }

  // --------------------------------------------------------------------------
  // [SetBool]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setBool(bool b) { return fog_api.stringw_opBool(this, CONTAINER_OP_REPLACE, b); }

  // --------------------------------------------------------------------------
  // [SetInt]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setInt(int8_t n) { return fog_api.stringw_opI32(this, CONTAINER_OP_REPLACE, n); }
  FOG_INLINE err_t setInt(uint8_t n) { return fog_api.stringw_opU32(this, CONTAINER_OP_REPLACE, n); }
  FOG_INLINE err_t setInt(int16_t n) { return fog_api.stringw_opI32(this, CONTAINER_OP_REPLACE, n); }
  FOG_INLINE err_t setInt(uint16_t n) { return fog_api.stringw_opU32(this, CONTAINER_OP_REPLACE, n); }
  FOG_INLINE err_t setInt(int32_t n) { return fog_api.stringw_opI32(this, CONTAINER_OP_REPLACE, n); }
  FOG_INLINE err_t setInt(uint32_t n) { return fog_api.stringw_opU32(this, CONTAINER_OP_REPLACE, n); }
  FOG_INLINE err_t setInt(int64_t n) { return fog_api.stringw_opI64(this, CONTAINER_OP_REPLACE, n); }
  FOG_INLINE err_t setInt(uint64_t n) { return fog_api.stringw_opU64(this, CONTAINER_OP_REPLACE, n); }

  FOG_INLINE err_t setInt(int8_t n, const FormatInt& fmt) { return fog_api.stringw_opI32Ex(this, CONTAINER_OP_REPLACE, n, &fmt, NULL); }
  FOG_INLINE err_t setInt(uint8_t n, const FormatInt& fmt) { return fog_api.stringw_opU32Ex(this, CONTAINER_OP_REPLACE, n, &fmt, NULL); }
  FOG_INLINE err_t setInt(int16_t n, const FormatInt& fmt) { return fog_api.stringw_opI32Ex(this, CONTAINER_OP_REPLACE, n, &fmt, NULL); }
  FOG_INLINE err_t setInt(uint16_t n, const FormatInt& fmt) { return fog_api.stringw_opU32Ex(this, CONTAINER_OP_REPLACE, n, &fmt, NULL); }
  FOG_INLINE err_t setInt(int32_t n, const FormatInt& fmt) { return fog_api.stringw_opI32Ex(this, CONTAINER_OP_REPLACE, n, &fmt, NULL); }
  FOG_INLINE err_t setInt(uint32_t n, const FormatInt& fmt) { return fog_api.stringw_opU32Ex(this, CONTAINER_OP_REPLACE, n, &fmt, NULL); }
  FOG_INLINE err_t setInt(int64_t n, const FormatInt& fmt) { return fog_api.stringw_opI64Ex(this, CONTAINER_OP_REPLACE, n, &fmt, NULL); }
  FOG_INLINE err_t setInt(uint64_t n, const FormatInt& fmt) { return fog_api.stringw_opU64Ex(this, CONTAINER_OP_REPLACE, n, &fmt, NULL); }

  // --------------------------------------------------------------------------
  // [SetReal]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setReal(float d)
  {
    return fog_api.stringw_opDouble(this, CONTAINER_OP_REPLACE, d);
  }

  FOG_INLINE err_t setReal(float d, const FormatReal& fmt)
  {
    return fog_api.stringw_opDoubleEx(this, CONTAINER_OP_REPLACE, d, &fmt, NULL);
  }

  FOG_INLINE err_t setReal(double d)
  {
    return fog_api.stringw_opDouble(this, CONTAINER_OP_REPLACE, d);
  }

  FOG_INLINE err_t setReal(double d, const FormatReal& fmt)
  {
    return fog_api.stringw_opDoubleEx(this, CONTAINER_OP_REPLACE, d, &fmt, NULL);
  }

  // --------------------------------------------------------------------------
  // [Format]
  // --------------------------------------------------------------------------

  FOG_NO_INLINE err_t format(const char* fmt, ...)
  {
    StubA fmtStub(fmt, DETECT_LENGTH);

    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringw_opVFormatStubA(this, CONTAINER_OP_REPLACE, &fmtStub, NULL, NULL, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t format_c(const char* fmt, const TextCodec& tc, ...)
  {
    StubA fmtStub(fmt, DETECT_LENGTH);

    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringw_opVFormatStubA(this, CONTAINER_OP_REPLACE, &fmtStub, &tc, NULL, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t format(const Ascii8& fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringw_opVFormatStubA(this, CONTAINER_OP_REPLACE, &fmt, NULL, NULL, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t format_c(const StubA& fmt, const TextCodec& tc, ...)
  {
    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringw_opVFormatStubA(this, CONTAINER_OP_REPLACE, &fmt, &tc, NULL, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t format(const StubW& fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringw_opVFormatStubW(this, CONTAINER_OP_REPLACE, &fmt, NULL, NULL, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t format_c(const StubW& fmt, const TextCodec& tc, ...)
  {
    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringw_opVFormatStubW(this, CONTAINER_OP_REPLACE, &fmt, &tc, NULL, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t format(const StringW& fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringw_opVFormatStringW(this, CONTAINER_OP_REPLACE, &fmt, NULL, NULL, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t format_c(const StringW& fmt, const TextCodec& tc, ...)
  {
    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringw_opVFormatStringW(this, CONTAINER_OP_REPLACE, &fmt, &tc, NULL, ap);
    va_end(ap);

    return err;
  }

  // --------------------------------------------------------------------------
  // [VFormat]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t vFormat(const char* fmt, va_list ap)
  {
    StubA fmtA(fmt, DETECT_LENGTH);
    return fog_api.stringw_opVFormatStubA(this, CONTAINER_OP_REPLACE, &fmtA, NULL, NULL, ap);
  }

  FOG_INLINE err_t vFormat_c(const char* fmt, const TextCodec& tc, va_list ap)
  {
    StubA fmtA(fmt, DETECT_LENGTH);
    return fog_api.stringw_opVFormatStubA(this, CONTAINER_OP_REPLACE, &fmtA, &tc, NULL, ap);
  }

  FOG_INLINE err_t vFormat(const Ascii8& fmt, va_list ap)
  {
    return fog_api.stringw_opVFormatStubA(this, CONTAINER_OP_REPLACE, &fmt, NULL, NULL, ap);
  }

  FOG_INLINE err_t vFormat_c(const StubA& fmt, const TextCodec& tc, va_list ap)
  {
    return fog_api.stringw_opVFormatStubA(this, CONTAINER_OP_REPLACE, &fmt, &tc, NULL, ap);
  }

  FOG_INLINE err_t vFormat(const StringW& fmt, va_list ap)
  {
    return fog_api.stringw_opVFormatStringW(this, CONTAINER_OP_REPLACE, &fmt, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_ASCII], NULL, ap);
  }

  FOG_INLINE err_t vFormat_c(const StringW& fmt, const TextCodec& tc, va_list ap)
  {
    return fog_api.stringw_opVFormatStringW(this, CONTAINER_OP_REPLACE, &fmt, &tc, NULL, ap);
  }

  // --------------------------------------------------------------------------
  // [ZFormat]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t zFormat(const CharW* fmt, CharW lex, const List<StringW>& args)
  {
    StubW fmtW(fmt, DETECT_LENGTH);
    ListUntypedData* args_d = reinterpret_cast<const ListUntyped*>(&args)->_d;

    return fog_api.stringw_opZFormatStubW(this, CONTAINER_OP_REPLACE, &fmtW, lex._value,
      reinterpret_cast<const StringW*>(args_d->data), args_d->length);
  }

  FOG_INLINE err_t zFormat(const CharW* fmt, CharW lex, const StringW* args, size_t argsLength)
  {
    StubW fmtW(fmt, DETECT_LENGTH);
    return fog_api.stringw_opZFormatStubW(this, CONTAINER_OP_REPLACE, &fmtW, lex._value,
      args, argsLength);
  }

  FOG_INLINE err_t zFormat(const StubW& fmt, CharW lex, const List<StringW>& args)
  {
    ListUntypedData* args_d = reinterpret_cast<const ListUntyped*>(&args)->_d;

    return fog_api.stringw_opZFormatStubW(this, CONTAINER_OP_REPLACE, &fmt, lex._value,
      reinterpret_cast<const StringW*>(args_d->data), args_d->length);
  }

  FOG_INLINE err_t zFormat(const StubW& fmt, CharW lex, const StringW* args, size_t argsLength)
  {
    return fog_api.stringw_opZFormatStubW(this, CONTAINER_OP_REPLACE, &fmt, lex._value,
      args, argsLength);
  }

  FOG_INLINE err_t zFormat(const StringW& fmt, CharW lex, const List<StringW>& args)
  {
    ListUntypedData* args_d = reinterpret_cast<const ListUntyped*>(&args)->_d;

    return fog_api.stringw_opZFormatStringW(this, CONTAINER_OP_REPLACE, &fmt, lex._value,
      reinterpret_cast<const StringW*>(args_d->data), args_d->length);
  }

  FOG_INLINE err_t zFormat(const StringW& fmt, CharW lex, const StringW* args, size_t argsLength)
  {
    return fog_api.stringw_opZFormatStringW(this, CONTAINER_OP_REPLACE, &fmt, lex._value,
      args, argsLength);
  }

  // --------------------------------------------------------------------------
  // [NormalizeSlashes]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t normalizeSlashes(uint32_t slashForm)
  {
    return fog_api.stringw_opNormalizeSlashesW(this, CONTAINER_OP_REPLACE, this, NULL, slashForm);
  }

  // --------------------------------------------------------------------------
  // [Append]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t _append(const StubA& stub)
  {
    return fog_api.stringw_appendStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_ASCII]);
  }

  FOG_INLINE err_t _append(const StubW& stub)
  {
    return fog_api.stringw_appendStubW(this, &stub);
  }

  FOG_INLINE err_t _append(const StringA& other)
  {
    return fog_api.stringw_appendStringA(this, &other, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_ASCII]);
  }

  FOG_INLINE err_t _append(const StringW& other)
  {
    return fog_api.stringw_appendStringW(this, &other);
  }

  FOG_INLINE err_t append(CharW ch)
  {
    return fog_api.stringw_opFill(this, CONTAINER_OP_APPEND, ch._value, 1);
  }

  FOG_INLINE err_t append(CharW ch, size_t length)
  {
    return fog_api.stringw_opFill(this, CONTAINER_OP_APPEND, ch._value, length);
  }

  FOG_INLINE err_t append(const CharW* str, size_t length = DETECT_LENGTH)
  {
    StubW stub(str, length);
    return fog_api.stringw_appendStubW(this, &stub);
  }

  FOG_INLINE err_t append(const Ascii8& stub)
  {
    return fog_api.stringw_appendStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_ASCII]);
  }

  FOG_INLINE err_t append(const Local8& stub)
  {
    return fog_api.stringw_appendStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_LOCAL]);
  }

  FOG_INLINE err_t append(const Utf8& stub)
  {
    return fog_api.stringw_appendStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_UTF8]);
  }

  FOG_INLINE err_t append(const StubA& stub, const TextCodec& tc)
  {
    return fog_api.stringw_appendStubA(this, &stub, &tc);
  }

  FOG_INLINE err_t append(const StubW& stub)
  {
    return fog_api.stringw_appendStubW(this, &stub);
  }

  FOG_INLINE err_t append(const StringA& other, const TextCodec& tc)
  {
    return fog_api.stringw_appendStringA(this, &other, &tc);
  }

  FOG_INLINE err_t append(const StringW& other)
  {
    return fog_api.stringw_appendStringW(this, &other);
  }

  FOG_INLINE err_t appendAscii8(const char* str, size_t length = DETECT_LENGTH)
  {
    StubA stub(str, length);
    return fog_api.stringw_appendStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_ASCII]);
  }

  FOG_INLINE err_t appendAscii8(const StubA& stub)
  {
    return fog_api.stringw_appendStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_ASCII]);
  }

  FOG_INLINE err_t appendLocal8(const char* str, size_t length = DETECT_LENGTH)
  {
    StubA stub(str, length);
    return fog_api.stringw_appendStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_LOCAL]);
  }

  FOG_INLINE err_t appendLocal8(const StubA& stub)
  {
    return fog_api.stringw_appendStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_LOCAL]);
  }

  FOG_INLINE err_t appendUtf8(const char* str, size_t length = DETECT_LENGTH)
  {
    StubA stub(str, length);
    return fog_api.stringw_appendStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_UTF8]);
  }

  FOG_INLINE err_t appendUtf8(const StubA& stub)
  {
    return fog_api.stringw_appendStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_UTF8]);
  }

  FOG_INLINE err_t appendUtf16(const uint16_t* str, size_t length = DETECT_LENGTH)
  {
    StubW stub(str, length);
    return fog_api.stringw_appendStubW(this, &stub);
  }

  FOG_INLINE err_t appendUtf16(const StubW& stub)
  {
    return fog_api.stringw_appendStubW(this, &stub);
  }

  FOG_INLINE err_t appendUtf32(const uint32_t* str, size_t length = DETECT_LENGTH)
  {
    StubA stub(reinterpret_cast<const char*>(str), length);
    return fog_api.stringw_appendStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_UTF32]);
  }

  FOG_INLINE err_t appendWChar(const wchar_t* str, size_t length = DETECT_LENGTH)
  {
    if (sizeof(wchar_t) == 2)
      return appendUtf16(reinterpret_cast<const uint16_t*>(str), length);
    else
      return appendUtf32(reinterpret_cast<const uint32_t*>(str), length);
  }

  FOG_INLINE err_t appendAndNormalizeSlashes(const StringW& other, uint32_t slashForm)
  {
    return fog_api.stringw_opNormalizeSlashesW(this, CONTAINER_OP_APPEND, &other, NULL, slashForm);
  }

  FOG_INLINE err_t appendAndNormalizeSlashes(const StringW& other, const Range& range, uint32_t slashForm)
  {
    return fog_api.stringw_opNormalizeSlashesW(this, CONTAINER_OP_APPEND, &other, &range, slashForm);
  }

  // --------------------------------------------------------------------------
  // [AppendBool]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t appendBool(bool b)
  {
    return fog_api.stringw_opBool(this, CONTAINER_OP_APPEND, b);
  }

  // --------------------------------------------------------------------------
  // [AppendInt]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t appendInt(int8_t n) { return fog_api.stringw_opI32(this, CONTAINER_OP_APPEND, n); }
  FOG_INLINE err_t appendInt(uint8_t n) { return fog_api.stringw_opU32(this, CONTAINER_OP_APPEND, n); }
  FOG_INLINE err_t appendInt(int16_t n) { return fog_api.stringw_opI32(this, CONTAINER_OP_APPEND, n); }
  FOG_INLINE err_t appendInt(uint16_t n) { return fog_api.stringw_opU32(this, CONTAINER_OP_APPEND, n); }
  FOG_INLINE err_t appendInt(int32_t n) { return fog_api.stringw_opI32(this, CONTAINER_OP_APPEND, n); }
  FOG_INLINE err_t appendInt(uint32_t n) { return fog_api.stringw_opU32(this, CONTAINER_OP_APPEND, n); }
  FOG_INLINE err_t appendInt(int64_t n) { return fog_api.stringw_opI64(this, CONTAINER_OP_APPEND, n); }
  FOG_INLINE err_t appendInt(uint64_t n) { return fog_api.stringw_opU64(this, CONTAINER_OP_APPEND, n); }

  FOG_INLINE err_t appendInt(int8_t n, const FormatInt& fmt) { return fog_api.stringw_opI32Ex(this, CONTAINER_OP_APPEND, n, &fmt, NULL); }
  FOG_INLINE err_t appendInt(uint8_t n, const FormatInt& fmt) { return fog_api.stringw_opU32Ex(this, CONTAINER_OP_APPEND, n, &fmt, NULL); }
  FOG_INLINE err_t appendInt(int16_t n, const FormatInt& fmt) { return fog_api.stringw_opI32Ex(this, CONTAINER_OP_APPEND, n, &fmt, NULL); }
  FOG_INLINE err_t appendInt(uint16_t n, const FormatInt& fmt) { return fog_api.stringw_opU32Ex(this, CONTAINER_OP_APPEND, n, &fmt, NULL); }
  FOG_INLINE err_t appendInt(int32_t n, const FormatInt& fmt) { return fog_api.stringw_opI32Ex(this, CONTAINER_OP_APPEND, n, &fmt, NULL); }
  FOG_INLINE err_t appendInt(uint32_t n, const FormatInt& fmt) { return fog_api.stringw_opU32Ex(this, CONTAINER_OP_APPEND, n, &fmt, NULL); }
  FOG_INLINE err_t appendInt(int64_t n, const FormatInt& fmt) { return fog_api.stringw_opI64Ex(this, CONTAINER_OP_APPEND, n, &fmt, NULL); }
  FOG_INLINE err_t appendInt(uint64_t n, const FormatInt& fmt) { return fog_api.stringw_opU64Ex(this, CONTAINER_OP_APPEND, n, &fmt, NULL); }

  // --------------------------------------------------------------------------
  // [AppendReal]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t appendReal(float d)
  {
    return fog_api.stringw_opDouble(this, CONTAINER_OP_APPEND, d);
  }

  FOG_INLINE err_t appendReal(float d, const FormatReal& fmt)
  {
    return fog_api.stringw_opDoubleEx(this, CONTAINER_OP_APPEND, d, &fmt, NULL);
  }

  FOG_INLINE err_t appendReal(double d)
  {
    return fog_api.stringw_opDouble(this, CONTAINER_OP_APPEND, d);
  }

  FOG_INLINE err_t appendReal(double d, const FormatReal& fmt)
  {
    return fog_api.stringw_opDoubleEx(this, CONTAINER_OP_APPEND, d, &fmt, NULL);
  }

  // --------------------------------------------------------------------------
  // [AppendFormat]
  // --------------------------------------------------------------------------

  FOG_NO_INLINE err_t appendFormat(const char* fmt, ...)
  {
    StubA fmtStub(fmt, DETECT_LENGTH);

    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringw_opVFormatStubA(this, CONTAINER_OP_APPEND, &fmtStub, NULL, NULL, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t appendFormat_c(const char* fmt, const TextCodec& tc, ...)
  {
    StubA fmtStub(fmt, DETECT_LENGTH);

    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringw_opVFormatStubA(this, CONTAINER_OP_APPEND, &fmtStub, &tc, NULL, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t appendFormat(const Ascii8& fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringw_opVFormatStubA(this, CONTAINER_OP_APPEND, &fmt, NULL, NULL, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t appendFormat_c(const StubA& fmt, const TextCodec& tc, ...)
  {
    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringw_opVFormatStubA(this, CONTAINER_OP_APPEND, &fmt, &tc, NULL, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t appendFormat(const StubW& fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringw_opVFormatStubW(this, CONTAINER_OP_APPEND, &fmt, NULL, NULL, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t appendFormat_c(const StubW& fmt, const TextCodec& tc, ...)
  {
    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringw_opVFormatStubW(this, CONTAINER_OP_APPEND, &fmt, &tc, NULL, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t appendFormat(const StringW& fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringw_opVFormatStringW(this, CONTAINER_OP_APPEND, &fmt, NULL, NULL, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t appendFormat_c(const StringW& fmt, const TextCodec& tc, ...)
  {
    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringw_opVFormatStringW(this, CONTAINER_OP_APPEND, &fmt, &tc, NULL, ap);
    va_end(ap);

    return err;
  }

  // --------------------------------------------------------------------------
  // [AppendVFormat]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t appendVFormat(const char* fmt, va_list ap)
  {
    StubA fmtA(fmt, DETECT_LENGTH);
    return fog_api.stringw_opVFormatStubA(this, CONTAINER_OP_APPEND, &fmtA, NULL, NULL, ap);
  }

  FOG_INLINE err_t appendVFormat_c(const char* fmt, const TextCodec& tc, va_list ap)
  {
    StubA fmtA(fmt, DETECT_LENGTH);
    return fog_api.stringw_opVFormatStubA(this, CONTAINER_OP_APPEND, &fmtA, &tc, NULL, ap);
  }

  FOG_INLINE err_t appendVFormat(const Ascii8& fmt, va_list ap)
  {
    return fog_api.stringw_opVFormatStubA(this, CONTAINER_OP_APPEND, &fmt, NULL, NULL, ap);
  }

  FOG_INLINE err_t appendVFormat_c(const StubA& fmt, const TextCodec& tc, va_list ap)
  {
    return fog_api.stringw_opVFormatStubA(this, CONTAINER_OP_APPEND, &fmt, &tc, NULL, ap);
  }

  FOG_INLINE err_t appendVFormat(const StringW& fmt, va_list ap)
  {
    return fog_api.stringw_opVFormatStringW(this, CONTAINER_OP_APPEND, &fmt, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_ASCII], NULL, ap);
  }

  FOG_INLINE err_t appendVFormat_c(const StringW& fmt, const TextCodec& tc, va_list ap)
  {
    return fog_api.stringw_opVFormatStringW(this, CONTAINER_OP_APPEND, &fmt, &tc, NULL, ap);
  }

  // --------------------------------------------------------------------------
  // [AppendZFormat]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t appendZFormat(const CharW* fmt, CharW lex, const List<StringW>& args)
  {
    StubW fmtW(fmt, DETECT_LENGTH);
    ListUntypedData* args_d = reinterpret_cast<const ListUntyped*>(&args)->_d;

    return fog_api.stringw_opZFormatStubW(this, CONTAINER_OP_APPEND, &fmtW, lex,
      reinterpret_cast<const StringW*>(args_d->data), args_d->length);
  }

  FOG_INLINE err_t appendZFormat(const CharW* fmt, CharW lex, const StringW* args, size_t argsLength)
  {
    StubW fmtW(fmt, DETECT_LENGTH);
    return fog_api.stringw_opZFormatStubW(this, CONTAINER_OP_APPEND, &fmtW, lex,
      args, argsLength);
  }

  FOG_INLINE err_t appendZFormat(const StubW& fmt, CharW lex, const List<StringW>& args)
  {
    ListUntypedData* args_d = reinterpret_cast<const ListUntyped*>(&args)->_d;

    return fog_api.stringw_opZFormatStubW(this, CONTAINER_OP_APPEND, &fmt, lex,
      reinterpret_cast<const StringW*>(args_d->data), args_d->length);
  }

  FOG_INLINE err_t appendZFormat(const StubW& fmt, CharW lex, const StringW* args, size_t argsLength)
  {
    return fog_api.stringw_opZFormatStubW(this, CONTAINER_OP_APPEND, &fmt, lex,
      args, argsLength);
  }

  FOG_INLINE err_t appendZFormat(const StringW& fmt, CharW lex, const List<StringW>& args)
  {
    ListUntypedData* args_d = reinterpret_cast<const ListUntyped*>(&args)->_d;

    return fog_api.stringw_opZFormatStringW(this, CONTAINER_OP_APPEND, &fmt, lex,
      reinterpret_cast<const StringW*>(args_d->data), args_d->length);
  }

  FOG_INLINE err_t appendZFormat(const StringW& fmt, CharW lex, const StringW* args, size_t argsLength)
  {
    return fog_api.stringw_opZFormatStringW(this, CONTAINER_OP_APPEND, &fmt, lex,
      args, argsLength);
  }

  // --------------------------------------------------------------------------
  // [Prepend]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t prepend(CharW ch)
  {
    return fog_api.stringw_prependChars(this, ch._value, 1);
  }

  FOG_INLINE err_t prepend(CharW ch, size_t length)
  {
    return fog_api.stringw_prependChars(this, ch._value, length);
  }

  FOG_INLINE err_t prepend(const CharW* str, size_t length = DETECT_LENGTH)
  {
    StubW stub(str, length);
    return fog_api.stringw_prependStubW(this, &stub);
  }

  FOG_INLINE err_t prepend(const Ascii8& stub)
  {
    return fog_api.stringw_prependStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_ASCII]);
  }

  FOG_INLINE err_t prepend(const Local8& stub)
  {
    return fog_api.stringw_prependStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_LOCAL]);
  }

  FOG_INLINE err_t prepend(const Utf8& stub)
  {
    return fog_api.stringw_prependStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_UTF8]);
  }

  FOG_INLINE err_t prepend(const StubA& stub, const TextCodec& tc)
  {
    return fog_api.stringw_prependStubA(this, &stub, &tc);
  }

  FOG_INLINE err_t prepend(const StubW& stub)
  {
    return fog_api.stringw_prependStubW(this, &stub);
  }

  FOG_INLINE err_t prependAscii8(const char* str, size_t length = DETECT_LENGTH)
  {
    StubA stub(str, length);
    return fog_api.stringw_prependStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_ASCII]);
  }

  FOG_INLINE err_t prependAscii8(const StubA& stub)
  {
    return fog_api.stringw_prependStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_ASCII]);
  }

  FOG_INLINE err_t prependLocal8(const char* str, size_t length = DETECT_LENGTH)
  {
    StubA stub(str, length);
    return fog_api.stringw_prependStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_LOCAL]);
  }

  FOG_INLINE err_t prependLocal8(const StubA& stub)
  {
    return fog_api.stringw_prependStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_LOCAL]);
  }

  FOG_INLINE err_t prependUtf8(const char* str, size_t length = DETECT_LENGTH)
  {
    StubA stub(str, length);
    return fog_api.stringw_prependStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_UTF8]);
  }

  FOG_INLINE err_t prependUtf8(const StubA& stub)
  {
    return fog_api.stringw_prependStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_UTF8]);
  }

  FOG_INLINE err_t prependUtf16(const uint16_t* str, size_t length = DETECT_LENGTH)
  {
    StubW stub(str, length);
    return fog_api.stringw_prependStubW(this, &stub);
  }

  FOG_INLINE err_t prependUtf16(const StubW& stub)
  {
    return fog_api.stringw_prependStubW(this, &stub);
  }

  FOG_INLINE err_t prependUtf32(const uint32_t* str, size_t length = DETECT_LENGTH)
  {
    StubA stub(reinterpret_cast<const char*>(str), length);
    return fog_api.stringw_prependStubA(this, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_UTF32]);
  }

  FOG_INLINE err_t prependWChar(const wchar_t* str, size_t length = DETECT_LENGTH)
  {
    if (sizeof(wchar_t) == 2)
      return prependUtf16(reinterpret_cast<const uint16_t*>(str), length);
    else
      return prependUtf32(reinterpret_cast<const uint32_t*>(str), length);
  }

  FOG_INLINE err_t prepend(const StringA& other, const TextCodec& tc)
  {
    return fog_api.stringw_prependStringA(this, &other, &tc);
  }

  FOG_INLINE err_t prepend(const StringW& other)
  {
    return fog_api.stringw_prependStringW(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Insert]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t insert(size_t index, CharW ch)
  {
    return fog_api.stringw_insertChars(this, index, ch._value, 1);
  }

  FOG_INLINE err_t insert(size_t index, CharW ch, size_t length)
  {
    return fog_api.stringw_insertChars(this, index, ch._value, length);
  }

  FOG_INLINE err_t insert(size_t index, const CharW* str, size_t length = DETECT_LENGTH)
  {
    StubW stub(str, length);
    return fog_api.stringw_insertStubW(this, index, &stub);
  }

  FOG_INLINE err_t insert(size_t index, const Ascii8& stub)
  {
    return fog_api.stringw_insertStubA(this, index, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_ASCII]);
  }

  FOG_INLINE err_t insert(size_t index, const Local8& stub)
  {
    return fog_api.stringw_insertStubA(this, index, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_LOCAL]);
  }

  FOG_INLINE err_t insert(size_t index, const Utf8& stub)
  {
    return fog_api.stringw_insertStubA(this, index, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_UTF8]);
  }

  FOG_INLINE err_t insert(size_t index, const StubA& stub, const TextCodec& tc)
  {
    return fog_api.stringw_insertStubA(this, index, &stub, &tc);
  }

  FOG_INLINE err_t insert(size_t index, const StubW& stub)
  {
    return fog_api.stringw_insertStubW(this, index, &stub);
  }

  FOG_INLINE err_t insertAscii8(size_t index, const char* str, size_t length = DETECT_LENGTH)
  {
    StubA stub(str, length);
    return fog_api.stringw_insertStubA(this, index, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_ASCII]);
  }

  FOG_INLINE err_t insertAscii8(size_t index, const StubA& stub)
  {
    return fog_api.stringw_insertStubA(this, index, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_ASCII]);
  }

  FOG_INLINE err_t insertLocal8(size_t index, const char* str, size_t length = DETECT_LENGTH)
  {
    StubA stub(str, length);
    return fog_api.stringw_insertStubA(this, index, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_LOCAL]);
  }

  FOG_INLINE err_t insertLocal8(size_t index, const StubA& stub)
  {
    return fog_api.stringw_insertStubA(this, index, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_LOCAL]);
  }

  FOG_INLINE err_t insertUtf8(size_t index, const char* str, size_t length = DETECT_LENGTH)
  {
    StubA stub(str, length);
    return fog_api.stringw_insertStubA(this, index, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_UTF8]);
  }

  FOG_INLINE err_t insertUtf8(size_t index, const StubA& stub)
  {
    return fog_api.stringw_insertStubA(this, index, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_UTF8]);
  }

  FOG_INLINE err_t insertUtf16(size_t index, const uint16_t* str, size_t length = DETECT_LENGTH)
  {
    StubW stub(str, length);
    return fog_api.stringw_insertStubW(this, index, &stub);
  }

  FOG_INLINE err_t insertUtf16(size_t index, const StubW& stub)
  {
    return fog_api.stringw_insertStubW(this, index, &stub);
  }

  FOG_INLINE err_t insertUtf32(size_t index, const uint32_t* str, size_t length = DETECT_LENGTH)
  {
    StubA stub(reinterpret_cast<const char*>(str), length);
    return fog_api.stringw_insertStubA(this, index, &stub, fog_api.textcodec_oCache[TEXT_CODEC_CACHE_UTF32]);
  }

  FOG_INLINE err_t insertWChar(size_t index, const wchar_t* str, size_t length = DETECT_LENGTH)
  {
    if (sizeof(wchar_t) == 2)
      return insertUtf16(index, reinterpret_cast<const uint16_t*>(str), length);
    else
      return insertUtf32(index, reinterpret_cast<const uint32_t*>(str), length);
  }

  FOG_INLINE err_t insert(size_t index, const StringA& other, const TextCodec& tc)
  {
    return fog_api.stringw_insertStringA(this, index, &other, &tc);
  }

  FOG_INLINE err_t insert(size_t index, const StringW& other)
  {
    return fog_api.stringw_insertStringW(this, index, &other);
  }

  // --------------------------------------------------------------------------
  // [Remove]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t remove(const Range& range)
  {
    return fog_api.stringw_removeRange(this, &range);
  }

  FOG_INLINE err_t remove(const Range* range, size_t rangeLength)
  {
    return fog_api.stringw_removeRangeList(this, range, rangeLength);
  }

  FOG_INLINE err_t remove(CharW ch, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringw_removeChar(this, NULL, ch._value, cs);
  }

  FOG_INLINE err_t remove(const Ascii8& stub, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringw_removeStubA(this, NULL, &stub, cs);
  }

  FOG_INLINE err_t remove(const StubW& stub, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringw_removeStubW(this, NULL, &stub, cs);
  }

  FOG_INLINE err_t remove(const StringW& other, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringw_removeStringW(this, NULL, &other, cs);
  }

  FOG_INLINE err_t remove(const RegExpW& re)
  {
    return fog_api.stringw_removeRegExpW(this, NULL, &re);
  }

  FOG_INLINE err_t remove(const Range& range, CharW ch, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringw_removeChar(this, &range, ch._value, cs);
  }

  FOG_INLINE err_t remove(const Range& range, const Ascii8& stub, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringw_removeStubA(this, &range, &stub, cs);
  }

  FOG_INLINE err_t remove(const Range& range, const StubW& stub, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringw_removeStubW(this, &range, &stub, cs);
  }

  FOG_INLINE err_t remove(const Range& range, const StringW& other, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringw_removeStringW(this, &range, &other, cs);
  }

  FOG_INLINE err_t remove(const Range& range, const RegExpW& re)
  {
    return fog_api.stringw_removeRegExpW(this, &range, &re);
  }

  // --------------------------------------------------------------------------
  // [Replace]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t replace(const Range& range, const StubW& replacement)
  {
    return fog_api.stringw_replaceRangeStubW(this, &range, &replacement);
  }

  FOG_INLINE err_t replace(const Range& range, const StringW& replacement)
  {
    return fog_api.stringw_replaceRangeStringW(this, &range, &replacement);
  }

  FOG_INLINE err_t replace(const Range* range, size_t rangeLength, const StubW& replacement)
  {
    return fog_api.stringw_replaceRangeListStubW(this, range, rangeLength, &replacement);
  }

  FOG_INLINE err_t replace(const Range* range, size_t rangeLength, const StringW& replacement)
  {
    return fog_api.stringw_replaceRangeListStringW(this, range, rangeLength, &replacement);
  }

  FOG_INLINE err_t replace(CharW before, CharW after, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringw_replaceChar(this, NULL, before._value, after._value, cs);
  }

  FOG_INLINE err_t replace(const StringW& pattern, const StringW& replacement, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringw_replaceStringW(this, NULL, &pattern, &replacement, cs);
  }

  FOG_INLINE err_t replace(const RegExpW& re, const StringW& replacement)
  {
    return fog_api.stringw_replaceRegExpW(this, NULL, &re, &replacement);
  }

  FOG_INLINE err_t replace(const Range& range, char before, char after, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringw_replaceChar(this, &range, before, after, cs);
  }

  FOG_INLINE err_t replace(const Range& range, const StringW& pattern, const StringW& replacement, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringw_replaceStringW(this, &range, &pattern, &replacement, cs);
  }

  FOG_INLINE err_t replace(const Range& range, const RegExpW& re, const StringW& replacement)
  {
    return fog_api.stringw_replaceRegExpW(this, &range, &re, &replacement);
  }

  // --------------------------------------------------------------------------
  // [Lower / Upper]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t lower() { return fog_api.stringw_lower(this, NULL); }
  FOG_INLINE err_t upper() { return fog_api.stringw_upper(this, NULL); }

  FOG_INLINE err_t lower(const Range& range) { return fog_api.stringw_lower(this, &range); }
  FOG_INLINE err_t upper(const Range& range) { return fog_api.stringw_upper(this, &range); }

  FOG_INLINE StringW lowered() const
  {
    StringW result(*this);
    fog_api.stringw_lower(&result, NULL);
    return result;
  }

  FOG_INLINE StringW uppered() const
  {
    StringW result(*this);
    fog_api.stringw_upper(&result, NULL);
    return result;
  }

  // --------------------------------------------------------------------------
  // [Trim / Simplify]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t trim()
  {
    return fog_api.stringw_trim(this);
  }

  FOG_INLINE err_t simplify()
  {
    return fog_api.stringw_simplify(this);
  }

  FOG_INLINE StringW trimmed() const
  {
    StringW result(*this);
    fog_api.stringw_trim(&result);
    return result;
  }

  FOG_INLINE StringW simplified() const
  {
    StringW result(*this);
    fog_api.stringw_simplify(&result);
    return result;
  }

  // --------------------------------------------------------------------------
  // [Justify]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t justify(size_t n, CharW ch, uint32_t flags)
  {
    return fog_api.stringw_justify(this, n, ch._value, flags);
  }

  FOG_INLINE StringW justified(size_t n, CharW ch, uint32_t flags) const
  {
    StringW result(*this);
    fog_api.stringw_justify(&result, n, ch._value, flags);
    return result;
  }

  // --------------------------------------------------------------------------
  // [Split]
  // --------------------------------------------------------------------------

  // Implemented-Later.
  FOG_INLINE List<StringW> split(CharW sep, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY, uint32_t cs = CASE_SENSITIVE);
  FOG_INLINE List<StringW> split(const StringW& sep, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY, uint32_t cs = CASE_SENSITIVE);
  FOG_INLINE List<StringW> split(const RegExpW& re, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY);

  FOG_INLINE List<StringW> split(const Range& range, CharW sep, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY, uint32_t cs = CASE_SENSITIVE);
  FOG_INLINE List<StringW> split(const Range& range, const StringW& sep, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY, uint32_t cs = CASE_SENSITIVE);
  FOG_INLINE List<StringW> split(const Range& range, const RegExpW& re, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY);

  FOG_INLINE err_t splitTo(List<StringW>& dst, uint32_t cntOp, CharW sep, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringw_splitChar(&dst, cntOp, this, NULL, sep._value, splitBehavior, cs);
  }

  FOG_INLINE err_t splitTo(List<StringW>& dst, uint32_t cntOp, const StringW& sep, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringw_splitStringW(&dst, cntOp, this, NULL, &sep, splitBehavior, cs);
  }

  FOG_INLINE err_t splitTo(List<StringW>& dst, uint32_t cntOp, const RegExpW& re, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY)
  {
    return fog_api.stringw_splitRegExpW(&dst, cntOp, this, NULL, &re, splitBehavior);
  }

  FOG_INLINE err_t splitTo(List<StringW>& dst, uint32_t cntOp, const Range& range, CharW sep, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringw_splitChar(&dst, cntOp, this, &range, sep._value, splitBehavior, cs);
  }

  FOG_INLINE err_t splitTo(List<StringW>& dst, uint32_t cntOp, const Range& range, const StringW& sep, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.stringw_splitStringW(&dst, cntOp, this, &range, &sep, splitBehavior, cs);
  }

  FOG_INLINE err_t splitTo(List<StringW>& dst, uint32_t cntOp, const Range& range, const RegExpW& re, uint32_t splitBehavior = SPLIT_REMOVE_EMPTY)
  {
    return fog_api.stringw_splitRegExpW(&dst, cntOp, this, &range, &re, splitBehavior);
  }

  // --------------------------------------------------------------------------
  // [Slice]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t slice(size_t start, size_t end)
  {
    Range range(start, end);
    return fog_api.stringw_slice(this, &range);
  }

  FOG_INLINE err_t slice(const Range& range)
  {
    return fog_api.stringw_slice(this, &range);
  }

  // --------------------------------------------------------------------------
  // [Substring]
  // --------------------------------------------------------------------------

  FOG_INLINE StringW substring(const Range& range) const
  {
    StringW result((StringDataW*)NULL);
    fog_api.stringw_ctorSubstr(&result, this, &range);
    return result;
  }

  // --------------------------------------------------------------------------
  // [ParseBool]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t parseBool(bool* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringw_parseBool(this, dst, base, pEnd, pFlags);
  }

  // --------------------------------------------------------------------------
  // [ParseInt]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t parseI8(int8_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringw_parseI8(this, dst, base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseU8(uint8_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringw_parseU8(this, dst, base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseI16(int16_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringw_parseI16(this, dst, base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseU16(uint16_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringw_parseU16(this, dst, base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseI32(int32_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringw_parseI32(this, dst, base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseU32(uint32_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringw_parseU32(this, dst, base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseI64(int64_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringw_parseI64(this, dst, base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseU64(uint64_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringw_parseU64(this, dst, base, pEnd, pFlags);
  }

  // --------------------------------------------------------------------------
  // [ParseLong]
  // --------------------------------------------------------------------------

#if FOG_SIZEOF_LONG == 32
  FOG_INLINE err_t parseLong(long* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringw_parseI32(this, reinterpret_cast<int32_t*>(dst), base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseULong(ulong* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringw_parseU32(this, reinterpret_cast<uint32_t*>(dst), base, pEnd, pFlags);
  }
#else
  FOG_INLINE err_t parseLong(long* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringw_parseI64(this, reinterpret_cast<int64_t*>(dst), base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseULong(ulong* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringw_parseU64(this, reinterpret_cast<uint64_t*>(dst), base, pEnd, pFlags);
  }
#endif

  // --------------------------------------------------------------------------
  // [ParseSizeT]
  // --------------------------------------------------------------------------

#if FOG_ARCH_BITS == 32
  FOG_INLINE err_t parseSSizeT(ssize_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringw_parseI32(this, reinterpret_cast<int32_t*>(dst), base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseSizeT(size_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringw_parseU32(this, reinterpret_cast<uint32_t*>(dst), base, pEnd, pFlags);
  }
#else
  FOG_INLINE err_t parseSSizeT(ssize_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringw_parseI64(this, reinterpret_cast<int64_t*>(dst), base, pEnd, pFlags);
  }

  FOG_INLINE err_t parseSizeT(size_t* dst, uint32_t base = 0, size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringw_parseU64(this, reinterpret_cast<uint64_t*>(dst), base, pEnd, pFlags);
  }
#endif

  // --------------------------------------------------------------------------
  // [ParseReal]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t parseReal(float* dst, CharW decimalPoint = CharW('.'), size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringw_parseFloat(this, dst, decimalPoint._value, pEnd, pFlags);
  }

  FOG_INLINE err_t parseReal(double* dst, CharW decimalPoint = CharW('.'), size_t* pEnd = NULL, uint32_t* pFlags = NULL) const
  {
    return fog_api.stringw_parseDouble(this, dst, decimalPoint._value, pEnd, pFlags);
  }

  // --------------------------------------------------------------------------
  // [Contains]
  // --------------------------------------------------------------------------

  FOG_INLINE bool contains(CharW ch, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_indexOfChar(this, NULL, ch._value, cs) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const Ascii8& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_indexOfStubA(this, NULL, &pattern, cs) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const StubW& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_indexOfStubW(this, NULL, &pattern, cs) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const StringW& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_indexOfStringW(this, NULL, &pattern, cs) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const RegExpW& re) const
  {
    return fog_api.stringw_indexOfRegExpW(this, NULL, &re) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const Range& range, CharW ch, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_indexOfChar(this, &range, ch._value, cs) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const Range& range, const Ascii8& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_indexOfStubA(this, &range, &pattern, cs) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const Range& range, const StubW& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_indexOfStubW(this, &range, &pattern, cs) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const Range& range, const StringW& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_indexOfStringW(this, &range, &pattern, cs) != INVALID_INDEX;
  }

  FOG_INLINE bool contains(const Range& range, const RegExpW& re) const
  {
    return fog_api.stringw_indexOfRegExpW(this, &range, &re) != INVALID_INDEX;
  }

  // --------------------------------------------------------------------------
  // [CountOf]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t countOf(CharW ch, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_countOfChar(this, NULL, ch._value, cs);
  }

  FOG_INLINE size_t countOf(const Ascii8& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_countOfStubA(this, NULL, &pattern, cs);
  }

  FOG_INLINE size_t countOf(const StubW& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_countOfStubW(this, NULL, &pattern, cs);
  }

  FOG_INLINE size_t countOf(const StringW& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_countOfStringW(this, NULL, &pattern, cs);
  }

  FOG_INLINE size_t countOf(const RegExpW& re) const
  {
    return fog_api.stringw_countOfRegExpW(this, NULL, &re);
  }

  FOG_INLINE size_t countOf(const Range& range, CharW ch, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_countOfChar(this, &range, ch._value, cs);
  }

  FOG_INLINE size_t countOf(const Range& range, const Ascii8& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_countOfStubA(this, &range, &pattern, cs);
  }

  FOG_INLINE size_t countOf(const Range& range, const StubW& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_countOfStubW(this, &range, &pattern, cs);
  }

  FOG_INLINE size_t countOf(const Range& range, const StringW& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_countOfStringW(this, &range, &pattern, cs);
  }

  FOG_INLINE size_t countOf(const Range& range, const RegExpW& re) const
  {
    return fog_api.stringw_countOfRegExpW(this, &range, &re);
  }

  // --------------------------------------------------------------------------
  // [IndexOf]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t indexOf(CharW ch, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_indexOfChar(this, NULL, ch._value, cs);
  }

  FOG_INLINE size_t indexOf(const Ascii8& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_indexOfStubA(this, NULL, &pattern, cs);
  }

  FOG_INLINE size_t indexOf(const StubW& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_indexOfStubW(this, NULL, &pattern, cs);
  }

  FOG_INLINE size_t indexOf(const StringW& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_indexOfStringW(this, NULL, &pattern, cs);
  }

  FOG_INLINE size_t indexOf(const RegExpW& re) const
  {
    return fog_api.stringw_indexOfRegExpW(this, NULL, &re);
  }

  FOG_INLINE size_t indexOf(const Range& range, CharW ch, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_indexOfChar(this, &range, ch._value, cs);
  }

  FOG_INLINE size_t indexOf(const Range& range, const Ascii8& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_indexOfStubA(this, &range, &pattern, cs);
  }

  FOG_INLINE size_t indexOf(const Range& range, const StubW& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_indexOfStubW(this, &range, &pattern, cs);
  }

  FOG_INLINE size_t indexOf(const Range& range, const StringW& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_indexOfStringW(this, &range, &pattern, cs);
  }

  FOG_INLINE size_t indexOf(const Range& range, const RegExpW& re) const
  {
    return fog_api.stringw_indexOfRegExpW(this, &range, &re);
  }

  FOG_INLINE size_t indexOfAny(const CharW* charArray, size_t charLength, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_indexOfAnyCharW(this, NULL, charArray, charLength, cs);
  }

  FOG_INLINE size_t indexOfAny(const Range& range, const CharW* charArray, size_t charLength, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_indexOfAnyCharW(this, &range, charArray, charLength, cs);
  }

  // --------------------------------------------------------------------------
  // [LastIndexOf]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t lastIndexOf(CharW ch, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_lastIndexOfChar(this, NULL, ch._value, cs);
  }

  FOG_INLINE size_t lastIndexOf(const Ascii8& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_lastIndexOfStubA(this, NULL, &pattern, cs);
  }

  FOG_INLINE size_t lastIndexOf(const StubW& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_lastIndexOfStubW(this, NULL, &pattern, cs);
  }

  FOG_INLINE size_t lastIndexOf(const StringW& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_lastIndexOfStringW(this, NULL, &pattern, cs);
  }

  FOG_INLINE size_t lastIndexOf(const RegExpW& re) const
  {
    return fog_api.stringw_lastIndexOfRegExpW(this, NULL, &re);
  }

  FOG_INLINE size_t lastIndexOf(const Range& range, CharW ch, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_lastIndexOfChar(this, &range, ch._value, cs);
  }

  FOG_INLINE size_t lastIndexOf(const Range& range, const Ascii8& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_lastIndexOfStubA(this, &range, &pattern, cs);
  }

  FOG_INLINE size_t lastIndexOf(const Range& range, const StubW& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_lastIndexOfStubW(this, &range, &pattern, cs);
  }

  FOG_INLINE size_t lastIndexOf(const Range& range, const StringW& pattern, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_lastIndexOfStringW(this, &range, &pattern, cs);
  }

  FOG_INLINE size_t lastIndexOf(const Range& range, const RegExpW& re) const
  {
    return fog_api.stringw_lastIndexOfRegExpW(this, &range, &re);
  }

  FOG_INLINE size_t lastIndexOfAny(const CharW* charArray, size_t charLength, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_lastIndexOfAnyCharW(this, NULL, charArray, charLength, cs);
  }

  FOG_INLINE size_t lastIndexOfAny(const Range& range, const CharW* charArray, size_t charLength, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_lastIndexOfAnyCharW(this, &range, charArray, charLength, cs);
  }

  // --------------------------------------------------------------------------
  // [StartsWith]
  // --------------------------------------------------------------------------

  FOG_INLINE bool startsWith(CharW ch, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_startsWithChar(this, ch._value, cs);
  }

  FOG_INLINE bool startsWith(const CharW* str, uint32_t cs = CASE_SENSITIVE) const
  {
    StubW stub(str, DETECT_LENGTH);
    return fog_api.stringw_startsWithStubW(this, &stub, cs);
  }

  FOG_INLINE bool startsWith(const Ascii8& stub, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_startsWithStubA(this, &stub, cs);
  }

  FOG_INLINE bool startsWith(const StubW& stub, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_startsWithStubW(this, &stub, cs);
  }

  FOG_INLINE bool startsWith(const StringW& other, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_startsWithStringW(this, &other, cs);
  }

  FOG_INLINE bool startsWith(const RegExpW& re) const
  {
    return fog_api.stringw_startsWithRegExpW(this, &re);
  }

  // --------------------------------------------------------------------------
  // [EndsWith]
  // --------------------------------------------------------------------------

  FOG_INLINE bool endsWith(CharW ch, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_endsWithChar(this, ch._value, cs);
  }

  FOG_INLINE bool endsWith(const CharW* str, uint32_t cs = CASE_SENSITIVE) const
  {
    StubW stub(str, DETECT_LENGTH);
    return fog_api.stringw_endsWithStubW(this, &stub, cs);
  }

  FOG_INLINE bool endsWith(const Ascii8& stub, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_endsWithStubA(this, &stub, cs);
  }

  FOG_INLINE bool endsWith(const StubW& stub, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_endsWithStubW(this, &stub, cs);
  }

  FOG_INLINE bool endsWith(const StringW& other, uint32_t cs = CASE_SENSITIVE) const
  {
    return fog_api.stringw_endsWithStringW(this, &other, cs);
  }

  FOG_INLINE bool endsWith(const RegExpW& re) const
  {
    return fog_api.stringw_endsWithRegExpW(this, &re);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const CharW* str) const
  {
    return eq(StubW(str));
  }

  FOG_INLINE bool eq(const CharW* str, uint32_t cs) const
  {
    return eq(StubW(str), cs);
  }

  FOG_INLINE bool eq(const Ascii8& stub) const
  {
    return fog_api.stringw_eqStubA(this, &stub);
  }

  FOG_INLINE bool eq(const Ascii8& stub, uint32_t cs) const
  {
    return fog_api.stringw_eqStubExA(this, &stub, cs);
  }

  FOG_INLINE bool eq(const StubW& stub) const
  {
    return fog_api.stringw_eqStubW(this, &stub);
  }

  FOG_INLINE bool eq(const StubW& stub, uint32_t cs) const
  {
    return fog_api.stringw_eqStubExW(this, &stub, cs);
  }

  FOG_INLINE bool eq(const StringW& other) const
  {
    return fog_api.stringw_eqStringW(this, &other);
  }

  FOG_INLINE bool eq(const StringW& other, uint32_t cs) const
  {
    return fog_api.stringw_eqStringExW(this, &other, cs);
  }

  // --------------------------------------------------------------------------
  // [Comparison]
  // --------------------------------------------------------------------------

  FOG_INLINE int compare(const CharW* str) const
  {
    return compare(StubW(str));
  }

  FOG_INLINE int compare(const CharW* str, uint32_t cs) const
  {
    return compare(StubW(str), cs);
  }

  FOG_INLINE int compare(const Ascii8& stub) const
  {
    return fog_api.stringw_compareStubA(this, &stub);
  }

  FOG_INLINE int compare(const Ascii8& stub, uint32_t cs) const
  {
    return fog_api.stringw_compareStubExA(this, &stub, cs);
  }

  FOG_INLINE int compare(const StubW& stub) const
  {
    return fog_api.stringw_compareStubW(this, &stub);
  }

  FOG_INLINE int compare(const StubW& stub, uint32_t cs) const
  {
    return fog_api.stringw_compareStubExW(this, &stub, cs);
  }

  FOG_INLINE int compare(const StringW& other) const
  {
    return fog_api.stringw_compareStringW(this, &other);
  }

  FOG_INLINE int compare(const StringW& other, uint32_t cs) const
  {
    return fog_api.stringw_compareStringExW(this, &other, cs);
  }

  // --------------------------------------------------------------------------
  // [Modified]
  // --------------------------------------------------------------------------

  FOG_INLINE void _modified()
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::StringW::_modified() - Not detached.");
    FOG_ASSERT_X(_d->data[_d->length] == 0,
      "Fog::StringW::_modified() - Null terminator corrupted.");

    _d->hashCode = 0;
  }

  FOG_INLINE void _modified(CharW* end)
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::StringW::_modified() - Not detached.");
    FOG_ASSERT_X(end <= _d->data + _d->capacity,
      "Fog::StringW::_modified() - Buffer overflow.");

    end->setValue(0);
    _d->hashCode = 0;
    _d->length = (size_t)(end - _d->data);
  }

  // --------------------------------------------------------------------------
  // [RValue]
  // --------------------------------------------------------------------------

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE StringDataW* _acquireData()
  {
    StringDataW* d = _d;
    _d = NULL;
    return d;
  }
#endif // FOG_CC_HAS_RVALUE

  // --------------------------------------------------------------------------
  // [Utf16 Support]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t validateUtf16(size_t* invalid = NULL) const
  {
    return fog_api.stringw_validateUtf16(this, invalid);
  }

  FOG_INLINE err_t getUcsLength(size_t* ucsLength) const
  {
    return fog_api.stringw_getUcsLength(this, ucsLength);
  }

  // --------------------------------------------------------------------------
  // [BSwap]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t bswap()
  {
    return fog_api.stringw_bswap(this);
  }

#if defined(FOG_OS_MAC)
  FOG_INLINE err_t fromNSString(const NSString* src)
  {
    return fog_api.stringw_fromNSString(this, src);
  }

  FOG_INLINE err_t toNSString(NSString** dst) const
  {
    return fog_api.stringw_toNSString(this, dst);
  }
#endif // FOG_OS_MAC

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE StringW& operator=(CharW ch) { setChar(ch); return *this; }
  FOG_INLINE StringW& operator=(const CharW* str) { set(str); return *this; }
  FOG_INLINE StringW& operator=(const Ascii8& str) { set(str); return *this; }
  FOG_INLINE StringW& operator=(const Local8& str) { set(str); return *this; }
  FOG_INLINE StringW& operator=(const Utf8& str) { set(str); return *this; }
  FOG_INLINE StringW& operator=(const StubW& str) { set(str); return *this; }
  FOG_INLINE StringW& operator=(const StringW& other) { set(other); return *this; }

  FOG_INLINE StringW& operator+=(CharW ch) { append(ch); return *this; }
  FOG_INLINE StringW& operator+=(const CharW* str) { append(str); return *this; }
  FOG_INLINE StringW& operator+=(const Ascii8& str) { append(str); return *this; }
  FOG_INLINE StringW& operator+=(const Local8& str) { append(str); return *this; }
  FOG_INLINE StringW& operator+=(const Utf8& str) { append(str); return *this; }
  FOG_INLINE StringW& operator+=(const StubW& str) { append(str); return *this; }
  FOG_INLINE StringW& operator+=(const StringW& other) { append(other); return *this; }

  FOG_INLINE CharW operator[](size_t index) const { return getAt(index); }

  // --------------------------------------------------------------------------
  // [Statics - From]
  // --------------------------------------------------------------------------

  static FOG_INLINE StringW fromData(const CharW* data, size_t length = DETECT_LENGTH) { return StringW(StubW(data, length)); }
  static FOG_INLINE StringW fromData(const StubW& data) { return StringW(data); }

  static FOG_INLINE StringW fromOther(const StringW& s) { return StringW(s); }
  static FOG_INLINE StringW fromOther(const StringW& s, const Range& range) { return StringW(s, range); }

  static FOG_INLINE StringW fromAscii8(const char* data, size_t length = DETECT_LENGTH) { return StringW(Ascii8(data, length)); }
  static FOG_INLINE StringW fromAscii8(const Ascii8& data) { return StringW(data); }

  static FOG_INLINE StringW fromLocal8(const char* data, size_t length = DETECT_LENGTH) { return StringW(Local8(data, length)); }
  static FOG_INLINE StringW fromLocal8(const Local8& data) { return StringW(data); }

  static FOG_INLINE StringW fromUtf8(const char* data, size_t length = DETECT_LENGTH) { return StringW(Utf8(data, length)); }
  static FOG_INLINE StringW fromUtf8(const Local8& data) { return StringW(data); }

  static FOG_INLINE StringW fromUtf16(const char* data, size_t length = DETECT_LENGTH) { return StringW(StubW(reinterpret_cast<const CharW*>(data), length)); }
  static FOG_INLINE StringW fromUtf16(const StubW& data) { return StringW(data); }

  static FOG_INLINE StringW fromTwo(const CharW* str1, const CharW* str2) { return StringW(str1, str2); }
  static FOG_INLINE StringW fromTwo(const StubW& str1, const StubW& str2) { return StringW(str1, str2); }
  static FOG_INLINE StringW fromTwo(const StringW& str1, const StringW& str2) { return StringW(str1, str2); }

  static FOG_INLINE StringW fromBool(bool b) { StringW t; t.setBool(b); return t; }

  static FOG_INLINE StringW fromInt(int8_t n) { StringW t(UNINITIALIZED); fog_api.stringw_ctorU32(&t, (uint8_t)n, false); return t; }
  static FOG_INLINE StringW fromInt(uint8_t n) { StringW t(UNINITIALIZED); fog_api.stringw_ctorU32(&t, (uint8_t)n, true); return t; }
  static FOG_INLINE StringW fromInt(int16_t n) { StringW t(UNINITIALIZED); fog_api.stringw_ctorU32(&t, (uint16_t)n, false); return t; }
  static FOG_INLINE StringW fromInt(uint16_t n) { StringW t(UNINITIALIZED); fog_api.stringw_ctorU32(&t, (uint16_t)n, true); return t; }
  static FOG_INLINE StringW fromInt(int32_t n) { StringW t(UNINITIALIZED); fog_api.stringw_ctorU32(&t, n, false); return t; }
  static FOG_INLINE StringW fromInt(uint32_t n) { StringW t(UNINITIALIZED); fog_api.stringw_ctorU32(&t, n, true); return t; }
  static FOG_INLINE StringW fromInt(int64_t n) { StringW t(UNINITIALIZED); fog_api.stringw_ctorU64(&t, n, false); return t; }
  static FOG_INLINE StringW fromInt(uint64_t n) { StringW t(UNINITIALIZED); fog_api.stringw_ctorU64(&t, n, true); return t; }

  static FOG_INLINE StringW fromReal(float d) { StringW t(UNINITIALIZED); fog_api.stringw_ctorDouble(&t, d); return t; }
  static FOG_INLINE StringW fromReal(double d) { StringW t(UNINITIALIZED); fog_api.stringw_ctorDouble(&t, d); return t; }

  // --------------------------------------------------------------------------
  // [Statics - Join]
  // --------------------------------------------------------------------------

  static FOG_INLINE StringW join(const List<StringW>& list, CharW separator)
  {
    StringW result;
    ListUntypedData* list_d = reinterpret_cast<const ListUntyped*>(&list)->_d;

    fog_api.stringw_joinChar(&result,
      reinterpret_cast<const StringW*>(list_d->data), list_d->length,
      separator._value);

    return result;
  }

  static FOG_INLINE StringW join(const List<StringW>& list, const StringW& separator)
  {
    StringW result;
    ListUntypedData* list_d = reinterpret_cast<const ListUntyped*>(&list)->_d;

    fog_api.stringw_joinStringW(&result,
      reinterpret_cast<const StringW*>(list_d->data), list_d->length,
      &separator);

    return result;
  }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const StringW* a, const StringW* b)
  {
    return fog_api.stringw_eqStringW(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.stringw_eqStringW;
  }

  // --------------------------------------------------------------------------
  // [Statics - Compare]
  // --------------------------------------------------------------------------

  static FOG_INLINE int compare(const StringW* a, const StringW* b)
  {
    return fog_api.stringw_compareStringW(a, b);
  }

  static FOG_INLINE CompareFunc getCompareFunc()
  {
    return (CompareFunc)fog_api.stringw_compareStringW;
  }

  // --------------------------------------------------------------------------
  // [Statics - Base64]
  // --------------------------------------------------------------------------

  static FOG_INLINE err_t base64Encode(StringW& dst, uint32_t cntOp, const StringA& src)
  {
    return fog_api.stringw_base64EncodeStringA(&dst, cntOp, &src);
  }

  static FOG_INLINE err_t base64Encode(StringW& dst, uint32_t cntOp, const char* src, size_t srcLength)
  {
    return fog_api.stringw_base64EncodeDataA(&dst, cntOp, src, srcLength);
  }

  // --------------------------------------------------------------------------
  // [Statics - StringDataW]
  // --------------------------------------------------------------------------

  static FOG_INLINE StringDataW* _dCreate(size_t capacity)
  {
    return fog_api.stringw_dCreate(capacity);
  }

  static FOG_INLINE StringDataW* _dCreate(size_t capacity, const StubA& stub)
  {
    return fog_api.stringw_dCreateStubA(capacity, &stub);
  }

  static FOG_INLINE StringDataW* _dCreate(size_t capacity, const StubW& stub)
  {
    return fog_api.stringw_dCreateStubW(capacity, &stub);
  }

  static FOG_INLINE StringDataW* _dAdopt(void* address, size_t capacity)
  {
    return fog_api.stringw_dAdopt(address, capacity);
  }

  static FOG_INLINE StringDataW* _dAdopt(void* address, size_t capacity, const StubA& stub)
  {
    return fog_api.stringw_dAdoptStubA(address, capacity, &stub);
  }

  static FOG_INLINE StringDataW* _dAdopt(void* address, size_t capacity, const StubW& stub)
  {
    return fog_api.stringw_dAdoptStubW(address, capacity, &stub);
  }

  static FOG_INLINE StringDataW* _dRealloc(StringDataW* d, size_t capacity)
  {
    return fog_api.stringw_dRealloc(d, capacity);
  }

  static FOG_INLINE void _dFree(StringDataW* d)
  {
    fog_api.stringw_dFree(d);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(StringDataW)
};

// ============================================================================
// [Fog::StringT<>]
// ============================================================================

_FOG_CHAR_T(String)
_FOG_CHAR_T(StringData)
_FOG_CHAR_A(String)
_FOG_CHAR_A(StringData)
_FOG_CHAR_W(String)
_FOG_CHAR_W(StringData)

// @}

} // Fog namespace

// ============================================================================
// [Global Operator Overload]
// ============================================================================

static FOG_INLINE const Fog::StringA operator+(const Fog::StringA& a, const Fog::StringA& b) { return Fog::StringA(a, b); }
static FOG_INLINE const Fog::StringA operator+(const Fog::StringA& a, const Fog::StubA&   b) { return Fog::StringA(a, b); }
static FOG_INLINE const Fog::StringA operator+(const Fog::StringA& a, const char*         b) { return Fog::StringA(a, Fog::StubA(b)); }
static FOG_INLINE const Fog::StringA operator+(const Fog::StringA& a, const Fog::CharA&   b) { return Fog::StringA(a, Fog::StubA(&b, 1)); }
static FOG_INLINE const Fog::StringA operator+(const Fog::StringA& a, char                b) { return Fog::StringA(a, Fog::StubA(&b, 1)); }

static FOG_INLINE const Fog::StringA operator+(const Fog::StubA& a, const Fog::StringA& b) { return Fog::StringA(a, b); }
static FOG_INLINE const Fog::StringA operator+(const char*       a, const Fog::StringA& b) { return Fog::StringA(Fog::StubA(a), b); }
static FOG_INLINE const Fog::StringA operator+(const Fog::CharA& a, const Fog::StringA& b) { return Fog::StringA(Fog::StubA(&a, 1), b); }
static FOG_INLINE const Fog::StringA operator+(char              a, const Fog::StringA& b) { return Fog::StringA(Fog::StubA(&a, 1), b); }

#if defined(FOG_CC_HAS_RVALUE)
static FOG_INLINE Fog::StringA operator+(Fog::StringA&& a, const Fog::StringA& b) { a.append(b); return Fog::StringA(a._acquireData()); }
static FOG_INLINE Fog::StringA operator+(Fog::StringA&& a, const Fog::StubA& b) { a.append(b); return Fog::StringA(a._acquireData()); }
static FOG_INLINE Fog::StringA operator+(Fog::StringA&& a, const Fog::CharA& b) { a.append(b); return Fog::StringA(a._acquireData()); }
static FOG_INLINE Fog::StringA operator+(Fog::StringA&& a, char b) { a.append(b); return Fog::StringA(a._acquireData()); }
#endif // FOG_CC_HAS_RVALUE

static FOG_INLINE bool operator==(const Fog::StringA& a, const Fog::StringA& b) { return  a.eq(b); }
static FOG_INLINE bool operator!=(const Fog::StringA& a, const Fog::StringA& b) { return !a.eq(b); }
static FOG_INLINE bool operator<=(const Fog::StringA& a, const Fog::StringA& b) { return  a.compare(b) <= 0; }
static FOG_INLINE bool operator< (const Fog::StringA& a, const Fog::StringA& b) { return  a.compare(b) <  0; }
static FOG_INLINE bool operator>=(const Fog::StringA& a, const Fog::StringA& b) { return  a.compare(b) >= 0; }
static FOG_INLINE bool operator> (const Fog::StringA& a, const Fog::StringA& b) { return  a.compare(b) >  0; }

static FOG_INLINE bool operator==(const Fog::StringA& a, const Fog::StubA& b) { return  a.eq(b); }
static FOG_INLINE bool operator!=(const Fog::StringA& a, const Fog::StubA& b) { return !a.eq(b); }
static FOG_INLINE bool operator<=(const Fog::StringA& a, const Fog::StubA& b) { return  a.compare(b) <= 0; }
static FOG_INLINE bool operator< (const Fog::StringA& a, const Fog::StubA& b) { return  a.compare(b) <  0; }
static FOG_INLINE bool operator>=(const Fog::StringA& a, const Fog::StubA& b) { return  a.compare(b) >= 0; }
static FOG_INLINE bool operator> (const Fog::StringA& a, const Fog::StubA& b) { return  a.compare(b) >  0; }

static FOG_INLINE bool operator==(const Fog::StringA& a, const char* b) { return  a.eq(b); }
static FOG_INLINE bool operator!=(const Fog::StringA& a, const char* b) { return !a.eq(b); }
static FOG_INLINE bool operator<=(const Fog::StringA& a, const char* b) { return  a.compare(b) <= 0; }
static FOG_INLINE bool operator< (const Fog::StringA& a, const char* b) { return  a.compare(b) <  0; }
static FOG_INLINE bool operator>=(const Fog::StringA& a, const char* b) { return  a.compare(b) >= 0; }
static FOG_INLINE bool operator> (const Fog::StringA& a, const char* b) { return  a.compare(b) >  0; }

static FOG_INLINE bool operator==(const Fog::StubA& a, const Fog::StringA& b) { return  b.eq(a); }
static FOG_INLINE bool operator!=(const Fog::StubA& a, const Fog::StringA& b) { return !b.eq(a); }
static FOG_INLINE bool operator<=(const Fog::StubA& a, const Fog::StringA& b) { return  b.compare(a) >= 0; }
static FOG_INLINE bool operator< (const Fog::StubA& a, const Fog::StringA& b) { return  b.compare(a) >  0; }
static FOG_INLINE bool operator>=(const Fog::StubA& a, const Fog::StringA& b) { return  b.compare(a) <= 0; }
static FOG_INLINE bool operator> (const Fog::StubA& a, const Fog::StringA& b) { return  b.compare(a) <  0; }

static FOG_INLINE bool operator==(const char* a, const Fog::StringA& b) { return  b.eq(a); }
static FOG_INLINE bool operator!=(const char* a, const Fog::StringA& b) { return !b.eq(a); }
static FOG_INLINE bool operator<=(const char* a, const Fog::StringA& b) { return  b.compare(a) >= 0; }
static FOG_INLINE bool operator< (const char* a, const Fog::StringA& b) { return  b.compare(a) >  0; }
static FOG_INLINE bool operator>=(const char* a, const Fog::StringA& b) { return  b.compare(a) <= 0; }
static FOG_INLINE bool operator> (const char* a, const Fog::StringA& b) { return  b.compare(a) <  0; }

static FOG_INLINE Fog::StringW operator+(const Fog::StringW& a, const Fog::StringW& b) { return Fog::StringW(a, b); }
static FOG_INLINE Fog::StringW operator+(const Fog::StringW& a, const Fog::Ascii8& b) { return Fog::StringW(a, b); }
static FOG_INLINE Fog::StringW operator+(const Fog::StringW& a, const Fog::StubW& b) { return Fog::StringW(a, b); }
static FOG_INLINE Fog::StringW operator+(const Fog::StringW& a, const Fog::CharW& b) { return Fog::StringW(a, Fog::StubW(&b, 1)); }

static FOG_INLINE Fog::StringW operator+(const Fog::StubW& a, const Fog::StringW& b) { return Fog::StringW(a, b); }
static FOG_INLINE Fog::StringW operator+(const Fog::StubW& a, const Fog::StubW& b) { return Fog::StringW(a, b); }
static FOG_INLINE Fog::StringW operator+(const Fog::Ascii8& a, const Fog::StringW& b) { return Fog::StringW(a, b); }
static FOG_INLINE Fog::StringW operator+(const Fog::CharW& a, const Fog::StringW& b) { return Fog::StringW(Fog::StubW(&a, 1), b); }

#if defined(FOG_CC_HAS_RVALUE)
static FOG_INLINE Fog::StringW operator+(Fog::StringW&& a, const Fog::StringW& b) { a.append(b); return Fog::StringW(a._acquireData()); }
static FOG_INLINE Fog::StringW operator+(Fog::StringW&& a, const Fog::StubW& b) { a.append(b); return Fog::StringW(a._acquireData()); }
static FOG_INLINE Fog::StringW operator+(Fog::StringW&& a, const Fog::Ascii8& b) { a.append(b); return Fog::StringW(a._acquireData()); }
static FOG_INLINE Fog::StringW operator+(Fog::StringW&& a, const Fog::CharW& b) { a.append(b); return Fog::StringW(a._acquireData()); }
#endif // FOG_CC_HAS_RVALUE

static FOG_INLINE bool operator==(const Fog::StringW& a, const Fog::StringW& b) { return  a.eq(b); }
static FOG_INLINE bool operator!=(const Fog::StringW& a, const Fog::StringW& b) { return !a.eq(b); }
static FOG_INLINE bool operator<=(const Fog::StringW& a, const Fog::StringW& b) { return  a.compare(b) <= 0; }
static FOG_INLINE bool operator< (const Fog::StringW& a, const Fog::StringW& b) { return  a.compare(b) <  0; }
static FOG_INLINE bool operator>=(const Fog::StringW& a, const Fog::StringW& b) { return  a.compare(b) >= 0; }
static FOG_INLINE bool operator> (const Fog::StringW& a, const Fog::StringW& b) { return  a.compare(b) >  0; }

static FOG_INLINE bool operator==(const Fog::StringW& a, const Fog::Ascii8& b) { return  a.eq(b); }
static FOG_INLINE bool operator!=(const Fog::StringW& a, const Fog::Ascii8& b) { return !a.eq(b); }
static FOG_INLINE bool operator<=(const Fog::StringW& a, const Fog::Ascii8& b) { return  a.compare(b) <= 0; }
static FOG_INLINE bool operator< (const Fog::StringW& a, const Fog::Ascii8& b) { return  a.compare(b) <  0; }
static FOG_INLINE bool operator>=(const Fog::StringW& a, const Fog::Ascii8& b) { return  a.compare(b) >= 0; }
static FOG_INLINE bool operator> (const Fog::StringW& a, const Fog::Ascii8& b) { return  a.compare(b) >  0; }

static FOG_INLINE bool operator==(const Fog::Ascii8& a, const Fog::StringW& b) { return  b.eq(a); }
static FOG_INLINE bool operator!=(const Fog::Ascii8& a, const Fog::StringW& b) { return !b.eq(a); }
static FOG_INLINE bool operator<=(const Fog::Ascii8& a, const Fog::StringW& b) { return  b.compare(a) >= 0; }
static FOG_INLINE bool operator< (const Fog::Ascii8& a, const Fog::StringW& b) { return  b.compare(a) >  0; }
static FOG_INLINE bool operator>=(const Fog::Ascii8& a, const Fog::StringW& b) { return  b.compare(a) <= 0; }
static FOG_INLINE bool operator> (const Fog::Ascii8& a, const Fog::StringW& b) { return  b.compare(a) <  0; }

static FOG_INLINE bool operator==(const Fog::StringW& a, const Fog::StubW& b) { return  a.eq(b); }
static FOG_INLINE bool operator!=(const Fog::StringW& a, const Fog::StubW& b) { return !a.eq(b); }
static FOG_INLINE bool operator<=(const Fog::StringW& a, const Fog::StubW& b) { return  a.compare(b) <= 0; }
static FOG_INLINE bool operator< (const Fog::StringW& a, const Fog::StubW& b) { return  a.compare(b) <  0; }
static FOG_INLINE bool operator>=(const Fog::StringW& a, const Fog::StubW& b) { return  a.compare(b) >= 0; }
static FOG_INLINE bool operator> (const Fog::StringW& a, const Fog::StubW& b) { return  a.compare(b) >  0; }

static FOG_INLINE bool operator==(const Fog::StubW& a, const Fog::StringW& b) { return  b.eq(a); }
static FOG_INLINE bool operator!=(const Fog::StubW& a, const Fog::StringW& b) { return !b.eq(a); }
static FOG_INLINE bool operator<=(const Fog::StubW& a, const Fog::StringW& b) { return  b.compare(a) >= 0; }
static FOG_INLINE bool operator< (const Fog::StubW& a, const Fog::StringW& b) { return  b.compare(a) >  0; }
static FOG_INLINE bool operator>=(const Fog::StubW& a, const Fog::StringW& b) { return  b.compare(a) <= 0; }
static FOG_INLINE bool operator> (const Fog::StubW& a, const Fog::StringW& b) { return  b.compare(a) <  0; }

// ============================================================================
// [Include Specialized Containers]
// ============================================================================

#if defined(_FOG_CORE_TOOLS_LIST_H)
# include <Fog/Core/Tools/ListString.h>
#endif // _FOG_CORE_TOOLS_LIST_H

#if defined(_FOG_CORE_TOOLS_HASH_H)
# include <Fog/Core/Tools/HashString.h>
#endif // _FOG_CORE_TOOLS_HASH_H

// [Guard]
#endif // _FOG_CORE_TOOLS_STRING_H
