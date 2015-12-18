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

#include "safe_range.hpp"
#include "native.hpp"

namespace boost {
namespace numeric {

template<std::intmax_t N>
class safe_literal;

template<std::intmax_t N>
struct is_safe<safe_literal<N> > : public std::true_type
{};

template<std::intmax_t N>
struct base_type<safe_literal<N> > {
    typedef std::intmax_t type;
};

template<std::intmax_t N>
constexpr std::intmax_t base_value(
    const safe_literal<N>  & st
) {
    return N;
}

template<std::intmax_t N>
std::ostream & operator<<(
    std::ostream & os,
    const safe_literal<N> & t
);

template<std::intmax_t N>
class safe_literal {
    std::intmax_t m_t;

    friend std::ostream & operator<< <N> (
        std::ostream & os,
        const safe_literal & t
    );

    template<
        class StoredX,
        StoredX MinX,
        StoredX MaxX,
        class PX, // promotion polic
        class EX  // exception policy
    >
    friend class safe_base;

public:

    ////////////////////////////////////////////////////////////
    // constructors
    // default constructor
    constexpr safe_literal() :
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

} // numeric
} // boost

#endif // BOOST_NUMERIC_SAFE_LITERAL_HPP
