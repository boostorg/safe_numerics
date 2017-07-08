//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <exception>
#include <cstdlib>   // EXIT_SUCCESS
#include <iostream>
#include <cassert>

#include "../include/checked_result.hpp"
#include "../include/checked_integer.hpp"

template<class T1, class T2>
bool test_checked_multiply(
    T1 v1,
    T2 v2,
    const char *av1,
    const char *av2,
    char expected_result
){
    using namespace boost::numeric;
    std::cout
        << "testing  "
        << av1 << " * " << av2
        << std::endl;

    using result_type = decltype(T1() + T2());

    checked_result<result_type> result
        = checked::multiply<result_type>(
            v1,
            v2
        );

    if(! result.exception()
    && expected_result != '.'){
        std::cout
            << "failed to detect error in addition "
            << std::hex << result << "(" << std::dec << result << ")"
            << " != "<< av1 << " * " << av2
            << std::endl;
        result = checked::multiply<result_type>(
                v1,
                v2
            );
        return false;
    }
    else
    if(result.exception()
    && expected_result != 'x'){
        std::cout
            << "erroneously detected error "
            << std::hex << result <<  av1 << " * " << av2
            << std::endl;
        result = checked::multiply<result_type>(
                v1,
                v2
            );
        return false;
    }

    return true; // correct result
}

#include "test.hpp"
#include "test_values.hpp"

// note: same test matrix as used in test_add.  Here we test all combinations
// safe and unsafe integers.  in test_checked we test all combinations of
// integer primitives

const char *test_multiplication_result[VALUE_ARRAY_SIZE] = {
//      0       0       0       0
//      012345670123456701234567012345670
//      012345678901234567890123456789012
/* 0*/ ".................................",
/* 1*/ ".........xx..xx..........xxx.xxx.",
/* 2*/ ".........xx..xx.........xxxxxxxx.",
/* 3*/ "..........x...x.........xxxxxxxx.",
/* 4*/ ".................................",
/* 5*/ ".........xx..xx..........xxx.xxx.",
/* 6*/ ".........xx..xx.........xxxxxxxx.",
/* 7*/ "..........x...x.........xxxxxxxx.",

/* 8*/ ".................................",
/* 9*/ ".xx..xx..xx..xx..xxx.xxx.xxx.xxx.",
/*10*/ ".xxx.xxx.xxx.xx..xxx.xxxxxxxxxxx.",
/*11*/ "..........x...x.........xxxxxxxx.",
/*12*/ ".................................",
/*13*/ ".xx..xx..xx..xx..xxx.xxx.xxx.xxx.",
/*14*/ ".xxx.xxx.xxx.xxx.xxx.xxx.xxxxxxx.",
/*15*/ "..............x.............xxxx.",

//      0       0       0       0
//      012345670123456701234567012345670
//      012345678901234567890123456789012
/*16*/ ".................................",
/*17*/ ".........xx..xx..........xxx.xxx.",
/*18*/ ".........xx..xx..........xxx.xxx.",
/*19*/ ".........xx..xx..........xxx.xxx.",
/*20*/ ".................................",
/*21*/ ".........xx..xx..........xxx.xxx.",
/*22*/ ".........xx..xx..........xxx.xxx.",
/*23*/ ".........xx..xx........x.xxx.xxx.",

/*24*/ "..xx..xx..xx.....................",
/*25*/ ".xxx.xxx.xxx.xx..xxx.xxx.xxx.xxx.",
/*26*/ ".xxx.xxx.xxx.xx..xxx.xxx.xxx.xxx.",
/*27*/ ".xxx.xxx.xxx.xx..xxx.xxx.xxx.xxx.",
/*28*/ "..xx..xx..xx..xx.................",
/*29*/ ".xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx.",
/*30*/ ".xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx.",
/*31*/ ".xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx.",
/*31*/ "................................."
};

#define TEST_IMPL(v1, v2, result) \
    rval &= test_checked_multiply(     \
        v1,                       \
        v2,                       \
        BOOST_PP_STRINGIZE(v1),   \
        BOOST_PP_STRINGIZE(v2),   \
        result                    \
    );
/**/

#define TESTX(value_index1, value_index2)          \
    (std::cout << std::dec << value_index1 << ',' << value_index2 << ','); \
    TEST_IMPL(                                     \
        BOOST_PP_ARRAY_ELEM(value_index1, VALUES), \
        BOOST_PP_ARRAY_ELEM(value_index2, VALUES), \
        test_multiplication_result[value_index1][value_index2] \
    )
/**/

int main(int argc, char *argv[]){
    // sanity check on test matrix - should be symetrical
    for(int i = 0; i < VALUE_ARRAY_SIZE; ++i)
        for(int j = i + 1; j < VALUE_ARRAY_SIZE; ++j)
            assert(test_multiplication_result[i][j] == test_multiplication_result[j][i]);

    bool rval = true;

    TEST_EACH_VALUE_PAIR
    std::cout << (rval ? "success!" : "failure") << std::endl;
    return ! rval ;
}


