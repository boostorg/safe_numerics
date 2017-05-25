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
#include "utility.hpp"

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
        invoke(const T & t) noexcept {
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
        invoke(const T & t) noexcept {
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
        invoke(const T & t) noexcept {
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
        invoke(const T & t) noexcept {
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
) noexcept {
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
    ) noexcept {
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
    ) noexcept {
        return
            // INT30-C. Ensure that unsigned integer operations do not wrap
            std::numeric_limits<R>::max() - u < t ?
                checked_result<R>(
                    exception_type::positive_overflow_error,
                    "addition result too large"
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
    ) noexcept {
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
) noexcept {
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
    ) noexcept {
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
    ) noexcept {
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
    ) noexcept { // INT32-C
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
) noexcept {
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
    ) noexcept {
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
    ) noexcept {
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
    ) noexcept {
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
    ) noexcept {
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
    ) noexcept { // INT32-C
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
) noexcept {
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
    ) noexcept {
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
    ) noexcept {
        return
            (u == -1 && t == std::numeric_limits<R>::min()) ?
                checked_result<R>(
                    exception_type::range_error,
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
) noexcept {
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
abs(const T & t) noexcept {
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
) noexcept {
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

    // see 5.8 & 1
    // if right operand is
    // greater than or equal to the length in bits of the promoted left operand.
    if(u > std::numeric_limits<R>::digits - utility::significant_bits(t)){
        // behavior is undefined
        return checked_result<R>(
           exception_type::undefined_behavior,
           "shifting more bits than available is undefined behavior"
        );
    }
    return t << u;
}

template<class R, class T, class U>
typename std::enable_if<
    // otherwise if E1 has a signed type
    std::numeric_limits<T>::is_signed,
    checked_result<R>
>::type
constexpr checked_left_shift(
    const T & t,
    const U & u
) noexcept {
    // and [E1] has a non-negative value
    if(t >= 0){
        // and E1 x 2^E2 is representable in the corresponding
        // unsigned type of the result type,

        // then that value, converted to the result type,
        // is the resulting value
        return checked_left_shift<R>(
            static_cast<typename std::make_unsigned<T>::type>(t),
            u
        );
    }
    // otherwise, the behavior is undefined.
    return checked_result<R>(
       exception_type::undefined_behavior,
       "shifting a negative value is undefined behavior"
    );
}

} // detail

template<class R, class T, class U>
constexpr checked_result<R> left_shift(
    const T & t,
    const U & u
) noexcept {
    // INT34-C - Do not shift an expression by a negative number of bits

    // standard paragraph 5.8 & 1
    // if the right operand is negative
    if(u < 0){
        return checked_result<R>(
           exception_type::implementation_defined_behavior,
           "shifting negative amount is undefined behavior"
        );
    }
    if(u > std::numeric_limits<R>::digits){
        // behavior is undefined
        return checked_result<R>(
           exception_type::implementation_defined_behavior,
           "shifting more bits than available is undefined behavior"
        );
    }
    if(t == 0)
        return cast<R>(t);
    return detail::checked_left_shift<R>(t, u);
}

// right shift
namespace detail {

// INT34-C C++

// standard paragraph 5.8 / 3
// The value of E1 << E2 is E1 left-shifted E2 bit positions;
// vacated bits are zero-filled.
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
    // the value of the result is the integral part of the
    // quotient of E1/2E2
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
) noexcept {
    if(t < 0){
        // note that the C++ standard considers this case is "implemenation
        // defined" rather than "undefined".
        return checked_result<R>(
           exception_type::implementation_defined_behavior,
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
) noexcept {
    // INT34-C - Do not shift an expression by a negative number of bits

    // standard paragraph 5.8 & 1
    // if the right operand is negative
    if(u < 0){
        return checked_result<R>(
           exception_type::implementation_defined_behavior,
           "shifting negative amount is undefined behavior"
        );
    }
    if(u > std::numeric_limits<R>::digits){
        // behavior is undefined
        return checked_result<R>(
           exception_type::implementation_defined_behavior,
           "shifting more bits than available is undefined behavior"
        );
    }
    if(t == 0)
        return cast<R>(0);
    return detail::checked_right_shift<R>(t, u);
}

///////////////////////////////////
// bitwise operations

// INT13-C Note: We don't enforce recommendation as acually written
// as it would break too many programs.  Specifically, we permit signed
// integer operands.

template<class R, class T, class U>
constexpr checked_result<R> bitwise_or(
    const T & t,
    const U & u
) noexcept {
    using namespace boost::numeric::utility;
    const unsigned int result_size
        = std::max(significant_bits(t), significant_bits(u));

    if(result_size > bits_type<R>::value){
        return checked_result<R>{
            exception_type::positive_overflow_error,
            "result type too small to hold bitwise or"
        };
    }
    return t | u;
}

template<class R, class T, class U>
constexpr checked_result<R> bitwise_xor(
    const T & t,
    const U & u
) noexcept {
    using namespace boost::numeric::utility;
    const unsigned int result_size
        = std::max(significant_bits(t), significant_bits(u));

    if(result_size > bits_type<R>::value){
        return checked_result<R>{
            exception_type::positive_overflow_error,
            "result type too small to hold bitwise or"
        };
    }
    
    const checked_result<R> rt = cast<R>(t);
    if(! rt.no_exception())
        return rt;

    const checked_result<R> ru = cast<R>(u);
    if(! ru.no_exception())
        return ru;

    return static_cast<R>(ru) ^ static_cast<R>(rt);
}

template<class R, class T, class U>
constexpr checked_result<R> bitwise_and(
    const T & t,
    const U & u
) noexcept {
    using namespace boost::numeric::utility;
    const unsigned int result_size
        = std::min(significant_bits(t), significant_bits(u));

    if(result_size > bits_type<R>::value){
        return checked_result<R>{
            exception_type::positive_overflow_error,
            "result type too small to hold bitwise or"
        };
    }
    return t & u;
}

} // checked
} // numeric
} // boost

#endif // BOOST_NUMERIC__HPP
