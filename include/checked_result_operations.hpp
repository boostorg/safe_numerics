#ifndef BOOST_NUMERIC_CHECKED_RESULT_OPERATIONS
#define BOOST_NUMERIC_CHECKED_RESULT_OPERATIONS

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
#include "exception.hpp"
//#include "exception_operations.hpp"
#include "checked_result.hpp"

#include <boost/logic/tribool.hpp>
#include "safe_compare.hpp"
#include "checked_default.hpp"
#include "checked_integer.hpp"

#include "utility.hpp"

namespace boost {
namespace numeric {

// integers addition
template<class T>
typename std::enable_if<
    std::is_integral<T>::value,
    checked_result<T>
>::type
constexpr inline operator+(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    constexpr const std::uint8_t order =
        static_cast<std::uint8_t>(safe_numerics_error::casting_error_count);
    constexpr const safe_numerics_error result[order][order]{
        // t == success
        {
            // u == ...
            safe_numerics_error::success,
            safe_numerics_error::positive_overflow_error,
            safe_numerics_error::negative_overflow_error,
            safe_numerics_error::range_error,
            safe_numerics_error::domain_error,
        },
        // t == positive_overflow_error,
        {
            // u == ...
            safe_numerics_error::positive_overflow_error,
            safe_numerics_error::positive_overflow_error,
            safe_numerics_error::range_error,
            safe_numerics_error::range_error,
            safe_numerics_error::domain_error,
        },
        // t == negative_overflow_error,
        {
            // u == ...
            safe_numerics_error::negative_overflow_error,
            safe_numerics_error::range_error,
            safe_numerics_error::negative_overflow_error,
            safe_numerics_error::range_error,
            safe_numerics_error::domain_error,
        },
        // t == range_error,
        {
            // u == ...
            safe_numerics_error::range_error,
            safe_numerics_error::range_error,
            safe_numerics_error::range_error,
            safe_numerics_error::range_error,
            safe_numerics_error::domain_error,
        },
        // t == domain_error,
        {
            // u == ...
            safe_numerics_error::domain_error,
            safe_numerics_error::domain_error,
            safe_numerics_error::domain_error,
            safe_numerics_error::domain_error,
            safe_numerics_error::domain_error,
        },
    };

    const safe_numerics_error e = result
        [static_cast<std::uint8_t>(static_cast<std::uint8_t>(t.m_e))]
        [static_cast<std::uint8_t>(static_cast<std::uint8_t>(u.m_e))]
    ;

    return
        safe_numerics_error::success == e
        ? checked::add<T>(t, u)
        : checked_result<T>(e, "addition result")
    ;
}

// when subtracting integers
template<class T>
typename std::enable_if<
    std::is_integral<T>::value && std::is_unsigned<T>::value,
    checked_result<T>
>::type
constexpr operator-(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    constexpr const std::uint8_t order =
        static_cast<std::uint8_t>(safe_numerics_error::casting_error_count);
    constexpr const safe_numerics_error result[order][order]{
        // t == success
        {
            // u == ...
            safe_numerics_error::success,
            safe_numerics_error::negative_overflow_error,
            safe_numerics_error::positive_overflow_error,
            safe_numerics_error::range_error,
            safe_numerics_error::domain_error,
        },
        // t == positive_overflow_error,
        {
            // u == ...
            safe_numerics_error::range_error,
            safe_numerics_error::range_error,
            safe_numerics_error::range_error,
            safe_numerics_error::range_error,
            safe_numerics_error::domain_error,
        },
        // t == negative_overflow_error,
        {
            // u == ...
            safe_numerics_error::range_error,
            safe_numerics_error::range_error,
            safe_numerics_error::negative_overflow_error,
            safe_numerics_error::range_error,
            safe_numerics_error::domain_error,
        },
        // t == range_error,
        {
            // u == ...
            safe_numerics_error::range_error,
            safe_numerics_error::range_error,
            safe_numerics_error::range_error,
            safe_numerics_error::range_error,
            safe_numerics_error::domain_error,
        },
        // t == domain_error,
        {
            // u == ...
            safe_numerics_error::domain_error,
            safe_numerics_error::domain_error,
            safe_numerics_error::domain_error,
            safe_numerics_error::domain_error,
            safe_numerics_error::domain_error,
        },
    };
    constexpr const safe_numerics_error e = result[t.m_e][u.m_e];
    return
        safe_numerics_error::success == e
        ? checked::subtract<T>(t.m_r, u.m_r)
        : checked_result<T>(e)
    ;
}

// comparison operators
template<class T>
constexpr boost::logic::tribool operator<(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    constexpr const std::uint8_t order =
        static_cast<std::uint8_t>(safe_numerics_error::casting_error_count);
    constexpr const boost::logic::tribool result[order][order]{
        // t == success
        {
            // u == ...
            boost::logic::tribool::tribool(indeterminate), // success,
            false, // negative_overflow_error,
            true,  // positive_overflow_error,
            boost::logic::tribool::tribool(indeterminate), // range_error,
            boost::logic::tribool::tribool(indeterminate) // domain_error,
        },
        // t == positive_overflow_error,
        {
            // u == ...
            false, // success,
            false, // negative_overflow_error,
            boost::logic::tribool::tribool(indeterminate), // positive_overflow_error,
            boost::logic::tribool::tribool(indeterminate), // range_error,
            boost::logic::tribool::tribool(indeterminate) // domain_error,
        },
        // t == negative_overflow_error,
        {
            // u == ...
            true, // success,
            boost::logic::tribool::tribool(indeterminate), // negative_overflow_error,
            true,  // positive_overflow_error,
            boost::logic::tribool::tribool(indeterminate), // range_error,
            boost::logic::tribool::tribool(indeterminate) // domain_error,
        },
        // t == range_error,
        {
            boost::logic::tribool::tribool(indeterminate), // success,
            boost::logic::tribool::tribool(indeterminate), // negative_overflow_error,
            boost::logic::tribool::tribool(indeterminate), // positive_overflow_error,
            boost::logic::tribool::tribool(indeterminate), // range_error,
            boost::logic::tribool::tribool(indeterminate) // domain_error,
        },
        // t == domain_error,
        {
            // u == ...
            boost::logic::tribool::tribool(indeterminate), // success,
            boost::logic::tribool::tribool(indeterminate), // negative_overflow_error,
            boost::logic::tribool::tribool(indeterminate), // positive_overflow_error,
            boost::logic::tribool::tribool(indeterminate), // range_error,
            boost::logic::tribool::tribool(indeterminate) // domain_error,
        },
    };
    // this seems to fail in preserving constexprness.
    // I can't figure out why.
    return (t.exception() || u.exception()) ?
        result
            [static_cast<std::uint8_t>(t.m_e)]
            [static_cast<std::uint8_t>(u.m_e)]
    :
        (t.m_r < u.m_r);
}
template<class T>
constexpr boost::logic::tribool
operator>=(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    // return ! (t < u);
    if(t.exception() || u.exception()){
        return boost::logic::tribool::tribool(indeterminate);
    }
    else{
        return !(t.m_r < u.m_r);
    }
}

template<class T>
constexpr boost::logic::tribool
operator>(
    const checked_result<T> & t,
    const checked_result<T> & u
) {
    constexpr const std::uint8_t order =
        static_cast<std::uint8_t>(safe_numerics_error::casting_error_count);
    constexpr const tribool result[order][order]{
        // t == success
        {
            // u == ...
            tribool::tribool(indeterminate), // success,
            true, // negative_overflow_error,
            false,  // positive_overflow_error,
            tribool::tribool(indeterminate), // range_error,
            tribool::tribool(indeterminate) // domain_error,
        },
        // t == positive_overflow_error,
        {
            // u == ...
            true, // success,
            true, // negative_overflow_error,
            tribool::tribool(indeterminate), // positive_overflow_error,
            tribool::tribool(indeterminate), // range_error,
            tribool::tribool(indeterminate) // domain_error,
        },
        // t == negative_overflow_error,
        {
            // u == ...
            false, // success,
            tribool::tribool(indeterminate), // negative_overflow_error,
            false,  // positive_overflow_error,
            tribool::tribool(indeterminate), // range_error,
            tribool::tribool(indeterminate) // domain_error,
        },
        // t == range_error,
        {
            tribool::tribool(indeterminate), // success,
            tribool::tribool(indeterminate), // negative_overflow_error,
            tribool::tribool(indeterminate), // positive_overflow_error,
            tribool::tribool(indeterminate), // range_error,
            tribool::tribool(indeterminate) // domain_error,
        },
        // t == domain_error,
        {
            // u == ...
            tribool::tribool(indeterminate), // success,
            tribool::tribool(indeterminate), // negative_overflow_error,
            tribool::tribool(indeterminate), // positive_overflow_error,
            tribool::tribool(indeterminate), // range_error,
            tribool::tribool(indeterminate) // domain_error,
        },
    };
    /*
    return (t.exception() || u.exception())
    ? result
            [static_cast<std::uint8_t>(t.m_e)]
            [static_cast<std::uint8_t>(u.m_e)]
    : t.m_r < u.m_r;
    */
    if(t.exception() || u.exception()){
        return result
            [static_cast<std::uint8_t>(t.m_e)]
            [static_cast<std::uint8_t>(u.m_e)];
    }
    else{
        return t.m_r < u.m_r;
    }
}

template<class T>
constexpr boost::logic::tribool
operator<=(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    //return ! (t < u);
    if(t.exception() || u.exception()){
        return boost::logic::tribool::tribool(indeterminate);
    }
    else{
        return !(t.m_r < u.m_r);
    }
}

template<class T>
constexpr boost::logic::tribool
operator==(
    const checked_result<T> & t,
    const checked_result<T> & u
){
    constexpr const std::uint8_t order =
        static_cast<std::uint8_t>(safe_numerics_error::casting_error_count);
    constexpr const tribool result[order][order]{
        // t == success
        {
            // u == ...
            tribool::tribool(indeterminate), // success,
            false, // negative_overflow_error,
            false,  // positive_overflow_error,
            tribool::tribool(indeterminate), // range_error,
            tribool::tribool(indeterminate) // domain_error,
        },
        // t == positive_overflow_error,
        {
            // u == ...
            false, // success,
            false, // negative_overflow_error,
            tribool::tribool(indeterminate), // positive_overflow_error,
            tribool::tribool(indeterminate), // range_error,
            tribool::tribool(indeterminate) // domain_error,
        },
        // t == negative_overflow_error,
        {
            // u == ...
            false, // success,
            tribool::tribool(indeterminate), // negative_overflow_error,
            false,  // positive_overflow_error,
            tribool::tribool(indeterminate), // range_error,
            tribool::tribool(indeterminate) // domain_error,
        },
        // t == range_error,
        {
            tribool::tribool(indeterminate), // success,
            tribool::tribool(indeterminate), // negative_overflow_error,
            tribool::tribool(indeterminate), // positive_overflow_error,
            tribool::tribool(indeterminate), // range_error,
            tribool::tribool(indeterminate) // domain_error,
        },
        // t == domain_error,
        {
            // u == ...
            tribool::tribool(indeterminate), // success,
            tribool::tribool(indeterminate), // negative_overflow_error,
            tribool::tribool(indeterminate), // positive_overflow_error,
            tribool::tribool(indeterminate), // range_error,
            tribool::tribool(indeterminate) // domain_error,
        },
    };
    if(t.exception() || u.exception()){
        return result
            [static_cast<std::uint8_t>(t.m_e)]
            [static_cast<std::uint8_t>(u.m_e)];
    }
    else{
        return t.m_r == u.m_r;
    }
}
template<class T, class U>
constexpr boost::logic::tribool
operator!=(const checked_result<T> & t, const checked_result<U> & u){
    return ! (t == u);
}

} // numeric
} // boost

#include <iosfwd>

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

#endif  // BOOST_NUMERIC_CHECKED_RESULT_OPERATIONS
