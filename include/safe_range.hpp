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
#include <boost/integer.hpp> // int_t, uint_t
#include <boost/mpl/max.hpp>

#include "safe_base.hpp"
#include "safe_base_operations.hpp"
#include "native.hpp"
#include "exception_policies.hpp"
#include "numeric.hpp"

/////////////////////////////////////////////////////////////////
// higher level types implemented in terms of safe_base

namespace boost {
namespace numeric {

namespace detail {
    template<
        std::intmax_t MIN,
        std::intmax_t MAX
    >
    struct signed_stored_type {
        // double check that MIN < MAX
        typedef typename boost::int_t<
            boost::mpl::max<
                typename boost::numeric::detail::log<MIN, 2>,
                typename boost::numeric::detail::log<MAX, 2>
            >::type::value
        >::least type;
    };
    template<
        std::uintmax_t MIN,
        std::uintmax_t MAX
    >
    struct unsigned_stored_type {
        // calculate max(abs(MIN, MAX))
        typedef typename boost::uint_t<
            boost::mpl::max<
                typename boost::numeric::detail::ulog<MIN, 2>,
                typename boost::numeric::detail::ulog<MAX, 2>
            >::type::value
        >::least type;
    };
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
    typename detail::signed_stored_type<MIN, MAX>::type,
    static_cast<typename detail::signed_stored_type<MIN, MAX>::type>(MIN),
    static_cast<typename detail::signed_stored_type<MIN, MAX>::type>(MAX),
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
    typename detail::unsigned_stored_type<MIN, MAX>::type,
    static_cast<typename detail::unsigned_stored_type<MIN, MAX>::type>(MIN),
    static_cast<typename detail::unsigned_stored_type<MIN, MAX>::type>(MAX),
    P,
    E
>;

} // numeric
} // boost

#endif // BOOST_NUMERIC_SAFE_RANGE_HPP
