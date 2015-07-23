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
#include <type_traits> // is_integral

#include <boost/utility/enable_if.hpp>

#include "safe_common.hpp"
#include "checked_result.hpp"

namespace boost {
namespace numeric {
namespace checked {

////////////////////////////////////////////////////
// layer 0 - implment safe operations for intrinsic integers
// Note presumption of twos complement integer arithmetic

////////////////////////////////////////////////////
// safe casting on primitive types

template<class R, class T>
SAFE_NUMERIC_CONSTEXPR checked_result<R>
cast(
    const T & t
) {
    return
    std::numeric_limits<R>::is_signed ?
        // T is signed
        std::numeric_limits<T>::is_signed ?
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
                checked_result<R>(t)
        : // T is unsigned
            t > std::numeric_limits<R>::max() ?
                checked_result<R>(
                    exception_type::range_error,
                    "converted unsigned value too large"
                )
            :
                checked_result<R>(t)
    : // std::numeric_limits<R>::is_signed
        // T is signed
        ! std::numeric_limits<T>::is_signed ?
            t > std::numeric_limits<R>::max() ?
                checked_result<R>(
                    exception_type::range_error,
                    "converted unsigned value too large"
                )
            :
                checked_result<R>(t)
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
                checked_result<R>(t)
    ;
}

////////////////////////////////////////////////////
// safe addition on primitive types

namespace detail {

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
                    exception_type::overflow_error,
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
                    exception_type::overflow_error,
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
    static_assert(std::is_integral<T>::value, "only intrinsic integers permitted");
    static_assert(std::is_integral<U>::value, "only intrinsic integers permitted");
    const checked_result<R> ru = cast<R>(u);
    const checked_result<R> rt = cast<R>(t);
    return
        rt != exception_type::no_exception ?
            rt
        :
        ru != exception_type::no_exception ?
            ru
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
                exception_type::overflow_error,
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
    static_assert(std::is_integral<T>::value, "only intrinsic integers permitted");
    static_assert(std::is_integral<U>::value, "only intrinsic integers permitted");
    return
        cast<R>(t) != exception_type::no_exception ?
            cast<R>(t)
        :
        cast<R>(u) != exception_type::no_exception ?
            cast<R>(u)
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
// safe multiplication on primitive types

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
SAFE_NUMERIC_CONSTEXPR checked_result<R> multiply(
    const R & minr,
    const R & maxr,
    const T & t,
    const U & u
) {
    static_assert(std::is_integral<T>::value, "only intrinsic integers permitted");
    static_assert(std::is_integral<U>::value, "only intrinsic integers permitted");
    return
        cast<R>(t) != exception_type::no_exception ?
            cast<R>(t)
        :
        cast<R>(u) != exception_type::no_exception ?
            cast<R>(u)
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
                exception_type::domain_error,
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
    static_assert(std::is_integral<T>::value, "only intrinsic integers permitted");
    static_assert(std::is_integral<U>::value, "only intrinsic integers permitted");
    return
        cast<R>(t) != exception_type::no_exception ?
            cast<R>(t)
        :
        cast<R>(u) != exception_type::no_exception ?
            cast<R>(u)
        :
            u == 0 ?
                checked_result<R>(
                    exception_type::domain_error,
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
                exception_type::domain_error,
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
    static_assert(std::is_integral<T>::value, "only intrinsic integers permitted");
    static_assert(std::is_integral<U>::value, "only intrinsic integers permitted");
    return
        cast<R>(t) != exception_type::no_exception ?
            cast<R>(t)
        :
        cast<R>(u) != exception_type::no_exception ?
            cast<R>(u)
        :
            u == 0 ?
                checked_result<R>(
                    exception_type::domain_error,
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

///////////////////////////////////
// shift operations

namespace detail {

template<class R, class T, class U>
SAFE_NUMERIC_CONSTEXPR checked_result<R> check_shift(
    const T & t,
    const U & u
) {
    // INT13-CPP C++ standard paragraph 5.8
    static_assert(
        std::numeric_limits<T>::is_integer,
        "shift operation can only be applied to integers"
    );
    static_assert(
        std::numeric_limits<U>::is_integer,
        "number of bits to shift must be an integer"
    );
    const checked_result<R> ru = cast<R>(u);
    // INT34-C C++ standard paragraph 5.8
    return
        ( ! ru.is_valid()) ?
            ru
        :
        ( ru < 0) ?
            checked_result<R>(
               exception_type::domain_error,
               "shifting negative amount is undefined behavior"
            )
        :
        ( ru > std::numeric_limits<T>::digits) ?
            checked_result<R>(
               exception_type::domain_error,
               "shifting more bits than available is undefined behavior"
            )
        :
            checked_result<R>(0)
    ;
}

} // detail

// left shift
template<class R, class T, class U>
SAFE_NUMERIC_CONSTEXPR checked_result<R> left_shift(
    const T & t,
    const U & u
) {
    const checked_result<R> r = detail::check_shift<R>(t, u);
    return
        (! r.is_valid()) ?
            r
        :
            checked_result<R>(static_cast<R>(r) << t);
        ;
}

// right shift
template<class R, class T, class U>
SAFE_NUMERIC_CONSTEXPR checked_result<R> right_shift(
    const T & t,
    const U & u
) {
    const checked_result<R> r = detail::check_shift<R>(t, u);
    return
        (! r.is_valid()) ?
            r
        :
            checked_result<R>(static_cast<R>(r) >> t);
        ;
}

///////////////////////////////////
// bitwise operations

template<class R, class T, class U>
SAFE_NUMERIC_CONSTEXPR checked_result<R> bitwise_or(
    const T & t,
    const U & u
) {
    static_assert(
        std::is_integral<T>::value && std::is_signed<T>::value
        && std::is_integral<U>::value && std::is_signed<T>::value,
        "only intrinsic unsigned integers permitted"
    );
    const checked_result<R> ru = cast<R>(u);
    const checked_result<R> rt = cast<R>(t);
    return
        rt != exception_type::no_exception ?
            rt
        :
        ru != exception_type::no_exception ?
            ru
        :
            static_cast<R>(ru) | static_cast<R>(rt)
    ;
}

template<class R, class T, class U>
SAFE_NUMERIC_CONSTEXPR checked_result<R> bitwise_and(
    const T & t,
    const U & u
) {
    static_assert(
        std::is_integral<T>::value && std::is_signed<T>::value
        && std::is_integral<U>::value && std::is_signed<T>::value,
        "only intrinsic unsigned integers permitted"
    );
    const checked_result<R> ru = cast<R>(u);
    const checked_result<R> rt = cast<R>(t);
    return
        rt != exception_type::no_exception ?
            rt
        :
        ru != exception_type::no_exception ?
            ru
        :
            static_cast<R>(ru) & static_cast<R>(rt)
    ;
}

template<class R, class T, class U>
SAFE_NUMERIC_CONSTEXPR checked_result<R> bitwise_xor(
    const T & t,
    const U & u
) {
    static_assert(
        std::is_integral<T>::value && std::is_signed<T>::value
        && std::is_integral<U>::value && std::is_signed<T>::value,
        "only intrinsic unsigned integers permitted"
    );
    const checked_result<R> ru = cast<R>(u);
    const checked_result<R> rt = cast<R>(t);
    return
        rt != exception_type::no_exception ?
            rt
        :
        ru != exception_type::no_exception ?
            ru
        :
            static_cast<R>(ru) ^ static_cast<R>(rt)
    ;
}

} // checked
} // numeric
} // boost

#endif // BOOST_NUMERIC__HPP
