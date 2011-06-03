// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_BYTEARRAYFILTER_H
#define _FOG_CORE_TOOLS_BYTEARRAYFILTER_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Tools/ByteArray.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::ByteArrayFilter]
// ============================================================================

//! @brief Byte array filter.
struct FOG_API ByteArrayFilter
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ByteArrayFilter();
  virtual ~ByteArrayFilter();

  // --------------------------------------------------------------------------
  // [Public]
  // --------------------------------------------------------------------------

  virtual Range indexOf(const char* str, size_t slen, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  virtual Range lastIndexOf(const char* str, size_t slen, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual size_t getLength() const;
  virtual Range match(const char* str, size_t slen, uint cs, const Range& range) const = 0;

private:
  _FOG_CLASS_NO_COPY(ByteArrayFilter)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_BYTEARRAYFILTER_H
