//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <exception>
#include <cassert>

// we could have used decltype and auto for C++11 but we've decided
// to use boost/typeof to be compatible with older compilers
#include <boost/typeof/typeof.hpp>

#include "../include/safe_integer.hpp"

template<class T1, class T2>
bool test_subtract(
    T1 v1,
    T2 v2,
    const char *av1,
    const char *av2,
    char expected_result
){
    std::cout
        << "testing  "
        << av1 << " - " << av2
        << std::endl;
    {
        boost::numeric::safe<T1> t1 = v1;
        // presuming native policy
        boost::numeric::safe<decltype(v1 - v2)> result;

        try{
            result = t1 - v2;
            static_assert(
                boost::numeric::is_safe<decltype(t1 + v2)>::value,
                "Expression failed to return safe type"
            );
            if(expected_result == 'x'){
                std::cout
                    << "failed to detect error in subtraction "
                    << std::hex << result << "(" << std::dec << result << ")"
                    << " ! = "<< av1 << " - " << av2
                    << std::endl;
                try{
                    result = t1 - v2;
                }
                catch(std::exception){}
                return false;
            }
        }
        catch(std::exception){
            if(expected_result == '.'){
                std::cout
                    << "erroneously detected error in subtraction "
                    << std::hex << result << "(" << std::dec << result << ")"
                    << " == "<< av1 << " - " << av2
                    << std::endl;
                try{
                    result = t1 - v2;
                }
                catch(std::exception){}
                return false;
            }
        }
    }
    {
        boost::numeric::safe<T1> t1 = v1;
        boost::numeric::safe<T2> t2 = v2;

        // presuming native policy
        boost::numeric::safe<decltype(v1 + v2)> result;

        try{
            result = t1 - t2;
            static_assert(
                boost::numeric::is_safe<decltype(t1 + v2)>::value,
                "Expression failed to return safe type"
            );
            if(expected_result == 'x'){
                std::cout
                    << "failed to detect error in subtraction "
                    << std::hex << result << "(" << std::dec << result << ")"
                    << " ! = "<< av1 << " - " << av2
                    << std::endl;
                try{
                    result = t1 - t2;
                }
                catch(std::exception){}
                return false;
            }
        }
        catch(std::exception){
            if(expected_result == '.'){
                std::cout
                    << "erroneously detected error in subtraction "
                    << std::hex << result << "(" << std::dec << result << ")"
                    << " == "<< av1 << " - " << av2
                    << std::endl;
                try{
                    result = t1 - t2;
                }
                catch(std::exception){}
                return false;
            }
        }
    }
    return true; // correct result
}

#include "test.hpp"
#include "test_values.hpp"

const char *test_subtraction_result[VALUE_ARRAY_SIZE] = {
//      0       0       0       0
//      01234567012345670123456701234567
//      01234567890123456789012345678901
/* 0*/ "..........x...x..........xxx.xxx",
/* 1*/ "..........x...x..........xxx.xxx",
/* 2*/ ".........x...x..........xxxxxxxx",
/* 3*/ "........................xxxxxxxx",
/* 4*/ "..........x...x..........xxx.xxx",
/* 5*/ "..........x...x..........xxx.xxx",
/* 6*/ ".........x...x..........xxxxxxxx",
/* 7*/ "........................xxxxxxxx",

/* 8*/ "..........x...x..........xxx.xxx",
/* 9*/ "..xx..xx..xx..x...........xx.xxx",
/*10*/ "xx..xx..xx...x..xxxxxxxxxxxxxxxx",
/*11*/ "........................xxxxxxxx",
/*12*/ "..............x..............xxx",
/*13*/ "..xx..xx..xx..xx..............xx",
/*14*/ "xx..xx..xx..xx..xxxxxxxxxxxxxxxx",
/*15*/ "............................xxxx",

//      0       0       0       0
//      01234567012345670123456701234567
//      01234567890123456789012345678901
/*16*/ "..........x...x..........xxx.xxx",
/*17*/ "..........x...x..........xxx.xxx",
/*18*/ "..........x...x..........xxx.xxx",
/*19*/ "..........x...x..........xxx.xxx",
/*20*/ "..........x...x..........xxx.xxx",
/*21*/ "..........x...x..........xxx.xxx",
/*22*/ "..........x...x..........xxx.xxx",
/*23*/ "..........x...x..........xxx.xxx",

/*24*/ ".xxx.xxx.xxx..x..xxx.xxx.xxx.xxx",
/*25*/ "..xx..xx..xx..x...........xx.xxx",
/*26*/ "..xx..xx..xx..x............x.xxx",
/*27*/ "..xx..xx..xx..x..............xxx",
/*28*/ ".xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx",
/*29*/ "..xx..xx..xx..xx..............xx",
/*30*/ "..xx..xx..xx..xx...............x",
/*31*/ "..xx..xx..xx..xx................"
};

#define TEST_IMPL(v1, v2, result) \
    rval &= test_subtract(        \
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

int main(int argc, char * argv[]){
    bool rval = true;
    TEST_EACH_VALUE_PAIR
    return ! rval ;
}
