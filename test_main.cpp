// This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_MAIN    1
#if     CATCH_CONFIG_MAIN == 1

    #include "source/catch.hpp"
    #include "test_includes.tdd"
    // main is provided by test framework Catch2

#else


    #include <iostream>
    #include "source/j5c_date.h"

    using namespace J5C_DSL_Code;

    int main()
    {

        std::cout << "j5c_Date tests disabled for faster compiling." << std::endl;

        j5c_Date d;
        d.setYear(1);
        d.setMonth(01);

        for (int i = 0; i < 21; i++)
        {
            std::cout << "                      1         2" << std::endl;
            std::cout << "             12345678901234567890" << std::endl;
            std::cout << "Max Column Size = " << i << std::endl;
            d.setYear(01);
            std::cout << d << " : " << d.getDayText(i) << "***" << std::endl;
            d.setYear(15);
            std::cout << d << " : " << d.getDayText(i) << "***" << std::endl;
            d.setYear(127);
            std::cout << d << " : " << d.getDayText(i) << "***" << std::endl;
            d.setYear(1571);
            std::cout << d << " : " << d.getDayText(i) << "***" << std::endl << std::endl;
        }


        return 0;

    }


#endif
