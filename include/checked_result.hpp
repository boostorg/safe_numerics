#ifndef BOOST_NUMERIC_CHECKED_RESULT
#define BOOST_NUMERIC_CHECKED_RESULT

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// contains operations for doing checked aritmetic on NATIVE
// C++ types.

#include <cassert>
#include <boost/logic/tribool.hpp>

#include "safe_common.hpp"
#include "safe_compare.hpp"
#include "exception.hpp"

namespace boost {
namespace numeric {

template<typename R>
struct checked_result {
    safe_numerics_error m_e;
    union {
        R m_r;
        char const * m_msg;
    };
    // constructors - use default copy constructor
    // checked_result(const checked_result<R> &) = default;
    
    // don't permit construction without initial value;
    checked_result() = delete;

    constexpr /*explicit*/ checked_result(const R & r) :
        m_e(safe_numerics_error::success),
        m_r(r)
    {}
    constexpr /*explicit*/ checked_result(
        safe_numerics_error e,
        const char * msg
    ) :
        m_e(e),
        m_msg(msg)
    {}
    // permit construct from convertible type
    template<typename T>
    constexpr /*explicit*/ checked_result<T>(const checked_result<T> & t) :
        m_e(t.m_e),
        m_r(t.m_r)
    {
        if(! t.exception())
            m_r = t.m_r;
        else
            m_msg = t.m_msg;
    }

    constexpr bool exception() const {
        return m_e != safe_numerics_error::success;
    }

    // accesors
    constexpr operator R() const {
        assert(! exception());
        return m_r;
    }
    
    constexpr operator const char *() const {
        assert(exception());
        return m_msg;
    }

    // disallow assignment
    // damn! turns out I use this in a sort of pathological case
    // fix this later
    // checked_result & operator=(const checked_result &) = delete;

    // comparison operators
    template<class T>
    constexpr boost::logic::tribool operator<(const checked_result<T> & t) const {
        return
            (! this->exception() && ! t.exception()) ?
                safe_compare::less_than(m_r, t.m_r)
            :
                boost::logic::tribool::indeterminate_value
            ;
    }
    template<class T>
    constexpr boost::logic::tribool operator>=(const checked_result<T> & t) const {
        return ! operator<(t);
    }
    template<class T>
    constexpr boost::logic::tribool operator>(const checked_result<T> & t) const {
        return
            (! this->exception() && ! t.exception()) ?
                safe_compare::greater_than(m_r, t.m_r)
            :
                boost::logic::tribool::indeterminate_value
            ;
    }
    template<class T>
    constexpr boost::logic::tribool  operator<=(const checked_result<T> & t) const {
        return ! operator>(t);
    }
    template<class T>
    constexpr boost::logic::tribool operator!=(const checked_result<T> & t) const {
        return operator<(t) || operator>(t);
    }
    template<class T>
    constexpr boost::logic::tribool operator==(const checked_result<T> & t) const {
        return ! operator!=(t);
    }
};

template<typename T>
constexpr bool operator==(const checked_result<T> &lhs, const safe_numerics_error & rhs){
    return (! lhs.exception()) ? false : lhs.m_e == rhs;
}
template<typename T>
constexpr bool operator==(const safe_numerics_error & lhs, const checked_result<T> &rhs){
    return (! rhs.exception()) ? false : lhs = rhs.m_e;
}
template<typename T>
constexpr bool operator!=(const checked_result<T> &lhs, const safe_numerics_error & rhs){
    return ! (lhs == rhs);
}
template<typename T>
constexpr bool operator!=(const safe_numerics_error & lhs, const checked_result<T> &rhs){
    return ! (lhs == rhs);
}

// invoke error handling
template<class EP, typename R>
constexpr void
dispatch(const checked_result<R> & cr){
    // if the result contains an error condition
    if(cr.exception())
        // dispatch to the appropriate function
        dispatch<EP>(cr.m_e, cr.m_msg);
    // otherwise just do a simple return
}

} // numeric
} // boost

#include <ostream>
#include <istream>

namespace std {

template<typename CharT, typename Traits, typename R>
inline std::basic_ostream<CharT, Traits> & operator<<(
    std::basic_ostream<CharT, Traits> & os,
    const boost::numeric::checked_result<R> & r
){
    if(!r.exception())
        os << static_cast<R>(r);
    else
        os << r.m_msg; //static_cast<const char *>(r);
    return os;
}

template<typename CharT, typename Traits>
inline std::basic_ostream<CharT, Traits> & operator<<(
    std::basic_ostream<CharT, Traits> & os,
    const boost::numeric::checked_result<signed char> & r
){
    if(! r.exception())
        os << static_cast<std::int16_t>(r);
    else
        os << r.m_msg; //static_cast<const char *>(r);
    return os;
}

template<typename CharT, typename Traits>
inline std::basic_ostream<CharT, Traits> & operator<<(
    std::basic_ostream<CharT, Traits> & os,
    const boost::numeric::checked_result<unsigned char> & r
){
    if(! r.exception())
        os << static_cast<std::uint16_t>(r);
    else
        os << r.m_msg; //static_cast<const char *>(r);
    return os;
}

template<typename CharT, typename Traits, typename R>
inline std::basic_istream<CharT, Traits> & operator>>(
    std::basic_istream<CharT, Traits> & is,
    boost::numeric::checked_result<R> & r
){
    is >> r.m_r;
    return is;
}

template<typename CharT, typename Traits, typename R>
inline std::basic_istream<CharT, Traits> & operator>>(
    std::basic_istream<CharT, Traits> & is, 
    boost::numeric::checked_result<signed char> & r
){
    std::int16_t i;
    is >> i;
    r.m_r = i;
    return is;
}

template<typename CharT, typename Traits, typename R>
inline std::basic_istream<CharT, Traits> & operator>>(
    std::basic_istream<CharT, Traits> & is,
    boost::numeric::checked_result<unsigned char> & r
){
    std::uint16_t i;
    is >> i;
    r.m_r = i;
    return is;
}

} // std

/////////////////////////////////////////////////////////////////
// numeric limits for checked<R>

#include <limits>

namespace std {

template<class R>
class numeric_limits<boost::numeric::checked_result<R> >
    : public std::numeric_limits<R>
{
    using this_type = boost::numeric::checked_result<R>;
public:
    constexpr static this_type min() noexcept {
        return this_type(std::numeric_limits<R>::min());
    }
    constexpr static this_type max() noexcept {
        return this_type(std::numeric_limits<R>::max());
    }
};

} // std

#endif  // BOOST_NUMERIC_CHECKED_RESULT
