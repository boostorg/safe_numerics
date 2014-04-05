#ifndef TEST_SUBTRACT_HPP
#define TEST_SUBTRACT_HPP

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
#include <algorithm>
#include <boost/mpl/or.hpp>
#include <boost/mpl/if.hpp>

#include "../include/safe_integer.hpp"
#include "../include/safe_cast.hpp"
#include "../include/safe_compare.hpp"
#include "../include/numeric.hpp"

#include "test.hpp"

template<class T1, class T2>
struct subtract_result {
    typedef typename boost::mpl::if_<
        boost::mpl::or_<
            boost::numeric::is_signed<T1>,
            boost::numeric::is_signed<T2>
        >,
        boost::intmax_t,
        boost::uintmax_t
    >::type type;
}; 

template<class T1, class T2>
bool test_subtract(
    T1 v1,
    T2 v2,
    const char *av1,
    const char *av2
){
        typename subtract_result<T1, T2>::type result;
    try{
        boost::numeric::safe<T1> t1 = v1;

        result = t1 - v2;
        if(boost::numeric::safe_compare::less_than(
            boost::numeric::bits<boost::uintmax_t>::value,
            std::max(count_bits(v1),count_bits(v2))
        )){
            std::cout
                << "failed to detect error in subtraction "
                << av1 << " - " << av2
                << std::endl;
            return false;
        }
    }
    catch(std::range_error){
        if(boost::numeric::safe_compare::greater_than_equal(
            boost::numeric::bits<boost::uintmax_t>::value,
            std::max(count_bits(v1),count_bits(v2))
        )){
            std::cout
                << "erroneously detected error in subtraction "
                << av1 << " - " << av2
                << std::endl;
            return false;
        }
    }
    return true; // correct result
}


#endif // TEST_SUBTRACT_HPP
