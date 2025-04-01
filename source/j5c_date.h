//
// j5c_Date Class Header File
//
// Copyright (C) 2017  Jay A. Carlson of J5C Marketing LLC.
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


#ifndef DATE_DATE_H
#define DATE_DATE_H

#include <iosfwd>
#include <iomanip>
#include <iostream>

namespace J5C_DSL_Code {

    //                                       Not Used  Jan  Feb  Mar  Apr  May  Jun  Jul  Aug  Sep  Oct  Nov  Dec
    static const int numberOfDaysBeforeMonth[13] = {0,   0,  31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334};

    static const int numberOfDaysBefore_forDayOfQuarter[13] = {0, 0, 31, 59, 0, 30, 61, 0, 31, 62, 0, 31, 61};

    static bool isLeapYear(int year)
    {
        bool result = false;
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
        {
            result = true;
        }
        return result;
    }


    class j5c_Date {

    friend std::ostream& operator<<(std::ostream& out, const j5c_Date& d);
    friend std::istream& operator>>(std::istream& ins,       j5c_Date& d);

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


    // firstDayOfYear[0] is not used ie -1
    // Note: valid years in this class are limited to 1 - 9999 AD (CE)

    // TimeLine Abbreviations in order...
    // Past Infinity -> 1 BC (BCE) -> (no zero getYear) -> 1 AD (CE) -> (Present Date) -> Future Infinity


private:
    [[nodiscard]] int LeapYearsSinceYear0001(int year, int month) const noexcept;
    j5c_Date  internal_addDays(int days)                                noexcept;
    static constexpr  int MIN_YEAR = 1;
    static constexpr int MAX_YEAR = 9999;
    //std::string DOWT = "Invalid DOW";
    void cout_InvalidDate() const noexcept;
    [[nodiscard]] int daysSinceYear0001Day001(int year, int month, int day) const noexcept;
    [[nodiscard]] std::string padright(int width, int value)    const noexcept;

protected:
    int m_year;
    int m_month;
    int m_day;


public:
    //constructors
    explicit j5c_Date()                                               noexcept;
    explicit j5c_Date(int year, int month, int day)                   noexcept
            : m_year(year) , m_month(month), m_day(day) { };

    explicit j5c_Date(int year, int dayOfTheYear)                     noexcept;
    //constructor helpers / methods
    void set_y_d(int year, int dayOfTheYear)                          noexcept;

    j5c_Date& operator=(const j5c_Date& date);

    //virtual destructor
    virtual ~j5c_Date() = default;
    bool isValidDate(int year, int month, int day)                    noexcept;
    bool isValid()                                                    noexcept;
    [[nodiscard]] bool isLeapYear(int year)                     const noexcept;
    [[nodiscard]] bool isLeapYear()                             const noexcept;

    [[nodiscard]] int getAge()                                  const noexcept;
    [[nodiscard]] int getDaysInMonth(int year, int month)       const noexcept;
    [[nodiscard]] int getDaysInMonth()                          const noexcept;


    void adjustMonth(int& year, int& month, int direction)            noexcept;
    int daysToMonthEnd(int year, int month, int day)                  noexcept;
    int daysFromMonthStart(int day)                                   noexcept;

    int getFirstDayOfYear()                                           noexcept;
    int getDayOfWeek(int year, int mont, int day)                     noexcept;
    int getDayOfWeek()                                                noexcept;
    [[nodiscard]] int getDayOfTheYear()                         const noexcept;
    [[nodiscard]] int getDayOfTheQuarter()                      const noexcept;
    [[nodiscard]] int getQuarter()                              const noexcept;
    [[nodiscard]] int getDaysDiff(const j5c_Date& dt2)          const noexcept;
    std::string getDayText(uint forcedLength)                         noexcept;
    [[nodiscard]] std::string strDate()                         const noexcept;
    j5c_Date getNext_Date()                                           noexcept;
    j5c_Date getPriorDate()                                           noexcept;
    // add days can accept negative numbers
    j5c_Date add_Days(int days)                                       noexcept;



    void addMonths(int months)                                        noexcept;

    const bool operator==(const j5c_Date &)     const noexcept;
    const bool operator!=(const j5c_Date &)     const noexcept;
    const bool operator<( const j5c_Date &)     const noexcept;
    const bool operator>( const j5c_Date &)     const noexcept;
    const bool operator<=(const j5c_Date &)     const noexcept;
    const bool operator>=(const j5c_Date &)     const noexcept;

    const j5c_Date& operator++()        noexcept;   // prefix
    const j5c_Date& operator--()        noexcept;   // prefix
    const j5c_Date  operator++(int)     noexcept;   // postfix
    const j5c_Date  operator--(int)     noexcept;   // postfix

    //too simple for tests...
    [[nodiscard]] int  getDay()   const noexcept { return m_day;   }
    [[nodiscard]] int  getMonth() const noexcept { return m_month; }
    [[nodiscard]] int  getYear()  const noexcept { return m_year;  }
    void setDay  (int day)   noexcept  { m_day   = day;   }
    void setMonth(int month) noexcept  { m_month = month; }
    void setYear (int year)  noexcept  { m_year  = year;  }

    };

}


#endif //DATE_DATE_H

