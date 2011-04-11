// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_RANGE_H
#define _FOG_CORE_TOOLS_RANGE_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/Uninitialized.h>

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
  FOG_INLINE Range() : _start(0), _end(DETECT_LENGTH) {}
  FOG_INLINE Range(_Uninitialized) {}

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
  // [Members]
  // --------------------------------------------------------------------------

  sysuint_t _start;
  sysuint_t _end;
};

// ============================================================================
// [Fog::RangeF]
// ============================================================================

//! @brief Range (32-bit float).
struct FOG_NO_EXPORT RangeF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create an infinite range.
  FOG_INLINE RangeF() : _start(0.0f), _end(0.0f) {}
  FOG_INLINE RangeF(_Uninitialized) {}

  //! @brief Create a copy of @a other.
  FOG_INLINE RangeF(const RangeF& other) : _start(other._start), _end(other._end) {}
  //! @brief Create a range from @a start to @a end.
  FOG_INLINE RangeF(float start, float end) : _start(start), _end(end) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get the start value.
  FOG_INLINE float getStart() const { return _start; }
  //! @brief Get the end value.
  FOG_INLINE float getEnd() const { return _end; }

  //! @brief Get the range length.
  FOG_INLINE float getLength() const { return _end - _start; }

  //! @brief Set the start value.
  FOG_INLINE void setStart(float start) { _start = start; }
  //! @brief Set the end value.
  FOG_INLINE void setEnd(float end) { _end = end; }

  //! @brief Set both the start and the end values.
  FOG_INLINE void setRange(const RangeF& range) { _start = range._start; _end = range._end; }
  //! @overload
  FOG_INLINE void setRange(float start, float end) { _start = start; _end = end; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset() { _start = 0.0f; _end = 0.0f; }

  // --------------------------------------------------------------------------
  // [Consistency]
  // --------------------------------------------------------------------------

  //! @brief Get whether the range is valid (@c _start must be smaller than @c _end).
  FOG_INLINE bool isValid() const { return _start < _end; }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE RangeF& operator=(const RangeF& other)
  {
    _start = other._start;
    _end = other._end;

    return *this;
  }

  FOG_INLINE bool operator==(const RangeF& other) { return _start == other._start && _end == other._end; }
  FOG_INLINE bool operator!=(const RangeF& other) { return !operator==(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float _start;
  float _end;
};

// ============================================================================
// [Fog::RangeD]
// ============================================================================

//! @brief Range (64-bit float).
struct FOG_NO_EXPORT RangeD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create an infinite range.
  FOG_INLINE RangeD() : _start(0.0), _end(0.0) {}
  FOG_INLINE RangeD(_Uninitialized) {}

  //! @brief Create a copy of @a other.
  FOG_INLINE RangeD(const RangeD& other) : _start(other._start), _end(other._end) {}
  //! @brief Create a range from @a start to @a end.
  FOG_INLINE RangeD(double start, double end) : _start(start), _end(end) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get the start value.
  FOG_INLINE double getStart() const { return _start; }
  //! @brief Get the end value.
  FOG_INLINE double getEnd() const { return _end; }

  //! @brief Get the range length.
  FOG_INLINE double getLength() const { return _end - _start; }

  //! @brief Set the start value.
  FOG_INLINE void setStart(double start) { _start = start; }
  //! @brief Set the end value.
  FOG_INLINE void setEnd(double end) { _end = end; }

  //! @brief Set both the start and the end values.
  FOG_INLINE void setRange(const RangeD& range) { _start = range._start; _end = range._end; }
  //! @overload
  FOG_INLINE void setRange(double start, double end) { _start = start; _end = end; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset() { _start = 0.0; _end = 0.0; }

  // --------------------------------------------------------------------------
  // [Consistency]
  // --------------------------------------------------------------------------

  //! @brief Get whether the range is valid (@c _start must be smaller than @c _end).
  FOG_INLINE bool isValid() const { return _start < _end; }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE RangeD& operator=(const RangeD& other)
  {
    _start = other._start;
    _end = other._end;

    return *this;
  }

  FOG_INLINE bool operator==(const RangeD& other) { return _start == other._start && _end == other._end; }
  FOG_INLINE bool operator!=(const RangeD& other) { return !operator==(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  double _start;
  double _end;
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::Range, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::RangeF, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::RangeD, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_CORE_TOOLS_RANGE_H
