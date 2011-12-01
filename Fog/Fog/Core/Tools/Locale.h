// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_LOCALE_H
#define _FOG_CORE_TOOLS_LOCALE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::LocaleData]
// ============================================================================

struct FOG_NO_EXPORT LocaleData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE LocaleData* addRef() const
  {
    reference.inc();
    return const_cast<LocaleData*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      fog_api.locale_dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // ${VAR:BEGIN}
  //
  // This data-object is binary compatible with the VarData header in the first
  // form called - "implicitly shared class". The members must be binary
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
  // +==============+============+============================================+
  //
  // ${VAR:END}

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Variable type and flags.
  uint32_t vType;

#if FOG_ARCH_BITS >= 64
  //! @brief Padding (0.32).
  uint32_t padding0_32;
#endif // FOG_ARCH_BITS >= 64

  //! @brief Locale string.
  Static<StringW> name;

  //! @brief Locale UTF-32 characters.
  CharW data[LOCALE_CHAR_COUNT];
};

// ============================================================================
// [Fog::Locale]
// ============================================================================

//! @brief Locale.
//!
//! Use @c Locale::user() method to get the current thread locale or
//! @c Locale::posix() method to get the universal POSIX locale.
struct FOG_NO_EXPORT Locale
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Locale()
  {
    fog_api.locale_ctor(this);
  }

  FOG_INLINE Locale(const Locale& other)
  {
    fog_api.locale_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE Locale(Locale&& other) : _d(other._d) { other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  explicit FOG_INLINE Locale(const StringW& localeName)
  {
    fog_api.locale_ctorString(this, &localeName);
  }

  explicit FOG_INLINE Locale(LocaleData* d) : _d(d)
  {
  }

  FOG_INLINE ~Locale()
  {
    fog_api.locale_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getReference().
  FOG_INLINE size_t getReference() const
  {
    return _d->reference.get();
  }

  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const
  {
    return getReference() == 1;
  }

  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach()
  {
    return isDetached() ? (err_t)ERR_OK : _detach();
  }

  //! @copydoc Doxygen::Implicit::_detach().
  FOG_INLINE err_t _detach()
  {
    return fog_api.locale_detach(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get locale string.
  FOG_INLINE const StringW& getName() const
  {
    return _d->name;
  }

  //! @brief Get locale character.
  FOG_INLINE CharW getChar(uint32_t id) const
  {
    FOG_ASSERT_X(id < LOCALE_CHAR_COUNT, "Fog::Locale::getChar() - Id out of range");
    return _d->data[id];
  }

  FOG_INLINE err_t setChar(uint32_t id, CharW ch)
  {
    return fog_api.locale_setChar(this, id, ch);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::reset().
  FOG_INLINE void reset()
  {
    fog_api.locale_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Create]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t create(const StringW& name)
  {
    return fog_api.locale_create(this, &name);
  }

  // --------------------------------------------------------------------------
  // [ConvertInt]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t convertInt(StringW& dst, int8_t   n) const { return fog_api.stringw_opI32Ex(&dst, CONTAINER_OP_REPLACE, n, NULL, this); }
  FOG_INLINE err_t convertInt(StringW& dst, uint8_t  n) const { return fog_api.stringw_opU32Ex(&dst, CONTAINER_OP_REPLACE, n, NULL, this); }
  FOG_INLINE err_t convertInt(StringW& dst, int16_t  n) const { return fog_api.stringw_opI32Ex(&dst, CONTAINER_OP_REPLACE, n, NULL, this); }
  FOG_INLINE err_t convertInt(StringW& dst, uint16_t n) const { return fog_api.stringw_opU32Ex(&dst, CONTAINER_OP_REPLACE, n, NULL, this); }
  FOG_INLINE err_t convertInt(StringW& dst, int32_t  n) const { return fog_api.stringw_opI32Ex(&dst, CONTAINER_OP_REPLACE, n, NULL, this); }
  FOG_INLINE err_t convertInt(StringW& dst, uint32_t n) const { return fog_api.stringw_opU32Ex(&dst, CONTAINER_OP_REPLACE, n, NULL, this); }
  FOG_INLINE err_t convertInt(StringW& dst, int64_t  n) const { return fog_api.stringw_opI64Ex(&dst, CONTAINER_OP_REPLACE, n, NULL, this); }
  FOG_INLINE err_t convertInt(StringW& dst, uint64_t n) const { return fog_api.stringw_opU64Ex(&dst, CONTAINER_OP_REPLACE, n, NULL, this); }

  FOG_INLINE err_t convertInt(StringW& dst, int8_t   n, const FormatInt& fmt) const { return fog_api.stringw_opI32Ex(&dst, CONTAINER_OP_REPLACE, n, &fmt, this); }
  FOG_INLINE err_t convertInt(StringW& dst, uint8_t  n, const FormatInt& fmt) const { return fog_api.stringw_opU32Ex(&dst, CONTAINER_OP_REPLACE, n, &fmt, this); }
  FOG_INLINE err_t convertInt(StringW& dst, int16_t  n, const FormatInt& fmt) const { return fog_api.stringw_opI32Ex(&dst, CONTAINER_OP_REPLACE, n, &fmt, this); }
  FOG_INLINE err_t convertInt(StringW& dst, uint16_t n, const FormatInt& fmt) const { return fog_api.stringw_opU32Ex(&dst, CONTAINER_OP_REPLACE, n, &fmt, this); }
  FOG_INLINE err_t convertInt(StringW& dst, int32_t  n, const FormatInt& fmt) const { return fog_api.stringw_opI32Ex(&dst, CONTAINER_OP_REPLACE, n, &fmt, this); }
  FOG_INLINE err_t convertInt(StringW& dst, uint32_t n, const FormatInt& fmt) const { return fog_api.stringw_opU32Ex(&dst, CONTAINER_OP_REPLACE, n, &fmt, this); }
  FOG_INLINE err_t convertInt(StringW& dst, int64_t  n, const FormatInt& fmt) const { return fog_api.stringw_opI64Ex(&dst, CONTAINER_OP_REPLACE, n, &fmt, this); }
  FOG_INLINE err_t convertInt(StringW& dst, uint64_t n, const FormatInt& fmt) const { return fog_api.stringw_opU64Ex(&dst, CONTAINER_OP_REPLACE, n, &fmt, this); }

  // --------------------------------------------------------------------------
  // [ConvertReal]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t convertReal(StringW& dst, float d) { return fog_api.stringw_opDoubleEx(&dst, CONTAINER_OP_REPLACE, d, NULL, this); }
  FOG_INLINE err_t convertReal(StringW& dst, double d) { return fog_api.stringw_opDoubleEx(&dst, CONTAINER_OP_REPLACE, d, NULL, this); }

  FOG_INLINE err_t convertReal(StringW& dst, float d, const FormatReal& fmt) { return fog_api.stringw_opDoubleEx(&dst, CONTAINER_OP_REPLACE, d, &fmt, this); }
  FOG_INLINE err_t convertReal(StringW& dst, double d, const FormatReal& fmt) { return fog_api.stringw_opDoubleEx(&dst, CONTAINER_OP_REPLACE, d, &fmt, this); }

  // --------------------------------------------------------------------------
  // [format]
  // --------------------------------------------------------------------------

  FOG_NO_INLINE err_t format(StringW& dst, const char* fmt, ...)
  {
    StubA fmtStub(fmt, DETECT_LENGTH);

    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringw_opVFormatStubA(&dst, CONTAINER_OP_REPLACE, &fmtStub, NULL, this, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t format_c(StringW& dst, const char* fmt, const TextCodec& tc, ...)
  {
    StubA fmtStub(fmt, DETECT_LENGTH);

    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringw_opVFormatStubA(&dst, CONTAINER_OP_REPLACE, &fmtStub, &tc, this, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t format(StringW& dst, const Ascii8& fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringw_opVFormatStubA(&dst, CONTAINER_OP_REPLACE, &fmt, NULL, this, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t format_c(StringW& dst, const StubA& fmt, const TextCodec& tc, ...)
  {
    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringw_opVFormatStubA(&dst, CONTAINER_OP_REPLACE, &fmt, &tc, this, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t format(StringW& dst, const StubW& fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringw_opVFormatStubW(&dst, CONTAINER_OP_REPLACE, &fmt, NULL, this, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t format_c(StringW& dst, const StubW& fmt, const TextCodec& tc, ...)
  {
    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringw_opVFormatStubW(&dst, CONTAINER_OP_REPLACE, &fmt, &tc, this, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t format(StringW& dst, const StringW& fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringw_opVFormatStringW(&dst, CONTAINER_OP_REPLACE, &fmt, NULL, this, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t format_c(StringW& dst, const StringW& fmt, const TextCodec& tc, ...)
  {
    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringw_opVFormatStringW(&dst, CONTAINER_OP_REPLACE, &fmt, &tc, this, ap);
    va_end(ap);

    return err;
  }

  // --------------------------------------------------------------------------
  // [VFormat]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t vFormat(StringW& dst, const char* fmt, va_list ap)
  {
    StubA fmtA(fmt, DETECT_LENGTH);
    return fog_api.stringw_opVFormatStubA(&dst, CONTAINER_OP_REPLACE, &fmtA, NULL, this, ap);
  }

  FOG_INLINE err_t vFormat_c(StringW& dst, const char* fmt, const TextCodec& tc, va_list ap)
  {
    StubA fmtA(fmt, DETECT_LENGTH);
    return fog_api.stringw_opVFormatStubA(&dst, CONTAINER_OP_REPLACE, &fmtA, &tc, this, ap);
  }

  FOG_INLINE err_t vFormat(StringW& dst, const Ascii8& fmt, va_list ap)
  {
    return fog_api.stringw_opVFormatStubA(&dst, CONTAINER_OP_REPLACE, &fmt, NULL, this, ap);
  }

  FOG_INLINE err_t vFormat_c(StringW& dst, const StubA& fmt, const TextCodec& tc, va_list ap)
  {
    return fog_api.stringw_opVFormatStubA(&dst, CONTAINER_OP_REPLACE, &fmt, &tc, this, ap);
  }

  FOG_INLINE err_t vFormat(StringW& dst, const StringW& fmt, va_list ap)
  {
    return fog_api.stringw_opVFormatStringW(&dst, CONTAINER_OP_REPLACE, &fmt, NULL, this, ap);
  }

  FOG_INLINE err_t vFormat_c(StringW& dst, const StringW& fmt, const TextCodec& tc, va_list ap)
  {
    return fog_api.stringw_opVFormatStringW(&dst, CONTAINER_OP_REPLACE, &fmt, &tc, this, ap);
  }

  // --------------------------------------------------------------------------
  // [AppendInt]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t appendInt(StringW& dst, int8_t   n) const { return fog_api.stringw_opI32Ex(&dst, CONTAINER_OP_APPEND, n, NULL, this); }
  FOG_INLINE err_t appendInt(StringW& dst, uint8_t  n) const { return fog_api.stringw_opU32Ex(&dst, CONTAINER_OP_APPEND, n, NULL, this); }
  FOG_INLINE err_t appendInt(StringW& dst, int16_t  n) const { return fog_api.stringw_opI32Ex(&dst, CONTAINER_OP_APPEND, n, NULL, this); }
  FOG_INLINE err_t appendInt(StringW& dst, uint16_t n) const { return fog_api.stringw_opU32Ex(&dst, CONTAINER_OP_APPEND, n, NULL, this); }
  FOG_INLINE err_t appendInt(StringW& dst, int32_t  n) const { return fog_api.stringw_opI32Ex(&dst, CONTAINER_OP_APPEND, n, NULL, this); }
  FOG_INLINE err_t appendInt(StringW& dst, uint32_t n) const { return fog_api.stringw_opU32Ex(&dst, CONTAINER_OP_APPEND, n, NULL, this); }
  FOG_INLINE err_t appendInt(StringW& dst, int64_t  n) const { return fog_api.stringw_opI64Ex(&dst, CONTAINER_OP_APPEND, n, NULL, this); }
  FOG_INLINE err_t appendInt(StringW& dst, uint64_t n) const { return fog_api.stringw_opU64Ex(&dst, CONTAINER_OP_APPEND, n, NULL, this); }

  FOG_INLINE err_t appendInt(StringW& dst, int8_t   n, const FormatInt& fmt) const { return fog_api.stringw_opI32Ex(&dst, CONTAINER_OP_APPEND, n, &fmt, this); }
  FOG_INLINE err_t appendInt(StringW& dst, uint8_t  n, const FormatInt& fmt) const { return fog_api.stringw_opU32Ex(&dst, CONTAINER_OP_APPEND, n, &fmt, this); }
  FOG_INLINE err_t appendInt(StringW& dst, int16_t  n, const FormatInt& fmt) const { return fog_api.stringw_opI32Ex(&dst, CONTAINER_OP_APPEND, n, &fmt, this); }
  FOG_INLINE err_t appendInt(StringW& dst, uint16_t n, const FormatInt& fmt) const { return fog_api.stringw_opU32Ex(&dst, CONTAINER_OP_APPEND, n, &fmt, this); }
  FOG_INLINE err_t appendInt(StringW& dst, int32_t  n, const FormatInt& fmt) const { return fog_api.stringw_opI32Ex(&dst, CONTAINER_OP_APPEND, n, &fmt, this); }
  FOG_INLINE err_t appendInt(StringW& dst, uint32_t n, const FormatInt& fmt) const { return fog_api.stringw_opU32Ex(&dst, CONTAINER_OP_APPEND, n, &fmt, this); }
  FOG_INLINE err_t appendInt(StringW& dst, int64_t  n, const FormatInt& fmt) const { return fog_api.stringw_opI64Ex(&dst, CONTAINER_OP_APPEND, n, &fmt, this); }
  FOG_INLINE err_t appendInt(StringW& dst, uint64_t n, const FormatInt& fmt) const { return fog_api.stringw_opU64Ex(&dst, CONTAINER_OP_APPEND, n, &fmt, this); }

  // --------------------------------------------------------------------------
  // [AppendReal]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t appendReal(StringW& dst, float d) { return fog_api.stringw_opDoubleEx(&dst, CONTAINER_OP_APPEND, d, NULL, this); }
  FOG_INLINE err_t appendReal(StringW& dst, double d) { return fog_api.stringw_opDoubleEx(&dst, CONTAINER_OP_APPEND, d, NULL, this); }

  FOG_INLINE err_t appendReal(StringW& dst, float d, const FormatReal& fmt) { return fog_api.stringw_opDoubleEx(&dst, CONTAINER_OP_APPEND, d, &fmt, this); }
  FOG_INLINE err_t appendReal(StringW& dst, double d, const FormatReal& fmt) { return fog_api.stringw_opDoubleEx(&dst, CONTAINER_OP_APPEND, d, &fmt, this); }

  // --------------------------------------------------------------------------
  // [AppendFormat]
  // --------------------------------------------------------------------------

  FOG_NO_INLINE err_t appendFormat(StringW& dst, const char* fmt, ...)
  {
    StubA fmtStub(fmt, DETECT_LENGTH);

    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringw_opVFormatStubA(&dst, CONTAINER_OP_APPEND, &fmtStub, NULL, this, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t appendFormat_c(StringW& dst, const char* fmt, const TextCodec& tc, ...)
  {
    StubA fmtStub(fmt, DETECT_LENGTH);

    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringw_opVFormatStubA(&dst, CONTAINER_OP_APPEND, &fmtStub, &tc, this, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t appendFormat(StringW& dst, const Ascii8& fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringw_opVFormatStubA(&dst, CONTAINER_OP_APPEND, &fmt, NULL, this, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t appendFormat_c(StringW& dst, const StubA& fmt, const TextCodec& tc, ...)
  {
    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringw_opVFormatStubA(&dst, CONTAINER_OP_APPEND, &fmt, &tc, this, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t appendFormat(StringW& dst, const StubW& fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringw_opVFormatStubW(&dst, CONTAINER_OP_APPEND, &fmt, NULL, this, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t appendFormat_c(StringW& dst, const StubW& fmt, const TextCodec& tc, ...)
  {
    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringw_opVFormatStubW(&dst, CONTAINER_OP_APPEND, &fmt, &tc, this, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t appendFormat(StringW& dst, const StringW& fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);
    err_t err = fog_api.stringw_opVFormatStringW(&dst, CONTAINER_OP_APPEND, &fmt, NULL, this, ap);
    va_end(ap);

    return err;
  }

  FOG_NO_INLINE err_t appendFormat_c(StringW& dst, const StringW& fmt, const TextCodec& tc, ...)
  {
    va_list ap;
    va_start(ap, tc);
    err_t err = fog_api.stringw_opVFormatStringW(&dst, CONTAINER_OP_APPEND, &fmt, &tc, this, ap);
    va_end(ap);

    return err;
  }

  // --------------------------------------------------------------------------
  // [AppendVFormat]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t appendVFormat(StringW& dst, const char* fmt, va_list ap)
  {
    StubA fmtA(fmt, DETECT_LENGTH);
    return fog_api.stringw_opVFormatStubA(&dst, CONTAINER_OP_APPEND, &fmtA, NULL, this, ap);
  }

  FOG_INLINE err_t appendVFormat_c(StringW& dst, const char* fmt, const TextCodec& tc, va_list ap)
  {
    StubA fmtA(fmt, DETECT_LENGTH);
    return fog_api.stringw_opVFormatStubA(&dst, CONTAINER_OP_APPEND, &fmtA, &tc, this, ap);
  }

  FOG_INLINE err_t appendVFormat(StringW& dst, const Ascii8& fmt, va_list ap)
  {
    return fog_api.stringw_opVFormatStubA(&dst, CONTAINER_OP_APPEND, &fmt, NULL, this, ap);
  }

  FOG_INLINE err_t appendVFormat_c(StringW& dst, const StubA& fmt, const TextCodec& tc, va_list ap)
  {
    return fog_api.stringw_opVFormatStubA(&dst, CONTAINER_OP_APPEND, &fmt, &tc, this, ap);
  }

  FOG_INLINE err_t appendVFormat(StringW& dst, const StringW& fmt, va_list ap)
  {
    return fog_api.stringw_opVFormatStringW(&dst, CONTAINER_OP_APPEND, &fmt, NULL, this, ap);
  }

  FOG_INLINE err_t appendVFormat_c(StringW& dst, const StringW& fmt, const TextCodec& tc, va_list ap)
  {
    return fog_api.stringw_opVFormatStringW(&dst, CONTAINER_OP_APPEND, &fmt, &tc, this, ap);
  }

  // --------------------------------------------------------------------------
  // [ParseInt]
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [ParseFloat / ParseDouble]
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Locale& other) const
  {
    return fog_api.locale_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Locale& operator=(const Locale& other)
  {
    fog_api.locale_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const Locale& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const Locale& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Instance]
  // --------------------------------------------------------------------------

  static FOG_INLINE const Locale& posix() { return *fog_api.locale_oPosix; }
  static FOG_INLINE const Locale& user() { return *fog_api.locale_oUser; }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const Locale* a, const Locale* b)
  {
    return fog_api.locale_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.locale_eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(LocaleData)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_LOCALE_H
