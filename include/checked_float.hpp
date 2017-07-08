#ifndef BOOST_NUMERIC_CHECKED_FLOAT_HPP
#define BOOST_NUMERIC_CHECKED_FLOAT_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2017 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// contains operation implementation of arithmetic operators
// on built-in floating point types.  The default implementation is to just
// invoke the operation with no checking.  These are overloaded
// for specific types such as integer, etc.

#include <type_traits> // std::is_floating_point, make_unsigned
/*
#include <limits>
#include <algorithm>   // std::max
#include <type_traits> // std::is_integer
#include <boost/utility/enable_if.hpp>

#include "safe_common.hpp"
#include "checked_result.hpp"
#include "utility.hpp"
#include "exception.hpp"
#include "safe_compare.hpp"
*/

namespace boost {
namespace numeric {
namespace checked {

template<class R, class T>
typename std::enable_if<
    std::is_floating_point<R>::value
    && std::is_floating_point<T>::value,
    checked_result<R>
>::type
constexpr checked_result<R>
cast(const T & t){
    return static_cast<R>(t);
}

template<class R, class T, class U>
typename std::enable_if<
    std::is_floating_point<R>::value
    && std::is_floating_point<T>::value
    && std::is_floating_point<U>::value,
    checked_result<R>
>::type
constexpr add(const T & t, const U & u) noexcept {
template<class R, class T, class U>
constexpr checked_result<R> add(const T & t, const U & u) noexcept {
    return t + u;
}

template<class R, class T, class U>
typename std::enable_if<
    std::is_floating_point<R>::value
    && std::is_floating_point<T>::value
    && std::is_floating_point<U>::value,
    checked_result<R>
>::type
constexpr checked_result<R> subtract(const T & t, const U & u) noexcept {
    return t - u;
}

template<class R, class T, class U>
typename std::enable_if<
    std::is_floating_point<R>::value
    && std::is_floating_point<T>::value
    && std::is_floating_point<U>::value,
    checked_result<R>
>::type
constexpr checked_result<R> multiply(const T & t, const U & u) noexcept {
    return t * u;
}

template<class R, class T, class U>
typename std::enable_if<
    std::is_floating_point<R>::value
    && std::is_floating_point<T>::value
    && std::is_floating_point<U>::value,
    checked_result<R>
>::type
constexpr checked_result<R> divide(const T & t, const U & u) noexcept {
    return t / u;
}

template<class R, class T, class U>
typename std::enable_if<
    std::is_floating_point<R>::value
    && std::is_floating_point<T>::value
    && std::is_floating_point<U>::value,
    checked_result<R>
>::type
constexpr checked_result<R> modulus(const T & t, const U & u) noexcept {
    return t % u;
}

template<class R, class T, class U>
typename std::enable_if<
    std::is_floating_point<R>::value
    && std::is_floating_point<T>::value
    && std::is_floating_point<U>::value,
    checked_result<R>
>::type
constexpr bool less_than(const T & t, const U & u) noexcept {
    return t < u;
}

template<class R, class T, class U>
typename std::enable_if<
    std::is_floating_point<R>::value
    && std::is_floating_point<T>::value
    && std::is_floating_point<U>::value,
    checked_result<R>
>::type
constexpr bool equal(const T & t, const U & u) noexcept {
    return t < u;
}

template<class R, class T, class U>
typename std::enable_if<
    std::is_floating_point<R>::value
    && std::is_floating_point<T>::value
    && std::is_floating_point<U>::value,
    checked_result<R>
>::type
constexpr checked_result<R> left_shift(const T & t, const U & u) noexcept {
    return t << u;
}

template<class R, class T, class U>
typename std::enable_if<
    std::is_floating_point<R>::value
    && std::is_floating_point<T>::value
    && std::is_floating_point<U>::value,
    checked_result<R>
>::type
constexpr checked_result<R> right_shift(const T & t, const U & u) noexcept {
    return t >> u;
}

template<class R, class T, class U>
typename std::enable_if<
    std::is_floating_point<R>::value
    && std::is_floating_point<T>::value
    && std::is_floating_point<U>::value,
    checked_result<R>
>::type
constexpr checked_result<R> bitwise_or(const T & t, const U & u) noexcept {
    return t | u;
}

template<class R, class T, class U>
typename std::enable_if<
    std::is_floating_point<R>::value
    && std::is_floating_point<T>::value
    && std::is_floating_point<U>::value,
    checked_result<R>
>::type
constexpr checked_result<R> bitwise_xor(const T & t, const U & u) noexcept {
    return t ^ u;
}

template<class R, class T, class U>
typename std::enable_if<
    std::is_floating_point<R>::value
    && std::is_floating_point<T>::value
    && std::is_floating_point<U>::value,
    checked_result<R>
>::type
constexpr checked_result<R> bitwise_and(const T & t, const U & u) noexcept {
    return t & u;
}

} // checked
} // numeric
} // boost

#endif // BOOST_NUMERIC_CHECKED_DEFAULT_HPP

