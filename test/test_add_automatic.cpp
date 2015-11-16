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

#include "test_add.hpp"
#include "test.hpp"
#include "test_values.hpp"

// note: same test matrix as used in test_checked.  Here we test all combinations
// safe and unsafe integers.  in test_checked we test all combinations of
// integer primitives

const char *test_addition_result[VALUE_ARRAY_SIZE] = {
//      0       0       0       0
//      01234567012345670123456701234567
//      01234567890123456789012345678901
/* 0*/ ".............x.................x",
/* 1*/ ".............x.................x",
/* 2*/ "..............x.............xxxx",
/* 3*/ "..............x.............xxxx",
/* 4*/ ".............x.................x",
/* 5*/ ".............x.................x",
/* 6*/ "..............x.............xxxx",
/* 7*/ "..............x.............xxxx",

/* 8*/ ".............x.................x",
/* 9*/ ".............x.................x",
/*10*/ "..............x.............xxxx",
/*11*/ "..............x.............xxxx",
/*12*/ ".............x.................x",
/*13*/ "xx..xx..xx..xx..xxxxxxxxxxxx...x",
/*14*/ "..xx..xx..xx..xx............xxxx",
/*15*/ "..............x.............xxxx",

//      0       0       0       0
//      01234567012345670123456701234567
//      01234567890123456789012345678901
/*16*/ ".............x.................x",
/*17*/ ".............x.................x",
/*18*/ ".............x.................x",
/*19*/ ".............x.................x",
/*20*/ ".............x.................x",
/*21*/ ".............x.................x",
/*22*/ ".............x.................x",
/*23*/ ".............x.................x",

/*24*/ ".............x.................x",
/*25*/ ".............x.................x",
/*26*/ ".............x.................x",
/*27*/ ".............x.................x",
/*28*/ "..xx..xx..xx..xx...............x",
/*29*/ "..xx..xx..xx..xx...............x",
/*30*/ "..xx..xx..xx..xx..............xx",
/*31*/ "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
};

#define TEST_IMPL(v1, v2, result) \
    rval &= test_add(             \
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
        test_addition_result[value_index1][value_index2] \
    )
/**/

int main(int argc, char * argv[]){
    // sanity check on test matrix - should be symetrical
    for(int i = 0; i < VALUE_ARRAY_SIZE; ++i)
        for(int j = i + 1; j < VALUE_ARRAY_SIZE; ++j)
            if(test_addition_result[i][j] != test_addition_result[j][i]){
                std::cout << i << ',' << j << std::endl;
                return 1;
            }
    bool rval = true;
    TEST_EACH_VALUE_PAIR
    std::cout << (rval ? "success!" : "failure") << std::endl;
    return ! rval ;
}
