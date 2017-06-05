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
#include "../include/checked.hpp"

template<class T1, class T2>
bool test_checked_subtract(
    T1 v1,
    T2 v2,
    const char *av1,
    const char *av2,
    char expected_result
){
    using namespace boost::numeric;
    std::cout
        << "testing  "
        << av1 << " + " << av2
        << std::endl;

    using result_type = decltype(T1() + T2());

    checked_result<result_type> result
        = checked::subtract<result_type>(v1, v2);

    if(result.no_exception()
    && expected_result != '.'){
        std::cout
            << "failed to detect error in subtraction "
            << std::hex << result << "(" << std::dec << result << ")"
            << " != "<< av1 << " + " << av2
            << std::endl;
        result = checked::subtract<result_type>(v1, v2);
        return false;
    }
    else
    if(result.exception()
    && expected_result != 'x'){
        std::cout
            << "erroneously detected error "
            << std::hex << result <<  av1 << " + " << av2
            << std::endl;
        result = checked::subtract<result_type>(v1, v2);
        return false;
    }

    return true; // correct result
}

#include "test.hpp"
#include "test_values.hpp"

// note: same test matrix as used in test_add.  Here we test all combinations
// safe and unsafe integers.  in test_checked we test all combinations of
// integer primitives

const char *test_subtraction_result[VALUE_ARRAY_SIZE] = {
//      0       0       0       0
//      012345670123456701234567012345670
//      012345678901234567890123456789012
/* 0*/ "..........x...x..........xxx.xxx.",
/* 1*/ "..........x...x..........xxx.xxx.",
/* 2*/ ".........x...x..........xxxxxxxx.",
/* 3*/ "........................xxxxxxxx.",
/* 4*/ "..........x...x..........xxx.xxx.",
/* 5*/ "..........x...x..........xxx.xxx.",
/* 6*/ ".........x...x..........xxxxxxxx.",
/* 7*/ "........................xxxxxxxx.",

/* 8*/ "..........x...x..........xxx.xxx.",
/* 9*/ "..xx..xx..xx..x...........xx.xxx.",
/*10*/ "xx..xx..xx...x..xxxxxxxxxxxxxxxx.",
/*11*/ "........................xxxxxxxx.",
/*12*/ "..............x..............xxx.",
/*13*/ "..xx..xx..xx..xx..............xx.",
/*14*/ "xx..xx..xx..xx..xxxxxxxxxxxxxxxx.",
/*15*/ "............................xxxx.",

//      0       0       0       0
//      012345670123456701234567012345670
//      012345678901234567890123456789012
/*16*/ "..........x...x..........xxx.xxx.",
/*17*/ "..........x...x..........xxx.xxx.",
/*18*/ "..........x...x..........xxx.xxx.",
/*19*/ "..........x...x..........xxx.xxx.",
/*20*/ "..........x...x..........xxx.xxx.",
/*21*/ "..........x...x..........xxx.xxx.",
/*22*/ "..........x...x..........xxx.xxx.",
/*23*/ "..........x...x..........xxx.xxx.",

/*24*/ ".xxx.xxx.xxx..x..xxx.xxx.xxx.xxx.",
/*25*/ "..xx..xx..xx..x...........xx.xxx.",
/*26*/ "..xx..xx..xx..x............x.xxx.",
/*27*/ "..xx..xx..xx..x..............xxx.",
/*28*/ ".xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx.",
/*29*/ "..xx..xx..xx..xx..............xx.",
/*30*/ "..xx..xx..xx..xx...............x.",
/*31*/ "..xx..xx..xx..xx.................",
/*31*/ "..........x...x.........xxxxxxxx."
};

#define TEST_IMPL(v1, v2, result) \
    rval &= test_checked_subtract(     \
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
        test_subtraction_result[value_index1][value_index2] \
    )
/**/

int main(int argc, char *argv[]){
    bool rval = true;

    TEST_EACH_VALUE_PAIR
    std::cout << (rval ? "success!" : "failure") << std::endl;
    return ! rval ;
}


