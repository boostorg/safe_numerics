//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "test_subtract.hpp"

#define TESTX(a, b) \
    rval &= test_subtract(a, b, BOOST_PP_STRINGIZE(a), BOOST_PP_STRINGIZE(b));
/**/

int main(int argc, char * argv[]){
    bool rval = true;
    TEST_ALL
    return ! rval ;
}
