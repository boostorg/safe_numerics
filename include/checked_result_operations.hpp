  #ifndef BOOST_NUMERIC_CHECKED_RESULT_OPERATIONS
#define BOOST_NUMERIC_CHECKED_RESULT_OPERATIONS

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Implemenation of arithmetic on "extended" integers.
// extended integers are
//     a) an interger range
//     b) extra elements +inf, -inf, indeterminant
//
// arithmetic operations are closed on the set of extended integers
// but operations are not associative when they result in the
// extensions +inf, -inf, and indeterminant
//
// in this code, the type "checked_result<T>" where T is some
// integer type is an "extended" integer.

#include <cassert>

#include <boost/logic/tribool.hpp>

#include "checked_result.hpp"
#include "checked_integer.hpp"

//////////////////////////////////////////////////////////////////////////
// the following idea of "value_type" is used by several of the operations
// defined by checked_result arithmetic.

namespace boost {
namespace numeric {

//////////////////////////////////////////////////////////////////////////
// implement C++ operators for check_result<T>

template<class T>
struct sum_value_type {
    // characterization of various values
    const enum flag {
        known_value = 0,
        less_than_min,
        greater_than_max,
        indeterminant,
        count
    } m_flag;
    constexpr enum flag to_flag(const checked_result<T> & t) const {
        switch(static_cast<safe_numerics_error>(t)){
        case safe_numerics_error::success:
            return known_value;
        case safe_numerics_error::negative_overflow_error:
            // result is below representational minimum
            return less_than_min;
        case safe_numerics_error::positive_overflow_error:
            // result is above representational maximum
            return greater_than_max;
        default:
            return indeterminant;
        }
    }
    constexpr sum_value_type(const checked_result<T> & t) :
        m_flag(to_flag(t))
    {}
    constexpr operator std::uint8_t () const {
        return static_cast<std::uint8_t>(m_flag);
    }
};

// integers addition
template<class T>
typename std::enable_if<
    std::is_integral<T>::value,
    checked_result<T>
>::type
constexpr inline operator+(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    using value_type = sum_value_type<T>;
    constexpr const std::uint8_t order = static_cast<std::uint8_t>(value_type::count);

    constexpr const enum safe_numerics_error result[order][order] = {
        // t == known_value
        {
            // u == ...
            safe_numerics_error::success,                   // known_value,
            safe_numerics_error::negative_overflow_error,   // less_than_min,
            safe_numerics_error::positive_overflow_error,   // greater_than_max,
            safe_numerics_error::range_error,               // indeterminant,
        },
        // t == less_than_min,
        {
            // u == ...
            safe_numerics_error::negative_overflow_error,   // known_value,
            safe_numerics_error::negative_overflow_error,   // less_than_min,
            safe_numerics_error::range_error,               // greater_than_max,
            safe_numerics_error::range_error,               // indeterminant,
        },
        // t == greater_than_max,
        {
            // u == ...
            safe_numerics_error::positive_overflow_error,   // known_value,
            safe_numerics_error::range_error,               // less_than_min,
            safe_numerics_error::positive_overflow_error,   // greater_than_max,
            safe_numerics_error::range_error,               // indeterminant,
        },
        // t == indeterminant,
        {
            // u == ...
            safe_numerics_error::range_error,      // known_value,
            safe_numerics_error::range_error,      // less_than_min,
            safe_numerics_error::range_error,      // greater_than_max,
            safe_numerics_error::range_error,      // indeterminant,
        },
    };

    safe_numerics_error e = result[value_type(t)][value_type(u)];
    if(safe_numerics_error::success == e)
        return checked::add<T>(t, u);
    return checked_result<T>(e, "addition result");
}

// integers subtraction
template<class T>
typename std::enable_if<
    std::is_integral<T>::value,
    checked_result<T>
>::type
constexpr inline operator-(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    using value_type = sum_value_type<T>;
    constexpr const std::uint8_t order = static_cast<std::uint8_t>(value_type::count);

    constexpr const enum safe_numerics_error result[order][order] = {
        // t == known_value
        {
            // u == ...
            safe_numerics_error::success,                   // known_value,
            safe_numerics_error::positive_overflow_error,   // less_than_min,
            safe_numerics_error::negative_overflow_error,   // greater_than_max,
            safe_numerics_error::range_error,               // indeterminant,
        },
        // t == less_than_min,
        {
            // u == ...
            safe_numerics_error::negative_overflow_error,   // known_value,
            safe_numerics_error::range_error,               // less_than_min,
            safe_numerics_error::negative_overflow_error,   // greater_than_max,
            safe_numerics_error::range_error,               // indeterminant,
        },
        // t == greater_than_max,
        {
            // u == ...
            safe_numerics_error::positive_overflow_error,   // known_value,
            safe_numerics_error::positive_overflow_error,   // less_than_min,
            safe_numerics_error::range_error,               // greater_than_max,
            safe_numerics_error::range_error,               // indeterminant,
        },
        // t == indeterminant,
        {
            // u == ...
            safe_numerics_error::range_error,               // known_value,
            safe_numerics_error::range_error,               // less_than_min,
            safe_numerics_error::range_error,               // greater_than_max,
            safe_numerics_error::range_error,               // indeterminant,
        },
    };

    value_type vt(t);
    value_type vu(u);
    safe_numerics_error e = result[vt][vu];
    if(safe_numerics_error::success == e)
        return checked::subtract<T>(t, u);
    return checked_result<T>(e, "subtraction result");
}

template<class T>
struct product_value_type {
    // characterization of various values
    const enum flag {
        less_than_min = 0,
        less_than_zero,
        zero,
        greater_than_zero,
        greater_than_max,
        indeterminant,
        count,
        minus_one
    } m_flag;
    constexpr enum flag to_flag(const checked_result<T> & t) const {
        switch(static_cast<safe_numerics_error>(t)){
        case safe_numerics_error::success:
            return (t < 0)
                ? less_than_zero
                : (t > 0)
                ? greater_than_zero
                : zero;
        case safe_numerics_error::negative_overflow_error:
            // result is below representational minimum
            return less_than_min;
        case safe_numerics_error::positive_overflow_error:
            // result is above representational maximum
            return greater_than_max;
        default:
            return indeterminant;
        }
    }
    constexpr product_value_type(const checked_result<T> & t) :
        m_flag(to_flag(t))
    {}
    constexpr operator std::uint8_t () const {
        return static_cast<std::uint8_t>(m_flag);
    }
};

// integers multiplication
template<class T>
typename std::enable_if<
    std::is_integral<T>::value,
    checked_result<T>
>::type
constexpr inline operator*(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    using value_type = product_value_type<T>;
    const std::uint8_t order = static_cast<std::uint8_t>(value_type::count);

    constexpr const enum value_type::flag result[order][order] = {
        // t == less_than_min
        {
            // u == ...
            value_type::greater_than_max,   // less_than_min,
            value_type::greater_than_max,   // less_than_zero,
            value_type::zero,               // zero,
            value_type::less_than_min,      // greater_than_zero,
            value_type::less_than_min,      // greater than max,
            value_type::indeterminant,      // indeterminant,
        },
        // t == less_than_zero,
        {
            // u == ...
            value_type::greater_than_max,   // less_than_min,
            value_type::greater_than_zero,  // less_than_zero,
            value_type::zero,               // zero,
            value_type::less_than_zero,     // greater_than_zero,
            value_type::less_than_min,      // greater than max,
            value_type::indeterminant,      // indeterminant,
        },
        // t == zero,
        {
            // u == ...
            value_type::zero,               // less_than_min,
            value_type::zero,               // less_than_zero,
            value_type::zero,               // zero,
            value_type::zero,               // greater_than_zero,
            value_type::zero,               // greater than max,
            value_type::indeterminant,      // indeterminant,
        },
        // t == greater_than_zero,
        {
            // u == ...
            value_type::less_than_min,      // less_than_min,
            value_type::less_than_zero,     // less_than_zero,
            value_type::zero,               // zero,
            value_type::greater_than_zero,  // greater_than_zero,
            value_type::greater_than_max,   // greater than max,
            value_type::indeterminant,      // indeterminant,
        },
        // t == greater_than_max
        {
            value_type::less_than_min,      // less_than_min,
            value_type::less_than_min,      // less_than_zero,
            value_type::zero,               // zero,
            value_type::greater_than_max,   // greater_than_zero,
            value_type::greater_than_max,   // greater than max,
            value_type::indeterminant,      // indeterminant,
        },
        // t == indeterminant
        {
            value_type::indeterminant,      // less_than_min,
            value_type::indeterminant,      // less_than_zero,
            value_type::indeterminant,      // zero,
            value_type::indeterminant,      // greater_than_zero,
            value_type::indeterminant,      // greater than max,
            value_type::indeterminant,      // indeterminant,
        }
    };

    switch(result[value_type(t)][value_type(u)]){
        case value_type::less_than_min:
            return safe_numerics_error::negative_overflow_error;
        case value_type::zero:
            return T(0);
        case value_type::greater_than_max:
            return safe_numerics_error::positive_overflow_error;
        case value_type::less_than_zero:
        case value_type::greater_than_zero:
            return checked::multiply<T>(t, u);
        case value_type::indeterminant:
            return safe_numerics_error::range_error;
        default:
            assert(false);
    }
}

// integers division
template<class T>
typename std::enable_if<
    std::is_integral<T>::value,
    checked_result<T>
>::type
constexpr inline operator/(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    using value_type = product_value_type<T>;
    const std::uint8_t order = static_cast<std::uint8_t>(value_type::count);

    constexpr const enum value_type::flag result[order][order] = {
        // t == less_than_min
        {
            // u == ...
            value_type::indeterminant,   // less_than_min,
            value_type::greater_than_max,   // less_than_zero,
            value_type::less_than_min,      // zero,
            value_type::less_than_min,      // greater_than_zero,
            value_type::less_than_min,      // greater than max,
            value_type::indeterminant,      // indeterminant,
        },
        // t == less_than_zero,
        {
            // u == ...
            value_type::zero,               // less_than_min,
            value_type::greater_than_zero,  // less_than_zero,
            value_type::less_than_min,      // zero,
            value_type::less_than_zero,     // greater_than_zero,
            value_type::zero,               // greater than max,
            value_type::indeterminant,      // indeterminant,
        },
        // t == zero,
        {
            // u == ...
            value_type::zero,               // less_than_min,
            value_type::zero,               // less_than_zero,
            value_type::indeterminant,      // zero,
            value_type::zero,               // greater_than_zero,
            value_type::zero,               // greater than max,
            value_type::indeterminant,               // indeterminant,
        },
        // t == greater_than_zero,
        {
            // u == ...
            value_type::zero,               // less_than_min,
            value_type::less_than_zero,     // less_than_zero,
            value_type::greater_than_max,   // zero,
            value_type::greater_than_zero,  // greater_than_zero,
            value_type::zero,               // greater than max,
            value_type::indeterminant,      // indeterminant,
        },
        // t == greater_than_max
        {
            value_type::less_than_min,      // less_than_min,
            value_type::less_than_min,      // less_than_zero,
            value_type::greater_than_max,   // zero,
            value_type::greater_than_max,   // greater_than_zero,
            value_type::indeterminant,   // greater than max,
            value_type::indeterminant,      // indeterminant,
        },
        // t == indeterminant
        {
            value_type::indeterminant,      // less_than_min,
            value_type::indeterminant,      // less_than_zero,
            value_type::indeterminant,      // zero,
            value_type::indeterminant,      // greater_than_zero,
            value_type::indeterminant,      // greater than max,
            value_type::indeterminant,      // indeterminant,
        }
    };

    switch(result[value_type(t)][value_type(u)]){
        case value_type::less_than_min:
            return safe_numerics_error::negative_overflow_error;
        case value_type::zero:
            return 0;
        case value_type::greater_than_max:
            return safe_numerics_error::positive_overflow_error;
        case value_type::less_than_zero:
        case value_type::greater_than_zero:
            return checked::divide<T>(t, u);
        case value_type::indeterminant:
            return safe_numerics_error::range_error;
        default:
            assert(false);
    }
}

// integers modulus
template<class T>
typename std::enable_if<
    std::is_integral<T>::value,
    checked_result<T>
>::type
constexpr inline operator%(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    using value_type = product_value_type<T>;
    const std::uint8_t order = static_cast<std::uint8_t>(value_type::count);

    constexpr const enum value_type::flag result[order][order] = {
        // t == less_than_min
        {
            // u == ...
            value_type::indeterminant,      // less_than_min,
            value_type::indeterminant,      // less_than_zero,
            value_type::indeterminant,      // zero,
            value_type::indeterminant,      // greater_than_zero,
            value_type::indeterminant,      // greater than max,
            value_type::indeterminant,      // indeterminant,
        },
        // t == less_than_zero,
        {
            // u == ...
            value_type::indeterminant,      // less_than_min,
            value_type::greater_than_zero,  // less_than_zero,
            value_type::indeterminant,      // zero,
            value_type::less_than_zero,     // greater_than_zero,
            value_type::indeterminant,      // greater than max,
            value_type::indeterminant,      // indeterminant,
        },
        // t == zero,
        {
            // u == ...
            value_type::zero,               // less_than_min,
            value_type::zero,               // less_than_zero,
            value_type::indeterminant,      // zero,
            value_type::zero,               // greater_than_zero,
            value_type::zero,               // greater than max,
            value_type::indeterminant,      // indeterminant,
        },
        // t == greater_than_zero,
        {
            // u == ...
            value_type::indeterminant,      // less_than_min,
            value_type::less_than_zero,     // less_than_zero,
            value_type::indeterminant,      // zero,
            value_type::greater_than_zero,  // greater_than_zero,
            value_type::indeterminant,      // greater than max,
            value_type::indeterminant,      // indeterminant,
        },
        // t == greater_than_max
        {
            value_type::indeterminant,      // less_than_min,
            value_type::indeterminant,      // less_than_zero,
            value_type::indeterminant,      // zero,
            value_type::indeterminant,      // greater_than_zero,
            value_type::indeterminant,      // greater than max,
            value_type::indeterminant,      // indeterminant,
        },
        // t == indeterminant
        {
            value_type::indeterminant,      // less_than_min,
            value_type::indeterminant,      // less_than_zero,
            value_type::indeterminant,      // zero,
            value_type::indeterminant,      // greater_than_zero,
            value_type::indeterminant,      // greater than max,
            value_type::indeterminant,      // indeterminant,
        }
    };

    switch(result[value_type(t)][value_type(u)]){
        case value_type::zero:
            return 0;
        case value_type::less_than_zero:
        case value_type::greater_than_zero:
            return checked::modulus<T>(t, u);
        case value_type::indeterminant:
            return safe_numerics_error::range_error;
        case value_type::greater_than_max:
        case value_type::less_than_min:
        default:
            assert(false);
    }
}

// comparison operators

template<class T>
constexpr boost::logic::tribool operator<(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    using value_type = sum_value_type<T>;
    constexpr const std::uint8_t order = static_cast<std::uint8_t>(value_type::count);

    enum class result_type : std::uint8_t {
        runtime,
        false_value,
        true_value,
        indeterminant,
    };

    constexpr const result_type result[order][order]{
        // t == known_value
        {
            // u == ...
            result_type::runtime,       // known_value,
            result_type::false_value,   // less_than_min,
            result_type::true_value,    // greater_than_max,
            result_type::indeterminant, // indeterminant,
        },
        // t == less_than_min
        {
            // u == ...
            result_type::true_value,    // known_value,
            result_type::indeterminant, // less_than_min,
            result_type::true_value,    // greater_than_max,
            result_type::indeterminant, // indeterminant,
        },
        // t == greater_than_max
        {
            // u == ...
            result_type::false_value,   // known_value,
            result_type::false_value,   // less_than_min,
            result_type::indeterminant, // greater_than_max,
            result_type::indeterminant, // indeterminant,
        },
        // t == indeterminant
        {
            // u == ...
            result_type::indeterminant, // known_value,
            result_type::indeterminant, // less_than_min,
            result_type::indeterminant, // greater_than_max,
            result_type::indeterminant, // indeterminant,
        },
    };

    switch(result[value_type(t)][value_type(u)]){
    case result_type::runtime:
        return static_cast<const T &>(t) < static_cast<const T &>(u);
    case result_type::false_value:
        return false;
    case result_type::true_value:
        return true;
    case result_type::indeterminant:
        return boost::logic::indeterminate;
    }
}

template<class T>
constexpr boost::logic::tribool
operator>=(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    return !(t < u);
}

template<class T>
constexpr boost::logic::tribool
operator>(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    return u < t;
}

template<class T>
constexpr boost::logic::tribool
operator<=(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    return !(u < t);
}

template<class T>
constexpr boost::logic::tribool
operator==(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    using value_type = sum_value_type<T>;
    constexpr const std::uint8_t order = static_cast<std::uint8_t>(value_type::count);

    enum class result_type : std::uint8_t {
        runtime,
        false_value,
        true_value,
        indeterminant,
    };

    constexpr const result_type result[order][order]{
        // t == known_value
        {
            // u == ...
            result_type::runtime,       // known_value,
            result_type::false_value,   // less_than_min,
            result_type::false_value,   // greater_than_max,
            result_type::indeterminant, // indeterminant,
        },
        // t == less_than_min
        {
            // u == ...
            result_type::false_value,   // known_value,
            result_type::indeterminant, // less_than_min,
            result_type::false_value,   // greater_than_max,
            result_type::indeterminant, // indeterminant,
        },
        // t == greater_than_max
        {
            // u == ...
            result_type::false_value,   // known_value,
            result_type::false_value,   // less_than_min,
            result_type::indeterminant, // greater_than_max,
            result_type::indeterminant, // indeterminant,
        },
        // t == indeterminant
        {
            // u == ...
            result_type::indeterminant, // known_value,
            result_type::indeterminant, // less_than_min,
            result_type::indeterminant, // greater_than_max,
            result_type::indeterminant, // indeterminant,
        },
    };
    switch(result[value_type(t)][value_type(u)]){
    case result_type::runtime:
        return static_cast<const T &>(t) == static_cast<const T &>(u);
    case result_type::false_value:
        return false;
    case result_type::true_value:
        return true;
    case result_type::indeterminant:
        return boost::logic::indeterminate;
    }
}

template<class T>
constexpr boost::logic::tribool
operator!=(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    return ! (t == u);
}

template<class T>
typename std::enable_if<
    std::is_integral<T>::value,
    checked_result<T>
>::type
constexpr inline operator>>(
    const checked_result<T> & t,
    const checked_result<T> & u
);

template<class T>
typename std::enable_if<
    std::is_integral<T>::value,
    checked_result<T>
>::type
constexpr inline operator-(
    const checked_result<T> & t
){
//    assert(false);
    return checked_result<T>(0) - t;
}

template<class T>
typename std::enable_if<
    std::is_integral<T>::value,
    checked_result<T>
>::type
constexpr inline operator~(
    const checked_result<T> & t
){
//    assert(false);
    return ~t.m_r;
}

template<class T>
typename std::enable_if<
    std::is_integral<T>::value,
    checked_result<T>
>::type
constexpr inline operator<<(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    using value_type = product_value_type<T>;
    const std::uint8_t order = static_cast<std::uint8_t>(value_type::count);

    const std::uint8_t result[order][order] = {
        // t == less_than_min
        {
            // u == ...
            1, // -1,                                           // less_than_min,
            2, // safe_numerics_error::negative_overflow_error, // less_than_zero,
            2, // safe_numerics_error::negative_overflow_error, // zero,
            2, // safe_numerics_error::negative_overflow_error, // greater_than_zero,
            2, // safe_numerics_error::negative_overflow_error, // greater than max,
            1, // safe_numerics_error::range_error,             // indeterminant,
        },
        // t == less_than_zero,
        {
            // u == ...
            3, // -1,                                           // less_than_min,
            4, // - (-t >> -u),                                 // less_than_zero,
            5, // safe_numerics_error::negative_overflow_error, // zero,
            6, // - (-t << u),                                  // greater_than_zero,
            2, // safe_numerics_error::negative_overflow_error, // greater than max,
            1, // safe_numerics_error::range_error,             // indeterminant,
        },
        // t == zero,
        {
            // u == ...
            3, // 0     // less_than_min,
            3, // 0     // less_than_zero,
            3, // 0,    // zero,
            3, // 0,    // greater_than_zero,
            3, // 0,    // greater than max,
            3, // safe_numerics_error::range_error,    // indeterminant,
        },
        // t == greater_than_zero,
        {
            // u == ...
            3, // 0,                                            // less_than_min,
            7, // t >> -u,                                      // less_than_zero,
            5, // t,                                            // zero,
            8, // t >> u                                        // greater_than_zero,
            9, // safe_numerics_error::positive_overflow_error, // greater than max,
            1, // safe_numerics_error::range_error,             // indeterminant,
        },
        // t == greater_than_max
        {
            // u == ...
            1, // safe_numerics_error::range_error,               // less_than_min,
            9, // safe_numerics_error::positive_overflow_error),  // less_than_zero,
            9, // safe_numerics_error::positive_overflow_error,   // zero,
            9, // safe_numerics_error::positive_overflow_error),  // greater_than_zero,
            9, // safe_numerics_error::positive_overflow_error,   // greater than max,
            1, // safe_numerics_error::range_error,               // indeterminant,
         },
        // t == indeterminant
        {
            1, // safe_numerics_error::range_error,    // indeterminant,
            1, // safe_numerics_error::range_error,    // indeterminant,
            1, // safe_numerics_error::range_error,    // indeterminant,
            1, // safe_numerics_error::range_error,    // indeterminant,
            1, // safe_numerics_error::range_error,    // indeterminant,
            1, // safe_numerics_error::range_error,    // indeterminant,
        }
    };

    const value_type vt(t);
    const value_type vu(u);
    switch(result[vt][vu]){
    case 1:
        return safe_numerics_error::range_error;
    case 2:
        return safe_numerics_error::negative_overflow_error;
    case 3:
        return 0;
    // the following gymnastics are to handle the case where 
    // a value is changed from a negative to a positive number.
    // For example, and 8 bit number t == -128.  Then -t also
    // equals -128 since 128 cannot be held in an 8 bit signed
    // integer.
    case 4:{ // - (-t >> -u)
        assert(t < 0);
        assert(u < 0);
        return t >> -u;
    }
    case 5:
        return t;
    case 6:{ // - (-t << u)
        assert(t < 0);
        assert(u > 0);
        const checked_result<T> tx = t * checked_result<T>(2);
        const checked_result<T> ux = u - checked_result<T>(1);
        return  - (-tx << ux);
    }
    case 7:{  // t >> -u
        assert(t > 0);
        assert(u < 0);
        return t >> -u;
    }
    case 8:{ // t << u
        assert(t > 0);
        assert(u > 0);
        checked_result<T> r = checked::left_shift<T>(t, u);
        return (r.m_e == safe_numerics_error::shift_too_large)
        ? checked_result<T>(safe_numerics_error::positive_overflow_error)
        : r;
    }
    case 9:
        return safe_numerics_error::positive_overflow_error;
    default:
        assert(false);
    };
}

template<class T>
typename std::enable_if<
    std::is_integral<T>::value,
    checked_result<T>
>::type
constexpr inline operator>>(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    using value_type = product_value_type<T>;
    const std::uint8_t order = static_cast<std::uint8_t>(value_type::count);

    const std::uint8_t result[order][order] = {
        // t == less_than_min
        {
            // u == ...
            2, // safe_numerics_error::negative_overflow_error, // less_than_min,
            2, // safe_numerics_error::negative_overflow_error, // less_than_zero,
            2, // safe_numerics_error::negative_overflow_error, // zero,
            2, // safe_numerics_error::negative_overflow_error, // greater_than_zero,
            1, // safe_numerics_error::range_error,             // greater than max,
            1, // safe_numerics_error::range_error,             // indeterminant,
        },
        // t == less_than_zero,
        {
            // u == ...
            2, // safe_numerics_error::negative_overflow_error  // less_than_min,
            4, // - (-t << -u),                                 // less_than_zero,
            5, // safe_numerics_error::negative_overflow_error. // zero,
            6, // - (-t >> u),                                  // greater_than_zero,
            3, // 0, ? or -1                                    // greater than max,
            1, // safe_numerics_error::range_error,             // indeterminant,
        },
        // t == zero,
        {
            // u == ...
            3, // 0     // less_than_min,
            3, // 0     // less_than_zero,
            3, // 0,    // zero,
            3, // 0,    // greater_than_zero,
            3, // 0,    // greater than max,
            3, // safe_numerics_error::range_error,    // indeterminant,
        },
        // t == greater_than_zero,
        {
            // u == ...
            9, // safe_numerics_error::positive_overflow_error  // less_than_min,
            7, // t << -u,                                      // less_than_zero,
            5, // t,                                            // zero,
            8, // t >> u                                        // greater_than_zero,
            3, // 0,                                            // greater than max,
            1, // safe_numerics_error::range_error,             // indeterminant,
        },
        // t == greater_than_max
        {
            // u == ...
            9, // safe_numerics_error::positive_overflow_error, // less_than_min,
            9, // safe_numerics_error::positive_overflow_error, // less_than_zero,
            9, // safe_numerics_error::positive_overflow_error, // zero,
            9, // safe_numerics_error::positive_overflow_error, // greater_than_zero,
            1, // safe_numerics_error::range_error,             // greater than max,
            1, // safe_numerics_error::range_error,             // indeterminant,
         },
        // t == indeterminant
        {
            1, // safe_numerics_error::range_error,    // indeterminant,
            1, // safe_numerics_error::range_error,    // indeterminant,
            1, // safe_numerics_error::range_error,    // indeterminant,
            1, // safe_numerics_error::range_error,    // indeterminant,
            1, // safe_numerics_error::range_error,    // indeterminant,
            1, // safe_numerics_error::range_error,    // indeterminant,
        }
    };

    const value_type vt(t);
    const value_type vu(u);
    switch(result[vt][vu]){
    case 1:
        return safe_numerics_error::range_error;
    case 2:
        return safe_numerics_error::negative_overflow_error;
    case 3:
        return 0;
    case 4:{ // - (-t << -u)
        assert(t < 0);
        assert(u < 0);
        return t << -u;
    }
    case 5:
        return t;
    case 6:{ //  - (-t >> u)
        assert(t < 0);
        assert(u > 0);
        const checked_result<T> tx = t / checked_result<T>(2);
        const checked_result<T> ux = u - checked_result<T>(1);
        return  - (-tx >> ux);
    }
    case 7:{ // t << -u,
        assert(t > 0);
        assert(u < 0);
        return t << -u;
    }
    case 8:{ // t >> u
        assert(t > 0);
        assert(u > 0);
        checked_result<T> r = checked::right_shift<T>(t, u);
        return (r.m_e == safe_numerics_error::shift_too_large)
        ? checked_result<T>(0)
        : r;
    }
    case 9:
        return safe_numerics_error::positive_overflow_error;
    default:
        assert(false);
    };
}

template<class T>
typename std::enable_if<
    std::is_integral<T>::value,
    checked_result<T>
>::type
constexpr inline operator|(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    return
        t.exception() || u.exception()
        ? checked_result<T>(safe_numerics_error::range_error)
        : checked::bitwise_or<T>(
            static_cast<T>(t),
            static_cast<T>(u)
        );
}
template<class T>
typename std::enable_if<
    std::is_integral<T>::value,
    checked_result<T>
>::type
constexpr inline operator^(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    return
        t.exception() || u.exception()
        ? checked_result<T>(safe_numerics_error::range_error)
        : checked::bitwise_xor<T>(
            static_cast<T>(t),
            static_cast<T>(u)
        );
}

template<class T>
typename std::enable_if<
    std::is_integral<T>::value,
    checked_result<T>
>::type
constexpr inline operator&(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    return
        t.exception() || u.exception()
        ? checked_result<T>(safe_numerics_error::range_error)
        : checked::bitwise_and<T>(
            static_cast<T>(t),
            static_cast<T>(u)
        );
}

} // numeric
} // boost

#include <iosfwd>

namespace std {

template<typename CharT, typename Traits, typename R>
inline std::basic_ostream<CharT, Traits> & operator<<(
    std::basic_ostream<CharT, Traits> & os,
    const boost::numeric::checked_result<R> & r
){
    if(!r.exception())
        os << static_cast<R>(r);
    else
        os << std::error_code(r.m_e).message() << ':' << r.m_msg;
    return os;
}

template<typename CharT, typename Traits>
inline std::basic_ostream<CharT, Traits> & operator<<(
    std::basic_ostream<CharT, Traits> & os,
    const boost::numeric::checked_result<signed char> & r
){
    if(! r.exception())
        os << static_cast<std::int16_t>(r);
    else
        os << std::error_code(r.m_e).message() << ':' << r.m_msg;
    return os;
}

template<typename CharT, typename Traits>
inline std::basic_ostream<CharT, Traits> & operator<<(
    std::basic_ostream<CharT, Traits> & os,
    const boost::numeric::checked_result<unsigned char> & r
){
    if(! r.exception())
        os << static_cast<std::uint16_t>(r);
    else
        os << std::error_code(r.m_e).message() << ':' << r.m_msg;
    return os;
}

template<typename CharT, typename Traits, typename R>
inline std::basic_istream<CharT, Traits> & operator>>(
    std::basic_istream<CharT, Traits> & is,
    boost::numeric::checked_result<R> & r
){
    is >> r.m_r;
    return is;
}

template<typename CharT, typename Traits, typename R>
inline std::basic_istream<CharT, Traits> & operator>>(
    std::basic_istream<CharT, Traits> & is, 
    boost::numeric::checked_result<signed char> & r
){
    std::int16_t i;
    is >> i;
    r.m_r = i;
    return is;
}

template<typename CharT, typename Traits, typename R>
inline std::basic_istream<CharT, Traits> & operator>>(
    std::basic_istream<CharT, Traits> & is,
    boost::numeric::checked_result<unsigned char> & r
){
    std::uint16_t i;
    is >> i;
    r.m_r = i;
    return is;
}

} // std

/////////////////////////////////////////////////////////////////
// numeric limits for checked<R>

#include <limits>

namespace std {

template<class R>
class numeric_limits<boost::numeric::checked_result<R> >
    : public std::numeric_limits<R>
{
    using this_type = boost::numeric::checked_result<R>;
public:
    constexpr static this_type min() noexcept {
        return this_type(std::numeric_limits<R>::min());
    }
    constexpr static this_type max() noexcept {
        return this_type(std::numeric_limits<R>::max());
    }
};

} // std

#endif  // BOOST_NUMERIC_CHECKED_RESULT_OPERATIONS
