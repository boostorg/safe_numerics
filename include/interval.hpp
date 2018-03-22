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
#include <initializer_list>
#include <algorithm> // minmax, min, max

#include <boost/logic/tribool.hpp>

#include "utility.hpp" // log
#include "safe_compare.hpp"

// from stack overflow
// http://stackoverflow.com/questions/23815138/implementing-variadic-min-max-functions

namespace boost {
namespace numeric {

template<typename R>
struct interval {
    #if 0
    static_assert(
        std::is_literal_type< checked_result<R> >::value,
        "is literal type"
    );
    #endif
    const R l;
    const R u;

    template<typename T>
    constexpr interval(const T & lower, const T & upper) :
        l(lower),
        u(upper)
    {
        // assert(static_cast<bool>(l <= u));
    }
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

    constexpr interval();

    #if 0
    // return true if this interval contains the given point
    template<typename T>
    constexpr boost::logic::tribool includes(const T & t) const {
        return l <= t && t <= u;
    }
    // if this interval contains every point found in some other inteval t
    //  return true
    // otherwise
    //  return false or indeterminant
    template<typename T>
    constexpr boost::logic::tribool includes(const interval<T> & t) const {
        return u >= t.u && l <= t.l;
    }

    // return true if this interval contains the given point
    template<typename T>
    constexpr boost::logic::tribool excludes(const T & t) const {
        return t < l || t > u;
    }
    // if this interval contains every point found in some other inteval t
    //  return true
    // otherwise
    //  return false or indeterminant
    template<typename T>
    constexpr boost::logic::tribool excludes(const interval<T> & t) const {
        return t.u < l || u < t.l;
    }
    #endif
    // return true if this interval contains the given point
    constexpr boost::logic::tribool includes(const R & t) const {
        return l <= t && t <= u;
    }
    // if this interval contains every point found in some other inteval t
    //  return true
    // otherwise
    //  return false or indeterminant
    constexpr boost::logic::tribool includes(const interval<R> & t) const {
        return u >= t.u && l <= t.l;
    }

    // return true if this interval contains the given point
    constexpr boost::logic::tribool excludes(const R & t) const {
        return t < l || t > u;
    }
    // if this interval contains every point found in some other inteval t
    //  return true
    // otherwise
    //  return false or indeterminant
    constexpr boost::logic::tribool excludes(const interval<R> & t) const {
        return t.u < l || u < t.l;
    }

};

template<class R>
constexpr interval<R> make_interval(){
    return interval<R>();
}
template<class R>
constexpr interval<R> make_interval(const R & r){
    return interval<R>(r, r);
}
template<class R>
constexpr interval<R>::interval() :
    l(std::numeric_limits<R>::lowest()),
    u(std::numeric_limits<R>::max())
{}
// account for the fact that for floats and doubles
// the most negative value is called "lowest" rather
// than min
template<>
constexpr interval<float>::interval() :
    l(std::numeric_limits<float>::lowest()),
    u(std::numeric_limits<float>::max())
{}
template<>
constexpr interval<double>::interval() :
    l(std::numeric_limits<double>::lowest()),
    u(std::numeric_limits<double>::max())
{}

template<typename T>
constexpr interval<T> operator+(const interval<T> & t, const interval<T> & u){
    // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
    return {t.l + u.l, t.u + u.u};
}

template<typename T>
constexpr interval<T> operator-(const interval<T> & t, const interval<T> & u){
    // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
    return {t.l - u.u, t.u - u.l};
}

template<typename T>
constexpr interval<T> operator*(const interval<T> & t, const interval<T> & u){

    // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
    return utility::minmax<T>(
        std::initializer_list<T> {
            t.l * u.l,
            t.l * u.u,
            t.u * u.l,
            t.u * u.u
        }
    );
}

// interval division
// note: presumes 0 is not included in the range of the denominator
template<typename T>
constexpr interval<T> operator/(const interval<T> & t, const interval<T> & u){
//    assert(static_cast<bool>(u.excludes(T(0))));
    return utility::minmax<T>(
        std::initializer_list<T> {
            t.l / u.l,
            t.l / u.u,
            t.u / u.l,
            t.u / u.u
        }
    );
}

// modulus of two intervals.  This will give a new range of for the modulus.  But
// it doesn't consider the possibility that the radix is zero.  This will
// have to be considered separately.
template<typename T>
constexpr interval<T> operator%(const interval<T> &, const interval<T> & u){

    /* Turns out that getting the exact range is a suprisingly difficult
     * problem.  But we can get a reasonable guaranteed range with the following
     * simple formula (due to Skona Brittain):
     *
     *  [-m + 1, m - 1] where m is max(|c|, |d|).
     *
     * where the operation is [a,b] % [c,d]
     *
     * But a naive implementation of this algorithm doesn't work.  We have a
     * problem with (8 bit integers for illustration) abs(-128) -> -128! So we
     * can't get the absolute value of the minimum value !
     *
     * so we impement the above in a slightly different way.
     */

    // const T xa = (t.l > 0) ? t.l - 1 : t.l + 1; // a
    // const T xb = (t.u > 0) ? t.u - 1 : t.u + 1; // b
    const T xc =
        (u.l > T(0))
        ? u.l - T(1)
        : (u.l < T(0))
            ? T(0) - (u.l + T(1))
            : T(0); // c
    const T xd =
        (u.u > T(0))
        ? u.u - T(1)
        : (u.u < T(0))
            ? T(0) -(u.u + T(1))
            : T(0); // d

    // special care to void problem when inverting -128

    const T mxc = T(0) - xc;
    const T mxd = T(0) - xd;

    return utility::minmax(
        std::initializer_list<T> {
            xc,
            mxc,
            xd,
            mxd,
            0
        }
    );
}

template<typename T>
constexpr interval<T> operator<<(const interval<T> & t, const interval<T> & u){
//    static_assert(std::is_integral<T>::value, "left shift only defined for integral type");
    //return interval<T>{t.l << u.l, t.u << u.u};
    return utility::minmax<T>(
        std::initializer_list<T> {
            t.l << u.l,
            t.l << u.u,
            t.u << u.l,
            t.u << u.u
        }
    );
}

template<typename T>
constexpr interval<T> operator>>(const interval<T> & t, const interval<T> & u){
//    static_assert(std::is_integral<T>::value, "right shift only defined for integral type");
    //return interval<T>{t.l >> u.u, t.u >> u.l};
    return utility::minmax<T>(
        std::initializer_list<T> {
            t.l >> u.l,
            t.l >> u.u,
            t.u >> u.l,
            t.u >> u.u
        }
    );
}

// union of two intervals
template<typename T>
constexpr interval<T> operator|(const interval<T> & t, const interval<T> & u){
    return utility::minmax<T>(
        std::initializer_list<T> {t.l, u.l, t.u, u.u}
    );
}

// intersection of two intervals
template<typename T>
constexpr interval<T> operator&(const interval<T> & t, const interval<T> & u){
    const T & rl = std::max(t.l, u.l);
    const T & ru = std::min(t.u, u.u);
    return interval<T>(rl, ru);
}

// determine whether two intervals intersect
template<typename T>
constexpr boost::logic::tribool intersect(const interval<T> & t, const interval<T> & u){
    return t.u >= u.l || t.l <= u.u;
}

template<typename T>
constexpr boost::logic::tribool operator<(
    const interval<T> & t,
    const interval<T> & u
){
    return
        // if every element in t is less than every element in u
        t.u < u.l ? boost::logic::tribool(true):
        // if every element in t is greater than every element in u
        t.l > u.u ? boost::logic::tribool(false):
        // otherwise some element(s) in t are greater than some element in u
        boost::logic::indeterminate
    ;
}

template<typename T>
constexpr boost::logic::tribool operator>(
    const interval<T> & t,
    const interval<T> & u
){
    return
        // if every element in t is greater than every element in u
        t.l > u.u ? boost::logic::tribool(true) :
        // if every element in t is less than every element in u
        t.u < u.l ? boost::logic::tribool(false) :
        // otherwise some element(s) in t are greater than some element in u
        boost::logic::indeterminate
    ;
}

template<typename T>
constexpr bool operator==(
    const interval<T> & t,
    const interval<T> & u
){
    // intervals have the same limits
    return t.l == u.l && t.u == u.u;
}

template<typename T>
constexpr bool operator!=(
    const interval<T> & t,
    const interval<T> & u
){
    return ! (t == u);
}

template<typename T>
constexpr boost::logic::tribool operator<=(
    const interval<T> & t,
    const interval<T> & u
){
    return ! (t > u);
}

template<typename T>
constexpr boost::logic::tribool operator>=(
    const interval<T> & t,
    const interval<T> & u
){
    return ! (t < u);
}

} // numeric
} // boost

#include <iosfwd>

namespace std {

template<typename CharT, typename Traits, typename T>
inline std::basic_ostream<CharT, Traits> &
operator<<(
    std::basic_ostream<CharT, Traits> & os,
    const boost::numeric::interval<T> & i
){
    return os << '[' << i.l << ',' << i.u << ']';
}
template<typename CharT, typename Traits>
inline std::basic_ostream<CharT, Traits> &
operator<<(
    std::basic_ostream<CharT, Traits> & os,
    const boost::numeric::interval<unsigned char> & i
){
    os << "[" << (unsigned)i.l << "," << (unsigned)i.u << "]";
    return os;
}

template<typename CharT, typename Traits>
inline std::basic_ostream<CharT, Traits> &
operator<<(
    std::basic_ostream<CharT, Traits> & os,
    const boost::numeric::interval<signed char> & i
){
    os << "[" << (int)i.l << "," << (int)i.u << "]";
    return os;
}

} // std


#endif // BOOST_NUMERIC_INTERVAL_HPP
