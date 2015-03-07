#ifndef BOOST_NUMERIC_CONCEPT_EXCEPTION_POLICY_HPP
#define BOOST_NUMERIC_CONCEPT_EXCEPTION_POLICY_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2015 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "boost/concept/assert.hpp"
#include "boost/concept/usage.hpp"

namespace boost {
namespace numeric {

template<class T>
struct ExceptionPolicy {
    const char * message;
    BOOST_CONCEPT_USAGE(ExceptionPolicy){
        T::overflow_error(message);
        T::underflow_error(message);
        T::range_error(message);
    }
};

} // numeric
} // boost

#endif // BOOST_NUMERIC_CONCEPT_EXCEPTION_POLICY_HPP
