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

#include <cstdint> // intmax_t, uintmax_t
#include <algorithm>
#include <boost/integer.hpp> // (u)int_t<>::least, exact

namespace boost {
namespace numeric {
    // section 4.13 integer conversion rank
    template<class T>
    struct rank;

    template<>
    struct rank<char> : public std::integral_constant<unsigned char, 1>{};
    template<>
    struct rank<signed char> : public std::integral_constant<unsigned char, 1>{};
    template<>
    struct rank<unsigned char> : public std::integral_constant<unsigned char, 1>{};

    template<>
    struct rank<wchar_t> : public std::integral_constant<unsigned char, 2>{};

    template<>
    struct rank<short> : public std::integral_constant<unsigned char, 3>{};
    template<>
    struct rank<unsigned short> : public std::integral_constant<unsigned char, 3>{};

    template<>
    struct rank<int> : public std::integral_constant<unsigned char, 4>{};
    template<>
    struct rank<unsigned int> : public std::integral_constant<unsigned char, 4>{};

    template<>
    struct rank<long> : public std::integral_constant<unsigned char, 5>{};
    template<>
    struct rank<unsigned long> : public std::integral_constant<unsigned char, 5>{};

    template<>
    struct rank<long long> : public std::integral_constant<unsigned char, 6>{};
    template<>
    struct rank<unsigned long long> : public std::integral_constant<unsigned char, 6>{};

    // the number of bits required to render the value in x
    template<typename T>
    typename std::enable_if<
        // T being unsigned
        ! std::is_signed<T>::value,
        std::uintmax_t
    >::type
    constexpr log(T x){
        std::uintmax_t i = 0;
        for(; x > 0; ++i)
            x >>= 1;
        return i;
    }
    template<typename T>
    typename std::enable_if<
        // T being signed
        std::is_signed<T>::value,
        std::uintmax_t
    >::type
    constexpr log(T x){
        if(x < 0)
            x = ~x;
        return log(
            static_cast<typename std::make_unsigned<T>::type>(x)
        ) + 1;
    }
    // return type required to store a particular range
    template<
        std::intmax_t Min,
        std::intmax_t Max
    >
    // signed range
    using signed_stored_type = typename boost::int_t<
        std::max({log(Min), log(Max)})
    >::least ;

    template<
        std::uintmax_t Min,
        std::uintmax_t Max
    >
    // unsigned range
    using unsigned_stored_type = typename boost::uint_t<
        std::max({log(Min), log(Max)})
    >::least ;

    // used for debugging
    // usage - print_type<T>;
    // provokes error message with name of type T

    template<typename Tx>
    using print_type = typename Tx::error_message;

    template<int N> 
    struct print_value
    {
        enum test : unsigned char {
            value = N + 256
        };
    };

} // numeric
} // boost

#endif  // BOOST_NUMERIC_UTILITY_HPP
