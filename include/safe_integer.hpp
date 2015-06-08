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
#include "safe_base_operations.hpp"
#include "concept/numeric.hpp"
#include "exception_policies.hpp"
#include "native.hpp"

#include "boost/concept/assert.hpp"

namespace boost {
namespace numeric {

template<
    class T,
    class P = boost::numeric::native,
    class E = boost::numeric::throw_exception
>
struct safe : public safe_base<T, safe<T, P, E>, P, E>{
    BOOST_CONCEPT_ASSERT((boost::numeric::Integer<T>));
    BOOST_CONCEPT_ASSERT((boost::numeric::PromotionPolicy<P>));
    BOOST_CONCEPT_ASSERT((boost::numeric::ExceptionPolicy<E>));

    typedef safe_base<T, safe<T, P, E>, P, E > base_type;

    SAFE_NUMERIC_CONSTEXPR bool validate(const T & t) const {
        return true;
    }
    SAFE_NUMERIC_CONSTEXPR bool validate(T && t) const {
        return true;
    }

    SAFE_NUMERIC_CONSTEXPR safe() :
        base_type()
    {}

    //SAFE_NUMERIC_CONSTEXPR safe(safe && s) :
    //    base_type()
    //{}
    template<class U>
    SAFE_NUMERIC_CONSTEXPR safe(const U & u) :
        base_type(u)
    {}
};

} // numeric
} // boost


/////////////////////////////////////////////////////////////////
// numeric limits for safe<int> etc.

#include <limits>

namespace std {

template<
    class T,
    class P,
    class E
>
class numeric_limits<boost::numeric::safe<T, P, E> >
    : public std::numeric_limits<T>
{
    typedef boost::numeric::safe<T, P, E> SI;
public:
    // these expressions are not SAFE_NUMERIC_CONSTEXPR until C++14 so re-implement them here
    SAFE_NUMERIC_CONSTEXPR static SI min() noexcept { return std::numeric_limits<T>::min(); }
    SAFE_NUMERIC_CONSTEXPR static SI max() noexcept { return std::numeric_limits<T>::max(); }
};

} // std

#endif // BOOST_NUMERIC_SAFE_INTEGER_HPP
