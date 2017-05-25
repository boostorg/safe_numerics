#ifndef BOOST_NUMERIC_EXCEPTION_POLICIES_HPP
#define BOOST_NUMERIC_EXCEPTION_POLICIES_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2015 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <exception>
#include <type_traits> // is_convertible

//#include "exception.hpp"

namespace boost {
namespace numeric {

enum class exception_handler {
    ignore_exception,
    throw_exception,
    trap_exception
};
template<
    exception_handler UV,
    exception_handler AE,
    exception_handler IDB,
    exception_handler UB
>
struct exception_policy {
    static exception_handler on_uninitialized_value(){
        return UV;
    }
    static exception_handler on_arithmetic_error(){
        return AE;
    }
    static exception_handler on_implementation_defined_behavior(){
        return IDB;
    }
    static exception_handler on_undefined_behavior(){
        return UB;
    }
};

// normal policy - permit runtime exceptions
using default_exception_policy = exception_policy<
    exception_handler::ignore_exception,
    exception_handler::throw_exception,
    exception_handler::ignore_exception,
    exception_handler::trap_exception
>;

// trap non portable C++ code at compile time but permit
// exceptions to be thrown for runtime errors
using strict_exception_policy = exception_policy<
    exception_handler::ignore_exception,
    exception_handler::throw_exception,
    exception_handler::trap_exception,
    exception_handler::trap_exception
>;

// use when any possible exceptions should be trapped at compile time
// but we're a little loose on things like bit shifts etc.
// this might be attractive choice for small embedded systems
using no_exceptions_policy = exception_policy<
    exception_handler::ignore_exception,
    exception_handler::trap_exception,
    exception_handler::ignore_exception,
    exception_handler::trap_exception
>;

// use when any possible exceptions should be trapped at compile time
// but we're a little loose on things like bit shifts etc.
// this might be attractive choice for small embedded systems
using strict_no_exceptions_policy = exception_policy<
    exception_handler::ignore_exception,
    exception_handler::trap_exception,
    exception_handler::trap_exception,
    exception_handler::trap_exception
>;
} // namespace numeric
} // namespace boost

#endif // BOOST_NUMERIC_EXCEPTION_POLICIES_HPP
