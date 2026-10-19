//
// jc_date_utils.hpp -- small helpers for std::chrono::year_month_day
//
// Copyright (C) 2026  Jay A. Carlson.
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
// C++20 std::chrono already has the calendar: year_month_day, leap years, weekday,
//   day arithmetic (sys_days), comparisons, formatting and parsing.
// This header adds only what chrono lacks, as free functions on the standard type,
//   so code can use year_month_day everywhere and reach for these when needed.
//   Header only; everything except the string and clock helpers is constexpr.
//
// Invalid input (a date that is not ok()) gives 0 from the int helpers and std::nullopt
//   from the optional ones. Nothing here prints or throws.
//
// The rules match jc313_Date (source/jc313_Date.h), and the tests check them against it.
//

#pragma once

#include <array>
#include <chrono>
#include <ctime>
#include <optional>
#include <string>
#include <string_view>

namespace jc_date {

    namespace chr = std::chrono;
    using ymd = chr::year_month_day;

    // ok() and inside years 1 - 9999 (the range jc313_Date supports; chrono allows -32767 - 32767)
    constexpr bool is_valid_1_9999(const ymd& d) noexcept
    {
        return d.ok() && d.year() >= chr::year{1} && d.year() <= chr::year{9999};
    }

    // 1 - 366
    constexpr int day_of_year(const ymd& d) noexcept
    {
        int result = 0;
        if (d.ok())
        {
            result = static_cast<int>((chr::sys_days{d} - chr::sys_days{d.year() / chr::January / 1}).count()) + 1;
        }
        return result;
    }

    // 1 - 4
    constexpr int quarter(const ymd& d) noexcept
    {
        int result = 0;
        if (d.ok())
        {
            result = static_cast<int>((static_cast<unsigned>(d.month()) - 1) / 3) + 1;
        }
        return result;
    }

    // 1 - 92 (day 1 is the first day of Jan, Apr, Jul or Oct)
    constexpr int day_of_quarter(const ymd& d) noexcept
    {
        int result = 0;
        if (d.ok())
        {
            const unsigned firstMonth = (static_cast<unsigned>(d.month()) - 1) / 3 * 3 + 1;
            const chr::sys_days start{d.year() / chr::month{firstMonth} / 1};
            result = static_cast<int>((chr::sys_days{d} - start).count()) + 1;
        }
        return result;
    }

    // whole years from birth to on (negative when on is before birth);
    //   a 29 Feb birthday counts on 1 Mar in other years
    constexpr std::optional<int> age(const ymd& birth, const ymd& on) noexcept
    {
        std::optional<int> result;
        if (birth.ok() && on.ok())
        {
            int years = static_cast<int>(on.year()) - static_cast<int>(birth.year());
            const bool beforeBirthday = (on.month() < birth.month())
                                     || (on.month() == birth.month() && on.day() < birth.day());
            if (beforeBirthday)
            {
                years--;
            }
            result = years;
        }
        return result;
    }

    // add months, keeping the day but clamping it to the new month's last day
    //   (2017-01-31 + 1 month = 2017-02-28; chrono's own d + months{1} gives 2017-02-31, not ok())
    constexpr std::optional<ymd> add_months_clamped(const ymd& d, const int months) noexcept
    {
        std::optional<ymd> result;
        if (d.ok())
        {
            const chr::year_month ym = chr::year_month{d.year(), d.month()} + chr::months{months};
            const chr::day lastDay = (ym / chr::last).day();
            const ymd moved{ym / (d.day() < lastDay ? d.day() : lastDay)};
            if (moved.ok())
            {
                result = moved;
            }
        }
        return result;
    }

    // days can be negative; the answer is checked against years 1 - 9999
    //   (in 64 bits, so a huge days cannot overflow the chrono day count)
    constexpr std::optional<ymd> add_days_1_9999(const ymd& d, const long long days) noexcept
    {
        constexpr long long firstDay = chr::sys_days{chr::year{1}    / chr::January  /  1}.time_since_epoch().count();
        constexpr long long lastDay  = chr::sys_days{chr::year{9999} / chr::December / 31}.time_since_epoch().count();
        std::optional<ymd> result;
        if (is_valid_1_9999(d))
        {
            const long long target = chr::sys_days{d}.time_since_epoch().count() + days;
            if (target >= firstDay && target <= lastDay)
            {
                result = ymd{chr::sys_days{chr::days{target}}};
            }
        }
        return result;
    }

    // b - a in days
    constexpr std::optional<int> days_between(const ymd& a, const ymd& b) noexcept
    {
        std::optional<int> result;
        if (a.ok() && b.ok())
        {
            result = static_cast<int>((chr::sys_days{b} - chr::sys_days{a}).count());
        }
        return result;
    }

    // 0 = Sunday ... 6 = Saturday, -1 when not ok()
    constexpr int day_of_week(const ymd& d) noexcept
    {
        int result = -1;
        if (d.ok())
        {
            result = static_cast<int>(chr::weekday{chr::sys_days{d}}.c_encoding());
        }
        return result;
    }

    // English day name, not locale dependent (chrono's %A follows the locale).
    //   width 0 = as is; otherwise padded with spaces or cut to width (at most 50).
    //   Not ok() gives "Invalid DOW" ("Invalid Dw" at width 10, as jc313_Date does).
    inline std::string day_name(const ymd& d, unsigned int width = 0)
    {
        static constexpr std::array<std::string_view, 7> names{
            "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
        const int dow = day_of_week(d);
        std::string result{dow >= 0 ? names[static_cast<std::size_t>(dow)] : std::string_view{"Invalid DOW"}};
        if (width > 50)
        {
            width = 50;
        }
        if (width > 0)
        {
            result.resize(width, ' ');
            if (result == "Invalid DO")
            {
                result = "Invalid Dw";
            }
        }
        return result;
    }

    // yyyy-mm-dd, zero padded (year 1 is 0001-01-01)
    inline std::string to_string(const ymd& d)
    {
        const auto pad = [](unsigned value, std::size_t width)
        {
            std::string text = std::to_string(value);
            return std::string(text.size() < width ? width - text.size() : 0, '0') + text;
        };
        const int y = static_cast<int>(d.year());
        return (y < 0 ? "-" : "") + pad(static_cast<unsigned>(y < 0 ? -y : y), 4) + '-'
             + pad(static_cast<unsigned>(d.month()), 2) + '-'
             + pad(static_cast<unsigned>(d.day()), 2);
    }

    // yyyy-mm-dd with matching '-', '.', '/' or '_' delimiters and nothing else;
    //   1 - 4 digit year, 1 - 2 digit month and day; the date must be valid in 1 - 9999
    constexpr std::optional<ymd> parse_ymd(const std::string_view text) noexcept
    {
        std::optional<ymd> result;
        int fields[3] = {0, 0, 0};
        const std::size_t maxDigits[3] = {4, 2, 2};
        char delim = 0;
        std::size_t field = 0;
        std::size_t digits = 0;
        bool good = !text.empty();
        for (std::size_t i = 0; good && i < text.size(); ++i)
        {
            const char c = text[i];
            if (c >= '0' && c <= '9')
            {
                fields[field] = fields[field] * 10 + (c - '0');
                digits++;
                good = (digits <= maxDigits[field]);
            }
            else
            {
                const bool isDelim = (c == '-' || c == '.' || c == '/' || c == '_');
                good = isDelim && digits > 0 && field < 2 && (delim == 0 || c == delim);
                delim = c;
                field++;
                digits = 0;
            }
        }
        good = good && field == 2 && digits > 0;
        if (good)
        {
            const ymd d{chr::year{fields[0]}, chr::month{static_cast<unsigned>(fields[1])},
                        chr::day{static_cast<unsigned>(fields[2])}};
            if (is_valid_1_9999(d))
            {
                result = d;
            }
        }
        return result;
    }

    // today in local time (thread-safe localtime_r; sys_days would be the UTC date)
    inline ymd today_local() noexcept
    {
        ymd result{};
        const std::time_t now = std::time(nullptr);
        std::tm parts{};
        if (now != static_cast<std::time_t>(-1) && localtime_r(&now, &parts) != nullptr)
        {
            result = ymd{chr::year{parts.tm_year + 1900}, chr::month{static_cast<unsigned>(parts.tm_mon + 1)},
                         chr::day{static_cast<unsigned>(parts.tm_mday)}};
        }
        return result;
    }

}
