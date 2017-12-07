//
// J5C_Date Class Implementation File
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

#include "j5c_date.h"

namespace J5C_DSL_Code {


    void J5C_Date::cout_InvalidDate() const noexcept
    {
        std::cout << "!!! *** Invalid Date *** !!! -> "
                  << std::setw(4) << std::setfill('0') << m_year << '-'
                  << std::setw(2) << std::setfill('0') << m_month << '-'
                  << std::setw(2) << std::setfill('0') << m_day << std::endl;
    }

    J5C_Date::J5C_Date() noexcept
    {
        time_t nowTime = time(&nowTime);
        struct tm *timeinfo = localtime(&nowTime);
        int year = timeinfo->tm_year + 1900;
        int month = timeinfo->tm_mon + 1;
        int day = timeinfo->tm_mday;
        set_ymd(year, month, day);
    };

    J5C_Date::J5C_Date(const int &year, const int &month, const int &day)  noexcept
    {
        set_ymd(year, month, day);
    };

    J5C_Date::J5C_Date(const int &year, const int &dayOfTheYear) noexcept
    {
        set_y_d(year, dayOfTheYear);
    }


    void J5C_Date::set_ymd(const int &year, const int &month, const int &day) noexcept
    {

        m_year = year;
        m_month = month;
        m_day = day;
        if (!this->isValid())
        {
            cout_InvalidDate();
        }
    }

    void J5C_Date::set_y_d(const int &year, const int &dayOfTheYear) noexcept
    {
        m_year = year;
        m_month = 0;
        if (dayOfTheYear == 0)
        {
            m_year = 0;
            m_month = 0;
            m_day = 0;
            cout_InvalidDate();
            return;
        }

        int workingDayOfTheYear = dayOfTheYear;
        bool isLeapYear = this->isLeapYear();
        if (isLeapYear) {
            //Checking for leap getYear day
            if (workingDayOfTheYear == 60) {
                m_month = 2;
                m_day = 29;
                return;
            }
            else
            {
                if (workingDayOfTheYear > 60) {
                    workingDayOfTheYear--;
                }
            }
        }
        while (m_month < 12) {
            workingDayOfTheYear -= numberOfDaysInMonth[m_month];
            m_month++;
            if (workingDayOfTheYear <= numberOfDaysInMonth[m_month]) {
                m_day = workingDayOfTheYear;
                return;
            }
        }
        m_year = 0;
        m_month = 0;
        m_day = 0;
        cout_InvalidDate();
    }

    int J5C_Date::getDay()     const noexcept
    { return m_day; };

    int J5C_Date::getMonth()   const noexcept
    { return m_month; };

    int J5C_Date::getYear()    const noexcept
    { return m_year; };

    void J5C_Date::setDay(const int &day) noexcept
    {
        m_day = day;
    };

    void J5C_Date::setMonth(const int &month) noexcept
    {
        m_month = month;
    };

    void J5C_Date::setYear(const int &year) noexcept
    {
        m_year = year;
    };


    bool J5C_Date::isLeapYear(int year) const noexcept
    {

        if (year < MIN_YEAR) return false;
        if (year > MAX_YEAR) return false;

        bool divideBy004remainder0 = (year %   4 == 0);
        if (!divideBy004remainder0) return false;

        bool divideBy100remainder0 = (year % 100 == 0);
        bool test2 = ((divideBy004remainder0) && (!divideBy100remainder0));
        if (test2) return true;

        bool divideBy400remainder0 = (year % 400 == 0);
        bool test3 = ((divideBy004remainder0) && (divideBy100remainder0) && (divideBy400remainder0));
        if (test3) return true;

        // there were no passing conditions so it must be false
        return false;
    }

    bool J5C_Date::isLeapYear() const noexcept
    {
        return isLeapYear(m_year);
    }

    int J5C_Date::LeapYearsSinceYear0001(int year, int month) const noexcept
        {
            // Check if the current getYear needs to be considered
            // for the count of leap years or not
            if (month <= 2) year--;

            // An year is a leap getYear if it is a multiple of 4,
            // multiple of 400 and not a multiple of 100.
            int result = ((year/4) - (year/100) + (year/400));
            return result;
        }

    int J5C_Date::daysSinceYear0001Day001(int year, int month, int day) const noexcept
    {
        // initialize count using years and day
        int n = year*365 + numberOfDaysBeforeMonth[month] + day;
        // Add a day for every leap getYear
        n += LeapYearsSinceYear0001(year, month);
        return n;
    }

    int J5C_Date::getDaysDiff(const J5C_Date& dt2) const noexcept
    {
        int n1 = this->daysSinceYear0001Day001(m_year, m_month, m_day);
        int n2 = this->daysSinceYear0001Day001(dt2.getYear(), dt2.getMonth(), dt2.getDay());
        return (n2 - n1);
    }

    int J5C_Date::getAge() const noexcept
    {
        const J5C_Date now{};
        int days = this->getDaysDiff(now);
        auto ddays = static_cast<double>(days);
        auto result = ddays / 365.25;
        return static_cast<int>(result);
    }

    J5C_Date J5C_Date::internal_addDays(int days) const noexcept
    {
        bool isLeapYear;
        int year = m_year;
        J5C_Date newDate;
        days = days + m_day;
        const int daysInYear     = 365;
        const int daysInLeapYear = 366;
        bool cont = true;

        while (cont)
        {
            cont = false;
            isLeapYear = this->isLeapYear(year);
            if (isLeapYear) {
                if (days > daysInLeapYear) {
                    year++;
                    days -= daysInLeapYear;
                    cont = true;
                }
            } else
            {
                if (days > daysInYear)
                {
                    year++;
                    days -= daysInYear;
                    cont = true;
                }
            }
        }
        newDate = J5C_Date(year,days);
        return newDate;

    }

    J5C_Date J5C_Date::internal_subDays(int days) const noexcept
    {
        bool cont = true;
        bool isLeapYear;
        int newYear = m_year;
        bool once = false;
        // lets work with positive numbers by flipping the sign
        days = days * -1;
        J5C_Date newDate = J5C_Date(0001, 01, 01);
        int newDOTY = this->getDayOfTheYear();
        while (cont) {
            if (newDOTY > days)
            {
                newDOTY = newDOTY - days;
                cont = false;
            }
            else
            {
                newDOTY += 365;
                if (!once)
                {
                    once = true;
                    newDOTY++;
                }
                isLeapYear = this->isLeapYear(newYear);
                if (isLeapYear) { newDOTY+= 1; }
                newYear--;
            }
        }
        newDate.set_y_d(newYear,newDOTY);
        return newDate;
    }


    J5C_Date J5C_Date::add_Days(int days) const noexcept
    {
        J5C_Date newDate = J5C_Date(0001, 01, 01);
        if (days == 0)
        {
            return newDate;
        }
        if (days > 0)
        {
            newDate = internal_addDays(days);
        }
        if (days < 0)
        {
            newDate = internal_subDays(days);
        }
        return newDate;
    }

    int J5C_Date::getFirstDayOfYear() const noexcept
    {
        return firstDayOfYear[m_year];
    }

    int J5C_Date::getDayOfTheYear() const noexcept
    {
        int DOTY = 0;
        if (m_month < 3) {
            DOTY = numberOfDaysBeforeMonth[m_month] + m_day;
        } else {
            int LeapYearConditionalOffset = 0;
            // although bool is usually converted to (int) 1, lets not assume that for portability
            if (isLeapYear()) {
                LeapYearConditionalOffset = 1;
            }
            DOTY = (LeapYearConditionalOffset + numberOfDaysBeforeMonth[m_month] + m_day);
        }
        return DOTY;
    }

    int J5C_Date::getDayOfTheQuarter() const noexcept
    {
        int result = 0;
        int leapYearOffset = 0;
        if (m_month != 3) {
            result = (numberOfDaysBefore_forDayOfQuarter[m_month]) + m_day;
        } else {
            if (this->isLeapYear()) {
                leapYearOffset = 1;
            }
            result = (numberOfDaysBefore_forDayOfQuarter[m_month]) + leapYearOffset + m_day;
        }
        return result;
    }

    int J5C_Date::getDayOfWeek() const noexcept
    {
        // -1 = invalid DOW
        // 0 = Sunday
        // 1 = Monday
        // 2 = Tuesday
        // 3 = Wednesday
        // 4 = Thursday
        // 5 = Friday
        // 6 = Saturday

        int DOW = -1;
        if (!this->isValid()) {
            return DOW;
        }
        // DateToDayConversion is the reason for the -1 in the next line
        DOW = ((this->getFirstDayOfYear() + this->getDayOfTheYear() - 1) % 7);
        return DOW;
    }

    std::string J5C_Date::getDayText(unsigned int forcedLength = 0) const noexcept
    {
        int DOW = this->getDayOfWeek();
        std::string DOWT = "Invalid Date";
        if (forcedLength > 50)
        {
            forcedLength = 50;
        }
        std::string result;
        if (DOW == -1) {
            DOWT = "Invalid DOW";
        }
        if (DOW == 0) DOWT = "Sunday";
        if (DOW == 1) DOWT = "Monday";
        if (DOW == 2) DOWT = "Tuesday";
        if (DOW == 3) DOWT = "Wednesday";
        if (DOW == 4) DOWT = "Thursday";
        if (DOW == 5) DOWT = "Friday";
        if (DOW == 6) DOWT = "Saturday";
        if (forcedLength == 0) {
            result = DOWT;
        } else {
            if (forcedLength > DOWT.length()) {
                unsigned long counter = forcedLength - DOWT.length();
                while (counter > 0) {
                    DOWT.append(" ");
                    counter--;
                }
                result = DOWT;
            } else {
                result = DOWT.substr(0, forcedLength);
            }
        }
        return result;
    }

    int J5C_Date::getQuarter() const noexcept
    {
        int quarter = 0;
        if (m_month > 0) quarter++;
        if (m_month > 3) quarter++;
        if (m_month > 6) quarter++;
        if (m_month > 9) quarter++;
        return quarter;
    }


    bool J5C_Date::isValid() const noexcept
    {

        //
        // warning!! --contains many early returns
        //

        if (m_year < MIN_YEAR) return false;
        //
        if ((m_month > 12) || (m_month < 1)) return false;
        //
        if ((m_day > 31) || (m_day < 1)) return false;
        //
        if ((m_day == 31) && (m_month == 2 ||
                              m_month == 4 ||
                              m_month == 6 ||
                              m_month == 9 ||
                              m_month == 11))
            return false;
        //
        if ((m_day >= 30) && (m_month == 2)) return false;
        //
        // At this point the date is valid except for possible leap years
        if ((m_month == 2) && (m_day == 29)) {
            if (!isLeapYear()) return false;
        }
        //
        // there were no failing conditions so it must be true
        return true;
    };

    const bool J5C_Date::operator==(const J5C_Date &d) const noexcept
    {
        return ((m_year == d.m_year) && (m_month == d.m_month) && (m_day == d.m_day));
    };

    const bool J5C_Date::operator<(const J5C_Date &d) const noexcept
    {
        if (m_year == d.m_year) { // same getYear
            if (m_month == d.m_month) { // same getMonth
                return (m_day < d.m_day);
            } else {
                return (m_month < d.m_month);
            };
        } else { // different getYear
            return (m_year < d.m_year);
        };
    };

    // remaining operators defined in terms of the above
    const bool J5C_Date::operator<=(const J5C_Date &d) const noexcept
        {
        if (this == &d) { return true; }
        return (operator<(d));
    };

    const bool J5C_Date::operator>=(const J5C_Date &d) const noexcept
    {
        if (this == &d) { return true; }
        return (operator>(d));
    };

    const bool J5C_Date::operator>(const J5C_Date &d) const noexcept
    {
        if (m_year == d.m_year) { // same getYear
            if (m_month == d.m_month) { // same getMonth
                return (m_day > d.m_day);
            } else {
                return (m_month > d.m_month);
            };
        } else { // different getYear
            return (m_year > d.m_year);
        };
    };

    const bool J5C_Date::operator!=(const J5C_Date &d) const noexcept
    {
        J5C_Date t = *this;
        return !(t == d);
    };

    J5C_Date J5C_Date::next_Date() const noexcept
    {
        // warning!! --early returns in code
        J5C_Date nextDate;
        nextDate = this->add_Days(1);
        return nextDate;
    };

    J5C_Date J5C_Date::prior_Date() const noexcept
    {
        // warning!! --early returns in code
        J5C_Date nextDate;
        nextDate = this->add_Days(-1);
        return nextDate;
    };

    const J5C_Date J5C_Date::operator++(int) noexcept
    {
        J5C_Date next = this->next_Date();
        return next;
    };

    const J5C_Date  J5C_Date::operator--(int) noexcept
    {
        J5C_Date prior = this->prior_Date();
        return prior;
    };

    const J5C_Date&  J5C_Date::operator++() noexcept
    {
        J5C_Date next = this->next_Date();
        m_year  = next.getYear();
        m_month = next.m_month;
        m_day   = next.m_day;
        return *this;
    };


    const J5C_Date&  J5C_Date::operator--() noexcept
    {
        J5C_Date prior = this->prior_Date();
        m_year  = prior.getYear();
        m_month = prior.m_month;
        m_day   = prior.m_day;
        return *this;
    };

    std::string J5C_Date::strDate() const noexcept
    {
        std::stringstream ss;
        ss << std::setw(4) << std::setfill('0') << m_year << '-'
           << std::setw(2) << std::setfill('0') << m_month << '-'
           << std::setw(2) << std::setfill('0') << m_day;
        return ss.str();


    }

    std::ostream &operator<<(std::ostream &out, const J5C_Date &d) {
        return out
                << std::setw(4) << std::setfill('0') << d.getYear() << '-'
                << std::setw(2) << std::setfill('0') << d.getMonth() << '-'
                << std::setw(2) << std::setfill('0') << d.getDay();
    }

}
