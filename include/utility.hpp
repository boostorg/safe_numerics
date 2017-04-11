#ifndef BOOST_NUMERIC_UTILITY_HPP
#define BOOST_NUMERIC_UTILITY_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2015 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdint> // intmax_t, uintmax_t, uint8_t, ...
#include <algorithm>
#include <type_traits>
#include <limits>
#include <cassert>

#include <boost/mpl/if.hpp> // if_c
#include <boost/integer.hpp> // (u)int_t<>::least, exact

namespace boost {
namespace numeric {
namespace utility {

// used for debugging
// usage - print_type<T>;
// provokes error message with name of type T

template<typename Tx>
using print_type = typename Tx::error_message;

template<int N> 
struct print_value
{
    enum test : char {
        value = N < 0 ? N - 256 : N + 256
    };
};

template<typename T>
using bits_type = std::integral_constant<
    int,
    std::numeric_limits<T>::digits
    + (std::numeric_limits<T>::is_signed ? 1 : 0)
>;

/*
From http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogObvious
Find the log base 2 of an integer with a lookup table

    static const char LogTable256[256] =
    {
    #define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
        -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
        LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
        LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
    };

    unsigned int v; // 32-bit word to find the log of
    unsigned r;     // r will be lg(v)
    register unsigned int t, tt; // temporaries

    if (tt = v >> 16)
    {
      r = (t = tt >> 8) ? 24 + LogTable256[t] : 16 + LogTable256[tt];
    }
    else 
    {
      r = (t = v >> 8) ? 8 + LogTable256[t] : LogTable256[v];
    }

The lookup table method takes only about 7 operations to find the log of a 32-bit value. 
If extended for 64-bit quantities, it would take roughly 9 operations. Another operation
can be trimmed off by using four tables, with the possible additions incorporated into each.
Using int table elements may be faster, depending on your architecture.
*/

// I've "improved" the above and recast as C++ code which depends upon
// the optimizer to minimize the operations.  This should result in
// nine operations to calculate the position of the highest order
// bit in a 64 bit number. RR

constexpr unsigned int log2(const boost::uint_t<8>::exact & t){
    constexpr const char LogTable256[256] = {
        #define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
        -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
        LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
        LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
    };
    return LogTable256[t];
}
constexpr unsigned int log2(const boost::uint_t<16>::exact & t){
    const boost::uint_t<8>::exact upper_half = (t >> 8);
    return upper_half == 0
        ? log2(static_cast<boost::uint_t<8>::exact>(t))
        : 8 + log2(upper_half);
}
constexpr unsigned int log2(const boost::uint_t<32>::exact & t){
    const boost::uint_t<16>::exact upper_half = (t >> 16);
    return upper_half == 0
        ? log2(static_cast<boost::uint_t<16>::exact>(t))
        : 16 + log2(upper_half);
}
constexpr unsigned int log2(const boost::uint_t<64>::exact & t){
    const boost::uint_t<32>::exact upper_half = (t >> 32);
    return upper_half == 0
        ? log2(static_cast<boost::uint_t<32>::exact>(t))
        : 32 + log2(upper_half);
}

template<typename T>
constexpr unsigned int log(const T & t){
//  log not defined for negative numbers
    assert(t >= 0);
    return log2(
        static_cast<
            typename boost::uint_t<
                bits_type<T>::value
            >::exact
        >(t)
    );
}

// the number of bits required to render the value in x
// excluding sign bit
template<typename T>
constexpr unsigned int significant_bits(const T & t){
    return 1 + (t < 0 ? log(~t) : log(t));
}

/*
// get bit max for values of type T
template<typename T>
constexpr unsigned int bits_value(const T & t){
    const unsigned int sb = significant_bits(t);
    const unsigned int sb_max = significant_bits(std::numeric_limits<T>::max());
    return sb < sb_max ? ((sb << 1) - 1) : std::numeric_limits<T>::max();
}
*/

// return type required to store a particular range
template<
    std::intmax_t Min,
    std::intmax_t Max
>
// signed range
using signed_stored_type = typename boost::int_t<
    std::max({
        significant_bits(Min),
        significant_bits(Max)
    }) + 1
>::least ;

template<
    std::uintmax_t Min,
    std::uintmax_t Max
>
// unsigned range
using unsigned_stored_type = typename boost::uint_t<
    significant_bits(Max)
>::least ;

// return type required to store a particular range
#ifdef BOOST_MSVC

// If we use std::max in here we get internal compiler errors
// (tested VC2017) ...

template <class T>
constexpr const T & msvc_max(
    const T & lhs,
    const T & rhs
){
    return lhs > rhs ? lhs : rhs;
}

template<
    std::intmax_t Min,
    std::intmax_t Max
>
// signed range
using signed_stored_type = typename boost::int_t<
    std::max({
        significant_bits(Min),
        significant_bits(Max)
    }) + 1
>::least ;

#else

template<
    std::intmax_t Min,
    std::intmax_t Max
>
// signed range
using signed_stored_type = typename boost::int_t<
    std::max({
        significant_bits(Min),
        significant_bits(Max)
    }) + 1
>::least;

#endif // BOOST_MSVC

template<
    std::uintmax_t Min,
    std::uintmax_t Max
>
// unsigned range
using unsigned_stored_type = typename boost::uint_t<
    significant_bits(Max)
>::least;

} // utility
} // numeric
} // boost

#endif  // BOOST_NUMERIC_UTILITY_HPP
