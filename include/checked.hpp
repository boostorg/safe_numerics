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
#include <type_traits> // is_fundamental, make_unsigned
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

namespace detail {

    template<bool RSIGNED, bool TSIGNED>
    struct cast_impl {
        template<class R, class T>
        constexpr checked_result<R>
        invoke(const T & t);
    };
    template<> struct cast_impl<true, true> {
        template<class R, class T>
        constexpr static checked_result<R>
        invoke(const T & t){
            // INT32-C Ensure that operations on signed
            // integers do not overflow
            return
            t > std::numeric_limits<R>::max() ?
                checked_result<R>(
                    exception_type::positive_overflow_error,
                    "converted signed value too large"
                )
            :
            t < std::numeric_limits<R>::min() ?
                checked_result<R>(
                    exception_type::negative_overflow_error,
                    "converted signed value too small"
                )
            :
                checked_result<R>(static_cast<R>(t))
            ;
        }
    };
    template<> struct cast_impl<true, false> {
        template<class R, class T>
        constexpr static checked_result<R>
        invoke(const T & t){
            // INT30-C Ensure that unsigned integer operations
            // do not wrap
            return
            t > std::numeric_limits<R>::max() ?
                checked_result<R>(
                    exception_type::positive_overflow_error,
                    "converted unsigned value too large"
                )
            :
                checked_result<R>(static_cast<R>(t))
            ;
        }
    };
    template<> struct cast_impl<false, false> {
        template<class R, class T>
        constexpr static checked_result<R>
        invoke(const T & t){
            // INT32-C Ensure that operations on unsigned
            // integers do not overflow
            return
            t > std::numeric_limits<R>::max() ?
                checked_result<R>(
                    exception_type::positive_overflow_error,
                    "converted unsigned value too large"
                )
            :
                checked_result<R>(static_cast<R>(t))
            ;
        }
    };
    template<> struct cast_impl<false, true> {
        template<class R, class T>
        constexpr static checked_result<R>
        invoke(const T & t){
            return
            t < 0 ?
                checked_result<R>(
                    exception_type::domain_error,
                    "converted negative value to unsigned"
                )
            :
            t > std::numeric_limits<R>::max() ?
                checked_result<R>(
                    exception_type::positive_overflow_error,
                    "converted signed value too large"
                )
            :
                checked_result<R>(static_cast<R>(t))
            ;
        }
    };
}

template<class R, class T>
constexpr checked_result<R>
cast(
    const T & t
) {
    return
    (! std::numeric_limits<R>::is_integer) ?
        // conversions to floating point types are always OK
        checked_result<R>(t)
    :
    (! std::numeric_limits<T>::is_integer) ?
        // conversions to integer types
        // from floating point types are never OK
        checked_result<R>(
            exception_type::domain_error,
            "conversion of integer to float loses precision"
        )
    :
        // for integer to integer conversions
        // it depends ...
        detail::cast_impl<
            std::numeric_limits<R>::is_signed,
            std::numeric_limits<T>::is_signed
        >::template invoke<R>(t)
    ;
}

////////////////////////////////////////////////////
// safe addition on primitive types

namespace detail {

    // result not an integer (float, double, etc)
    template<class R>
    typename boost::enable_if_c<
        ! std::numeric_limits<R>::is_integer,
        checked_result<R>
    >::type
    constexpr add(
        const R t,
        const R u
    ) {
        return t + u;
    }

    // result unsigned
    template<class R>
    typename boost::enable_if_c<
        std::numeric_limits<R>::is_integer
        && ! std::numeric_limits<R>::is_signed,
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
                    exception_type::positive_overflow_error,
                    "addition overflow"
                )
            :
                checked_result<R>(t + u)
        ;
    }

    // result signed
    template<class R>
    typename boost::enable_if_c<
        std::numeric_limits<R>::is_integer
        && std::numeric_limits<R>::is_signed,
        checked_result<R>
    >::type
    constexpr add(
        const R t,
        const R u
    ) {
        return
            // INT32-C. Ensure that operations on signed integers do not result in overflow
            ((u > 0) && (t > (std::numeric_limits<R>::max() - u))) ?
                checked_result<R>(
                    exception_type::positive_overflow_error,
                    "addition result too large"
                )
            :
            ((u < 0) && (t < (std::numeric_limits<R>::min() - u))) ?
                checked_result<R>(
                    exception_type::negative_overflow_error,
                    "addition result too low"
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
    static_assert(std::is_fundamental<T>::value, "only intrinsic types permitted");
    const checked_result<R> rt(cast<R>(t));
    if(rt.exception() )
        return rt;
    const checked_result<R> ru(cast<R>(u));
    if(ru.exception() )
        return ru;
    return detail::add<R>(t, u);
}

////////////////////////////////////////////////////
// safe subtraction on primitive types
namespace detail {

    // result not an integer (float, double, etc)
    template<class R>
    typename boost::enable_if_c<
        ! std::numeric_limits<R>::is_integer,
        checked_result<R>
    >::type
    constexpr subtract(
        const R t,
        const R u
    ) {
        return t - u;
    }

    // result unsigned
    template<class R>
    typename boost::enable_if_c<
        std::numeric_limits<R>::is_integer
        && ! std::numeric_limits<R>::is_signed,
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
                    exception_type::range_error,
                    "subtraction result cannot be negative"
                )
            :
                checked_result<R>(t - u)
        ;
    }

    // result signed
    template<class R>
    typename boost::enable_if_c<
        std::numeric_limits<R>::is_integer
        && std::numeric_limits<R>::is_signed,
        checked_result<R>
    >::type
    constexpr subtract(
        const R t,
        const R u
    ) { // INT32-C
        return
            // INT32-C. Ensure that operations on signed integers do not result in overflow
            ((u > 0) && (t < (std::numeric_limits<R>::min() + u))) ?
                checked_result<R>(
                    exception_type::positive_overflow_error,
                    "subtraction result overflows result type"
                )
            :
            ((u < 0) && (t > (std::numeric_limits<R>::max() + u))) ?
                checked_result<R>(
                    exception_type::negative_overflow_error,
                    "subtraction result overflows result type"
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
    static_assert(std::is_fundamental<T>::value, "only intrinsic types permitted");
    const checked_result<R> rt(cast<R>(t));
    if(! rt.no_exception() )
        return rt;
    const checked_result<R> ru(cast<R>(u));
    if(! ru.no_exception() )
        return ru;
    return detail::subtract<R>(t, u);
}

////////////////////////////////////////////////////
// safe multiplication on primitive types

namespace detail {

    // result is not an integer (ie float, double)
    template<class R>
    typename boost::enable_if_c<
        ! std::numeric_limits<R>::is_integer,
        checked_result<R>
    >::type
    constexpr multiply(
        const R t,
        const R u
    ){
        return t * u;
    }
    // result unsigned
    template<class R>
    typename boost::enable_if_c<
        std::numeric_limits<R>::is_integer
        && std::is_unsigned<R>::value && (sizeof(R) <= (sizeof(std::uintmax_t) / 2)),
        checked_result<R>
    >::type
    constexpr multiply(
        const R t,
        const R u
    ){
        // INT30-C
        // fast method using intermediate result guaranteed not to overflow
        // todo - replace std::uintmax_t with a size double the size of R
        using i_type = std::uintmax_t;
        return
            static_cast<i_type>(t) * static_cast<i_type>(u)
            > std::numeric_limits<R>::max() ?
                checked_result<R>(
                    exception_type::positive_overflow_error,
                    "multiplication overflow"
                )
            :
                checked_result<R>(t * u)
        ;
    }
    template<class R>
    typename boost::enable_if_c<
        std::numeric_limits<R>::is_integer
        && std::is_unsigned<R>::value && (sizeof(R) > sizeof(std::uintmax_t) / 2),
        checked_result<R>
    >::type
    constexpr multiply(
        const R t,
        const R u
    ){
        // INT30-C
        return
            u > 0 && t > std::numeric_limits<R>::max() / u ?
                checked_result<R>(
                    exception_type::positive_overflow_error,
                    "multiplication overflow"
                )
            :
                checked_result<R>(t * u)
        ;
    }

    // result signed
    template<class R>
    typename boost::enable_if_c<
        std::numeric_limits<R>::is_integer
        && std::is_signed<R>::value && (sizeof(R) <= (sizeof(std::intmax_t) / 2)),
        checked_result<R>
    >::type
    constexpr multiply(
        const R t,
        const R u
    ){
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
                    exception_type::positive_overflow_error,
                    "multiplication overflow"
                )
            :
            (
                static_cast<i_type>(t) * static_cast<i_type>(u)
                < static_cast<i_type>(std::numeric_limits<R>::min())
            ) ?
                checked_result<R>(
                    exception_type::negative_overflow_error,
                    "multiplication overflow"
                )
            :
                checked_result<R>(t * u)
        ;
    }
    template<class R>
    typename boost::enable_if_c<
        std::numeric_limits<R>::is_integer
        && std::is_signed<R>::value && (sizeof(R) > (sizeof(std::intmax_t) / 2)),
        checked_result<R>
    >::type
    constexpr multiply(
        const R t,
        const R u
    ){ // INT32-C
        return t > 0 ?
            u > 0 ?
                t > std::numeric_limits<R>::max() / u ?
                    checked_result<R>(
                        exception_type::positive_overflow_error,
                        "multiplication overflow"
                    )
                :
                    checked_result<R>(t * u)
            : // u <= 0
                u < std::numeric_limits<R>::min() / t ?
                    checked_result<R>(
                        exception_type::negative_overflow_error,
                        "multiplication overflow"
                    )
                :
                    checked_result<R>(t * u)
        : // t <= 0
            u > 0 ?
                t < std::numeric_limits<R>::min() / u ?
                    checked_result<R>(
                        exception_type::negative_overflow_error,
                        "multiplication overflow"
                    )
                :
                    checked_result<R>(t * u)
            : // u <= 0
                t != 0 && u < std::numeric_limits<R>::max() / t ?
                    checked_result<R>(
                        exception_type::positive_overflow_error,
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
    static_assert(std::is_fundamental<T>::value, "only intrinsic types permitted");
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

// note that we presume that the size of R >= size of T
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
    checked_result<R> tx = cast<R>(t);
    checked_result<R> ux = cast<R>(u);
    if(tx.exception()
    || ux.exception())
        return checked_result<R>(
            exception_type::domain_error,
            "failure converting argument types"
        );
    return detail::divide<R>(tx, ux);
}

////////////////////////////////
// safe modulus on unsafe types

// built-in abs isn't constexpr - so fix this here
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
    static_assert(std::is_fundamental<T>::value, "only intrinsic types permitted");
    if(0 == u)
        return checked_result<R>(
            exception_type::domain_error,
            "denominator is zero"
        );

    // why to we need abs here? the sign of the modulus is the sign of the
    // dividend. Consider -128 % -1  The result of this operation should be -1
    // but if I use t % u the x86 hardware uses the divide instruction
    // capturing the modulus as a side effect.  When it does this, it
    // invokes the operation -128 / -1 -> 128 which overflows a signed type
    // and provokes a hardware exception.  We can fix this using abs()
    // since -128 % -1 = -128 % 1 = 0
    return t % abs(u);
}

///////////////////////////////////
// shift operations

namespace detail {

/*
From http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogObvious
Find the log base 2 of an integer with a lookup table

    static const char LogTable256[256] =
    {
    #define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
        -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
        LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
        LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
    };

    unsigned int v; // 32-bit word to find the log of
    unsigned r;     // r will be lg(v)
    register unsigned int t, tt; // temporaries

    if (tt = v >> 16)
    {
      r = (t = tt >> 8) ? 24 + LogTable256[t] : 16 + LogTable256[tt];
    }
    else 
    {
      r = (t = v >> 8) ? 8 + LogTable256[t] : LogTable256[v];
    }

The lookup table method takes only about 7 operations to find the log of a 32-bit value. 
If extended for 64-bit quantities, it would take roughly 9 operations. Another operation
can be trimmed off by using four tables, with the possible additions incorporated into each.
Using int table elements may be faster, depending on your architecture.
*/

// I've "improved" the above and recast as C++ code which depends upon
// the optimizer to minimize the operations.  This should result in
// nine operations to calculate the position of the highest order
// bit in a 64 bit number. RR

constexpr unsigned int log2(const std::uint8_t & t){
    constexpr const char LogTable256[256] = {
        #define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
        -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
        LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
        LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
    };
    return LogTable256[t];
}
constexpr unsigned int log2(const std::uint16_t & t){
    const std::uint8_t upper_half = (t >> 8);
    return upper_half == 0
        ? log2(static_cast<std::uint8_t>(t))
        : 8 + log2(upper_half);
}
constexpr unsigned int log2(const std::uint32_t & t){
    const std::uint16_t upper_half = (t >> 16);
    return upper_half == 0
        ? log2(static_cast<std::uint16_t>(t))
        : 16 + log2(upper_half);
}
constexpr unsigned int log2(const std::uint64_t & t){
    const std::uint32_t upper_half = (t >> 32);
    return upper_half == 0
        ? log2(static_cast<std::uint32_t>(t))
        : 32 + log2(upper_half);
}

#if 0
// todo - optimize for gcc to exploit builtin
/* for gcc compilers
int __builtin_clz (unsigned int x)
      Returns the number of leading 0-bits in x, starting at the
      most significant bit position.  If x is 0, the result is undefined.
*/

#ifndef __has_feature         // Optional of course.
  #define __has_feature(x) 0  // Compatibility with non-clang compilers.
#endif

template<typename T>
constexpr unsigned int leading_zeros(const T & t){
    if(0 == t)
        return 0;
    #if __has_feature(builtin_clz)
        return  __builtin_clz(t);
    #else
    #endif
}
#endif

template<typename T>
constexpr unsigned int significant_bits(const T & t){
    return log2(
        static_cast<typename std::make_unsigned<T>::type>(t)
    ) + 1;
}

// INT34-C C++

// standard paragraph 5.8 / 2
// The value of E1 << E2 is E1 left-shifted E2 bit positions;
// vacated bits are zero-filled.
template<class R, class T, class U>
typename std::enable_if<
    // If E1 has an unsigned type
    ! std::numeric_limits<T>::is_signed,
    checked_result<R>
>::type
constexpr checked_left_shift(
    const T & t,
    const U & u
) noexcept {
    // the value of the result is E1 x 2^E2, reduced modulo one more than
    // the maximum value representable in the result type.

    // note: we are intentionally varying from the standards language here.
    // a safe<unsigned> is meant to be value preserving.  That is t << u should
    // equal an arithmetically correct value or fail in some visible manner.
    // So we're going to fail if are shift loses higher order bits.

    // note: there is a good argument for following exactly the standards
    // language above.  Reasonable people can disagree.

    if(u > std::numeric_limits<R>::digits - significant_bits(t)){
        // behavior is undefined
        return checked_result<R>(
           exception_type::domain_error,
           "shifting more bits than available is undefined behavior"
        );
    }
    return t << u;
}

template<class R, class T, class U>
typename std::enable_if<
    // if E1 has a signed type
    std::numeric_limits<T>::is_signed,
    checked_result<R>
>::type
constexpr checked_left_shift(
    const T & t,
    const U & u
) {
    if(t < 0){
        return checked_result<R>(
           exception_type::domain_error,
           "shifting a negative value is undefined behavior"
        );
    }

    // and E1 x 2^E2 is representable in the corresponding
    // unsigned type of the result type,

    return checked_left_shift<R>(
        static_cast<typename std::make_unsigned<T>::type>(t),
        u
    );
}

} // detail

template<class R, class T, class U>
constexpr checked_result<R> left_shift(
    const T & t,
    const U & u
){
    // INT34-C - Do not shift an expression by a negative number of bits
    // C++ Standard standard paragraph 5.8.3 Doesn't excactly say this.
    // It says the operation is defined as E1 / 2^E2 which IS defined when
    // E2 is negative.
    // I'm going to err on the side of concervativism dis-allowing negative
    // shift counts.
    if(u < 0){
        return checked_result<R>(
           exception_type::domain_error,
           "shifting negative amount is undefined behavior"
        );
    }
    if(t == 0)
        return cast<R>(t);
    return detail::checked_left_shift<R>(t, u);
}

// right shift
namespace detail {

template<class R, class T, class U>
typename std::enable_if<
    // If E1 has an unsigned type
    ! std::numeric_limits<T>::is_signed,
    checked_result<R>
>::type
constexpr checked_right_shift(
    const T & t,
    const U & u
) noexcept {
    return cast<R>(t >> u);
}

template<class R, class T, class U>
typename std::enable_if<
    // or if E1 has a signed type
    std::numeric_limits<T>::is_signed,
    checked_result<R>
>::type
constexpr checked_right_shift(
    const T & t,
    const U & u
) {
    // INT13C - Use bitwise operators only on unsigned integers
    if(t < 0){
        // note that the C++ standard considers this case is "implemenation
        // defined" rather than "undefined".  We'll err on the side of
        // caution and disallow this.
        return checked_result<R>(
           exception_type::domain_error,
           "shifting a negative value is undefined behavior"
        );
    }

    // the value is the integral part of E1 / 2^E2,
    return cast<R>(t >> u);
}

} // detail

// right shift
template<class R, class T, class U>
constexpr checked_result<R> right_shift(
    const T & t,
    const U & u
) {
    // INT34-C - Do not shift an expression by a negative number of bits
    // C++ Standard standard paragraph 5.8.3 Doesn't excactly say this.
    // It says the operation is defined as E1 / 2^E2 which IS defined when
    // E2 is negative.
    // I'm going to err on the side of concervativism dis-allowing negative
    // shift counts.
    if(u < 0){
        // note that the C++ standard considers this case is "implemenation
        // defined" rather than "undefined".  We'll err on the side of
        // caution and dis-allow this.
        return checked_result<R>(
           exception_type::domain_error,
           "shifting a negative value is undefined behavior"
        );
    }
    // standard paragraph 5.8 / 3
    // Again, C++ Standard standard paragraph 5.8.3 doesn't seem to disallow
    // shifts count > argument width.

    // Also, my Clang compiler traps this an an error when used in a constexpr
    // at compile time.

    // Given all of the above, we're going to disallow shifting which exceeds
    // the width of the argument.
    else
    if(u > std::numeric_limits<R>::digits){
        // behavior is undefined
        return checked_result<R>(
           exception_type::domain_error,
           "shifting more bits than available is undefined behavior"
        );
    }
    if(t == 0)
        return cast<R>(0);
    return detail::checked_right_shift<R>(t, u);
}

///////////////////////////////////
// bitwise operations

namespace detail {
    // INT13-C Note: We don't enforce recommendation as acually written
    // as it would break too many programs.  Specifically, we permit signed
    // integer operand but require that it not be negative.  This we can only
    // enforced at runtime.
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
} // detail

template<class R, class T, class U>
constexpr checked_result<R> bitwise_or(
    const T & t,
    const U & u
) {
    /*
    if(! detail::check_bitwise_operand(t))
        return checked_result<R>(
           exception_type::domain_error,
           "bitwise operands cannot be negative"
        );
    */
    
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
