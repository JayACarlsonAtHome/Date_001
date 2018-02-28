// This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_MAIN    0
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

        std::string strTemplate1 = "         11111111112";
        std::string strTemplate2 = "12345678901234567890";
        std::string str1;
        std::string str2;
        for (int i = 0; i < 21; i++)
        {
            std::cout << "Max Column Size = " << i << std::endl;

            str1 = strTemplate1.substr(0,i);
            str2 = strTemplate2.substr(0,i);
            std::cout << "             " << str1 << std::endl;
            std::cout << "             " << str2 << std::endl;

            d.setYear(01);
            std::cout << d << " : " << d.getDayText(i) << "***" << std::endl;
            d.setYear(15);
            std::cout << d << " : " << d.getDayText(i) << "***" << std::endl;
            d.setYear(127);
            std::cout << d << " : " << d.getDayText(i) << "***" << std::endl;
            d.setYear(1571);
            std::cout << d << " : " << d.getDayText(i) << "***" << std::endl << std::endl;
        }
        int year = 0;
        d.setMonth(01);
        d.setDay(01);
        for (int i = 0; i < 26; i++)
        {
            year = i * 400;
            d.setYear(year);
            std::cout << d << " : " << d.getDayText(10) << "***" << std::endl;
        }

        j5c_Date inDate;

        bool cont = true;
        std::string input;
        while (cont) {

            std::cout << "Enter a valid date: ";
            std::cin >> inDate;
            std::cout << "inDate = " << inDate << std::endl;
            std::cout << "Enter another date? (y/n): " << std::endl;
            std::cin >> input;
            if (!((input.at(0) == 'y') || (input.at(0) == 'Y')))
            {
                cont = false;
            }
        }




        return 0;

    }


#endif
