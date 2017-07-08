#ifndef BOOST_NUMERIC_CHECKED_DEFAULT_HPP
#define BOOST_NUMERIC_CHECKED_DEFAULT_HPP

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
// on built-in types.  The default implementation is to just
// invoke the operation with no checking.  These are overloaded
// for specific types such as integer, etc.

/*
#include <limits>
#include <type_traits> // is_fundamental, make_unsigned
#include <algorithm>   // std::max
#include <type_traits> // std::is_integer
#include <boost/utility/enable_if.hpp>

#include "safe_common.hpp"
#include "checked_result.hpp"
#include "utility.hpp"
#include "exception.hpp"
#include "safe_compare.hpp"
*/

// implement the equivant of template partial specialization for functions

// what we need is
// a) a default implementation of add, subtract, etc which just
// implements the standard operations and returns the result
// b) specific implementations to be called from safe implementation
// such as safe<int> ... and someday maybe money<T, D> ...
//
// What we need is partial function specialization - but this doesn't
// exist in C++ (yet?).  But particial specialization of structures DOES
// exist.  So put our functions into a class which can then be
// partially specialized.  Finally. add a function interface to so that
// data types can be deduced from the function call.  We now have
// the equivalent of partial function template specialization.

// usage example: checked<int>::add(t, u) ...

namespace boost {
namespace numeric {

// main function object which contains functions which handle
// primitives which haven't been overriden

template<typename R, typename T, class Default = void>
struct checked_unary_operation{
    constexpr static checked_result<R>
    cast(const T & t){
        return static_cast<R>(t);
    }
};

template<typename R, typename T, typename U, class Default = void>
struct checked_binary_operation{
    constexpr static checked_result<R> add(const T & t, const U & u) noexcept {
        return t + u;
    }
    constexpr static checked_result<R> subtract(const T & t, const U & u) noexcept {
        return t - u;
    }
    constexpr static checked_result<R> multiply(const T & t, const U & u) noexcept {
        return t * u;
    }
    constexpr static checked_result<R> divide(const T & t, const U & u) noexcept {
        return t / u;
    }
    constexpr static checked_result<R> modulus(const T & t, const U & u) noexcept {
        return t % u;
    }
    constexpr static bool less_than(const T & t, const U & u) noexcept {
        return t < u;
    }
    constexpr static bool equal(const T & t, const U & u) noexcept {
        return t < u;
    }
    constexpr static checked_result<R> left_shift(const T & t, const U & u) noexcept {
        return t << u;
    }
    constexpr static checked_result<R> right_shift(const T & t, const U & u) noexcept {
        return t >> u;
    }
    constexpr static checked_result<R> bitwise_or(const T & t, const U & u) noexcept {
        return t | u;
    }
    constexpr static checked_result<R> bitwise_xor(const T & t, const U & u) noexcept {
        return t ^ u;
    }
    constexpr static checked_result<R> bitwise_and(const T & t, const U & u) noexcept {
        return t & u;
    }
};

namespace checked {

// implement function call interface so that types other than
// the result type R can be deduced from the function parameters.

template<typename R, typename T>
constexpr checked_result<R> cast(const T & t) noexcept {
    return checked_unary_operation<R, T, void>::cast(t);
}
template<typename R, typename T, typename U>
constexpr checked_result<R> add(const T & t, const U & u) noexcept {
    return checked_binary_operation<R, T, U>::add(t, u);
}
template<typename R, typename T, typename U>
constexpr checked_result<R> subtract(const T & t, const U & u) noexcept {
    return checked_binary_operation<R, T, U>::subtract(t, u);
}
template<typename R, typename T, typename U>
constexpr checked_result<R> multiply(const T & t, const U & u) noexcept {
    return checked_binary_operation<R, T, U>::multiply(t, u);
}
template<typename R, typename T, typename U>
constexpr checked_result<R> divide(const T & t, const U & u) noexcept {
    return checked_binary_operation<R, T, U>::divide(t, u);
}
template<typename R, typename T, typename U>
constexpr checked_result<R> modulus(const T & t, const U & u) noexcept {
    return checked_binary_operation<R, T, U>::modulus(t, u);
}
template<typename R, typename T, typename U>
constexpr checked_result<bool> less_than(const T & t, const U & u) noexcept {
    return checked_binary_operation<R, T, U>::less_than(t, u);
}
template<typename R, typename T, typename U>
constexpr checked_result<bool> equal(const T & t, const U & u) noexcept {
    return checked_binary_operation<R, T, U>::equal(t, u);
}
template<typename R, typename T, typename U>
constexpr checked_result<R> left_shift(const T & t, const U & u) noexcept {
    return checked_binary_operation<R, T, U>::left_shift(t, u);
}
template<typename R, typename T, typename U>
constexpr checked_result<R> right_shift(const T & t, const U & u) noexcept {
    return checked_binary_operation<R, T, U>::right_shift(t, u);
}
template<typename R, typename T, typename U>
constexpr checked_result<R> bitwise_or(const T & t, const U & u) noexcept {
    return checked_binary_operation<R, T, U>::bitwise_or(t, u);
}
template<typename R, typename T, typename U>
constexpr checked_result<R> bitwise_xor(const T & t, const U & u) noexcept {
    return checked_binary_operation<R, T, U>::bitwise_or(t, u);
}
template<typename R, typename T, typename U>
constexpr checked_result<R> bitwise_and(const T & t, const U & u) noexcept {
    return checked_binary_operation<R, T, U>::bitwise_and(t, u);
}

} // checked
} // numeric
} // boost

#endif // BOOST_NUMERIC_CHECKED_DEFAULT_HPP

