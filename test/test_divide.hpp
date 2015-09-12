#ifndef BOOST_TEST_DIVIDE_HPP
#define BOOST_TEST_DIVIDE_HPP

//  Copyright (c) 2015 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <exception>
#include <cxxabi.h>

#include "../include/safe_integer.hpp"

template<class T1, class T2>
bool test_divide(
    T1 v1,
    T2 v2,
    const char *av1,
    const char *av2,
    char expected_result
){
    using namespace boost::numeric;
    // can't so this as -min / -1 invokes hardware trap

    decltype(v1 / v2) unsafe_result;
    if(expected_result == '.')
        //unsafe_result = v1 / v2;
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
                //assert(result != unsafe_result);
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
                //assert(result == unsafe_result);
                return false;
            }
        }
    }
    {
        std::cout << "testing  " << av1 << " / " << "safe<"<< av2 << "> -> ";
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
                //assert(result != unsafe_result);
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
        std::cout << "testing  safe<" << av1 << "> / safe<" << av2 << "> -> ";
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
                assert(result == unsafe_result);
                return false;
            }
        }
    }
    return true;
}

#endif // BOOST_TEST_DIVIDE
