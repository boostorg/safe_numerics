//
//  checked_result.hpp
//  SafeIntegers
//
//  Created by Robert Ramey on 2/26/15.
//
//

#ifndef BOOST_NUMERIC_CHECKED_RESULT
#define BOOST_NUMERIC_CHECKED_RESULT

#include <boost/logic/tribool.hpp>

#include "safe_common.hpp" // constexpr
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
    exception_type m_e;
    union {
        R m_r;
        char const * m_msg;
    };
    // constructors
    // can't select constructor based on the current status of another
    // checked_result object.
    /*
    constexpr checked_result(const checked_result<R> & r) :
        m_e(r.m_e)
    {
        (no_exception()) ?
            (m_r = r.m_r), 0
        :
            (m_msg = r.m_msg), 0
        ;
    }
    */
    // don't permit construction without initial value;
    checked_result() = delete;

    constexpr /*explicit*/ checked_result(const R & r) :
        m_e(exception_type::no_exception),
        m_r(r)
    {}

    constexpr /*explicit*/ checked_result(
        exception_type e,
        const char * msg
    ) :
        m_e(e),
        m_msg(msg)
    {}

    // accesors
    constexpr operator R() const {
        // assert(exception_type::no_exception == m_e);
        return m_r;
    }

    constexpr operator const char *() const {
        // assert(exception_type::no_exception != m_e);
        return m_msg;
    }

    template<class T>
    constexpr boost::logic::tribool operator<(const checked_result<T> & t) const {
        return
            (this->no_exception() && t.no_exception()) ?
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
            (this->no_exception() && t.no_exception()) ?
                safe_compare::greater_than(m_r, t.m_r)
            :
                boost::logic::tribool::indeterminate_value
            ;
    }
    template<class T>
    constexpr boost::logic::tribool  operator<=(const checked_result<T> & t) const {
        return ! operator>(t) && ! operator<(t);
    }
    template<class T>
    constexpr boost::logic::tribool operator==(const checked_result<T> & t) const {
        return ! operator>(t);
    }
/*
    constexpr bool operator==(const exception_type & et) const {
        return m_e == et;
    }
    constexpr bool operator!=(const exception_type & et) const {
        return m_e != et;
    }
*/
    constexpr bool no_exception() const {
        return m_e == exception_type::no_exception;
    }

    template<class EP>
    constexpr void
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
        case exception_type::uninitialized:
            EP::domain_error(m_msg);
        case exception_type::no_exception:
            break;
        default:
            break;
        }
    }
};

// C++ does not (yet) permit constexpr lambdas.  So create some
// constexpr predicates to be used by constexpr algorthms.
template<typename R>
constexpr bool no_exception(const checked_result<R> & cr){
    return cr.no_exception();
}

} // numeric
} // boost

#include <ostream>
#include <istream>

namespace std {

template<typename R>
std::ostream & operator<<(
    std::ostream & os,
    const boost::numeric::checked_result<R> & r
){
    if(r.no_exception())
        os << static_cast<R>(r);
    else
        os << r.m_msg; //static_cast<const char *>(r);
    return os;
}

template<>
std::ostream & operator<<(
    std::ostream & os,
    const boost::numeric::checked_result<std::int8_t> & r
){
    if(r.no_exception())
        os << static_cast<std::int16_t>(r);
    else
        os << r.m_msg; //static_cast<const char *>(r);
    return os;
}

template<>
std::ostream & operator<<(
    std::ostream & os,
    const boost::numeric::checked_result<std::uint8_t> & r
){
    if(r.no_exception())
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
    constexpr static this_type min() noexcept {
        return this_type(std::numeric_limits<R>::min());
    }
    constexpr static this_type max() noexcept {
        return this_type(std::numeric_limits<R>::max());
    }
};

} // std

#endif  // BOOST_NUMERIC_CHECKED_CHECKED_RESULT
