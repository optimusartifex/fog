// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/Cpu.h>
#include <Fog/Core/Tools/Date.h>
#include <Fog/Core/Tools/Time.h>

// [Dependencies - Windows]
#if defined(FOG_OS_WINDOWS)
# include <mmsystem.h>
#endif // FOG_OS_WINDOWS

// [Dependencies - Posix]
#if defined(FOG_OS_POSIX)
# include <sys/time.h>
# include <time.h>
# include <unistd.h>
#endif // FOG_OS_POSIX

// [Dependencies - Mac]
#if defined(FOG_OS_MAC)
# include <mach/mach_time.h>
#endif // FOG_OS_MAC

namespace Fog {

// ============================================================================
// [Fog::Time - Windows]
// ============================================================================

#if defined(FOG_OS_WINDOWS)

// FILETIME "Contains a 64-bit value representing the number of 100-nanosecond
// intervals since January 1, 1601 (UTC).
union FILETIME_U64
{
  uint64_t u64;
  FILETIME ft;
};

static FOG_INLINE void Time_FILETIMEFromUS(FILETIME& ft, const int64_t& us)
{
  uint64_t ns100 = (uint64_t)(us * 10);

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  FILETIME_U64 u;
  u.u64 = ns100;
  ft = u.ft;
#else
  ft.dwHighDateTime = (uint32_t)(ns100 >> 32);
  ft.dwLowDateTime  = (uint32_t)(ns100      );
#endif // FOG_BYTE_ORDER
}

static FOG_INLINE void Time_USFromFILETIME(int64_t& us, const FILETIME& ft)
{
  uint64_t ns100;

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  FILETIME_U64 u;
  u.ft.dwLowDateTime  = ft.dwLowDateTime;
  u.ft.dwHighDateTime = ft.dwHighDateTime;
  ns100 = u.u64;
#else
  ns100 = ((uint64_t)ft.dwHighDateTime << 32) +
          ((uint64_t)ft.dwLowDateTime       ) ;
#endif // FOG_BYTE_ORDER

  us = (int64_t)(ns100 / 10);
}

static FOG_INLINE int64_t Time_getCurrentWallClock()
{
  int64_t us;
  FILETIME ft;

  ::GetSystemTimeAsFileTime(&ft);
  Time_USFromFILETIME(us, ft);

  return us;
}

// Helper which uses winmm (Windows multimedia library) to get high-resolution
// ticks-count. It handles rollover which happens each ~50 days.
struct FOG_NO_EXPORT Time_Global
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Time_Global()
  {
    last = timeGetTime();
    rollover = 0;
    ticksPerMicrosecond = 0.0;

    initialTicks = last;
    initialTime = Time_getCurrentWallClock();

    initHiResClock();
  }

  FOG_INLINE ~Time_Global()
  {
  }

  // --------------------------------------------------------------------------
  // [Get]
  // --------------------------------------------------------------------------

  int64_t getTime()
  {
    for (;;)
    {
      // Calculate the time elapsed since we started our timer
      int64_t ticks = getTicks(TICKS_PRECISION_LOW);
      int64_t elapsed = ticks - initialTicks;

      // Check if enough time has elapsed.
      if (elapsed < FOG_INT64_C(60000000))
        return elapsed + initialTime;

      // Otherwise resync the clock.
      initialTicks = getTicks(TICKS_PRECISION_LOW);
      initialTime = Time_getCurrentWallClock();
    }
  }

  int64_t getTicks(uint32_t ticksPrecision)
  {
    // High-precision ticks.
    if (ticksPrecision == TICKS_PRECISION_HIGH && hasHiResClock())
    {
      LARGE_INTEGER now;
      ::QueryPerformanceCounter(&now);
      return (int64_t)(double(now.QuadPart) / ticksPerMicrosecond);
    }

    // Low-precision ticks
    DWORD now = timeGetTime();

    // Handle the rollover, which happens each ~50 days.
    if (now < last)
      rollover += FOG_INT64_C(0x100000000) * TIME_US_PER_MS;

    last = now;
    return (int64_t)now * TIME_US_PER_MS + rollover;
  }

  // --------------------------------------------------------------------------
  // [Helpers]
  // --------------------------------------------------------------------------

  // Synchronize the QPC clock with GetSystemTimeAsFileTime.
  FOG_INLINE void initHiResClock()
  {
    LARGE_INTEGER ticksPerSecond;
    if (!::QueryPerformanceFrequency(&ticksPerSecond))
      return;
    ticksPerMicrosecond = double(ticksPerSecond.QuadPart) / 1000000.0;
  }

  FOG_INLINE bool hasHiResClock()
  {
    return ticksPerMicrosecond != 0.0;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Lock to protected _last and _rollover.
  Lock lock;

  //! @brief The last timeGetTime for rollover detection.
  DWORD last;
  //! @brief Rollover accumulator.
  int64_t rollover;

  //! @brief Cached clock frequency -> microseconds. This assumes that the
  //! clock frequency is faster than one microsecond (which is 1 MHz).
  //!
  //! Zero indicates that QPF failed and hi-resolution timer can't be used.
  double ticksPerMicrosecond;

  int64_t initialTime;
  int64_t initialTicks;
};

static Static<Time_Global> Time_global;

static int64_t FOG_CDECL Time_now()
{
  AutoLock locked(Time_global->lock);
  return Time_global->getTime();
}

static int64_t FOG_CDECL TimeTicks_now(uint32_t ticksPrecision)
{
  AutoLock locked(Time_global->lock);
  return Time_global->getTicks(ticksPrecision);
}

// The internal representation of time_t under Windows uses FILETIME, which
// epoch is 1601-01-01 00:00:00 UTC.
//
// 11644473600000000 == ((1970-1601)*365+89)*24*60*60*1000*1000, where 89 is
// the number of leap year days between 1601 and 1970 (1970-1601)/4, excluding
// 1700, 1800, and 1900.
static const int64_t TIME_T_OFFSET = FOG_INT64_C(11644473600000000);

static int64_t FOG_CDECL Time_fromTimeT(time_t t)
{
  if (t != 0)
    return (int64_t)t * TIME_US_PER_SECOND + TIME_T_OFFSET;
  else
    return 0;
}

static time_t FOG_CDECL Time_toTimeT(int64_t us)
{
  if (us != 0)
    return (time_t)((us - TIME_T_OFFSET) / TIME_US_PER_SECOND);
  else
    return (time_t)0;
}

/*
Time Time::fromFILETIME(FILETIME ft)
{
  return Time(fileTimeToMicroseconds(ft));
}

FILETIME Time::toFILETIME() const
{
  FILETIME utc_ft;
  microsecondsToFileTime(_us, &utc_ft);
  return utc_ft;
}
*/

#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::Time - Posix]
// ============================================================================

#if defined(FOG_OS_POSIX)

static int64_t FOG_CDECL Time_now()
{
  struct timeval tv;
  struct timezone tz = { 0, 0 }; // UTC.

  if (gettimeofday(&tv, &tz) != 0)
  {
    // TODO: "gettimeofday() failed".
    return 0;
  }

  // Combine to microseconds.
  return tv.tv_sec * TIME_US_PER_SECOND + tv.tv_usec;
}

static int64_t FOG_CDECL TimeTicks_now(uint32_t ticksPrecision)
{
  FOG_UNUSED(ticksPrecision);

#if defined(FOG_OS_MAC)
  static mach_timebase_info_data_t timebase_info;
  if (timebase_info.denom == 0)
  {
    // Zero-initialization of statics guarantees that denom will be 0 before
    // calling mach_timebase_info. mach_timebase_info will never set denom to
    // 0 as that would be invalid, so the zero-check can be used to determine
    // whether mach_timebase_info has already been called. This is recommended
    // by Apple's QA1398.
    kern_return_t kr = mach_timebase_info(&timebase_info);
    FOG_ASSERT(kr == KERN_SUCCESS);
  }

  // mach_absolute_time is it when it comes to ticks on the Mac. Other calls with
  // less precision (such as TickCount) just call through to mach_absolute_time.

  // timebase_info converts absolute time tick units into nanoseconds.  Convert
  // to microseconds up front to stave off overflows.
  return mach_absolute_time() / 1000 * timebase_info.numer / timebase_info.denom;
#elif defined(_POSIX_MONOTONIC_CLOCK) && _POSIX_MONOTONIC_CLOCK >= 0
  struct timespec ts;

  if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
  {
    // TODO: "clock_gettime(CLOCK_MONOTONIC) failed."
    return 0;
  }

  // Combine to microseconds.
  return (int64_t(ts.tv_sec ) * 1000000) +
         (int64_t(ts.tv_nsec) / 1000   ) ;
#else  // _POSIX_MONOTONIC_CLOCK
# error "Fog::TimeTicks::now() - _POSIX_MONOTONIC_CLOCK not defined."
#endif  // _POSIX_MONOTONIC_CLOCK
}

static int64_t FOG_CDECL Time_fromTimeT(time_t t)
{
  if (t != 0)
    return (int64_t)t * TIME_US_PER_SECOND;
  else
    return 0;
}

static time_t FOG_CDECL Time_toTimeT(int64_t us)
{
  if (us != 0)
    return (time_t)(us / TIME_US_PER_SECOND);
  else
    return (time_t)0;
}

#endif // FOG_OS_POSIX

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Time_init(void)
{
  _api.time.now = Time_now;
  _api.time.fromTimeT = Time_fromTimeT;
  _api.time.toTimeT = Time_toTimeT;

  _api.timeticks.now = TimeTicks_now;

#if defined(FOG_OS_WINDOWS)
  Time_global.init();
#endif // FOG_OS_WINDOWS
}

} // Fog namespace
