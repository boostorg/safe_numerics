//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <cassert>
#include <iostream>

#include "../include/safe_integer.hpp"

template<class T1, class T2>
bool test_add(
    T1 v1,
    T2 v2,
    const char *av1,
    const char *av2,
    char expected_result
){
    std::cout
        << "testing  "
        << av1 << " + " << av2
        << std::endl;

    boost::numeric::safe<T1> t1 = v1;
    decltype(boost::numeric::safe<T1>() + T2()) result;

    try{
        result = t1 + v2;

        static_assert(
            boost::numeric::is_safe<decltype(t1 + v2)>::value,
            "Expression failed to return safe type"
        );

        if(expected_result == 'x'){
            std::cout
                << "failed to detect error in addition "
                << std::hex << result << "(" << std::dec << result << ")"
                << " ! = "<< av1 << " + " << av2
                << std::endl;
            try{
                result = t1 + v2;
            }
            catch(...){}
            return false;
        }
    }
    catch(std::exception & e){
        if(expected_result == '.'){
            std::cout
                << "erroneously detected error in addition "
                << std::hex << result << "(" << std::dec << result << ")"
                << " == "<< av1 << " + " << av2
                << std::endl;
            try{
                result = t1 + v2;
            }
            catch(...){}
            return false;
        }
    }

    boost::numeric::safe<T2> t2 = v2;
    try{
        result = t1 + t2;

        static_assert(
            boost::numeric::is_safe<decltype(t1 + t2)>::value,
            "Expression failed to return safe type"
        );

        if(expected_result == 'x'){
            std::cout
                << "failed to detect error in addition "
                << std::hex << result << "(" << std::dec << result << ")"
                << " ! = "<< av1 << " + " << av2
                << std::endl;
            try{
                result = t1 + v2;
            }
            catch(...){}
            return false;
        }
    }
    catch(std::exception & e){
        if(expected_result == '.'){
            std::cout
                << "erroneously detected error in addition "
                << std::hex << result << "(" << std::dec << result << ")"
                << " == "<< av1 << " + " << av2
                << std::endl;
            try{
                result = t1 + t2;
            }
            catch(...){}
            return false;
        }
    }
    return true; // correct result
}

#include "test.hpp"
#include "test_values.hpp"

const char *test_addition_result[VALUE_ARRAY_SIZE] = {
//      0       0       0       0
//      01234567012345670123456701234567
//      01234567890123456789012345678901
/* 0*/ ".........x...x.............x...x",
/* 1*/ ".........x...x.............x...x",
/* 2*/ "..........x...x.........xxxxxxxx",
/* 3*/ "..........x...x.........xxxxxxxx",
/* 4*/ ".........x...x.............x...x",
/* 5*/ ".........x...x.............x...x",
/* 6*/ "..........x...x.........xxxxxxxx",
/* 7*/ "..........x...x.........xxxxxxxx",

/* 8*/ ".........x...x.............x...x",
/* 9*/ "xx..xx..xx...x..xxxxxxxx...x...x",
/*10*/ "..xx..xx..xx..x.........xxxxxxxx",
/*11*/ "..........x...x.........xxxxxxxx",
/*12*/ ".............x.................x",
/*13*/ "xx..xx..xx..xx..xxxxxxxxxxxx...x",
/*14*/ "..xx..xx..xx..xx............xxxx",
/*15*/ "..............x.............xxxx",

//      0       0       0       0
//      01234567012345670123456701234567
//      01234567890123456789012345678901
/*16*/ ".........x...x.............x...x",
/*17*/ ".........x...x.............x...x",
/*18*/ ".........x...x.............x...x",
/*19*/ ".........x...x.............x...x",
/*20*/ ".........x...x.............x...x",
/*21*/ ".........x...x.............x...x",
/*22*/ ".........x...x.............x...x",
/*23*/ ".........x...x.............x...x",

/*24*/ "..xx..xx..xx.x.............x...x",
/*25*/ "..xx..xx..xx.x.............x...x",
/*26*/ "..xx..xx..xx.x............xx...x",
/*27*/ "xxxxxxxxxxxx.x..xxxxxxxxxxxx...x",
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
    (std::cout << value_index1 << ',' << value_index2 << ','); \
    TEST_IMPL(                                     \
        BOOST_PP_ARRAY_ELEM(value_index1, VALUES), \
        BOOST_PP_ARRAY_ELEM(value_index2, VALUES), \
        test_addition_result[value_index1][value_index2] \
    )
/**/

#define COUNT sizeof(test_addition_result)
int main(int argc, char * argv[]){
    // sanity check on test matrix - should be symetrical
    for(int i = 0; i < VALUE_ARRAY_SIZE; ++i)
        for(int j = i + 1; j < VALUE_ARRAY_SIZE; ++j)
            assert(test_addition_result[i][j] == test_addition_result[j][i]);

    bool rval = true;
    TEST_EACH_VALUE_PAIR
    return ! rval ;
}
