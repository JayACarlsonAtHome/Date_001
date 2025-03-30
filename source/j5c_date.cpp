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

// Note originally I included a lot of early returns in methods
//   because is just made it easier and cleaner to do so.
//   however I have become aware of requirements that in some cases
//   do not allow any early returns.
//   So in order to be more compliant with those requirements I
//      am starting the process to remove the early returns.
//
//   Status -- Just starting to remove early returns.


#include "j5c_date.h"

namespace J5C_DSL_Code {


    void j5c_Date::cout_InvalidDate() const noexcept
    {
        std::ostringstream ss;

        ss << "!!! *** Invalid Date *** !!! -> "
                  << std::setw(4) << std::setfill('0') << m_year << '-'
                  << std::setw(2) << std::setfill('0') << m_month << '-'
                  << std::setw(2) << std::setfill('0') << m_day << std::endl;

        std::cerr << ss.str();
        std::cout << ss.str();
        
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
        // assuming we can continue
        //  and we are starting with a real date
        bool cont = true;
        bool invalid = false;

        m_year = year;
        m_month = 0;
        if (dayOfTheYear == 0)
        {
            invalid = true;
            cont = false;
        }

        if (cont) {
            int workingDayOfTheYear = dayOfTheYear;
            bool isaLeapYear = isLeapYear(this->m_year);
            if (isaLeapYear) {
                //Checking for leap getYear day
                if (workingDayOfTheYear == 60) {
                    m_month = 2;
                    m_day = 29;
                    cont = false;
                } else {
                    if (workingDayOfTheYear > 60) {
                        workingDayOfTheYear--;
                    }
                }
            }
            if (cont) {
                while ((m_month < 12) && (cont)) {
                    workingDayOfTheYear -= numberOfDaysInMonth[m_month];
                    m_month++;
                    if (workingDayOfTheYear <= numberOfDaysInMonth[m_month]) {
                        m_day = workingDayOfTheYear;
                        cont = false;
                    }
                }
            }
        }
        if (invalid)
        {
            cout_InvalidDate();
            m_year  = 0;
            m_month = 0;
            m_day   = 0;
        }
        return;
    }


    int j5c_Date::getDaysInMonth() const noexcept
    {
        static const int numberOfDaysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        int result = numberOfDaysInMonth[this->m_month - 1];  // -1 for 0-based array
        if (this->m_month == 2 && isLeapYear(this->m_year))
        {
            result = 29;
        }
        return result;
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

    void j5c_Date::addMonths(int months) noexcept
    {
        int newMonth = this->m_month + months;
        int yearAdjust = 0;

        while (newMonth > 12)
        {
            newMonth -= 12;
            yearAdjust += 1;
        }
        while (newMonth < 1)
        {
            newMonth += 12;
            yearAdjust -= 1;
        }

        this->m_month = newMonth;
        this->m_year += yearAdjust;

        // Adjust day if it exceeds new month's max
        int maxDays = J5C_DSL_Code::numberOfDaysInMonth[this->m_month];
        if (this->m_day > maxDays)
        {
            this->m_day = maxDays;
        }
    }

    j5c_Date j5c_Date::internal_addDays(int days) noexcept
    {
        j5c_Date result(*this);
        int remainingDays = days;
        while (remainingDays != 0)
        {
            int daysInThisMonth = getDaysInMonth();
            int daysLeftInMonth = daysInThisMonth - result.m_day + 1;
            if (remainingDays < 0)
            {
                daysLeftInMonth = result.m_day;
                result.m_day = 1;
                result.addMonths(-1);
                remainingDays += daysLeftInMonth;
            }
            else if (remainingDays >= daysLeftInMonth)
            {
                result.addMonths(1);  // Roll month first
                result.m_day = 1;       // Then set day
                remainingDays -= daysLeftInMonth;
            }
            else
            {
                result.m_day += remainingDays;
                remainingDays = 0;
            }
        }
        return result;
    }

    j5c_Date j5c_Date::internal_subDays(int days) noexcept
    {
        j5c_Date result(*this);
        int remainingDays = -days;  // Flip to negative for subtraction
        while (remainingDays != 0)
        {
            int daysInThisMonth = getDaysInMonth();
            int daysLeftInMonth = result.m_day;  // Days to start of month
            if (remainingDays > 0)  // Subtracting days
            {
                if (remainingDays >= daysLeftInMonth)
                {
                    result.addMonths(-1);
                    result.m_day = getDaysInMonth();
                    remainingDays -= daysLeftInMonth;
                }
                else
                {
                    result.m_day -= remainingDays;
                    remainingDays = 0;
                }
            }
            else  // Adding days (unlikely, but complete)
            {
                int daysToEnd = daysInThisMonth - result.m_day + 1;
                result.addMonths(1);
                result.m_day = 1;
                remainingDays += daysToEnd;
            }
        }
        return result;
    }
    j5c_Date j5c_Date::add_Days(int days) noexcept
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

    int j5c_Date::getFirstDayOfYear() const noexcept {
        int yearConversion = 0;
        int result = -1;
        if (m_year < 1) {
            cout_InvalidDate();
        } else {
            yearConversion = (m_year - 1) % 400; // 0-399 cycle
            result = firstDayOfYear[yearConversion];
        }
        return result;
    }

    int j5c_Date::getDayOfTheYear() const noexcept
    {

        int DOTY = 0;
        if (m_month < 3) {
            DOTY = numberOfDaysBeforeMonth[m_month] + m_day;
        } else {
            int LeapYearConditionalOffset = 0;
            // although bool is usually converted to (int) 1, lets not assume that for portability
            if (isLeapYear(this->m_year)) {
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
            if (isLeapYear(this->m_year)) {
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

        int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
        int adjustedYear = this->m_year;
        int adjustedMonth = this->m_month;
        if (adjustedMonth < 3)
        {
            adjustedYear -= 1;
        }
        int result = (adjustedYear + adjustedYear/4 - adjustedYear/100 + adjustedYear/400 + t[adjustedMonth-1] + this->m_day) % 7;
        return result;
    }

    std::string j5c_Date::getDayText(unsigned int forcedLength = 0) const noexcept
    {
        int DOW = this->getDayOfWeek();
        std::string DOWT = "Invalid Date";
        std::string result;
        if (forcedLength > 50)
        {
            forcedLength = 50;
        }

        if (DOW == -1) DOWT = "Invalid DOW";
        if (DOW ==  0) DOWT = "Sunday";
        if (DOW ==  1) DOWT = "Monday";
        if (DOW ==  2) DOWT = "Tuesday";
        if (DOW ==  3) DOWT = "Wednesday";
        if (DOW ==  4) DOWT = "Thursday";
        if (DOW ==  5) DOWT = "Friday";
        if (DOW ==  6) DOWT = "Saturday";
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
                // Most invalid dates are either a compromise of length
                //   or at least interpretable as a result of truncation.
                //   To me it seems "Invalid DO" is not very self descriptive
                //   So we are going to change it to Invalid Dw"
                //   Inv             = Understandable
                //   Inva            = Understandable
                //   Inval           = Understandable
                //   Invali          = Understandable
                //   Invalid         = Understandable
                //   Inavalid_       = Understandable
                //   Invalid D       = Understandable
                //   Invalid DO      = Confusing -- what is this...
                //   Invalid Dw      = Changed from confusing to Understandable
                //   Invalid DOW     = Understandable

                if (result == "Invalid DO") result = "Invalid Dw";
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
        bool result = isValidDate(this->m_day, this->m_month, this->m_day);
        return result;
    };

    const bool j5c_Date::operator==(const j5c_Date &d) const noexcept
    {
        return ((m_year == d.m_year) && (m_month == d.m_month) && (m_day == d.m_day));
    };

    const bool j5c_Date::operator<(const j5c_Date &d) const noexcept
    {
        bool cont   = true;
        bool result = false;

        if (m_year == d.m_year)
        {
            if (m_month == d.m_month)
            {
                result = (m_day < d.m_day);
            }
            else
            {
                result = (m_month < d.m_month);
            };
            cont = false;
        }
        if (cont)
        {
            result = (m_year < d.m_year);
        };
        return result;
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
        bool cont   = true;
        bool result = false;

        if (m_year == d.m_year) { // same getYear
            if (m_month == d.m_month) { // same getMonth
                result = (m_day > d.m_day);
            } else {
                result = (m_month > d.m_month);
            };
            cont = false;
        }
        if (cont)
        { // different getYear
            result = (m_year > d.m_year);
        };
        return result;
    };

    const bool j5c_Date::operator!=(const j5c_Date &d) const noexcept
    {
        j5c_Date t = *this;
        return !(t == d);
    };

    j5c_Date j5c_Date::getNext_Date() noexcept
    {
        return internal_addDays(1);
    };

    j5c_Date j5c_Date::getPriorDate() noexcept
    {
        return internal_subDays(1);
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

    std::ostream& operator<<(std::ostream &out, const j5c_Date &d)
    {
        out << std::setw(4) << std::setfill('0') << d.m_year << '-'
            << std::setw(2) << std::setfill('0') << d.m_month << '-'
            << std::setw(2) << std::setfill('0') << d.m_day;
        return out;
    }

    std::istream& operator>>(std::istream &ins, j5c_Date &d)
    {
        // store value in temporary to validate before assignment
        char delim1 = 0;
        char delim2 = 0;

        ins >> d.m_year >> delim1 >>  d.m_month >> delim2 >> d.m_day;
        bool check0 = ((delim1 == '_') && (delim2 == '_'));
        bool check1 = ((delim1 == '-') && (delim2 == '-'));
        bool check2 = ((delim1 == '.') && (delim2 == '.'));
        bool check3 = ((delim1 == '/') && (delim2 == '/'));

        if (!(check0 || check1 || check2 || check3))
        {
            // this will make the date invalid
            d.m_year = 0;
        }

        if (!d.isValid())
        {
            d.m_year  = 0;
            d.m_month = 0;
            d.m_day   = 0;
            std::cout << "!!! Warning --> Invalid Date Entered !!!" << std::endl;
        }
        return ins;
    }
}
