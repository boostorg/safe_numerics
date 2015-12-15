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
    constexpr static void no_error(const char * message) {}
    constexpr static void uninitialized_error(const char * message) {}
    constexpr static void overflow_error(const char * message) {}
    constexpr static void underflow_error(const char * message) {}
    constexpr static void range_error(const char * message) {}
    constexpr static void domain_error(const char * message) {}
};

// example - if you want to specify specific behavior for particular exception
// types, use this policy.  The most likely situation is where you don't have
// exception support and you want to trap "exceptions" by calling your own
// special functions.
template<
    void (*NO_EXCEPTION)(const char *),
    void (*UNINITIALIZED)(const char *),
    void (*OVERFLOW)(const char *),
    void (*UNDERFLOW)(const char *) = *OVERFLOW,
    void (*RANGE)(const char *) = *OVERFLOW,
    void (*DOMAIN)(const char *) = *OVERFLOW
>
struct no_exception_support {
    constexpr static void no_error(const char * message) {
        NO_EXCEPTION(message);
    }
    constexpr static void uninitialized_error(const char * message) {
        UNINITIALIZED(message);
    }
    constexpr static void overflow_error(const char * message) {
        OVERFLOW(message);
    }
    constexpr static void underflow_error(const char * message) {
        UNDERFLOW(message);
    }
    constexpr static void range_error(const char * message) {
        RANGE(message);
    }
    constexpr static void domain_error(const char * message) {
        DOMAIN(message);
    }
};

// If an exceptional condition is detected at runtime throw the exception.
// map our exception list to the ones in stdexcept
struct throw_exception {
    constexpr static void no_error(const char * message) {
    }
    constexpr static void unintialized_error(const char * message) {
        throw std::invalid_argument(message);
    }
    constexpr static void overflow_error(const char * message) {
        throw std::overflow_error(message);
    }
    constexpr static void underflow_error(const char * message) {
        throw std::underflow_error(message);
    }
    constexpr static void range_error(const char * message) {
        throw std::range_error(message);
    }
    constexpr static void domain_error(const char * message) {
        throw std::domain_error(message);
    }
};

// use this policy to trap at compile time any operation which
// would otherwise trap at runtime.  Hence expressions such as i/j
// will trap at compile time unless j can be guaranteed to not be zero.

/*
struct trap_exception {
    template<class T>
    constexpr static void overflow_error(const T *) {
        static_assert(std::is_void<T>::value, "overflow_error");
    }
    template<class T>
    constexpr static void underflow_error(const T *) {
        static_assert(std::is_void<T>::value, "underflow_error");
    }
    template<class T>
    constexpr static void range_error(const T *) {
        static_assert(std::is_void<T>::value, "range_error");
    }
    template<class T>
    constexpr static void domain_error(const T *) {
        static_assert(std::is_void<T>::value, "domain_error");
    }
};

template<typename T>
bool noconstexpr(const T & t){
    return false;
}

struct trap_exception {
    template<class T>
    constexpr static bool overflow_error(const T & t) {
        return noconstexpr(t);
        //static_assert(noconstexpr(t), "overflow_error");
    }
    template<class T>
    constexpr static void underflow_error(const T & t) {
        noconstexpr(t);
        //static_assert(std::is_void<T>::value, "underflow_error");
    }
    template<class T>
    constexpr static void range_error(const T & t) {
        noconstexpr(t);
        //static_assert(std::is_void<T>::value, "range_error");
    }
    template<class T>
    constexpr static void domain_error(const T & t) {
        noconstexpr(t);
        //static_assert(std::is_void<T>::value, "domain_error");
    }
};
*/

// meant to be trap the case where a program MIGHT throw an exception
struct trap_exception {
/*

    constexpr static void range_error(const char * message) {
        throw std::range_error(message);
    }
    constexpr static void domain_error(const char * message) {
        //static_assert(std::is_void<T>::value, "domain_error");
    }
*/};

} // namespace numeric
} // namespace boost

#endif // BOOST_NUMERIC_POLICIES_HPP
