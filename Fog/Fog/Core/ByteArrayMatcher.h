// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_BYTEARRAYMATCHER_H
#define _FOG_CORE_BYTEARRAYMATCHER_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Assert.h>
#include <Fog/Core/Basics.h>
#include <Fog/Core/ByteArray.h>
#include <Fog/Core/ByteArrayFilter.h>

namespace Fog {

//! @addtogroup Fog_Core_String
//! @{

// ============================================================================
// [Fog::ByteArrayMatcher]
// ============================================================================

//! @brief Byte array matcher.
//!
//! Byte array matcher is designed to match pattern in string as quick as
//! possible. It creates internally small skip table and time to match string
//! with this table is constant.
//!
//! ByteArrayMatcher hasn't many methods to do this job, use @c setPattern()
//! or @c = operator to set pattern and @c indexIn() to match pattern in a given
//! byte array. In @c indexIn() methods are last argument often case sensitivity
//! that defaults to @c Fog::CaseSensitive.
//!
//! Example:
//! @verbatim
//! ByteArrayMatcher matcher("test");
//!
//! ByteArray b1("This is test string...");
//! ByteArray b2("This is another TEST string...");
//!
//! b1.indexOf(matcher, CaseSensitive);        // result == 8
//! b1.indexOf(matcher, CaseInsensitive);      // result == 8
//!
//! b2.indexOf(matcher, CaseSensitive);        // result == InvalidIndex
//! b2.indexOf(matcher, CaseInsensitive);      // result == 16
//! @endverbatim
//!
//! @sa @c Fog::ByteArray, @c Fog::ByteArrayFilter.
struct FOG_API ByteArrayMatcher : public ByteArrayFilter
{
  // --------------------------------------------------------------------------
  // [SkipTable]
  // --------------------------------------------------------------------------

  //! @internal
  //!
  //! @brief @c Fog::ByteArrayMatcher skip table.
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

  ByteArrayMatcher();
  ByteArrayMatcher(const ByteArray& pattern);
  ByteArrayMatcher(const ByteArrayMatcher& matcher);
  virtual ~ByteArrayMatcher();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const ByteArray& getPattern() const { return _pattern; }

  err_t setPattern(const ByteArray& pattern);
  err_t setPattern(const ByteArrayMatcher& matcher);

  // --------------------------------------------------------------------------
  // [Filter Implementation]
  // --------------------------------------------------------------------------

  virtual sysuint_t getLength() const;

  virtual Range match(const char* str, sysuint_t slen, uint cs, const Range& range) const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE ByteArrayMatcher& operator=(const ByteArray& pattern) { setPattern(pattern); return *this; }
  FOG_INLINE ByteArrayMatcher& operator=(const ByteArrayMatcher& matcher) { setPattern(matcher); return *this; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

private:
  ByteArray _pattern;
  mutable SkipTable _skipTable[2];
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::ByteArrayMatcher, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_CORE_BYTEARRAYMATCHER_H
