//
//  checked_result.hpp
//  SafeIntegers
//
//  Created by Robert Ramey on 2/26/15.
//
//

#ifndef BOOST_NUMERIC_CHECKED_RESULT
#define BOOST_NUMERIC_CHECKED_RESULT

#include <ostream>
#include "safe_base.hpp" // SAFE_NUMERIC_CONSTEXPR
#include "exception_policies.hpp"

namespace boost {
namespace numeric {

template<typename R>
struct checked_result {
// poor man's variant which supports SAFE_NUMERIC_CONSTEXPR
    enum class exception_type {
        no_exception,
        overflow_error,
        underflow_error,
        range_error
    };
    exception_type m_e;
    union {
        R m_r;
        const char * m_msg;
    };
    // constructors
    SAFE_NUMERIC_CONSTEXPR explicit checked_result(const R & r) :
        m_e(exception_type::no_exception),
        m_r(r)
    {}
    SAFE_NUMERIC_CONSTEXPR explicit checked_result(exception_type e, const char * msg) :
        m_e(e),
        m_msg(msg)
    {}
    // copy constructor
    /*
    SAFE_NUMERIC_CONSTEXPR checked_result(checked_result && r) :
        m_e(r.m_e),
        m_r(r.m_r)
    {}
    SAFE_NUMERIC_CONSTEXPR checked_result(const checked_result & r) :
        m_e(r.m_e),
        m_r(r.m_r)
    {}
    */
    // accesors
    SAFE_NUMERIC_CONSTEXPR operator R() const {
        return m_r;
    }
    SAFE_NUMERIC_CONSTEXPR operator exception_type() const {
        return m_e;
    }
    SAFE_NUMERIC_CONSTEXPR operator const char *() const {
        return m_msg;
    }
    SAFE_NUMERIC_CONSTEXPR bool operator==(const exception_type & et) const {
        return m_e == et;
    }
    SAFE_NUMERIC_CONSTEXPR bool operator!=(const exception_type & et) const {
        return m_e != et;
    }

    template<class EP>
    void
    dispatch() const {
        BOOST_CONCEPT_ASSERT((ExceptionPolicy<EP>));
        switch(m_e){
        case exception_type::overflow_error:
            EP::overflow_error(m_msg);
            break;
        case checked_result<R>::exception_type::underflow_error:
            EP::underflow_error(m_msg);
            break;
        case checked_result<R>::exception_type::range_error:
            EP::range_error(m_msg);
            break;
        case checked_result<R>::exception_type::no_exception:
            break;
        default:
            break;
        }
    }
};

} // numeric
} // boost

template<typename R>
std::ostream & operator<<(std::ostream & os, const boost::numeric::checked_result<R> & r){
    if(r == boost::numeric::checked_result<R>::exception_type::no_exception)
        os << static_cast<R>(r);
    else
        os << static_cast<const char *>(r);
    return os;
}


#endif  // BOOST_NUMERIC_CHECKED_CHECKED_RESULT
