#ifndef BOOST_NUMERIC_INTERVAL_HPP
#define BOOST_NUMERIC_INTERVAL_HPP

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
#include <cassert>
#include <type_traits>
#include <utility>

#include <boost/logic/tribool.hpp>


#include "checked_result.hpp"
#include "checked.hpp"

// from stack overflow
// http://stackoverflow.com/questions/23815138/implementing-variadic-min-max-functions

namespace boost {
namespace numeric {

template <typename T>
constexpr const checked_result<T> & vmin(
    const checked_result<T> & a,
    const checked_result<T> & b
)      //this is an overload, not specialization
{
    return
    (! a.no_exception()) ?
        a :
    (! b.no_exception()) ?
        b :
    (a < b) ? a : b;
}

template <typename T, typename ... Ts>
constexpr const checked_result<T> & vmin(
    const checked_result<T> & a,
    const checked_result<T> & b,
    const checked_result<Ts> & ... vs
){
    return vmin(vmin(a, b), vs ...);
}

template <typename T>
constexpr const checked_result<T> & vmax(
    const checked_result<T> & a,
    const checked_result<T> & b
)      //this is an overload, not specialization
{
    return
    (! a.no_exception()) ?
        a :
    (! b.no_exception()) ?
        b :
    (a < b) ? b : a;
}

template <typename T, typename ... Ts>
constexpr const checked_result<T> & vmax(
    const checked_result<T> & a,
    const checked_result<T> & b,
    const checked_result<Ts> & ... vs
){
    return vmax(vmax(a, b), vs ...);
}

/*
template <typename T>
constexpr const T & vmax(const T & a, const T & b)      //this is an overload, not specialization
{
   return (a < b) ? b : a;
}

template <typename T, typename ... Ts>
constexpr const T & vmax(const T & a, const T & b, const Ts & ... vs)
{
    return vmax(vmax(a, b), vs ...);
}

template <typename T>
constexpr bool is_valid(){
    return true;
}

template <typename T, typename ... Ts>
constexpr bool is_valid(const checked_result<T> & t, const checked_result<Ts> & ... ts){
    return t.no_exception() ?
        false
    :
        is_valid(ts ...);
}
*/

template<typename R>
struct interval {
    static_assert(
        std::is_literal_type< checked_result<R> >::value,
        "is literal type"
    );

    const checked_result<R> l;
    const checked_result<R> u;

/*
    SAFE_NUMERIC_CONSTEXPR checked_result<R> get_l() const {
        return l;
    }
    SAFE_NUMERIC_CONSTEXPR checked_result<R> get_u() const {
        return u;
    }
    template<typename T, typename U>
    SAFE_NUMERIC_CONSTEXPR interval(const T & lower, const U & upper) :
        l(checked::cast<R>(lower)),
        u(checked::cast<R>(upper))
    {}
    SAFE_NUMERIC_CONSTEXPR interval(
        const R & lower,
        const R & upper
    ) :
        l(checked_result<R>(lower)),
        u(checked_result<R>(upper))
    {}
    SAFE_NUMERIC_CONSTEXPR interval(const interval<R> & rhs) :
        l(rhs.l),
        u(rhs.u)
    {}
    template<class T>
    SAFE_NUMERIC_CONSTEXPR interval(
        const T & lower,
        const T & upper
    ) :
        l(checked_result<T>(lower)),
        u(checked_result<T>(upper))
    {}
    template<typename T, typename U>
    SAFE_NUMERIC_CONSTEXPR interval(const T & lower, const U & upper) :
        l(checked::cast<R>(lower)),
        u(checked::cast<R>(upper))
    {}
*/
    template<typename T>
    SAFE_NUMERIC_CONSTEXPR interval(const T & lower, const T & upper) :
        l(checked::cast<R>(lower)),
        u(checked::cast<R>(upper))
    {}
    template<class T>
    SAFE_NUMERIC_CONSTEXPR interval(const interval<T> & rhs) :
        l(rhs.l),
        u(rhs.u)
    {}
    template<class T>
    SAFE_NUMERIC_CONSTEXPR interval(
        const checked_result<T> & lower,
        const checked_result<T> & upper
    ) :
        //l(checked::cast<R>(lower)),
        l(lower),
        //u(checked::cast<R>(upper))
        u(upper)
    {}

    SAFE_NUMERIC_CONSTEXPR interval() :
        l(std::numeric_limits<R>::min()),
        u(std::numeric_limits<R>::max())
    {}
    SAFE_NUMERIC_CONSTEXPR bool no_exception() const {
        return l.no_exception() && u.no_exception();
    }
    // return true if this interval contains every point found in some
    // other inteval t
    template<typename T>
    SAFE_NUMERIC_CONSTEXPR bool includes(const interval<T> & t) const {
        // note very tricky algebra here.  the <= and >= operators
        // on checked_result yield tribool.  If either argument is an exception
        // condition, he result is indeterminate.  The result of && on two
        // tribools is indeterminant if either is indeterminate.
        return l <= t.l && u >= t.u ;
    }
};

template<typename R, typename T, typename U>
SAFE_NUMERIC_CONSTEXPR interval<R> operator+(const interval<T> & t, const interval<U> & u){
    // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
    return interval<R>(
        checked::add<R>(static_cast<T>(t.l), static_cast<U>(u.l)),
        checked::add<R>(static_cast<T>(t.u), static_cast<U>(u.u))
    );
}

template<typename R, typename T, typename U>
SAFE_NUMERIC_CONSTEXPR interval<R> operator-(const interval<T> & t, const interval<U> & u){
    // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
    return interval<R>(
        checked::subtract<R>(static_cast<T>(t.l), static_cast<U>(u.u)),
        checked::subtract<R>(static_cast<T>(t.u), static_cast<U>(u.l))
    );
}

template<typename R, typename T, typename U>
SAFE_NUMERIC_CONSTEXPR interval<R> operator*(const interval<T> & t, const interval<U> & u){
    // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
    return
        interval<R>(
            vmin(
                checked::multiply<R>(static_cast<T>(t.l), static_cast<U>(u.l)),
                checked::multiply<R>(static_cast<T>(t.l), static_cast<U>(u.u)),
                checked::multiply<R>(static_cast<T>(t.u), static_cast<U>(u.l)),
                checked::multiply<R>(static_cast<T>(t.u), static_cast<U>(u.u))
            ),
            vmax(
                checked::multiply<R>(static_cast<T>(t.l), static_cast<U>(u.l)),
                checked::multiply<R>(static_cast<T>(t.l), static_cast<U>(u.u)),
                checked::multiply<R>(static_cast<T>(t.u), static_cast<U>(u.l)),
                checked::multiply<R>(static_cast<T>(t.u), static_cast<U>(u.u))
            )
        );
}

template<typename R, typename T, typename U>
SAFE_NUMERIC_CONSTEXPR inline interval<R> operator/(
    const interval<T> & t,
    const interval<U> & u
){
    // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
    return
        (u.l <= 0 && u.u >= 0) ?
            interval<R>(
                checked_result<R>(0),
                checked_result<R>(
                    exception_type::domain_error,
                    "interval divisor includes zero"
                )
            )
        :
            interval<R>(
                vmin(
                    checked::divide<R>(static_cast<T>(t.l), static_cast<U>(u.l)),
                    checked::divide<R>(static_cast<T>(t.l), static_cast<U>(u.u)),
                    checked::divide<R>(static_cast<T>(t.u), static_cast<U>(u.l)),
                    checked::divide<R>(static_cast<T>(t.u), static_cast<U>(u.u))
                ),
                vmax(
                    checked::divide<R>(static_cast<T>(t.l), static_cast<U>(u.l)),
                    checked::divide<R>(static_cast<T>(t.l), static_cast<U>(u.u)),
                    checked::divide<R>(static_cast<T>(t.u), static_cast<U>(u.l)),
                    checked::divide<R>(static_cast<T>(t.u), static_cast<U>(u.u))
                )
            )
        ;
}

template<typename R, typename T, typename U>
SAFE_NUMERIC_CONSTEXPR interval<R> mod1(
    const interval<T> & t,
    const interval<U> & u
){
    return
        interval<R>(
            vmin(
                checked::modulus<R>(static_cast<T>(t.l), static_cast<U>(u.l)),
                checked::modulus<R>(static_cast<T>(t.l), static_cast<U>(u.u)),
                checked::modulus<R>(static_cast<T>(t.u), static_cast<U>(u.l)),
                checked::modulus<R>(static_cast<T>(t.u), static_cast<U>(u.u))
            ),
            vmax(
                checked::modulus<R>(static_cast<T>(t.l), static_cast<U>(u.l)),
                checked::modulus<R>(static_cast<T>(t.l), static_cast<U>(u.u)),
                checked::modulus<R>(static_cast<T>(t.u), static_cast<U>(u.l)),
                checked::modulus<R>(static_cast<T>(t.u), static_cast<U>(u.u))
            )
        );
}

template<typename R, typename T, typename U>
SAFE_NUMERIC_CONSTEXPR interval<R> operator%(
    const interval<T> & t,
    const interval<U> & u
){
    interval<R> i[4];
    if(t.l < 0 && u.l < 0)
        i[0] = mod1(interval<R>(t.l, -1), interval<R>(u.l, -1));
    if(t.l > 0 && u.l < 0)
        i[1] = mod1(interval<R>(t.l, t.u), interval<R>(u.l, -1));
    if(t.l < 0 && u.l > 0)
        i[2] = mod1(interval<R>(t.l, -1), interval<R>(u.l, u.u));
    if(t.l > 0 && u.l > 0)
        i[3] = mod1(interval<R>(t.l, t.u), interval<R>(u.l, u.u));

    return interval<R>(
        vmin(i[0].l, i[1].l, i[2].l, i[3].l),
        vmax(i[0].l, i[1].l, i[2].l, i[3].l)
    );

}

template<typename T, typename U>
SAFE_NUMERIC_CONSTEXPR boost::logic::tribool operator<(
    const interval<T> & t,
    const interval<U> & u
){
    return
        (! t.no_exception() || ! u.no_exception()) ?
            boost::logic::indeterminate
        :
        // if every element in t is less than every element in u
        safe_compare::less_than(static_cast<T>(t.u), static_cast<U>(u.l)) ?
            boost::logic::tribool(true)
        :
        // if every element in t is greater than every element in u
        safe_compare::greater_than(static_cast<T>(t.l), static_cast<U>(u.u)) ?
            boost::logic::tribool(false)
        :
        // otherwise some element(s) in t are greater than some element in u
            boost::logic::indeterminate
    ;
}

template<typename T, typename U>
SAFE_NUMERIC_CONSTEXPR boost::logic::tribool operator>(
    const interval<T> & t,
    const interval<U> & u
){
    return
        (! t.no_exception() || ! u.no_exception()) ?
            boost::logic::indeterminate
        :
        // if every element in t is greater than every element in u
        safe_compare::greater_than(static_cast<T>(t.l), static_cast<U>(u.u)) ?
            boost::logic::tribool(true)
        :
        // if every element in t is less than every element in u
        safe_compare::less_than(static_cast<T>(t.u), static_cast<U>(u.l)) ?
            boost::logic::tribool(false)
        :
        // otherwise some element(s) in t are greater than some element in u
            boost::logic::indeterminate
    ;
}

template<typename T, typename U>
SAFE_NUMERIC_CONSTEXPR bool operator==(
    const interval<T> & t,
    const interval<U> & u
){
    // every element in t can only equal every element in u if and only if
    return (t.l == u.l && t.u == u.u) ;
}

template<typename T, typename U>
SAFE_NUMERIC_CONSTEXPR boost::logic::tribool operator<=(
    const interval<T> & t,
    const interval<U> & u
){
    return ! (t > u);
}

template<typename T, typename U>
SAFE_NUMERIC_CONSTEXPR boost::logic::tribool operator>=(
    const interval<T> & t,
    const interval<U> & u
){
    return ! (t < u);
}

} // numeric
} // boost

#include <iosfwd>

namespace std {

template<typename T>
std::ostream & operator<<(std::ostream & os, const boost::numeric::interval<T> & i){
    os << "[" << i.l << "," << i.u << "]";
    return os;
}

} // std

#endif // BOOST_NUMERIC_INTERVAL_HPP
