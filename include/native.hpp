#ifndef BOOST_NUMERIC_NATIVE_HPP
#define BOOST_NUMERIC_NATIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//#include <algorithm> // max, min
#include <type_traits>

#include <boost/mpl/if.hpp>
#include <boost/mpl/print.hpp>

#include "policies.hpp"
#include "safe_base_operations.hpp"
#include "limits.hpp"
//#include "safe_integer.hpp"
//#include "safe_range.hpp"

// policy which creates results types equal to that of C++ promotions.
// Using the policy will permit the program to build and run in release
// mode which is identical to that in debug mode except for the fact
// that errors aren't trapped.

namespace boost {
namespace numeric {

struct native {
    template<
        typename T,
        typename U
    >
    struct addition_result {
        typedef decltype(
            typename base_type<T>::type()
          + typename base_type<U>::type()
        ) result_base_type;

        template<class TX>
        constexpr static result_base_type min_value() {
            return std::numeric_limits<TX>::min();
        }
        template<class TX>
        constexpr static result_base_type max_value() {
            return std::numeric_limits<TX>::max();
        }

        constexpr static result_base_type max(
            const result_base_type & a,
            const result_base_type & b
        ){
            return (a < b) ? b : a;
        }
        constexpr static result_base_type min(
            const result_base_type & a,
            const result_base_type & b
        ){
            return (a < b) ? a : b;
        }

        constexpr static result_base_type sum(
            const result_base_type & a,
            const result_base_type & b
        ){
            return a + b;
        }

        typedef safe<result_base_type, native> type;

        // someday maybe we can replace this with
        /*
        typedef typename ::boost::mpl::if_<
            std::is_signed<result_base_type>,
            safe_signed_range<
            typedef safe_unsigned_range<
                min_value<T>() + min_value<U>(),
                max_value<T>() + max_value<U>(),
                native
            >,
            safe_unsigned_range<
                min_value<T>() + min_value<U>(),
                max_value<T>() + max_value<U>(),
                native
            >
        >::type type;
        */
    };
};

} // numeric
} // boost

#endif // BOOST_NUMERIC_NATIVE_HPP
