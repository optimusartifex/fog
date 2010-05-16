// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// [Guard]
#ifndef _FOG_CORE_TIME_H
#define _FOG_CORE_TIME_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Assert.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/TypeInfo.h>

// Time represents an absolute point in time, internally represented as
// microseconds (s/1,000,000) since a platform-dependent epoch.  Each
// platform's epoch, along with other system-dependent clock interface
// routines, is defined in time_PLATFORM.cc.
//
// TimeDelta represents a duration of time, internally represented in
// microseconds.
//
// TimeTicks represents an abstract time that is always incrementing for use
// in measuring time durations. It is internally represented in microseconds.
// It can not be converted to a human-readable time, but is guaranteed not to
// decrease (if the user changes the computer clock, Time::Now() may actually
// decrease or jump).
//
// These classes are represented as only a 64-bit value, so they can be
// efficiently passed by value.

#if defined(FOG_OS_WINDOWS)
#include <windows.h>
#endif

#include <time.h>

namespace Fog {

//! @addtogroup Fog_Core_DateTime
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Time;
struct TimeDelta;
struct TimeTicks;

// ============================================================================
// [Fog::TimeDelta]
// ============================================================================

//! @brief Time delta (the delta between two @a Time instances).
struct FOG_API TimeDelta
{
public:
  FOG_INLINE TimeDelta() : _delta(0) {}
  FOG_INLINE ~TimeDelta() {}

  // Converts units of time to TimeDeltas.
  static TimeDelta fromDays(int64_t days);
  static TimeDelta fromHours(int64_t hours);
  static TimeDelta fromMinutes(int64_t minutes);
  static TimeDelta fromSeconds(int64_t secs);
  static TimeDelta fromMilliseconds(int64_t ms);
  static TimeDelta fromMicroseconds(int64_t us);

  FOG_INLINE void clear()
  { _delta = 0; }

  // Returns the internal numeric value of the TimeDelta object. Please don't
  // use this and do arithmetic on it, as it is more error prone than using the
  // provided operators.
  FOG_INLINE int64_t toInternalValue() const { return _delta; }

  // Returns the time delta in some unit. The F versions return a floating
  // point value, the "regular" versions return a rounded-down value.
  int inDays() const;
  int inHours() const;
  int inMinutes() const;
  double inSecondsF() const;
  int64_t inSeconds() const;
  double inMillisecondsF() const;
  int64_t inMilliseconds() const;
  int64_t inMicroseconds() const;

  FOG_INLINE TimeDelta& operator=(TimeDelta other)
  {
    _delta = other._delta;
    return *this;
  }

  // Computations with other deltas.
  FOG_INLINE TimeDelta operator+(TimeDelta other) const
  {
    return TimeDelta(_delta + other._delta);
  }

  FOG_INLINE TimeDelta operator-(TimeDelta other) const
  {
    return TimeDelta(_delta - other._delta);
  }

  FOG_INLINE TimeDelta& operator+=(TimeDelta other)
  {
    _delta += other._delta;
    return *this;
  }
  FOG_INLINE TimeDelta& operator-=(TimeDelta other)
  {
    _delta -= other._delta;
    return *this;
  }
  FOG_INLINE TimeDelta operator-() const
  {
    return TimeDelta(-_delta);
  }

  // Computations with ints, note that we only allow multiplicative operations
  // with ints, and additive operations with other deltas.
  FOG_INLINE TimeDelta operator*(int64_t a) const
  {
    return TimeDelta(_delta * a);
  }
  FOG_INLINE TimeDelta operator/(int64_t a) const
  {
    return TimeDelta(_delta / a);
  }
  FOG_INLINE TimeDelta& operator*=(int64_t a)
  {
    _delta *= a;
    return *this;
  }
  FOG_INLINE TimeDelta& operator/=(int64_t a)
  {
    _delta /= a;
    return *this;
  }
  FOG_INLINE int64_t operator/(TimeDelta a) const
  {
    return _delta / a._delta;
  }

  // Defined below because it depends on the definition of the other classes.
  FOG_INLINE Time operator+(Time t) const;
  FOG_INLINE TimeTicks operator+(TimeTicks t) const;

  // Comparison operators.
  FOG_INLINE bool operator==(TimeDelta other) const
  {
    return _delta == other._delta;
  }
  FOG_INLINE bool operator!=(TimeDelta other) const
  {
    return _delta != other._delta;
  }
  FOG_INLINE bool operator<(TimeDelta other) const
  {
    return _delta < other._delta;
  }
  FOG_INLINE bool operator<=(TimeDelta other) const
  {
    return _delta <= other._delta;
  }
  FOG_INLINE bool operator>(TimeDelta other) const
  {
    return _delta > other._delta;
  }
  FOG_INLINE bool operator>=(TimeDelta other) const
  {
    return _delta >= other._delta;
  }

  FOG_INLINE int64_t delta() const
  {
    return _delta;
  }

private:
  friend struct Time;
  friend struct TimeTicks;

  // Constructs a delta given the duration in microseconds. This is private
  // to avoid confusion by callers with an integer constructor. Use
  // fromSeconds, fromMilliseconds, etc. instead.
  FOG_INLINE explicit TimeDelta(int64_t _deltaus) : _delta(_deltaus) {}

  // Delta in microseconds.
  int64_t _delta;
};

FOG_INLINE TimeDelta operator*(int64_t a, TimeDelta td)
{
  return TimeDelta::fromMicroseconds(a * td.delta());
}

// ============================================================================
// [Fog::Time]
// ============================================================================

//! @brief Time.
struct FOG_API Time
{
public:
  static const int64_t MillisecondsPerSecond      = 1000;
  static const int64_t MicrosecondsPerMillisecond = 1000;
  static const int64_t MicrosecondsPerSecond      = MicrosecondsPerMillisecond *
                                                    MillisecondsPerSecond;
  static const int64_t MicrosecondsPerMinute      = MicrosecondsPerSecond * 60;
  static const int64_t MicrosecondsPerHour        = MicrosecondsPerMinute * 60;
  static const int64_t MicrosecondsPerDay         = MicrosecondsPerHour * 24;
  static const int64_t MicrosecondsPerWeek        = MicrosecondsPerDay * 7;
  static const int64_t NanosecondsPerMicrosecond  = 1000;
  static const int64_t NanosecondsPerSecond       = NanosecondsPerMicrosecond *
                                                    MicrosecondsPerSecond;

  // Represents an exploded time that can be formatted nicely. This is kind of
  // like the Win32 SYSTEMTIME structure or the Unix "struct tm" with a few
  // additions and changes to prevent errors.
  struct Exploded
  {
    int year;          // Four digit year "2007"
    int month;         // 1-based month (values 1 = January, etc.)
    int dayOfWeek;     // 0-based day of week (0 = Sunday, etc.)
    int dayOfMonth;    // 1-based day of month (1-31)
    int hour;          // Hour within the current day (0-23)
    int minute;        // Minute within the current hour (0-59)
    int second;        // Second within the current minute (0-59 plus leap
                       //   seconds which may take it up to 60).
    int millisecond;   // Milliseconds within the current second (0-999)
  };

  // Contains the NULL time. Use Time::now() to get the current time.
  FOG_INLINE explicit Time() : _us(0) {}

  // Returns true if the time object has not been initialized.
  FOG_INLINE bool isNull() const
  {
    return _us == 0;
  }

  FOG_INLINE void clear()
  { _us = 0; }

  // Returns the current time. Watch out, the system might adjust its clock
  // in which case time will actually go backwards. We don't guarantee that
  // times are increasing, or that two calls to now() won't be the same.
  static Time now();

  // Converts to/from time_t in UTC and a Time class.
  static Time fromTimeT(time_t tt);
  time_t toTimeT() const;

  // Converts time to a double which is the number of seconds since epoch
  // (Jan 1, 1970). Webkit uses this format to represent time.
  double toDoubleT() const;

#if defined(FOG_OS_WINDOWS)
  static Time fromFILETIME(FILETIME ft);
  FILETIME toFILETIME() const;
#endif

  // Converts an exploded structure representing either the local time or UTC
  // into a Time class.
  FOG_INLINE static Time fromUTCExploded(const Exploded& exploded)
  {
    return fromExploded(false, exploded);
  }
  FOG_INLINE static Time fromLocalExploded(const Exploded& exploded)
  {
    return fromExploded(true, exploded);
  }

  // Converts an integer value representing Time to a class. This is used
  // when deserializing a |Time| structure, using a value known to be
  // compatible. It is not provided as a constructor because the integer type
  // may be unclear from the perspective of a caller.
  FOG_INLINE static Time fromInternalValue(int64_t us)
  {
    return Time(us);
  }

  // For serializing, use fromInternalValue to reconstitute. Please don't use
  // this and do arithmetic on it, as it is more error prone than using the
  // provided operators.
  FOG_INLINE int64_t toInternalValue() const
  {
    return _us;
  }

  // Fills the given exploded structure with either the local time or UTC from
  // this time structure (containing UTC).
  FOG_INLINE void utcExplode(Exploded* exploded) const
  {
    return explode(false, exploded);
  }
  FOG_INLINE void localExplode(Exploded* exploded) const
  {
    return explode(true, exploded);
  }

  // Rounds this time down to the nearest day in local time. It will represent
  // midnight on that day.
  Time LocalMidnight() const;

  FOG_INLINE Time& operator=(Time other)
  {
    _us = other._us;
    return *this;
  }

  // Compute the difference between two times.
  FOG_INLINE TimeDelta operator-(Time other) const
  {
    return TimeDelta(_us - other._us);
  }

  // Modify by some time delta.
  FOG_INLINE Time& operator+=(TimeDelta delta)
  {
    _us += delta._delta;
    return *this;
  }
  FOG_INLINE Time& operator-=(TimeDelta delta)
  {
    _us -= delta._delta;
    return *this;
  }

  // Return a new time modified by some delta.
  FOG_INLINE Time operator+(TimeDelta delta) const
  {
    return _us + delta._delta;
  }
  FOG_INLINE Time operator-(TimeDelta delta) const
  {
    return _us - delta._delta;
  }

  // Comparison operators
  FOG_INLINE bool operator==(Time other) const
  {
    return _us == other._us;
  }
  FOG_INLINE bool operator!=(Time other) const
  {
    return _us != other._us;
  }
  FOG_INLINE bool operator<(Time other) const
  {
    return _us < other._us;
  }
  FOG_INLINE bool operator<=(Time other) const
  {
    return _us <= other._us;
  }
  FOG_INLINE bool operator>(Time other) const
  {
    return _us > other._us;
  }
  FOG_INLINE bool operator>=(Time other) const
  {
    return _us >= other._us;
  }

  FOG_INLINE int64_t us() const 
  {
    return _us;
  }

private:
  friend struct TimeDelta;

  // Explodes the given time to either local time |is_local = true| or UTC
  // |is_local = false|.
  void explode(bool isLocal, Exploded* exploded) const;

  // Unexplodes a given time assuming the source is either local time
  // |is_local = true| or UTC |is_local = false|.
  static Time fromExploded(bool is_local, const Exploded& exploded);

  FOG_INLINE Time(int64_t us) : _us(us) {}

  // The representation of Jan 1, 1970 UTC in microseconds since the
  // platform-dependent epoch.
  static const int64_t kTimeTToMicrosecondsOffset;

  // Time in microseconds in UTC.
  int64_t _us;
};

FOG_INLINE Time TimeDelta::operator+(Time t) const
{
  return Time(t._us + _delta);
}

// ============================================================================
// [Fog::TimeTicks]
// ============================================================================

//! @brief Tick count.
struct FOG_API TimeTicks
{
public:
  FOG_INLINE TimeTicks() : _ticks(0) {}

  // Platform-dependent tick count representing "right now."
  // The resolution of this clock is ~1-15ms.  Resolution varies depending
  // on hardware/operating system configuration.
  static TimeTicks now();

  // Returns a platform-dependent high-resolution tick count. Implementation
  // is hardware dependent and may or may not return sub-millisecond
  // resolution.  THIS CALL IS GENERALLY MUCH MORE EXPENSIVE THAN Now() AND
  // SHOULD ONLY BE USED WHEN IT IS REALLY NEEDED.
  static TimeTicks highResNow();

  FOG_INLINE void clear()
  { _ticks = 0; }

  // Returns true if this object has not been initialized.
  FOG_INLINE bool isNull() const
  {
    return _ticks == 0;
  }

  // Returns the internal numeric value of the TimeTicks object.
  FOG_INLINE int64_t toInternalValue() const
  {
    return _ticks;
  }

  FOG_INLINE TimeTicks& operator=(TimeTicks other)
  {
    _ticks = other._ticks;
    return *this;
  }

  // Compute the difference between two times.
  FOG_INLINE TimeDelta operator-(TimeTicks other) const
  {
    return TimeDelta(_ticks - other._ticks);
  }

  // Modify by some time delta.
  FOG_INLINE TimeTicks& operator+=(TimeDelta delta)
  {
    _ticks += delta._delta;
    return *this;
  }
  FOG_INLINE TimeTicks& operator-=(TimeDelta delta)
  {
    _ticks -= delta._delta;
    return *this;
  }

  // Return a new TimeTicks modified by some delta.
  FOG_INLINE TimeTicks operator+(TimeDelta delta) const
  {
    return TimeTicks(_ticks + delta._delta);
  }
  FOG_INLINE TimeTicks operator-(TimeDelta delta) const
  {
    return TimeTicks(_ticks - delta._delta);
  }

  // Comparison operators
  FOG_INLINE bool operator==(TimeTicks other) const
  {
    return _ticks == other._ticks;
  }
  FOG_INLINE bool operator!=(TimeTicks other) const
  {
    return _ticks != other._ticks;
  }
  FOG_INLINE bool operator<(TimeTicks other) const
  {
    return _ticks < other._ticks;
  }
  FOG_INLINE bool operator<=(TimeTicks other) const
  {
    return _ticks <= other._ticks;
  }
  FOG_INLINE bool operator>(TimeTicks other) const
  {
    return _ticks > other._ticks;
  }
  FOG_INLINE bool operator>=(TimeTicks other) const
  {
    return _ticks >= other._ticks;
  }

protected:
  friend struct TimeDelta;

  // Please use now() to create a new object. This is for internal use
  // and testing. Ticks is in microseconds.
  FOG_INLINE explicit TimeTicks(int64_t ticks) : _ticks(ticks) {}

  // Tick count in microseconds.
  int64_t _ticks;

#if defined(FOG_OS_WINDOWS)
  typedef DWORD (*TickFunctionType)(void);
  static TickFunctionType setMockTickFunction(TickFunctionType ticker);
#endif // FOG_OS_WINDOWS
};

FOG_INLINE TimeTicks TimeDelta::operator+(TimeTicks t) const
{
  return TimeTicks(t._ticks + _delta);
}

//! @}

} // Fog namespace

// ============================================================================
// Fog::TypeInfo<>
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::Time, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::TimeDelta, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::TimeTicks, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_CORE_TIME_H
