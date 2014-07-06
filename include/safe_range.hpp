#ifndef BOOST_NUMERIC_SAFE_RANGE_HPP
#define BOOST_NUMERIC_SAFE_RANGE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "numeric.hpp"
#include "safe_compare.hpp"
#include "safe_cast.hpp"
#include "overflow.hpp"

#include <boost/limits.hpp>
#include <boost/concept_check.hpp>


//#include <boost/integer.hpp>

#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/make_signed.hpp>
#include <boost/type_traits/make_unsigned.hpp>

#include <boost/utility/enable_if.hpp>
#include <boost/mpl/min_max.hpp>
#include <boost/mpl/less_equal.hpp>
#include <boost/mpl/greater.hpp>
#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/sizeof.hpp>
#include <boost/mpl/times.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/void.hpp>
#include <boost/static_assert.hpp>

#include <ostream>
#include <istream>
#include <stdexcept>

// we could have used decltype and auto for C++11 but we've decided
// to use boost/typeof to be compatible with older compilers
#include <boost/typeof/typeof.hpp>

namespace boost {
namespace numeric {

namespace checked {
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

    template<class R, class T, class U>
    R add(const T & t, const U & u){
        return addition<boost::is_unsigned<T>::value, boost::is_unsigned<U>::value>
        ::template add<R>(
            t, u
        );
    }
}
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

struct safe_tag {};

template<
    class Stored, 
    class Derived,
    class PromotionPolicyType
>
class safe_range_base : public safe_tag {
    Derived & 
    derived() {
        return static_cast<Derived &>(*this);
    }
    const Derived & 
    derived() const {
        return static_cast<const Derived &>(*this);
    }
    template<class T>
    Stored validate(const T & t) const {
        return derived().validate(t);
    }
    Stored m_t;
protected:
    ////////////////////////////////////////////////////////////
    // constructors
    // default constructor
    safe_range_base() {}

    // copy constructor 
    safe_range_base(const safe_range_base & t) :
        m_t(t.m_t)
    {}
    template<class T>
    safe_range_base(const T & t)
    {
        // verify that this is convertible to the storable type
        BOOST_STATIC_ASSERT(( boost::is_convertible<T, Stored>::value ));
        m_t = validate(t);
    }
    Stored & get_stored_value();

public:
    struct PromotionPolicy {
        typedef PromotionPolicyType type;
    };

    /////////////////////////////////////////////////////////////////
    // modification binary operators
    template<class T>
    Derived & operator=(const T & rhs){
        m_t = validate(rhs);
        return derived();
    }
    template<class T>
    Derived & operator+=(const T & rhs){
        *this = *this + rhs;
        return derived();
    }
    template<class T>
    Derived & operator-=(const T & rhs){
        *this = *this - rhs;
        return derived();
    }
    template<class T>
    Derived & operator*=(const T & rhs){
        *this = *this * rhs;
        return derived();
    }
    template<class T>
    Derived & operator/=(const T & rhs){
        *this = *this / rhs;
        return derived();
    }
    template<class T>
    Derived & operator%=(const T & rhs){
        *this = *this % rhs;
        return derived();
    }
    template<class T>
    Derived & operator|=(const T & rhs){
        *this = *this | rhs;
        return derived();
    }
    template<class T>
    Derived & operator&=(const T & rhs){
        *this = *this & rhs;
        return derived();
    }
    template<class T>
    Derived & operator^=(const T & rhs){
        *this = *this * rhs;
        return derived();
    }
    template<class T>
    Derived & operator>>=(const T & rhs){
        *this = *this >> rhs;
        return derived();
    }
    template<class T>
    Derived & operator<<=(const T & rhs){
        *this = *this << rhs;
        return derived();
    }
    // unary operators
    Derived operator++(){
        *this = *this + 1;
        return derived();
    }
    Derived operator--(){
        *this = *this - 1;
        return derived();
    }
    Derived operator++(int){
        Derived rvalue = *this;
        m_t = validate(*this + 1);
        return rvalue;
    }
    Derived & operator--(int){
        Derived rvalue = *this;
        m_t = validate(*this - 1);
        return rvalue;
    }
    Derived operator-() const {
        return validate(
            check_unary_negation_overflow(m_t)
        );
    }
    Derived operator~() const {
        return validate(~m_t);
    }

    /////////////////////////////////////////////////////////////////
    // comparison operators
    template<class U>
    bool operator<(const U & rhs) const {
        return safe_compare::less_than(m_t, rhs);
    }
    template<class U>
    bool operator>(const U & rhs) const {
        return safe_compare::greater_than(m_t, rhs);
    }
    template<class U>
    bool operator==(const U & rhs) const {
        return safe_compare::equal(m_t, rhs);
    }
    template<class U>
    bool inline operator!=(const U & rhs) const {
        return ! safe_compare::equal(m_t,rhs);
    }
    template<class U>
    bool inline operator>=(const U & rhs) const {
        return ! safe_compare::less_than(m_t, rhs);
    }
    template<class U>
    bool inline operator<=(const U & rhs) const {
        return ! safe_compare::greater_than(m_t, rhs);
    }

    /////////////////////////////////////////////////////////////////
    // subtraction

    template<class T, class U>
    struct no_subtraction_overflow_possible : public
         boost::mpl::and_<
            typename boost::mpl::greater<
                typename boost::mpl::sizeof_< BOOST_TYPEOF_TPL(Stored() - U()) >,
                typename boost::mpl::max<
                    boost::mpl::sizeof_<U>,
                    boost::mpl::sizeof_<Stored>
                >::type
            >,
            boost::numeric::is_signed<BOOST_TYPEOF_TPL(Stored() - U())>
        >
    {};

    // case 1 - no overflow possible

    template<class U>
    typename boost::enable_if<
        no_subtraction_overflow_possible<Stored, U>,
        BOOST_TYPEOF_TPL(Stored() - U())
    >::type
    inline operator-(const U & rhs) const {
        return m_t - rhs;
    }

    template<class U>
    typename boost::disable_if<
        no_subtraction_overflow_possible<Stored, U>,
        BOOST_TYPEOF_TPL(Stored() - U())
    >::type
    inline operator-(const U & rhs) const {
        return detail::check_subtraction_overflow<
            boost::numeric::is_signed<Stored>::value,
            boost::numeric::is_signed<U>::value
        >::subtract(m_t, safe_cast<BOOST_TYPEOF_TPL(Stored() - U())>(rhs));
    }
    /////////////////////////////////////////////////////////////////
    // multiplication

    template<class U>
    BOOST_TYPEOF_TPL(U() * Stored())
    inline operator*(const U & rhs) const {
        return detail::check_multiplication_overflow(m_t, rhs);
    }

    /////////////////////////////////////////////////////////////////
    // division
    template<class U>
    BOOST_TYPEOF_TPL(U() / Stored())
    inline operator/(const U & rhs) const {
        return detail::check_division_overflow(m_t, rhs);
    }

    /////////////////////////////////////////////////////////////////
    // modulus
    template<class U>
    BOOST_TYPEOF_TPL(Stored() % U())
    inline operator%(const U & rhs) const {
        if(0 == rhs)
            throw std::domain_error("Divide by zero");
        return detail::check_modulus_overflow(m_t, rhs);
    }

    /////////////////////////////////////////////////////////////////
    // logical operators
    template<class U>
    BOOST_TYPEOF_TPL(Stored() | U())
    inline operator|(const U & rhs) const {
        // verify that U is an integer type
        BOOST_STATIC_ASSERT_MSG(
            std::numeric_limits<U>::is_integer,
            "right hand side is not an integer type"
        );
        return m_t | rhs;
    }
    template<class U>
    BOOST_TYPEOF_TPL(Stored() & U())
    inline operator&(const U & rhs) const {
        // verify that U is an integer type
        BOOST_STATIC_ASSERT_MSG(
            std::numeric_limits<U>::is_integer,
            "right hand side is not an integer type"
        );
        return m_t & rhs;
    }
    template<class U>
    BOOST_TYPEOF_TPL(Stored() ^ U())
    inline operator^(const U & rhs) const {
        // verify that U is an integer type
        BOOST_STATIC_ASSERT_MSG(
            std::numeric_limits<U>::is_integer,
            "right hand side is not an integer type"
        );
        return m_t ^ rhs;
    }
    template<class U>
    Stored inline operator>>(const U & rhs) const {
        // verify that U is an integer type
        BOOST_STATIC_ASSERT_MSG(
            std::numeric_limits<U>::is_integer,
            "right hand side is not an integer type"
        );
        if(m_t < 0)
            overflow("right shift of negative number undefined");
        typedef BOOST_TYPEOF_TPL(Stored() >> U()) result_type;
        if(rhs > boost::numeric::bits<Stored>::value)
            overflow("conversion of negative value to unsigned");

        return m_t >> rhs;
    }
    template<class U>
    Stored inline operator<<(const U & rhs) const {
        // verify that U is an integer type
        BOOST_STATIC_ASSERT_MSG(
            std::numeric_limits<U>::is_integer,
            "right hand side is not an integer type"
        );
        if(m_t < 0)
            overflow("right shift of negative number undefined");
        typedef BOOST_TYPEOF_TPL(Stored() >> U()) result_type;
        if(rhs > boost::numeric::bits<Stored>::value)
            overflow("conversion of negative value to unsigned");
        return m_t << rhs;
    }

    ////////////////////////////////
    // unary negation implementation
    Stored
    operator-(){
        // this makes no sense for unsigned types
        BOOST_STATIC_ASSERT((boost::numeric::is_signed<Stored>::value));
        // the most common situation would be doing something like
        // boost::uint8_t x = -128;
        // ...
        // --x;
        if(boost::integer_traits<Stored>::const_max == m_t)
            overflow("safe range unary negation overflow");
        return -m_t;
    }

    /////////////////////////////////////////////////////////////////
    // casting operators for intrinsic integers
    operator Stored () const {
        return m_t;
    }
};

template<class T>
struct is_safe : public
    boost::is_convertible<T &, const boost::numeric::safe_tag &>
{};

template<class T>
struct get_policy_t : public
    T::PromotionPolicy
{};

template<class T, class U>
struct get_policy {
    // verify that at least one is a safe type
    BOOST_STATIC_ASSERT((
        boost::mpl::or_<
            is_safe<T>,
            is_safe<U>
        >::value
    ));

    typedef typename boost::mpl::if_<
        is_safe<T>,
        typename boost::numeric::get_policy_t<T>,
        boost::mpl::identity<boost::mpl::void_>
    >::type::type policy_t;

    //typedef typename boost::mpl::print<policy_t>::type t0;

    typedef typename boost::mpl::if_<
        is_safe<U>,
        typename boost::numeric::get_policy_t<U>,
        boost::mpl::identity<boost::mpl::void_>
    >::type::type policy_u;

    //typedef typename boost::mpl::print<policy_u>::type t1;

    // if both types are safe, the policies have to be the same!
    BOOST_STATIC_ASSERT((
        boost::mpl::if_<
            boost::mpl::and_<
                is_safe<T>,
                is_safe<U>
            >,
            typename boost::is_same<policy_t, policy_u>,
            boost::mpl::true_
        >::type::value
    ));

    typedef typename boost::mpl::if_<
        is_safe<T>,
        policy_t,
    typename boost::mpl::if_<
        is_safe<U>,
        policy_u,
    boost::mpl::void_
    >::type >::type type;

    // quadriple check all of the above
    BOOST_STATIC_ASSERT((
        boost::mpl::not_<boost::is_same<boost::mpl::void_, type> >::value
    ));

    // typedef typename boost::mpl::print<type>::type t2;
};

template<class T>
struct safe_base_type {
    typedef typename T::stored_type type;
};

template<class T>
struct base_type {
    typedef typename boost::mpl::if_<
        is_safe<T>,
        safe_base_type<T>,
        boost::mpl::identity<T>
    >::type tx;
    typedef typename tx::type type;
};

template<class T, class U>
struct addition_result {
    typedef typename get_policy<T, U>::type policy;
    typedef typename boost::mpl::print<policy>::type t0;
    typedef typename policy::template addition_result<T, U>::type type;
};

template<class T, class U>
typename addition_result<T, U>::type
inline operator+(const T & t, const U & u){
    typedef typename addition_result<T, U>::type type;
    return checked::add<typename addition_result<T, U>::type>(
        static_cast<const typename base_type<T>::type &>(t),
        static_cast<const typename base_type<U>::type &>(u)
    );
}

/////////////////////////////////////////////////////////////////
// Note: the following global operators will be only found via 
// argument dependent lookup.  So they won't conflict any
// other global operators for types in namespaces other than
// boost::numeric

// These should catch things like U < safe_range_base<...> and implement them
// as safe_range_base<...> >= U which should be handled above.

/////////////////////////////////////////////////////////////////
// binary operators

// comparison operators
template<class T, class Stored, class Derived, class PromotionPolicy>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
operator<(const T & lhs, const safe_range_base<Stored, Derived, PromotionPolicy> & rhs) {
    return rhs > lhs;
}
template<class T, class Stored, class Derived, class PromotionPolicy>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
inline operator>(const T & lhs, const safe_range_base<Stored, Derived, PromotionPolicy> & rhs) {
    return rhs < lhs;
}
template<class T, class Stored, class Derived, class PromotionPolicy>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
inline operator==(const T & lhs, const safe_range_base<Stored, Derived, PromotionPolicy> & rhs) {
    return rhs == lhs;
}
template<class T, class Stored, class Derived, class PromotionPolicy>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
inline operator!=(const T & lhs, const safe_range_base<Stored, Derived, PromotionPolicy> & rhs) {
    return rhs != rhs;
}
template<class T, class Stored, class Derived, class PromotionPolicy>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
inline operator>=(const T & lhs, const safe_range_base<Stored, Derived, PromotionPolicy> & rhs) {
    return rhs <= lhs;
}
template<class T, class Stored, class Derived, class PromotionPolicy>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
inline operator<=(const T & lhs, const safe_range_base<Stored, Derived, PromotionPolicy> & rhs) {
    return  rhs >= lhs;
}

#if 0
// addition
template<class T, class Stored, class Derived, class PromotionPolicy>
typename boost::enable_if<
    boost::is_integral<T>, // native integer type
    //BOOST_TYPEOF_TPL(T() + Stored())
    typename PromotionPolicy::template addition_result<T, Stored>::type
>::type
inline operator+(
    const T & lhs,
    const safe_range_base<Stored, Derived, PromotionPolicy> & rhs
){
    return rhs + lhs;
}
#endif

// subtraction
template<class T, class Stored, class Derived, class PromotionPolicy>
typename boost::enable_if<
    boost::is_integral<T>,  // native integer type
    BOOST_TYPEOF_TPL(T() - Stored())
>::type
inline operator-(const T & lhs, const safe_range_base<Stored, Derived, PromotionPolicy> & rhs) {
    BOOST_TYPEOF_TPL(T() - Stored()) tmp = rhs - lhs;
    return - tmp;
}

// multiplication
template<class T, class Stored, class Derived, class PromotionPolicy>
typename boost::enable_if<
    boost::is_integral<T>,
    BOOST_TYPEOF_TPL(T() * Stored())
>::type
inline operator*(const T & lhs, const safe_range_base<Stored, Derived, PromotionPolicy> & rhs) {
    return rhs * lhs;
}

// division
// special case - possible overflow
template<class T, class Stored, class Derived, class PromotionPolicy>
typename boost::enable_if<
    boost::is_integral<T>,
    BOOST_TYPEOF_TPL(T() / Stored())
>::type
inline operator/(const T & lhs, const safe_range_base<Stored, Derived, PromotionPolicy> & rhs) {
    if(safe_compare::equal(0, rhs))
        throw std::domain_error("Divide by zero");
    return static_cast<
        BOOST_TYPEOF_TPL(T() / Stored())
    >(lhs / static_cast<const Stored &>(rhs));
}

// modulus
template<class T, class Stored, class Derived, class PromotionPolicy>
typename boost::enable_if<
    boost::is_integral<T>,
    BOOST_TYPEOF_TPL(T() % Stored())
>::type
inline operator%(const T & lhs, const safe_range_base<Stored, Derived, PromotionPolicy> & rhs) {
    if(safe_compare::equal(0, rhs))
        throw std::domain_error("Divide by zero");
    return static_cast<
        BOOST_TYPEOF_TPL(T() % Stored())
    >(lhs % static_cast<const Stored &>(rhs));
}

// logical operators
template<class T, class Stored, class Derived, class PromotionPolicy>
typename boost::enable_if<
    boost::is_integral<T>,
    typename multiply_result_type<T, Stored>::type
>::type
inline operator|(const T & lhs, const safe_range_base<Stored, Derived, PromotionPolicy> & rhs) {
    return rhs | lhs;
}
template<class T, class Stored, class Derived, class PromotionPolicy>
typename boost::enable_if<
    boost::is_integral<T>,
    BOOST_TYPEOF_TPL(T() & Stored())
>::type
inline operator&(const T & lhs, const safe_range_base<Stored, Derived, PromotionPolicy> & rhs) {
    return rhs & lhs;
}
template<class T, class Stored, class Derived, class PromotionPolicy>
typename boost::enable_if<
    boost::is_integral<T>,
    BOOST_TYPEOF_TPL(T() ^ Stored())
>::type
inline operator^(const T & lhs, const safe_range_base<Stored, Derived, PromotionPolicy> & rhs) {
    return rhs ^ lhs;
}

/////////////////////////////////////////////////////////////////
// higher level types implemented in terms of safe_range_base

namespace detail {
    template<
        boost::intmax_t MIN,
        boost::intmax_t MAX
    >
    struct signed_stored_type {
        // double check that MIN < MAX
        typedef typename boost::int_t<
            boost::mpl::max<
                typename boost::numeric::detail::log<MIN, 2>,
                typename boost::numeric::detail::log<MAX, 2>
            >::type::value
        >::least type;
    };
    template<
        boost::uintmax_t MIN,
        boost::uintmax_t MAX
    >
    struct unsigned_stored_type {
        // calculate max(abs(MIN, MAX))
        typedef typename boost::uint_t<
            boost::mpl::max<
                typename boost::numeric::detail::ulog<MIN, 2>,
                typename boost::numeric::detail::ulog<MAX, 2>
            >::type::value
        >::least type;
    };
} // detail
} // numeric
} // boost

#include "native.hpp" // boost::promotion::native

namespace boost {
namespace numeric {

/////////////////////////////////////////////////////////////////
// safe_signed_range

template<
    boost::intmax_t MIN,
    boost::intmax_t MAX,
    typename PromotionPolicy = boost::numeric::promotion::native
>
class safe_signed_range : public
    safe_range_base<
        typename detail::signed_stored_type<MIN, MAX>::type, 
        safe_signed_range<MIN, MAX, PromotionPolicy>,
        PromotionPolicy
    > 
{
    BOOST_STATIC_ASSERT_MSG(
        MIN < MAX,
        "minimum must be less than maximum"
    );
public:
    typedef typename boost::numeric::safe_range_base<
        typename detail::signed_stored_type<MIN, MAX>::type, 
        safe_signed_range<MIN, MAX, PromotionPolicy>,
        PromotionPolicy
    > base;

    typedef typename detail::signed_stored_type<MIN, MAX>::type stored_type;
    template<class T>
    stored_type validate(const T & t) const {
        if(safe_compare::less_than(t, MIN)
        || safe_compare::greater_than(t, MAX))
            overflow("safe range out of range");
        return static_cast<stored_type>(t);
    }
    safe_signed_range() :
        base()
    {}

    template<class T>
    safe_signed_range(const T & t) :
        base(t)
    {}
};

template<
    boost::intmax_t MIN,
    boost::intmax_t MAX,
    typename PromotionPolicy
>
std::ostream & operator<<(
    std::ostream & os,
    const safe_signed_range<MIN, MAX, PromotionPolicy> & t
){
    return os << t.get_stored_value();
}

template<
    boost::intmax_t MIN,
    boost::intmax_t MAX,
    typename PromotionPolicy
>
std::istream & operator>>(
    std::istream & is,
    safe_signed_range<MIN, MAX, PromotionPolicy> & t
){
    return is >> t.get_stored_value();
}

/////////////////////////////////////////////////////////////////
// safe_unsigned_range

template<
    boost::uintmax_t MIN,
    boost::uintmax_t MAX,
    typename PromotionPolicy = boost::numeric::promotion::native
>
class safe_unsigned_range : public
    safe_range_base<
        typename detail::unsigned_stored_type<MIN, MAX>::type,
        safe_unsigned_range<MIN, MAX, PromotionPolicy>,
        PromotionPolicy
    >
{
    BOOST_STATIC_ASSERT_MSG(
        MIN < MAX,
        "minimum must be less than maximum"
    );
public:
    typedef typename boost::numeric::safe_range_base<
        typename detail::unsigned_stored_type<MIN, MAX>::type, 
        safe_unsigned_range<MIN, MAX, PromotionPolicy>,
        PromotionPolicy
    > base;
    typedef typename detail::unsigned_stored_type<MIN, MAX>::type stored_type;
    template<class T>
    stored_type validate(const T & t) const {
        if(safe_compare::less_than(t, MIN)
        || safe_compare::greater_than(t, MAX))
            overflow("safe range out of range");
        return static_cast<stored_type>(t);
    }
    safe_unsigned_range(){}

    template<class T>
    safe_unsigned_range(const T & t) :
        base(t)
    {}
};

template<
    boost::uintmax_t MIN,
    boost::uintmax_t MAX,
    typename PromotionPolicy
>
std::ostream & operator<<(
    std::ostream & os,
    const safe_unsigned_range<MIN, MAX, PromotionPolicy> & t
){
    return os << t.get_stored_value();
}

template<
    boost::uintmax_t MIN,
    boost::uintmax_t MAX,
    typename PromotionPolicy
>
std::istream & operator>>(
    std::istream & is,
    safe_unsigned_range<MIN, MAX, PromotionPolicy> & t
){
    return is >> t.get_stored_value();
}

} // numeric
} // boost

#endif // BOOST_NUMERIC_SAFE_RANGE_HPP
