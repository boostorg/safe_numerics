#ifndef TEST_MODULUS_HPP
#define TEST_MODULUS_HPP

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
#include <boost/cstdint.hpp>
#include <boost/limits.hpp>

#include "../include/safe_integer.hpp"
#include "../include/safe_cast.hpp"
#include "../include/safe_compare.hpp"

#include "test.hpp"

template<class T1, class T2, class V>
bool test_modulus(
    V v1, 
    V v2, 
    const char *at1,
    const char *at2,
    const char *av1,
    const char *av2
){
    bool success;

    T1 t1;
    try{
        t1 = boost::numeric::safe_cast<T1>(v1);
        success = true;
    }
    catch(std::range_error){
        success = false;
    }
    if(success){
        if(boost::numeric::safe_compare::greater_than(
                v1,
                std::numeric_limits<T1>::max()
            )
        || boost::numeric::safe_compare::less_than(
                v1,
                std::numeric_limits<T1>::min()
            )
        ){
            std::cout
                << "constructed invalid value "
                << at1 << ' ' << av1
                << std::endl;
            return false;
        }
    }
    else{
        if(! boost::numeric::safe_compare::greater_than(
                v1,
                std::numeric_limits<T1>::max()
            )
        && ! boost::numeric::safe_compare::less_than(
                v1,
                std::numeric_limits<T1>::min()
            )
        ){
            std::cout
                << "failed to construct valid value "
                << at1 << ' ' << av1
                << std::endl;
            return false;
        }
        return true;
    }

    T2 t2;
    try{
        t2 = boost::numeric::safe_cast<T2>(v2);
        success = true;
    }
    catch(std::range_error){
        success = false;
    }
    if(success){
        if(boost::numeric::safe_compare::greater_than(
            v2,
            std::numeric_limits<T2>::max()
        )
        || boost::numeric::safe_compare::less_than(
            v2,
            std::numeric_limits<T2>::min()
        )){
            std::cout
                << "constructed invalid value "
                << at2 << ' ' << av2
                << std::endl;
            return false;
        }
    }
    else{
        if(!boost::numeric::safe_compare::greater_than(
            v2,
            std::numeric_limits<T2>::max()
        )
        && !boost::numeric::safe_compare::less_than(
            v2,
            std::numeric_limits<T2>::min()
        )){
            std::cout
                << "failed to construct valid value "
                << at2 << ' ' << av2
                << std::endl;
            return false;
        }
        return true;
    }

    V result;
    try{
        result = t1 % t2;
        success = true;
    }
    catch(std::range_error){
        success = false;
    }
    if(success){
        if(result != v1 % v2){
            std::cout
                << "failed to detect error in modulus "
                << at1 << ' ' << at2 << ' ' << av1 << ' ' << av2
                << std::endl;
            return false;
        }
    }
    else{
        if(boost::numeric::safe_compare::greater_than_equal(
            boost::numeric::bits<boost::uintmax_t>::value, 
            std::max(count_bits(v1),count_bits(v2))
        )){
            std::cout
                << "erroneously detected error in modulus "
                << at1 << ' ' << at2 << ' ' << av1 << ' ' << av2
                << std::endl;
            return false;
        }
    }
    return true; // correct result
}

template<class T1, class T2>
struct modulus_result {
    typedef typename boost::mpl::if_<
        boost::mpl::or_<
            boost::numeric::is_signed<T1>,
            boost::numeric::is_signed<T2>
        >,
        boost::intmax_t,
        boost::uintmax_t
    >::type type;
}; 

#define TEST_IMPL(a, b, c, d)                   \
    rval &= test_modulus<d, c>(                \
        (static_cast<modulus_result<c, d>::type>(a)),   \
        (static_cast<modulus_result<c, d>::type>(b)),   \
        BOOST_PP_STRINGIZE(d),                  \
        BOOST_PP_STRINGIZE(c),                  \
        BOOST_PP_STRINGIZE(b),                  \
        BOOST_PP_STRINGIZE(a)                   \
    );                                          \
/**/

#endif // TEST_MODULUS_HPP
