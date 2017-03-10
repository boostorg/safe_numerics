#ifndef BOOST_TEST_ADD_HPP
#define BOOST_TEST_ADD_HPP

//  Copyright (c) 2015 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <exception>

#include "../include/safe_integer.hpp"

template<class T1, class T2>
bool test_left_shift(
    T1 v1,
    T2 v2,
    const char *av1,
    const char *av2,
    char expected_result
){
    std::cout
        << "testing  "
        << av1 << " << " << av2
        << std::endl;
    {
        safe_t<T1> t1 = v1;
        using result_type = decltype(t1 << v2);
        static_assert(
            boost::numeric::is_safe<result_type>::value,
            "Expression failed to return safe type"
        );
        result_type result;

        try{
            result = t1 << v2;
            
            if(expected_result == 'x'){
                std::cout
                    << "failed to detect error in left shift "
                    << std::hex << result << "(" << std::dec << result << ")"
                    << " ! = "<< av1 << " << " << av2
                    << std::endl;
                try{
                    t1 << v2;
                }
                catch(std::exception){}
                return false;
            }
        }
        catch(const std::exception & e){
            if(expected_result == '.'){
                std::cout
                    << "erroneously detected error in left shift "
                    << std::hex << result << "(" << std::dec << result << ")"
                    << " == "<< av1 << " << " << av2
                    << ' ' << e.what()
                    << std::endl;
                try{
                    t1 << v2;
                }
                catch(...){}
                return false;
            }
        }
    }
    {
        safe_t<T2> t2 = v2;
        using result_type = decltype(v1 << t2);
        static_assert(
            boost::numeric::is_safe<result_type>::value,
            "Expression failed to return safe type"
        );
        result_type result;

        try{
            result = v1 << t2;
            if(expected_result == 'x'){
                std::cout
                    << "failed to detect error in left shift "
                    << std::hex << result << "(" << std::dec << result << ")"
                    << " ! = "<< av1 << " << " << av2
                    << std::endl;
                try{
                    v1 << t2;
                }
                catch(std::exception){}
                return false;
            }
        }
        catch(const std::exception & e){
            if(expected_result == '.'){
                std::cout
                    << "erroneously detected error in left shift "
                    << std::hex << result << "(" << std::dec << result << ")"
                    << " == "<< av1 << " << " << av2
                    << ' ' << e.what()
                    << std::endl;
                try{
                    v1 << t2;
                }
                catch(std::exception){}
                return false;
            }
        }
    }
    {
        safe_t<T1> t1 = v1;
        safe_t<T2> t2 = v2;
        using result_type = decltype(t1 << t2);
        static_assert(
            boost::numeric::is_safe<result_type>::value,
            "Expression failed to return safe type"
        );
        result_type result;
        
        try{
            result = t1 << t2;
            if(expected_result == 'x'){
                std::cout
                    << "failed to detect error in left shift "
                    << std::hex << result << "(" << std::dec << result << ")"
                    << " ! = "<< av1 << " << " << av2
                    << std::endl;
                try{
                    t1 << t2;
                }
                catch(std::exception){}
                return false;
            }
        }
        catch(const std::exception & e){
            if(expected_result == '.'){
                std::cout
                    << "erroneously detected error in left shift "
                    << std::hex << result << "(" << std::dec << result << ")"
                    << " == "<< av1 << " << " << av2
                    << ' ' << e.what()
                    << std::endl;
                try{
                    t1 << t2;
                }
                catch(std::exception){}
                return false;
            }
        }
    }
    return true; // correct result
}

#endif // BOOST_TEST_DIVIDE
