//
// jc313_Date Class Implementation File
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

// Note originally I included a lot of early returns in methods
//   because is just made it easier and cleaner to do so.
//   however I have become aware of requirements that in some cases
//   do not allow any early returns.
//   So in order to be more compliant with those requirements I
//      removed the early returns.
//
//   Status -- No early returns.
//
// Day counting (add_Days, getDaysDiff, day of week) is done by std::chrono::sys_days,
//   so it takes the same time for 1 day or 3 million days.


#include "jc313_Date.h"

#include <ctime>
#include <iomanip>
#include <iostream>

namespace JC313_DSL_Code {

    namespace chr = std::chrono;

    bool jc313_Date::isLeapYear(const int year) const noexcept
    {
        bool result = JC313_DSL_Code::isLeapYear(year);
        return result;
    }
    bool jc313_Date::isLeapYear() const noexcept
    {
        bool result = JC313_DSL_Code::isLeapYear(m_year);
        return result;
    }

    bool jc313_Date::isValidDate(const int year, const  int month, const int day) const noexcept
    {
        bool result = (year >= MIN_YEAR && year <= MAX_YEAR)
                   && (month >= 1 && month <= 12)
                   && (day >= 1 && day <= getDaysInMonth(year, month));
        return result;
    }

    bool jc313_Date::isValid() const noexcept
    {
        bool result = this->isValidDate(this->m_year, this->m_month, this->m_day);
        return result;
    };

    chr::sys_days jc313_Date::toSysDays() const noexcept
    {
        return chr::sys_days{chr::year{m_year} / chr::month{static_cast<unsigned>(m_month)}
                                               / chr::day{static_cast<unsigned>(m_day)}};
    }

    jc313_Date jc313_Date::fromSysDays(const chr::sys_days days) noexcept
    {
        const chr::year_month_day ymd{days};
        // the constructor turns a year outside 1 - 9999 into 0000-00-00
        return jc313_Date{static_cast<int>(ymd.year()),
                          static_cast<int>(static_cast<unsigned>(ymd.month())),
                          static_cast<int>(static_cast<unsigned>(ymd.day()))};
    }

    jc313_Date::jc313_Date() noexcept
    {
        time_t nowTime = time(nullptr);
        struct tm timeinfo {};
        bool valid = (nowTime != static_cast<time_t>(-1))
                  && (localtime_r(&nowTime, &timeinfo) != nullptr);   // thread-safe localtime
        if (valid)
        {
            m_year  = timeinfo.tm_year + 1900;
            m_month = timeinfo.tm_mon + 1;
            m_day   = timeinfo.tm_mday;
        }
        if (!isValid())
        {
            setInvalid();
        }
    }

    jc313_Date::jc313_Date(int year, int month, int day) noexcept
        : m_year(year), m_month(month), m_day(day)
    {
        if (!isValid())
        {
            setInvalid();
        }
    }

    jc313_Date::jc313_Date(int year, int dayOfTheYear) noexcept
    {
        set_y_d(year, dayOfTheYear);
    }

    std::optional<jc313_Date> jc313_Date::make(int year, int month, int day) noexcept
    {
        std::optional<jc313_Date> result;
        jc313_Date d{year, month, day};
        if (d.isValid())
        {
            result = d;
        }
        return result;
    }

    void jc313_Date::set_y_d(const int year, const int dayOfTheYear) noexcept
    {
        bool valid = (year >= MIN_YEAR && year <= MAX_YEAR)
                  && (dayOfTheYear > 0 && dayOfTheYear <= (JC313_DSL_Code::isLeapYear(year) ? 366 : 365));
        if (valid)
        {
            const chr::sys_days jan1{chr::year{year} / chr::January / 1};
            *this = fromSysDays(jan1 + chr::days{dayOfTheYear - 1});
        }
        else
        {
            setInvalid();
        }
    }

    void jc313_Date::adjustMonth(int& year, int& month, const int direction) noexcept
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

    int jc313_Date::daysToMonthEnd(const int year, const int month, const int day) const noexcept
    {
        return jc313_Date::getDaysInMonth(year, month) - day + 1;
    }

    int jc313_Date::daysFromMonthStart(const int day) noexcept
    {
        return day;
    }

    int jc313_Date::getDaysInMonth(const int year, const int month) const noexcept
    {
        static constexpr int numberOfDaysInMonth[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        int result = 0;   // month out of range
        if (month >= 1 && month <= 12)
        {
            result = numberOfDaysInMonth[month];
        }
        if (month == 2 && jc313_Date::isLeapYear(year))
        {
            result = 29;
        }
        return result;
    }

    int jc313_Date::getDaysInMonth() const noexcept
    {
        return getDaysInMonth(this->m_year, this->m_month);
    }

    int jc313_Date::getDaysDiff(const jc313_Date& dt2) const noexcept
    {
        int result = std::numeric_limits<int>::min();
        if (this->isValid() && dt2.isValid())
        {
            result = static_cast<int>((dt2.toSysDays() - this->toSysDays()).count());
        }
        return result;
    }

    int jc313_Date::getAge() const noexcept
    {
        const jc313_Date now{};
        return getAge(now);
    }

    int jc313_Date::getAge(const jc313_Date& onDate) const noexcept
    {
        // whole years: the year difference, less one if onDate is before this year's birthday
        //   (a 29 Feb birthday counts on 1 Mar in other years)
        int result = std::numeric_limits<int>::min();
        if (this->isValid() && onDate.isValid())
        {
            result = onDate.m_year - m_year;
            bool beforeBirthday = (onDate.m_month < m_month)
                               || (onDate.m_month == m_month && onDate.m_day < m_day);
            if (beforeBirthday)
            {
                result--;
            }
        }
        return result;
    }

    void jc313_Date::addMonths(int months) noexcept
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
            setInvalid();
        }
    }

    jc313_Date jc313_Date::getNext_Date() const noexcept {
        return add_Days(1);
    }

    jc313_Date jc313_Date::getPriorDate() const noexcept {
        return add_Days(-1);
    }

    jc313_Date jc313_Date::add_Days(int days) const noexcept
    {
        // the range check is in 64 bits so a huge days cannot overflow the chrono day count
        static const long long firstDay = chr::sys_days{chr::year{MIN_YEAR} / chr::January  /  1}.time_since_epoch().count();
        static const long long lastDay  = chr::sys_days{chr::year{MAX_YEAR} / chr::December / 31}.time_since_epoch().count();
        jc313_Date result{0, 0, 0};   // 0000-00-00 unless the answer is in range
        if (isValid())
        {
            const long long target = toSysDays().time_since_epoch().count() + static_cast<long long>(days);
            if (target >= firstDay && target <= lastDay)
            {
                result = fromSysDays(chr::sys_days{chr::days{target}});
            }
        }
        return result;
    }

    int jc313_Date::getFirstDayOfYear() const noexcept
    {
        return getDayOfWeek( m_year,1,1);
    }

    int jc313_Date::getDayOfTheYear() const noexcept
    {
        int DOTY = 0;   // month out of range
        if (m_month >= 1 && m_month <= 12)
        {
            DOTY = numberOfDaysBeforeMonth[m_month] + m_day;
            if (isLeapYear(m_year))
            {
                if (m_month > 2) DOTY++;
            }
        }
        return DOTY;
    }

    int jc313_Date::getDayOfTheQuarter() const noexcept
    {
        int result = 0;   // month out of range
        if (m_month >= 1 && m_month <= 12)
        {
            result = numberOfDaysBefore_forDayOfQuarter[m_month] + m_day;
        }
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

    int jc313_Date::getDayOfWeek(const int year, const int month, const int day) const noexcept
    {
        // -1 = invalid DOW
        // 0 = Sunday
        // 1 = Monday
        // 2 = Tuesday
        // 3 = Wednesday
        // 4 = Thursday
        // 5 = Friday
        // 6 = Saturday
        int result = -1;
        const jc313_Date d{year, month, day};
        if (d.isValid())
        {
            result = static_cast<int>(chr::weekday{d.toSysDays()}.c_encoding());   // 0 = Sunday
        }
        return result;
    }

    int jc313_Date::getDayOfWeek() const noexcept
    {
        int result = getDayOfWeek(m_year, m_month, m_day);
        return result;
    }

    std::string jc313_Date::getDayText(unsigned int forcedLength) const
    {
        static const char* const names[7] = {"Sunday", "Monday", "Tuesday", "Wednesday",
                                             "Thursday", "Friday", "Saturday"};
        int DOW = this->getDayOfWeek();
        std::string DOWT = (DOW >= 0 && DOW <= 6) ? names[DOW] : "Invalid DOW";
        std::string result;
        if (forcedLength > 50)
        {
            forcedLength = 50;
        }

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

    int jc313_Date::getQuarter() const noexcept
    {
        int quarter = 0;
        if (m_month > 0) quarter++;
        if (m_month > 3) quarter++;
        if (m_month > 6) quarter++;
        if (m_month > 9) quarter++;
        return quarter;
    }

    jc313_Date& jc313_Date::operator++() noexcept {
        *this = add_Days(1);
        return *this;
    }

    jc313_Date& jc313_Date::operator--() noexcept {
        *this = add_Days(-1);  // Subtract 1 day
        return *this;
    }
    jc313_Date jc313_Date::operator++(int) noexcept {
        jc313_Date temp(*this);
        *this = add_Days(1);
        return temp;
    }

    jc313_Date jc313_Date::operator--(int) noexcept {  // Postfix
        jc313_Date temp(*this);
        *this = add_Days(-1);  // Subtract 1 day
        return temp;
    }

    std::string jc313_Date::padright(const int width, const int value)
    {
        unsigned long w = static_cast<unsigned long>(width);
        std::string output = std::string{"0000"} + std::to_string(value);
        unsigned long len = output.length();
        return output.substr(len-w, len);
    }

    std::string jc313_Date::strDate() const
    {
        return
                  padright(4, m_year)  + '-'
                + padright(2, m_month) + '-'
                + padright(2, m_day);
    }

    std::ostream& operator<<(std::ostream &out, const jc313_Date &d)
    {
        const char oldFill = out.fill('0');   // setfill is sticky: put the caller's fill back
        out << std::setw(4) << d.m_year << '-'
            << std::setw(2) << d.m_month << '-'
            << std::setw(2) << d.m_day;
        out.fill(oldFill);
        return out;
    }

    std::istream& operator>>(std::istream &ins, jc313_Date &d)
    {
        // read into temporaries; yyyy-mm-dd with matching '-', '.', '/' or '_' delimiters
        int year = 0, month = 0, day = 0;
        char delim1 = 0;
        char delim2 = 0;

        ins >> year >> delim1 >> month >> delim2 >> day;
        bool delimsOk = (delim1 == delim2)
                     && (delim1 == '-' || delim1 == '.' || delim1 == '/' || delim1 == '_');

        d = jc313_Date{year, month, day};   // 0000-00-00 when invalid
        if (!ins || !delimsOk || !d.isValid())
        {
            d.setInvalid();
            ins.setstate(std::ios::failbit);   // the caller checks the stream, as for any >>
        }
        return ins;
    }
}
