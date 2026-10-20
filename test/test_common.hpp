//
// test_common.hpp -- what every test file sees. CMakeLists.txt compiles each .tdd file on its
//   own (a small generated .cpp that includes this, then the .tdd), so ninja builds them side by
//   side. One file holding every test took minutes to optimize in a Release build.
//

#pragma once

#include "source/catch_amalgamated.hpp"
#include "source/jc313_Date.h"
#include "source/jc_date_utils.hpp"

#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

using namespace JC313_DSL_Code;
