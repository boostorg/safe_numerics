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
#include <type_traits> // is_base_of, is_same
//#include <boost/config.hpp>

#include "concept/exception_policy.hpp"
#include "boost/concept/assert.hpp"

namespace boost {
namespace numeric {

// run time trap handlers

// this would emulate the normal C/C++ behavior of permitting overflows
// and the like.
struct ignore_exception {
    BOOST_CONCEPT_ASSERT((boost::numeric::ExceptionPolicy<ignore_exception>));
    static void overflow_error(const char * message) {}
    static void underflow_error(const char * message) {}
    static void range_error(const char * message) {}
};

// If an exceptional condition is detected at runtime throw the exception.
struct throw_exception {
    BOOST_CONCEPT_ASSERT((ExceptionPolicy<throw_exception>));
    static void overflow_error(const char * message) {
        throw std::overflow_error(message);
    }
    static void underflow_error(const char * message) {
        throw std::underflow_error(message);
    }
    static void range_error(const char * message) {
        throw std::domain_error(message);
    }
};

// example - if you want to specify specific behavior for particular exception
// types, use this policy.  The most likely situation is where you don't have
// exception support and you want to trap "exceptions" by calling your own
// special functions.
template<
    void (*OVERFLOW)(const char *),
    void (*UNDERFLOW)(const char *),
    void (*RANGE)(const char *)
>
struct no_exception_support {
    BOOST_CONCEPT_ASSERT((boost::numeric::ExceptionPolicy<no_exception_support>));
    static void overflow_error(const char * message) {
        OVERFLOW(message);
    }
    static void underflow_error(const char * message) {
        UNDERFLOW(message);
    }
    static void range_error(const char * message) {
        RANGE(message);
    }
};

// use this policy to trap at compile time any operation which
// would otherwise trap at runtime.  Hence expressions such as i/j
// will trap at compile time unless j can be guaranteed to not be zero.

struct trap_exception {
    // note: we can't use the concept here.  It "tests" that the policy is correct
    // by calling the functions on it.  But the purpose of these functions are
    // to fail!!!. So comment this out as a reminder
    // BOOST_CONCEPT_ASSERT((boost::numeric::ExceptionPolicy<trap_exception>));
    template<class T>
    static void overflow_error(const T *) {
        static_assert(std::is_void<T>::value, "overflow_error");
    }
    template<class T>
    static void underflow_error(const T *) {
        static_assert(std::is_void<T>::value, "underflow_error");
    }
    template<class T>
    static void range_error(const T *) {
        static_assert(std::is_void<T>::value, "range_error");
    }
};

// policies structure
// note: if there's no exception policy then any operation which
// could result in a runtime exception will trap during compile
// time.  For example i = i / j will not be permited because we
// can't know at compiler time that j can never be zero

struct policies_tag {};
template<class P, class E = throw_exception>
struct policies : public policies_tag {
    typedef P t_p; // promotion policy
    typedef E t_e; // exception policy
};

template<class P>
struct is_policies : public
    std::is_base_of<boost::numeric::policies_tag, P>
{};

template<class P>
struct get_promotion_policy {
    static_assert(boost::numeric::is_policies<P>::value, "P is not a pair of policies");
    typedef typename P::t_p type;
};
template<class P>
struct get_exception_policy {
    static_assert(boost::numeric::is_policies<P>::value, "P is not a pair of policies");
    typedef typename P::t_e type;
};

} // namespace numeric
} // namespace boost

#endif // BOOST_NUMERIC_POLICIES_HPP
