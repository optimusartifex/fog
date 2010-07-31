// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/CpuInfo.h>
#include <Fog/Core/Lazy.h>
#include <Fog/Core/Lock.h>
#include <Fog/Core/Time.h>

#if defined(FOG_OS_WINDOWS)
#include <windows.h>
#include <mmsystem.h>
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#endif // FOG_OS_POSIX

#if defined(FOG_OS_MAC)
#include <mach/mach_time.h>
#endif // FOG_OS_MAC

namespace Fog {

// ============================================================================
// [Fog::TimeDelta]
// ============================================================================

// static
TimeDelta TimeDelta::fromDays(int64_t days)
{
  return TimeDelta(days * Time::MicrosecondsPerDay);
}

// static
TimeDelta TimeDelta::fromHours(int64_t hours)
{
  return TimeDelta(hours * Time::MicrosecondsPerHour);
}

// static
TimeDelta TimeDelta::fromMinutes(int64_t minutes)
{
  return TimeDelta(minutes * Time::MicrosecondsPerMinute);
}

// static
TimeDelta TimeDelta::fromSeconds(int64_t secs)
{
  return TimeDelta(secs * Time::MicrosecondsPerSecond);
}

// static
TimeDelta TimeDelta::fromMilliseconds(int64_t ms)
{
  return TimeDelta(ms * Time::MicrosecondsPerMillisecond);
}

// static
TimeDelta TimeDelta::fromMicroseconds(int64_t us)
{
  return TimeDelta(us);
}

int TimeDelta::inDays() const
{
  return static_cast<int>(_delta / Time::MicrosecondsPerDay);
}

int TimeDelta::inHours() const
{
  return static_cast<int>(_delta / Time::MicrosecondsPerHour);
}

int TimeDelta::inMinutes() const
{
  return static_cast<int>(_delta / Time::MicrosecondsPerMinute);
}

double TimeDelta::inSecondsF() const
{
  return static_cast<double>(_delta) / Time::MicrosecondsPerSecond;
}

int64_t TimeDelta::inSeconds() const
{
  return _delta / Time::MicrosecondsPerSecond;
}

double TimeDelta::inMillisecondsF() const
{
  return static_cast<double>(_delta) / Time::MicrosecondsPerMillisecond;
}

int64_t TimeDelta::inMilliseconds() const
{
  return _delta / Time::MicrosecondsPerMillisecond;
}

int64_t TimeDelta::inMicroseconds() const
{
  return _delta;
}

// [Fog::Time]

// static
Time Time::fromTimeT(time_t tt)
{
  if (tt == 0)
    // Preserve 0 so we can tell it doesn't exist.
    return Time();

  return (tt * MicrosecondsPerSecond) + kTimeTToMicrosecondsOffset;
}

time_t Time::toTimeT() const
{
  if (_us == 0)
    return 0;  // Preserve 0 so we can tell it doesn't exist.

  return (_us - kTimeTToMicrosecondsOffset) / MicrosecondsPerSecond;
}

double Time::toDoubleT() const
{
  if (_us == 0)
    // Preserve 0 so we can tell it doesn't exist.
    return 0;

  return (static_cast<double>(_us - kTimeTToMicrosecondsOffset) /
          static_cast<double>(MicrosecondsPerSecond));
}

Time Time::LocalMidnight() const
{
  Exploded exploded;
  localExplode(&exploded);
  
  exploded.hour = 0;
  exploded.minute = 0;
  exploded.second = 0;
  exploded.millisecond = 0;
  
  return fromLocalExploded(exploded);
}

// [Windows Specific]
#if defined(FOG_OS_WINDOWS)
// Windows Timer Primer
//
// A good article:  http://www.ddj.com/windows/184416651
// A good mozilla bug:  http://bugzilla.mozilla.org/show_bug.cgi?id=363258
//
// The default windows timer, GetSystemTimeAsFileTime is not very precise.
// It is only good to ~15.5ms.
//
// QueryPerformanceCounter is the logical choice for a high-precision timer.
// However, it is known to be buggy on some hardware.  Specifically, it can
// sometimes "jump".  On laptops, QPC can also be very expensive to call.
// It's 3-4x slower than timeGetTime() on desktops, but can be 10x slower
// on laptops.  A unittest exists which will show the relative cost of various
// timers on any system.
//
// The next logical choice is timeGetTime().  timeGetTime has a precision of
// 1ms, but only if you call APIs (timeBeginPeriod()) which affect all other
// applications on the system.  By default, precision is only 15.5ms.
// Unfortunately, we don't want to call timeBeginPeriod because we don't
// want to affect other applications.  Further, on mobile platforms, use of
// faster multimedia timers can hurt battery life.  See the intel 
// article about this here: 
// http://softwarecommunity.intel.com/articles/eng/1086.htm
//
// To work around all this, we're going to generally use timeGetTime().  We
// will only increase the system-wide timer if we're not running on battery
// power.  Using timeBeginPeriod(1) is a requirement in order to make our
// event loop waits have the same resolution that our time measurements
// do.  Otherwise, WaitForSingleObject(..., 1) will no less than 15ms when
// there is nothing else to waken the Wait.

namespace {

// From MSDN, FILETIME "Contains a 64-bit value representing the number of
// 100-nanosecond intervals since January 1, 1601 (UTC)."
static int64_t fileTimeToMicroseconds(const FILETIME& ft)
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  const int64_t* ft64 = reinterpret_cast<const int64_t*>(&ft);

  // Need to bit_cast to fix alignment, then divide by 10 to convert
  // 100-nanoseconds to milliseconds. This only works on little-endian
  // machines.
  return *ft64 / 10;
#else
#error "Error"
#endif
}

static void microsecondsToFileTime(int64_t us, FILETIME* ft)
{
  FOG_ASSERT(us >= 0);

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  int64_t* ft64 = reinterpret_cast<int64_t*>(ft);

  // Multiply by 10 to convert milliseconds to 100-nanoseconds. Bit_cast will
  // handle alignment problems. This only works on little-endian machines.
  *ft64 = us * 10;
#else
# error "Fog::Time - PORT ME!"
#endif
}

static int64_t currentWallclockMicroseconds()
{
  FILETIME ft;
  ::GetSystemTimeAsFileTime(&ft);
  return fileTimeToMicroseconds(ft);
}

// Time between resampling the un-granular clock for this API.  60 seconds.
static const int kMaxMillisecondsToAvoidDrift = 60 * Fog::Time::MillisecondsPerSecond;

static int64_t initialTime = 0;
static Fog::TimeTicks initialTicks;

static void initializeClock()
{
  initialTicks = Fog::TimeTicks::now();
  initialTime = currentWallclockMicroseconds();
}

}  // namespace

// The internal representation of Time uses FILETIME, whose epoch is 1601-01-01
// 00:00:00 UTC.  ((1970-1601)*365+89)*24*60*60*1000*1000, where 89 is the
// number of leap year days between 1601 and 1970: (1970-1601)/4 excluding
// 1700, 1800, and 1900.
// static
const int64_t Time::kTimeTToMicrosecondsOffset = FOG_INT64_C(11644473600000000);

// static
Time Time::now()
{
  if (initialTime == 0) initializeClock();

  // We implement time using the high-resolution timers so that we can get
  // timeouts which are smaller than 10-15ms.  If we just used
  // currentWallclockMicroseconds(), we'd have the less-granular timer.
  //
  // To make this work, we initialize the clock (initialTime) and the
  // counter (initial_ctr).  To compute the initial time, we can check
  // the number of ticks that have elapsed, and compute the delta.
  //
  // To avoid any drift, we periodically resync the counters to the system
  // clock.
  for (;;)
  {
    TimeTicks ticks = TimeTicks::now();

    // Calculate the time elapsed since we started our timer
    TimeDelta elapsed = ticks - initialTicks;

    // Check if enough time has elapsed that we need to resync the clock.
    if (elapsed.inMilliseconds() > kMaxMillisecondsToAvoidDrift)
    {
      initializeClock();
      continue;
    }

    return elapsed + initialTime;
  }
}

// static
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

// static
Time Time::fromExploded(bool isLocal, const Time::Exploded& exploded)
{
  // Create the system struct representing our exploded time. It will either be
  // in local time or UTC.
  SYSTEMTIME st;

  st.wYear = exploded.year;
  st.wMonth = exploded.month;
  st.wDayOfWeek = exploded.dayOfWeek;
  st.wDay = exploded.dayOfMonth;
  st.wHour = exploded.hour;
  st.wMinute = exploded.minute;
  st.wSecond = exploded.second;
  st.wMilliseconds = exploded.millisecond;

  // Convert to FILETIME.
  FILETIME ft;

  if (!SystemTimeToFileTime(&st, &ft))
  {
    FOG_ASSERT_NOT_REACHED();
    return Time(0);
  }

  // Ensure that it's in UTC.
  if (isLocal)
  {
    FILETIME utc_ft;
    LocalFileTimeToFileTime(&ft, &utc_ft);
    return Time(fileTimeToMicroseconds(utc_ft));
  }
  return Time(fileTimeToMicroseconds(ft));
}

void Time::explode(bool isLocal, Time::Exploded* exploded) const
{
  // FILETIME in UTC.
  FILETIME utc_ft;
  microsecondsToFileTime(_us, &utc_ft);

  // FILETIME in local time if necessary.
  BOOL success = TRUE;
  FILETIME ft;
  if (isLocal)
    success = ::FileTimeToLocalFileTime(&utc_ft, &ft);
  else
    ft = utc_ft;

  // FILETIME in SYSTEMTIME (exploded).
  SYSTEMTIME st;

  if (!success || !FileTimeToSystemTime(&ft, &st))
  {
    // "Unable to convert time, don't know why"
    FOG_ASSERT_NOT_REACHED();
    ZeroMemory(exploded, sizeof(exploded));
    return;
  }

  exploded->year = st.wYear;
  exploded->month = st.wMonth;
  exploded->dayOfWeek = st.wDayOfWeek;
  exploded->dayOfMonth = st.wDay;
  exploded->hour = st.wHour;
  exploded->minute = st.wMinute;
  exploded->second = st.wSecond;
  exploded->millisecond = st.wMilliseconds;
}

// TimeTicks ------------------------------------------------------------------
namespace {

// We define a wrapper to adapt between the __stdcall and __cdecl call of the
// mock function, and to avoid a static constructor.  Assigning an import to a
// function pointer directly would require setup code to fetch from the IAT.
static DWORD timeGetTimeWrapper()
{
  return timeGetTime();
}


static DWORD (*tickFunction)(void) = &timeGetTimeWrapper;

// We use timeGetTime() to implement TimeTicks::now().  This can be problematic
// because it returns the number of milliseconds since Windows has started,
// which will roll over the 32-bit value every ~49 days.  We try to track
// rollover ourselves, which works if TimeTicks::now() is called at least every
// 49 days.
class NowSingleton
{
public:
  NowSingleton() : 
    _rollover(TimeDelta::fromMilliseconds(0)),
    _lastSeen(0), 
    _hiResClockEnabled(false)
  {
    useHiResClock(true);
  }

  ~NowSingleton()
  {
    useHiResClock(false);
  }

  TimeDelta now()
  {
    AutoLock locked(_lock);

    // We should hold the lock while calling tickFunction to make sure that
    // we keep our _lastSeen stay correctly in sync.
    DWORD now = tickFunction();
    if (now < _lastSeen)
      _rollover += TimeDelta::fromMilliseconds(FOG_INT64_C(0x100000000)); // ~49.7 days.
    _lastSeen = now;
    return TimeDelta::fromMilliseconds(now) + _rollover;
  }

private:
  // Enable or disable the faster multimedia timer.
  void useHiResClock(bool enabled)
  {
    if (enabled == _hiResClockEnabled)
      return;
    if (enabled)
      timeBeginPeriod(1);
    else
      timeEndPeriod(1);
    _hiResClockEnabled = enabled;
  }

  Lock _lock; // To protected _lastSeen and _rollover.
  TimeDelta _rollover;   // Accumulation of time lost due to rollover.
  DWORD _lastSeen;       // The last timeGetTime value we saw, to detect rollover.
  bool _hiResClockEnabled;

  FOG_DISABLE_COPY(NowSingleton)
};

static Lazy<NowSingleton> nowSingleton;

// Overview of time counters:
// (1) CPU cycle counter. (Retrieved via RDTSC)
// The CPU counter provides the highest resolution time stamp and is the least
// expensive to retrieve. However, the CPU counter is unreliable and should not
// be used in production. Its biggest issue is that it is per processor and it
// is not synchronized between processors. Also, on some computers, the counters
// will change frequency due to thermal and power changes, and stop in some
// states.
//
// (2) QueryPerformanceCounter (QPC). The QPC counter provides a high-
// resolution (100 nanoseconds) time stamp but is comparatively more expensive
// to retrieve. What QueryPerformanceCounter actually does is up to the HAL.
// (with some help from ACPI).
// According to http://blogs.msdn.com/oldnewthing/archive/2005/09/02/459952.aspx
// in the worst case, it gets the counter from the rollover interrupt on the
// programmable interrupt timer. In best cases, the HAL may conclude that the
// RDTSC counter runs at a constant frequency, then it uses that instead. On
// multiprocessor machines, it will try to verify the values returned from
// RDTSC on each processor are consistent with each other, and apply a handful
// of workarounds for known buggy hardware. In other words, QPC is supposed to
// give consistent result on a multiprocessor computer, but it is unreliable in
// reality due to bugs in BIOS or HAL on some, especially old computers.
// With recent updates on HAL and newer BIOS, QPC is getting more reliable but
// it should be used with caution.
//
// (3) System time. The system time provides a low-resolution (typically 10ms
// to 55 milliseconds) time stamp but is comparatively less expensive to
// retrieve and more reliable.
class HighResNowSingleton
{
public:
  HighResNowSingleton() : 
    _ticksPerMicrosecond(0.0),
    _skew(0)
  {
    initializeClock();

    // On Athlon X2 CPUs (e.g. model 15) QueryPerformanceCounter is
    // unreliable. Fallback to low-res clock.
    if (strcmp(getCpuInfo()->vendor, "AuthenticAMD") == 0 && getCpuInfo()->family == 15)
    {
      disableHighResClock();
    }
  }

  bool isUsingHighResClock()
  {
    return _ticksPerMicrosecond != 0.0;
  }

  void disableHighResClock()
  {
    _ticksPerMicrosecond = 0.0;
  }

  TimeDelta now()
  {
    // Our maximum tolerance for QPC drifting.
    const int kMaxTimeDrift = 50 * Time::MicrosecondsPerMillisecond;

    if (isUsingHighResClock())
    {
      int64_t now = unreliableNow();

      // Verify that QPC does not seem to drift.
      FOG_ASSERT(now - reliableNow() - _skew < kMaxTimeDrift);

      return TimeDelta::fromMicroseconds(now);
    }

    // Just fallback to the slower clock.
    return nowSingleton.get()->now();
  }

private:
  // Synchronize the QPC clock with GetSystemTimeAsFileTime.
  void initializeClock()
  {
    LARGE_INTEGER ticks_per_sec = {0};

    if (!::QueryPerformanceFrequency(&ticks_per_sec))
      return;  // Broken, we don't guarantee this function works.

    _ticksPerMicrosecond = 
      static_cast<float>(ticks_per_sec.QuadPart) /
      static_cast<float>(Time::MicrosecondsPerSecond);

    _skew = unreliableNow() - reliableNow();
  }

  // Get the number of microseconds since boot in a reliable fashion
  int64_t unreliableNow()
  {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return static_cast<int64_t>(now.QuadPart / _ticksPerMicrosecond);
  }

  // Get the number of microseconds since boot in a reliable fashion
  int64_t reliableNow()
  {
    return nowSingleton.get()->now().inMicroseconds();
  }

  // Cached clock frequency -> microseconds. This assumes that the clock
  // frequency is faster than one microsecond (which is 1MHz, should be OK).
  float _ticksPerMicrosecond;  // 0 indicates QPF failed and we're broken.
  int64_t _skew;  // Skew between lo-res and hi-res clocks (for debugging).

  FOG_DISABLE_COPY(HighResNowSingleton)
};

static Lazy<HighResNowSingleton> highResNowSingleton;

}  // namespace

// static
TimeTicks::TickFunctionType TimeTicks::setMockTickFunction(TimeTicks::TickFunctionType ticker)
{
  TickFunctionType old = tickFunction;
  tickFunction = ticker;
  return old;
}

// static
TimeTicks TimeTicks::now()
{
  return TimeTicks() + nowSingleton.get()->now();
}

// static
TimeTicks TimeTicks::highResNow()
{
  return TimeTicks() + highResNowSingleton.get()->now();
}
#endif

// [Posix Specific]
#if defined(FOG_OS_POSIX)

// The Time routines in this file use standard POSIX routines, or almost-
// standard routines in the case of timegm.  We need to use a Mach-specific
// function for TimeTicks::now() on Mac OS X.

// The internal representation of Time uses time_t directly, so there is no
// offset.  The epoch is 1970-01-01 00:00:00 UTC.

// static
const int64_t Time::kTimeTToMicrosecondsOffset = FOG_INT64_C(0);

// static
Time Time::now()
{
  struct timeval tv;
  struct timezone tz = { 0, 0 };  // UTC
  if (gettimeofday(&tv, &tz) != 0)
  {
    // "Could not determine time of day"
    FOG_ASSERT(0);
  }
  // Combine seconds and microseconds in a 64-bit field containing microseconds
  // since the epoch.  That's enough for nearly 600 centuries.
  return tv.tv_sec * MicrosecondsPerSecond + tv.tv_usec;
}

// static
Time Time::fromExploded(bool isLocal, const Time::Exploded& exploded)
{
  struct tm timestruct;

  timestruct.tm_sec    = exploded.second;
  timestruct.tm_min    = exploded.minute;
  timestruct.tm_hour   = exploded.hour;
  timestruct.tm_mday   = exploded.dayOfMonth;
  timestruct.tm_mon    = exploded.month - 1;
  timestruct.tm_year   = exploded.year - 1900;
  timestruct.tm_wday   = exploded.dayOfWeek;  // mktime/timegm ignore this
  timestruct.tm_yday   = 0;     // mktime/timegm ignore this
  timestruct.tm_isdst  = -1;    // attempt to figure it out
  timestruct.tm_gmtoff = 0;     // not a POSIX field, so mktime/timegm ignore
  timestruct.tm_zone   = NULL;  // not a POSIX field, so mktime/timegm ignore

  time_t seconds;
  if (isLocal)
    seconds = mktime(&timestruct);
  else
    seconds = timegm(&timestruct);

  // "mktime/timegm could not convert from exploded"
  FOG_ASSERT(seconds >= 0);

  uint64_t milliseconds = seconds * MillisecondsPerSecond + exploded.millisecond;
  return Time(milliseconds * MicrosecondsPerMillisecond);
}

void Time::explode(bool isLocal, Exploded* exploded) const
{
  // Time stores times with microsecond resolution, but Exploded only carries
  // millisecond resolution, so begin by being lossy.
  uint64_t milliseconds = _us / MicrosecondsPerMillisecond;
  time_t seconds = milliseconds / MillisecondsPerSecond;

  struct tm timestruct;
  if (isLocal)
    localtime_r(&seconds, &timestruct);
  else
    gmtime_r(&seconds, &timestruct);

  exploded->year         = timestruct.tm_year + 1900;
  exploded->month        = timestruct.tm_mon + 1;
  exploded->dayOfWeek    = timestruct.tm_wday;
  exploded->dayOfMonth   = timestruct.tm_mday;
  exploded->hour         = timestruct.tm_hour;
  exploded->minute       = timestruct.tm_min;
  exploded->second       = timestruct.tm_sec;
  exploded->millisecond  = milliseconds % MillisecondsPerSecond;
}

// static
TimeTicks TimeTicks::now()
{
  uint64_t absolute_micro;

#if defined(FOG_OS_MAC)

  static mach_timebase_info_data_t timebase_info;
  if (timebase_info.denom == 0)
  {
    // Zero-initialization of statics guarantees that denom will be 0 before
    // calling mach_timebase_info.  mach_timebase_info will never set denom to
    // 0 as that would be invalid, so the zero-check can be used to determine
    // whether mach_timebase_info has already been called.  This is
    // recommended by Apple's QA1398.
    kern_return_t kr = mach_timebase_info(&timebase_info);
    FOG_ASSERT(kr == KERN_SUCCESS);
  }

  // mach_absolute_time is it when it comes to ticks on the Mac.  Other calls
  // with less precision (such as TickCount) just call through to
  // mach_absolute_time.

  // timebase_info converts absolute time tick units into nanoseconds.  Convert
  // to microseconds up front to stave off overflows.
  absolute_micro = mach_absolute_time() / Time::NanosecondsPerMicrosecond *
                   timebase_info.numer / timebase_info.denom;

  // Don't bother with the rollover handling that the Windows version does.
  // With numer and denom = 1 (the expected case), the 64-bit absolute time
  // reported in nanoseconds is enough to last nearly 585 years.

#elif defined(FOG_OS_POSIX) && defined(_POSIX_MONOTONIC_CLOCK) && _POSIX_MONOTONIC_CLOCK >= 0

  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
  {
    // "clock_gettime(CLOCK_MONOTONIC) failed."
    FOG_ASSERT_NOT_REACHED();
    return TimeTicks();
  }

  absolute_micro =
      (static_cast<int64_t>(ts.tv_sec) * Time::MicrosecondsPerSecond) +
      (static_cast<int64_t>(ts.tv_nsec) / Time::NanosecondsPerMicrosecond);

#else  // _POSIX_MONOTONIC_CLOCK
#error "No usable tick clock function on this platform."
#endif  // _POSIX_MONOTONIC_CLOCK

  return TimeTicks(absolute_micro);
}

// static
TimeTicks TimeTicks::highResNow()
{
  return now();
}

#endif // FOG_OS_POSIX

} // Fog namespace
