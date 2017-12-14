cmake_minimum_required(VERSION 3.7)
project(Date)

set(CMAKE_CXX_STANDARD 17)

set(TEST_FILES
        test/year/test_leap_year_0001_0500.tdd
        test/year/test_leap_year_0501_1000.tdd
        test/year/test_leap_year_1001_1500.tdd
        test/year/test_leap_year_1501_2000.tdd
        test/year/test_leap_year_2001_2500.tdd
        test/year/test_leap_year_3001_3500.tdd
        test/year/test_leap_year_3501_4000.tdd
        test/year/test_leap_year_4001_4500.tdd
        test/year/test_leap_year_4501_5000.tdd
        test/year/test_leap_year_5001_5500.tdd
        test/year/test_leap_year_5501_6000.tdd
        test/year/test_leap_year_6501_7000.tdd
        test/year/test_leap_year_7001_7500.tdd
        test/year/test_leap_year_7501_8000.tdd
        test/year/test_leap_year_8001_8500.tdd
        test/year/test_leap_year_8501_9000.tdd
        test/year/test_leap_year_9501_9999.tdd
        test/days/test_add_days_pos_0001_2190.tdd
        test/days/test_add_days_neg_0001_2190.tdd
        test/quarter/get_day_of_quarter_2017.tdd
        test/quarter/get_day_of_quarter_2020.tdd
        test/days/test_getFirstDayOfYear.tdd
        test/days/test_dayGetText.tdd
        )

set(SOURCE_FILES
        source/catch.hpp
        ${TEST_FILES}
        source/catch2.h
        source/j5c_date.cpp
        source/j5c_date.h
        test_main.cpp
        )
add_executable(Date ${SOURCE_FILES})
