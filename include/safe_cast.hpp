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

#include "safe_common.hpp"
#include "safe_base_operations.hpp"

namespace boost {
namespace numeric {

template<class R, class U>
R safe_cast(const U & u) {
    checked_result<R> r = checked::detail::cast<R>(base_value(u));
    r.template dispatch<typename P::exception_policy>();
    return static_cast<R>(r);
}

} // numeric
} // boost

#endif // BOOST_NUMERIC_SAFE_CAST_HPP
