#ifndef BOOST_TEST_MULTIPLY_HPP
#define BOOST_TEST_MULTIPLY_HPP

//  Copyright (c) 2015 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <exception>
#include <boost/core/demangle.hpp>

#include "../include/safe_integer.hpp"

template<class T1, class T2>
bool test_multiply(
    T1 v1,
    T2 v2,
    const char *av1,
    const char *av2,
    char expected_result
){
    std::cout << "testing"<< std::endl;
    {
        safe_t<T1> t1 = v1;

        using result_type = decltype(t1 * v2);
        std::cout << "safe<" << av1 << "> * " << av2 << " -> ";
        static_assert(
            boost::numeric::is_safe<safe_t<T1> >::value,
            "safe_t not safe!"
        );
        static_assert(
            boost::numeric::is_safe<result_type>::value,
            "Expression failed to return safe type"
        );
        result_type result;

        try{
            result = t1 * v2;
            std::cout << std::hex << result << "(" << std::dec << result << ")"
            << std::endl;
            if(expected_result == 'x'){
                const std::type_info & ti = typeid(result);
                std::cout
                    << "*** failed to detect error in multiplication "
                    << boost::core::demangle(ti.name()) << '\n'
                    << std::endl;
                try{
                    t1 * v2;
                }
                catch(std::exception){}
                return false;
            }
        }
        catch(std::exception){
            std::cout << std::hex << result << "(" << std::dec << result << ")"
            << std::endl;
            if(expected_result == '.'){
                const std::type_info & ti = typeid(result);
                std::cout
                    << "*** erroneously detected error in multiplication "
                    << boost::core::demangle(ti.name()) << '\n'
                    << std::endl;
                try{
                    t1 * v2;
                }
                catch(std::exception){}
                return false;
            }
        }
    }
    {
        safe_t<T2> t2 = v2;
        using result_type = decltype(v1 * t2);
        std::cout << av1 << " * " << "safe<" << av2 << "> -> ";
        static_assert(
            boost::numeric::is_safe<safe_t<T2> >::value,
            "safe_t not safe!"
        );
        static_assert(
            boost::numeric::is_safe<result_type>::value,
            "Expression failed to return safe type"
        );
        result_type result;

        try{
            result = v1 * t2;
            std::cout << std::hex << result << "(" << std::dec << result << ")"
            << std::endl;
            if(expected_result == 'x'){
                const std::type_info & ti = typeid(result);
                std::cout
                    << "*** failed to detect error in multiplication "
                    << boost::core::demangle(ti.name()) << '\n'
                    << std::endl;
                try{
                    v1 * t2;
                }
                catch(std::exception){}
                return false;
            }
        }
        catch(std::exception){
            std::cout << std::hex << result << "(" << std::dec << result << ")"
            << std::endl;
            if(expected_result == '.'){
                const std::type_info & ti = typeid(result);
                std::cout
                    << "*** erroneously detected error in multiplication "
                    << boost::core::demangle(ti.name()) << '\n'
                    << std::endl;
                try{
                    v1 * t2;
                }
                catch(std::exception){}
                return false;
            }
        }
    }
    {
        safe_t<T1> t1 = v1;
        safe_t<T2> t2 = v2;

        using result_type = decltype(t1 * t2);
        std::cout << "safe<" << av1 << "> * " << "safe<" << av2 << "> -> ";
        static_assert(
            boost::numeric::is_safe<result_type>::value,
            "Expression failed to return safe type"
        );
        result_type result;

        try{
            result = t1 * t2;
            std::cout << std::hex << result << "(" << std::dec << result << ")"
            << std::endl;
            if(expected_result == 'x'){
                const std::type_info & ti = typeid(result);
                std::cout
                    << "*** failed to detect error in multiplication "
                    << boost::core::demangle(ti.name()) << '\n'
                    << std::endl;
                try{
                    t1 * t2;
                }
                catch(std::exception){}
                return false;
            }
        }
        catch(std::exception){
            std::cout << std::hex << result << "(" << std::dec << result << ")"
            << std::endl;
            if(expected_result == '.'){
                const std::type_info & ti = typeid(result);
                std::cout
                    << "*** erroneously detected error in multiplication "
                    << boost::core::demangle(ti.name()) << '\n'
                    << std::endl;
                try{
                    t1 * t2;
                }
                catch(std::exception){}
                return false;
            }
        }
    }
    return true;
}


#endif // BOOST_TEST_MULTIPLY
