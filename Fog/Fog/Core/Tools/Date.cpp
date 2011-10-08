// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Tools/Date.h>

namespace Fog {

// ============================================================================
// [Fog::Date - Helpers]
// ============================================================================

static const uint8_t Date_daysInMonth[12] =
{
  31, // January.
  28, // February (+1 on leap year).
  31, // March.
  30, // April.
  31, // May.
  30, // June.
  31, // July.
  31, // August.
  30, // September.
  31, // October.
  30, // November.
  31, // December.
};

static const uint32_t DATE_GREGORIAN_CUTOVER = 1582;

// ============================================================================
// [Fog::Date - Normalize]
// ============================================================================

// Fit day to current date, using floor rounding. This means that for example
// Feb 30 will be converted to Feb 28/29, instead of being converted to March
// 1.
static err_t FOG_CDECL Date_normalize(Date* self)
{
  int days = self->getNumberOfDaysInMonth(self->_year, self->_month);
  if (self->_day >= (uint)days) self->_day = (uint)(days - 1);

  // TODO: Calculate day in week.

  return ERR_OK;
}

// ============================================================================
// [Fog::Date - GetValue/SetValue]
// ============================================================================

static int FOG_CDECL Date_getValue(const Date* self, uint32_t key)
{
  switch (key)
  {
    case DATE_VALUE_YEAR:
      return self->_year;

    case DATE_VALUE_MONTH:
      return self->_month;

    case DATE_VALUE_DAY:
      return self->_day;

    case DATE_VALUE_DAY_OF_WEEK:
      return self->_dayOfWeek;

    case DATE_VALUE_HOUR:
      return self->_hour;

    case DATE_VALUE_MINUTE:
      return self->_minute;

    case DATE_VALUE_SECOND:
      return self->_second;

    case DATE_VALUE_MS:
      // Compiler can optimize the division using multiplication and shift.
      FOG_ASSUME(self->_us < 1000000);
      return self->_us / 1000;

    case DATE_VALUE_US:
      return self->_us;

    default:
      return 0;
  }
}

static err_t FOG_CDECL Date_setValue(Date* self, uint32_t key, int value)
{
  switch (key)
  {
    case DATE_VALUE_YEAR:
    {
      self->_year = value;
      return Date_normalize(self);
    }

    case DATE_VALUE_MONTH:
    {
      if ((uint)value >= 12)
        return ERR_RT_INVALID_ARGUMENT;

      self->_month = value;
      return Date_normalize(self);
    }

    case DATE_VALUE_DAY:
    {
      if ((uint)value >= 32)
        return ERR_RT_INVALID_ARGUMENT;

      self->_day = value;
      return Date_normalize(self);
    }

    case DATE_VALUE_DAY_OF_WEEK:
    {
      if ((uint)value >= 7)
        return ERR_RT_INVALID_ARGUMENT;

      int diff = value - (int)self->_dayOfWeek;
      return self->addValue(DATE_VALUE_DAY, diff);
    }

    case DATE_VALUE_HOUR:
      if ((uint)value >= 24)
        return ERR_RT_INVALID_ARGUMENT;

      self->_hour = value;
      goto _RetState;

    case DATE_VALUE_MINUTE:
      if ((uint)value >= 60)
        return ERR_RT_INVALID_ARGUMENT;

      self->_minute = value;
      goto _RetState;

    case DATE_VALUE_SECOND:
      if ((uint)value >= 61)
        return ERR_RT_INVALID_ARGUMENT;

      self->_second = value;
      goto _RetState;

    case DATE_VALUE_MS:
      if ((uint)value >= 1000)
        return ERR_RT_INVALID_ARGUMENT;
      self->_us = value * 1000;
      goto _RetState;

    case DATE_VALUE_US:
      if ((uint)value >= 1000000)
        return ERR_RT_INVALID_ARGUMENT;
      self->_us = value;
_RetState:
      return ERR_OK;

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }
}

// ============================================================================
// [Fog::Date - AddValue]
// ============================================================================

static err_t FOG_CDECL Date_addDay(Date* self, int64_t day)
{
  // TODO:
  return ERR_OK;
}

static err_t FOG_CDECL Date_addValue(Date* self, uint32_t key, int64_t value)
{
  if (key == DATE_VALUE_YEAR)
  {
    int64_t year = (int64_t)self->_year + value;
    if (year < (int64_t)INT32_MIN || year > (int64_t)INT32_MAX)
      return ERR_RT_OVERFLOW;

    self->_year = (int32_t)year;
    return Date_normalize(self);
  }

  if (key == DATE_VALUE_MONTH)
  {
    int64_t month = (int64_t)self->_month + value;
    int64_t year  = (int64_t)self->_year;

    if ((uint64_t)month >= DATE_MONTH_COUNT)
    {
      year += month / DATE_MONTH_COUNT;
      month = month % DATE_MONTH_COUNT;
      if (month < 0)
      {
        month += DATE_MONTH_COUNT;
        year--;
      }
    }

    if (year < (int64_t)INT32_MIN || year > (int64_t)INT32_MAX)
      return ERR_RT_OVERFLOW;

    self->_year = (int32_t)year;
    self->_month = (uint8_t)month;
    return Date_normalize(self);
  }

  // Work on different instance, because the values can overflow. In this case
  // the ERR_RT_OVERFLOW error is returned and the original date is unchanged.
  Date d(*self);
  // Ignore leap second.
  if (d._second == 60) d._second = 59;

  int64_t t;

  switch (key)
  {
    case DATE_VALUE_MS:
      if (value < (int64_t)INT64_MIN / 1000 || value > (int64_t)INT64_MAX / 1000)
        return ERR_RT_OVERFLOW;

      value *= 1000;
      // ... Fall through ...

    case DATE_VALUE_US:
      t = value % 1000000;
      value /= 1000000;
      if (t < 0)
      {
        t += 1000000;
        value--;
      }

      t += d._us;
      if (t >= 1000000)
      {
        t -= 1000000;
        value++;
      }

      d._us = (uint32_t)t;
      // ... Fall through ...

    case DATE_VALUE_SECOND:
      t = value % 60;
      value /= 60;
      if (t < 0)
      {
        t += 60;
        value--;
      }

      t += d._second;
      if (t >= 60)
      {
        t -= 60;
        value++;
      }

      d._second = (uint8_t)t;
      // ... Fall through ...

    case DATE_VALUE_MINUTE:
      t = value % 60;
      value /= 60;
      if (t < 0)
      {
        t += 60;
        value--;
      }

      t += d._minute;
      if (t >= 60)
      {
        t -= 60;
        value++;
      }

      d._minute = (uint8_t)t;
      // ... Fall through ...

    case DATE_VALUE_HOUR:
      t = value % 24;
      value /= 24;
      if (t < 0)
      {
        t += 24;
        value--;
      }

      t += d._hour;
      if (t >= 24)
      {
        t -= 24;
        value++;
      }

      d._hour = (uint8_t)t;
      // ... Fall through ...

    case DATE_VALUE_DAY:
    case DATE_VALUE_DAY_OF_WEEK:
      return Date_addDay(self, value);

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }
}

// ============================================================================
// [Fog::Date - SetDate / SetTime]
// ============================================================================

static err_t FOG_CDECL Date_setYMD(Date* self, int year, int month, int day)
{
  if ((uint)month >= 12 || (uint)day >= 31)
    return ERR_RT_INVALID_ARGUMENT;

  self->_year = year;
  self->_month = (uint8_t)month;
  self->_day = (uint8_t)day;
  return Date_normalize(self);
}

static err_t FOG_CDECL Date_setHMS(Date* self, int hour, int minute, int second, int us)
{
  if ((uint)hour >= 24 || (uint)minute >= 60 || (uint)second >= 61 || (uint)us >= 1000000)
    return ERR_RT_INVALID_ARGUMENT;

  self->_hour = (uint8_t)hour;
  self->_minute = (uint8_t)minute;
  self->_us = (uint32_t)us;
  return Date_normalize(self);
}

// ============================================================================
// [Fog::Date - FromTime]
// ============================================================================

static err_t FOG_CDECL Date_fromTime(Date* self, const Time* time)
{
  // TODO: Implement.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::Date - Statics]
// ============================================================================

static bool FOG_CDECL Date_isLeapYear(const Date* self, int year)
{
  if ((year & 3) != 0)
    return false;

  int cutover = self->_gregorianCutoverYear;

  // Julian calendar rules.
  if (year < cutover)
    return true;

  if (year == cutover && self->_gregorianCutoverMonth < DATE_MONTH_MARCH)
    return true;

  // Gregorian calendar rules.
  return (year % 100) != 0 || (year % 400) == 0;
}

static int FOG_CDECL Date_getNumberOfDaysInYear(const Date* self, int year)
{
  uint32_t days = 365;

  if (self->isLeapYear(year))
    days++;

  return days;
}

static int FOG_CDECL Date_getNumberOfDaysInMonth(const Date* self, int year, int month)
{
  if (FOG_UNLIKELY((uint)month >= DATE_MONTH_COUNT))
    return 0;

  uint32_t days = Date_daysInMonth[month];
  if (month == DATE_MONTH_FEBRUARY && self->isLeapYear(year))
    days++;

  return days;
}

static err_t FOG_CDECL Date_convert(Date* dst, const Date* src, uint32_t timeZone)
{
  if (src->_timeZone == timeZone)
  {
    *dst = *src;
    return ERR_OK;
  }

  // TODO: Time zone switch.
  return ERR_OK;
}

// ============================================================================
// [Fog::Date - Equality]
// ============================================================================

static bool FOG_CDECL Date_eq(const Date* a, const Date* b)
{
  Date aUTC(UNINITIALIZED);
  Date bUTC(UNINITIALIZED);

  if (a->getZone() != TIME_ZONE_LOCAL)
  {
    aUTC = *a;
    aUTC.setZone(TIME_ZONE_UTC);
    a = &aUTC;
  }

  if (b->getZone() != TIME_ZONE_LOCAL)
  {
    bUTC = *b;
    bUTC.setZone(TIME_ZONE_UTC);
    b = &bUTC;
  }

  return MemOps::eq_t<Date>(a, b);
}

// ============================================================================
// [Fog::Date - Compare]
// ============================================================================

static int FOG_CDECL Date_compare(const Date* a, const Date* b)
{
  Date aUTC(UNINITIALIZED);
  Date bUTC(UNINITIALIZED);

  if (a->getZone() != TIME_ZONE_LOCAL)
  {
    aUTC = *a;
    aUTC.setZone(TIME_ZONE_UTC);
    a = &aUTC;
  }

  if (b->getZone() != TIME_ZONE_LOCAL)
  {
    bUTC = *b;
    bUTC.setZone(TIME_ZONE_UTC);
    b = &bUTC;
  }

  if (a->_year > b->_year)
    return 1;
  if (a->_year < b->_year)
    return -1;

  if (a->_month > b->_month)
    return 1;
  if (a->_month < b->_month)
    return -1;

  if (a->_day > b->_day)
    return 1;
  if (a->_day < b->_day)
    return -1;

  if (a->_hour > b->_hour)
    return 1;
  if (a->_hour < b->_hour)
    return -1;

  if (a->_minute > b->_minute)
    return 1;
  if (a->_minute < b->_minute)
    return -1;

  if (a->_us > b->_us)
    return 1;
  if (a->_us < b->_us)
    return -1;

  return 0;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Date_init(void)
{
  _api.date_getValue = Date_getValue;
  _api.date_setValue = Date_setValue;
  _api.date_addValue = Date_addValue;

  _api.date_setYMD = Date_setYMD;
  _api.date_setHMS = Date_setHMS;

  _api.date_fromTime = Date_fromTime;

  _api.date_isLeapYear = Date_isLeapYear;
  _api.date_getNumberOfDaysInYear = Date_getNumberOfDaysInYear;
  _api.date_getNumberOfDaysInMonth = Date_getNumberOfDaysInMonth;

  _api.date_convert = Date_convert;
  _api.date_eq = Date_eq;
  _api.date_compare = Date_compare;
}

} // Fog namespace
