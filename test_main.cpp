// 1 = run the Catch2 tests; 0 = skip them (faster compiling) and run the small demo below.
// Catch2 v3 itself is compiled once from source/catch_amalgamated.cpp, which is built with
// CATCH_AMALGAMATED_CUSTOM_MAIN, so main() is always defined here.
#define CATCH_CONFIG_MAIN    1
#if     CATCH_CONFIG_MAIN == 1

    #include "source/catch_amalgamated.hpp"
    #include "test_includes.tdd"

    int main(int argc, char* argv[])
    {
        return Catch::Session().run(argc, argv);
    }

#else


    #include <iostream>
    #include "source/jc313_Date.h"

    using namespace JC313_DSL_Code;

    int main()
    {

        std::cout << "jc313_Date tests disabled for faster compiling." << std::endl;

        jc313_Date d;
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

        jc313_Date inDate;

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
