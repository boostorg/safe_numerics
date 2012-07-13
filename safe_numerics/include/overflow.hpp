#ifndef BOOST_NUMERIC_SAFE_OVERFLOW_HPP
#define BOOST_NUMERIC_SAFE_OVERFLOW_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <stdexcept>
#include <boost/config.hpp>

namespace boost {
namespace numeric {

#ifndef BOOST_NO_EXCEPTIONS
    inline void overflow(char const * const msg) {
        throw std::range_error(msg);
    }
#else
    void overflow(char const * const msg);
#endif

} // namespace numeric
} // namespace boost

#endif // BOOST_NUMERIC_SAFE_OVERFLOW_HPP
