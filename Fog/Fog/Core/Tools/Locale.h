// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_LOCALE_H
#define _FOG_CORE_TOOLS_LOCALE_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Swap.h>
#include <Fog/Core/Global/TypeInfo.h>
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
  //! @brief Reference count.
  mutable Atomic<size_t> refCount;

  //! @brief Locale string.
  String locale;

  //! @brief Locale UTF-32 characters.
  Char data[LOCALE_CHAR_COUNT];
};

// ============================================================================
// [Fog::Locale]
// ============================================================================

//! @brief Locale.
//!
//! Locales are useful when translating numbers or floats from / to strings.
//! In some cases there are needed that numbers and strings are converted to
//! user native locale, this happen in GUI input components, but in some
//! situations there is needed to use exactly one locale. For example
//! configuration files or platform indenpendent documents should use POSIX
//! locale only.
//!
//! Use @c Locale::user() method to get the user locale and @c Locale::posix()
//! method to get the posix (C) locale.
struct FOG_API Locale
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Locale();
  Locale(const Locale& other);
  explicit Locale(const String& name);
  explicit FOG_INLINE Locale(LocaleData* d) { _d = d; }
  ~Locale();

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getReference().
  FOG_INLINE size_t getReference() const { return _d->refCount.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return getReference() == 1; }
  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach() { return isDetached() ? (err_t)ERR_OK : _detach(); }
  //! @copydoc Doxygen::Implicit::_detach().
  err_t _detach();

  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isNull() const { return _d == _dnull->_d; }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get locale string.
  FOG_INLINE const String& getLocale() const { return _d->locale; }

  //! @brief Get locale character.
  FOG_INLINE Char getChar(uint32_t id) const
  {
    FOG_ASSERT_X(id < LOCALE_CHAR_COUNT, "Fog::Locale::getChar() - Id out of range");
    return _d->data[id];
  }

  err_t setChar(uint32_t id, Char uc);

  // --------------------------------------------------------------------------
  // [Create]
  // --------------------------------------------------------------------------

  err_t create(const String& locale);

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::reset().
  void reset();

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  Locale& operator=(const Locale& other);

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static Locale* _dnull;
  static Locale* _dposix;
  static Locale* _duser;

  static FOG_INLINE const Locale& null() { return *_dnull; }
  static FOG_INLINE const Locale& posix() { return *_dposix; }
  static FOG_INLINE const Locale& user() { return *_duser; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(LocaleData)
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::Locale, Fog::TYPEINFO_MOVABLE)

// ============================================================================
// [Fog::Swap]
// ============================================================================

_FOG_SWAP_D(Fog::Locale)

// [Guard]
#endif // _FOG_CORE_TOOLS_LOCALE_H
