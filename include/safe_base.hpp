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
#include <type_traits> // is_integral, enable_if
#include <iosfwd>
#include <cassert>

#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/and.hpp>

#include "checked.hpp"
#include "safe_common.hpp"
#include "concept/numeric.hpp"
#include "exception_policies.hpp"
#include "native.hpp"

#include "boost/concept/assert.hpp"

namespace boost {
namespace numeric {

/////////////////////////////////////////////////////////////////
// forward declarations to support friend function declarations
// in safe_base

template<
    class Stored,
    Stored Min,
    Stored Max,
    class P, // promotion polic
    class E  // exception policy
>
class safe_base;

template<
    class T,
    T Min,
    T Max,
    class P, // promotion polic
    class E  // exception policy
>
std::ostream & operator<<(
    std::ostream & os,
    const boost::numeric::safe_base<T, Min, Max, P, E> & t
);

template<
    class T,
    T Min,
    T Max,
    class P, // promotion polic
    class E  // exception policy
>
std::istream & operator>>(
    std::istream & is,
    boost::numeric::safe_base<T, Min, Max, P, E> & t
);

/////////////////////////////////////////////////////////////////
// Main implementation

template<
    class Stored,
    Stored Min,
    Stored Max,
    class P, // promotion polic
    class E  // exception policy
>
class safe_base {
    BOOST_CONCEPT_ASSERT((Integer<Stored>));
    BOOST_CONCEPT_ASSERT((PromotionPolicy<P>));
    BOOST_CONCEPT_ASSERT((ExceptionPolicy<E>));
public:
    Stored m_t;
private:
    friend
    std::ostream & operator<< <Stored, Min, Max, P, E> (
        std::ostream & os,
        const safe_base & t
    );
    friend
    std::istream & operator>> <Stored, Min, Max, P, E> (
        std::istream & is,
        safe_base & t
    );

    template<class T>
    SAFE_NUMERIC_CONSTEXPR bool validate(const T & t) const {
        return ! (
            boost::numeric::checked::greater_than(
                base_value(t),
                Max
            )
            ||
            boost::numeric::checked::less_than(
                base_value(t),
                Min
            )
        );
    }
public:
    ////////////////////////////////////////////////////////////
    // constructors
    // default constructor
    SAFE_NUMERIC_CONSTEXPR explicit safe_base() {}

    template<class T>
    SAFE_NUMERIC_CONSTEXPR safe_base(const T & t) :
        m_t(static_cast<Stored>(t))
    {
        if(! validate(t)){
            E::range_error("Invalid value");
        }
    }

    template<class T>
    SAFE_NUMERIC_CONSTEXPR safe_base(const safe_base & t) :
        m_t(static_cast<Stored>(t.m_t))
    {
    }

    template<
        class T,
        T MinT,
        T MaxT,
        class PT, // promotion polic
        class ET  // exception policy
    >
    SAFE_NUMERIC_CONSTEXPR safe_base(const safe_base<T, MinT, MaxT, PT, ET> & t){
        T tx = t.m_t;
        if(! validate(tx)){
            E::range_error("Invalid value");
        }
        m_t = tx;
    }

    // note: Rule of Five.  Don't specify custom destructor,
    // custom move, custom copy, custom assignment, custom
    // move assignment.  Let the compiler build the defaults.
public:
    /////////////////////////////////////////////////////////////////
    // casting operators for intrinsic integers
    // convert to any type which is not safe.  safe types need to be
    // excluded to prevent ambiguous function selection which
    // would otherwise occur
    template<
        class R,
        typename std::enable_if<
            !boost::numeric::is_safe<R>::value,
            int
        >::type = 0
    >
    SAFE_NUMERIC_CONSTEXPR operator R () const {
        checked_result<R> r = checked::cast<R>(m_t);
        if(r != checked_result<R>::exception_type::no_exception)
            E::range_error("conversion not possible");
        return static_cast<R>(r);
    }

    /////////////////////////////////////////////////////////////////
    // modification binary operators
    template<class T>
    safe_base & operator=(const T & rhs){
        if(! validate(rhs)){
            E::range_error(
                "Invalid value passed on assignment"
            );
        }
        m_t = static_cast<Stored>(rhs);
        return *this;
    }
    template<
        class T,
        T MinT,
        T MaxT,
        class PT, // promotion polic
        class ET  // exception policy
    >
    safe_base & operator=(const safe_base<T, MinT, MaxT, PT, ET> & rhs){
        if(! validate(rhs)){
            E::range_error("Invalid value");
        }
        m_t = rhs.m_t;
        return *this;
    }
    template<class T>
    safe_base & operator+=(const T & rhs){
        return *this = *this + rhs;
    }
    template<class T>
    safe_base & operator-=(const T & rhs){
        return *this = *this - rhs;
    }
    template<class T>
    safe_base & operator*=(const T & rhs){
        return *this = *this * rhs;
    }
    template<class T>
    safe_base & operator/=(const T & rhs){
        return *this = *this / rhs;
    }
    template<class T>
    safe_base & operator%=(const T & rhs){
        return *this = *this % rhs;
    }
    template<class T>
    safe_base & operator|=(const T & rhs){
        return *this = *this | rhs;
    }
    template<class T>
    safe_base & operator&=(const T & rhs){
        return *this = *this & rhs;
    }
    template<class T>
    safe_base & operator^=(const T & rhs){
        return *this = *this ^ rhs;
    }
    template<class T>
    safe_base & operator>>=(const T & rhs){
        return *this = *this >> rhs;
    }
    template<class T>
    safe_base & operator<<=(const T & rhs){
        return *this = *this << rhs;
    }
    // unary operators
    safe_base operator++(){
        return *this = *this + 1;
    }
    safe_base operator--(){
        return *this = *this - 1;
    }
    safe_base operator++(int){ // post increment
        safe_base old_t = *this;
        ++(*this);
        return old_t;
    }
    safe_base & operator--(int){ // post decrement
        safe_base old_t = *this;
        --(*this);
        return old_t;
    }
    safe_base operator-() const { // unary minus
        return *this = 0 - *this; // this will check for overflow
    }
    safe_base operator~() const {
        static_assert(
            std::numeric_limits<Stored>::is_signed,
            "Bitwise inversion of signed value is an error"
        );
        return *this = ~(m_t);
    }

/*

    /////////////////////////////////////////////////////////////////
    // logical operators

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

template<
    class T,
    T Min,
    T Max,
    class P,
    class E
>
struct is_safe<safe_base<T, Min, Max, P, E> > : public std::true_type
{};

template<
    class T,
    T Min,
    T Max,
    class P,
    class E
>
struct get_promotion_policy<safe_base<T, Min, Max, P, E> > {
    typedef P type;
};

template<
    class T,
    T Min,
    T Max,
    class P,
    class E
>
struct get_exception_policy<safe_base<T, Min, Max, P, E> > {
    typedef E type;
};

template<
    class T,
    T Min,
    T Max,
    class P,
    class E
>
struct base_type<safe_base<T, Min, Max, P, E> > {
    typedef T type;
};

template<
    class T,
    T Min,
    T Max,
    class P,
    class E
>
SAFE_NUMERIC_CONSTEXPR T base_value(
    const safe_base<T, Min, Max, P, E>  & st
) {
    return static_cast<T>(st.m_t);
}

} // numeric
} // boost

/////////////////////////////////////////////////////////////////
// numeric limits for safe<int> etc.

#include <limits>

namespace std {

template<
    class T,
    T Min,
    T Max,
    class P,
    class E
>
class numeric_limits<boost::numeric::safe_base<T, Min, Max, P, E> >
    : public std::numeric_limits<T>
{
    typedef boost::numeric::safe_base<T, Min, Max, P, E> SB;
public:
    // these expressions are not SAFE_NUMERIC_CONSTEXPR until C++14 so re-implement them here
    SAFE_NUMERIC_CONSTEXPR static SB min() noexcept {
        return SB(Min);
    }
    SAFE_NUMERIC_CONSTEXPR static SB max() noexcept {
        return SB(Max);
    }
};

} // std

#endif // BOOST_NUMERIC_SAFE_BASE_HPP
