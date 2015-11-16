//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <exception>
#include <cxxabi.h>

#include "../include/safe_integer.hpp"
#include "../include/automatic.hpp"

template <class T>
using safe_t = boost::numeric::safe<
    T,
    boost::numeric::automatic
>;

#include "test_subtract.hpp"
#include "test.hpp"
#include "test_values.hpp"

// note: same test matrix as used in test_checked.  Here we test all combinations
// safe and unsafe integers.  in test_checked we test all combinations of
// integer primitives

const char *test_subtraction_result[VALUE_ARRAY_SIZE] = {
//      0       0       0       0
//      01234567012345670123456701234567
//      01234567890123456789012345678901
/* 0*/ "..............x..............xxx",
/* 1*/ "..............x..............xxx",
/* 2*/ ".............x..............xxxx",
/* 3*/ "............................xxxx",
/* 4*/ "..............x..............xxx",
/* 5*/ "..............x..............xxx",
/* 6*/ ".............x..............xxxx",
/* 7*/ "............................xxxx",

/* 8*/ "..............x..............xxx",
/* 9*/ "..............x..............xxx",
/*10*/ ".............x..............xxxx",
/*11*/ "............................xxxx",
/*12*/ "..............x..............xxx",
/*13*/ "..xx..xx..xx..xx..............xx",
/*14*/ "xx..xx..xx..xx..xxxxxxxxxxxxxxxx",
/*15*/ "............................xxxx",

//      0       0       0       0
//      01234567012345670123456701234567
//      01234567890123456789012345678901
/*16*/ "..............x..xxx.xxx.xxx.xxx",
/*17*/ "..............x...xx.xxx.xxx.xxx",
/*18*/ "..............x....x.xxx.xxx.xxx",
/*19*/ "..............x......xxx.xxx.xxx",
/*20*/ "..............x..xxx.xxx.xxx.xxx",
/*21*/ "..............x.......xx.xxx.xxx",
/*22*/ "..............x........x.xxx.xxx",
/*23*/ "..............x..........xxx.xxx",

/*24*/ "..............x..xxx.xxx.xxx.xxx",
/*25*/ "..............x...........xx.xxx",
/*26*/ "..............x............x.xxx",
/*27*/ "..............x..............xxx",
/*28*/ ".xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx",
/*29*/ "..xx..xx..xx..xx..............xx",
/*30*/ "..xx..xx..xx..xx...............x",
/*31*/ "..xx..xx..xx..xx................"
};

#define TEST_IMPL(v1, v2, result) \
    rval &= test_subtract(   \
        v1,                       \
        v2,                       \
        BOOST_PP_STRINGIZE(v1),   \
        BOOST_PP_STRINGIZE(v2),   \
        result                    \
    );
/**/

#define TESTX(value_index1, value_index2)          \
    (std::cout << value_index1 << ',' << value_index2 << std::endl); \
    TEST_IMPL(                                     \
        BOOST_PP_ARRAY_ELEM(value_index1, VALUES), \
        BOOST_PP_ARRAY_ELEM(value_index2, VALUES), \
        test_subtraction_result[value_index1][value_index2] \
    )
/**/

int main(int argc, char * argv[]){
    bool rval = true;
    TEST_EACH_VALUE_PAIR
    std::cout << (rval ? "success!" : "failure") << std::endl;
    return ! rval ;
}
