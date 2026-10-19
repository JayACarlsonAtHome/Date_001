# Date_001

I started writing C++ code again, and I was making a simple person class, and I realized that there wasn't a simple date
class without using boost, and that seemed like a little bit of overkill just for dates.
(After all the work I put into the Date class maybe it wouldn't have been overkill)

But anyhow building this class improved my knowledge of C++ a great deal.
A Date class would be very easy to have a lot of mistakes, errors, and bugs.

Originally I wrote my own testing framework.
Later I heard of Catch2 (also on Github) and tried that.
I was and am pleased with Catch2, but unfortunately with the number of tests, it seems to be slowing down CLion
more than I appreciate.  So if you don't want to be annoyed, please comment out a bunch of tests.
(The tests now use Catch2 v3.8.1, vendored as `source/catch_amalgamated.hpp/.cpp`. Catch2 itself is compiled
once, so editing a test only rebuilds the tests. `CATCH_CONFIG_MAIN 0` in `test_main.cpp` still skips them.)

I believe I have tested 99% or more of all common scenarios, but before you use this class for anything more than 
a school project be sure to analyse the tests, test coverage, test results, etc for yourself and see if they are 
acceptable for your requirements.  Even though at the time of this writing all tests that I created pass.  That doesn't
mean that I covered every possible scenario, or that I coded each test correctly.
So what I am saying is this is free to use (as per license) but I do not guarantee the results. 


## Build and test

```
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build      # or run ./build/Date directly
```

Expected output:

```
Starting Testing: ... Please be patient...
===============================================================================
All tests passed (29460 assertions in 185 test cases)
```

One of those tests walks every date from 0001-01-01 to 9999-12-31 (3,652,059 dates), so a run takes about half
a second. Needs a C++23 compiler (GCC 13+ or clang 17+; tested with GCC 15 and clang 21).

`./build/date_demo` prints the day of the week for every date in 2017-2020 (from `main.cpp`).

## Using the class

```cpp
#include "source/jc313_Date.h"
using namespace JC313_DSL_Code;

jc313_Date d{2017, 3, 4};                 // an invalid year/month/day gives 0000-00-00
if (auto checked = jc313_Date::make(2017, 2, 29)) { /* only runs for a valid date */ }
jc313_Date later = d.add_Days(1000);       // negative counts go back; out of range gives 0000-00-00
int age = jc313_Date{2000, 10, 19}.getAge(jc313_Date{2026, 10, 19});   // 26
std::cin >> d;                             // yyyy-mm-dd (or . / _); bad input sets failbit
```

- Valid dates are years 1 - 9999 in the proleptic Gregorian calendar (see the notes below).
- The class never prints. Anything that would give an invalid date gives 0000-00-00 instead, so check
  `isValid()`. `setYear/setMonth/setDay` do not validate, so check `isValid()` after them too.
- An invalid date answers -1 from `getDayOfWeek()`, "Invalid DOW" from `getDayText()`, 0 from
  `getDaysInMonth/getDayOfTheYear/getDayOfTheQuarter`, and INT_MIN from `getDaysDiff/getAge`.
- `getAge()` counts whole years up to the birthday; a 29 Feb birthday counts on 1 Mar in other years.
- Day arithmetic and the day of the week come from `std::chrono`, so `add_Days(1000000)` is as quick as `add_Days(1)`.
- `==, !=, <, >, <=, >=` all come from one defaulted `operator<=>` (year, then month, then day).


Other Notes:

Here is a brief history from the MacBook Pro dictionary
--Gregorian Calendar--
To bring the calendar back into line with the solar year, 10 days were suppressed, and centenary years were made leap 
years only if they were divisible by 400. England did not adopt the reformed calendar until 1752, 
by which time 11 days had to be suppressed. At the same time, New Year's Day was changed 
from March 25 to January 1, and dates using the new calendar were designated ‘New Style.’.  
--End of entry...

This history is very short and insufficient to cover all cases from a programming perspective 
because countries came into and when out of existance, the countries didn't adopt the calendar all at the same time, 
although the timing was probably pretty close for "Catholic" nations.

So--> 
I am considering valid years to be 1 to 9999.  It may be extreme, but no one says you have to use all those years.
Anyhow the calenders across the world were not standardized for a very long time.  There are many other implementations
that will have Julian Calanders and Gregorian calendars.  My philosophy is this:  There
were a lot of countries in the world, that had different calendar systems, and adopted
our current system at different times. It would be impossible to correctly implement 
all the different possibilities for each country until the beginning of time.
(Although the boost library probably comes as close as possible).  Let it also be known that
at some future date they (some international body) may have to add or subtract a day or two
to keep the calender in sync with the seasons of the year.  This also brings up another point
and that is that the period of the Earth revolving around the Sun is not constant, although it is pretty close
which is a totally relative, and non scientific definition.  The Sun and the 
Earth get tugs and pulls of gravity from a variety of sources, and the affect is there is no constant 
when it comes to time in the universe. 




I would like an email if you like this class, or hate it, or somewhere  in between.
There are some more methods that could be made, but I will most likely be moving on to other classes, and using this as is,
and improving it once in a while at this point, and continue to write verification tests.

