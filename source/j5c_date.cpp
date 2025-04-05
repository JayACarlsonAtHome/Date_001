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

    bool j5c_Date::isLeapYear(const int year) const noexcept
    {
        bool result = J5C_DSL_Code::isLeapYear(year);
        return result;
    }
    bool j5c_Date::isLeapYear() const noexcept
    {
        bool result = J5C_DSL_Code::isLeapYear(m_year);
        return result;
    }

    bool j5c_Date::isValidDate(const int year, const  int month, const int day) const noexcept
    {
        bool result = true;
        if ( year < MIN_YEAR || year > MAX_YEAR) {
            result = false;
        }
        if (month < 1 || month > 12 )
        {
            result = false;
        }
        if (day < 1 || day > 31)
        {
            result = false;
        }
        if (month == 2)
        {
            if (isLeapYear(year))
            {
                if (day > 29) result = false;
            }
            else
            {
                if (day > 28) result = false;
            }
        }
        else if (month == 4 || month == 6 || month == 9 || month == 11)
        {
            if (day > 30) result = false;
        }
        return result;
    }

    bool j5c_Date::isValid() const noexcept
    {
        bool result = this->isValidDate(this->m_year, this->m_month, this->m_day);
        return result;
    };

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

    j5c_Date::j5c_Date() noexcept {
        time_t nowTime = time(nullptr);
        bool valid = (nowTime != static_cast<time_t>(-1));
        if (valid) {
            struct tm* timeinfo = localtime(&nowTime);
            valid = (timeinfo != nullptr);
            if (valid) {
                m_year = timeinfo->tm_year + 1900;
                m_month = timeinfo->tm_mon + 1;
                m_day = timeinfo->tm_mday;
            }
        }
        if (!valid) {
            m_year = 0; m_month = 0; m_day = 0;
            cout_InvalidDate();
        };
    }

    j5c_Date::j5c_Date(int year, int dayOfTheYear) noexcept
    {
        set_y_d(year, dayOfTheYear);
    }

    void j5c_Date::set_y_d(const int year, const int dayOfTheYear) noexcept
    {
        m_year = year;
        m_month = 1;
        m_day = 1;
        bool valid = (dayOfTheYear > 0 && dayOfTheYear <= (isLeapYear(year) ? 366 : 365));
        if (valid) {
            int days = dayOfTheYear;
            bool leap = isLeapYear(year);
            if (leap && days == 60) {
                m_month = 2;
                m_day = 29;
            } else {
                if (leap && days > 60) days--;
                while (m_month < 12 && days > numberOfDaysBeforeMonth[m_month + 1]) {
                    m_month++;
                }
                days -= numberOfDaysBeforeMonth[m_month];
                m_day = days;
            }
        } else {
            m_year = 0; m_month = 0; m_day = 0;
            cout_InvalidDate();
        }    }

    void j5c_Date::adjustMonth(int& year, int& month, const int direction) noexcept
    {
        month += direction;
        if (month > 12)
        {
            month = 1;
            year++;
        }
        else if (month < 1)
        {
            month = 12;
            year--;
        }
    }

    int j5c_Date::daysToMonthEnd(const int year, const int month, const int day) noexcept
    {
        return j5c_Date::getDaysInMonth(year, month) - day + 1;
    }

    int j5c_Date::daysFromMonthStart(const int day) noexcept
    {
        return day;
    }

    int j5c_Date::getDaysInMonth(const int year, const int month) const noexcept
    {
        static const int numberOfDaysInMonth[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        int result = numberOfDaysInMonth[month];
        if (month == 2 && j5c_Date::isLeapYear(year))
        {
            result = 29;
        }
        return result;
    }

    int j5c_Date::getDaysInMonth() const noexcept
    {
        return getDaysInMonth(this->m_year, this->m_month);
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

    int j5c_Date::LeapYearsSinceYear0001(int year, const int month) const noexcept
        {
            // Check if the current getYear needs to be considered
            // for the count of leap years or not
            if (month <= 2) year--;

            // An year is a leap getYear if it is a multiple of 4,
            // multiple of 400 and not a multiple of 100.
            int result = ((year/4) - (year/100) + (year/400));
            return result;
        }

    int j5c_Date::daysSinceYear0001Day001(const int year, const int month, const int day) const noexcept
    {
        int n = std::numeric_limits<int>::min();
        bool valid = isValidDate(year, month, day);
        // initialize count using years and day
        if (valid) {
            n = (year * 365) + numberOfDaysBeforeMonth[month] + day;
            // Add a day for every leap getYear
            n += LeapYearsSinceYear0001(year, month);
        }
        return n;
    }

    int j5c_Date::getDaysDiff(const j5c_Date& dt2) const noexcept {

        int n1 = this->daysSinceYear0001Day001(m_year, m_month, m_day);
        int n2 = this->daysSinceYear0001Day001(dt2.getYear(), dt2.getMonth(), dt2.getDay());

        bool isN1Valid = (n1 != std::numeric_limits<int>::min());
        bool isN2Valid = (n2 != std::numeric_limits<int>::min());

        int result = std::numeric_limits<int>::min();
        if (isN1Valid && isN2Valid) {
                result = n2 - n1;
        }
        return result;
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
        int newMonth = m_month + months;
        int yearAdjust = 0;
        while (newMonth > 12) {
            newMonth -= 12;
            yearAdjust++;
        }
        while (newMonth < 1) {
            newMonth += 12;
            yearAdjust--;
        }
        int newYear = m_year + yearAdjust;
        bool valid = (newYear >= MIN_YEAR && newYear <= MAX_YEAR);
        if (valid) {
            m_year = newYear;
            m_month = newMonth;
            int maxDays = getDaysInMonth();
            if (m_day > maxDays) m_day = maxDays;
        } else {
            m_year = 0; m_month = 0; m_day = 0;
            cout_InvalidDate();
        }
    }

    j5c_Date j5c_Date::internal_addDays(int days) noexcept
{
        int Year = m_year;
        int Month = m_month;
        int Day = m_day;
        int remainingDays = days;

        //std::cout << "Start: Y=" << Year << ", M=" << Month << ", D=" << Day << ", Days to add: " << days << std::endl;
        while (remainingDays != 0)
        {
            //std::cout << "Loop: Y=" << Year << ", M=" << Month << ", D=" << Day << ", Remaining=" << remainingDays << std::endl;
            if (remainingDays > 0)
            {
                int daysLeft = daysToMonthEnd(Year, Month, Day);
                if (remainingDays >= daysLeft)
                {
                    remainingDays -= daysLeft;
                    adjustMonth(Year, Month, 1);
                    Day = 1;
                    //std::cout << "Add Month: Y=" << Year << ", M=" << Month << ", D=" << Day << ", Remaining=" << remainingDays << std::endl;
                }
                else
                {
                    Day += remainingDays;
                    remainingDays = 0;
                    //std::cout << "Add Done: Y=" << Year << ", M=" << Month << ", D=" << Day << std::endl;
                }
            }
            else if (remainingDays < 0)
            {
                int daysBack = daysFromMonthStart(Day);
                if (-remainingDays >= daysBack)
                {
                    remainingDays += daysBack;
                    adjustMonth(Year, Month, -1);
                    Day = j5c_Date::getDaysInMonth(Year, Month);
                    //std::cout << "Sub Month: Y=" << Year << ", M=" << Month << ", D=" << Day << ", Remaining=" << remainingDays << std::endl;
                }
                else
                {
                    Day += remainingDays;
                    remainingDays = 0;
                    //std::cout << "Sub Done: Y=" << Year << ", M=" << Month << ", D=" << Day << std::endl;
                }
            }
        }
    j5c_Date result;
    result.setYear(Year);
    result.setMonth(Month);
    result.setDay(Day);
    //std::cout << "End: " << result.strDate() << " (Expect: " << (days == 58 ? "2017-02-28" : days == 59 ? "2017-03-01" : days == 1 ? "2017-03-01" : "2016-12-31") << ")" << std::endl;
    return result;
}

    j5c_Date j5c_Date::add_Days(const int days) noexcept
    {
        return internal_addDays(days);  // Handles + and - now
    }
    int j5c_Date::getFirstDayOfYear() noexcept
    {
        return getDayOfWeek( m_year,1,1);
    }
    int j5c_Date::getDayOfTheYear() const noexcept
    {
        int DOTY = numberOfDaysBeforeMonth[m_month];
        DOTY += m_day;
        if (isLeapYear(m_year))
        {
            if (m_month > 2) DOTY++;
        }
        return DOTY;
    }

    int j5c_Date::getDayOfTheQuarter() const noexcept
    {
        int result = numberOfDaysBefore_forDayOfQuarter[m_month] + m_day;
        bool isLeap = isLeapYear(this->m_year);
        bool isQ1 = (m_month <= 3);
        if (isLeap) {
            if (isQ1) {
                if (m_month > 2) {
                    result = result + 1;
                }
            }
        }
        return result;
    }
    int j5c_Date::getDayOfWeek(const int year, const int month, const int day) noexcept
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
        int adjustedYear = year;
        int adjustedMonth = month;
        int result = -1;
        if (isValidDate(year, month, day))
        {
            if (month < 3)
            {
                adjustedYear -= 1;
            }
            result = (adjustedYear + adjustedYear/4 - adjustedYear/100 + adjustedYear/400 + t[adjustedMonth-1] + day) % 7;
        }
        return result;    }

    int j5c_Date::getDayOfWeek() noexcept
    {
        int result = getDayOfWeek(m_year, m_month, m_day);
        return result;
    }

    std::string j5c_Date::getDayText(unsigned int forcedLength) noexcept
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

    j5c_Date j5c_Date::getPriorDate() noexcept {
        return internal_addDays(-1);  // Subtract 1 day
    }

    const j5c_Date& j5c_Date::operator++() noexcept {
        *this = internal_addDays(1);
        return *this;
    }

    const j5c_Date& j5c_Date::operator--() noexcept {
        *this = internal_addDays(-1);  // Subtract 1 day
        return *this;
    }
    const j5c_Date j5c_Date::operator++(int) noexcept {
        j5c_Date temp(*this);
        *this = internal_addDays(1);
        return temp;
    }

    const j5c_Date j5c_Date::operator--(int) noexcept {  // Postfix
        j5c_Date temp(*this);
        *this = internal_addDays(-1);  // Subtract 1 day
        return temp;
    }

    std::string j5c_Date::padright(const int width, const int value) const noexcept
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

