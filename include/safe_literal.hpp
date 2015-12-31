#ifndef BOOST_NUMERIC_SAFE_LITERAL_HPP
#define BOOST_NUMERIC_SAFE_LITERAL_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdint> // for intmax

#include "utility.hpp"

namespace boost {
namespace numeric {

template<typename T, T N>
class safe_literal_impl;

template<typename T, T N>
struct is_safe<safe_literal_impl<T, N> > : public std::true_type
{};

template<typename T, T N>
struct base_type<safe_literal_impl<T, N> > {
    using type = T;
};

template<typename T, T N>
constexpr T base_value(
    const safe_literal_impl<T, N>  & st
) {
    return N;
}

template<typename T, T N>
std::ostream & operator<<(
    std::ostream & os,
    const safe_literal_impl<T, N> & t
);

template<typename T, T N>
class safe_literal_impl {
    T m_t;

    friend std::ostream & operator<< <T, N> (
        std::ostream & os,
        const safe_literal_impl & t
    );

public:

    ////////////////////////////////////////////////////////////
    // constructors
    // default constructor
    constexpr safe_literal_impl() :
        m_t(N)
    {}

    /////////////////////////////////////////////////////////////////
    // casting operators for intrinsic integers
    // convert to any type which is not safe.  safe types need to be
    // excluded to prevent ambiguous function selection which
    // would otherwise occur
    template<
        class R,
        typename std::enable_if<
            !boost::numeric::is_safe<R>::value,
            int
        >::type = 0
    >
    constexpr operator R () const {
        return m_t;
    }
};

template<std::intmax_t N>
using safe_literal = safe_literal_impl<
    typename boost::numeric::signed_stored_type<N, N>,
    N
>;

template<std::uintmax_t N>
using safe_unsigned_literal = safe_literal_impl<
    typename boost::numeric::unsigned_stored_type<N, N>,
    N
>;

} // numeric
} // boost

#endif // BOOST_NUMERIC_SAFE_LITERAL_HPP
