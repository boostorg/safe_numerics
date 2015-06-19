#ifndef BOOST_NUMERIC_SAFE_COMPARE_HPP
#define BOOST_NUMERIC_SAFE_COMPARE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <type_traits>
#include <limits>

#include "checked.hpp"

namespace boost {
namespace numeric {
namespace safe_compare {
    template<class T, class U>
    SAFE_NUMERIC_CONSTEXPR bool less_than(const T & lhs, const U & rhs) {
        return checked::less_than(base_value(lhs), base_value(rhs));
    }

    template<class T, class U>
    SAFE_NUMERIC_CONSTEXPR bool greater_than(const T & lhs, const U & rhs) {
        return checked::greater_than(base_value(lhs), base_value(rhs));
    }

    template<class T, class U>
    SAFE_NUMERIC_CONSTEXPR bool equal(const T & lhs, const U & rhs) {
        return checked::equal(base_value(lhs), base_value(rhs));
    }

    template<class T, class U>
    SAFE_NUMERIC_CONSTEXPR bool not_equal(const T & lhs, const U & rhs) {
        return checked::not_equal(base_value(lhs), base_value(rhs));
    }

    template<class T, class U>
    SAFE_NUMERIC_CONSTEXPR bool greater_than_equal(const T & lhs, const U & rhs) {
        return checked::greater_than_equal(base_value(lhs), base_value(rhs));
    }

    template<class T, class U>
    SAFE_NUMERIC_CONSTEXPR bool less_than_equal(const T & lhs, const U & rhs) {
        return checked::less_than_equal(base_value(lhs), base_value(rhs));
    }
} // safe_compare
} // numeric
} // boost

#endif // BOOST_NUMERIC_SAFE_COMPARE_HPP
