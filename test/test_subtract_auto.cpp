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

template<class T1, class T2>
bool test_subtract_auto(
    T1 v1,
    T2 v2,
    const char *av1,
    const char *av2,
    char expected_result
){
    auto unsafe_result = v1 + v2;
    {
        std::cout << "testing  safe<" << av1 << "> - " << av2 << " -> ";
        static_assert(boost::numeric::is_safe<safe_t<T1> >::value, "safe_t not safe!");

        safe_t<T1> t1 = v1;

        using result_type = decltype(t1 - v2);
        result_type result;

        static_assert(
            boost::numeric::is_safe<result_type>::value,
            "Expression failed to return safe type"
        );

        try{
            result = t1 - v2;
            std::cout << std::hex << result << "(" << std::dec << result << ")"
                << std::endl;
            if(expected_result == 'x'){
                const std::type_info & ti = typeid(result);
                int status;
                std::cout
                    << "*** failed to detect error in subtraction "
                    << abi::__cxa_demangle(ti.name(),0,0,&status) << '\n'
                    << std::endl;
                try{
                    t1 - v2;
                }
                catch(std::exception){}
                assert(result != unsafe_result);
                return false;
            }
        }
        catch(std::exception){
            std::cout << std::hex << result << "(" << std::dec << result << ")"
                << std::endl;
            if(expected_result == '.'){
                const std::type_info & ti = typeid(result);
                int status;
                std::cout
                    << "*** erroneously detected error in subtraction "
                    << abi::__cxa_demangle(ti.name(),0,0,&status) << '\n'
                    << std::endl;
                try{
                    t1 - v2;
                }
                catch(std::exception){}
                assert(result == unsafe_result);
                return false;
            }
        }
    }
    {
        std::cout << "testing  " << av1 << " - " << "safe<"<< av2 << "> -> ";
        static_assert(boost::numeric::is_safe<safe_t<T2> >::value, "safe_t not safe!");

        safe_t<T2> t2 = v2;

        using result_type = decltype(v1 - t2);
        result_type result;

        static_assert(
            boost::numeric::is_safe<result_type>::value,
            "Expression failed to return safe type"
        );

        try{
            result = v1 - t2;
            std::cout << std::hex << result << "(" << std::dec << result << ")"
                << std::endl;
            if(expected_result == 'x'){
                const std::type_info & ti = typeid(result);
                int status;
                std::cout
                    << "*** failed to detect error in subtraction "
                    << abi::__cxa_demangle(ti.name(),0,0,&status) << '\n'
                    << std::endl;
                try{
                    v1 - t2;
                }
                catch(std::exception){}
                assert(result != unsafe_result);
                return false;
            }
        }
        catch(std::exception){
            std::cout << std::hex << result << "(" << std::dec << result << ")"
                << std::endl;
            if(expected_result == '.'){
                const std::type_info & ti = typeid(result);
                int status;
                std::cout
                    << "*** erroneously detected error in subtraction "
                    << abi::__cxa_demangle(ti.name(),0,0,&status) << '\n'
                    << std::endl;
                try{
                    v1 - t2;
                }
                catch(std::exception){}
                assert(result == unsafe_result);
                return false;
            }
        }
    }
    {
        std::cout << "testing  safe<" << av1 << "> - safe<" << av2 << "> -> ";
        safe_t<T1> t1 = v1;
        safe_t<T2> t2 = v2;

        using result_type = decltype(t1 - t2);
        result_type result;

        static_assert(
            boost::numeric::is_safe<result_type>::value,
            "Expression failed to return safe type"
        );

        try{
            result = t1 - t2;
            std::cout << std::hex << result << "(" << std::dec << result << ")"
                << std::endl;
            if(expected_result == 'x'){
                const std::type_info & ti = typeid(result);
                int status;
                std::cout
                    << "*** failed to detect error in subtraction "
                    << abi::__cxa_demangle(ti.name(),0,0,&status) << '\n'
                    << std::endl;
                try{
                    t1 - t2;
                }
                catch(std::exception){}
                assert(result != unsafe_result);
                return false;
            }
        }
        catch(std::exception){
            std::cout << std::hex << result << "(" << std::dec << result << ")"
                << std::endl;
            if(expected_result == '.'){
                const std::type_info & ti = typeid(result);
                int status;
                std::cout
                    << "*** erroneously detected error in subtraction "
                    << abi::__cxa_demangle(ti.name(),0,0,&status) << '\n'
                    << std::endl;
                try{
                    t1 - t2;
                }
                catch(std::exception){}
                assert(result == unsafe_result);
                return false;
            }
        }
    }
    return true; // correct result
}

#include "test.hpp"
#include "test_values.hpp"

// note: same test matrix as used in test_checked.  Here we test all combinations
// safe and unsafe integers.  in test_checked we test all combinations of
// integer primitives

const char *test_subtraction_result[VALUE_ARRAY_SIZE] = {
//      0       0       0       0
//      01234567012345670123456701234567
//      01234567890123456789012345678901
/* 0*/ "..............x..............xxx",
/* 1*/ "..............x..............xxx",
/* 2*/ ".............x..............xxxx",
/* 3*/ "............................xxxx",
/* 4*/ "..............x..............xxx",
/* 5*/ "..............x..............xxx",
/* 6*/ ".............x..............xxxx",
/* 7*/ "............................xxxx",

/* 8*/ "..............x..............xxx",
/* 9*/ "..............x..............xxx",
/*10*/ ".............x..............xxxx",
/*11*/ "............................xxxx",
/*12*/ "..............x..............xxx",
/*13*/ "..xx..xx..xx..xx..............xx",
/*14*/ "xx..xx..xx..xx..xxxxxxxxxxxxxxxx",
/*15*/ "............................xxxx",

//      0       0       0       0
//      01234567012345670123456701234567
//      01234567890123456789012345678901
/*16*/ "..............x..xxx.xxx.xxx.xxx",
/*17*/ "..............x...xx.xxx.xxx.xxx",
/*18*/ "..............x....x.xxx.xxx.xxx",
/*19*/ "..............x......xxx.xxx.xxx",
/*20*/ "..............x..xxx.xxx.xxx.xxx",
/*21*/ "..............x.......xx.xxx.xxx",
/*22*/ "..............x........x.xxx.xxx",
/*23*/ "..............x..........xxx.xxx",

/*24*/ "..............x..xxx.xxx.xxx.xxx",
/*25*/ "..............x...........xx.xxx",
/*26*/ "..............x............x.xxx",
/*27*/ "..............x..............xxx",
/*28*/ ".xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx",
/*29*/ "..xx..xx..xx..xx..............xx",
/*30*/ "..xx..xx..xx..xx...............x",
/*31*/ "..xx..xx..xx..xx................"
};

#define TEST_IMPL(v1, v2, result) \
    rval &= test_subtract_auto(   \
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
        test_subtraction_result[value_index1][value_index2] \
    )
/**/

int main(int argc, char * argv[]){
    bool rval = true;
    TEST_EACH_VALUE_PAIR
    return ! rval ;
}
