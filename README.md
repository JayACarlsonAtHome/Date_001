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

I have not written and tested all the parts of the date class yet, so if you use it, check the parts you are using for accuracy.

Other Notes:
I am considering valid years to be 1 to 9999.  It may be extreme, but no one says you have to use all those years.
Anyhow the calenders are and were not constant the whole time, but if you stay with years greater then 1600 you should be ok.

I would like an email if you like this class, or hate it, or somewhere inbetween.
There are some more methods that could be made, but I will most likely be moving on to other classes, and using this as is,
and improving it once in a while at this point, and continue to write verifcation tests.


