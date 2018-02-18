//
// j5c_Date Class Implementation File
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


    void j5c_Date::cout_InvalidDate() const noexcept
    {
        std::cout << "!!! *** Invalid Date *** !!! -> "
                  << std::setw(4) << std::setfill('0') << m_year << '-'
                  << std::setw(2) << std::setfill('0') << m_month << '-'
                  << std::setw(2) << std::setfill('0') << m_day << std::endl;
    }

    j5c_Date::j5c_Date() noexcept
    {
        time_t nowTime = time(&nowTime);
        struct tm *timeinfo = localtime(&nowTime);
        m_year = timeinfo->tm_year + 1900;
        m_month = timeinfo->tm_mon + 1;
        m_day = timeinfo->tm_mday;
    };

    j5c_Date::j5c_Date(int year, int dayOfTheYear) noexcept
    {
        set_y_d(year, dayOfTheYear);
    }

    void j5c_Date::set_y_d(int year, int dayOfTheYear) noexcept
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

    j5c_Date&  j5c_Date::operator=(const j5c_Date& date)
    {
        if (this != &date)
        {
            m_year  = date.m_year;
            m_month = date.m_month;
            m_day   = date.m_day;
        }
        return *this;
    }

    bool j5c_Date::isLeapYear(int year) const noexcept
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
        return test3;
    }

    bool j5c_Date::isLeapYear() const noexcept
    {
        return isLeapYear(m_year);
    }

    int j5c_Date::LeapYearsSinceYear0001(int year, int month) const noexcept
        {
            // Check if the current getYear needs to be considered
            // for the count of leap years or not
            if (month <= 2) year--;

            // An year is a leap getYear if it is a multiple of 4,
            // multiple of 400 and not a multiple of 100.
            int result = ((year/4) - (year/100) + (year/400));
            return result;
        }

    int j5c_Date::daysSinceYear0001Day001(int year, int month, int day) const noexcept
    {
        // initialize count using years and day
        int n = year*365 + numberOfDaysBeforeMonth[month] + day;
        // Add a day for every leap getYear
        n += LeapYearsSinceYear0001(year, month);
        return n;
    }

    int j5c_Date::getDaysDiff(const j5c_Date& dt2) const noexcept
    {
        int n1 = this->daysSinceYear0001Day001(m_year, m_month, m_day);
        int n2 = this->daysSinceYear0001Day001(dt2.getYear(), dt2.getMonth(), dt2.getDay());
        return (n2 - n1);
    }

    int j5c_Date::getAge() const noexcept
    {
        const j5c_Date now{};
        int days = this->getDaysDiff(now);
        auto ddays = static_cast<double>(days);
        auto result = ddays / 365.25;
        return static_cast<int>(result);
    }

    j5c_Date j5c_Date::internal_addDays(int days) const noexcept
    {
        bool isLeapYear;
        int year = m_year;
        for (int m = 1; m < m_month; ++m)
        {
            days += numberOfDaysInMonth[m];
        }
        isLeapYear = this->isLeapYear(year);
        if ((isLeapYear) && (m_month >2))
        {
            days++;
        }
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
        j5c_Date newDate{year,days};
        return newDate;

    }

    j5c_Date j5c_Date::internal_subDays(int days) const noexcept
    {
        bool cont = true;
        bool isLeapYear;
        int newYear = m_year;
        int temp = 0;
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
                temp = newYear - 1;
                isLeapYear = this->isLeapYear(temp);
                if (isLeapYear) { newDOTY+= 1; }
                newYear--;
            }
        }
        j5c_Date newDate{newYear,newDOTY};
        return newDate;
    }


    j5c_Date j5c_Date::add_Days(int days) const noexcept
    {
        j5c_Date newDate = j5c_Date(0001, 01, 01);
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
            days = days * -1;
            newDate = internal_subDays(days);
        }
        return newDate;
    }

    int j5c_Date::getFirstDayOfYear() const noexcept
    {
        //This needs a little explanation...
        //  There is no year 0, years go from -1 directly to 1.
        //  For this class we don't use years less than 1.
        //  The first day of the year is a repeating pattern every 400 years.
        //  The first year the first day is Monday      and is stored at array position   1 not zero.
        //  On the 400th year the first day is Saturday and is stored at array position 400
        //  ( array position 400 is position 0 in a repeating pattern but we do a substitution instead. )

        //early return possible
        if (m_year < 1)   return -1;
        if (m_year < 400) return firstDayOfYear[m_year];
        //
        int range = m_year / 400;
        int yearConversion = m_year - (range * 400);

        //this is year 400,800,1200, etc...
        // the value at array position [5] is 6 which is the first Saturday in the array
        if (yearConversion == 0) yearConversion = 5;

        //this is (converted) years 1-399
        return firstDayOfYear[yearConversion];
    }

    int j5c_Date::getDayOfTheYear() const noexcept
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

    int j5c_Date::getDayOfTheQuarter() const noexcept
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

    int j5c_Date::getDayOfWeek() const noexcept
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

    std::string j5c_Date::getDayText(unsigned int forcedLength = 0) const noexcept
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
                auto counter = forcedLength - DOWT.length();
                std::string padding(counter, ' ');
                DOWT.append(padding);
                result = DOWT;
            } else {
                result = DOWT.substr(0, forcedLength);
            }
        }
        return result;
    }

    int j5c_Date::getQuarter() const noexcept
    {
        int quarter = 0;
        if (m_month > 0) quarter++;
        if (m_month > 3) quarter++;
        if (m_month > 6) quarter++;
        if (m_month > 9) quarter++;
        return quarter;
    }


    bool j5c_Date::isValid() const noexcept
    {

        if (m_year < MIN_YEAR) return false;
        if (m_year > MAX_YEAR) return false;
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

    const bool j5c_Date::operator==(const j5c_Date &d) const noexcept
    {
        return ((m_year == d.m_year) && (m_month == d.m_month) && (m_day == d.m_day));
    };

    const bool j5c_Date::operator<(const j5c_Date &d) const noexcept
    {
        if (m_year == d.m_year)
        {
            if (m_month == d.m_month)
            {
                return (m_day < d.m_day);
            }
            else
            {
                return (m_month < d.m_month);
            };
        }
        else
        {
            return (m_year < d.m_year);
        };
    };

    // remaining operators defined in terms of the above
    const bool j5c_Date::operator<=(const j5c_Date &d) const noexcept
        {
            if (operator==(d)) return true;
            return operator<(d);
        };

    const bool j5c_Date::operator>=(const j5c_Date &d) const noexcept
    {
        if (operator==(d)) return true;
        return operator>(d);
    };

    const bool j5c_Date::operator>(const j5c_Date &d) const noexcept
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

    const bool j5c_Date::operator!=(const j5c_Date &d) const noexcept
    {
        j5c_Date t = *this;
        return !(t == d);
    };

    j5c_Date j5c_Date::getNext_Date() const noexcept
    {
        return this->internal_addDays(1);
    };

    j5c_Date j5c_Date::getPriorDate() const noexcept
    {
        return this->internal_subDays(1);
    };

    const j5c_Date j5c_Date::operator++(int) noexcept
    {
        // postfix++
        j5c_Date postfix{*this};
        j5c_Date newThis = this->getNext_Date();
        this->m_year  = newThis.m_year;
        this->m_month = newThis.m_month;
        this->m_day   = newThis.m_day;
        return postfix;
    };

    const j5c_Date  j5c_Date::operator--(int) noexcept
    {
        // postfix--
        j5c_Date postfix{*this};
        j5c_Date newThis = this->getPriorDate();
        this->m_year  = newThis.m_year;
        this->m_month = newThis.m_month;
        this->m_day   = newThis.m_day;
        return postfix;
    };

    const j5c_Date& j5c_Date::operator++() noexcept
    {
        // prefix++
        j5c_Date prefix = this->getNext_Date();
        this->m_year  = prefix.m_year;
        this->m_month = prefix.m_month;
        this->m_day   = prefix.m_day;
        return *this;

    };

    const j5c_Date& j5c_Date::operator--() noexcept
    {
        // prefix--
        j5c_Date prefix = this->getPriorDate();
        this->m_year  = prefix.m_year;
        this->m_month = prefix.m_month;
        this->m_day   = prefix.m_day;
        return *this;
    };

    std::string j5c_Date::padright(int width, int value) const noexcept
    {
        unsigned long w = static_cast<unsigned long>(width);
        std::string output = std::string{"0000"} + std::to_string(value);
        unsigned long len = output.length();
        return output.substr(len-w, len);
    }

    std::string j5c_Date::strDate() const noexcept
    {
        return
                  padright(4, m_year)  + '-'
                + padright(2, m_month) + '-'
                + padright(2, m_day);
    }

    std::ostream &operator<<(std::ostream &out, const j5c_Date &d)
    {
        return out
            << std::setw(4) << std::setfill('0') << d.m_year << '-'
            << std::setw(2) << std::setfill('0') << d.m_month << '-'
            << std::setw(2) << std::setfill('0') << d.m_day;
    }

}
