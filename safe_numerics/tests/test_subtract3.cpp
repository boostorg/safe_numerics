//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "test_subtract.hpp"

#define TESTX(a, b, c, d)                        \
    TEST_IMPL(                                   \
        a,                                       \
        b,                                       \
        c,                                       \
        boost::numeric::safe<d>                  \
    )                                            \
/**/

bool test_subtract3(){
    bool rval = true;
    #pragma message("0")
    EACH_TYPE1(boost::int8_t);
    #pragma message("1")
    EACH_TYPE1(boost::uint8_t);
    #pragma message("2")
    EACH_TYPE1(boost::int16_t);
    #pragma message("3")
    EACH_TYPE1(boost::uint16_t);
    #pragma message("4")
    EACH_TYPE1(boost::int32_t);
    #pragma message("5")
    EACH_TYPE1(boost::uint32_t);
    #pragma message("6")
    EACH_TYPE1(boost::int64_t);
    #pragma message("7")
    EACH_TYPE1(boost::uint64_t);
    return rval;
}

