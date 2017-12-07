#include "source/catch2.h"
#include "source/j5c_date.h"

using namespace J5C_DSL_Code;

J5C_Date d1;
J5C_Date d2;

TEST_CASE("Dates are equal", "[J5C_Date]")
{
    REQUIRE(d1 == d2);
}

bool isLeapYear(int year)
{
    J5C_Date d(year, 01, 01);
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


TEST_CASE("Starting Tests", "[J5C_Date]")
{
    std::cout << std::endl;
    std::cout << "Starting Testing: ... Please be patient..." << std::endl;
}

TEST_CASE("Method J5C_Date::next_Date()", "[J5C_Date]")
{
    J5C_Date d1{2017,01,01};
    J5C_Date d2{2017,01,01};
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

TEST_CASE("Method J5C_Date::prior_Date()", "[J5C_Date]")
{
    J5C_Date d1{2017,12,31};
    J5C_Date d2{2017,12,31};
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

TEST_CASE("Method J5C_Date::++prefix", "[J5C_Date]")
{
    J5C_Date d1{2017,01,01};
    J5C_Date d2{2017,01,01};
    REQUIRE(d1.next_Date().strDate() == (++d2).strDate());
};

TEST_CASE("Method J5C_Date::postfix++", "[J5C_Date]")
{
    J5C_Date d1{2017,01,01};
    J5C_Date d2{2017,01,01};
    REQUIRE(d1.next_Date().strDate() == (d2++).strDate());
};

TEST_CASE("Method J5C_Date::--prefix", "[J5C_Date]")
{
    J5C_Date d1{2017,01,01};
    J5C_Date d2{2017,01,01};
    REQUIRE(d1.prior_Date().strDate() == (--d2).strDate());
};

TEST_CASE("Method J5C_Date::postfix--", "[J5C_Date]")
{
    J5C_Date d1{2017,01,01};
    J5C_Date d2{2017,01,01};
    REQUIRE(d1.prior_Date().strDate() == (d2--).strDate());
};

TEST_CASE("Method J5C_Date::getAge()", "[J5C_Date]")
{
    const int expectedResult = 77;
    J5C_Date d1{1940,10,8};
    if (d1.getAge() != expectedResult)
    {
        std::cout << "Please update this test once a year..." << std::endl;
    }
    REQUIRE(d1.getAge() == expectedResult);
};


