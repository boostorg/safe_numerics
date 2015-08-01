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
#include <cstdint>     // (u)intmax_t,
#include <type_traits> // true_type, false_type
#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>

#include "safe_common.hpp"
#include "interval.hpp"
#include "safe_range.hpp"

namespace boost {
namespace numeric {

struct automatic {

    template<class T, class U>
    using calculate_max_t2 =
        typename boost::mpl::if_c<
            std::numeric_limits<T>::is_signed
            || std::numeric_limits<U>::is_signed,
            std::intmax_t,
            std::uintmax_t
        >::type;

    // section 4.13 integer conversion rank
    template<class T>
    using rank =
        typename boost::mpl::if_c<
            sizeof(char) == sizeof(T),
            std::integral_constant<int, 1>,
        typename boost::mpl::if_c<
            sizeof(short) == sizeof(T),
            std::integral_constant<int, 2>,
        typename boost::mpl::if_c<
            sizeof(int) == sizeof(T),
            std::integral_constant<int, 3>,
        typename boost::mpl::if_c<
            sizeof(long) == sizeof(T),
            std::integral_constant<int, 4>,
        typename boost::mpl::if_c<
            sizeof(long long) == sizeof(T),
            std::integral_constant<int, 5>,
            void
        >::type >::type >::type >::type >::type;

    // note presumption that T & U don't have he same sign
    // if that's not true, these won't work
    template<class T, class U>
    using select_signed = typename boost::mpl::if_c<
        std::numeric_limits<T>::is_signed,
        T,
        U
    >::type;

    template<class T, class U>
    using select_unsigned = typename boost::mpl::if_c<
        std::numeric_limits<T>::is_signed,
        U,
        T
    >::type;

    template<class T, class U>
    using calculate_max_t =
        typename boost::mpl::if_c<
            // clause 1 - if both operands have the same sign
            std::numeric_limits<T>::is_signed
            == std::numeric_limits<U>::is_signed,
            // use that sign
            typename boost::mpl::if_c<
                std::numeric_limits<T>::is_signed,
                std::intmax_t,
                std::uintmax_t
            >::type,
        // clause 2 - otherwise if the rank of he unsigned type exceeds
        // the rank of the of the maximum signed type
        typename boost::mpl::if_c<
            (rank< select_unsigned<T, U>>::value
            > rank< std::intmax_t >::value),
            // use unsigned type
            std::uintmax_t,
        // clause 3 - otherwise if the type of the signed integer type can
        // represent all the values of the unsigned type
        typename boost::mpl::if_c<
            std::numeric_limits< std::intmax_t >::digits >=
            std::numeric_limits< select_unsigned<T, U> >::digits,
            // use signed type
            std::intmax_t,
        // clause 4 - otherwise use unsigned version of the signed type
            std::uintmax_t
        >::type >::type >::type;

    template<typename T, T Min, T Max, class P, class E>
    struct defer_signed_lazily {
        using type = boost::numeric::safe_signed_range<Min, Max, P, E>;
    };

    template<typename T, T Min, T Max, class P, class E>
    struct defer_unsigned_lazily {
        using type = boost::numeric::safe_unsigned_range<Min, Max, P, E>;
    };

    template<typename T, T Min, T Max, class P, class E>
    using safe_range =
        typename boost::mpl::if_<
            std::is_signed<T>,
            defer_signed_lazily<T, Min, Max, P, E>,
            defer_unsigned_lazily<T, Min, Max, P, E>
        >::type;

    template<typename T, typename U, typename P, typename E>
    struct addition_result {
        typedef typename base_type<T>::type base_type_t;
        typedef typename base_type<U>::type base_type_u;

        SAFE_NUMERIC_CONSTEXPR static const interval<base_type_t> t = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };

        SAFE_NUMERIC_CONSTEXPR static const interval<base_type_u> u = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

        typedef  calculate_max_t<T, U> max_t;

        SAFE_NUMERIC_CONSTEXPR static const interval< max_t> r
            = operator+<max_t>(t, u);

        SAFE_NUMERIC_CONSTEXPR static const max_t newmin = r.l.is_valid() ?
            static_cast<max_t>(r.l)
        :
            std::numeric_limits<max_t>::min()
        ;

        SAFE_NUMERIC_CONSTEXPR static const max_t newmax = r.u.is_valid() ?
            static_cast<max_t>(r.u)
        :
            std::numeric_limits<max_t>::max()
        ;

        //typedef typename print<max_t>::type p_max_t;
        //typedef typename print<std::integral_constant<max_t, newmin>>::type p_newmin;
        //typedef typename print<std::integral_constant<max_t, newmax>>::type p_newmax;

        static_assert(
            newmin < newmax,
            "new minimumum must be less than new maximum"
        );

        static_assert(
            std::numeric_limits<max_t>::min() >= 0 || std::is_signed<max_t>::value,
            "newmin < 0 and unsigned can't happen"
        );

        typedef typename safe_range<
            max_t,
            newmin,
            newmax,
            P,
            E
        >::type type;

        /*
        typedef typename print<
            typename safe_range<
                max_t,
                newmin,
                newmax,
                P,
                E
            >::type
        >::type p_safe_range;
        */
    };
    template<typename T, typename U, typename P, typename E>
    struct subtraction_result {
        typedef typename base_type<T>::type base_type_t;
        typedef typename base_type<U>::type base_type_u;

        SAFE_NUMERIC_CONSTEXPR static const interval<base_type_t> t = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };

        SAFE_NUMERIC_CONSTEXPR static const interval<base_type_u> u = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

        typedef calculate_max_t<T, U> max_t;

        SAFE_NUMERIC_CONSTEXPR static const interval< max_t> r
            = operator-<max_t>(t, u);

        SAFE_NUMERIC_CONSTEXPR static const max_t newmin = r.l.is_valid() ?
            static_cast<max_t>(r.l)
        :
            std::numeric_limits<max_t>::min()
        ;

        SAFE_NUMERIC_CONSTEXPR static const max_t newmax = r.u.is_valid() ?
            static_cast<max_t>(r.u)
        :
            std::numeric_limits<max_t>::max()
        ;

        // typedef typename print<max_t>::type p_max_t;
        // typedef typename print<std::integral_constant<max_t, newmin>>::type p_newmin;
        // typedef typename print<std::integral_constant<max_t, newmax>>::type p_newmax;

        static_assert(
            newmin < newmax,
            "new minimumum must be less than new maximum"
        );

        static_assert(
            std::numeric_limits<max_t>::min() >= 0 || std::is_signed<max_t>::value,
            "newmin < 0 and unsigned can't happen"
        );

        typedef typename safe_range<
            max_t,
            newmin,
            newmax,
            P,
            E
        >::type type;
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
