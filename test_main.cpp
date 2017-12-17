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
    d.setYear(2017);
    d.setMonth(01);
    d.setDay(01);
    int bl = 0;
    for (int i = 1; i < 3652; ++i)
    {
        d = ++d;
        std::cout << "REQUIRE((d.add_Days(" << std::setw(5) << i << ")).strDate() == \"" << d.strDate() << "\" ));" << std::endl;
        ++bl;
        if (bl == 10)
        {
            std::cout << std::endl;
            bl = 0;
        }
    }

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;


    return 0;

}


#endif
