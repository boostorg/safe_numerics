#ifndef BOOST_NUMERIC_CHECKED_HPP
#define BOOST_NUMERIC_CHECKED_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// contains operations for doing checked aritmetic on NATIVE
// C++ types.

#include <limits>

#include <boost/utility/enable_if.hpp>
#include "safe_base.hpp"
#include "checked_result.hpp"

namespace boost {
namespace numeric {
namespace checked {
    ////////////////////////////////////////////////////
    // layer 0 - detect overflows / alteration at the
    // atomic operation level taking care to work around
    // otherwise undetect alterations in integers due
    // to machine architecture.  Note presumption of twos
    // complement integer arithmetic

    namespace detail {

    template<class R, class T>
    SAFE_NUMERIC_CONSTEXPR checked_result<R>
    cast(
        const T & t
    ){
        return
        std::numeric_limits<R>::is_signed ?
            // T is signed
            std::numeric_limits<T>::is_signed ?
                t > std::numeric_limits<R>::max() ?
                    checked_result<R>(
                        checked_result<R>::exception_type::range_error,
                        "converted signed value too large"
                    )
                :
                t < std::numeric_limits<R>::min() ?
                    checked_result<R>(
                        checked_result<R>::exception_type::range_error,
                        "converted signed value too small"
                    )
                :
                    checked_result<R>(t)
            : // T is unsigned
                t > std::numeric_limits<R>::max() ?
                    checked_result<R>(
                        checked_result<R>::exception_type::range_error,
                        "converted unsigned value too large"
                    )
                :
                    checked_result<R>(t)
        : // std::numeric_limits<R>::is_signed
            // T is signed
            ! std::numeric_limits<T>::is_signed ?
                t > std::numeric_limits<R>::max() ?
                    checked_result<R>(
                        checked_result<R>::exception_type::range_error,
                        "converted unsigned value too large"
                    )
                :
                    checked_result<R>(t)
            : // T is signed
                t < 0 ?
                    checked_result<R>(
                        checked_result<R>::exception_type::range_error,
                        "converted negative value to unsigned"
                    )
                :
                t > std::numeric_limits<R>::max() ?
                    checked_result<R>(
                        checked_result<R>::exception_type::range_error,
                        "converted signed value too large"
                    )
                :
                    checked_result<R>(t)
        ;
    }

    ////////////////////////////////////////////////////
    // safe addition on primitive types

    // result unsigned
    template<class R>
    typename boost::enable_if<
        typename std::is_unsigned<R>,
        checked_result<R>
    >::type
    SAFE_NUMERIC_CONSTEXPR add(
        const R & minr,
        const R & maxr,
        const R t,
        const R u
    ) {
        return
            maxr - u < t ?
                checked_result<R>(
                    checked_result<R>::exception_type::overflow_error,
                    "addition overflow"
                )
            :
                checked_result<R>(t + u)
        ;
    }

    // result signed
    template<class R>
    typename boost::enable_if<
        typename std::is_signed<R>,
        checked_result<R>
    >::type
    SAFE_NUMERIC_CONSTEXPR add(
        const R & minr,
        const R & maxr,
        const R t,
        const R u
    ) {
        return
            ((u > 0) && (t > (maxr - u)))
            || ((u < 0) && (t < (minr - u))) ?
                checked_result<R>(
                    checked_result<R>::exception_type::overflow_error,
                    "addition overflow"
                )
            :
                checked_result<R>(t + u)
        ;
    }

    } // namespace detail

    template<class R, class T, class U>
    SAFE_NUMERIC_CONSTEXPR checked_result<R> add(
        const R & minr,
        const R & maxr,
        const T & t,
        const U & u
    ) {
        return
            detail::cast<R>(t) != checked_result<R>::exception_type::no_exception ?
                detail::cast<R>(t)
            :
            detail::cast<R>(u) != checked_result<R>::exception_type::no_exception ?
                detail::cast<R>(u)
            :
                detail::add<R>(minr, maxr, t, u)
        ;
    }

    template<class R, class T, class U>
    SAFE_NUMERIC_CONSTEXPR checked_result<R> add(
        const T & t,
        const U & u
    ) {
        return
            add<R, T, U>(
                std::numeric_limits<R>::min(),
                std::numeric_limits<R>::max(),
                t,
                u
            )
        ;
    }

    ////////////////////////////////////////////////////
    // safe subtraction on primitive types
    namespace detail {

    // result unsigned
    template<class R>
    typename boost::enable_if<
        typename std::is_unsigned<R>,
        checked_result<R>
    >::type
    SAFE_NUMERIC_CONSTEXPR subtract(
        const R & minr,
        const R & maxr,
        const R t,
        const R u
    ) {
        // INT30-C
        return
            t < u ?
                checked_result<R>(
                    checked_result<R>::exception_type::overflow_error,
                    "subtraction overflow"
                )
            :
                checked_result<R>(t - u)
        ;
    }

    // result signed
    template<class R>
    typename boost::enable_if<
        typename std::is_signed<R>,
        checked_result<R>
    >::type
    SAFE_NUMERIC_CONSTEXPR subtract(
        const R & minr,
        const R & maxr,
        const R t,
        const R u
    ) { // INT32-C
        return
            ((u > 0) && (t < (minr + u)))
            || ((u < 0) && (t > (maxr + u))) ?
                checked_result<R>(
                    checked_result<R>::exception_type::overflow_error,
                    "subtraction overflow"
                )
            :
                checked_result<R>(t - u)
        ;
    }

    } // namespace detail

    template<class R, class T, class U>
    SAFE_NUMERIC_CONSTEXPR checked_result<R> subtract(
        const R & minr,
        const R & maxr,
        const T & t,
        const U & u
    ) {
        return
            detail::cast<R>(t) != checked_result<R>::exception_type::no_exception ?
                detail::cast<R>(t)
            :
            detail::cast<R>(u) != checked_result<R>::exception_type::no_exception ?
                detail::cast<R>(u)
            :
                detail::subtract<R>(minr, maxr, t, u)
        ;
    }
    
    template<class R, class T, class U>
    SAFE_NUMERIC_CONSTEXPR checked_result<R> subtract(
        const T & t,
        const U & u
    ) {
        return
            subtract<R, T, U>(
                std::numeric_limits<R>::min(),
                std::numeric_limits<R>::max(),
                t,
                u
            )
        ;
    }

    ////////////////////////////////////////////////////
    // safe multiplication on unsafe types
    namespace detail {

    // result unsigned
    template<class R>
    typename boost::enable_if_c<
        std::is_unsigned<R>::value && (sizeof(R) <= (sizeof(std::uintmax_t) / 2)),
        checked_result<R>
    >::type
    SAFE_NUMERIC_CONSTEXPR multiply(
        const R & minr,
        const R & maxr,
        const R t,
        const R u
    ) {
        // INT30-C
        // fast method using intermediate result guaranteed not to overflow
        // todo - replace std::uintmax_t with a size double the size of R
        typedef std::uintmax_t i_type;
        return
            static_cast<i_type>(t) * static_cast<i_type>(u)
            > std::numeric_limits<R>::max() ?
                checked_result<R>(
                    checked_result<R>::exception_type::overflow_error,
                    "multiplication overflow"
                )
            :
                checked_result<R>(t * u)
        ;
    }
    template<class R>
    typename boost::enable_if_c<
        std::is_unsigned<R>::value && (sizeof(R) > sizeof(std::uintmax_t) / 2),
        checked_result<R>
    >::type
    SAFE_NUMERIC_CONSTEXPR multiply(
        const R & minr,
        const R & maxr,
        const R t,
        const R u
    ) {
        // INT30-C
        return
            u > 0 && t > std::numeric_limits<R>::max() / u ?
                checked_result<R>(
                    checked_result<R>::exception_type::overflow_error,
                    "multiplication overflow"
                )
            :
                checked_result<R>(t * u)
        ;
    }

    // result signed
    template<class R>
    typename boost::enable_if_c<
        std::is_signed<R>::value && (sizeof(R) <= (sizeof(std::intmax_t) / 2)),
        checked_result<R>
    >::type
    SAFE_NUMERIC_CONSTEXPR multiply(
        const R & minr,
        const R & maxr,
        const R t,
        const R u
    ) {
        // INT30-C
        // fast method using intermediate result guaranteed not to overflow
        // todo - replace std::uintmax_t with a size double the size of R
        typedef std::intmax_t i_type;
        return
            (
                static_cast<i_type>(t) * static_cast<i_type>(u)
                > static_cast<i_type>(std::numeric_limits<R>::max())
            ) ?
                checked_result<R>(
                    checked_result<R>::exception_type::overflow_error,
                    "multiplication overflow"
                )
            :
            (
                static_cast<i_type>(t) * static_cast<i_type>(u)
                < static_cast<i_type>(std::numeric_limits<R>::min())
            ) ?
                checked_result<R>(
                    checked_result<R>::exception_type::underflow_error,
                    "multiplication underflow"
                )
            :
                checked_result<R>(t * u)
        ;
    }
    template<class R>
    typename boost::enable_if_c<
        std::is_signed<R>::value && (sizeof(R) > (sizeof(std::intmax_t) / 2)),
        checked_result<R>
    >::type
    SAFE_NUMERIC_CONSTEXPR multiply(
        const R & minr,
        const R & maxr,
        const R t,
        const R u
    ) { // INT32-C
        return t > 0 ?
            u > 0 ?
                t > std::numeric_limits<R>::max() / u ?
                    checked_result<R>(
                        checked_result<R>::exception_type::overflow_error,
                        "multiplication overflow"
                    )
                :
                    checked_result<R>(t * u)
            : // u <= 0
                u < std::numeric_limits<R>::min() / t ?
                    checked_result<R>(
                        checked_result<R>::exception_type::overflow_error,
                        "multiplication overflow"
                    )
                :
                    checked_result<R>(t * u)
        : // t <= 0
            u > 0 ?
                t < std::numeric_limits<R>::min() / u ?
                    checked_result<R>(
                        checked_result<R>::exception_type::overflow_error,
                        "multiplication overflow"
                    )
                :
                    checked_result<R>(t * u)
            : // u <= 0
                t != 0 && u < std::numeric_limits<R>::max() / t ?
                    checked_result<R>(
                        checked_result<R>::exception_type::overflow_error,
                        "multiplication overflow"
                    )
                :
                    checked_result<R>(t * u)
        ;
    }

    } // namespace detail

    template<class R, class T, class U>
    SAFE_NUMERIC_CONSTEXPR checked_result<R> multiply(
        const R & minr,
        const R & maxr,
        const T & t,
        const U & u
    ) {
        static_assert(! is_safe<T>::value, "should not be a base type here!");
        return
            detail::cast<R>(t) != checked_result<R>::exception_type::no_exception ?
                detail::cast<R>(t)
            :
            detail::cast<R>(u) != checked_result<R>::exception_type::no_exception ?
                detail::cast<R>(u)
            :
            sizeof(R) >= sizeof(T) + sizeof(U) ?
                checked_result<R>(static_cast<R>(t) * static_cast<R>(u))
            :
                detail::multiply<R>(minr, maxr, t, u)
        ;
    }
    template<class R, class T, class U>
    SAFE_NUMERIC_CONSTEXPR checked_result<R> multiply(
        const T & t,
        const U & u
    ) {
        return
            multiply<R, T, U>(
                std::numeric_limits<R>::min(),
                std::numeric_limits<R>::max(),
                t,
                u
            )
        ;
    }

    ////////////////////////////////
    // safe division on unsafe types
    namespace detail {

    template<class R>
    typename boost::enable_if_c<
        std::is_unsigned<R>::value,
        checked_result<R>
    >::type
    SAFE_NUMERIC_CONSTEXPR divide(
        const R & minr,
        const R & maxr,
        const R t,
        const R u
    ) {
        return checked_result<R>(t / u);
    }

    template<class R>
    typename boost::enable_if_c<
        std::is_signed<R>::value,
        checked_result<R>
    >::type
    SAFE_NUMERIC_CONSTEXPR divide(
        const R & minr,
        const R & maxr,
        const R t,
        const R u
    ){
        return
            // note presumption of two's complement arithmetic
            (u < 0 && t == std::numeric_limits<R>::min()) ?
                checked_result<R>(
                    checked_result<R>::exception_type::domain_error,
                    "divide by zero"
                )
            :
                checked_result<R>(t / u)
            ;
    }

    } // namespace detail

    template<class R, class T, class U>
    SAFE_NUMERIC_CONSTEXPR checked_result<R> divide(
        const R & minr,
        const R & maxr,
        const T & t,
        const U & u
    ) {
        static_assert(! is_safe<T>::value, "should not be a base type here!");
        return
            detail::cast<R>(t) != checked_result<R>::exception_type::no_exception ?
                detail::cast<R>(t)
            :
            detail::cast<R>(u) != checked_result<R>::exception_type::no_exception ?
                detail::cast<R>(u)
            :
                u == 0 ?
                    checked_result<R>(
                        checked_result<R>::exception_type::domain_error,
                        "divide by zero"
                    )
                :
                    detail::divide<R>(minr, maxr, t, u)
        ;
    }
    template<class R, class T, class U>
    SAFE_NUMERIC_CONSTEXPR checked_result<R> divide(
        const T & t,
        const U & u
    ) {
        return
            divide<R, T, U>(
                std::numeric_limits<R>::min(),
                std::numeric_limits<R>::max(),
                t,
                u
            )
        ;
    }

    ////////////////////////////////
    // safe modulus on unsafe types
    namespace detail {

    template<class R>
    typename boost::enable_if_c<
        std::is_unsigned<R>::value,
        checked_result<R>
    >::type
    SAFE_NUMERIC_CONSTEXPR modulus(
        const R & minr,
        const R & maxr,
        const R t,
        const R u
    ) {
        return checked_result<R>(t % u);
    }

    template<class R>
    typename boost::enable_if_c<
        std::is_signed<R>::value,
        checked_result<R>
    >::type
    SAFE_NUMERIC_CONSTEXPR modulus(
        const R & minr,
        const R & maxr,
        const R t,
        const R u
    ){
        return
            // note presumption of two's complement arithmetic
            (u < 0 && t == std::numeric_limits<R>::min()) ?
                checked_result<R>(
                    checked_result<R>::exception_type::domain_error,
                    "divide by zero"
                )
            :
                checked_result<R>(t % u)
            ;
    }

    } // namespace detail

    template<class R, class T, class U>
    SAFE_NUMERIC_CONSTEXPR checked_result<R> modulus(
        const R & minr,
        const R & maxr,
        const T & t,
        const U & u
    ) {
        static_assert(! is_safe<T>::value, "should not be a base type here!");
        return
            detail::cast<R>(t) != checked_result<R>::exception_type::no_exception ?
                detail::cast<R>(t)
            :
            detail::cast<R>(u) != checked_result<R>::exception_type::no_exception ?
                detail::cast<R>(u)
            :
                u == 0 ?
                    checked_result<R>(
                        checked_result<R>::exception_type::domain_error,
                        "divide by zero"
                    )
                :
                    detail::divide<R>(minr, maxr, t, u)
        ;
    }
    template<class R, class T, class U>
    SAFE_NUMERIC_CONSTEXPR checked_result<R> modulus(
        const T & t,
        const U & u
    ) {
        return
            divide<R, T, U>(
                std::numeric_limits<R>::min(),
                std::numeric_limits<R>::max(),
                t,
                u
            )
        ;
    }

} // checked
} // numeric
} // boost

#endif // BOOST_NUMERIC__HPP
