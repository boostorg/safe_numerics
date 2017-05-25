#ifndef BOOST_NUMERIC_EXCEPTION
#define BOOST_NUMERIC_EXCEPTION

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// contains error indicators for results of doing checked
// arithmetic on native C++ types

#include <stdexcept>
#include <algorithm>

#include "exception_policies.hpp"
#include "concept/exception_policy.hpp"

namespace boost {
namespace numeric {

// in spite of the similarity, this list is distinct from the exceptions
// listed in documentation for std::exception.
enum class exception_type {
    no_exception,
    positive_overflow_error,
    negative_overflow_error,
    underflow_error,
    range_error,
    domain_error,
    implementation_defined_behavior,
    undefined_behavior,
    uninitialized
};

enum class exception_group {
    no_exception,
    uninitialized_value,
    arithmetic_error,
    implementation_defined_behavior,
    undefined_behavior
};

// translate exception type to exception handler type
constexpr exception_group
group(const exception_type & et){
    // we can't use standard algorithms since we want this to be constexpr
    // this brute force solution is simple and pretty fast anyway
    switch(et){
    case exception_type::negative_overflow_error:
    case exception_type::underflow_error:
    case exception_type::range_error:
    case exception_type::domain_error:
    case exception_type::positive_overflow_error:
        return exception_group::arithmetic_error;
    case exception_type::undefined_behavior:
        return exception_group::undefined_behavior;
    case exception_type::implementation_defined_behavior:
        return exception_group::implementation_defined_behavior;
    case exception_type::uninitialized:
        return exception_group::uninitialized_value;
    case exception_type::no_exception:
        return exception_group::no_exception;
    }
}

template<class EP>
constexpr exception_handler
handler(const exception_group & eg){
    // we can't use standard algorithms since we want this to be constexpr
    // this brute force solution is simple and pretty fast anyway
    switch(eg){
    case exception_group::no_exception:
        return exception_handler::ignore_exception;
    case exception_group::uninitialized_value:
        return EP::on_uninitialized_value();
    case exception_group::arithmetic_error:
        return EP::on_arithmetic_error();
    case exception_group::implementation_defined_behavior:
        return EP::on_implementation_defined_behavior();
    case exception_group::undefined_behavior:
        return EP::on_undefined_behavior();
    default:
        assert(false);
    }
}

constexpr void
throw_exception(const exception_type & e, char const * const & msg){
    switch(e){
    case exception_type::positive_overflow_error:
    case exception_type::negative_overflow_error:
        throw std::overflow_error(msg);
    case exception_type::underflow_error:
        throw std::underflow_error(msg);
    case exception_type::range_error:
        throw std::range_error(msg);
    case exception_type::domain_error:
        throw std::domain_error(msg);
    case exception_type::implementation_defined_behavior:
    case exception_type::undefined_behavior:
    case exception_type::uninitialized:
    case exception_type::no_exception:
    default:
        assert(false);
    }
};

template<class EP>
constexpr void
dispatch(const exception_type & e, char const * const & msg){
    const exception_group eg = group(e);
    const exception_handler eh = handler<EP>(eg);
    switch(eh){
    case exception_handler::ignore_exception:
        return;
    case exception_handler::throw_exception:
        throw_exception(e, msg);
    case exception_handler::trap_exception:
        assert(false);
    default:
        assert(false);
    }

}

} // numeric
} // boost

#endif // BOOST_NUMERIC_CHECKED_RESULT
