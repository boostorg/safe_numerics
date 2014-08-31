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

#include <boost/integer.hpp>
#include "numeric.hpp"
#include "safe_base.hpp"
#include "native.hpp"

namespace boost {
namespace numeric {

typedef policies<boost::numeric::native, relaxed, throw_exception> default_policies;

/////////////////////////////////////////////////////////////////
// higher level types implemented in terms of safe_base

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

namespace boost {
namespace numeric {

/////////////////////////////////////////////////////////////////
// safe_signed_range

template<
    boost::intmax_t MIN,
    boost::intmax_t MAX,
    class P = boost::numeric::default_policies
>
class safe_signed_range : public
    safe_base<
        typename detail::signed_stored_type<MIN, MAX>::type, 
        safe_signed_range<MIN, MAX, P>
    >
{
    BOOST_STATIC_ASSERT_MSG(
        MIN < MAX,
        "minimum must be less than maximum"
    );
    typedef typename boost::numeric::safe_base<
        typename detail::signed_stored_type<MIN, MAX>::type, 
        safe_signed_range<MIN, MAX, P>
    > base;

public:
    typedef typename detail::signed_stored_type<MIN, MAX>::type stored_type;
    struct Policies {
        typedef P type;
    };

    template<class T>
    stored_type validate(const T & t) const {
        if(safe_compare::less_than(t, MIN)
        || safe_compare::greater_than(t, MAX))
            overflow("safe range out of range");
        return static_cast<stored_type>(t);
    }
    constexpr safe_signed_range() :
        base()
    {}

    template<class T>
    constexpr safe_signed_range(const T & t) :
        base(t)
    {}
};

template<
    boost::intmax_t MIN,
    boost::intmax_t MAX,
    class P
>
std::ostream & operator<<(
    std::ostream & os,
    const safe_signed_range<MIN, MAX, P> & t
){
    return os << t.get_stored_value();
}

template<
    boost::intmax_t MIN,
    boost::intmax_t MAX,
    class P
>
std::istream & operator>>(
    std::istream & is,
    safe_signed_range<MIN, MAX, P> & t
){
    return is >> t.get_stored_value();
}

/////////////////////////////////////////////////////////////////
// safe_unsigned_range

template<
    boost::uintmax_t MIN,
    boost::uintmax_t MAX,
    class P = boost::numeric::default_policies
>
class safe_unsigned_range : public
    safe_base<
        typename detail::unsigned_stored_type<MIN, MAX>::type,
        safe_unsigned_range<MIN, MAX, P>
    >
{
    BOOST_STATIC_ASSERT_MSG(
        MIN < MAX,
        "minimum must be less than maximum"
    );
    typedef typename boost::numeric::safe_base<
        typename detail::unsigned_stored_type<MIN, MAX>::type, 
        safe_unsigned_range<MIN, MAX, P>
    > base;

public:
    typedef typename detail::unsigned_stored_type<MIN, MAX>::type stored_type;

    struct Policies {
        typedef P type;
    };
    template<class T>
    stored_type validate(const T & t) const {
        if(safe_compare::less_than(t, MIN)
        || safe_compare::greater_than(t, MAX))
            overflow("safe range out of range");
        return static_cast<stored_type>(t);
    }
    constexpr safe_unsigned_range(){}

    template<class T>
    constexpr safe_unsigned_range(const T & t) :
        base(t)
    {}
};

template<
    boost::uintmax_t MIN,
    boost::uintmax_t MAX,
    class P
>
std::ostream & operator<<(
    std::ostream & os,
    const safe_unsigned_range<MIN, MAX, P> & t
){
    return os << t.get_stored_value();
}

template<
    boost::uintmax_t MIN,
    boost::uintmax_t MAX,
    class P
>
std::istream & operator>>(
    std::istream & is,
    safe_unsigned_range<MIN, MAX, P> & t
){
    return is >> t.get_stored_value();
}

} // numeric
} // boost


#endif // BOOST_NUMERIC_SAFE_RANGE_HPP
