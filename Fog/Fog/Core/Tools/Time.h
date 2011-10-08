// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_TIME_H
#define _FOG_CORE_TOOLS_TIME_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>

// [Dependencies - C]
#include <time.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::TimeDelta]
// ============================================================================

//! @brief Time delta (the difference between two @a Time oe @c TimeTicks instances).
struct FOG_NO_EXPORT TimeDelta
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create a time delta initialized to zero.
  FOG_INLINE TimeDelta() : _delta(0) {}

  //! @brief Create a uninitialized @c TimeDelta.
  explicit FOG_INLINE TimeDelta(_Uninitialized) {}
  //! @brief Create a @c TimeDelta from the value (in microseconds).
  explicit FOG_INLINE TimeDelta(int64_t value) : _delta(value) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get time-delta value.
  FOG_INLINE int64_t getDelta() const { return _delta; }
  //! @brief Set time-delta value.
  FOG_INLINE void setDelta(int64_t value) { _delta = value; }

  int64_t getMicroseconds() const { return _delta; }
  int64_t getMilliseconds() const { return _delta / TIME_US_PER_MS; }
  int64_t getSeconds() const { return _delta / TIME_US_PER_SECOND; }

  int getMinutes() const { return (int)(_delta / TIME_US_PER_MINUTE); }
  int getHours() const { return (int)(_delta / TIME_US_PER_HOUR); }
  int getDays() const { return (int)(_delta / TIME_US_PER_DAY); }

  float getMillisecondsF() const { return (float)(double(_delta) / double(TIME_US_PER_MS)); }
  float getSecondsF() const { return (float)(double(_delta) / double(TIME_US_PER_SECOND)); }

  double getMillisecondsD() const { return double(_delta) / double(TIME_US_PER_MS);}
  double getSecondsD() const { return double(_delta) / double(TIME_US_PER_SECOND); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _delta = 0;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE TimeDelta& operator =(const TimeDelta& other) { _delta  = other._delta; return *this; }
  FOG_INLINE TimeDelta& operator+=(const TimeDelta& other) { _delta += other._delta; return *this; }
  FOG_INLINE TimeDelta& operator-=(const TimeDelta& other) { _delta -= other._delta; return *this; }

  FOG_INLINE TimeDelta operator+(const TimeDelta& other) const { return TimeDelta(_delta + other._delta); }
  FOG_INLINE TimeDelta operator-(const TimeDelta& other) const { return TimeDelta(_delta - other._delta); }

  FOG_INLINE TimeDelta operator-() const { return TimeDelta(-_delta); }

  // Implemented-Later.
  FOG_INLINE Time operator+(const Time& t) const;
  FOG_INLINE TimeTicks operator+(const TimeTicks& t) const;

  FOG_INLINE bool operator==(const TimeDelta& other) const { return _delta == other._delta; }
  FOG_INLINE bool operator!=(const TimeDelta& other) const { return _delta != other._delta; }
  FOG_INLINE bool operator< (const TimeDelta& other) const { return _delta <  other._delta; }
  FOG_INLINE bool operator<=(const TimeDelta& other) const { return _delta <= other._delta; }
  FOG_INLINE bool operator> (const TimeDelta& other) const { return _delta >  other._delta; }
  FOG_INLINE bool operator>=(const TimeDelta& other) const { return _delta >= other._delta; }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static TimeDelta fromDays(int64_t days) { return TimeDelta(days * TIME_US_PER_DAY); }
  static TimeDelta fromHours(int64_t hours) { return TimeDelta(hours * TIME_US_PER_HOUR); }
  static TimeDelta fromMinutes(int64_t minutes) { return TimeDelta(minutes * TIME_US_PER_MINUTE); }
  static TimeDelta fromSeconds(int64_t secs) { return TimeDelta(secs * TIME_US_PER_SECOND); }
  static TimeDelta fromMilliseconds(int64_t ms) { return TimeDelta(ms * TIME_US_PER_MS); }
  static TimeDelta fromMicroseconds(int64_t us) { return TimeDelta(us); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Time delta in microseconds.
  int64_t _delta;
};

// ============================================================================
// [Fog::Time]
// ============================================================================

//! @brief Time.
struct FOG_NO_EXPORT Time
{
  FOG_INLINE Time() : _us(0) {}
  explicit FOG_INLINE Time(_Uninitialized) {}
  explicit FOG_INLINE Time(int64_t us) : _us(us) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get time value (in microseconds).
  FOG_INLINE int64_t getValue() const { return _us; }
  //! @brief Set time value (in microseconds).
  FOG_INLINE void setValue(int64_t value) { _us = value; }

  //! @brief Get whether the object has not been initialized.
  FOG_INLINE bool isNull() const { return _us == 0; }

  //! @brief Get time converted to @c time_t.
  FOG_INLINE time_t getTimeT() const { return _api.time_toTimeT(_us); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _us = 0;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Time& operator=(const Time& other) { _us = other._us; return *this; }
  FOG_INLINE Time& operator+=(const TimeDelta& delta) { _us += delta._delta; return *this; }
  FOG_INLINE Time& operator-=(const TimeDelta& delta) { _us -= delta._delta; return *this; }

  FOG_INLINE TimeDelta operator-(const Time& other) const { return TimeDelta(_us - other._us); }

  FOG_INLINE Time operator+(const TimeDelta& delta) const { return Time(_us + delta._delta); }
  FOG_INLINE Time operator-(const TimeDelta& delta) const { return Time(_us - delta._delta); }

  FOG_INLINE bool operator==(const Time& other) const { return _us == other._us; }
  FOG_INLINE bool operator!=(const Time& other) const { return _us != other._us; }
  FOG_INLINE bool operator< (const Time& other) const { return _us <  other._us; }
  FOG_INLINE bool operator<=(const Time& other) const { return _us <= other._us; }
  FOG_INLINE bool operator> (const Time& other) const { return _us >  other._us; }
  FOG_INLINE bool operator>=(const Time& other) const { return _us >= other._us; }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  //! @brief Get the current time.
  //!
  //! @note Return value depends on system time, which can be adjusted by user
  //! or by the system itself. There is no guarantee that times returned by
  //! successive calls will be increasing.
  static FOG_INLINE Time now()
  {
    return Time(_api.time_now());
  }

  //! @brief Create a @c Time from @c time_t (in seconds).
  static FOG_INLINE Time fromTimeT(time_t t)
  {
    return Time(_api.time_fromTimeT(t));
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Time in microseconds in UTC.
  int64_t _us;
};

// ============================================================================
// [Fog::TimeTicks]
// ============================================================================

//! @brief Tick count.
struct FOG_NO_EXPORT TimeTicks
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE TimeTicks() : _ticks(0) {}

  explicit FOG_INLINE TimeTicks(_Uninitialized) {}
  explicit FOG_INLINE TimeTicks(int64_t ticks) : _ticks(ticks) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get ticks value.
  FOG_INLINE int64_t getTicks() const { return _ticks; }
  //! @brief Set ticks value.
  FOG_INLINE void setTicks(int64_t ticks) { _ticks = ticks; }

  //! @brief Get whether the object has not been initialized.
  FOG_INLINE bool isNull() const { return _ticks == 0; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  //! @brief Clear the value to zero.
  FOG_INLINE void reset()
  {
    _ticks = 0;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE TimeDelta operator-(const TimeTicks& other) const { return TimeDelta(_ticks - other._ticks); }

  FOG_INLINE TimeTicks& operator =(const TimeTicks& other) { _ticks  = other._ticks; return *this; }
  FOG_INLINE TimeTicks& operator+=(const TimeDelta& delta) { _ticks += delta._delta; return *this; }
  FOG_INLINE TimeTicks& operator-=(const TimeDelta& delta) { _ticks -= delta._delta; return *this; }

  FOG_INLINE TimeTicks operator+(const TimeDelta& delta) const { return TimeTicks(_ticks + delta._delta); }
  FOG_INLINE TimeTicks operator-(const TimeDelta& delta) const { return TimeTicks(_ticks - delta._delta); }

  FOG_INLINE bool operator==(const TimeTicks& other) const { return _ticks == other._ticks; }
  FOG_INLINE bool operator!=(const TimeTicks& other) const { return _ticks != other._ticks; }
  FOG_INLINE bool operator< (const TimeTicks& other) const { return _ticks <  other._ticks; }
  FOG_INLINE bool operator<=(const TimeTicks& other) const { return _ticks <= other._ticks; }
  FOG_INLINE bool operator> (const TimeTicks& other) const { return _ticks >  other._ticks; }
  FOG_INLINE bool operator>=(const TimeTicks& other) const { return _ticks >= other._ticks; }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  //! @brief Get the platform dependent tick-count.
  //!
  //! The precision can be adjusted using @c ticksPrecision argument. If the
  //! low precision is requested (@c TICKS_PRECISION_LOW) then the precision
  //! varies between 1-15ms, otherwise the high-resolution tick-count is used.
  //!
  //! @note The high-resolution tick-count is generally slower and should be
  //! used only when it is really needed (benchmarking, for example)
  static FOG_INLINE TimeTicks now(uint32_t ticksPrecision = TICKS_PRECISION_LOW)
  {
    return TimeTicks(_api.timeticks_now(ticksPrecision));
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Time ticks value in microseconds.
  int64_t _ticks;
};

// ============================================================================
// [Implemented-Later]
// ============================================================================

FOG_INLINE Time TimeDelta::operator+(const Time& t) const
{
  return Time(t._us + _delta);
}

FOG_INLINE TimeTicks TimeDelta::operator+(const TimeTicks& t) const
{
  return TimeTicks(t._ticks + _delta);
}

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_TIME_H
