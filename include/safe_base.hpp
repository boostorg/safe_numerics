#ifndef BOOST_NUMERIC_SAFE_BASE_HPP
#define BOOST_NUMERIC_SAFE_BASE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "checked.hpp"
#include "safe_compare.hpp"

#include <boost/utility/enable_if.hpp>
#include <boost/mpl/and.hpp>
#include <boost/type_traits/is_convertible.hpp>

#include <boost/mpl/sizeof.hpp>

namespace boost {
namespace numeric {

struct safe_tag {};

template<
    class Stored, 
    class Derived
>
class safe_base : public safe_tag {
    Derived & 
    derived() {
        return static_cast<Derived &>(*this);
    }
    const Derived & 
    derived() const {
        return static_cast<const Derived &>(*this);
    }
    template<class T>
    bool validate(const T & t) const {
        return derived().validate(t);
    }
    Stored m_t;
protected:
    ////////////////////////////////////////////////////////////
    // constructors
    // default constructor
    safe_base() {}

    // copy constructor 
    safe_base(const safe_base & t) :
        m_t(t.m_t)
    {}
    template<class T>
    safe_base(const T & t)
    {
        // verify that this is convertible to the storable type
        BOOST_STATIC_ASSERT(( boost::is_convertible<T, Stored>::value ));
        validate(t);
        m_t = t;
    }
    Stored & get_stored_value();

public:

    /////////////////////////////////////////////////////////////////
    // modification binary operators
    template<class T>
    Derived & operator=(const T & rhs){
        validate(rhs);
        m_t = rhs;
        return derived();
    }
    template<class T>
    Derived & operator+=(const T & rhs){
        // validate?
        m_t = derived() + rhs;
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
        return boost::numeric::safe_compare::less_than(m_t, rhs);
    }
    template<class U>
    bool operator>(const U & rhs) const {
        return boost::numeric::safe_compare::greater_than(m_t, rhs);
    }
    template<class U>
    bool operator==(const U & rhs) const {
        return boost::numeric::safe_compare::equal(m_t, rhs);
    }
    template<class U>
    bool inline operator!=(const U & rhs) const {
        return ! boost::numeric::safe_compare::equal(m_t,rhs);
    }
    template<class U>
    bool inline operator>=(const U & rhs) const {
        return ! boost::numeric::safe_compare::less_than(m_t, rhs);
    }
    template<class U>
    bool inline operator<=(const U & rhs) const {
        return ! boost::numeric::safe_compare::greater_than(m_t, rhs);
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

    template<class T, class U>
    struct no_subtraction_overflow_possible;

    // case 1 - no overflow possible

    template<class U>
    typename boost::enable_if<
        no_subtraction_overflow_possible<Stored, U>,
        decltype(Stored() - U())
    >::type
    inline operator-(const U & rhs) const {
        return m_t - rhs;
    }

    template<class U>
    typename boost::disable_if<
        no_subtraction_overflow_possible<Stored, U>,
        decltype(Stored() - U())
    >::type
    inline operator-(const U & rhs) const {
        return detail::check_subtraction_overflow<
            boost::numeric::is_signed<Stored>::value,
            boost::numeric::is_signed<U>::value
        >::subtract(m_t, boost::numeric::safe_cast<BOOST_TYPEOF_TPL(Stored() - U())>(rhs));
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
            boost::numeric::overflow("right shift of negative number undefined");
        typedef BOOST_TYPEOF_TPL(Stored() >> U()) result_type;
        if(rhs > boost::numeric::bits<Stored>::value)
            boost::numeric::overflow("conversion of negative value to unsigned");

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
            boost::numeric::overflow("right shift of negative number undefined");
        typedef BOOST_TYPEOF_TPL(Stored() >> U()) result_type;
        if(rhs > boost::numeric::bits<Stored>::value)
            boost::numeric::overflow("conversion of negative value to unsigned");
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
            boost::numeric::overflow("safe range unary negation overflow");
        return -m_t;
    }

    /////////////////////////////////////////////////////////////////
    // casting operators for intrinsic integers
    operator Stored () const {
        return m_t;
    }
};

} // numeric
} // boost

#endif // BOOST_NUMERIC_SAFE_BASE_HPP
