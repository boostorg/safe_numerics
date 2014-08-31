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
#include <boost/integer_traits.hpp>
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

// the following is necessary because we need numeric_limits to be
// constexpr - but this won't be available until C++14.  So make our
// namespace which implements these.  When C++14 comes around, we can
// just eliminate everything but the "using namespace std"
//namespace limits {

namespace X {
    template<class T>
    class numeric_limits : public std::numeric_limits<T> {};
}

namespace std {

// until C++14
template<class T>
class numeric_limits
    : public X::numeric_limits<T>
{
            typedef typename boost::mpl::print<T>::type t0;
public:
    // these expressions are not constexpr until C++14 so re-implement them here
    BOOST_STATIC_CONSTEXPR T min() BOOST_NOEXCEPT { return boost::integer_traits<T>::const_min; }
    BOOST_STATIC_CONSTEXPR T max() BOOST_NOEXCEPT { return boost::integer_traits<T>::const_max; }
    // BOOST_STATIC_CONSTEXPR SI lowest() BOOST_NOEXCEPT { return boost::integer_traits<T>::const_min;; }
};

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
    BOOST_STATIC_CONSTEXPR SI min() BOOST_NOEXCEPT { return boost::integer_traits<T>::const_min; }
    BOOST_STATIC_CONSTEXPR SI max() BOOST_NOEXCEPT { return boost::integer_traits<T>::const_max; }
    // BOOST_STATIC_CONSTEXPR SI lowest() BOOST_NOEXCEPT { return boost::integer_traits<T>::const_min;; }
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
    : public numeric_limits<int>
{
    typedef boost::numeric::safe_signed_range<MIN, MAX, P> SSR;
    typedef typename boost::mpl::print<SSR>::type t0;
public:
    BOOST_STATIC_CONSTEXPR SSR min() BOOST_NOEXCEPT { return SSR(MIN); }
    BOOST_STATIC_CONSTEXPR SSR max() BOOST_NOEXCEPT { return SSR(MAX); }
    // BOOST_STATIC_CONSTEXPR SSR lowest() BOOST_NOEXCEPT { return SSR(MIN); }

    // BOOST_STATIC_CONSTANT(int,digits = (boost::numeric::detail::log<MAX, 2>::value)); // in base 2
    // BOOST_STATIC_CONSTANT(int,digits10 = (boost::numeric::detail::log<MAX, 10>::value)); // in base 10
    // BOOST_STATIC_CONSTANT(int,max_digits10 = digits10);
};

// until C++14
/////////////////////////////////////////////////////////////////
// numeric limits for safe_unsigned_range
template<
    std::uintmax_t MIN,
    std::uintmax_t MAX,
    class P
>
class numeric_limits<boost::numeric::safe_unsigned_range<MIN, MAX, P> >
    : public numeric_limits<unsigned int>
{
    typedef boost::numeric::safe_unsigned_range<MIN, MAX, P> SUR;
            typedef typename boost::mpl::print<SUR>::type t0;
public:
    BOOST_STATIC_CONSTEXPR SUR min() BOOST_NOEXCEPT { return SUR(MIN); }
    BOOST_STATIC_CONSTEXPR SUR max() BOOST_NOEXCEPT { return SUR(MAX); }
    // BOOST_STATIC_CONSTEXPR SUR lowest() BOOST_NOEXCEPT { return SUR(MIN); }

    // BOOST_STATIC_CONSTEXPR int digits = boost::numeric::detail::ulog<MAX, 2>::value; // in base 2
    // BOOST_STATIC_CONSTEXPR int digits10 = boost::numeric::detail::ulog<MAX, 10>::value; // in base 10
    // BOOST_STATIC_CONSTEXPR int max_digits10 = digits10;
};

// until C++14
/////////////////////////////////////////////////////////////////

} // std
#endif // BOOST_NUMERIC_LIMITS_HPP
