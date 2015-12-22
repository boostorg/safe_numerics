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
#include <type_traits> // true_type, false_type, is_same
#include <boost/mpl/if.hpp>

#include "utility.hpp"
#include "safe_common.hpp"
#include "checked_result.hpp"
#include "interval.hpp"

namespace boost {
namespace numeric {

struct automatic {
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
        // clause 2 - otherwise if the rank of the unsigned type exceeds
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

    template<typename T, T Min, T Max>
    struct defer_stored_signed_lazily {
        using type = signed_stored_type<Min, Max>;
    };

    template<typename T, T Min, T Max>
    struct defer_stored_unsigned_lazily {
        using type = unsigned_stored_type<Min, Max>;
    };

    template<typename T, T Min, T Max>
    using result_type =
        typename boost::mpl::if_<
            std::is_signed<T>,
            defer_stored_signed_lazily<T, Min, Max>,
            defer_stored_unsigned_lazily<T, Min, Max>
        >::type;

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct addition_result {

        using result_base_type = calculate_max_t<T, U>;
        using t_base_type = typename base_type<T>::type;
        using u_base_type = typename base_type<U>::type;

        // filter out case were overflow cannot occur
        // note: subtle trickery.  Suppose t is safe_range<MIN, ..>.  Then
        // std::numeric_limits<T>::min() will be safe_range<MIN with a value of MIN
        // Use base_value(T) ( which equals MIN ) to create a new interval. Same
        // for MAX.  Now
        constexpr static const interval<t_base_type> t_interval{
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };

        constexpr static const interval<u_base_type> u_interval{
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

        // when we add the temporary intervals above, we'll get a new interval
        // with the correct range for the sum !
        constexpr static const checked_result<interval<result_base_type>> r_interval
            = add<result_base_type>(t_interval, u_interval);

        constexpr static const interval<result_base_type> result_interval =
            r_interval.no_exception() ?
                static_cast<interval<result_base_type>>(r_interval)
            :
                interval<result_base_type>{}
            ;

        using type = typename result_type<
            result_base_type,
            result_interval.l,
            result_interval.u
        >::type;
    };

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct subtraction_result {
        using result_base_type = calculate_max_t<T, U>;
        using t_base_type = typename base_type<T>::type;
        using u_base_type = typename base_type<U>::type;

        constexpr static const interval<t_base_type> t_interval{
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };

        constexpr static const interval<u_base_type> u_interval{
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

        constexpr static const checked_result<interval<result_base_type>> r_interval
            = subtract<result_base_type>(t_interval, u_interval);

        constexpr static const interval<result_base_type> result_interval =
            r_interval.no_exception() ?
                static_cast<interval<result_base_type>>(r_interval)
            :
                interval<result_base_type>{}
            ;

        using type = typename result_type<
            result_base_type,
            result_interval.l,
            result_interval.u
        >::type;
    };

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct multiplication_result {
        using result_base_type = calculate_max_t<T, U>;
        using t_base_type = typename base_type<T>::type;
        using u_base_type = typename base_type<U>::type;

        constexpr static const interval<t_base_type> t_interval{
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };

        constexpr static const interval<u_base_type> u_interval{
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

        constexpr static const checked_result<interval<result_base_type>> r_interval
            = multiply<result_base_type>(t_interval, u_interval);

        constexpr static const interval<result_base_type> result_interval =
            r_interval.no_exception() ?
                static_cast<interval<result_base_type>>(r_interval)
            :
                interval<result_base_type>{}
            ;

        using type = typename result_type<
            result_base_type,
            result_interval.l,
            result_interval.u
        >::type;
    };

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct division_result {
        using t_base_type = typename base_type<T>::type;
        using u_base_type = typename base_type<U>::type;

        constexpr static const interval<t_base_type> t_interval{
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };

        constexpr static const interval<u_base_type> u_interval{
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

        using result_base_type = typename boost::mpl::if_c<
            std::numeric_limits<t_base_type>::is_signed
            || std::numeric_limits<u_base_type>::is_signed,
            std::intmax_t,
            std::uintmax_t
        >::type;

        constexpr static checked_result<interval<result_base_type>> r {
            divide_nz<result_base_type>(t_interval, u_interval)
        };

        constexpr static const interval<result_base_type> result_interval {
            r.no_exception() ?
                static_cast<interval<result_base_type>>(r)
            :
                interval<result_base_type>{}
         };

        using type = typename result_type<
            result_base_type,
            result_interval.l,
            result_interval.u
        >::type;
    };

    // forward to correct divide implementation
    template<class R, class T, class U>
    checked_result<R>
    static constexpr divide(
        const T & t,
        const U & u
    ){
        return checked::divide_automatic<R>(t, u);
    }

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct modulus_result {
        using t_base_type = typename base_type<T>::type;
        using u_base_type = typename base_type<U>::type;

        constexpr static const interval<t_base_type> t_interval{
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };

        constexpr static const interval<u_base_type> u_interval{
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

        using r_base_type = std::make_unsigned_t<u_base_type>;

        constexpr static const checked_result<interval<r_base_type>> r
            { modulus_nz<r_base_type>(t_interval, u_interval) };

        constexpr static const interval<r_base_type> result_interval =
            r.no_exception() ?
                static_cast<interval<r_base_type>>(r)
            :
                interval<r_base_type>{}
            ;

        using type = typename result_type<
            r_base_type,
            result_interval.l,
            result_interval.u
        >::type;
    };

    // forward to correct modulus implementation
    template<class R, class T, class U>
    checked_result<R>
    static constexpr modulus(
        const T & t,
        const U & u
    ){
        return checked::modulus<R>(t, u);
    }

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct left_shift_result {
        using t_base_type = typename base_type<T>::type;
        using u_base_type = typename base_type<U>::type;

        constexpr static const interval<t_base_type> t_interval{
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };

        constexpr static const interval<u_base_type> u_interval{
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

        typedef calculate_max_t<T, U> max_t;

        constexpr static const checked_result<interval< max_t>> r
            = left_shift<max_t>(t_interval, u_interval);

        constexpr static const interval<max_t> result_interval =
            r.no_exception() ?
                static_cast<interval<max_t>>(r)
            :
                interval< max_t>{}
            ;

        using type = typename result_type<
            max_t,
            result_interval.l,
            result_interval.u
        >::type;
    };

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct right_shift_result {
        using t_base_type = typename base_type<T>::type;
        using u_base_type = typename base_type<U>::type;

        constexpr static const interval<t_base_type> t_interval{
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };

        constexpr static const interval<u_base_type> u_interval{
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

        typedef calculate_max_t<T, U> max_t;

        constexpr static const checked_result<interval< max_t>> r
            = right_shift<max_t>(t_interval, u_interval);

        constexpr static const interval<max_t> result_interval =
            r.no_exception() ?
                static_cast<interval<max_t>>(r)
            :
                interval< max_t>{}
            ;

        using type = typename result_type<
            max_t,
            result_interval.l,
            result_interval.u
        >::type;
    };

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct bitwise_result {
        using t_base_type = typename base_type<T>::type;
        using u_base_type = typename base_type<U>::type;
        using type = typename boost::mpl::if_c<
            (sizeof(t_base_type) > sizeof(u_base_type)),
            t_base_type,
            u_base_type
        >::type;
    };
};

} // numeric
} // boost

#endif // BOOST_NUMERIC_AUTOMATIC_HPP
