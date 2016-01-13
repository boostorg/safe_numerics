#ifndef BOOST_NUMERIC_POLICIES_HPP
#define BOOST_NUMERIC_POLICIES_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2015 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <stdexcept>
#include <functional>
#include <type_traits> // is_base_of, is_same
#include <exception>

namespace boost {
namespace numeric {

// run time trap handlers

// this would emulate the normal C/C++ behavior of permitting overflows
// and the like.
struct ignore_exception {
    static void no_error(const char * message) {}
    static void uninitialized_error(const char * message) {}
    static void overflow_error(const char * message) {}
    static void underflow_error(const char * message) {}
    static void range_error(const char * message) {}
    static void domain_error(const char * message) {}
};

// example - if you want to specify specific behavior for particular exception
// types, use this policy.  The most likely situation is where you don't have
// exception support and you want to trap "exceptions" by calling your own
// special functions.
template<
    void (*FUNCTION_NO_EXCEPTION)(const char *),
    void (*FUNCTION_UNINITIALIZED)(const char *),
    void (*FUNCTION_OVERFLOW)(const char *),
    void (*FUNCTION_UNDERFLOW)(const char *) = FUNCTION_OVERFLOW,
    void (*FUNCTION_RANGE)(const char *) = FUNCTION_OVERFLOW,
    void (*FUNCTION_DOMAIN)(const char *) = FUNCTION_OVERFLOW
>
struct no_exception_support {
    static void no_error(const char * message) {
        (*FUNCTION_NO_EXCEPTION)(message);
    }
    static void uninitialized_error(const char * message) {
        (*FUNCTION_UNINITIALIZED)(message);
    }
    static void overflow_error(const char * message) {
        (*FUNCTION_OVERFLOW)(message);
    }
    static void underflow_error(const char * message) {
        (FUNCTION_UNDERFLOW)(message);
    }
    static void range_error(const char * message) {
        FUNCTION_RANGE(message);
    }
    static void domain_error(const char * message) {
        FUNCTION_DOMAIN(message);
    }
};


// If an exceptional condition is detected at runtime throw the exception.
// map our exception list to the ones in stdexcept
struct throw_exception {
    static void no_error(const char * message) {
    }
    static void unintialized_error(const char * message) {
        throw std::invalid_argument(message);
    }
    static void overflow_error(const char * message) {
        throw std::overflow_error(message);
    }
    static void underflow_error(const char * message) {
        throw std::underflow_error(message);
    }
    static void range_error(const char * message) {
        throw std::range_error(message);
    }
    static void domain_error(const char * message) {
        throw std::domain_error(message);
    }
};

// use this policy to trap at compile time any operation which
// would otherwise trap at runtime.  Hence expressions such as i/j
// will trap at compile time unless j can be guaranteed to not be zero.

// meant to be trap the case where a program MIGHT throw an exception
struct trap_exception {
};

} // namespace numeric
} // namespace boost

#endif // BOOST_NUMERIC_POLICIES_HPP
