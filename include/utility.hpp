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
} // numeric
} // boost

#endif  // BOOST_NUMERIC_UTILITY_HPP
