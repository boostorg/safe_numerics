//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <exception>

#include "../include/safe_integer.hpp"
#include "../include/automatic.hpp"

template <class T>
using safe_t = boost::numeric::safe<
    T,
    boost::numeric::automatic
>;

#include "test_right_shift.hpp"
#include "test.hpp"
#include "test_values.hpp"

// note: same test matrix as used in test_checked.  Here we test all combinations
// safe and unsafe integers.  in test_checked we test all combinations of
// integer primitives

const char *test_right_shift_result[VALUE_ARRAY_SIZE] = {
//      0       0       0       0
//      01234567012345670123456701234567
//      01234567890123456789012345678901
/* 0*/ ".xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx",
/* 1*/ ".xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx",
/* 2*/ "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
/* 3*/ "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
/* 4*/ ".xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx",
/* 5*/ ".xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx",
/* 6*/ "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
/* 7*/ "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",

/* 8*/ ".xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx",
/* 9*/ ".xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx",
/*10*/ "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
/*11*/ "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
/*12*/ ".xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx",
/*13*/ ".xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx",
/*14*/ "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
/*15*/ "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",

//      0       0       0       0
//      01234567012345670123456701234567
//      01234567890123456789012345678901
/*16*/ "..xx..xx..xx..xx................",
/*17*/ "..xx..xx..xx..xx................",
/*18*/ "..xx..xx..xx..xx................",
/*19*/ "..xx..xx..xx..xx................",
/*20*/ "..xx..xx..xx..xx................",
/*21*/ "..xx..xx..xx..xx................",
/*22*/ "..xx..xx..xx..xx................",
/*23*/ "..xx..xx..xx..xx................",

/*24*/ "..xx..xx..xx..xx................",
/*25*/ "..xx..xx..xx..xx................",
/*26*/ "..xx..xx..xx..xx................",
/*27*/ "..xx..xx..xx..xx................",
/*28*/ "..xx..xx..xx..xx................",
/*29*/ "..xx..xx..xx..xx................",
/*30*/ "..xx..xx..xx..xx................",
/*31*/ "..xx..xx..xx..xx................"
};

#include <boost/preprocessor/stringize.hpp>

#define TEST_IMPL(v1, v2, result) \
    rval &= test_right_shift(      \
        v1,                       \
        v2,                       \
        BOOST_PP_STRINGIZE(v1),   \
        BOOST_PP_STRINGIZE(v2),   \
        result                    \
    );
/**/

#define TESTX(value_index1, value_index2)          \
    (std::cout << value_index1 << ',' << value_index2 << ','); \
    TEST_IMPL(                                     \
        BOOST_PP_ARRAY_ELEM(value_index1, VALUES), \
        BOOST_PP_ARRAY_ELEM(value_index2, VALUES), \
        test_right_shift_result[value_index1][value_index2] \
    )
/**/
int main(int argc, char * argv[]){
    bool rval = true;
    TEST_EACH_VALUE_PAIR
    std::cout << (rval ? "success!" : "failure") << std::endl;
    return ! rval ;
}
