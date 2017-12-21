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

I believe I have tested 99% or more of all common scenarios, but before you use this class for anything more than 
a school project be sure to analyse the tests, test coverage, test results, etc for yourself and see if they are 
acceptable for your requirements.  Even though at the time of this writing all tests that I created pass.  That doesn't
mean that I covered every possible scenario, or that I coded each test correctly.
So what I am saying is this is free to use (as per license) but I do not guarantee the results. 


/Users/jaycarlson/git/Date_001/cmake-build-debug/Date

Starting Testing: ... Please be patient...
===============================================================================
All tests passed (28660 assertions in 176 test cases)


Process finished with exit code 0


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

