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

// contains operations for doing checked aritmetic on NATIVE
// C++ types.

#include "concept/exception_policy.hpp"

namespace boost {
namespace numeric {

enum class exception_type {
    no_exception,
    overflow_error,
    underflow_error,
    range_error,
    domain_error,
    uninitialized
};

template<class EP>
constexpr void
dispatch(const exception_type & e, char const * const msg){
    switch(e){
    case exception_type::overflow_error:
        EP::overflow_error(msg);
        break;
    case exception_type::underflow_error:
        EP::underflow_error(msg);
        break;
    case exception_type::range_error:
        EP::range_error(msg);
        break;
    case exception_type::domain_error:
        EP::domain_error(msg);
        break;
    case exception_type::uninitialized:
        EP::domain_error(msg);
        break;
    case exception_type::no_exception:
        EP::no_error(msg);
        break;
    default:
        assert(false);
        break;
    }
}

} // numeric
} // boost

#endif // BOOST_NUMERIC_CHECKED_RESULT
