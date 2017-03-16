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
#include <array>
#include <initializer_list>

#include <boost/logic/tribool.hpp>

#include "utility.hpp" // log
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
    constexpr interval();

    // return true if this interval contains every point found in some
    // other inteval t
    template<typename T>
    constexpr bool includes(const interval<T> & t) const {
        return
            safe_compare::greater_than_equal(t.l, l)
            &&
            safe_compare::less_than_equal(t.u, u)
        ;
    }
    template<typename T>
    constexpr bool includes(const T & t) const {
        return
            safe_compare::greater_than_equal(t, l)
            &&
            safe_compare::less_than_equal(t, u)
        ;
    }
};

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

namespace detail {

template<typename R>
constexpr checked_result<interval<R>> failed_result(
    exception_type::domain_error,
    "indefinite interval"
);

// create constexpr versions of stl algorthms which are not (yet)
// constexpr.
template<class InputIt, class UnaryPredicate>
constexpr InputIt find_if(InputIt first, InputIt last, UnaryPredicate q){
    for (; first != last; ++first) {
        if (q(*first)) {
            return first;
        }
    }
    return last;
}

// helper function used below to select the minimum and maximum value
// from a list of values
template<typename R>
constexpr checked_result<interval<R>> minmax(
    const std::initializer_list<checked_result<R>> & acr
){
    typename std::initializer_list<checked_result<R>>::const_iterator const e =         find_if(
            acr.begin(),
            acr.end(),
            exception<R>
        );
    return (acr.end() == e) ?
        checked_result<interval<R>>(
            interval<R>(std::minmax(acr, & safe_compare::less_than<R, R>))
        )
        :
        failed_result<R>// throw assert_failure([]{assert(!"input not in range");})
        ;
}

}  // detail

template<typename R, typename T, typename U>
constexpr checked_result<interval<R>> add(
    const interval<T> & t,
    const interval<U> & u
){
    // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
    checked_result<R> lower = checked::add<R>(static_cast<T>(t.l), static_cast<U>(u.l));
    if(! lower.no_exception())
        return detail::failed_result<R>;
    checked_result<R> upper = checked::add<R>(static_cast<T>(t.u), static_cast<U>(u.u));
    if(! upper.no_exception())
        return detail::failed_result<R>;
    return interval<R>(lower, upper);
}

template<typename R, typename T, typename U>
constexpr checked_result<interval<R>> subtract(const interval<T> & t, const interval<U> & u){
    // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
    checked_result<R> lower = checked::subtract<R>(static_cast<T>(t.l), static_cast<U>(u.u));
    if(! lower.no_exception())
        return detail::failed_result<R>;
    checked_result<R> upper = checked::subtract<R>(static_cast<T>(t.u), static_cast<U>(u.l));
    if(! upper.no_exception())
        return detail::failed_result<R>;
    return interval<R>(lower, upper);
}

template<typename R, typename T, typename U>
constexpr checked_result<interval<R>> multiply(const interval<T> & t, const interval<U> & u){
    // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
    return detail::minmax<R>(
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
    return detail::minmax<R>( (u.u < 0 || u.l > 0) ?
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
    // adapted from https://en.wikipedia.org/wiki/Modulo_operation
    return detail::minmax<R>( (u.l < 0 && 0 < u.u) ?
        // if divisor range includes zero then we need include +1 and -1
        // among the possible divisors.  But since % +1 and % -1 yield
        // the same result, which is 0, then adding 0 to the possibilities
        // is sufficient to make sure we've got the whole possible range
        std::initializer_list<checked_result<R>> {
            checked::modulus<R>(t.l, u.l),
            checked::modulus<R>(t.l, u.u),
            checked::modulus<R>(t.u, u.l),
            checked::modulus<R>(t.u, u.u),
            0 // % 1 or % -1
        }
        :
        // if divisor range doesn't include zero, the following should be
        // sufficient.
        std::initializer_list<checked_result<R>> {
            checked::modulus<R>(t.l, u.l),
            checked::modulus<R>(t.l, u.u),
            checked::modulus<R>(t.u, u.l),
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
    return detail::minmax<R>( std::initializer_list<checked_result<R>> {
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
    return detail::minmax<R>( std::initializer_list<checked_result<R>> {
        checked::right_shift<R>(t.l, u.l),
        checked::right_shift<R>(t.l, u.u),
        checked::right_shift<R>(t.u, u.l),
        checked::right_shift<R>(t.u, u.u)
    });
}

template<typename R, typename T, typename U>
constexpr checked_result<interval<R>> right_shift_positive(
    const interval<T> & t,
    const interval<U> & u
){
    const U ul = safe_compare::greater_than(0, u.l) ? 0 : u.l;

    const U ux = boost::numeric::log(std::numeric_limits<U>::max());
    const U uu = safe_compare::less_than(u.u, ux) ? u.u : ux;

    return detail::minmax<R>( std::initializer_list<checked_result<R>> {
        checked::right_shift<R>(t.l, ul),
        checked::right_shift<R>(t.l, uu),
        checked::right_shift<R>(t.u, ul),
        checked::right_shift<R>(t.u, uu)
    });
}

template<typename R, typename T, typename U>
constexpr checked_result<interval<R>> intersection(
    const interval<T> & t,
    const interval<U> & u
){
    const checked_result<R> rl =
        checked::cast<R>(safe_compare::greater_than(t.l, u.l) ? t.l : u.l);
    const checked_result<R> ru =
        checked::cast<R>(safe_compare::less_than(t, u) ? t.u : u.u);

    if(rl > ru){
        return checked_result<interval<R>>(
            exception_type::uninitialized,
            "null intersection"
        );
    }
    return interval<R>(rl, ru);
}

template<typename R, typename T, typename U>
constexpr checked_result<interval<R>> union_interval(
    const interval<T> & t,
    const interval<U> & u
){
    const checked_result<R> rl =
        checked::cast<R>(safe_compare::less_than(t.l, u.l) ? t.l : u.l);
    const checked_result<R> ru =
        checked::cast<R>(safe_compare::greater_than(t, u) ? t.u : u.u);

    return interval<R>(rl, ru);
}

template<typename T, typename U>
constexpr boost::logic::tribool operator<(
    const interval<T> & t,
    const interval<U> & u
){
    return
        // if every element in t is less than every element in u
        safe_compare::less_than(t.u, u.l) ?
            boost::logic::tribool(true)
        :
        // if every element in t is greater than every element in u
        safe_compare::greater_than(t.l, u.u) ?
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
        safe_compare::greater_than(t.l, u.u) ?
            boost::logic::tribool(true)
        :
        // if every element in t is less than every element in u
        safe_compare::less_than(t.u, u.l) ?
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
    return safe_compare::equal(t.l, u.l)
    && safe_compare::equal(t.u, u.u) ;
}

template<typename T, typename U>
constexpr bool operator!=(
    const interval<T> & t,
    const interval<U> & u
){
    return ! (t == u);
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

template<typename CharT, typename Traits, typename T>
inline std::basic_ostream<CharT, Traits> &
operator<<(
    std::basic_ostream<CharT, Traits> & os,
    const boost::numeric::interval<T> & i
){
    return os << '[' << i.l << ',' << i.u << ']';
}
template<typename CharT, typename Traits, typename T>
inline std::basic_ostream<CharT, Traits> &
operator<<(
    std::basic_ostream<CharT, Traits> & os,
    const boost::numeric::interval<unsigned char> & i
){
    os << "[" << (unsigned)i.l << "," << (unsigned)i.u << "]";
    return os;
}

template<typename CharT, typename Traits, typename T>
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
