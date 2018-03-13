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

#include "test_and.hpp"
#include "test.hpp"
#include "test_values.hpp"

// note: Here we test all combinations safe and unsafe integers.

constexpr const char *test_and_result[VALUE_ARRAY_SIZE] = {
    //      0       0       0       0
    //      012345670123456701234567012345670
    //      012345678901234567890123456789012
    /* 0*/ ".................................",
    /* 1*/ ".................................",
    /* 2*/ ".................................",
    /* 3*/ ".................................",
    /* 4*/ ".................................",
    /* 5*/ ".................................",
    /* 6*/ ".................................",
    /* 7*/ ".................................",
    
    /* 8*/ ".................................",
    /* 9*/ ".................................",
    /*10*/ ".................................",
    /*11*/ ".................................",
    /*12*/ ".................................",
    /*13*/ ".................................",
    /*14*/ ".................................",
    /*15*/ ".................................",
    
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
    
    /*24*/ ".................................",
    /*25*/ ".................................",
    /*26*/ ".................................",
    /*27*/ ".................................",
    /*28*/ ".................................",
    /*29*/ ".................................",
    /*30*/ ".................................",
    /*31*/ ".................................",
    /*32*/ "................................."
};

// given an array of values of particula
template<typename T, unsigned int N>
constexpr bool check_symmetry(const T (&value)[N]) {
    using namespace boost::numeric;
    // for each pair of values p1, p2 (100)
    for(unsigned int i = 0; i < N; i++)
    for(unsigned int j = 0; j < N; j++)
        if(value[i][j] != value[j][i])
            return false;
    return true;
}

#include <boost/preprocessor/stringize.hpp>

#define TEST_IMPL(v1, v2, result) \
    rval &= test_and(             \
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
        test_and_result[value_index1][value_index2] \
    )
/**/
int main(int, char * []){
    // sanity check on test matrix - should be symetrical
    //static_assert(check_symmetry(test_and_result), "should be symmetrical");
    bool rval = true;
    TEST_EACH_VALUE_PAIR
    std::cout << (rval ? "success!" : "failure") << std::endl;
    return ! rval ;
}
