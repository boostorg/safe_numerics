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

// policy which creates expanded results types designed
// to avoid overflows.

#include <limits>
#include <cstdint>     // (u)intmax_t,
#include <type_traits> // true_type, false_type, is_same
#include <boost/mpl/if.hpp>
#include <boost/mpl/min_max.hpp>

#include "safe_common.hpp"
#include "checked_result.hpp"
#include "interval.hpp"

namespace boost {
namespace numeric {

struct automatic {
    // the following returns the "true" type.  After calculating the new max and min
    // these return the minimum size type which can hold the expected result.
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
        typename boost::mpl::if_c<
            std::numeric_limits<T>::is_signed,
            defer_stored_signed_lazily<T, Min, Max>,
            defer_stored_unsigned_lazily<T, Min, Max>
        >::type;


    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct addition_result {
        using temp_base_type = typename boost::mpl::if_c<
            // if both arguments are unsigned
            ! std::numeric_limits<T>::is_signed
            && ! std::numeric_limits<U>::is_signed,
            // result is unsigned
            std::uintmax_t,
            // otherwise result is signed
            std::intmax_t
        >::type;

        constexpr static const interval<typename base_type<T>::type> t_interval{
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };

        constexpr static const interval<typename base_type<U>::type> u_interval{
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

        constexpr static const interval<checked_result<temp_base_type>>
        r_interval = add<temp_base_type>(t_interval, u_interval);

        using type = typename result_type<
            temp_base_type,
            r_interval.l.exception()
                ? std::numeric_limits<temp_base_type>::min()
                : static_cast<temp_base_type>(r_interval.l),
            r_interval.u.exception()
                ? std::numeric_limits<temp_base_type>::max()
                : static_cast<temp_base_type>(r_interval.u)
        >::type;
    };

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct subtraction_result {
        // result of subtraction are always signed.
        using temp_base_type = intmax_t;

        constexpr static const interval<typename base_type<T>::type> t_interval{
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };

        constexpr static const interval<typename base_type<U>::type> u_interval{
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };
        constexpr static const interval<checked_result<temp_base_type>>
        r_interval = subtract<temp_base_type>(t_interval, u_interval);

        using type = typename result_type<
            temp_base_type,
            r_interval.l.exception()
                ? std::numeric_limits<temp_base_type>::min()
                : static_cast<temp_base_type>(r_interval.l),
            r_interval.u.exception()
                ? std::numeric_limits<temp_base_type>::max()
                : static_cast<temp_base_type>(r_interval.u)
        >::type;
    };

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct multiplication_result {
        using temp_base_type = typename boost::mpl::if_c<
            // if both arguments are unsigned
            ! std::numeric_limits<T>::is_signed
            && ! std::numeric_limits<U>::is_signed,
            // result is unsigned
            std::uintmax_t,
            // otherwise result is signed
            std::intmax_t
        >::type;

        constexpr static const interval<typename base_type<T>::type> t_interval{
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };

        constexpr static const interval<typename base_type<U>::type> u_interval{
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

        constexpr static const interval<checked_result<temp_base_type>>
        r_interval = multiply<temp_base_type>(t_interval, u_interval);

        using type = typename result_type<
            temp_base_type,
            r_interval.l.exception()
                ? std::numeric_limits<temp_base_type>::min()
                : static_cast<temp_base_type>(r_interval.l),
            r_interval.u.exception()
                ? std::numeric_limits<temp_base_type>::max()
                : static_cast<temp_base_type>(r_interval.u)
        >::type;
    };

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct division_result {
        using temp_base_type = typename boost::mpl::if_c<
            // if both arguments are unsigned
            ! std::numeric_limits<T>::is_signed
            && ! std::numeric_limits<U>::is_signed,
            // result is unsigned
            std::uintmax_t,
            // otherwise result is signed
            std::intmax_t
        >::type;

        constexpr static const interval<typename base_type<T>::type> t_interval{
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };

        constexpr static const interval<typename base_type<U>::type> u_interval{
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

        constexpr static const interval<checked_result<temp_base_type>>
        r_interval = divide<temp_base_type>(t_interval, u_interval);

        constexpr static temp_base_type lower_helper =
            r_interval.l.exception()
            ? std::numeric_limits<temp_base_type>::min()
            : static_cast<temp_base_type>(r_interval.l);

        constexpr static temp_base_type lower =
            lower_helper < 0 &&
            lower_helper != std::numeric_limits<temp_base_type>::min()
            ? lower_helper - 1
            : lower_helper;

        constexpr static temp_base_type upper =
            r_interval.u.exception()
            ? std::numeric_limits<temp_base_type>::max()
            : static_cast<temp_base_type>(r_interval.u);

        using type = typename result_type<
            temp_base_type,
            lower,
            upper
        >::type;
    };

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct modulus_result {
        using temp_base_type = typename boost::mpl::if_c<
            // if both arguments are unsigned
            ! std::numeric_limits<T>::is_signed
            && ! std::numeric_limits<U>::is_signed,
            // result is unsigned
            std::uintmax_t,
            // otherwise result is signed
            std::intmax_t
        >::type;

        constexpr static const interval<typename base_type<T>::type> t_interval{
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };

        constexpr static const interval<typename base_type<U>::type> u_interval{
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

        constexpr static const interval<checked_result<temp_base_type>>
        r_interval = modulus<temp_base_type>(t_interval, u_interval);

        using type = typename result_type<
            temp_base_type,
            r_interval.l.exception()
                ? std::numeric_limits<temp_base_type>::min()
                : static_cast<temp_base_type>(r_interval.l),
            r_interval.u.exception()
                ? std::numeric_limits<temp_base_type>::max()
                : static_cast<temp_base_type>(r_interval.u)
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
    // shift operations
    template<typename T, typename U>
    struct left_shift_result {
        using t_base_type = typename base_type<T>::type;
        using u_base_type = typename base_type<U>::type;
        constexpr static const interval<t_base_type> t_interval{
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };

        using temp_base_type = typename boost::mpl::if_c<
            std::numeric_limits<T>::is_signed,
            std::intmax_t,
            std::uintmax_t
        >::type;

        constexpr static const interval<u_base_type> u_interval{
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

        constexpr static const interval<checked_result<temp_base_type>> r_interval =
            left_shift<temp_base_type>(t_interval, u_interval);

        using type = typename result_type<
            temp_base_type,
            r_interval.l.exception()
                ? std::numeric_limits<temp_base_type>::min()
                : static_cast<temp_base_type>(r_interval.l),
            r_interval.u.exception()
                ? std::numeric_limits<temp_base_type>::max()
                : static_cast<temp_base_type>(r_interval.u)
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

        using temp_base_type = typename boost::mpl::if_c<
            std::numeric_limits<T>::is_signed,
            std::intmax_t,
            std::uintmax_t
        >::type;

        constexpr static const interval<u_base_type> u_interval{
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

        constexpr static const interval<checked_result<temp_base_type>> r_interval =
            right_shift<temp_base_type>(t_interval, u_interval);

        using type = typename result_type<
            temp_base_type,
            r_interval.l.exception()
                ? std::numeric_limits<temp_base_type>::min()
                : static_cast<temp_base_type>(r_interval.l),
            r_interval.u.exception()
                ? std::numeric_limits<temp_base_type>::max()
                : static_cast<temp_base_type>(r_interval.u)
        >::type;
    };

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct bitwise_and_result {
        using t_base_type = typename base_type<T>::type;
        using u_base_type = typename base_type<U>::type;
        using type = typename boost::mpl::if_c<
            (sizeof(t_base_type) > sizeof(u_base_type)),
            u_base_type,
            t_base_type
        >::type;
    };
    template<typename T, typename U>
    struct bitwise_or_result {
        using t_base_type = typename base_type<T>::type;
        using u_base_type = typename base_type<U>::type;
        using type = typename boost::mpl::if_c<
            (sizeof(t_base_type) > sizeof(u_base_type)),
            t_base_type,
            u_base_type
        >::type;
    };
    template<typename T, typename U>
    struct bitwise_xor_result {
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
