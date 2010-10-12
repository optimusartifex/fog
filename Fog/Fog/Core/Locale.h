// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_LOCALE_H
#define _FOG_CORE_LOCALE_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/Char.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/String.h>
#include <Fog/Core/TypeInfo.h>

namespace Fog {

//! @addtogroup Fog_Core_Localization
//! @{

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
//! To get user locale use @c Locale::user() method and to get built-in posix
//! locale use @c Locale::posix().
struct FOG_API Locale
{
  // [Data]

  struct FOG_API Data
  {
    // [Construction / Destruction]

    Data();
    Data(const Data& other);
    ~Data();

    // [Ref / Deref]

    FOG_INLINE Data* ref() const
    {
      refCount.inc();
      return const_cast<Data*>(this);
    }

    void deref();

    // [Members]

    mutable Atomic<sysuint_t> refCount;
    String name;
    uint32_t data[LOCALE_CHAR_INVALID];
  };


  static Static<Data> _dnull;
  static Static<Data> sharedPosix;
  static Static<Data> sharedUser;

  static Locale* _dnullLocale;
  static Locale* sharedPosixLocale;
  static Locale* sharedUserLocale;

  // [Construction / Destruction]

  Locale();
  Locale(const Locale& other);
  explicit Locale(Data* d);
  explicit Locale(const String& name);
  ~Locale();

  // [Implicit Sharing]

  //! @copydoc Doxygen::Implicit::refCount().
  FOG_INLINE sysuint_t refCount() const { return _d->refCount.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return _d->refCount.get() == 1; }
  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach() { return (!isDetached()) ? _detach() : ERR_OK; }
  //! @copydoc Doxygen::Implicit::_detach().
  err_t _detach();
  //! @copydoc Doxygen::Implicit::free().
  void free();
  
  // [Flags]

  FOG_INLINE bool isNull() const { return _d == _dnull.instancep(); }

  // [Set]

  bool set(const String& name);
  bool set(const Locale& other);

  // [Accessors]

  FOG_INLINE Char getChar(int id) const
  {
    FOG_ASSERT_X((uint)id < LOCALE_CHAR_INVALID, "Fog::Locale::getChar() - Id out of range");
    return Char(_d->data[id]);
  }

  FOG_INLINE uint32_t getValue(int id) const
  {
    FOG_ASSERT_X((uint)id < LOCALE_CHAR_INVALID, "Fog::Locale::getValue() - Id out of range");
    return _d->data[id];
  }

  err_t setValue(int id, uint32_t value);

  // [Operator Overload]

  FOG_INLINE Locale& operator=(const Locale& other) { set(other); return *this; }

  // [Statics]

  static FOG_INLINE const Locale& null() { return *_dnullLocale; }
  static FOG_INLINE const Locale& posix() { return *sharedPosixLocale; }
  static FOG_INLINE const Locale& user() { return *sharedUserLocale; }

  // [Members]

  FOG_DECLARE_D(Data)
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::Locale, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_CORE_LOCALE_H
