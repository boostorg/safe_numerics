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

#include "test_divide.hpp"
#include "test.hpp"
#include "test_values.hpp"

// note: These tables presume that the the size of an int is 32 bits.
// This should be changed for a different architecture or better yet
// be dynamically adjusted depending on the indicated architecture

const char *test_division_result[VALUE_ARRAY_SIZE] = {
//      0       0       0       0
//      01234567012345670123456701234567
//      01234567890123456789012345678901
/* 0*/ "................................",
/* 1*/ "................................",
/* 2*/ "................................",
/* 3*/ "................................",
/* 4*/ "................................",
/* 5*/ "................................",
/* 6*/ "................................",
/* 7*/ "................................",

/* 8*/ "................................",
/* 9*/ "................................",
/*10*/ "................................",
/*11*/ "................................",
/*12*/ "................................",
/*13*/ "................................",
/*14*/ "...x...x...x...x................",
/*15*/ "................................",

//      0       0       0       0
//      01234567012345670123456701234567
//      01234567890123456789012345678901
/*16*/ "................................",
/*17*/ "................................",
/*18*/ "................................",
/*19*/ "................................",
/*20*/ "................................",
/*21*/ "................................",
/*22*/ "................................",
/*23*/ "................................",

/*24*/ "................................",
/*25*/ "................................",
/*26*/ "................................",
/*27*/ "................................",
/*28*/ "................................",
/*29*/ "................................",
/*30*/ "xxxxxxxxxxxxxxxx................",
/*31*/ "xxxxxxxxxxxxxxxx................"
};

#define TEST_IMPL(v1, v2, result) \
    rval &= test_divide(          \
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
        test_division_result[value_index1][value_index2] \
    )
/**/

int main(int argc, char * argv[]){
    bool rval = true;
    TEST_EACH_VALUE_PAIR
    std::cout << (rval ? "success!" : "failure") << std::endl;
    return ! rval ;
}
