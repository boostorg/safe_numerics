#ifndef BOOST_NUMERIC_SAFE_CAST_HPP
#define BOOST_NUMERIC_SAFE_CAST_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/mpl/plus.hpp>
#include <boost/mpl/less.hpp>
#include <boost/mpl/greater.hpp>

//#include "overflow.hpp"
#include "safe_compare.hpp"

namespace boost {
namespace numeric {

template<class T, class U>
T safe_cast(const U & u) {
    if(std::numeric_limits<T>::is_unsigned)
        if(u < 0)
            overflow("casting alters value");
    if(safe_compare::greater_than(u, std::numeric_limits<T>::max()))
        overflow("safe range overflow");
    if(safe_compare::less_than(u, std::numeric_limits<T>::min()))
        overflow("safe range underflow");
    return static_cast<T>(u);
}

} // numeric
} // boost

#endif // BOOST_NUMERIC_SAFE_CAST_HPP
