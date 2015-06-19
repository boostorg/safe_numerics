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
#include <algorithm> // min, max

#include "safe_base.hpp"
#include "checked_result.hpp"
#include "checked.hpp"

namespace boost {
namespace numeric {

template<typename R>
struct interval {
    checked_result<R> l;
    checked_result<R> u;
    template<typename T, typename U>
    SAFE_NUMERIC_CONSTEXPR interval(const T & lower, const U & upper) :
        l(lower),
        u(upper)
    {}
    SAFE_NUMERIC_CONSTEXPR interval(const interval<R> & rhs) :
        l(rhs.l),
        u(rhs.u)
    {}
    SAFE_NUMERIC_CONSTEXPR interval() :
        l(std::numeric_limits<R>::min()),
        u(std::numeric_limits<R>::max())
    {}
    SAFE_NUMERIC_CONSTEXPR bool no_exception() const {
        return l == checked_result<R>::exception_type::no_exception
        && u == checked_result<R>::exception_type::no_exception ;
    }
};

template<typename R, typename T, typename U>
SAFE_NUMERIC_CONSTEXPR interval<R> operator+(const interval<T> & t, const interval<U> & u){
    // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
    return interval<R>(
        checked::add<R>(static_cast<R>(t.l), static_cast<R>(u.l)),
        checked::add<R>(static_cast<R>(t.u), static_cast<R>(u.u))
    );
}

template<typename R, typename T, typename U>
SAFE_NUMERIC_CONSTEXPR interval<R> operator-(const interval<T> & t, const interval<U> & u){
    // adapted from https://en.wikipedia.org/wiki/Interval_arithmetic
    return interval<R>(
        checked::subtract<R>(static_cast<R>(t.l), static_cast<R>(u.u)),
        checked::subtract<R>(static_cast<R>(t.u), static_cast<R>(u.l))
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
        (u.l <= 0) ?
            interval<R>(
                0,
                checked_result<R>(
                    checked_result<R>::exception_type::domain_error,
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
                0,
                checked_result<R>(
                    checked_result<R>::exception_type::domain_error,
                    "interval divisor includes zero"
                )
            )
        :
            interval<R>(0, max(u.u, u.l))
        ;
}

} // umeric
} // boost

#endif // BOOST_NUMERIC_INTERVAL_HPP
