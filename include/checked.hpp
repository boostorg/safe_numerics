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
#include <type_traits> // is_integral, make_unsigned
#include <algorithm> // std::max

#include <boost/utility/enable_if.hpp>

#include "safe_common.hpp"
#include "checked_result.hpp"

namespace boost {
namespace numeric {
namespace checked {

////////////////////////////////////////////////////
// layer 0 - implement safe operations for intrinsic integers
// Note presumption of twos complement integer arithmetic

////////////////////////////////////////////////////
// safe casting on primitive types

template<class R, class T>
constexpr checked_result<R>
cast(
    const T & t
) {
    // INT31-C requires that integer conversions, both implicit
    // and explicit (using a cast),must be guaranteed not to result
    // in lost or misinterpreted data [Seacord 2008].
    return
    std::numeric_limits<R>::is_signed ?
        // T is signed
        std::numeric_limits<T>::is_signed ?
            // INT32-C Ensure that operations on signed
            // integers do not overflow
            t > std::numeric_limits<R>::max() ?
                checked_result<R>(
                    exception_type::range_error,
                    "converted signed value too large"
                )
            :
            t < std::numeric_limits<R>::min() ?
                checked_result<R>(
                    exception_type::range_error,
                    "converted signed value too small"
                )
            :
                checked_result<R>(static_cast<R>(t))
        : // T is unsigned
            // INT30-C Ensure that unsigned integer operations
            // do not wrap
            t > std::numeric_limits<R>::max() ?
                checked_result<R>(
                    exception_type::range_error,
                    "converted unsigned value too large"
                )
            :
                checked_result<R>(static_cast<R>(t))
    : // std::numeric_limits<R>::is_signed
        // T is signed
        // INT32-C Ensure that operations on signed
        // integers do not overflow
        ! std::numeric_limits<T>::is_signed ?
            t > std::numeric_limits<R>::max() ?
                checked_result<R>(
                    exception_type::range_error,
                    "converted unsigned value too large"
                )
            :
                checked_result<R>(static_cast<R>(t))
        : // T is signed
            t < 0 ?
                checked_result<R>(
                    exception_type::range_error,
                    "converted negative value to unsigned"
                )
            :
            t > std::numeric_limits<R>::max() ?
                checked_result<R>(
                    exception_type::range_error,
                    "converted signed value too large"
                )
            :
                checked_result<R>(static_cast<R>(t))
    ;
}

////////////////////////////////////////////////////
// safe addition on primitive types

namespace detail {

    // result unsigned
    template<class R>
    typename boost::enable_if_c<
        std::is_unsigned<R>::value,
        checked_result<R>
    >::type
    constexpr add(
        const R t,
        const R u
    ) {
        return
            // INT30-C. Ensure that unsigned integer operations do not wrap
            std::numeric_limits<R>::max() - u < t ?
                checked_result<R>(
                    exception_type::overflow_error,
                    "addition overflow"
                )
            :
                checked_result<R>(t + u)
        ;
    }

    // result signed
    template<class R>
    typename boost::enable_if_c<
        std::is_signed<R>::value,
        checked_result<R>
    >::type
    constexpr add(
        const R t,
        const R u
    ) {
        return
            // INT32-C. Ensure that operations on signed integers do not result in overflow
            ((u > 0) && (t > (std::numeric_limits<R>::max() - u)))
            || ((u < 0) && (t < (std::numeric_limits<R>::min() - u))) ?
                checked_result<R>(
                    exception_type::overflow_error,
                    "addition overflow"
                )
            :
                checked_result<R>(t + u)
        ;
    }

} // namespace detail

template<class R, class T, class U>
constexpr checked_result<R> add(
    const T & t,
    const U & u
) {
    static_assert(std::is_integral<T>::value, "only intrinsic integers permitted");
    static_assert(std::is_integral<U>::value, "only intrinsic integers permitted");
    const checked_result<R> ru = cast<R>(u);
    const checked_result<R> rt = cast<R>(t);
    return
        (! rt.no_exception()) ?
            rt
        :
        (! ru.no_exception()) ?
            ru
        :
            detail::add<R>(t, u)
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
constexpr subtract(
    const R t,
    const R u
) {
    // INT30-C. Ensure that unsigned integer operations do not wrap
    return
        t < u ?
            checked_result<R>(
                exception_type::overflow_error,
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
constexpr subtract(
    const R t,
    const R u
) { // INT32-C
    return
        // INT32-C. Ensure that operations on signed integers do not result in overflow
        ((u > 0) && (t < (std::numeric_limits<R>::min() + u)))
        || ((u < 0) && (t > (std::numeric_limits<R>::max() + u))) ?
            checked_result<R>(
                exception_type::overflow_error,
                "subtraction overflow"
            )
        :
            checked_result<R>(t - u)
    ;
}

} // namespace detail

template<class R, class T, class U>
constexpr checked_result<R> subtract(
    const T & t,
    const U & u
) {
    static_assert(std::is_integral<T>::value, "only intrinsic integers permitted");
    static_assert(std::is_integral<U>::value, "only intrinsic integers permitted");
    checked_result<R> rt(cast<R>(t));
    if(! rt.no_exception() )
        return rt;
    checked_result<R> ru(cast<R>(u));
    if(! ru.no_exception() )
        return ru;
    return detail::subtract<R>(t, u);
}

////////////////////////////////////////////////////
// safe multiplication on primitive types

namespace detail {

// result unsigned
template<class R>
typename boost::enable_if_c<
    std::is_unsigned<R>::value && (sizeof(R) <= (sizeof(std::uintmax_t) / 2)),
    checked_result<R>
>::type
constexpr multiply(
    const R t,
    const R u
) {
    // INT30-C
    // fast method using intermediate result guaranteed not to overflow
    // todo - replace std::uintmax_t with a size double the size of R
    using i_type = std::uintmax_t;
    return
        static_cast<i_type>(t) * static_cast<i_type>(u)
        > std::numeric_limits<R>::max() ?
            checked_result<R>(
                exception_type::overflow_error,
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
constexpr multiply(
    const R t,
    const R u
) {
    // INT30-C
    return
        u > 0 && t > std::numeric_limits<R>::max() / u ?
            checked_result<R>(
                exception_type::overflow_error,
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
constexpr multiply(
    const R t,
    const R u
) {
    // INT30-C
    // fast method using intermediate result guaranteed not to overflow
    // todo - replace std::intmax_t with a size double the size of R
    using i_type = std::intmax_t;
    return
        (
            static_cast<i_type>(t) * static_cast<i_type>(u)
            > static_cast<i_type>(std::numeric_limits<R>::max())
        ) ?
            checked_result<R>(
                exception_type::overflow_error,
                "multiplication overflow"
            )
        :
        (
            static_cast<i_type>(t) * static_cast<i_type>(u)
            < static_cast<i_type>(std::numeric_limits<R>::min())
        ) ?
            checked_result<R>(
                exception_type::underflow_error,
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
constexpr multiply(
    const R t,
    const R u
) { // INT32-C
    return t > 0 ?
        u > 0 ?
            t > std::numeric_limits<R>::max() / u ?
                checked_result<R>(
                    exception_type::overflow_error,
                    "multiplication overflow"
                )
            :
                checked_result<R>(t * u)
        : // u <= 0
            u < std::numeric_limits<R>::min() / t ?
                checked_result<R>(
                    exception_type::overflow_error,
                    "multiplication overflow"
                )
            :
                checked_result<R>(t * u)
    : // t <= 0
        u > 0 ?
            t < std::numeric_limits<R>::min() / u ?
                checked_result<R>(
                    exception_type::overflow_error,
                    "multiplication overflow"
                )
            :
                checked_result<R>(t * u)
        : // u <= 0
            t != 0 && u < std::numeric_limits<R>::max() / t ?
                checked_result<R>(
                    exception_type::overflow_error,
                    "multiplication overflow"
                )
            :
                checked_result<R>(t * u)
    ;
}

} // namespace detail

template<class R, class T, class U>
constexpr checked_result<R> multiply(
    const T & t,
    const U & u
) {
    static_assert(std::is_integral<T>::value, "only intrinsic integers permitted");
    static_assert(std::is_integral<U>::value, "only intrinsic integers permitted");
    checked_result<R> rt(cast<R>(t));
    if(! rt.no_exception() )
        return rt;
    checked_result<R> ru(cast<R>(u));
    if(! ru.no_exception() )
        return ru;
    return detail::multiply<R>(t, u);
}

////////////////////////////////
// safe division on unsafe types

namespace detail {

    template<class R>
    typename boost::enable_if_c<
        !std::numeric_limits<R>::is_signed,
        checked_result<R>
    >::type
    constexpr divide(
        const R & t,
        const R & u
    ){
        return t / u;
    }

    template<class R>
    typename boost::enable_if_c<
        std::numeric_limits<R>::is_signed,
        checked_result<R>
    >::type
    constexpr divide(
        const R & t,
        const R & u
    ){
        return
            (u == -1 && t == std::numeric_limits<R>::min()) ?
                checked_result<R>(
                    exception_type::domain_error,
                    "result cannot be represented"
                )
            :
                checked_result<R>(t / u)
        ;
    }
} // detail

template<class R, class T, class U>
checked_result<R>
constexpr divide(
    const T & t,
    const U & u
){
    if(u == 0){
        return checked_result<R>(
            exception_type::domain_error,
            "divide by zero"
        );
    }
    auto tx = cast<R>(t);
    auto ux = cast<R>(u);
    if(!tx.no_exception()
    || !ux.no_exception())
        return checked_result<R>(
            exception_type::overflow_error,
            "failure converting argument types"
        );
    return detail::divide<R>(tx.m_r, ux.m_r);
}

namespace detail_automatic {

    template<class R, class T, class U>
    typename boost::enable_if_c<
        ! std::numeric_limits<U>::is_signed,
        checked_result<R>
    >::type
    constexpr divide(
        const T & t,
        const U & u
    ){
        return t / u;
    }

    template<class R, class T, class U>
    typename boost::enable_if_c<
        std::numeric_limits<U>::is_signed,
        checked_result<R>
    >::type
    constexpr divide(
        const T & t,
        const U & u
    ){
        if(u == -1 && t == std::numeric_limits<R>::min())
            return
                checked_result<R>(
                    exception_type::domain_error,
                    "result cannot be represented"
                )
            ;
        checked_result<R> tx = checked::cast<R>(t);
        if(! tx.no_exception())
            return tx;
        return static_cast<R>(tx) / u;

    }

} // detail_automatic

template<class R, class T, class U>
checked_result<R>
constexpr divide_automatic(
    const T & t,
    const U & u
){
    if(u == 0){
        return checked_result<R>(
            exception_type::domain_error,
            "divide by zero"
        );
    }
    return detail_automatic::divide<R>(t, u);
}

////////////////////////////////
// safe modulus on unsafe types

template<class T>
constexpr std::make_unsigned_t<T>
abs(const T & t){
    return (t < 0 && t != std::numeric_limits<T>::min()) ?
        -t
    :
        t
    ;
}

template<class R, class T, class U>
checked_result<R>
constexpr modulus(
    const T & t,
    const U & u
) {
    static_assert(std::is_integral<T>::value, "only intrinsic integers permitted");
    static_assert(std::is_integral<U>::value, "only intrinsic integers permitted");
    if(0 == u)
        return checked_result<R>(
            exception_type::domain_error,
            "denominator is zero"
        );

    return cast<R>(abs(t) % abs(u));
}

///////////////////////////////////
// shift operations

namespace detail {

template<class R, class T, class U>
typename std::enable_if<
    ! std::numeric_limits<U>::is_signed,
    checked_result<R>
>::type
constexpr check_shift(
    const T & t,
    const U & u
) {
    // INT34-C C++ standard paragraph 5.8
    if(u > std::numeric_limits<T>::digits){
        return checked_result<R>(
           exception_type::domain_error,
           "shifting more bits than available is undefined behavior"
        );
    }
    return cast<R>(t);
}

template<class R, class T, class U>
typename std::enable_if<
    std::numeric_limits<U>::is_signed,
    checked_result<R>
>::type
constexpr check_shift(
    const T & t,
    const U & u
) {
    // INT34-C C++ standard paragraph 5.8
    if(std::numeric_limits<U>::max() > std::numeric_limits<T>::digits){
        if(u > std::numeric_limits<T>::digits){
            return checked_result<R>(
               exception_type::domain_error,
               "shifting more bits than available is undefined behavior"
            );
        }
    }
    // INT34-C C++ standard paragraph 5.8
    if(u < 0){
        return checked_result<R>(
           exception_type::domain_error,
           "shifting negative amount is undefined behavior"
        );
    }
    return cast<R>(t);
}

template<class R, class U>
typename std::enable_if<
    ! std::numeric_limits<R>::is_signed,
    checked_result<R>
>::type
constexpr left_shift(
    const R & r,
    const U & u
){
    return static_cast<R>(r << u);
}

template<class R, class U>
typename std::enable_if<
    std::numeric_limits<R>::is_signed,
    checked_result<R>
>::type
constexpr left_shift(
    const R & r,
    const U & u
){
    // INT13-C. Use bitwise operators only on unsigned operands
    // cannot shift negative values to the left
    return (r < 0) ?
        checked_result<R>(
           exception_type::domain_error,
           "shifting negative values off left is undefined"
        )
    :
        checked_result<R>(r << u)
    ;

    /*
    // if a negative value is shifted left, then it could all of
    // a sudden change sign - we inhibit this here.
    if(r < 0){
        U ui = u;
        R ri = r;
        while(ui-- > 0){
            ri <<= 1;
            if(ri >= 0){
                return checked_result<R>(
                   exception_type::domain_error,
                   "shifting negative values off left is undefined"
                );
            }
        }
    }
    return r;
    */
}

} // detail

// left shift
template<class R, class T, class U>
constexpr checked_result<R> left_shift(
    const T & t,
    const U & u
) {
    // INT13-C Note: We don't enforce recommendation as acually written
    // as it would break too many programs.  Specifically, we permit signed
    // integer operands but require that they not be negative.  This we can only
    // enforce at runtime.

    const checked_result<R> rx = detail::check_shift<R>(t, u);

    if(! rx.no_exception())
        return rx;

    return detail::left_shift<R>(rx, u);
}

// right shift
template<class R, class T, class U>
constexpr checked_result<R> right_shift(
    const T & t,
    const U & u
) {
    // INT13-C Note: We don't enforce recommendation as acually written
    // as it would break too many programs.  Specifically, we permit signed
    // integer operand but require that it not be negative.  This we can only
    // enforce at runtime.

    const checked_result<R> rx = detail::check_shift<R>(t, u);

    if(! rx.no_exception())
        return rx;

    if(u > std::numeric_limits<T>::digits){
        return checked_result<R>(
           exception_type::domain_error,
           "shifting more bits than available is undefined behavior"
        );
    }
    return static_cast<R>(rx) >> u;
}

///////////////////////////////////
// bitwise operations

namespace detail {
    // INT13-C Note: We don't enforce recommendation as acually written
    // as it would break too many programs.  Specifically, we permit signed
    // integer operand but require that it not be negative.  This we can only
    // enforce at runtime.
    template<typename T>
    typename boost::enable_if<
        typename std::is_signed<T>,
        bool
    >::type
    check_bitwise_operand(const T & t){
        return t >= 0;
    }

    template<typename T>
    typename boost::disable_if<
        typename std::is_signed<T>,
        bool
    >::type
    check_bitwise_operand(const T & t){
        return true;
    }
}

template<class R, class T, class U>
constexpr checked_result<R> bitwise_or(
    const T & t,
    const U & u
) {
    if(! detail::check_bitwise_operand(t))
        return checked_result<R>(
           exception_type::domain_error,
           "bitwise operands cannot be negative"
        );

    const checked_result<R> rt = cast<R>(t);
    if(! rt.no_exception())
        return rt;

    const checked_result<R> ru = cast<R>(u);
    if(! ru.no_exception())
        return ru;

    return static_cast<R>(ru) | static_cast<R>(rt);
}

template<class R, class T, class U>
constexpr checked_result<R> bitwise_and(
    const T & t,
    const U & u
) {
    if(! detail::check_bitwise_operand(t))
        return checked_result<R>(
           exception_type::domain_error,
           "bitwise operands cannot be negative"
        );
    const checked_result<R> rt = cast<R>(t);
    if(! rt.no_exception())
        return rt;

    const checked_result<R> ru = cast<R>(u);
    if(! ru.no_exception())
        return ru;

    return static_cast<R>(ru) & static_cast<R>(rt);
}

template<class R, class T, class U>
constexpr checked_result<R> bitwise_xor(
    const T & t,
    const U & u
) {
    if(! detail::check_bitwise_operand(t))
        return checked_result<R>(
           exception_type::domain_error,
           "bitwise operands cannot be negative"
        );
    const checked_result<R> rt = cast<R>(t);
    if(! rt.no_exception())
        return rt;

    const checked_result<R> ru = cast<R>(u);
    if(! ru.no_exception())
        return ru;

    return static_cast<R>(ru) ^ static_cast<R>(rt);
}

} // checked
} // numeric
} // boost

#endif // BOOST_NUMERIC__HPP
