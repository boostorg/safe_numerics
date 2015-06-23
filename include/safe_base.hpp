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

#include <limits>
#include <type_traits> // is_integral

#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/and.hpp>

#include "checked.hpp"
#include "safe_common.hpp"

namespace boost {
namespace numeric {

template<
    class Stored,
    class Derived,
    class P, // promotion polic
    class E  // exception policy
>
class safe_base {
    SAFE_NUMERIC_CONSTEXPR const Derived &
    derived() const {
        return static_cast<const Derived &>(*this);
    }
    template<class T>
    SAFE_NUMERIC_CONSTEXPR bool validate(const T & t) const {
        return ! (
            boost::numeric::checked::greater_than(t, base_value(Derived::max()))
            && boost::numeric::checked::less_than(t, base_value(Derived::min()))
        );
    }
    Stored m_t;
protected:
    // note: Rule of Three.  Don't specify custom move, copy etc.
    ////////////////////////////////////////////////////////////
    // constructors
    // default constructor
    SAFE_NUMERIC_CONSTEXPR safe_base() {}

    // copy constructor 
    SAFE_NUMERIC_CONSTEXPR safe_base(const safe_base & t) :
        m_t(t.m_t)
    {}
    template<class T>
    SAFE_NUMERIC_CONSTEXPR safe_base(T & t) :
        m_t(static_cast<Stored>(t))
    {
        // verify that this is convertible to the storable type
        static_assert(
            std::is_convertible<T, Stored>::value,
            "Constructor argument is convertible to the storable type"
        );
        if(! validate(t)){
            E::range_error("Invalid value");
        }
    }

public:
    /////////////////////////////////////////////////////////////////
    // casting operators for intrinsic integers
    template<class R>
    explicit SAFE_NUMERIC_CONSTEXPR operator R () const {
        return static_cast<R>(m_t);
    }
    explicit SAFE_NUMERIC_CONSTEXPR operator const Stored & () const {
        return m_t;
    }
    // used to implement stream i/o operators
    Stored & get_stored_value() {
        return m_t;
    }
    SAFE_NUMERIC_CONSTEXPR const Stored & get_stored_value() const {
        return m_t;
    }
    
    /////////////////////////////////////////////////////////////////
    // modification binary operators
    template<class T>
    Derived & operator=(const T & rhs){
        if(! derived().validate(rhs)){
            E::range_error(
                "Invalid value passed on assignment"
            );
        }
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
        return static_cast<Derived &>(*this) = static_cast<Derived &>(*this) - rhs;
    }
    template<class T>
    Derived & operator*=(const T & rhs){
        return static_cast<Derived &>(*this) = static_cast<Derived &>(*this) * rhs;
        return derived();
    }
    template<class T>
    Derived & operator/=(const T & rhs){
        return static_cast<Derived &>(*this) = static_cast<Derived &>(*this) / rhs;
        return derived();
    }
    template<class T>
    Derived & operator%=(const T & rhs){
        return static_cast<Derived &>(*this) = static_cast<Derived &>(*this) % rhs;
        return derived();
    }
    template<class T>
    Derived & operator|=(const T & rhs){
        return static_cast<Derived &>(*this) = static_cast<Derived &>(*this) | rhs;
        return derived();
    }
    template<class T>
    Derived & operator&=(const T & rhs){
        return static_cast<Derived &>(*this) = static_cast<Derived &>(*this) & rhs;
        return derived();
    }
    template<class T>
    Derived & operator^=(const T & rhs){
        return static_cast<Derived &>(*this) = static_cast<Derived &>(*this) * rhs;
        return derived();
    }
    template<class T>
    Derived & operator>>=(const T & rhs){
        return static_cast<Derived &>(*this) = static_cast<Derived &>(*this) >> rhs;
        return derived();
    }
    template<class T>
    Derived & operator<<=(const T & rhs){
        return static_cast<Derived &>(*this) = static_cast<Derived &>(*this) << rhs;
        return derived();
    }
    // unary operators
    Derived operator++(){
        // this checks for overflow
        return static_cast<Derived &>(*this) = static_cast<Derived &>(*this) + 1;
        return derived();
    }
    Derived operator--(){
        // this checks for overflow
        return static_cast<Derived &>(*this) = static_cast<Derived &>(*this) - 1;
    }
    Derived operator++(int){ // post increment
        Stored t = m_t;
        if(! validate(*this + 1)){
            E::overflow_error("Overflow on increment");
        }
        ++t;
        return derived();
    }
    Derived & operator--(int){ // post decrement
        Stored t = m_t;
        if(! validate(*this - 1)){
            E::overflow_error("Overflow on increment");
        }
        --t;
        return derived();
    }
    Derived operator-() const { // unary minus
        static_assert(
            std::numeric_limits<Stored>::is_signed,
            "Application of unary minus to unsigned value is an error"
        );
        *this = 0 - *this; // this will check for overflow
        return derived();
    }
    Derived operator~() const {
        static_assert(
            std::numeric_limits<Stored>::is_signed,
            "Bitwise inversion of unsigned value is an error"
        );
        if(! validate(~m_t)){
            E::overflow_error("Overflow on increment");
        }
        return derived();
    }

/*

    /////////////////////////////////////////////////////////////////
    // logical operators
    template<class U>
    decltype(Stored() | U())
    inline operator|(const U & rhs) const {
        // verify that U is an integer type
        static_assert(
            std::numeric_limits<U>::is_integer,
            "right hand side is not an integer type"
        );
        return m_t | rhs;
    }
    template<class U>
    decltype(Stored() & U())
    inline operator&(const U & rhs) const {
        // verify that U is an integer type
        static_assert(
            std::numeric_limits<U>::is_integer,
            "right hand side is not an integer type"
        );
        return m_t & rhs;
    }
    template<class U>
    decltype(Stored() ^ U())
    inline operator^(const U & rhs) const {
        // verify that U is an integer type
        static_assert(
            std::numeric_limits<U>::is_integer,
            "right hand side is not an integer type"
        );
        return m_t ^ rhs;
    }
    template<class U>
    Stored inline operator>>(const U & rhs) const {
        // verify that U is an integer type
        static_assert(
            std::numeric_limits<U>::is_integer,
            "right hand side is not an integer type"
        );
        if(m_t < 0)
            boost::numeric::overflow("right shift of negative number undefined");
        typedef decltype(Stored() >> U()) result_type;
        if(rhs > boost::numeric::bits<Stored>::value)
            boost::numeric::overflow("conversion of negative value to unsigned");

        return m_t >> rhs;
    }
    template<class U>
    Stored inline operator<<(const U & rhs) const {
        // verify that U is an integer type
        static_assert(
            std::numeric_limits<U>::is_integer,
            "right hand side is not an integer type"
        );
        if(m_t < 0)
            boost::numeric::overflow("right shift of negative number undefined");
        typedef decltype(Stored() >> U()) result_type;
        if(rhs > boost::numeric::bits<Stored>::value)
            boost::numeric::overflow("conversion of negative value to unsigned");
        return m_t << rhs;
    }

*/

};

} // numeric
} // boost

#endif // BOOST_NUMERIC_SAFE_BASE_HPP
