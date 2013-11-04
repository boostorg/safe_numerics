#ifndef BOOST_NUMERIC_SAFE_INTEGER_HPP
#define BOOST_NUMERIC_SAFE_INTEGER_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/limits.hpp>
#include <boost/mpl/if.hpp>
#include "safe_range.hpp"

namespace boost {
namespace numeric {

namespace detail{
    template<class T>
    struct safe_integer_base {
        typedef typename boost::mpl::if_c<
            std::numeric_limits<T>::is_signed,
            safe_signed_range<
                boost::integer_traits<T>::const_min,
                boost::integer_traits<T>::const_max
            >,
            safe_unsigned_range<
                boost::integer_traits<T>::const_min,
                boost::integer_traits<T>::const_max
            >
        >::type type;
    };
} // detail

template<class T>
struct safe : public detail::safe_integer_base<T>::type {
    typedef typename detail::safe_integer_base<T>::type base_type;

    // verify that std::numeric_limits has been specialized for this type
    BOOST_STATIC_ASSERT_MSG(
        std::numeric_limits<T>::is_specialized,
        "std::numeric_limits<T> has not been specialized for this type"
    );
    // verify that T is an integer type
    BOOST_STATIC_ASSERT_MSG(
        std::numeric_limits<T>::is_integer,
        "T is not an integer type"
    );
    safe(){}

    template<class U>
    safe(const U & u) :
        detail::safe_integer_base<T>::type(u)
    {}
};

} // numeric
} // boost

#include <boost/config.hpp> // BOOST_NOEXCEPT

namespace std {

template<class T>
class numeric_limits< boost::numeric::safe<T> > : public
    numeric_limits<T>
{
    typedef boost::numeric::safe<T> SI;
public:
    BOOST_STATIC_CONSTEXPR SI min() BOOST_NOEXCEPT { return numeric_limits<T>::min(); }
    BOOST_STATIC_CONSTEXPR SI max() BOOST_NOEXCEPT { return numeric_limits<T>::max(); }
    BOOST_STATIC_CONSTEXPR SI lowest() BOOST_NOEXCEPT { return numeric_limits<T>::min(); }
};

} // std

#endif // BOOST_NUMERIC_SAFE_INTEGER_HPP
