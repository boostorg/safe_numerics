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

#include "safe_base.hpp"
#include "policies.hpp"
#include "native.hpp"

namespace boost {
namespace numeric {

typedef policies<native, relaxed, throw_exception> default_policies;

template<
    class T,
    class P = default_policies
>
struct safe : public safe_base<T, safe<T, P> >{
    BOOST_CONCEPT_ASSERT((Integer<T>));

    typedef T stored_type;
    struct Policies {
        typedef P type;
    };

    bool validate(const T & t) const {
        return true;
    }

    typedef safe_base<T, safe<T, P> > base_type;
    safe() :
        base_type()
    {}

    template<class U>
    safe(const U & u) :
        base_type()
    {
        // verify that
    }
};

} // numeric
} // boost

#include <boost/integer_traits.hpp>

namespace std {

template<
    class T,
    class P
>
class numeric_limits< boost::numeric::safe<T, P> >
    : public numeric_limits<T>
{
    typedef boost::numeric::safe<T, P> SI;
public:
    // these expressions are not constexpr until C++14 so re-implement them here
    BOOST_STATIC_CONSTEXPR SI min() BOOST_NOEXCEPT { return boost::integer_traits<T>::const_min; }
    BOOST_STATIC_CONSTEXPR SI max() BOOST_NOEXCEPT { return boost::integer_traits<T>::const_max; }
    BOOST_STATIC_CONSTEXPR SI lowest() BOOST_NOEXCEPT { return boost::integer_traits<T>::const_min;; }
};

} // std

#endif // BOOST_NUMERIC_SAFE_INTEGER_HPP
