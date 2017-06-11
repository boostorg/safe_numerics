#ifndef BOOST_TEST_CHECKED_DIVIDE_HPP
#define BOOST_TEST_CHECKED_DIVIDE_HPP

//  Copyright (c) 2015 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

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

    checked_result<result_type> result
        = checked::divide<result_type>(v1, v2);

    if(! result.exception()
    && expected_result != '.'){
        std::cout
            << "failed to detect error in division "
            << std::hex << "0x" << result << "(" << std::dec << result << ")"
            << " != "<< av1 << " / " << av2
            << std::endl;
        result = checked::divide<result_type>(v1, v2);
        return false;
    }
    else
    if(result.exception()
    && expected_result != 'x'){
        std::cout
            << "erroneously detected error "
            << std::hex << result <<  av1 << " / " << av2 << std::dec << std::endl;
        result = checked::divide<result_type>(v1, v2);
        return false;
    }
    return true; // correct result
}

#endif // BOOST_TEST_CHECKED_DIVIDE
