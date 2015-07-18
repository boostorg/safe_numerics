#ifndef BOOST_NUMERIC_CPP_HPP
#define BOOST_NUMERIC_CPP_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// policy which creates results types equal to that of C++ promotions.
// Using the policy will permit the program to build and run in release
// mode which is identical to that in debug mode except for the fact
// that errors aren't trapped. 

#include <type_traits> // integral constant, remove_cv
#include <limits>
#include <boost/integer.hpp> // integer type selection
#include <boost/mpl/if.hpp>

#include "safe_base.hpp"

namespace boost {
namespace numeric {

// in C++ the following rules govern integer arithmetic

// This policy is use to emulate another compiler/machine architecture
// For example, a Z80 has 8 bit char, 16 bit short, 16 bit int, 32 bit long.  So one
// would use cpp<8, 16, 16, 32, 32> to test programs destined to run on a Z80

// Follow section 5 of the standard.
template<
    int CharBits,
    int ShortBits,
    int IntBits,
    int LongBits,
    int LongLongBits
>
struct cpp {
    template<typename T>
    using normalize = typename std::make_signed<typename std::remove_cv<T>::type>::type;

    using local_char_type = typename boost::int_t<CharBits>::exact;
    using local_short_type = typename boost::int_t<ShortBits>::exact;
    using local_int_type = typename boost::int_t<IntBits>::exact;
    using local_long_type = typename boost::int_t<LongBits>::exact;
    using local_long_long_type = typename boost::int_t<LongLongBits>::exact;

    // section 4.13 integer conversion rank
    template<class T>
    using rank =
        typename boost::mpl::if_<
            std::is_same<bool, normalize<T> >,
            std::integral_constant<int, 0>,
        typename boost::mpl::if_c<
            sizeof(local_char_type) == sizeof(T),
            std::integral_constant<int, 1>,
        typename boost::mpl::if_c<
            sizeof(local_short_type) == sizeof(T),
            std::integral_constant<int, 2>,
        typename boost::mpl::if_c<
            sizeof(local_int_type) == sizeof(T),
            std::integral_constant<int, 3>,
        typename boost::mpl::if_c<
            sizeof(local_long_type) == sizeof(T),
            std::integral_constant<int, 4>,
        typename boost::mpl::if_c<
            sizeof(local_long_long_type) == sizeof(T),
            std::integral_constant<int, 5>,
            void
        >::type >::type >::type >::type >::type >::type;

    // section 4.5 integral promotions
    template<class T>
    using integral_promotion = typename boost::mpl::if_c<
        rank<T>::value < rank<local_int_type>::value,
        local_int_type,
        T
    >::type;

    // convert smaller of two types to the size of the larger
    template<class T, class U>
    using higher_ranked_type = typename boost::mpl::if_c<
        (rank<T>::value < rank<U>::value),
        U,
        T
    >::type;

    // note presumption that T & U don't have he same sign
    // if that's not true, these won't work
    template<class T, class U>
    using select_signed = typename boost::mpl::if_c<
        std::numeric_limits<T>::is_signed,
        T,
        U
    >::type;

    template<class T, class U>
    using select_unsigned = typename boost::mpl::if_c<
        std::numeric_limits<T>::is_signed,
        U,
        T
    >::type;

    // section 5 - usual arithmetic conversions
    template<typename T, typename U>
    using usual_arithmetic_conversions =
        // clause 0 - if both operands have the same type
        typename boost::mpl::if_c<
            std::is_same<T, U>::value,
            // no further conversion is needed
            T,
        // clause 1 - otherwise if both operands have the same sign
        typename boost::mpl::if_c<
            std::numeric_limits<T>::is_signed
            == std::numeric_limits<U>::is_signed,
            // convert to the higher ranked type
            higher_ranked_type<T, U>,
        // clause 2 - otherwise if the rank of he unsigned type exceeds
        // the rank of the of the signed type
        typename boost::mpl::if_c<
            rank< select_unsigned<T, U>>::value
            >= rank< select_signed<T, U>>::value,
            // use unsigned type
            select_unsigned<T, U>,
        // clause 3 - otherwise if the type of the signed integer type can
        // represent all the values of the unsigned type
        typename boost::mpl::if_c<
            std::numeric_limits< select_signed<T, U> >::digits >=
            std::numeric_limits< select_unsigned<T, U> >::digits,
            // use signed type
            select_signed<T, U>,
        // clause 4 - otherwise use unsigned version of the signed type
            typename std::make_signed< select_signed<T, U> >
        >::type >::type >::type >::type;

    template<typename T, typename U, typename P, typename E>
    struct safe_type_promotion {
        typedef typename base_type<T>::type base_type_t;
        typedef typename base_type<U>::type base_type_u;
        typedef usual_arithmetic_conversions<
            integral_promotion<base_type_t>,
            integral_promotion<base_type_u>
        > result_base_type;

        typedef safe_base<
            result_base_type,
            std::numeric_limits<result_base_type>::min(),
            std::numeric_limits<result_base_type>::max(),
            P,
            E
        > type;
    };

    template<typename T, typename U, typename P, typename E>
    struct addition_result {
       using type = typename  safe_type_promotion<T, U, P, E>::type;
    };

};

}   // numeric
}   // boost

#endif // BOOST_NUMERIC_cpp_HPP
