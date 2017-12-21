//using namespace J5C_DSL_Code;

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
        d.setDay(01);
        std::cout << d.strDate() << std::endl;

        d.setYear(10);
        std::cout << d.strDate() << std::endl;

        d.setYear(100);
        std::cout << d.strDate() << std::endl;

        d.setYear(1000);
        std::cout << d.strDate() << std::endl;

        return 0;

    }


#endif
