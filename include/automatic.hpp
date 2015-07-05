#ifndef BOOST_NUMERIC_AUTOMATIC_HPP
#define BOOST_NUMERIC_AUTOMATIC_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// policy which creates results types equal to that of C++ promotions.
// Using the policy will permit the program to build and run in release
// mode which is identical to that in debug mode except for the fact
// that errors aren't trapped.

#include <limits>

#include "safe_common.hpp"
#include "interval.hpp"
//#include "safe_base.hpp"

namespace boost {
namespace numeric {

// forward declaration - safe type
template<
    class Stored,
    Stored Min,
    Stored Max,
    class P, // promotion polic
    class E  // exception policy
>
class safe_base;

struct automatic {
    template<typename T, typename U, typename P, typename E>
    struct addition_result {
        typedef typename base_type<T>::type t_base_type;
        typedef typename base_type<U>::type u_base_type;
        SAFE_NUMERIC_CONSTEXPR static const interval<t_base_type> t = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };
        SAFE_NUMERIC_CONSTEXPR static const interval<u_base_type> u = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };
        typedef decltype(t_base_type() + u_base_type()) r_base_type;

        SAFE_NUMERIC_CONSTEXPR static const interval<r_base_type> r
            = operator+<r_base_type>(t, u);

        typedef safe_base<r_base_type, r.l, r.u, P, E> type;
    };
    template<typename T, typename U, typename P, typename E>
    struct subtraction_result {
        typedef typename base_type<T>::type t_base_type;
        typedef typename base_type<U>::type u_base_type;
        SAFE_NUMERIC_CONSTEXPR static const interval<t_base_type> t = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };
        SAFE_NUMERIC_CONSTEXPR static const interval<u_base_type> u = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };
        typedef decltype(t_base_type() - u_base_type()) r_base_type;

        SAFE_NUMERIC_CONSTEXPR static const interval<r_base_type> r
            = operator-<r_base_type>(t, u);

        typedef safe_base<r_base_type, r.l, r.u, P, E> type;
    };
    template<typename T, typename U, typename P, typename E>
    struct multiplication_result {
        typedef typename base_type<T>::type t_base_type;
        typedef typename base_type<U>::type u_base_type;
        SAFE_NUMERIC_CONSTEXPR static const interval<t_base_type> t = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };
        SAFE_NUMERIC_CONSTEXPR static const interval<u_base_type> u = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };
        typedef decltype(t_base_type() * u_base_type()) r_base_type;

        SAFE_NUMERIC_CONSTEXPR static const interval<r_base_type> r
            = operator*<r_base_type>(t, u);

        typedef safe_base<r_base_type, r.l, r.u, P, E> type;
    };
    template<typename T, typename U, typename P, typename E>
    struct division_result {
        typedef typename base_type<T>::type t_base_type;
        typedef typename base_type<U>::type u_base_type;
        SAFE_NUMERIC_CONSTEXPR static const interval<t_base_type> t = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };
        SAFE_NUMERIC_CONSTEXPR static const interval<u_base_type> u = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };
        typedef decltype(t_base_type() / u_base_type()) r_base_type;

        SAFE_NUMERIC_CONSTEXPR static const interval<r_base_type> r
            = operator/<r_base_type>(t, u);

        typedef safe_base<r_base_type, r.l, r.u, P, E> type;
    };
    template<typename T, typename U, typename P, typename E>
    struct modulus_result {
        typedef typename base_type<T>::type t_base_type;
        typedef typename base_type<U>::type u_base_type;
        SAFE_NUMERIC_CONSTEXPR static const interval<t_base_type> t = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };
        SAFE_NUMERIC_CONSTEXPR static const interval<u_base_type> u = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };
        typedef decltype(t_base_type() % u_base_type()) r_base_type;

        SAFE_NUMERIC_CONSTEXPR static const interval<r_base_type> r
            = operator%<r_base_type>(t, u);

        typedef safe_base<r_base_type, r.l, r.u, P, E> type;
    };
    template<typename T, typename U, typename P, typename E>    
    struct left_shift_result {
        typedef typename base_type<T>::type t_base_type;
        typedef typename base_type<U>::type u_base_type;
        SAFE_NUMERIC_CONSTEXPR static const interval<t_base_type> t = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };
        SAFE_NUMERIC_CONSTEXPR static const interval<u_base_type> u = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };
        typedef decltype(t_base_type() << u_base_type()) r_base_type;

        SAFE_NUMERIC_CONSTEXPR static const interval<r_base_type> r
            = operator<<<r_base_type>(t, u);

        typedef safe_base<r_base_type, r.l, r.u, P, E> type;
    };
    template<typename T, typename U, typename P, typename E>    
    struct right_shift_result {
        typedef typename base_type<T>::type t_base_type;
        typedef typename base_type<U>::type u_base_type;
        SAFE_NUMERIC_CONSTEXPR static const interval<t_base_type> t = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };
        SAFE_NUMERIC_CONSTEXPR static const interval<u_base_type> u = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };
        typedef decltype(t_base_type() >> u_base_type()) r_base_type;

        SAFE_NUMERIC_CONSTEXPR static const interval<r_base_type> r
            = operator>><r_base_type>(t, u);

        typedef safe_base<r_base_type, r.l, r.u, P, E> type;
    };
};

} // numeric
} // boost

#endif // BOOST_NUMERIC_AUTOMATIC_HPP
