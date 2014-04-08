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
#include <boost/integer.hpp>

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
#include <boost/static_assert.hpp>
#include <boost/mpl/sizeof.hpp>

#include <ostream>
#include <istream>
#include <stdexcept>

// we could have used decltype and auto for C++11 but we've decided
// to use boost/typeof to be compatible with older compilers
#include <boost/typeof/typeof.hpp>

namespace boost {
namespace numeric {

namespace detail {
    //////////////////////////
    // addition implementation
    template<bool TS, bool US>
    struct check_addition_overflow{};

    // both arguments unsigned
    template<>
    struct check_addition_overflow<false, false> {
        template<class T, class U>
        static BOOST_TYPEOF_TPL(T() + U())
        add(const T & t, const U & u) {
            BOOST_AUTO_TPL(tmp, t + u);
            if(boost::numeric::safe_compare::less_than(tmp, t)
            || boost::numeric::safe_compare::less_than(tmp, u))
                 overflow("safe range addition result overflow");
            return tmp;
        }
    };
    // T unsigned, U signed
    template<>
    struct check_addition_overflow<false, true> {
        template<class T, class U>
        static BOOST_TYPEOF_TPL(T() + U())
        add(const T & t, const U & u){
            typedef BOOST_TYPEOF_TPL(T() + U()) result_type;
            if(boost::numeric::is_unsigned<result_type>::value){
                if(u < 0)
                    overflow("safe range left operand value altered");
                else
                    return check_addition_overflow<false, false>::add(
                        t,
                        static_cast<typename boost::make_unsigned<T>::type>(u)
                    );
            }
            else{
                if(u > 0){
                    auto tmp = t + u;
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
    struct check_addition_overflow<true, false> {
        template<class T, class U>
        static BOOST_TYPEOF_TPL(T() + U())
        add(const T & t, const U & u){
            typedef BOOST_TYPEOF_TPL(T() + U()) result_type;
            if(boost::numeric::is_unsigned<result_type>::value){
                if(t < 0)
                    overflow("safe range left operand value altered");
                else
                    return check_addition_overflow<false, false>::add(
                        static_cast<typename boost::make_unsigned<T>::type>(t),
                        u
                    );
            }
            else{
                if(t > 0){
                    auto tmp = t + u;
                    if(tmp < 0)
                        overflow("safe range addition result overflow");
                }
            }

            BOOST_AUTO_TPL(tmp, t + u);
            return tmp;
        }
    };
    // both arguments signed
    template<>
    struct check_addition_overflow<true, true> {
        template<class T, class U>
        static BOOST_TYPEOF_TPL(T() + U())
        add(const T & t, const U & u){
            if(t > 0 && u > 0){
                auto tmp = t + u;
                if(tmp < 0)
                    overflow("safe range addition result overflow");
                return tmp;
            }
            if(t < 0 && u < 0){
                auto tmp = t + u;
                if(tmp >= 0)
                    overflow("safe range addition result underflow");
                return tmp;
            }
            return t + u;
        }
    };

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
            /*
            if(t > 0 && u < 0){
                auto tmp = t - u;
                if(tmp < 0)
                    overflow("safe range subtraction out of range");
                return tmp;
            }
            else
            if(t < 0 && u > 0){
                auto tmp = t - u;
                if(tmp >= 0)
                    overflow("safe range subtraction out of range");
                return tmp;
            }
            */
            auto tmp = t - u;
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
                return check_subtraction_overflow<false, false>::subtract(
                    t,
                    static_cast<typename boost::make_unsigned<T>::type>(u)
                );
            }
            else{
                if(u > 0){
                    return check_subtraction_overflow<false, false>::subtract(
                        t,
                        static_cast<typename boost::make_unsigned<T>::type>(u)
                    );
                }
                else{
                    // u <= 0
                    auto tmp = t - u;
                    if(t < 0)
                        overflow("safe range subtraction result underflow");
                }
            }
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
                if(t < 0)
                    overflow("safe range left operand value altered");
                else
                    return check_subtraction_overflow<false, false>::subtract(
                        static_cast<result_type>(t),
                        u
                    );
            }
            else{
                return check_subtraction_overflow<true, true>::subtract(
                    t,
                    static_cast<result_type>(u)
                );
            }
            return t - u;
        }
    };

    template<bool TS, bool US>
    struct check_multiplication_overflow;

    // both arguments unsigned
    template<>
    struct check_multiplication_overflow<false, false> {
        template<class T, class U>
        static void invoke(const T & t, const U & u){
            char const * const msg = "safe range multiplication out of range";
            typedef boost::uintmax_t accumulator_type; 
            const int temp_bits = bits<accumulator_type>::value / 2;
            typedef typename boost::uint_t<temp_bits>::least temp_type;

            temp_type a = (static_cast<accumulator_type>(t) >> temp_bits);
            temp_type c = (static_cast<accumulator_type>(u) >> temp_bits);
            if(0 != a && 0 != c)
                overflow(msg);

            temp_type b = static_cast<temp_type>(t);
            if((static_cast<accumulator_type>(b) * static_cast<accumulator_type>(c) >> temp_bits) > 0)
                overflow(msg);

            temp_type d = static_cast<const temp_type>(u);
            if(0 != (static_cast<accumulator_type>(a) * static_cast<accumulator_type>(d) >> temp_bits))
                overflow(msg);
        }
    };
    // T unsigned, U signed
    template<>
    struct check_multiplication_overflow<false, true> {
        template<class T, class U>
        static void invoke(const T & t, const U & u){
            check_multiplication_overflow<false, false>::invoke(
                t, 
                u < 0 ? -u : u
            );
        }
    };
    // T signed, U unsigned
    template<>
    struct check_multiplication_overflow<true, false> {
        template<class T, class U>
        static void invoke(const T & t, const U & u){
            check_multiplication_overflow<false, false>::invoke(
                t < 0 ? -t : t,
                u 
            );
        }
    };
    // both arguments signed, signed
    template<>
    struct check_multiplication_overflow<true, true> {
        template<class T, class U>
        static void invoke(const T & t, const U & u){
            check_multiplication_overflow<false, false>::invoke(
                t < 0 ? -t : t,
                u < 0 ? -u : u
            );
        }
    };
}   // detail

template<
    class Stored, 
    class Derived
>
class safe_range_base {
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
        validate(t);
        m_t = static_cast<const Stored &>(t);
    }
    typedef Stored stored_type;
public:
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
        return validate(- m_t);
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
    // addition
    // case 1 - no overflow possible
    template<class U>
    typename boost::enable_if<
        typename boost::mpl::greater<
            typename boost::mpl::sizeof_< BOOST_TYPEOF_TPL(U() + Stored()) >,
            typename boost::mpl::max<
                boost::mpl::sizeof_<U>,
                boost::mpl::sizeof_<Stored>
            >::type
        >,
        BOOST_TYPEOF_TPL(U() + Stored())
    >::type
    inline operator+(const U & rhs) const {
        return m_t + rhs;
    }

    // case 2 - overflow possible - must be checked at run time
    template<class U>
    typename boost::disable_if<
        typename boost::mpl::greater<
            typename boost::mpl::sizeof_< BOOST_TYPEOF_TPL(U() + Stored()) >,
            typename boost::mpl::max<
                boost::mpl::sizeof_<U>,
                boost::mpl::sizeof_<Stored>
            >::type
        >,
        BOOST_TYPEOF_TPL(U() + Stored())
    >::type
    inline operator+(const U & rhs) const {
        return detail::check_addition_overflow<
            boost::numeric::is_signed<Stored>::value,
            boost::numeric::is_signed<U>::value
        >::add(m_t, rhs);
    }
    /////////////////////////////////////////////////////////////////
    // subtraction
    // case 1 - no overflow possible
    template<class U>
    typename boost::enable_if<
        typename boost::mpl::greater<
            typename boost::mpl::sizeof_< BOOST_TYPEOF_TPL(U() + Stored()) >,
            typename boost::mpl::max<
                boost::mpl::sizeof_<U>,
                boost::mpl::sizeof_<Stored>
            >::type
        >,
        BOOST_TYPEOF_TPL(U() + Stored())
    >::type
    inline operator-(const U & rhs) const {
        typedef typename subtraction_result_type<Stored, U>::type result_type;
        return m_t - rhs;
    }

    template<class U>
    typename boost::disable_if<
        typename boost::mpl::greater<
            typename boost::mpl::sizeof_< BOOST_TYPEOF_TPL(U() + Stored()) >,
            typename boost::mpl::max<
                boost::mpl::sizeof_<U>,
                boost::mpl::sizeof_<Stored>
            >::type
        >,
        BOOST_TYPEOF_TPL(U() + Stored())
    >::type
    inline operator-(const U & rhs) const {
        return detail::check_subtraction_overflow<
            boost::numeric::is_signed<Stored>::value,
            boost::numeric::is_signed<U>::value
        >::subtract(m_t, rhs);
    }
    /////////////////////////////////////////////////////////////////
    // multiplication
    template<class U>
    typename boost::enable_if<
        typename boost::mpl::less_equal<
            multiply_result_bits<Stored, U>,
            // note presumption that size(boost::uintmax) == size(boost::intmax)
            bits<boost::uintmax_t>
        >,
        typename multiply_result_type<Stored, U>::type
    >::type
    inline operator*(const U & rhs) const {
        typedef typename multiply_result_type<Stored, U>::type result_type;
        return static_cast<result_type>(m_t) * static_cast<result_type>(rhs);
    }

    // implement multiply larger numbers.  This is
    // intended to function for all combinations of
    // signed/unsigned types when the product exceeds
    // the maximum integer size
    template<class U>
    typename boost::enable_if<
        typename boost::mpl::greater<
            multiply_result_bits<Stored, U>, 
            bits<boost::uintmax_t>
        >,
        boost::uintmax_t
    >::type
    inline operator*(const U & rhs) const {
        detail::check_multiplication_overflow<
            boost::numeric::is_signed<Stored>::value,
            boost::numeric::is_signed<U>::value
        >::invoke(m_t, rhs);

        return static_cast<boost::uintmax_t>(m_t) * static_cast<boost::uintmax_t>(rhs);
    }

    /////////////////////////////////////////////////////////////////
    // division
    // simple case - default rules work
    template<class U>
    typename boost::enable_if<
        typename boost::mpl::less_equal<
            division_result_bits<Stored, U>, 
            bits<boost::uintmax_t>
        >,
        typename division_result_type<Stored, U>::type
    >::type
    inline operator/(const U & rhs) const {
        if(0 == rhs)
            throw std::domain_error("Divide by zero");
        return safe_cast<typename division_result_type<Stored, U>::type>(m_t / rhs);
    }

    // special case - possible overflow
    template<class U>
    typename boost::enable_if<
        typename boost::mpl::greater<
            division_result_bits<Stored, U>, 
            bits<boost::uintmax_t>
        >,
        typename division_result_type<Stored, U>::type
    >::type
    inline operator/(const U & rhs) const {
        if(0 == rhs)
            throw std::domain_error("Divide by zero");
        return safe_cast<typename division_result_type<Stored, U>::type>(m_t / rhs);
    }

    /////////////////////////////////////////////////////////////////
    // modulus
    template<class U>
    typename division_result_type<Stored, U>::type
    inline operator%(const U & rhs) const {
        if(0 == rhs)
            throw std::domain_error("Divide by zero");
        return safe_cast<typename division_result_type<Stored, U>::type>(m_t % rhs);
    }

    /////////////////////////////////////////////////////////////////
    // logical operators
    template<class U>
    typename logical_result_type<Stored, U>::type
    inline operator|(const U & rhs) const {
        typedef typename logical_result_type<Stored, U>::type result_type;
        return static_cast<result_type>(m_t) | static_cast<result_type>(rhs);
    }
    template<class U>
    typename logical_result_type<Stored, U>::type
    inline operator&(const U & rhs) const {
        typedef typename logical_result_type<Stored, U>::type result_type;
        return static_cast<result_type>(m_t) & static_cast<result_type>(rhs);
    }
    template<class U>
    typename logical_result_type<Stored, U>::type
    inline operator^(const U & rhs) const {
        typedef typename logical_result_type<Stored, U>::type result_type;
        return static_cast<result_type>(m_t) ^ static_cast<result_type>(rhs);
    }
    template<class U>
    Stored inline operator>>(const U & rhs) const {
        typedef typename logical_result_type<Stored, U>::type result_type;
        return static_cast<result_type>(m_t) >> static_cast<result_type>(rhs);
    }
    template<class U>
    Stored inline operator<<(const U & rhs) const {
        typedef typename logical_result_type<Stored, U>::type result_type;
        return static_cast<result_type>(m_t) << static_cast<result_type>(rhs);
    }

    /////////////////////////////////////////////////////////////////
    // casting operators for intrinsic integers
    operator stored_type () const {
        return m_t;
    }
};

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
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
operator<(const T & lhs, const safe_range_base<Stored, Derived> & rhs) {
    return rhs > lhs;
}
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
inline operator>(const T & lhs, const safe_range_base<Stored, Derived> & rhs) {
    return rhs < lhs;
}
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
inline operator==(const T & lhs, const safe_range_base<Stored, Derived> & rhs) {
    return rhs == lhs;
}
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
inline operator!=(const T & lhs, const safe_range_base<Stored, Derived> & rhs) {
    return rhs != rhs;
}
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
inline operator>=(const T & lhs, const safe_range_base<Stored, Derived> & rhs) {
    return rhs <= lhs;
}
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
inline operator<=(const T & lhs, const safe_range_base<Stored, Derived> & rhs) {
    return  rhs >= lhs;
}

// addition
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    typename addition_result_type<T, Stored>::type
>::type
inline operator+(const T & lhs, const safe_range_base<Stored, Derived> & rhs) {
    return rhs + lhs;
}

// subtraction
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    typename subtraction_result_type<T, Stored>::type
>::type
inline operator-(const T & lhs, const safe_range_base<Stored, Derived> & rhs) {
    typename subtraction_result_type<T, Stored>::type tmp;
    tmp = rhs - lhs;
    return - tmp;
}

// multiplication
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    typename multiply_result_type<T, Stored>::type
>::type
inline operator*(const T & lhs, const safe_range_base<Stored, Derived> & rhs) {
    return rhs * lhs;
}

// division
// special case - possible overflow
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    typename division_result_type<T, Stored>::type
>::type
inline operator/(const T & lhs, const safe_range_base<Stored, Derived> & rhs) {
    if(safe_compare::equal(0, rhs))
        throw std::domain_error("Divide by zero");
    return static_cast<
        typename division_result_type<T, Stored>::type
    >(lhs / static_cast<const Stored &>(rhs));
}

// modulus
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    typename division_result_type<T, Stored>::type
>::type
inline operator%(const T & lhs, const safe_range_base<Stored, Derived> & rhs) {
    if(safe_compare::equal(0, rhs))
        throw std::domain_error("Divide by zero");
    return static_cast<
        typename division_result_type<T, Stored>::type
    >(lhs % static_cast<const Stored &>(rhs));
}

// logical operators
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    typename multiply_result_type<T, Stored>::type
>::type
inline operator|(const T & lhs, const safe_range_base<Stored, Derived> & rhs) {
    return rhs | lhs;
}
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    typename logical_result_type<T, Stored>::type
>::type
inline operator&(const T & lhs, const safe_range_base<Stored, Derived> & rhs) {
    return rhs & lhs;
}
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    typename logical_result_type<T, Stored>::type
>::type
inline operator^(const T & lhs, const safe_range_base<Stored, Derived> & rhs) {
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

/////////////////////////////////////////////////////////////////
// safe_signed_range

template<
    boost::intmax_t MIN,
    boost::intmax_t MAX
>
class safe_signed_range : public
    safe_range_base<
        typename detail::signed_stored_type<MIN, MAX>::type, 
        safe_signed_range<MIN, MAX> 
    > 
{
    BOOST_STATIC_ASSERT_MSG(
        MIN < MAX,
        "minimum must be less than maximum"
    );
public:
    typedef typename boost::numeric::safe_range_base<
        typename detail::signed_stored_type<MIN, MAX>::type, 
        safe_signed_range<MIN, MAX> 
    > base;

    typedef typename detail::signed_stored_type<MIN, MAX>::type stored_type;
    template<class T>
    stored_type validate(const T & t) const {
        const boost::intmax_t tx = t;
        if(MAX < tx
        || MIN > tx
        )
            overflow("safe range out of range");
        return static_cast<stored_type>(t);
    }
    safe_signed_range(){}

    template<class T>
    safe_signed_range(const T & t) :
        base(t)
    {}
};

template<
    boost::intmax_t MIN,
    boost::intmax_t MAX
>
std::ostream & operator<<(std::ostream & os, const safe_signed_range<MIN, MAX> & t){
    return os << static_cast<const typename safe_signed_range<MIN, MAX>::stored_type &>(t);
}

template<
    boost::intmax_t MIN,
    boost::intmax_t MAX
>
std::istream & operator>>(std::istream & is, safe_signed_range<MIN, MAX> & t){
    typename safe_signed_range<MIN, MAX>::stored_type tx;
    is >> tx;
    t = tx;
    return is;
}

/////////////////////////////////////////////////////////////////
// safe_unsigned_range

template<
    boost::uintmax_t MIN,
    boost::uintmax_t MAX
>
class safe_unsigned_range : public
    safe_range_base<
        typename detail::unsigned_stored_type<MIN, MAX>::type, 
        safe_unsigned_range<MIN, MAX> 
    > 
{
    BOOST_STATIC_ASSERT_MSG(
        MIN < MAX,
        "minimum must be less than maximum"
    );
public:
    typedef typename boost::numeric::safe_range_base<
        typename detail::unsigned_stored_type<MIN, MAX>::type, 
        safe_unsigned_range<MIN, MAX> 
    > base;
    typedef typename detail::unsigned_stored_type<MIN, MAX>::type stored_type;
    template<class T>
    stored_type validate(const T & t) const {
        const boost::uintmax_t tx = t;
        if(MAX < tx)
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
    boost::uintmax_t MAX
>
std::ostream & operator<<(std::ostream & os, const safe_unsigned_range<MIN, MAX> & t){
    return os << static_cast<const typename safe_unsigned_range<MIN, MAX>::stored_type &>(t);
}

template<
    boost::uintmax_t MIN,
    boost::uintmax_t MAX
>
std::istream & operator>>(std::istream & is, safe_unsigned_range<MIN, MAX> & t){
    typename safe_unsigned_range<MIN, MAX>::stored_type tx;
    is >> tx;
    t = tx;
    return is;
}

} // numeric
} // boost

#endif // BOOST_NUMERIC_SAFE_RANGE_HPP
