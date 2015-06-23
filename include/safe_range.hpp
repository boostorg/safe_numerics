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

#include "safe_base.hpp"
#include "safe_base_operations.hpp"
#include "native.hpp"
#include "exception_policies.hpp"
#include "native.hpp"

/////////////////////////////////////////////////////////////////
// higher level types implemented in terms of safe_base

namespace boost {
namespace numeric {

namespace detail {
    template<
        boost::intmax_t MIN,
        boost::intmax_t MAX
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
        boost::uintmax_t MIN,
        boost::uintmax_t MAX
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

} // numeric
} // boost

/////////////////////////////////////////////////////////////////
// safe_signed_range

namespace boost {
namespace numeric {

template<
    boost::intmax_t MIN,
    boost::intmax_t MAX,
    class P,
    class E
>
struct safe_signed_range;

template<
    boost::intmax_t MIN,
    boost::intmax_t MAX,
    class P,
    class E
>
struct is_safe<safe_signed_range<MIN, MAX, P, E> > : public std::true_type
{};

template<
    boost::intmax_t MIN,
    boost::intmax_t MAX,
    class P,
    class E
>
struct get_promotion_policy<safe_signed_range<MIN, MAX, P, E> > {
    typedef P type;
};

template<
    boost::intmax_t MIN,
    boost::intmax_t MAX,
    class P,
    class E
>
struct get_exception_policy<safe_signed_range<MIN, MAX, P, E> > {
    typedef E type;
};

template<
    boost::intmax_t MIN,
    boost::intmax_t MAX,
    class P,
    class E
>
struct base_type<safe_signed_range<MIN, MAX, P, E> > {
    typedef T type;
};

template<
    boost::intmax_t MIN,
    boost::intmax_t MAX,
    class P = native
    class E = throw_exception
>
class safe_signed_range : public
    safe_base<
        typename detail::signed_stored_type<MIN, MAX>::type,
        safe_signed_range<MIN, MAX, P, E>,
        P,
        E
    >
{
    static_assert(
        MIN <= MAX,
        "minimum cannot exceed maximum"
    );
    BOOST_CONCEPT_ASSERT((PromotionPolicy<P>));
    BOOST_CONCEPT_ASSERT((ExceptionPolicy<E>));
    typedef typename detail::signed_stored_type<MIN, MAX>::type stored_type;

    typedef typename boost::numeric::safe_base<
        stored_type,
        safe_signed_range<MIN, MAX, P, E>
    > base_type;
    friend base_type;

public:
    // note: Rule of Three.  Don't specify custom move, copy etc.
    SAFE_NUMERIC_CONSTEXPR safe_signed_range() :
        base_type()
    {}
    template<class T>
    SAFE_NUMERIC_CONSTEXPR safe_signed_range(const U & u) :
        base_type(u)
    {}

};

} // numeric
} // boost

/////////////////////////////////////////////////////////////////
// numeric limits for safe_signed_range

#include <limits>

namespace std {

template<
    intmax_t MIN,
    intmax_t MAX,
    class P,
    class E
>
class numeric_limits<boost::numeric::safe_signed_range<MIN, MAX, P, E> >
    : public boost::numeric::base_type<boost::numeric::safe_signed_range<MIN, MAX, P, E> >
{
    typedef  boost::numeric::base_type<boost::numeric::safe_signed_range<MIN, MAX, P, E> SSR;
    typedef typename boost::mpl::print<SSR>::type t0;
public:
    SAFE_NUMERIC_CONSTEXPR static SSR min() noexcept { return SSR(MIN); }
    SAFE_NUMERIC_CONSTEXPR static SSR max() noexcept { return SSR(MAX); }
};

} // std

/////////////////////////////////////////////////////////////////
// safe_unsigned_range

namespace boost {
namespace numeric {

template<
    boost::uintmax_t MIN,
    boost::uintmax_t MAX,
    class P,
    class E
>
struct safe_unsigned_range;

template<
    boost::uintmax_t MIN,
    boost::uintmax_t MAX,
    class P,
    class E
>
struct is_safe<safe_unsigned_range<MIN, MAX, P, E> > : public std::true_type
{};

template<
    boost::uintmax_t MIN,
    boost::uintmax_t MAX,
    class P,
    class E
>
struct get_promotion_policy<safe_unsigned_range<MIN, MAX, P, E> > {
    typedef P type;
};

template<
    boost::uintmax_t MIN,
    boost::uintmax_t MAX,
    class P,
    class E
>
struct get_exception_policy<safe_unsigned_range<MIN, MAX, P, E> > {
    typedef E type;
};

template<
    boost::uintmax_t MIN,
    boost::uintmax_t MAX,
    class P,
    class E
>
struct base_type<safe_unsigned_range<MIN, MAX, P, E> > {
    typedef T type;
};

template<
    boost::uintmax_t MIN,
    boost::uintmax_t MAX,
    class P = native
    class E = throw_exception
>
class safe_unsigned_range : public
    safe_base<
        typename detail::unsigned_stored_type<MIN, MAX>::type,
        safe_unsigned_range<MIN, MAX, P, E>,
        P,
        E
    >
{
private
    static_assert(
        MIN < MAX,
        "minimum must be less than maximum"
    );
    BOOST_CONCEPT_ASSERT((PromotionPolicy<E>));
    BOOST_CONCEPT_ASSERT((ExceptionPolicy<E>));
    typedef typename boost::numeric::safe_base<
        typename detail::unsigned_stored_type<MIN, MAX>::type,
        safe_unsigned_range<MIN, MAX, P, E>,
        P,
        E
    > base_type;
    friend base_type;

public:
    // note: Rule of Three.  Don't specify custom move, copy etc.
    SAFE_NUMERIC_CONSTEXPR safe_unsigned_range() :
        base_type()
    {}
    template<class T>
    SAFE_NUMERIC_CONSTEXPR safe_unsigned_range(const U & u) :
        base_type(u)
    {}
};

} // numeric
} // boost

/////////////////////////////////////////////////////////////////
// numeric limits for safe_unsigned_range
namespace std {

template<
    std::uintmax_t MIN,
    std::uintmax_t MAX,
    class P,
    class E
>
class numeric_limits<boost::numeric::safe_unsigned_range<MIN, MAX, P> >
    : public boost::numeric::base_type<boost::numeric::safe_unsigned_range<MIN, MAX, P, E> >
{
    typedefboost::numeric::base_type<boost::numeric::safe_unsigned_range<MIN, MAX, P, E> SUR;
    typedef typename boost::mpl::print<SUR>::type t0;
public:
    SAFE_NUMERIC_CONSTEXPR static SUR min() noexcept { return SUR(MIN); }
    SAFE_NUMERIC_CONSTEXPR static SUR max() noexcept { return SUR(MAX); }
};

} // std
#endif // BOOST_NUMERIC_SAFE_RANGE_HPP
