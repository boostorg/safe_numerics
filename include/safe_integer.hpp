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

#include <limits>

#include <boost/utility/enable_if.hpp>
#include <boost/mpl/or.hpp>
#include "safe_base_operations.hpp"
#include "policies.hpp"
#include "native.hpp"

typedef boost::numeric::policies<boost::numeric::native, boost::numeric::throw_exception> default_policies;

namespace boost {
namespace numeric {

template<
    class T,
    class P = default_policies
>
struct safe : public safe_base<T, safe<T, P>, P>{
    BOOST_CONCEPT_ASSERT((Integer<T>));
    BOOST_CONCEPT_ASSERT((ExceptionPolicy<
        typename get_exception_policy<P>::type
    >));
    typedef safe_base<T, safe<T, P>, P > base_type;

    constexpr bool validate(const T & t) const {
        return true;
    }
    constexpr bool validate(T && t) const {
        return true;
    }

    constexpr safe() :
        base_type()
    {}

    //constexpr safe(safe && s) :
    //    base_type()
    //{}
    template<class U>
    constexpr safe(const U & u) :
        base_type(u)
    {}
};

} // numeric
} // boost

namespace std {

/////////////////////////////////////////////////////////////////
// numeric limits for safe<int> etc.

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
    constexpr static SI min() noexcept { return std::numeric_limits<T>::min(); }
    constexpr static SI max() noexcept { return std::numeric_limits<T>::max(); }
};

} // std

#endif // BOOST_NUMERIC_SAFE_INTEGER_HPP
