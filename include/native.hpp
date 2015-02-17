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

#include <type_traits>
#include <limits>

#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/print.hpp>

// policy which creates results types equal to that of C++ promotions.
// Using the policy will permit the program to build and run in release
// mode which is identical to that in debug mode except for the fact
// that errors aren't trapped.

namespace boost {
namespace numeric {

// forward declaration
/*
template<
    boost::intmax_t MIN,
    boost::intmax_t MAX,
    class P // policies
>
class safe_signed_range;

template<
    boost::uintmax_t MIN,
    boost::uintmax_t MAX,
    class P // default_policies
>
class safe_unsigned_range;
*/
template<
    class B,
    class P
>
struct safe;

struct native {
    template<
        typename T,
        typename U,
        typename P
    >
    struct addition_result {

        typedef decltype(
            typename boost::numeric::base_type<T>::type()
          + typename boost::numeric::base_type<U>::type()
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
            const result_base_type &a,
            const result_base_type &b
        ){
            return a + b;
        }

        // someday maybe we can replace this with
        typedef safe<result_base_type, P> type;
        /*
        typedef typename ::boost::mpl::if_c<
            std::numeric_limits<result_base_type>::is_signed,
            safe_signed_range<
                max(
                    min_value<result_base_type>(),
                    sum(min_value<T>(), min_value<U>())
                ),
                min(max_value<result_base_type>(), max_value<T>() + max_value<U>()),
                P
            >,
            safe_unsigned_range<
                max(min_value<result_base_type>(), min_value<T>() + min_value<U>()),
                min(max_value<result_base_type>(), max_value<T>() + max_value<U>()),
                P
            >
        >::type type;
        */
    };
};

} // numeric
} // boost

#endif // BOOST_NUMERIC_NATIVE_HPP
