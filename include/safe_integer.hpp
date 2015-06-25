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
#include "concept/numeric.hpp"
#include "exception_policies.hpp"
#include "native.hpp"

#include "boost/concept/assert.hpp"

// specialization for meta functions with safe<T> argument
namespace boost {
namespace numeric {

template<
    class T,
    class P,
    class E
>
struct safe;

template<
    class T,
    class P,
    class E
>
struct is_safe<safe<T, P, E> > : public std::true_type
{};

template<
    class T,
    class P,
    class E
>
struct get_promotion_policy<safe<T, P, E> > {
    typedef P type;
};

template<
    class T,
    class P,
    class E
>
struct get_exception_policy<safe<T, P, E> > {
    typedef E type;
};

template<
    class T,
    class P,
    class E
>
struct base_type<safe<T, P, E> > {
    typedef T type;
};

template<
    class T,
    class P,
    class E
>
SAFE_NUMERIC_CONSTEXPR T base_value(
    const safe<T, P, E>  & st
) {
    return static_cast<T>(st);
}

template<
    class T,
    class P = native,
    class E = throw_exception
>
struct safe : public safe_base<T, safe<T, P, E>, P, E>{
private:
    typedef safe_base<T, safe<T, P, E>, P, E > base_type;
    friend base_type;
    BOOST_CONCEPT_ASSERT((Integer<T>));
    BOOST_CONCEPT_ASSERT((PromotionPolicy<P>));
    BOOST_CONCEPT_ASSERT((ExceptionPolicy<E>));
    SAFE_NUMERIC_CONSTEXPR static const T min(){
        return std::numeric_limits<T>::min();
    }
    SAFE_NUMERIC_CONSTEXPR static const T max(){
        return std::numeric_limits<T>::max();
    }
public:
    // note: Rule of Three.  Don't specify custom move, copy etc.
    SAFE_NUMERIC_CONSTEXPR safe() :
        base_type()
    {}
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
    SAFE_NUMERIC_CONSTEXPR static SI min() noexcept {
        return SI(std::numeric_limits<T>::min());
    }
    SAFE_NUMERIC_CONSTEXPR static SI max() noexcept {
        return SI(std::numeric_limits<T>::max());
    }
};

} // std

#include "safe_base_operations.hpp"

#endif // BOOST_NUMERIC_SAFE_INTEGER_HPP
