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
#include <cstdlib> // quick_exit
#include <array>
#include <algorithm>
#include <initializer_list>

#include <boost/logic/tribool.hpp>

#include "checked_result.hpp"
#include "checked.hpp"

// from stack overflow
// http://stackoverflow.com/questions/23815138/implementing-variadic-min-max-functions

namespace boost {
namespace numeric {

template<typename R>
struct interval {
    static_assert(
        std::is_literal_type< checked_result<R> >::value,
        "is literal type"
    );

    const R l;
    const R u;

    template<typename T>
    constexpr interval(const T & lower, const T & upper) :
        l(lower),
        u(upper)
    {}
    template<typename T>
    constexpr interval(const std::pair<T, T> & p) :
        l(p.first),
        u(p.second)
    {}
    template<class T>
    constexpr interval(const interval<T> & rhs) :
        l(rhs.l),
        u(rhs.u)
    {}
    constexpr interval() :
        l(std::numeric_limits<R>::min()),
        u(std::numeric_limits<R>::max())
    {}

    // return true if this interval contains every point found in some
    // other inteval t
    template<typename T>
    constexpr bool includes(const interval<T> & t) const {
        // note very tricky algebra here.  the <= and >= operators
        // on checked_result yield tribool.  If either argument is an exception
        // condition, he result is indeterminate.  The result of && on two
        // tribools is indeterminant if either is indeterminate.
        return l <= t.l && u >= t.u ;
    }
};

namespace {

template<typename R>
constexpr checked_result<interval<R>> failed_result(
    exception_type::domain_error,
    "indefinite interval"
);

// create constexpr versions of stl algorthms which are not (yet)
// constexpr.
template<class InputIt, class UnaryPredicate>
constexpr InputIt find_if_not(InputIt first, InputIt last, UnaryPredicate q)
{
    for (; first != last; ++first) {
        if (!q(*first)) {
            return first;
        }
    }
    return last;
}

template<class InputIt, class T>
constexpr InputIt find(InputIt first, InputIt last, const T& value)
{
    for (; first != last; ++first) {
        if (*first == value) {
            return first;
        }
    }
    return last;
}

template<typename R>
constexpr bool less_than(
    const checked_result<R> & x,
    const checked_result<R> & y
){
    return x < y;
}

template<typename R>
constexpr checked_result<interval<R>> select(
    const std::initializer_list<checked_result<R>> & acr
){
    typename std::initializer_list<checked_result<R>>::const_iterator const e =         find_if_not(
            acr.begin(),
            acr.end(),
            no_exception<R>
        );
    return (acr.end() == e) ?
        checked_result<interval<R>>(
            interval<R>(std::minmax(acr, less_than<R>))
        )
        :
        failed_result<R>// throw assert_failure([]{assert(!"input not in range");})
        ;
}

}  // namespace

template<typename R, typename T, typename U>
constexpr checked_result<interval<R>> add(
    const interval<T> & t,
    const interval<U> & u
){
    // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
    checked_result<R> lower = checked::add<R>(static_cast<T>(t.l), static_cast<U>(u.l));
    if(! lower.no_exception())
        return failed_result<R>;
    checked_result<R> upper = checked::add<R>(static_cast<T>(t.u), static_cast<U>(u.u));
    if(! upper.no_exception())
        return failed_result<R>;
    return interval<R>(lower, upper);
}

template<typename R, typename T, typename U>
constexpr checked_result<interval<R>> subtract(const interval<T> & t, const interval<U> & u){
    // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
    checked_result<R> lower = checked::subtract<R>(static_cast<T>(t.l), static_cast<U>(u.u));
    if(! lower.no_exception())
        return failed_result<R>;
    checked_result<R> upper = checked::subtract<R>(static_cast<T>(t.u), static_cast<U>(u.l));
    if(! upper.no_exception())
        return failed_result<R>;
    return interval<R>(lower, upper);
}

template<typename R, typename T, typename U>
constexpr checked_result<interval<R>> multiply(const interval<T> & t, const interval<U> & u){
    // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic

    return select<R>(
        std::initializer_list<checked_result<R>> {
            checked::multiply<R>(t.l, u.l),
            checked::multiply<R>(t.l, u.u),
            checked::multiply<R>(t.u, u.l),
            checked::multiply<R>(t.u, u.u)
        }
    );
}

// divide two intervals.  BUT don't consider the possibility that the
// denominator might contain a zero.
template<typename R, typename T, typename U>
constexpr inline checked_result<interval<R>> divide_nz(
    const interval<T> & t,
    const interval<U> & u
){
    // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
    return select<R>( (u.u < 0 || u.l > 0) ?
        std::initializer_list<checked_result<R>> {
            checked::divide<R>(t.l, u.l),
            checked::divide<R>(t.l, u.u),
            checked::divide<R>(t.u, u.l),
            checked::divide<R>(t.u, u.u)
        }
    :
        std::initializer_list<checked_result<R>> {
            checked::divide<R>(t.l, u.l),
            checked::divide<R>(t.l, -1),
            checked::divide<R>(t.u, u.l),
            checked::divide<R>(t.u, -1),
            checked::divide<R>(t.l, 1),
            checked::divide<R>(t.l, u.u),
            checked::divide<R>(t.u, 1),
            checked::divide<R>(t.u, u.u)
        }
    );
}

template<typename R, typename T, typename U>
constexpr inline checked_result<interval<R>> divide(
    const interval<T> & t,
    const interval<U> & u
){
    // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
    if(u.l <= 0 && u.u >= 0)
        return checked_result<interval<R>>(
            exception_type::domain_error,
            "interval divisor includes zero"
        );
    return divide_nz<R>(t, u);
}

template<typename R, typename T, typename U>
constexpr checked_result<interval<R>> modulus_nz(
    const interval<T> & t,
    const interval<U> & u
){
    return select<R>( (u.u < 0 || u.l > 0) ?
        std::initializer_list<checked_result<R>> {
            checked::modulus<R>(t.l, u.l),
            checked::modulus<R>(t.l, u.u),
            checked::modulus<R>(t.u, u.l),
            checked::modulus<R>(t.u, u.u)
        }
    :
        std::initializer_list<checked_result<R>> {
            checked::modulus<R>(t.l, u.l),
            checked::modulus<R>(t.l, -1),
            checked::modulus<R>(t.u, u.l),
            checked::modulus<R>(t.u, 1),
            checked::modulus<R>(t.l, 1),
            checked::modulus<R>(t.l, u.u),
            checked::modulus<R>(t.u, 1),
            checked::modulus<R>(t.u, u.u)
        }
    );
}

template<typename R, typename T, typename U>
constexpr checked_result<interval<R>> modulus(
    const interval<T> & t,
    const interval<U> & u
){
    // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
    if(u.l <= 0 && u.u >= 0)
        return checked_result<interval<R>>(
            exception_type::domain_error,
            "interval modulus includes zero"
        );
    return modulus_nz<R>(t, u);
}

template<typename R, typename T, typename U>
constexpr checked_result<interval<R>> left_shift(
    const interval<T> & t,
    const interval<U> & u
){
    return select<R>( std::initializer_list<checked_result<R>> {
        checked::left_shift<R>(t.l, u.l),
        checked::left_shift<R>(t.l, u.u),
        checked::left_shift<R>(t.u, u.l),
        checked::left_shift<R>(t.u, u.u)
    });
}

template<typename R, typename T, typename U>
constexpr checked_result<interval<R>> right_shift(
    const interval<T> & t,
    const interval<U> & u
){
    return select<R>( std::initializer_list<checked_result<R>> {
        checked::right_shift<R>(t.l, u.l),
        checked::right_shift<R>(t.l, u.u),
        checked::right_shift<R>(t.u, u.l),
        checked::right_shift<R>(t.u, u.u)
    });
}

template<typename T, typename U>
constexpr boost::logic::tribool operator<(
    const interval<T> & t,
    const interval<U> & u
){
    return
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
constexpr boost::logic::tribool operator>(
    const interval<T> & t,
    const interval<U> & u
){
    return
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
constexpr bool operator==(
    const interval<T> & t,
    const interval<U> & u
){
    // intervals have the same limits
    return (t.l == u.l && t.u == u.u) ;
}

template<typename T, typename U>
constexpr bool operator!=(
    const interval<T> & t,
    const interval<U> & u
){
    return ! t == u;
}

template<typename T, typename U>
constexpr boost::logic::tribool operator<=(
    const interval<T> & t,
    const interval<U> & u
){
    return ! (t > u);
}

template<typename T, typename U>
constexpr boost::logic::tribool operator>=(
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

template<>
std::ostream & operator<<(std::ostream & os, const boost::numeric::interval<unsigned char> & i){
    os << "[" << (unsigned)i.l << "," << (unsigned)i.u << "]";
    return os;
}

template<>
std::ostream & operator<<(std::ostream & os, const boost::numeric::interval<signed char> & i){
    os << "[" << (int)i.l << "," << (int)i.u << "]";
    return os;
}

} // std

#endif // BOOST_NUMERIC_INTERVAL_HPP
