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

// contains operations for doing checked aritmetic on native
// C++ types.

#include "overflow.hpp"
#include "safe_compare.hpp"
#include "safe_cast.hpp"

// we could have used decltype and auto for C++11 but we've decided
// to use boost/typeof to be compatible with older compilers
#include <boost/typeof/typeof.hpp>

namespace boost {
namespace numeric {
namespace checked {

    namespace detail {
    template<bool TS, bool US>
    struct addition;

    // both arguments unsigned
    template<>
    struct addition<false, false> {
        template<class R, class T, class U>
        static constexpr bool overflow(const R & r, const T & t, const U & u){
            return boost::numeric::safe_compare::less_than(r, t)
            || boost::numeric::safe_compare::less_than(r, u);
        }
        template<class T, class U>
        static constexpr bool addition_overflow(const T & t, const U & u){
            return overflow(t + u, t, u);
        }
        template<class R, class T, class U>
        static R add(const T & t, const U & u) {
            R tmp = t + u;
            if(overflow(tmp, t, u))
                 boost::numeric::overflow("safe range addition result overflow");
            return tmp;
        }
    };
    // both arguments signed
    template<>
    struct addition<true, true> {
        template<class R, class T, class U>
        static constexpr bool overflow(const R & r, const T & t, const U & u){
            return boost::numeric::safe_compare::less_than(r, t)
            || boost::numeric::safe_compare::less_than(r, u);
        }
        template<class T, class U>
        static constexpr bool addition_overflow(const T & t, const U & u){
            return overflow(t + u, t, u);
        }
        template<class R, class T, class U>
        static R add(const T & t, const U & u) {
            if(t > 0 && u > 0){
                R tmp = t + u;
                if(tmp < 0)
                    boost::numeric::overflow("safe range addition result overflow");
                return tmp;
            }
            if(t < 0 && u < 0){
                R tmp = t + u;
                if(tmp >= 0)
                    boost::numeric::overflow("safe range addition result underflow");
                return tmp;
            }
            return t + u;
        }
    };
    // T unsigned, U signed
    template<>
    struct addition<false, true> {
        template<class R, class T, class U>
        static R add(const T & t, const U & u){
            if(boost::numeric::is_unsigned<R>::value){
                if(u < 0)
                    overflow("safe range right operand value altered");
                return addition<false, false>::add<R>(
                    t,
                    static_cast<typename boost::make_unsigned<T>::type>(u)
                );
            }
            else{
                if(u > 0){
                    R tmp = t + u;
                    if(tmp <= 0)
                        overflow("safe range addition result overflow");
                    return t + u;
                }
            }
            return t + u;
        }
    };
    // T signed, U unsigned
    template<>
    struct addition<true, false> {
        template<class R, class T, class U>
        static R add(const T & t, const U & u){
            return addition<false, true>::add<R>(u, t);
        }
    };
    } // detail

    template<class R, class T, class U>
    R add(const T & t, const U & u){
        return detail::addition<
            boost::is_unsigned<T>::value,
            boost::is_unsigned<U>::value
        >::template add<R>(
            t, u
        );
    }
} // checked

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
    struct check_subtraction_overflow{};

    // both arguments signed
    template<>
    struct check_subtraction_overflow<true, true> {
        template<class T, class U>
        static BOOST_TYPEOF_TPL(T() + U())
        subtract(const T & t, const U & u){
            BOOST_AUTO_TPL(tmp, t - u);
            if(t > 0 && u < 0){
                if(tmp < 0)
                    overflow("safe range subtraction result overflow");
            }
            if(t < 0 && u > 0)
                if(tmp >= 0){
                    overflow("safe range subtraction result underflow");
            }
            return tmp;
        }
    };
    // both arguments unsigned
    template<>
    struct check_subtraction_overflow<false, false> {
        template<class T, class U>
        static BOOST_TYPEOF_TPL(T() + U())
        subtract(const T & t, const U & u) {
            if(safe_compare::less_than(t, u))
                overflow("safe range subtraction unsigned difference less than zero");
            return t - u;
        }
    };
    // T unsigned, U signed
    template<>
    struct check_subtraction_overflow<false, true> {
        template<class T, class U>
        static BOOST_TYPEOF_TPL(T() + U())
        subtract(const T & t, const U & u){
            typedef BOOST_TYPEOF_TPL(T() + U()) result_type;
            if(boost::numeric::is_unsigned<result_type>::value){
                if(u < 0)
                    overflow("safe range left operand value altered");
                // u >= 0
                if(u > t)
                    overflow("unsigned result is negative");
           }
           // result is signed
           return t - u;
        }
    };
    // T signed, U unsigned
    template<>
    struct check_subtraction_overflow<true, false> {
        template<class T, class U>
        static BOOST_TYPEOF_TPL(T() + U())
        subtract(const T & t, const U & u){
            typedef BOOST_TYPEOF_TPL(T() + U()) result_type;
            if(boost::numeric::is_unsigned<result_type>::value){
                return check_subtraction_overflow<false, false>::subtract(
                    safe_cast<result_type>(t),
                    safe_cast<result_type>(u)
                );
            }
            // result is signed
            return check_subtraction_overflow<true, true>::subtract(
                t,
                safe_cast<result_type>(u)
            );
        }
    };

    ////////////////////////////////
    // multiplication implementation

    template<class T, class U>
    BOOST_TYPEOF_TPL(T() * U())
    check_multiplication_overflow(const T & t, const U & u){
        typedef BOOST_TYPEOF_TPL(T() * U()) result_type;
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
    BOOST_TYPEOF_TPL(T() / U())
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
    BOOST_TYPEOF_TPL(T() / U())
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


} // numeric
} // boost

#endif // BOOST_NUMERIC__HPP
