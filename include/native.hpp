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

#include <boost/limits.hpp>
#include <boost/integer_traits.hpp>

#include "safe_base_operations.hpp"
#include "policies.hpp"

// policy which creates results types equal to that of C++ promotions.
// Using the policy will permit the program to build and run in release
// mode which is identical to that in debug mode except for the fact
// that errors aren't trapped.

namespace boost {
namespace numeric {

template<
    class T,
    class P
>
struct safe;

template<
    boost::intmax_t MIN,
    boost::intmax_t MAX,
    class P
>
class safe_signed_range;

template<
    boost::uintmax_t MIN,
    boost::uintmax_t MAX,
    class P
>
class safe_unsigned_range;

struct native {
    template<
        typename T,
        typename U
    >
    struct addition_result {
        typedef decltype(
            typename boost::numeric::base_type<T>::type()
          + typename boost::numeric::base_type<U>::type()
        ) result_base_type;

        /*
        template<class TX, class UX>
        constexpr static const TX & max(const TX & a, const UX & b) {
            return (a < b) ? b : a;
        }
        template<class TX, class UX>
        constexpr static const TX & min(const TX & a, const UX & b) {
            return (a < b) ? a : b;
        }

        template<class TX>
        constexpr static const TX min_value(){
            return std::numeric_limits<TX>::min();
        }
        template<class TX>
        constexpr static const TX max_value(){
            return std::numeric_limits<TX>::max();
        }
        typedef typename boost::mpl::if_<
            boost::is_signed<result_base_type>,
            boost::numeric::safe_signed_range<
                max(min_value<result_base_type>(), min_value<T>() + min_value<U>()),
                min(max_value<result_base_type>(), max_value<T>() + max_value<U>()),
                native
            >,
            boost::numeric::safe_unsigned_range<
                max(min_value<result_base_type>(), min_value<T>() + min_value<U>()),
                min(max_value<result_base_type>(), max_value<T>() + max_value<U>()),
                native
            >
        >::type type;
        */
        typedef boost::numeric::safe<result_base_type, native> type;
    };
};

#if  0
struct automatic {
    template<
        typename T,
        typename U
    >
    struct addition_result {
        typedef boost::numeric::safe_unsigned_range<
            std::numeric_limits<T>::min() + std::numeric_limits<U>::min(),
            std::numeric_limits<T>::max() + std::numeric_limits<U>::max(),
        > type;
    };
};
#endif

} // numeric
} // boost

#endif // BOOST_NUMERIC_NATIVE_HPP
