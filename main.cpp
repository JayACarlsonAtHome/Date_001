#include "source/j5c_date.h"

using namespace J5C_DSL_Code;

int main()
{
    using namespace J5C_DSL_Code;

    J5C_Date d1;
    bool isLeapYear = false;
    for(int yyyy = 1; yyyy < 401; yyyy++)
    {
        d1.setYear(yyyy);
        isLeapYear = d1.isLeapYear();
        std::cout << "Year = " << yyyy << "  :  isLeapYear = " << std::boolalpha << isLeapYear << std::endl;
    }
    return 0;
};

