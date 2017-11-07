#ifndef BOOST_NUMERIC_EXCEPTION_OPERATIONS
#define BOOST_NUMERIC_EXCEPTION_OPERATIONS

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "exception.hpp"

namespace boost {
namespace numeric {

constexpr safe_numerics_error
operator+(
    const safe_numerics_error & t,
    const safe_numerics_error & u
) noexcept {
    const safe_numerics_error
        result[safe_numerics_error_count][safe_numerics_error_count]{
            // t == success
            {
                // u == ...
                safe_numerics_error::success,
                safe_numerics_error::positive_overflow_error,
                safe_numerics_error::negative_overflow_error,
                safe_numerics_error::range_error,
                safe_numerics_error::precision_overflow_error,
                safe_numerics_error::underflow_error,
                safe_numerics_error::domain_error,
                safe_numerics_error::implementation_defined_behavior,
                safe_numerics_error::undefined_behavior,
                safe_numerics_error::uninitialized_value
            },
            // t == positive_overflow_error,
            {
                // u == ...
                safe_numerics_error::domain_error,
                safe_numerics_error::positive_overflow_error,
                safe_numerics_error::negative_overflow_error,
                safe_numerics_error::range_error,
                safe_numerics_error::precision_overflow_error,
                safe_numerics_error::underflow_error,
                safe_numerics_error::domain_error,
                safe_numerics_error::implementation_defined_behavior,
                safe_numerics_error::undefined_behavior,
                safe_numerics_error::uninitialized_value
            },
            {}
        };
    return result[static_cast<int>(t)][static_cast<int>(u)];
}

} // numeric
} // boost
#endif // BOOST_NUMERIC_EXCEPTION_OPERATIONS
