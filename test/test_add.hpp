#ifndef TEST_ADD_HPP
#define TEST_ADD_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <algorithm> // max

#include "../include/numeric.hpp"
#include "../include/safe_integer.hpp"

#include "test.hpp"

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

    typename boost::numeric::addition_result_type<T1, T2>::type result;

    try{
        boost::numeric::safe<T1> t1 = v1;
        result = t1 + v2;

        if(expected_result == 'x'){
            std::cout
                << "failed to detect error in addition "
                << av1 << " + " << av2
                << std::endl;
        result = t1 + v2;
            return false;
        }
    }
    catch(std::range_error){
        if(expected_result != 'x'){
            std::cout
                << "erroneously detected error in addition "
                << av1 << " + " << av2
                << std::endl;
            return false;
        }
    }

    return true; // correct result
}

#endif // TEST_ADD_HPP
