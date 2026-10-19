//
// jc313_Date Class Header File
//
// Copyright (C) 2017  Jay A. Carlson.
//
// This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//


#pragma once

#include <chrono>
#include <compare>
#include <iosfwd>
#include <limits>
#include <optional>
#include <string>

namespace JC313_DSL_Code {

    //                                       Not Used  Jan  Feb  Mar  Apr  May  Jun  Jul  Aug  Sep  Oct  Nov  Dec
    inline constexpr int numberOfDaysBeforeMonth[13] = {0,   0,  31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334};

    inline constexpr int numberOfDaysBefore_forDayOfQuarter[13] = {0, 0, 31, 59, 0, 30, 61, 0, 31, 62, 0, 31, 61};

    constexpr bool isLeapYear(const int year) noexcept
    {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    // A calendar date (proleptic Gregorian, years 1 - 9999), a thin wrapper over std::chrono.
    //
    // Invalid dates: the class never prints. A constructor or calculation that would give an
    //   invalid date gives 0000-00-00 instead; check isValid(). make() returns std::nullopt.
    //   The set*() methods do not validate (handy for building a date a field at a time),
    //   so check isValid() after using them.
    // Methods that return an int answer -1 (day of week) or INT_MIN (getDaysDiff, getAge)
    //   when a date involved is invalid.
    class jc313_Date final {

    friend std::ostream& operator<<(std::ostream& out, const jc313_Date& d);
    friend std::istream& operator>>(std::istream& ins,       jc313_Date& d);

private:

    //
    // DOW definitions
    //
    // 0 = Sunday (The Lord's Day)
    // 1 = Monday
    // . . .
    //
    // Month Definitions
    //  01 - Jan
    //  02 - Feb
    //  . . .


    // Note: valid years in this class are limited to 1 - 9999 AD (CE)

    // TimeLine Abbreviations in order...
    // Past Infinity -> 1 BC (BCE) -> (no zero getYear) -> 1 AD (CE) -> (Present Date) -> Future Infinity

    static constexpr int MIN_YEAR = 1;
    static constexpr int MAX_YEAR = 9999;

    // these three are compared in this order by operator<=> (year, then month, then day)
    int m_year  = 0;
    int m_month = 0;
    int m_day   = 0;

    void setInvalid() noexcept { m_year = 0; m_month = 0; m_day = 0; }
    // only for valid dates: the chrono day count, and back (an out of range result is invalid)
    [[nodiscard]] std::chrono::sys_days toSysDays()                        const noexcept;
    [[nodiscard]] static jc313_Date fromSysDays(std::chrono::sys_days days)      noexcept;
    [[nodiscard]] static std::string padright(const int width, const int value);

public:
    //constructors
    explicit jc313_Date()                                               noexcept;   // today (local time)
    explicit jc313_Date(int year, int month, int day)                   noexcept;   // invalid -> 0000-00-00
    explicit jc313_Date(int year, int dayOfTheYear)                     noexcept;
    // the checked way to build a date: std::nullopt when year/month/day is not a valid date
    [[nodiscard]] static std::optional<jc313_Date> make(int year, int month, int day) noexcept;
    //constructor helpers / methods
    void set_y_d(const int year, const int dayOfTheYear)              noexcept;

    [[nodiscard]] bool isValidDate(const int year, const int month, const int day) const noexcept;
    [[nodiscard]] bool isValid()                                const noexcept;
    [[nodiscard]] bool isLeapYear(const int year)               const noexcept;
    [[nodiscard]] bool isLeapYear()                             const noexcept;

    [[nodiscard]] int getAge()                                  const noexcept;   // whole years up to today
    [[nodiscard]] int getAge(const jc313_Date& onDate)          const noexcept;   // whole years up to onDate
    [[nodiscard]] int getDaysInMonth(const int year, const int month)  const noexcept;
    [[nodiscard]] int getDaysInMonth()                          const noexcept;


    static void adjustMonth(int& year, int& month, const int direction) noexcept;
    [[nodiscard]] int daysToMonthEnd(const int year, const int month, const int day) const noexcept;
    [[nodiscard]] static int daysFromMonthStart(const int day)                 noexcept;

    [[nodiscard]] int getFirstDayOfYear()                        const noexcept;
    [[nodiscard]] int getDayOfWeek(const int year, const int month, const int day) const noexcept;
    [[nodiscard]] int getDayOfWeek()                             const noexcept;
    [[nodiscard]] int getDayOfTheYear()                          const noexcept;
    [[nodiscard]] int getDayOfTheQuarter()                       const noexcept;
    [[nodiscard]] int getQuarter()                               const noexcept;
    [[nodiscard]] int getDaysDiff(const jc313_Date& dt2)         const noexcept;
    [[nodiscard]] std::string getDayText(unsigned int forcedLength = 0) const;
    [[nodiscard]] std::string strDate()                          const;
    [[nodiscard]] jc313_Date getNext_Date()                      const noexcept;
    [[nodiscard]] jc313_Date getPriorDate()                      const noexcept;
    // add days can accept negative numbers
    [[nodiscard]] jc313_Date add_Days(int days)                  const noexcept;



    void addMonths(int months)                                         noexcept;

    // ==, !=, <, >, <=, >= compare year, then month, then day (the member order above)
    bool operator==(const jc313_Date&)                 const noexcept = default;
    std::strong_ordering operator<=>(const jc313_Date&) const noexcept = default;

    jc313_Date& operator++()                            noexcept;   // prefix
    jc313_Date& operator--()                            noexcept;   // prefix
    jc313_Date  operator++(int)                         noexcept;   // postfix
    jc313_Date  operator--(int)                         noexcept;   // postfix

    //too simple for tests...
    [[nodiscard]] int  getDay()                 const noexcept { return m_day;   }
    [[nodiscard]] int  getMonth()               const noexcept { return m_month; }
    [[nodiscard]] int  getYear()                const noexcept { return m_year;  }
    void setDay  (int day)                            noexcept  { m_day   = day;   }
    void setMonth(int month)                          noexcept  { m_month = month; }
    void setYear (int year)                           noexcept  { m_year  = year;  }

    };

}
