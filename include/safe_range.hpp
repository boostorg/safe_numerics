#ifndef BOOST_NUMERIC_SAFE_RANGE_HPP
#define BOOST_NUMERIC_SAFE_RANGE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdint> // intmax_t, uintmax_t
#include <boost/integer.hpp> // (u)int_t<>::least, exact
#include <boost/mpl/max.hpp>

#include "safe_base.hpp"
#include "safe_base_operations.hpp"
#include "native.hpp"
#include "exception_policies.hpp"

/////////////////////////////////////////////////////////////////
// higher level types implemented in terms of safe_base

namespace boost {
namespace numeric {

namespace detail {
    constexpr int ulog(boost::uintmax_t x){
        unsigned i = 0;
        for(; x > 0; ++i)
            x >>= 1;
        return i;
    }
    constexpr int log(std::intmax_t x){
        if(x < 0)
            x = ~x;
        return ulog(x) + 1;
    }
    template<
        std::intmax_t MIN,
        std::intmax_t MAX
    >
    using signed_stored_type = typename boost::int_t<
        std::max(log(MIN), log(MAX))
    >::least ;

    template<
        std::uintmax_t MIN,
        std::uintmax_t MAX
    >
    using unsigned_stored_type = typename boost::uint_t<
        std::max(ulog(MIN), ulog(MAX))
    >::least ;

} // detail

/////////////////////////////////////////////////////////////////
// safe_signed_range

template <
    std::intmax_t MIN,
    std::intmax_t MAX,
    class P = native,
    class E = throw_exception
>
using safe_signed_range = safe_base<
    typename detail::signed_stored_type<MIN, MAX>,
    static_cast<typename detail::signed_stored_type<MIN, MAX> >(MIN),
    static_cast<typename detail::signed_stored_type<MIN, MAX> >(MAX),
    P,
    E
>;

/////////////////////////////////////////////////////////////////
// safe_unsigned_range

template <
    std::uintmax_t MIN,
    std::uintmax_t MAX,
    class P = native,
    class E = throw_exception
>
using safe_unsigned_range = safe_base<
    typename detail::unsigned_stored_type<MIN, MAX>,
    static_cast<typename detail::unsigned_stored_type<MIN, MAX> >(MIN),
    static_cast<typename detail::unsigned_stored_type<MIN, MAX> >(MAX),
    P,
    E
>;

} // numeric
} // boost

#endif // BOOST_NUMERIC_SAFE_RANGE_HPP
