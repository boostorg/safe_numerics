//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <cassert>

#include "../include/safe_integer.hpp"

// we could have used decltype and auto for C++11 but we've decided
// to use boost/typeof to be compatible with older compilers
#include <boost/typeof/typeof.hpp>

#include "test.hpp"

template<class T1, class T2>
bool test_modulus(
    T1 v1,
    T2 v2,
    const char *av1,
    const char *av2,
    char expected_result
){
    std::cout
        << "testing  "
        << av1 << " % " << av2
        << std::endl;

    boost::numeric::safe<T1> t1 = v1;
    BOOST_TYPEOF_TPL(T1() / T2()) result;

    try{
        result = t1 % v2;

        if(expected_result != '.'){
        //if(expected_result == 'x'){
            std::cout
                << "failed to detect error in division "
                << std::hex << result << "(" << std::dec << result << ")"
                << " ! = "<< av1 << " % " << av2
                << std::endl;
            try{
                result = t1 % v2;
            }
            catch(...){}
            return false;
        }
    }
    catch(std::range_error){
        if(expected_result != 'x'){
        //if(expected_result == '.'){
            std::cout
                << "erroneously detected error in division "
                << std::hex << result << "(" << std::dec << result << ")"
                << " == "<< av1 << " % " << av2
                << std::endl;
            try{
                result = t1 % v2;
            }
            catch(...){}
            return false;
        }
    }
    boost::numeric::safe<T2> t2 = v2;
    try{
        result = t1 % t2;

        if(expected_result != '.'){
        //if(expected_result == 'x'){
            std::cout
                << "failed to detect error in division "
                << std::hex << result << "(" << std::dec << result << ")"
                << " ! = "<< av1 << " % " << av2
                << std::endl;
            try{
                result = t1 % t2;
            }
            catch(...){}
            return false;
        }
    }
    catch(std::range_error){
        if(expected_result != 'x'){
        //if(expected_result == '.'){
            std::cout
                << "erroneously detected error in division "
                << std::hex << result << "(" << std::dec << result << ")"
                << " == "<< av1 << " % " << av2
                << std::endl;
            try{
                result = t1 % t2;
            }
            catch(...){}
            return false;
        }
    }
    return true;
}

#include "test.hpp"
#include "test_values.hpp"

// note: These tables presume that the the size of an int is 32 bits.
// This should be changed for a different architecture or better yet
// be dynamically adjusted depending on the indicated architecture

const char *test_multiplication_result[VALUE_ARRAY_SIZE] = {
//      0       0       0       0
//      01234567012345670123456701234567
//      01234567890123456789012345678901
/* 0*/ "................................",
/* 1*/ "................................",
/* 2*/ "...x...x...x...x................",
/* 3*/ "................................",
/* 4*/ ".................................",
/* 5*/ "................................",
/* 6*/ "...x...x...x...x................",
/* 7*/ "................................",

/* 8*/ "................................",
/* 9*/ "................................",
/*10*/ "...x...x...x...x................",
/*11*/ "................................",
/*12*/ "................................",
/*13*/ "................................",
/*14*/ "...x...x...x...x................",
/*15*/ "................................",

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

#define TEST_IMPL(v1, v2, result) \
    rval &= test_modulus(        \
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
        test_multiplication_result[value_index1][value_index2] \
    )
/**/

#define COUNT sizeof(test_multiplication_result)
int main(int argc, char * argv[]){
    bool rval = true;
    TEST_EACH_VALUE_PAIR
    return ! rval ;
}
