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

#include "checked.hpp"

// policy which creates results types and values equal to that of C++ promotions.
// When used in conjunction with a desired exception policy, traps errors but
// does not otherwise alter the results produced by the program using it.
namespace boost {
namespace numeric {

struct native {
    // Standard C++ type promotion for expressions doesn't depend
    // on the operation being performed so we can just as well
    // use any operation to determine it.  We choose + for this
    // purpose.

    template<typename T, typename U>
    using additive_operator_type =
        decltype(
            typename base_type<T>::type()
            + typename base_type<U>::type()
        );
    template<typename T, typename U>
    using multiplicative_operator_type =
        decltype(
            typename base_type<T>::type()
            * typename base_type<U>::type()
        );
    // note: right/left shift require integer arguments
    template<typename T, typename U>
    using bitwise_shift_operator_type =
        decltype(
            typename base_type<T>::type()
            << typename base_type<U>::type()
        );
    template<typename T, typename U>
    using bitwise_logic_operator_type =
        decltype(
            typename base_type<T>::type()
            & typename base_type<U>::type()
        );

    template<typename T, typename U>
    struct addition_result {
        using type = additive_operator_type<T, U>;
    };
    template<typename T, typename U>
    struct subtraction_result {
        using type = additive_operator_type<T, U>;
    };
    template<typename T, typename U>
    struct multiplication_result {
        using type = multiplicative_operator_type<T, U>;
    };
    template<typename T, typename U>
    struct division_result {
        using type = multiplicative_operator_type<T, U>;
    };

    // forward to correct divide implementation
    template<class R, class T, class U>
    checked_result<R>
    static constexpr divide(
        const T & t,
        const U & u
    ){
        return checked::divide<R>(t, u);
    }

    template<typename T, typename U>
    struct modulus_result {
        using type = multiplicative_operator_type<T, U>;
    };

    // forward to correct modulus implementation
    template<class R, class T, class U>
    checked_result<R>
    static constexpr modulus(
        const T & t,
        const U & u
    ){
        return checked::modulus<R>(t, u);
    }

    template<typename T, typename U>
    struct left_shift_result {
        using type = bitwise_shift_operator_type<T, U>;
    };
    template<typename T, typename U>
    struct right_shift_result {
        using type = bitwise_shift_operator_type<T, U>;
    };

    template<typename T, typename U>
    struct bitwise_result {
        using type = bitwise_logic_operator_type<T, U>;
    };
};

} // numeric
} // boost

#endif // BOOST_NUMERIC_NATIVE_HPP
