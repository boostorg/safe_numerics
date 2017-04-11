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
#include <boost/core/demangle.hpp>

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
constexpr interval<R> make_interval(const R & r){
    return interval<R>();
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

namespace detail {

template<typename T>
constexpr interval<T>
minmax(const std::initializer_list<T> & l){
    using namespace boost::numeric;
    T minimum{exception_type::positive_overflow_error, ""};
    T maximum{exception_type::negative_overflow_error, ""};
    // note: we can't use for_each and a lambda because neither of these are
    // constexpr
    for(
        typename std::initializer_list<T>::const_iterator i = l.begin();
        i != l.end();
        ++i
    ){
        // std::cout << *i << ',';
        // std::cout.flush();
        if(minimum != exception_type::negative_overflow_error){
            // if it corresponds to the lowest value
            if(*i == exception_type::negative_overflow_error){
                // initialize the minimum
                minimum = *i;
            }
            // skip any other exceptions
            else
            if(! (*i).exception()){
                if(minimum.exception()
                || (*i).m_r < minimum.m_r){
                    minimum = *i;
                }
            }
        }
        if(maximum != exception_type::positive_overflow_error){
            // if it corresponds to the highest value
            if(*i == exception_type::positive_overflow_error){
                // initialize the maximum
                maximum = *i;
            }
            // skip any other exceptions
            else
            if(! (*i).exception()){
                if(maximum.exception()
                || (*i).m_r > maximum.m_r){
                    maximum = *i;
                }
            }
        }
    }
    return interval<T>{minimum, maximum};
}

}  // detail

template<typename R, typename T, typename U>
constexpr interval<checked_result<R>> add(
    const interval<T> & t,
    const interval<U> & u
){
    // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
    const checked_result<R> lower = checked::add<R>(t.l, u.l);
    const checked_result<R> upper = checked::add<R>(t.u, u.u);
    return interval<checked_result<R>>(lower, upper);
}

template<typename R, typename T, typename U>
constexpr interval<checked_result<R>> subtract(
    const interval<T> & t,
    const interval<U> & u
){
    // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
    const checked_result<R> lower = checked::subtract<R>(t.l, u.u);
    const checked_result<R> upper = checked::subtract<R>(t.u, u.l);
    return interval<checked_result<R>>(lower, upper);
}

template<typename R, typename T, typename U>
constexpr interval<checked_result<R>> multiply(
    const interval<T> & t,
    const interval<U> & u
){
    // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
    return detail::minmax<checked_result<R>>(
        std::initializer_list<checked_result<R>> {
            checked::multiply<R>(t.l, u.l),
            checked::multiply<R>(t.l, u.u),
            checked::multiply<R>(t.u, u.l),
            checked::multiply<R>(t.u, u.u)
        }
    );
}

// divide two intervals.  This will give a new range of the quotient.  But
// it doesn't consider the possibility that the divisor is zero.  This will
// have to be considered separately.
template<typename R, typename T, typename U>
constexpr interval<checked_result<R>> divide(
    const interval<T> & t,
    const interval<U> & u
){
//    assert(u.l != 0 || u.u != 0);
    return detail::minmax<checked_result<R>>(
    (u.l == 0)
    ? std::initializer_list<checked_result<R>> {
        checked::divide<R>(t.l, +1),
        checked::divide<R>(t.l, u.u),
        checked::divide<R>(t.u, +1),
        checked::divide<R>(t.u, u.u)
    }
    : (0 == u.u)
    ? std::initializer_list<checked_result<R>> {
        checked::divide<R>(t.l, u.l),
        checked::divide<R>(t.l, -1),
        checked::divide<R>(t.u, u.l),
        checked::divide<R>(t.u, -1)
    }
    : (u.u < 0 || 0 < u.l) // divisor range excludes 0
    ? std::initializer_list<checked_result<R>> {
        checked::divide<R>(t.l, u.l),
        checked::divide<R>(t.l, u.u),
        checked::divide<R>(t.u, u.l),
        checked::divide<R>(t.u, u.u)
    }
    : std::initializer_list<checked_result<R>> { // divisor includes 0
        checked::divide<R>(t.l, u.l),
        checked::divide<R>(t.l, -1),
        checked::divide<R>(t.l, +1),
        checked::divide<R>(t.l, u.u),

        checked::divide<R>(t.u, u.l),
        checked::divide<R>(t.u, -1),
        checked::divide<R>(t.u, +1),
        checked::divide<R>(t.u, u.u)
    }
    );
 }

// modulus of two intervals.  This will give a new range of for the modulus.  But
// it doesn't consider the possibility that the radix is zero.  This will
// have to be considered separately.
template<typename R, typename T, typename U>
constexpr interval<checked_result<R>> modulus(
    const interval<T> & t,
    const interval<U> & u
){
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

    // const R xa = (t.l > 0) ? t.l - 1 : t.l + 1; // a
    // const R xb = (t.u > 0) ? t.u - 1 : t.u + 1; // b
    const checked_result<R> xc = (u.l > 0) ? u.l - 1 : (u.l < 0) ? -(u.l + 1) : 0; // c
    const checked_result<R> xd = (u.u > 0) ? u.u - 1 : (u.u < 0) ? -(u.u + 1) : 0; // d

    // special care to void problem when inverting -128
    const checked_result<R> mxc = checked::subtract<R>(0, xc);
    const checked_result<R> mxd = checked::subtract<R>(0, xd);

    return detail::minmax<checked_result<R>>(
        std::initializer_list<checked_result<R>> {
            xc,
            mxc,
            xd,
            mxd,
            checked_result<R>(0)
        }
    );

}

template<typename R, typename T, typename U>
constexpr interval<checked_result<R>> left_shift(
    const interval<T> & t,
    const interval<U> & u
){
    return interval<checked_result<R>>{
        checked::left_shift<R>(t.l, u.l),
        checked::left_shift<R>(t.u, u.u),
    };
}

template<typename R, typename T, typename U>
constexpr interval<checked_result<R>> right_shift(
    const interval<T> & t,
    const interval<U> & u
){
    return interval<checked_result<R>>{
        checked::right_shift<R>(t.l, u.u),
        checked::right_shift<R>(t.u, u.l)
    };
}

template<typename R, typename T, typename U>
constexpr interval<checked_result<R>> bitwise_or(
    const interval<T> & t,
    const interval<U> & u
){
    return detail::minmax<checked_result<R>>(
        std::initializer_list<checked_result<R>> {
            checked::bitwise_or<R>(t.l, u.l),
            checked::bitwise_or<R>(t.l, u.u),
            checked::bitwise_or<R>(t.u, u.l),
            checked::bitwise_or<R>(t.u, u.u)
        }
    );
}

template<typename R, typename T, typename U>
constexpr interval<checked_result<R>> bitwise_and(
    const interval<T> & t,
    const interval<U> & u
){
    return detail::minmax<checked_result<R>>(
        std::initializer_list<checked_result<R>> {
            checked::bitwise_and<R>(t.l, u.l),
            checked::bitwise_and<R>(t.l, u.u),
            checked::bitwise_and<R>(t.u, u.l),
            checked::bitwise_and<R>(t.u, u.u)
        }
    );
}

template<typename R, typename T, typename U>
constexpr interval<checked_result<R>> bitwise_xor(
    const interval<T> & t,
    const interval<U> & u
){
    return detail::minmax<checked_result<R>>(
        std::initializer_list<checked_result<R>> {
            checked::bitwise_xor<R>(t.l, u.l),
            checked::bitwise_xor<R>(t.l, u.u),
            checked::bitwise_xor<R>(t.u, u.l),
            checked::bitwise_xor<R>(t.u, u.u)
        }
    );
}

template<typename R, typename T, typename U>
constexpr interval<checked_result<R>> intersection(
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
constexpr interval<checked_result<R>> union_interval(
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
