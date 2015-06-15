#ifndef BOOST_NUMERIC_NATIVE_HPP
#define BOOST_NUMERIC_NATIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <type_traits>
#include <limits>
#include "concept/promotion_policy.hpp"
// policy which creates results types and values equal to that of C++ promotions.
// When used in conjunction with a desired exception policy, traps errors but
// does not otherwise alter the results produced by the program using it.
namespace boost {
namespace numeric {

// forward declaration - safe type
template<
    class BaseType,
    class PromotionPolicy,
    class ExceptionPolicy
>
struct safe;

struct native {
    template<typename T, typename U, typename P, typename E>
    struct addition_result {
        typedef typename base_type<T>::type base_type_t;
        typedef typename base_type<U>::type base_type_u;
        typedef decltype(base_type_t() + base_type_u()) result_base_type;
        typedef safe<result_base_type, P, E> type;
    };
    template<typename T, typename U, typename P, typename E>
    struct subtraction_result {
        typedef typename base_type<T>::type base_type_t;
        typedef typename base_type<U>::type base_type_u;
        typedef decltype(base_type_t() - base_type_u()) result_base_type;
        typedef safe<result_base_type, P, E> type;
    };
    template<typename T, typename U, typename P, typename E>
    struct multiplication_result {
        typedef typename base_type<T>::type base_type_t;
        typedef typename base_type<U>::type base_type_u;
        typedef decltype(base_type_t() * base_type_u()) result_base_type;
        typedef safe<result_base_type, P, E> type;
    };
    template<typename T, typename U, typename P, typename E>
    struct division_result {
        typedef typename base_type<T>::type base_type_t;
        typedef typename base_type<U>::type base_type_u;
        typedef decltype(base_type_t() / base_type_u()) result_base_type;
        typedef safe<result_base_type, P, E> type;
    };
};

} // numeric
} // boost

#endif // BOOST_NUMERIC_NATIVE_HPP
