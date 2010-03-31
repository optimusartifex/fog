// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_STRINGMATCHER_H
#define _FOG_CORE_STRINGMATCHER_H

// [Dependencies]
#include <Fog/Build/Build.h>
#include <Fog/Core/Assert.h>
#include <Fog/Core/Basics.h>
#include <Fog/Core/String.h>
#include <Fog/Core/StringFilter.h>

//! @addtogroup Fog_Core
//! @{

namespace Fog {

// ============================================================================
// [Fog::StringMatcher]
// ============================================================================

//! @brief String matcher.
//!
//! String matcher is designed to match pattern in string as quick as possible.
//! It creates internally small skip table and time to match string with this
//! table is constant.
//!
//! StringMatcher hasn't many methods to do this job, use @c setPattern()
//! or @c = operator to set pattern and @c indexIn() to match pattern in a given
//! string. In @c indexIn() methods are last argument often case sensitivity
//! that defaults to @c Fog::CaseSensitive.
//!
//! Example:
//! @verbatim
//! StringMatcher32 matcher(Ascii8("test"));
//!
//! String s1(Ascii8("This is test string..."));
//! String s2(Ascii8("This is another TEST string..."));
//!
//! s1.indexOf(matcher, CaseSensitive);        // result == 8
//! s1.indexOf(matcher, CaseInsensitive);      // result == 8
//!
//! s2.indexOf(matcher, CaseSensitive);        // result == InvalidIndex
//! s2.indexOf(matcher, CaseInsensitive);      // result == 16
//! @endverbatim
//!
//! @sa @c Fog::String, @c Fog::StringMatcher
struct FOG_API StringMatcher : public StringFilter
{
  // --------------------------------------------------------------------------
  // [SkipTable]
  // --------------------------------------------------------------------------

  //! @brief @c Fog::StringMatcher skip table.
  //!
  //! @note The table is generated on the fly by first search call.
  struct SkipTable
  {
    enum STATUS
    {
      STATUS_NOT_INITIALIZED = 0,
      STATUS_INITIALIZING_NOW = 1,
      STATUS_INITIALIZED = 2
    };

    uint data[256];
    Atomic<uint> status;
  };

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  StringMatcher();
  StringMatcher(const String& pattern);
  StringMatcher(const StringMatcher& matcher);
  virtual ~StringMatcher();

  // --------------------------------------------------------------------------
  // [Accesors]
  // --------------------------------------------------------------------------

  FOG_INLINE const String& getPattern() const { return _pattern; }

  err_t setPattern(const String& pattern);
  err_t setPattern(const StringMatcher& matcher);

  // --------------------------------------------------------------------------
  // [Filter Implementation]
  // --------------------------------------------------------------------------

  virtual sysuint_t getLength() const;
  virtual Range match(const Char* str, sysuint_t length, uint cs, const Range& range) const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE StringMatcher& operator=(const String& pattern) { setPattern(pattern); return *this; }
  FOG_INLINE StringMatcher& operator=(const StringMatcher& matcher) { setPattern(matcher); return *this; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

private:
  String _pattern;
  mutable SkipTable _skipTable[2];
};

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::StringMatcher, Fog::TYPEINFO_MOVABLE)

//! @}

// [Guard]
#endif // _FOG_CORE_STRINGMATCHER_H
