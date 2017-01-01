#ifndef BOOST_NUMERIC_SAFE_BASE_HPP
#define BOOST_NUMERIC_SAFE_BASE_HPP

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
#include <type_traits> // is_integral, enable_if
#include <iosfwd>
#include <cassert>

#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/and.hpp>

#include "concept/numeric.hpp"
#include "concept/exception_policy.hpp"
#include "concept/promotion_policy.hpp"

#include "safe_common.hpp"
#include "exception_policies.hpp"

#include "boost/concept/assert.hpp"

namespace boost {
namespace numeric {

/////////////////////////////////////////////////////////////////
// forward declarations to support friend function declarations
// in safe_base

template<
    class Stored,
    Stored Min,
    Stored Max,
    class P, // promotion polic
    class E  // exception policy
>
class safe_base;

template<
    class T,
    T Min,
    T Max,
    class P,
    class E
>
struct is_safe<safe_base<T, Min, Max, P, E> > : public std::true_type
{};

template<
    class T,
    T Min,
    T Max,
    class P,
    class E
>
struct get_promotion_policy<safe_base<T, Min, Max, P, E> > {
    using type = P;
};

template<
    class T,
    T Min,
    T Max,
    class P,
    class E
>
struct get_exception_policy<safe_base<T, Min, Max, P, E> > {
    using type = E;
};

template<
    class T,
    T Min,
    T Max,
    class P,
    class E
>
struct base_type<safe_base<T, Min, Max, P, E> > {
    using type = T;
};

template<
    class T,
    T Min,
    T Max,
    class P,
    class E
>
constexpr T base_value(
    const safe_base<T, Min, Max, P, E>  & st
) {
    return static_cast<T>(st);
}

template<
    class T,
    T Min,
    T Max,
    class P, // promotion polic
    class E  // exception policy
>
std::ostream & operator<<(
    std::ostream & os,
    const safe_base<T, Min, Max, P, E> & t
);

template<
    class T,
    T Min,
    T Max,
    class P, // promotion polic
    class E  // exception policy
>
std::istream & operator>>(
    std::istream & is,
    safe_base<T, Min, Max, P, E> & t
);

template<typename T, T N>
class safe_literal_impl;

/////////////////////////////////////////////////////////////////
// Main implementation

template<
    class Stored,
    Stored Min,
    Stored Max,
    class P, // promotion polic
    class E  // exception policy
>
class safe_base {
    BOOST_CONCEPT_ASSERT((Integer<Stored>));
    BOOST_CONCEPT_ASSERT((PromotionPolicy<P>));
    BOOST_CONCEPT_ASSERT((ExceptionPolicy<E>));
    Stored m_t;

    friend std::ostream & operator<< <Stored, Min, Max, P, E> (
        std::ostream & os,
        const safe_base & t
    );

    friend std::istream & operator>> <Stored, Min, Max, P, E> (
        std::istream & is,
        safe_base & t
    );

    template<
        class StoredX,
        StoredX MinX,
        StoredX MaxX,
        class PX, // promotion polic
        class EX  // exception policy
    >
    friend class safe_base;

    friend class std::numeric_limits<
        safe_base<Stored, Min, Max, P, E>
    >;

    template<class T>
    constexpr Stored validated_cast(const T & t) const;

    template<typename T, T N>
    constexpr Stored validated_cast(const safe_literal_impl<T, N> & t) const;
protected:

public:
    ////////////////////////////////////////////////////////////
    // constructors

    constexpr explicit safe_base(const Stored & rhs, std::false_type);
    // default constructor
    constexpr explicit safe_base() {
        // this permits creating of invalid instances.  This is inline
        // with C++ built-in but violates the premises of the whole library
        // choice are:
        // do nothing - violates premise of he library that all safe objects
        // are valid
        // initialize to valid value - violates C++ behavior of types.
        // add "initialized" flag.  Preserves fixes the above, but doubles
        // "overhead"
        // still pending on this.
    }

    template<class T>
    constexpr /*explicit*/ safe_base(const T & t);

    // note: Rule of Five.  Don't specify
    // custom constructor, custom destructor, custom assignment
    // custom move, custom move assignment
    // Let the compiler build the defaults.
public:
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
    constexpr operator R () const;
    constexpr operator Stored () const;

    /////////////////////////////////////////////////////////////////
    // modification binary operators
    template<class T>
    constexpr safe_base & operator=(const T & rhs){
        m_t = validated_cast(rhs);
        return *this;
    }
    
    template<class T, T MinT, T MaxT, class PT, class ET>
    constexpr safe_base &
    operator=(const safe_base<T, MinT, MaxT, PT, ET> & rhs);

    // unary operators
    safe_base operator++(){      // pre increment
        return *this = *this + 1;
    }
    safe_base operator--(){      // pre increment
        return *this = *this - 1;
    }
    safe_base operator++(int){   // post increment
        safe_base old_t = *this;
        ++(*this);
        return old_t;
    }
    safe_base & operator--(int){ // post decrement
        safe_base old_t = *this;
        --(*this);
        return old_t;
    }
    safe_base operator-(){ // unary minus
        return *this = 0 - *this; // this will check for overflow
    }
    safe_base operator~(){
        static_assert(
            std::numeric_limits<Stored>::is_signed,
            "Bitwise inversion of signed value is an error"
        );
        return *this = ~(m_t);
    }
};


} // numeric
} // boost

/////////////////////////////////////////////////////////////////
// numeric limits for safe<int> etc.

#include <limits>

namespace std {

template<
    class T,
    T Min,
    T Max,
    class P,
    class E
>
class numeric_limits<boost::numeric::safe_base<T, Min, Max, P, E> >
    : public std::numeric_limits<T>
{
    using SB = boost::numeric::safe_base<T, Min, Max, P, E>;
public:
    constexpr static SB min() noexcept {
        return SB(Min, std::false_type());
    }
    constexpr static SB max() noexcept {
        return SB(Max, std::false_type());
    }
};

} // std

#endif // BOOST_NUMERIC_SAFE_BASE_HPP
