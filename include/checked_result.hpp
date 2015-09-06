//
//  checked_result.hpp
//  SafeIntegers
//
//  Created by Robert Ramey on 2/26/15.
//
//

#ifndef BOOST_NUMERIC_CHECKED_RESULT
#define BOOST_NUMERIC_CHECKED_RESULT

#include <cstddef> //nullptr_t
#include <cassert>
#include <boost/logic/tribool.hpp>

#include "safe_common.hpp" // SAFE_NUMERIC_CONSTEXPR
#include "exception_policies.hpp"
#include "concept/exception_policy.hpp"
#include "safe_compare.hpp"

namespace boost {
namespace numeric {

enum class exception_type {
    no_exception,
    overflow_error,
    underflow_error,
    range_error,
    domain_error,
    uninitialized,
    static_assertion
};

template<typename R>
struct checked_result {
// poor man's variant which supports SAFE_NUMERIC_CONSTEXPR
    exception_type m_e;
    union {
        R m_r;
        char const * m_msg;
    };
    // constructors
    // breaks add/subtract etc.!!!
    // perhaps because defining a copy constructure suppresses implicite move?
    /*
    SAFE_NUMERIC_CONSTEXPR checked_result(const checked_result<R> & r) :
        m_e(r.m_e)
    {
        (m_e == exception_type::no_exception) ?
            (m_r = r.m_r), 0
        :
            (m_msg = r.m_msg), 0
        ;
    }
    // don't permit construction without initial value;
    SAFE_NUMERIC_CONSTEXPR explicit checked_result() :
        m_e(exception_type::uninitialized),
        m_r(0)
    {}
    template<class T>
    SAFE_NUMERIC_CONSTEXPR //explicit// checked_result(const checked_result<T> & t) :
        m_e(t.m_e),
        m_r(t.m_r)
    {}
    */

    SAFE_NUMERIC_CONSTEXPR /*explicit*/ checked_result(const R & r) :
        m_e(exception_type::no_exception),
        m_r(r)
    {}

    SAFE_NUMERIC_CONSTEXPR /*explicit*/ checked_result(
        exception_type e,
        const char * msg
    ) :
        m_e(e),
        m_msg(msg)
    {}

    // accesors
    SAFE_NUMERIC_CONSTEXPR operator R() const {
       // assert(exception_type::no_exception == m_e);
        return m_r;
    }
    /*
    SAFE_NUMERIC_CONSTEXPR operator exception_type() const {
        return m_e;
    }
    SAFE_NUMERIC_CONSTEXPR operator const char *() const {
        // assert(exception_type::no_exception != m_e);
        return m_msg;
    }
    */

    template<class T>
    SAFE_NUMERIC_CONSTEXPR boost::logic::tribool operator<(const checked_result<T> & t) const {
        return
            (m_e == exception_type::no_exception
            || t.m_e == exception_type::no_exception) ?
                boost::logic::tribool::indeterminate_value
            :
                safe_compare::less_than(m_r, t.m_r)
            ;
    }
    template<class T>
    SAFE_NUMERIC_CONSTEXPR boost::logic::tribool operator>=(const checked_result<T> & t) const {
        return ! operator<(t);
    }
    template<class T>
    SAFE_NUMERIC_CONSTEXPR boost::logic::tribool operator>(const checked_result<T> & t) const {
        return !
            (m_e == exception_type::no_exception
            || t.m_e == exception_type::no_exception) ?
                boost::logic::tribool::indeterminate_value
            :
                safe_compare::greater_than(m_r, t.m_r)
            ;
    }
    template<class T>
    SAFE_NUMERIC_CONSTEXPR boost::logic::tribool  operator<=(const checked_result<T> & t) const {
        return ! operator>(t) && ! operator<(t);
    }
    template<class T>
    SAFE_NUMERIC_CONSTEXPR boost::logic::tribool operator==(const checked_result<T> & t) const {
        return ! operator>(t);
    }

    SAFE_NUMERIC_CONSTEXPR bool operator==(const exception_type & et) const {
        return m_e == et;
    }
    SAFE_NUMERIC_CONSTEXPR bool operator!=(const exception_type & et) const {
        return m_e != et;
    }
    SAFE_NUMERIC_CONSTEXPR bool no_exception() const {
        return m_e == exception_type::no_exception;
    }

/*
    template<class EP>
    void
    dispatch() const {
        switch(m_e){
        case exception_type::overflow_error:
            EP::overflow_error(m_msg);
            break;
        case exception_type::underflow_error:
            EP::underflow_error(m_msg);
            break;
        case exception_type::range_error:
            EP::range_error(m_msg);
            break;
        case exception_type::domain_error:
            EP::domain_error(m_msg);
            break;
        case exception_type::no_exception:
            break;
        default:
            break;
        }
    }
};
*/

    template<class EP>
    SAFE_NUMERIC_CONSTEXPR void
    dispatch() const {
        switch(m_e){
        case exception_type::overflow_error:
            EP::overflow_error(m_msg);
            break;
        case exception_type::underflow_error:
            EP::underflow_error(m_msg);
            break;
        case exception_type::range_error:
            EP::range_error(m_msg);
            break;
        case exception_type::domain_error:
            EP::domain_error(m_msg);
            break;
        case exception_type::no_exception:
            break;
        default:
            break;
        }
    }
};

template<typename R>
SAFE_NUMERIC_CONSTEXPR inline const checked_result<R> min(
    const checked_result<R> & t,
    const checked_result<R> & u
){
    return
        (t.m_e == exception_type::no_exception
        && u.m_e == exception_type::no_exception) ?
            t.m_r < u.m_r ?
                t
            :
                u
        :
            checked_result<R>(
                exception_type::range_error,
                "Can't compare values without values"
            )
        ;
}

template<typename R>
SAFE_NUMERIC_CONSTEXPR inline const checked_result<R> max(
    const checked_result<R> & t,
    const checked_result<R> & u
){
    return
        (t.m_e == exception_type::no_exception
        && u.m_e == exception_type::no_exception) ?
            t.m_r < u.m_r ?
                u
            :
                t
        :
            checked_result<R>(
                exception_type::range_error,
                "Can't compare values without values"
            )
        ;
}

} // numeric
} // boost

//#include <iosfwd>
#include <ostream>
#include <istream>

namespace std {

template<typename R>
std::ostream & operator<<(std::ostream & os, const boost::numeric::checked_result<R> & r){
    if(r == boost::numeric::exception_type::no_exception)
        os << static_cast<R>(r);
    else
        os << r.m_msg; //static_cast<const char *>(r);
    return os;
}

template<>
std::ostream & operator<<(std::ostream & os, const boost::numeric::checked_result<std::int8_t> & r){
    if(r == boost::numeric::exception_type::no_exception)
        os << static_cast<std::int16_t>(r);
    else
        os << r.m_msg; //static_cast<const char *>(r);
    return os;
}

template<>
std::ostream & operator<<(std::ostream & os, const boost::numeric::checked_result<std::uint8_t> & r){
    if(r == boost::numeric::exception_type::no_exception)
        os << static_cast<std::uint16_t>(r);
    else
        os << r.m_msg; //static_cast<const char *>(r);
    return os;
}

/*
template<typename R>
std::istream & operator>>(std::istream & is, const boost::numeric::checked_result<R> & r){
    is >> r.m_r;
    return is;
}

template<typename R>
std::istream & operator>>(std::istream & is, const boost::numeric::checked_result<std::int8_t> & r){
    std::int16_t i;
    is >> i;
    r.m_r = i;
    return is;
}

template<typename R>
std::istream & operator>>(std::istream & is, const boost::numeric::checked_result<std::uint8_t> & r){
    std::uint16_t i;
    is >> i;
    r.m_r = i;
    return is;
}
*/

} // std

/////////////////////////////////////////////////////////////////
// numeric limits for checked<R>

#include <limits>

namespace std {

template<class R>
class numeric_limits<boost::numeric::checked_result<R> >
    : public std::numeric_limits<R>
{
    typedef boost::numeric::checked_result<R> this_type;
public:
    SAFE_NUMERIC_CONSTEXPR static this_type min() noexcept {
        return this_type(std::numeric_limits<R>::min());
    }
    SAFE_NUMERIC_CONSTEXPR static this_type max() noexcept {
        return this_type(std::numeric_limits<R>::max());
    }
};

} // std

#endif  // BOOST_NUMERIC_CHECKED_CHECKED_RESULT
