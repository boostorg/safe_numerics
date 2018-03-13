//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <exception>

#include "../include/safe_integer.hpp"
#include "../include/native.hpp"

template <class T>
using safe_t = boost::numeric::safe<
    T,
    boost::numeric::native
>;

#include "test_xor.hpp"
#include "test.hpp"
#include "test_values.hpp"

// note: same test matrix as used in test_checked.  Here we test all combinations
// safe and unsafe integers.  in test_checked we test all combinations of
// integer primitives

const char *test_xor_result[VALUE_ARRAY_SIZE] = {
//      0       0       0       0
//      012345670123456701234567012345670
//      012345678901234567890123456789012
/* 0*/ ".................................",
/* 1*/ ".................................",
/* 2*/ "........................xxxxxxxx.",
/* 3*/ "........................xxxxxxxx.",
/* 4*/ ".................................",
/* 5*/ ".................................",
/* 6*/ "........................xxxxxxxx.",
/* 7*/ "........................xxxxxxxx.",

/* 8*/ ".................................",
/* 9*/ ".................................",
/*10*/ "........................xxxxxxxx.",
/*11*/ "........................xxxxxxxx.",
/*12*/ ".................................",
/*13*/ ".................................",
/*14*/ "............................xxxx.",
/*15*/ "............................xxxx.",

//      0       0       0       0
//      012345670123456701234567012345670
//      012345678901234567890123456789012
/*16*/ ".................................",
/*17*/ ".................................",
/*18*/ ".................................",
/*19*/ ".................................",
/*20*/ ".................................",
/*21*/ ".................................",
/*22*/ ".................................",
/*23*/ ".................................",

/*24*/ "..xx..xx..xx.....................",
/*25*/ "..xx..xx..xx.....................",
/*26*/ "..xx..xx..xx.....................",
/*27*/ "..xx..xx..xx.....................",
/*28*/ "..xx..xx..xx..xx.................",
/*29*/ "..xx..xx..xx..xx.................",
/*30*/ "..xx..xx..xx..xx.................",
/*31*/ "..xx..xx..xx..xx.................",
/*32*/ "................................."
};

#include <boost/preprocessor/stringize.hpp>

#define TEST_IMPL(v1, v2, result) \
    rval &= test_xor(             \
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
        test_xor_result[value_index1][value_index2] \
    )
/**/
int main(int, char *[]){
    // sanity check on test matrix - should be symetrical
    for(int i = 0; i < VALUE_ARRAY_SIZE; ++i)
        for(int j = i + 1; j < VALUE_ARRAY_SIZE; ++j)
            if(test_xor_result[i][j] != test_xor_result[j][i]){
                std::cout << i << ',' << j << std::endl;
                return 1;
            }
    bool rval = true;
    TEST_EACH_VALUE_PAIR
    std::cout << (rval ? "success!" : "failure") << std::endl;
    return ! rval ;
}
