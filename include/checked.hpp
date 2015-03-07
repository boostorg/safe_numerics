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
#include <utility>

//#include "safe_compare.hpp"
//#include "safe_cast.hpp"
#include "checked_result.hpp"

namespace boost {
namespace numeric {
namespace checked {

    namespace detail {
    
    template<bool RS, bool TS, bool US>
    struct addition;

    // both arguments unsigned
    template<>
    struct addition<false, false, false> {
        template<class R, class T, class U>
        static constexpr checked_result<R> add(const R && r, const T & t, const U & u) {
            return
                r < t || r < u ?
                    checked_result<R>(
                        checked_result<R>::exception_type::overflow_error,
                        "addition overflow"
                    )
                :
                    checked_result<R>(r)
                ;
        }
    };
    template<>
    struct addition<true, false, false> {
        template<class R, class T, class U>
        static constexpr checked_result<R> add(const R && r, const T & t, const U & u) {
            return
                r < 0 ?
                    checked_result<R>(
                        checked_result<R>::exception_type::overflow_error,
                        "addition overflow"
                    )
                :
                    checked_result<R>(r)
                ;
        }
    };

    // both arguments signed
    template<>
    struct addition<false, true, true> {
        template<class R, class T, class U>
        static constexpr checked_result<R> add(const R && r, const T & t, const U & u) {
            return
                (t > 0 && u > 0 && (r < t || r < u))
                || (t < 0 && u < 0 && (r > t || r > u))  ?
                    checked_result<R>(
                        checked_result<R>::exception_type::overflow_error,
                        "addition overflow"
                    )
                :
                    checked_result<R>(r)
                ;
        }
    };
    template<>
    struct addition<true, true, true> {
        template<class R, class T, class U>
        static constexpr checked_result<R> add(const R && r, const T & t, const U & u) {
            return
                (t > 0 && u > 0 && r < 0)
                || (t < 0 && u < 0 && r >= 0)  ?
                    checked_result<R>(
                        checked_result<R>::exception_type::overflow_error,
                        "addition overflow"
                    )
                :
                    checked_result<R>(r)
                ;
        }
    };


    // T unsigned, U signed
    template<>
    struct addition<false, false, true> {
        template<class R, class T, class U>
        static constexpr checked_result<R> add(const R && r, const T & t, const U & u){
            return
                (u > 0) ?
                    addition<false, false, false>::add(
                        std::move(r),
                        t,
                        static_cast<typename std::make_unsigned<U>::type >(u)
                    )
                :
                (r > t) ?
                    checked_result<R>(
                        checked_result<R>::exception_type::overflow_error,
                        "addition overflow"
                    )
                :
                    checked_result<R>(r)
                ;
            }
    };
    template<>
    struct addition<true, false, true> {
        template<class R, class T, class U>
        static constexpr checked_result<R> add(const R && r, const T & t, const U & u){
            return
                (u > 0) && (r < 0) ?
                    checked_result<R>(
                        checked_result<R>::exception_type::overflow_error,
                        "addition overflow"
                    )
                :
                    checked_result<R>(r)
                ;
        }
    };

    // T signed, U unsigned
    template<>
    struct addition<false, true, false> {
        template<class R, class T, class U>
        static constexpr checked_result<R> add(const R && r, const T & t, const U & u) {
            return addition<false, false, true>::add<R>(std::move(r), u, t);
        }
    };
    template<>
    struct addition<true, true, false> {
        template<class R, class T, class U>
        static constexpr checked_result<R> add(const R && r, const T & t, const U & u) {
            return addition<true, false, true>::add<R>(std::move(r), u, t);
        }
    };

    } // namespace detail

    template<class R, class T, class U>
    constexpr checked_result<R> add(const T & t, const U & u){
        static_assert(
            std::is_same<decltype(t + u), R>::value,
            "invalid result type"
        );
        return
            (std::numeric_limits<R>::min()
            <= std::numeric_limits<T>::min() + std::numeric_limits<U>::min())
            &&
            (std::numeric_limits<R>::max()
            >= std::numeric_limits<T>::max() + std::numeric_limits<U>::max())
            ?
                detail::addition<
                    std::numeric_limits<R>::is_signed,
                    std::numeric_limits<T>::is_signed,
                    std::numeric_limits<U>::is_signed
                >::template add(t + u, t, u)
            :
                checked_result<R>(t + u)
            ;
    }






/*
    namespace detail {

    ////////////////////////////////////////////////////
    // layer 0 - detect overflows / alteration at the
    // atomic operation level taking care to work around
    // otherwise undetect alterations in integers due
    // to machine architecture.  Note presumption of twos
    // complement integer arithmetic

    /////////////////////////////
    // subtraction implementation
    template<bool TS, bool US>
    struct subtraction;

    // both arguments unsigned
    template<>
    struct subtraction<false, false> {
        template<class R, class T, class U>
        constexpr static bool overflow(const R & r, const T & t, const U & u){
            return safe_compare::less_than(t, u);
        }
        template<class R, class P, class T, class U>
        static R subtract(const T & t, const U & u) {
            R tmp = t - u;
            if(overflow(tmp, t, u))
                P::overflow_error("safe range subtraction unsigned difference less than zero");
            return tmp;
        }
    };
    // both arguments signed
    template<>
    struct subtraction<true, true> {
        template<class R, class T, class U>
        constexpr static bool overflow(const R & r, const T & t, const U & u){
            return (t > 0 && u < 0 && r < 0) ||(t < 0 && u > 0 && r >= 0);
        }
        template<class R, class P, class T, class U>
        static R subtract(const T & t, const U & u){
            R tmp = t - u;
            if(overflow(tmp, t, u))
                P::overflow_error("safe range subtraction result overflow");
            return tmp;
        }
    };
    // T unsigned, U signed
    template<>
    struct subtraction<false, true> {
        template<class R, class T, class U>
        constexpr static bool overflow(const R & r, const T & t, const U & u){
            return u < 0 || u >= t;
        }
        template<class R, class P, class T, class U>
        static R subtract(const T & t, const U & u){
            if(boost::numeric::is_unsigned<R>::value){
                if(u < 0)
                    P::overflow_error("safe range left operand value altered");
                // u >= 0
                if(u > t)
                    P::overflow_error("unsigned result is negative");
           }
           // result is signed
           return t - u;
        }
    };
    // T signed, U unsigned
    template<>
    struct subtraction<true, false> {
        template<class R, class T, class U>
        constexpr static bool overflow(const R & r, const T & t, const U & u){
            return u < 0 || u >= t;
        }
        template<class R, class P, class T, class U>
        static R subtract(const T & t, const U & u){
            if(boost::numeric::is_unsigned<R>::value){
                return subtraction<false, false>::subtract<R, P>(
                    safe_cast<R>(t),
                    safe_cast<R>(u)
                );
            }
            // result is signed
            return subtraction<true, true>::subtract<R, P>(
                t,
                safe_cast<R>(u)
            );
        }
    };

    } // detail

    template<class R, class P, class T, class U>
    R subtract(const T & t, const U & u){
        return detail::subtraction<
            boost::is_signed<T>::value,
            boost::is_signed<U>::value
        >::template subtract<R, P, T, U>(t, u);
    }

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
