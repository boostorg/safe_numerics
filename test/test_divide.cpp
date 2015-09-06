//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <exception>
#include <cxxabi.h>

#include "../include/safe_integer.hpp"

template <class T>
using safe_t = boost::numeric::safe<T>;

template<class T1, class T2>
bool test_divide(
    T1 v1,
    T2 v2,
    const char *av1,
    const char *av2,
    char expected_result
){
    using namespace boost::numeric;
    auto unsafe_result = v1 / v2;
    {
        std::cout << "testing  safe<" << av1 << "> / " << av2 << " -> ";
        static_assert(is_safe<safe_t<T1> >::value, "safe_t not safe!");

        safe_t<T1> t1 = v1;

        using result_type = decltype(t1 / v2);
        result_type result;

        static_assert(
            is_safe<result_type>::value,
            "Expression failed to return safe type"
        );

        try{
            result = t1 / v2;
            std::cout << std::hex << result << "(" << std::dec << result << ")"
                << std::endl;
            if(expected_result == 'x'){
                const std::type_info & ti = typeid(result);
                int status;
                std::cout
                    << "*** failed to detect error in division "
                    << abi::__cxa_demangle(ti.name(),0,0,&status) << '\n'
                    << std::endl;
                try{
                    t1 / v2;
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
                    << "*** erroneously detected error in division "
                    << abi::__cxa_demangle(ti.name(),0,0,&status) << '\n'
                    << std::endl;
                try{
                    t1 / v2;
                }
                catch(std::exception){}
                assert(result == unsafe_result);
                return false;
            }
        }
    }
    {
        std::cout << "testing  " << av1 << " * " << "safe<"<< av2 << "> -> ";
        static_assert(is_safe<safe_t<T2> >::value, "safe_t not safe!");

        safe_t<T2> t2 = v2;

        using result_type = decltype(v1 / t2);
        result_type result;

        static_assert(
            is_safe<result_type>::value,
            "Expression failed to return safe type"
        );

        try{
            result = v1 / t2;
            std::cout << std::hex << result << "(" << std::dec << result << ")"
                << std::endl;
            if(expected_result == 'x'){
                const std::type_info & ti = typeid(result);
                int status;
                std::cout
                    << "*** failed to detect error in division "
                    << abi::__cxa_demangle(ti.name(),0,0,&status) << '\n'
                    << std::endl;
                try{
                    v1 / t2;
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
                    << "*** erroneously detected error in division "
                    << abi::__cxa_demangle(ti.name(),0,0,&status) << '\n'
                    << std::endl;
                try{
                    v1 / t2;
                }
                catch(std::exception){}
                assert(result == unsafe_result);
                return false;
            }
        }
    }
    {
        std::cout << "testing  safe<" << av1 << "> * safe<" << av2 << "> -> ";
        safe_t<T1> t1 = v1;
        safe_t<T2> t2 = v2;

        using result_type = decltype(t1 / t2);
        result_type result;

        static_assert(
            is_safe<result_type>::value,
            "Expression failed to return safe type"
        );

        try{
            result = t1 / t2;
            std::cout << std::hex << result << "(" << std::dec << result << ")"
                << std::endl;
            if(expected_result == 'x'){
                const std::type_info & ti = typeid(result);
                int status;
                std::cout
                    << "*** failed to detect error in division "
                    << abi::__cxa_demangle(ti.name(),0,0,&status) << '\n'
                    << std::endl;
                try{
                    t1 / t2;
                }
                catch(std::exception){}
                // assert(result != unsafe_result);
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
                    << "*** erroneously detected error in division "
                    << abi::__cxa_demangle(ti.name(),0,0,&status) << '\n'
                    << std::endl;
                try{
                    t1 / t2;
                }
                catch(std::exception){}
                // assert(result == unsafe_result);
                return false;
            }
        }
    }
    return true;
}

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
/*14*/ "...x...x...x...x...........xxxxx",
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
    rval &= test_divide(        \
        v1,                       \
        v2,                       \
        BOOST_PP_STRINGIZE(v1),   \
        BOOST_PP_STRINGIZE(v2),   \
        result                    \
    );
/**/

#define TESTX(value_index1, value_index2)          \
    (std::cout << value_index1 << ',' << value_index2 << '\n'); \
    TEST_IMPL(                                     \
        BOOST_PP_ARRAY_ELEM(value_index1, VALUES), \
        BOOST_PP_ARRAY_ELEM(value_index2, VALUES), \
        test_division_result[value_index1][value_index2] \
    )
/**/

int main(int argc, char * argv[]){
    bool rval = true;
    TEST_EACH_VALUE_PAIR
    return ! rval ;
}
