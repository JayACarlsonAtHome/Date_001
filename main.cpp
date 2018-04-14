#include "source/j5c_date.h"

using namespace J5C_DSL_Code;

std::string getDOW_Text(const int day, uint length)
{
    std::string result = "";
    if (length > 10) return ("Invalid length test parameter");

    //
    // begin processing
    //
    if ( day == -1) { result = "Invalid";   };
    if ( day ==  0) { result = "Sunday";    };
    if ( day ==  1) { result = "Monday";    };
    if ( day ==  2) { result = "Tuesday";   };
    if ( day ==  3) { result = "Wednesday"; };
    if ( day ==  4) { result = "Thursday";  };
    if ( day ==  5) { result = "Friday";    };
    if ( day ==  6) { result = "Saturday";  };

    if (length == 0)
    {
        return result;
    }
    else
    {
        result = result + "          ";
        result = result.substr(0,length);
    }
    return result;
}

int main()
{

    j5c_Date d {2017,01,01};
    std::string dayText1 = "";
    std::string dayText2 = "";
    int dayCounter =-1;
    for (int yyyy = 2017; yyyy < 2021; ++yyyy)
    {
        dayCounter = 0;
        for (int month = 1; month < 13; ++month)
        {
            for (int day = 1; day < 32; ++day)
            {
                d.setYear(yyyy);
                d.setMonth(month);
                d.setDay(day);
                if ((month == 1) && (day == 1))
                {
                    dayCounter = d.getFirstDayOfYear();
                }

                if (d.isValid())
                {
                    std::cout << "dayCounter =  " << dayCounter << std::endl;
                    std::cout << d << " : " << d.getDayText(10) << std::endl;
                    //
                    // Setup for test
                    //
                    if ( dayCounter > 6 ) { dayCounter = 0; }

                    //
                    // test1
                    //
                    dayText1 = getDOW_Text(dayCounter, 0);
                    dayText2 = d.getDayText(0);
                    if (dayText1 != dayText2)
                    {
                        std::cout << "Date: " << d   << std::endl;
                        std::cout << "Day = " << day << std::endl;
                        std::cout << "dayText1 = " << dayText1 << std::endl;
                        std::cout << "dayText2 = " << dayText2 << std::endl;
                    }

                    //
                    // test 2
                    //
                    dayText1 = getDOW_Text(dayCounter, 10);
                    dayText2 = d.getDayText(10);
                    if (dayText1 != dayText2)
                    {
                        std::cout << "Date: " << d << std::endl;
                        std::cout << "dayText1 = " << dayText1 << std::endl;
                        std::cout << "dayText2 = " << dayText2 << std::endl;
                    }
                    std::cout << std::endl;
                    std::cout << std::endl;
                    dayCounter+=1;
                }
            }
        }
    }
    return 0;
};

