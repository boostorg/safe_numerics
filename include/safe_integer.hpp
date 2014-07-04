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

#include <boost/config.hpp> // BOOST_NOEXCEPT
#include <boost/limits.hpp>
#include <boost/integer_traits.hpp>
#include <boost/mpl/if.hpp>

#include "safe_range.hpp"
#include "numeric.hpp"

namespace boost {
namespace numeric {

namespace detail{

template<
    class T,
    class PromotionPolicy
>
struct safe_integer_base {
    typedef typename boost::mpl::if_<
        boost::numeric::is_signed<T>,
        boost::numeric::safe_signed_range<
            static_cast<boost::intmax_t>(boost::integer_traits<T>::const_min),
            static_cast<boost::intmax_t>(boost::integer_traits<T>::const_max),
            PromotionPolicy
        >,
        boost::numeric::safe_unsigned_range<
            static_cast<boost::uintmax_t>(boost::integer_traits<T>::const_min),
            static_cast<boost::uintmax_t>(boost::integer_traits<T>::const_max),
            PromotionPolicy
        >
    >::type type;
};

} // detail

} // numeric
} // boost

#include "boost/concept/assert.hpp"
#include "concept/numeric.hpp"
#include "native.hpp" // boost::promotion::native

namespace boost {
namespace numeric {

template<
    class T,
    class PromotionPolicy = boost::numeric::promotion::native
>
struct safe : public detail::safe_integer_base<T, PromotionPolicy>::type {
    //typedef typename detail::safe_integer_base<T, PromotionPolicy>::type base_type;

    BOOST_CONCEPT_ASSERT((Integer<T>));

    safe() :
        detail::safe_integer_base<T, PromotionPolicy>::type()
    {}

    template<class U>
    safe(const U & u) :
        detail::safe_integer_base<T, PromotionPolicy>::type(u)
    {}
    /*
    safe(const T & t) :
        detail::safe_integer_base<T, PromotionPolicy>::type(t)
    {}
    */
};

} // numeric
} // boost

namespace std {

template<
    class T,
    class PromotionPolicy
>
class numeric_limits< boost::numeric::safe<T, PromotionPolicy> > : public
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
