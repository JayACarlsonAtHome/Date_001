#include "source/catch2.h"
#include "source/j5c_date.h"

using namespace J5C_DSL_Code;

j5c_Date d1;
j5c_Date d2;

TEST_CASE("Dates are equal", "[j5c_Date]")
{
    REQUIRE(d1 == d2);
}

bool isLeapYear(int year)
{
    j5c_Date d(year, 01, 01);
    return d.isLeapYear();
}

#include "test/year/test_leap_year_0001_0500.tdd"
#include "test/year/test_leap_year_0501_1000.tdd"
#include "test/year/test_leap_year_1001_1500.tdd"
#include "test/year/test_leap_year_1501_2000.tdd"
#include "test/year/test_leap_year_2001_2500.tdd"
#include "test/year/test_leap_year_2501_3000.tdd"
#include "test/year/test_leap_year_3001_3500.tdd"
#include "test/year/test_leap_year_3501_4000.tdd"
#include "test/year/test_leap_year_4001_4500.tdd"
#include "test/year/test_leap_year_4501_5000.tdd"
#include "test/year/test_leap_year_5001_5500.tdd"
#include "test/year/test_leap_year_5501_6000.tdd"
#include "test/year/test_leap_year_6001_6500.tdd"
#include "test/year/test_leap_year_6501_7000.tdd"
#include "test/year/test_leap_year_7001_7500.tdd"
#include "test/year/test_leap_year_7501_8000.tdd"
#include "test/year/test_leap_year_8001_8500.tdd"
#include "test/year/test_leap_year_8501_9000.tdd"
#include "test/year/test_leap_year_9001_9500.tdd"
#include "test/year/test_leap_year_9501_9999.tdd"
#include "test/days/test_add_days_neg_0001_2190.tdd"
#include "test/days/test_add_days_pos_0001_2190.tdd"
#include "test/quarter/get_day_of_quarter_2017.tdd"
#include "test/quarter/get_day_of_quarter_2020.tdd"
#include "test/days/test_getFirstDayOfYear.tdd"
#include "test/days/test_dayGetText.tdd"

TEST_CASE("Starting Tests", "[j5c_Date]")
{
    std::cout << std::endl;
    std::cout << "Starting Testing: ... Please be patient..." << std::endl;
}


TEST_CASE("Date Assignment operator", "[j5c_Date]")
{
    j5c_Date d1{2017, 1, 1};
    j5c_Date d2{2018,12,31};
    d2 = d1;
    REQUIRE(d1.getYear()  == d2.getYear());
    REQUIRE(d1.getMonth() == d2.getMonth());
    REQUIRE(d1.getDay()   == d2.getDay());
    REQUIRE(d1.getYear()  == 2017);
    REQUIRE(d1.getMonth() == 1);
    REQUIRE(d1.getDay()   == 1);
};

TEST_CASE("Get Day of Week Text", "[j5c_Date]")
{
    j5c_Date d1{2017, 1, 1};
    //
    // Test set 1
    //
    d1.setDay(1);
    REQUIRE(d1.getDayText(0) == "Sunday");
    d1.setDay(2);
    REQUIRE(d1.getDayText(0) == "Monday");
    d1.setDay(3);
    REQUIRE(d1.getDayText(0) == "Tuesday");
    d1.setDay(4);
    REQUIRE(d1.getDayText(0) == "Wednesday");
    d1.setDay(5);
    REQUIRE(d1.getDayText(0) == "Thursday");
    d1.setDay(6);
    REQUIRE(d1.getDayText(0) == "Friday");
    d1.setDay(7);
    REQUIRE(d1.getDayText(0) == "Saturday");

    //
    // Test set 2
    //
    d1.setDay(15);
    REQUIRE(d1.getDayText(10) == "Sunday    ");
    d1.setDay(16);
    REQUIRE(d1.getDayText(10) == "Monday    ");
    d1.setDay(17);
    REQUIRE(d1.getDayText(10) == "Tuesday   ");
    d1.setDay(18);
    REQUIRE(d1.getDayText(10) == "Wednesday ");
    d1.setDay(19);
    REQUIRE(d1.getDayText(10) == "Thursday  ");
    d1.setDay(20);
    REQUIRE(d1.getDayText(10) == "Friday    ");
    d1.setDay(21);
    REQUIRE(d1.getDayText(10) == "Saturday  ");


};


TEST_CASE("Method j5c_Date::next_Date()", "[j5c_Date]")
{
    j5c_Date d1{2017, 1, 1};
    j5c_Date d2{2017, 1, 1};
    for (int yyyy = 2017; yyyy < 2020; yyyy++) {
        d1.setYear(yyyy);
        d2.setYear(yyyy);
        for (int ddd = 1; ddd < 366; ddd++)
        {
            d1.set_y_d(yyyy, ddd);
            d2.set_y_d(yyyy, ddd);
            REQUIRE(d1.next_Date().strDate() == d2.add_Days(1).strDate());
        }
    }
};

TEST_CASE("Method j5c_Date::prior_Date()", "[j5c_Date]")
{
    j5c_Date d1{2017,12,31};
    j5c_Date d2{2017,12,31};
    for (int yyyy = 2017; yyyy > 2010; yyyy--) {
        d1.setYear(yyyy);
        d2.setYear(yyyy);
        for (int ddd = 365; ddd > 1; ddd--)
        {
            d1.set_y_d(yyyy,ddd);
            d2.set_y_d(yyyy,ddd);
            REQUIRE(d1.next_Date().strDate() == d2.add_Days(1).strDate());
        }
    }
};

TEST_CASE("Method j5c_Date::++prefix", "[j5c_Date]")
{
    j5c_Date d1{2017, 1, 1};
    j5c_Date d2{2017, 1, 1};
    REQUIRE(d1.next_Date().strDate() == (++d2).strDate());
};

TEST_CASE("Method j5c_Date::postfix++", "[j5c_Date]")
{
    j5c_Date d1{2017, 1, 1};
    j5c_Date d2{2017, 1, 1};
    REQUIRE(d1.next_Date().strDate() == (d2++).strDate());
};

TEST_CASE("Method j5c_Date::--prefix", "[j5c_Date]")
{
    j5c_Date d1{2017, 1, 1};
    j5c_Date d2{2017, 1, 1};
    REQUIRE(d1.prior_Date().strDate() == (--d2).strDate());
};

TEST_CASE("Method j5c_Date::postfix--", "[j5c_Date]")
{
    j5c_Date d1{2017, 1, 1};
    j5c_Date d2{2017, 1, 1};
    REQUIRE(d1.prior_Date().strDate() == (d2--).strDate());
};

TEST_CASE("Method j5c_Date::getAge()", "[j5c_Date]")
{
    const int expectedResult = 77;
    j5c_Date d1{1940,10,8};
    if (d1.getAge() != expectedResult)
    {
        std::cout << "Please update this test once a year..." << std::endl;
    }
    REQUIRE(d1.getAge() == expectedResult);
};


