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
        static_assert(! is_safe<T>::value, "should only be a base type here!");
        return
            detail::cast<R>(t) != checked_result<R>::exception_type::no_exception ?
                detail::cast<R>(t)
            :
            detail::cast<R>(u) != checked_result<R>::exception_type::no_exception ?
                detail::cast<R>(u)
            :
            //sizeof(R) >= sizeof(T) + sizeof(U) ?
            //    checked_result<R>(static_cast<R>(t) * static_cast<R>(u))
            //:
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

/*
    namespace detail {

    ////////////////////////////////
    // multiplication implementation

    template<class T, class U>
    decltype(T() * U())
    check_multiplication_overflow(const T & t, const U & u){
        typedef decltype(T() * U()) result_type;
        char const * const msg = "safe range multiplication overflow";
        // presume that size of uintmax_t and intmax_t are the same
        typedef bits<boost::uintmax_t> available_bits;

        if(multiply_result_bits<T, U>::value
        <= boost::numeric::bits<result_type>::value)
            return t * u;

        if(multiply_result_bits<T, U>::value <= available_bits::value){
            typedef typename multiply_result_type<T, U>::type temp_type;
            temp_type tmp = static_cast<temp_type>(t) * temp_type(u);
            // the following works for both positive and negative results
            // and for both signed and unsigned numbers
            if(tmp > boost::integer_traits<result_type>::const_max)
                boost::numeric::overflow(msg);
            if(tmp < boost::integer_traits<result_type>::const_min)
                boost::numeric::overflow(msg);
            return static_cast<result_type>(tmp);
        }

        // when the there is no native type which can hold the product
        // use multible precision

        // t is factored as (a << temp_bits) + b
        // u is factored as (c << temp_bits) + d
        // so we use multi-precision:
        // a + b
        // c + d
        // -----
        //    bd
        //   ad
        //   cb
        //  ac
        // -----
        //    ..

        if(boost::numeric::is_unsigned<result_type>::value
        && (t < 0 || u < 0))
            overflow("conversion of negative value to unsigned");

        if(t == 1)
            return u;
        if(u == 1)
            return t;
            
        result_type rt = t;
        if(rt < 0){
            rt = ~rt + 1;
            // address
            if(rt < 0)
                overflow("overflow of negative value");
        }
        result_type ru = u;
        if(ru < 0){
            ru = ~ru + 1;
            // address
            if(ru < 0)
                overflow("overflow of negative value");
        }

        // check positive values for overflow

        // t is factored as (a << temp_bits) + b
        // u is factored as (c << temp_bits) + d
        // so we use multi-precision:
        // a + b
        // c + d
        // -----
        //    bd
        //   ad
        //   cb
        //  ac
        // -----
        //    ..

        typedef boost::uintmax_t accumulator_type;
        const int temp_bits = bits<accumulator_type>::value / 2;
        typedef typename boost::uint_t<temp_bits>::least temp_type;

        temp_type a = (static_cast<accumulator_type>(rt) >> temp_bits);
        temp_type c = (static_cast<accumulator_type>(ru) >> temp_bits);
        if(0 != a && 0 != c)
            overflow(msg);

        temp_type b = static_cast<temp_type>(rt);
        if((static_cast<accumulator_type>(b) * static_cast<accumulator_type>(c) >> temp_bits) > 0)
            overflow(msg);

        temp_type d = static_cast<const temp_type>(ru);
        if(0 != (static_cast<accumulator_type>(a) * static_cast<accumulator_type>(d) >> temp_bits))
            overflow(msg);
            
        return t * u;
    }
    template<class T, class U>
    decltype(T() / U())
    check_division_overflow(const T & t, const U & u){
        if(0 == u)
            overflow("divide by zero");

        if(boost::numeric::is_signed<U>::value){
            // t unsigned, u signed
            if(boost::numeric::is_unsigned<T>::value){
                if(u < 0){
                    overflow("conversion of negative value to unsigned");
                }
            }
            else{
            // both signed
                // pathological case: change sign on negative number so it overflows
                if(t == boost::integer_traits<T>::const_min && u == -1)
                    overflow("overflow in result");
            }
        }
        // both unsigned
        // t signed, u unsigned
        return t / u;
    }
    template<class T, class U>
    decltype(T() / U())
    check_modulus_overflow(const T & t, const U & u){
        if(0 == u)
            overflow("modulus divide by zero");

        if(boost::numeric::is_signed<U>::value){
            // t unsigned, u signed
            if(boost::numeric::is_unsigned<T>::value){
                if(u < 0){
                    overflow("conversion of negative value to unsigned");
                }
            }
            else{
            // both signed
                // pathological case: change sign on negative number so it overflows
                if(t == boost::integer_traits<T>::const_min && u == -1)
                    overflow("overflow in result");
            }
        }
        // both unsigned
        // t signed, u unsigned
        return t % u;
    }
}   // detail
*/
} // checked
} // numeric
} // boost

#endif // BOOST_NUMERIC__HPP
