// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_RANGE_H
#define _FOG_CORE_RANGE_H

// [Dependencies]
#include <Fog/Core/Constants.h>
#include <Fog/Core/TypeInfo.h>

namespace Fog {

//! @addtogroup Fog_Core_String
//! @{

// ============================================================================
// [Fog::Range]
// ============================================================================

//! @brief Range.
//!
//! Range is a small data structure that contains start and end position. It
//! can be use to specify part of byte-array, string, list or other array[]
//! related data types.
struct FOG_HIDDEN Range
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create an infinite range.
  FOG_INLINE Range() :
    _start(0),
    _end(DETECT_LENGTH)
  {
  }

  //! @brief Create a range from @a start to @a end.
  explicit FOG_INLINE Range(sysuint_t start, sysuint_t end = DETECT_LENGTH) :
    _start(start),
    _end(end)
  {
  }

  //! @brief Create a copy of @a other range.
  FOG_INLINE Range(const Range& other) :
    _start(other._start),
    _end(other._end)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get whether the range is valid.
  //!
  //! Range is not valid either if the end is less or equal to start.
  FOG_INLINE bool isValid() const { return _start < _end; }

  //! @brief Get start position.
  FOG_INLINE sysuint_t getStart() const { return _start; }
  //! @brief Get end position.
  FOG_INLINE sysuint_t getEnd() const { return _end; }

  //! @brief Calculate range length.
  //!
  //! @note If end is smaller than start then 0 is returned.
  FOG_INLINE sysuint_t getLength() const
  {
    return (_end > _start) ? _end - _start : 0;
  }

  //! @brief Calculate range length without checking for correct data.
  //!
  //! This function is used internally in places where we know that the length
  //! is zero or larger. There is assertion so if this method is used inproperly
  //! then assertion-failure is shown.
  FOG_INLINE sysuint_t getLengthNoCheck() const
  {
    FOG_ASSERT(_start <= _end);
    return _end - _start;
  }

  //! @brief Set start position.
  FOG_INLINE void setStart(sysuint_t start) { _start = start; }
  //! @brief Set end position.
  FOG_INLINE void setEnd(sysuint_t end) { _end = end; }

  //! @brief Set both start and end position.
  FOG_INLINE void setRange(sysuint_t start, sysuint_t end)
  {
    _start = start;
    _end = end;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Range& operator=(const Range& other)
  {
    _start = other._start;
    _end = other._end;

    return *this;
  }

  FOG_INLINE bool operator==(const Range& other)
  {
    return (_start == other._start) & (_end == other._end);
  }

  FOG_INLINE bool operator!=(const Range& other)
  {
    return (_start != other._start) | (_end != other._end);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  sysuint_t _start;
  sysuint_t _end;
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<T>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::Range, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_CORE_RANGE_H
