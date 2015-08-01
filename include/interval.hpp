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

#include <boost/logic/tribool.hpp>

#include "checked_result.hpp"
#include "checked.hpp"

namespace boost {
namespace numeric {

template<typename R>
struct interval {
    const checked_result<R> l;
    const checked_result<R> u;

/*
    template<typename T, typename U>
    SAFE_NUMERIC_CONSTEXPR interval(const T & lower, const U & upper) :
        l(checked::cast<R>(lower)),
        u(checked::cast<R>(upper))
    {}
*/
    SAFE_NUMERIC_CONSTEXPR interval(
        const checked_result<R> & lower,
        const checked_result<R> & upper
    ) :
        l(checked_result<R>(lower)),
        u(checked_result<R>(upper))
    {}
    SAFE_NUMERIC_CONSTEXPR interval(const R & lower, const R & upper) :
        l(checked_result<R>(lower)),
        u(checked_result<R>(upper))
    {}
    /*
    SAFE_NUMERIC_CONSTEXPR interval(const interval<R> & rhs) :
        l(rhs.l),
        u(rhs.u)
    {}
    */

    SAFE_NUMERIC_CONSTEXPR interval() :
        l(std::numeric_limits<R>::min()),
        u(std::numeric_limits<R>::max())
    {}
    SAFE_NUMERIC_CONSTEXPR bool no_exception() const {
        return l == exception_type::no_exception
        && u == exception_type::no_exception ;
    }
    // return true if this interval contains every point found in some
    // other inteval t
    template<typename T>
    SAFE_NUMERIC_CONSTEXPR bool includes(const interval<T> & t) const {
        // not very tricky algebra here.  the <= and >= operators
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
            min(
                min(
                    checked::multiply<R>(static_cast<R>(t.l), static_cast<R>(u.l)),
                    checked::multiply<R>(static_cast<R>(t.l), static_cast<R>(u.u))
                ),
                min(
                    checked::multiply<R>(static_cast<R>(t.u), static_cast<R>(u.l)),
                    checked::multiply<R>(static_cast<R>(t.u), static_cast<R>(u.u))
                )
            ),
            max(
                max(
                    checked::multiply<R>(static_cast<R>(t.l), static_cast<R>(u.l)),
                    checked::multiply<R>(static_cast<R>(t.l), static_cast<R>(u.u))
                ),
                max(
                    checked::multiply<R>(static_cast<R>(t.u), static_cast<R>(u.l)),
                    checked::multiply<R>(static_cast<R>(t.u), static_cast<R>(u.u))
                )
            )
        );
}

template<typename R, typename T, typename U>
SAFE_NUMERIC_CONSTEXPR interval<R> operator/(const interval<T> & t, const interval<U> & u){
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
                min(
                    min(
                        checked::divide<R>(static_cast<R>(t.l), static_cast<R>(u.l)),
                        checked::divide<R>(static_cast<R>(t.l), static_cast<R>(u.u))
                    ),
                    min(
                        checked::divide<R>(static_cast<R>(t.u), static_cast<R>(u.l)),
                        checked::divide<R>(static_cast<R>(t.u), static_cast<R>(u.u))
                    )
                ),
                max(
                    max(
                        checked::divide<R>(static_cast<R>(t.l), static_cast<R>(u.l)),
                        checked::divide<R>(static_cast<R>(t.l), static_cast<R>(u.u))
                    ),
                    max(
                        checked::divide<R>(static_cast<R>(t.u), static_cast<R>(u.l)),
                        checked::divide<R>(static_cast<R>(t.u), static_cast<R>(u.u))
                    )
                )
            )
        ;
}

template<typename R, typename T, typename U>
SAFE_NUMERIC_CONSTEXPR interval<R> operator%(
    const interval<T> & t,
    const interval<U> & u
){
    // adapted from / operator above
    return
        (u.l <= 0) ?
            interval<R>(
                checked_result<R>(0),
                checked_result<R>(
                    exception_type::domain_error,
                    "interval divisor includes zero"
                )
            )
        :
            interval<R>(checked_result<R>(0), max(u.u, u.l))
        ;
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
    os << "[" << i.l << "," << i.u << "]" << std::endl;
    return os;
}

} // std

#endif // BOOST_NUMERIC_INTERVAL_HPP
