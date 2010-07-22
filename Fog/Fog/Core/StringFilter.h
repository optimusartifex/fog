// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_STRINGFILTER_H
#define _FOG_CORE_STRINGFILTER_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Assert.h>
#include <Fog/Core/Basics.h>
#include <Fog/Core/String.h>

namespace Fog {

//! @addtogroup Fog_Core_Essentials
//! @{

// ============================================================================
// [Fog::StringFilter]
// ============================================================================

struct FOG_API StringFilter
{
  // [Construction / Destruction]

  StringFilter();
  virtual ~StringFilter();

  // [Public]

  virtual Range indexOf(const Char* str, sysuint_t slen, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  virtual Range lastIndexOf(const Char* str, sysuint_t slen, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  // [Filter Implementation]

  virtual sysuint_t getLength() const;
  virtual Range match(const Char* str, sysuint_t slen, uint cs, const Range& range) const = 0;

private:
  FOG_DISABLE_COPY(StringFilter)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_STRINGFILTER_H
