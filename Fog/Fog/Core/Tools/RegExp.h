// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_REGEXP_H
#define _FOG_CORE_TOOLS_REGEXP_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::RegExpDataA]
// ============================================================================

struct FOG_NO_EXPORT RegExpDataA
{
  // --------------------------------------------------------------------------
  // [Typedefs]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *Destroy)(RegExpDataA* d);
  typedef bool (FOG_CDECL *Match)(RegExpDataA* d, const char* sData, size_t sLength, const Range* sRange, Range* out);

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE RegExpDataA* addRef() const
  {
    reference.inc();
    return const_cast<RegExpDataA*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      destroy(this);
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

  //! @brief Type (see @ref REGEXP_TYPE).
  uint8_t type;
  //! @brief Internal type.
  uint8_t internalType;
  //! @brief Case sensitivity (see @ref CASE_SENSITIVITY).
  uint8_t caseSensitivity;
  //! @brief Reserved for future use.
  uint8_t reserved;

  //! @brief Destructor.
  Destroy destroy;
  //! @brief Match.
  Match match;

  //! @brief Pattern.
  Static<StringA> pattern;
  //! @brief Fixed length.
  size_t fixedLength;
};

// ============================================================================
// [Fog::RegExpDataW]
// ============================================================================

struct FOG_NO_EXPORT RegExpDataW
{
  // --------------------------------------------------------------------------
  // [Typedefs]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *Destroy)(RegExpDataW* d);
  typedef bool (FOG_CDECL *Match)(RegExpDataW* d, const CharW* sData, size_t sLength, const Range* sRange, Range* out);

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE RegExpDataW* addRef() const
  {
    reference.inc();
    return const_cast<RegExpDataW*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      destroy(this);
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

  //! @brief Type (see @ref REGEXP_TYPE).
  uint8_t type;
  //! @brief Internal type.
  uint8_t internalType;
  //! @brief Case sensitivity (see @ref CASE_SENSITIVITY).
  uint8_t caseSensitivity;
  //! @brief Reserved for future use.
  uint8_t reserved;

  //! @brief Destructor.
  Destroy destroy;
  //! @brief Match.
  Match match;

  //! @brief Pattern.
  Static<StringW> pattern;
  //! @brief Fixed length.
  size_t fixedLength;
};

// ============================================================================
// [Fog::RegExpA]
// ============================================================================

struct FOG_NO_EXPORT RegExpA
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RegExpA()
  {
    fog_api.regexpa_ctor(this);
  }

  FOG_INLINE RegExpA(const RegExpA& other)
  {
    fog_api.regexpa_ctorCopy(this, &other);
  }

  explicit FOG_INLINE RegExpA(RegExpDataA* d) : _d(d)
  {
  }

  FOG_INLINE ~RegExpA()
  {
    fog_api.regexpa_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const
  {
    return _d->reference.get();
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getType() const { return _d->type; }
  FOG_INLINE uint32_t getCaseSensitivity() const { return _d->caseSensitivity; }
  FOG_INLINE const StringA& getPattern() const { return _d->pattern; }
  FOG_INLINE bool hasFixedLength() const { return _d->fixedLength != INVALID_INDEX; }
  FOG_INLINE size_t getFixedLength() const { return _d->fixedLength; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    fog_api.regexpa_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Create]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t create(const StubA& pattern, uint32_t type, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.regexpa_createStubA(this, &pattern, type, cs);
  }

  FOG_INLINE err_t create(const StringA& pattern, uint32_t type, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.regexpa_createStringA(this, &pattern, type, cs);
  }

  // --------------------------------------------------------------------------
  // [IndexIn]
  // --------------------------------------------------------------------------

  FOG_INLINE Range indexIn(const char* sData, size_t sLength) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpa_indexIn(this, sData, sLength, NULL, &result);
    return result;
  }

  FOG_INLINE Range indexIn(const char* sData, size_t sLength, const Range& range) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpa_indexIn(this, sData, sLength, &range, &result);
    return result;
  }

  FOG_INLINE Range indexIn(const StubA& src) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpa_indexIn(this, src.getData(), src.getLength(), NULL, &result);
    return result;
  }

  FOG_INLINE Range indexIn(const StubA& src, const Range& range) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpa_indexIn(this, src.getData(), src.getLength(), &range, &result);
    return result;
  }

  FOG_INLINE Range indexIn(const StringA& src) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpa_indexIn(this, src.getData(), src.getLength(), NULL, &result);
    return result;
  }

  FOG_INLINE Range indexIn(const StringA& src, const Range& range) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpa_indexIn(this, src.getData(), src.getLength(), &range, &result);
    return result;
  }

  // --------------------------------------------------------------------------
  // [LastIndexIn]
  // --------------------------------------------------------------------------

  FOG_INLINE Range lastIndexIn(const char* sData, size_t sLength) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpa_lastIndexIn(this, sData, sLength, NULL, &result);
    return result;
  }

  FOG_INLINE Range lastIndexIn(const char* sData, size_t sLength, const Range& range) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpa_lastIndexIn(this, sData, sLength, &range, &result);
    return result;
  }

  FOG_INLINE Range lastIndexIn(const StubA& src) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpa_lastIndexIn(this, src.getData(), src.getLength(), NULL, &result);
    return result;
  }

  FOG_INLINE Range lastIndexIn(const StubA& src, const Range& range) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpa_lastIndexIn(this, src.getData(), src.getLength(), &range, &result);
    return result;
  }

  FOG_INLINE Range lastIndexIn(const StringA& src) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpa_lastIndexIn(this, src.getData(), src.getLength(), NULL, &result);
    return result;
  }

  FOG_INLINE Range lastIndexIn(const StringA& src, const Range& range) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpa_lastIndexIn(this, src.getData(), src.getLength(), &range, &result);
    return result;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE RegExpA& operator=(const RegExpA& other)
  {
    fog_api.regexpa_copy(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(RegExpDataA)
};

// ============================================================================
// [Fog::RegExpW]
// ============================================================================

struct FOG_NO_EXPORT RegExpW
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RegExpW()
  {
    fog_api.regexpw_ctor(this);
  }

  FOG_INLINE RegExpW(const RegExpW& other)
  {
    fog_api.regexpw_ctorCopy(this, &other);
  }

  explicit FOG_INLINE RegExpW(RegExpDataW* d) : _d(d)
  {
  }

  FOG_INLINE ~RegExpW()
  {
    fog_api.regexpw_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const
  {
    return _d->reference.get();
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getType() const { return _d->type; }
  FOG_INLINE uint32_t getCaseSensitivity() const { return _d->caseSensitivity; }
  FOG_INLINE const StringW& getPattern() const { return _d->pattern; }
  FOG_INLINE bool hasFixedLength() const { return _d->fixedLength != INVALID_INDEX; }
  FOG_INLINE size_t getFixedLength() const { return _d->fixedLength; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    fog_api.regexpw_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Create]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t create(const Ascii8& pattern, uint32_t type, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.regexpw_createStubA(this, &pattern, type, cs);
  }

  FOG_INLINE err_t create(const StubW& pattern, uint32_t type, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.regexpw_createStubW(this, &pattern, type, cs);
  }

  FOG_INLINE err_t create(const StringW& pattern, uint32_t type, uint32_t cs = CASE_SENSITIVE)
  {
    return fog_api.regexpw_createStringW(this, &pattern, type, cs);
  }

  // --------------------------------------------------------------------------
  // [IndexIn]
  // --------------------------------------------------------------------------

  FOG_INLINE Range indexIn(const CharW* sData, size_t sLength) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpw_indexIn(this, sData, sLength, NULL, &result);
    return result;
  }

  FOG_INLINE Range indexIn(const CharW* sData, size_t sLength, const Range& range) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpw_indexIn(this, sData, sLength, &range, &result);
    return result;
  }

  FOG_INLINE Range indexIn(const StubW& src) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpw_indexIn(this, src.getData(), src.getLength(), NULL, &result);
    return result;
  }

  FOG_INLINE Range indexIn(const StubW& src, const Range& range) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpw_indexIn(this, src.getData(), src.getLength(), &range, &result);
    return result;
  }

  FOG_INLINE Range indexIn(const StringW& src) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpw_indexIn(this, src.getData(), src.getLength(), NULL, &result);
    return result;
  }

  FOG_INLINE Range indexIn(const StringW& src, const Range& range) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpw_indexIn(this, src.getData(), src.getLength(), &range, &result);
    return result;
  }

  // --------------------------------------------------------------------------
  // [LastIndexIn]
  // --------------------------------------------------------------------------

  FOG_INLINE Range lastIndexIn(const CharW* sData, size_t sLength) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpw_lastIndexIn(this, sData, sLength, NULL, &result);
    return result;
  }

  FOG_INLINE Range lastIndexIn(const CharW* sData, size_t sLength, const Range& range) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpw_lastIndexIn(this, sData, sLength, &range, &result);
    return result;
  }

  FOG_INLINE Range lastIndexIn(const StubW& src) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpw_lastIndexIn(this, src.getData(), src.getLength(), NULL, &result);
    return result;
  }

  FOG_INLINE Range lastIndexIn(const StubW& src, const Range& range) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpw_lastIndexIn(this, src.getData(), src.getLength(), &range, &result);
    return result;
  }

  FOG_INLINE Range lastIndexIn(const StringW& src) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpw_lastIndexIn(this, src.getData(), src.getLength(), NULL, &result);
    return result;
  }

  FOG_INLINE Range lastIndexIn(const StringW& src, const Range& range) const
  {
    Range result(UNINITIALIZED);
    fog_api.regexpw_lastIndexIn(this, src.getData(), src.getLength(), &range, &result);
    return result;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE RegExpW& operator=(const RegExpW& other)
  {
    fog_api.regexpw_copy(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(RegExpDataW)
};

// ============================================================================
// [Fog::RegExpT<> / Fog::RegExpDataT<>]
// ============================================================================

_FOG_CHAR_T(RegExp)
_FOG_CHAR_T(RegExpData)
_FOG_CHAR_A(RegExp)
_FOG_CHAR_A(RegExpData)
_FOG_CHAR_W(RegExp)
_FOG_CHAR_W(RegExpData)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_REGEXP_H
