// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_RANGE_H
#define _FOG_CORE_TOOLS_RANGE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::Range]
// ============================================================================

//! @brief Range (int).
//!
//! Range is a small data structure that contains start and end position. It
//! can be use to specify part of byte-array, string, list or other array[]
//! related data types.
struct FOG_NO_EXPORT Range
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

  //! @brief Create a copy of @a other range.
  FOG_INLINE Range(const Range& other) :
    _start(other._start),
    _end(other._end)
  {
  }

  //! @brief Create a uninitialized range.
  explicit FOG_INLINE Range(_Uninitialized) {}
  //! @brief Create a range from @a start to @a end.
  explicit FOG_INLINE Range(size_t start, size_t end) :
    _start(start),
    _end(end)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get start position.
  FOG_INLINE size_t getStart() const { return _start; }
  //! @brief Get end position.
  FOG_INLINE size_t getEnd() const { return _end; }

  //! @brief Calculate range length.
  //!
  //! @note If end is smaller than start then 0 is returned.
  FOG_INLINE size_t getLength() const
  {
    return (_end > _start) ? _end - _start : 0;
  }

  //! @brief Calculate range length without checking for correct data.
  //!
  //! This function is used internally in places where we know that the length
  //! is zero or larger. There is assertion so if this method is used inproperly
  //! then assertion-failure is shown.
  FOG_INLINE size_t getLengthNoCheck() const
  {
    FOG_ASSERT(_start <= _end);
    return _end - _start;
  }

  //! @brief Set start position.
  FOG_INLINE void setStart(size_t start) { _start = start; }
  //! @brief Set end position.
  FOG_INLINE void setEnd(size_t end) { _end = end; }

  //! @brief Set both start and end position.
  FOG_INLINE void setRange(size_t start, size_t end)
  {
    _start = start;
    _end = end;
  }

  // --------------------------------------------------------------------------
  // [Consistency]
  // --------------------------------------------------------------------------

  //! @brief Get whether the range is valid.
  //!
  //! @brief Get whether the range is valid (@c _start must be smaller than @c _end).
  FOG_INLINE bool isValid() const { return _start < _end; }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Range& operator=(const Range& other)
  {
    _start = other._start;
    _end = other._end;

    return *this;
  }

  FOG_INLINE bool operator==(const Range& other) { return (_start == other._start) & (_end == other._end); }
  FOG_INLINE bool operator!=(const Range& other) { return (_start != other._start) | (_end != other._end); }

  // --------------------------------------------------------------------------
  // [Static]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool fit(size_t& rStart, size_t& rEnd, size_t length, const Range* range)
  {
    rStart = 0;
    rEnd = length;

    if (range != NULL)
    {
      rStart = range->getStart();
      rEnd = range->getEnd();

      if (rEnd > length) rEnd = length;
    }

    return rStart < rEnd;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  size_t _start;
  size_t _end;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_RANGE_H
