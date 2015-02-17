#ifndef BOOST_NUMERIC_LIMITS_HPP
#define BOOST_NUMERIC_LIMITS_HPP

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
#include <cstdint> // intmax_t, uintmax_t
#include <boost/mpl/print.hpp>

namespace boost {
namespace numeric {

template<
    std::uintmax_t MIN,
    std::uintmax_t MAX,
    class P
>
class safe_unsigned_range;

template<
    std::intmax_t MIN,
    std::intmax_t MAX,
    class P
>
class safe_signed_range;

template<
    class T,
    class P
>
struct safe;

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
    typedef typename boost::mpl::print<T>::type t0;
public:
    // these expressions are not constexpr until C++14 so re-implement them here
    constexpr static SI min() noexcept { return std::numeric_limits<SI>::min(); }
    constexpr static SI max() noexcept { return std::numeric_limits<SI>::max(); }
};

/////////////////////////////////////////////////////////////////
// numeric limits for safe_(un)signed_range

// numeric limits for safe_signed_range
template<
    intmax_t MIN,
    intmax_t MAX,
    class P
>
class numeric_limits<boost::numeric::safe_signed_range<MIN, MAX, P> >
    : public boost::numeric::base_type<boost::numeric::safe_signed_range<MIN, MAX, P> >
{
    typedef  boost::numeric::base_type<boost::numeric::safe_signed_range<MIN, MAX, P> SSR;
    typedef typename boost::mpl::print<SSR>::type t0;
public:
    constexpr static SSR min() noexcept { return SSR(MIN); }
    constexpr static SSR max() noexcept { return SSR(MAX); }
};

/////////////////////////////////////////////////////////////////
// numeric limits for safe_unsigned_range
template<
    std::uintmax_t MIN,
    std::uintmax_t MAX,
    class P
>
class numeric_limits<boost::numeric::safe_unsigned_range<MIN, MAX, P> >
    : public boost::numeric::base_type<boost::numeric::safe_unsigned_range<MIN, MAX, P> >
{
    typedefboost::numeric::base_type<boost::numeric::safe_unsigned_range<MIN, MAX, P> SUR;
    typedef typename boost::mpl::print<SUR>::type t0;
public:
    constexpr static SUR min() noexcept { return SUR(MIN); }
    constexpr static SUR max() noexcept { return SUR(MAX); }
};

} // std
#endif // BOOST_NUMERIC_LIMITS_HPP
