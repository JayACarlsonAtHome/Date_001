cmake_minimum_required(VERSION 3.26)
project(Date)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(TEST_FILES

        test/days/get_day_of_year_2017.tdd
        test/days/get_day_of_year_2020.tdd
        test/days/test_add_days_neg_0001_3653.tdd
        test/days/test_add_days_pos_0001_3652.tdd
        test/days/test_dayGetText.tdd
        test/days/test_getFirstDayOfYear.tdd
        test/days/test_isValid_2017_negative.tdd
        test/days/test_isValid_2017_positive.tdd
        test/days/test_isValid_2020_negative.tdd
        test/days/test_isValid_2020_positive.tdd

        test/quarter/get_day_of_quarter_2017.tdd
        test/quarter/get_day_of_quarter_2020.tdd
        test/quarter/get_quarter_2017.tdd
        test/quarter/get_quarter_2020.tdd

        test/year/test_leap_year_0001_0500.tdd
        test/year/test_leap_year_0501_1000.tdd
        test/year/test_leap_year_1001_1500.tdd
        test/year/test_leap_year_1501_2000.tdd
        test/year/test_leap_year_2001_2500.tdd
        test/year/test_leap_year_2501_3000.tdd
        test/year/test_leap_year_3001_3500.tdd
        test/year/test_leap_year_3501_4000.tdd
        test/year/test_leap_year_4001_4500.tdd
        test/year/test_leap_year_4501_5000.tdd
        test/year/test_leap_year_5001_5500.tdd
        test/year/test_leap_year_5501_6000.tdd
        test/year/test_leap_year_6001_6500.tdd
        test/year/test_leap_year_6501_7000.tdd
        test/year/test_leap_year_7001_7500.tdd
        test/year/test_leap_year_7501_8000.tdd
        test/year/test_leap_year_8001_8500.tdd
        test/year/test_leap_year_8501_9000.tdd
        test/year/test_leap_year_9001_9500.tdd
        test/year/test_leap_year_9501_9999.tdd

        test/validation/test_checked_dates.tdd

        test/z_operator/operators.tdd

        test_includes.tdd
        )

set(SOURCE_FILES
        source/catch_amalgamated.hpp
        source/catch_amalgamated.cpp
        ${TEST_FILES}
        source/jc313_Date.cpp
        source/jc313_Date.h
        test_main.cpp
        )
add_executable(Date ${SOURCE_FILES})
target_compile_options(Date PRIVATE -Wall -Wextra)
# Catch2 v3.8.1 (amalgamated); main() is in test_main.cpp, and Catch2's own code is not ours to warn about
target_compile_definitions(Date PRIVATE CATCH_AMALGAMATED_CUSTOM_MAIN)
set_source_files_properties(source/catch_amalgamated.cpp PROPERTIES COMPILE_OPTIONS "-w")

# main.cpp: a small day-of-week printout for 2017-2020 (not a test)
add_executable(date_demo main.cpp source/jc313_Date.cpp)
target_compile_options(date_demo PRIVATE -Wall -Wextra)

enable_testing()
add_test(NAME Date COMMAND Date)
