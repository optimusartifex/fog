// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_DATE_H
#define _FOG_CORE_TOOLS_DATE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Tools/Time.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

#include <Fog/Core/C++/PackByte.h>
//! @brief Date.
//!
//! The @c Date is similar to @c Time, but offers more accurate date handling.
//! The @c Date is using a combination of Julian and Gregorian calendar and
//! handles the year where the Gregorian calendar replaced Julian (cutoff).
//!
//! @section History (adapted from an article by John Walker)
//!
//! The Julian calendar was proclaimed by Julius Caesar in 46 b.c. and underwent
//! several modifications before reaching its final form in 8 c.e. The Julian
//! calendar differs from the Gregorian only in the determination of leap years,
//! lacking the correction for years divisible by 100 and 400 in the Gregorian
//! calendar. In the Julian calendar, any positive year is a leap year if
//! divisible by 4. (Negative years are leap years if the absolute value divided
//! by 4 yields a remainder of 1.) Days are considered to begin at midnight.
//!
//! In the Julian calendar the average year has a length of 365.25 days.
//! Compared to the actual solar tropical year of 365.24219878 days. The
//! calendar thus accumulates one day of error with respect to the solar year
//! every 128 years. Being a purely solar calendar, no attempt is made to
//! synchronise the start of months to the phases of the Moon.
//!
//! The Gregorian calendar was proclaimed by Pope Gregory XIII and took effect
//! in most Catholic states in 1582, in which October 4, 1582 of the Julian
//! calendar was followed by October 15 in the new calendar, correcting for
//! the accumulated discrepancy between the Julian calendar and the equinox as
//! of that date. When comparing historical dates, it's important to note that
//! the Gregorian calendar, used universally today in Western countries and in
//! international commerce, was adopted at different times by different
//! countries. Britain and her colonies (including what is now the United
//! States), did not switch to the Gregorian calendar until 1752, when
//! Wednesday 2nd September in the Julian calendar dawned as Thursday the
//! 14th in the Gregorian.
//!
//! The Gregorian calendar is a minor correction to the Julian. In the Julian
//! calendar every fourth year is a leap year in which February has 29, not
//! 28 days, but in the Gregorian, years divisible by 100 are not leap years
//! unless they are also divisible by 400.
//!
//! The switch from Julian to Gregorian calendar creates a gap (some days are
//! skipped) which is called 'cutoff'. The @c Date handles the cutoff and the
//! year when the switch happened can be customized.
//!
//! @section Usage
//!
//! When the @c Date is created, the values are set to the default and the
//! time zone is set to UTC.
//!
//! @section Implementation Notes
//!
//! The @c Date class is designed to be more accurate than @c Time, but this
//! is not without price. The @c Date instance is 16 bytes long, compared to
//! @c Time, which takes only 8 bytes. The @c Date values (year, month, ...)
//! are stored as class members, compared to time which uses only one integer
//! which represents number of microseconds from an platform-dependent epoch.
//!
//! @note The documentation is based on the following page which is in public
//! domain: 'http://www.fourmilab.ch/documents/calendar/'. It explains the
//! history of Julian and Gregorian calendars and history of all other major
//! calendar systems which may be added to the Fog-Framework in the future.
struct FOG_NO_EXPORT Date
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Date() {}
  FOG_INLINE Date(const Date& other) { operator=(other); }

  explicit FOG_INLINE Date(_Uninitialized) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get date value, see @c DATE_VALUE.
  FOG_INLINE int getValue(uint32_t key) const { return _api.date.getValue(this, key); }
  //! @brief Set date value, see @c DATE_VALUE.
  FOG_INLINE err_t setValue(uint32_t key, int value) { return _api.date.setValue(this, key, value); }

  //! @brief Get year.
  FOG_INLINE int getYear() const { return getValue(DATE_VALUE_YEAR); }
  //! @brief Set year.
  FOG_INLINE err_t setYear(int year) { return setValue(DATE_VALUE_YEAR, year); }

  //! @brief Get month of year (0-11), see @c DATE_MONTH.
  FOG_INLINE int getMonth() const { return getValue(DATE_VALUE_MONTH); }
  //! @brief Set month of year (0-11), see @c DATE_MONTH.
  FOG_INLINE err_t setMonth(int month) { return setValue(DATE_VALUE_MONTH, month); }

  //! @brief Get day of month (0-30).
  FOG_INLINE int getDay() const { return getValue(DATE_VALUE_DAY); }
  //! @brief Set day of month (0-30).
  FOG_INLINE err_t setDay(int day) { return setValue(DATE_VALUE_DAY, day); }

  //! @brief Get day of week (0-6), see @c DATE_DAY.
  FOG_INLINE int getDayOfWeek() const { return getValue(DATE_VALUE_DAY_OF_WEEK); }

  //! @brief Get hour within the current day (0-23).
  FOG_INLINE int getHour() const { return getValue(DATE_VALUE_HOUR); }
  //! @brief Set month of year (0-11), see @c DATE_MONTH.
  FOG_INLINE err_t setHour(int hour) { return setValue(DATE_VALUE_HOUR, hour); }

  //! @brief Get minute within the current hour (0-59).
  FOG_INLINE int getMinute() const { return getValue(DATE_VALUE_MINUTE); }
  //! @brief Set minute within the current hour (0-59).
  FOG_INLINE err_t setMinute(int minute) { return setValue(DATE_VALUE_MINUTE, minute); }

  //! @brief Get second within the current minute (0-60).
  //!
  //! @note Leap seconds may take up to 60.
  FOG_INLINE int getSecond() const { return getValue(DATE_VALUE_SECOND); }
  //! @brief Set second within the current minute (0-60).
  //!
  //! @note Leap seconds may take up to 60.
  FOG_INLINE err_t setSecond(int second) { return setValue(DATE_VALUE_SECOND, second); }

  //! @brief Get millisecond within the current second (0-999).
  FOG_INLINE int getMS() const { return getValue(DATE_VALUE_MS); }
  //! @brief Set millisecond within the current second (0-999).
  //!
  //! @note Milliseconds and microseconds are stored within one value and
  //! calling @c Date::setMS() will clear microsecond.
  FOG_INLINE err_t setMS(int ms) { return setValue(DATE_VALUE_MS, ms); }

  //! @brief Get microsecond within the current second (0-999999).
  FOG_INLINE int getUS() const { return getValue(DATE_VALUE_US); }
  //! @brief Set microsecond within the current second (0-999999).
  FOG_INLINE err_t setUS(int us) { return setValue(DATE_VALUE_US, us); }

  //! @brief Get time-zone (see @c TIME_ZONE).
  FOG_INLINE uint32_t getZone() const { return _timeZone; }
  //! @brief Set time-zone (see @c TIME_ZONE).
  FOG_INLINE void setZone(uint32_t timeZone) { _timeZone = timeZone; }

  // --------------------------------------------------------------------------
  // [Add]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t addValue(uint32_t key, int64_t value)
  {
    return _api.date.addValue(this, key, value);
  }

  // --------------------------------------------------------------------------
  // [SetDate/SetTime]
  // --------------------------------------------------------------------------

  //! @brief
  FOG_INLINE err_t setYMD(int year, int month, int day)
  {
    return _api.date.setYMD(this, year, month, day);
  }

  FOG_INLINE err_t setHMS(int hour, int minute, int second, int us)
  {
    return _api.date.setHMS(this, hour, minute, second, us);
  }

  // --------------------------------------------------------------------------
  // [Consistency]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isNull() const
  {
    return _data[0] == 0 && _data[1] == 0 && _data[2] == 0 && _data[3] == 0;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    MemOps::zero_t<Date>(this);
  }

  // --------------------------------------------------------------------------
  // [Tools]
  // --------------------------------------------------------------------------

  //! @brief Get whether the @a year is a leap year.
  //!
  //! @note The calculation is based on the Gregorian cutoff date.
  FOG_INLINE bool isLeapYear(int year) const
  {
    return _api.date.isLeapYear(this, year);
  }

  //! @brief Get number of days in year.
  //!
  //! @note The calculation is based on the Gregorian cutoff date.
  FOG_INLINE int getNumberOfDaysInYear(int year) const
  {
    return _api.date.getNumberOfDaysInYear(this, year);
  }

  //! @brief Get number of days in year and month.
  //!
  //! @note The calculation is based on the Gregorian cutoff date.
  FOG_INLINE int getNumberOfDaysInMonth(int year, int month) const
  {
    return _api.date.getNumberOfDaysInMonth(this, year, month);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Date& operator=(const Date& other)
  {
    MemOps::copy_t<Date>(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE Date fromTime(const Time& time)
  {
    Date result(UNINITIALIZED);
    _api.date.fromTime(&result, &time);
    return result;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  union
  {
    struct
    {
      //! @brief Year.
      int32_t _year;

      //! @brief Month of year (0-11), see @c DATE_MONTH.
      uint32_t _month : 4;
      //! @brief Day of month (0-30)
      uint32_t _day : 5;
      //! @brief Day of week (0-6), see @c DATE_DAY.
      uint32_t _dayOfWeek : 3;

      //! @brief Hour within the current day (0-23).
      uint32_t _hour : 5;
      //! @brief Hour within the current hour (0-59).
      uint32_t _minute : 6;
      //! @brief Second within the current minute (0-60).
      //!
      //! @note Leap seconds may take up to 60.
      uint32_t _second : 6;
      //! @brief Time zone.
      uint32_t _timeZone : 1;
      //! @brief Reserved for future use.
      uint32_t _reserved : 2;

      //! @brief Microseconds within the current second (0-999999).
      uint32_t _us;

      //! @brief Which year the switch to the Gregorian calendar happened.
      uint32_t _gregorianCutoverYear : 23;
      //! @brief Which month in year the switch to the Gregorian calendar happened.
      uint32_t _gregorianCutoverMonth : 4;
      //! @brief Which day in month the switch to the Gregorian calendar happened.
      uint32_t _gregorianCutoverDay : 5;
    };

    uint32_t _data[4];
  };
};
#include <Fog/Core/C++/PackRestore.h>

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_DATE_H
