//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <exception>
#include <cxxabi.h>
#include <cstdlib>   // EXIT_SUCCESS
#include <iostream>

#include "../include/checked_result.hpp"
#include "../include/checked.hpp"

template<class T1, class T2>
bool test_checked_divide(
    T1 v1,
    T2 v2,
    const char *av1,
    const char *av2,
    char expected_result
){
    using namespace boost::numeric;

    std::cout
        << "testing  "
        << av1 << " / " << av2
        << std::endl;

    typedef decltype(T1() / T2()) result_type;

    checked_result<result_type> result = checked::divide<result_type>(
        std::numeric_limits<result_type>::min(),
        std::numeric_limits<result_type>::max(),
        v1,
        v2
    );

    if(result == exception_type::no_exception
    && expected_result != '.'){
        std::cout
            << "failed to detect error in division "
            << std::hex << "0x" << result << "(" << std::dec << result << ")"
            << " != "<< av1 << " / " << av2
            << std::endl;
        result = checked::divide<result_type>(
            std::numeric_limits<result_type>::min(),
            std::numeric_limits<result_type>::max(),
            v1,
            v2
        );
        return false;
    }
    else
    if(result != exception_type::no_exception
    && expected_result != 'x'){
        std::cout
            << "erroneously detected error "
            << std::hex << result <<  av1 << " / " << av2 << std::dec << std::endl;
        result = checked::divide<result_type>(
            std::numeric_limits<result_type>::min(),
            std::numeric_limits<result_type>::max(),
            v1,
            v2
        );
        return false;
    }
    return true; // correct result
}

#include "test.hpp"
#include "test_values.hpp"

// note: same test matrix as used in test_divide.  Here we test all combinations
// safe and unsafe integers.  in test_checked we test all combinations of
// integer primitives

const char *test_division_result[VALUE_ARRAY_SIZE] = {
//      0       0       0       0
//      01234567012345670123456701234567
//      01234567890123456789012345678901
/* 0*/ "................................",
/* 1*/ "................................",
/* 2*/ "........................xxxxxxxx",
/* 3*/ "........................xxxxxxxx",
/* 4*/ ".................................",
/* 5*/ "................................",
/* 6*/ "........................xxxxxxxx",
/* 7*/ "........................xxxxxxxx",

/* 8*/ "................................",
/* 9*/ "................................",
/*10*/ "...x...x...x............xxxxxxxx",
/*11*/ "........................xxxxxxxx",
/*12*/ "................................",
/*13*/ "................................",
/*14*/ "...x...x...x...x............xxxx",
/*15*/ "............................xxxx",

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

/*24*/ "..xx..xx..xx....................",
/*25*/ "..xx..xx..xx....................",
/*26*/ "..xx..xx..xx....................",
/*27*/ "..xx..xx..xx....................",
/*28*/ "..xx..xx..xx..xx................",
/*29*/ "..xx..xx..xx..xx................",
/*30*/ "..xx..xx..xx..xx................",
/*31*/ "..xx..xx..xx..xx................"
};

#define TEST_IMPL(v1, v2, result) \
    rval &= test_checked_divide(     \
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

int main(int argc, char *argv[]){
    bool rval = true;

    TEST_EACH_VALUE_PAIR
    return ! rval ;
}


