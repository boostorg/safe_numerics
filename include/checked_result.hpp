//
//  checked_result.hpp
//  SafeIntegers
//
//  Created by Robert Ramey on 2/26/15.
//
//

#ifndef BOOST_NUMERIC_CHECKED_CHECKED_RESULT
#define BOOST_NUMERIC_CHECKED_CHECKED_RESULT

#include "policies.hpp"

namespace boost {
namespace numeric {

// poor man's variant which supports constexpr
struct checked_result_base {
    enum class exception_type {
        no_exception,
        overflow_error,
        underflow_error,
        range_error
    };
};

template<typename R>
struct checked_result : public checked_result_base {
    exception_type m_e;
    union {
        R m_r;
        const char * m_msg;
    };
    // constructors
    constexpr explicit checked_result(const R & r) :
        m_e(exception_type::no_exception),
        m_r(r)
    {}
    constexpr explicit checked_result(exception_type e, const char * msg) :
        m_e(e),
        m_msg(msg)
    {}
    // copy constructor
    /*
    constexpr checked_result(checked_result && r) :
        m_e(r.m_e),
        m_r(r.m_r)
    {}
    constexpr checked_result(const checked_result & r) :
        m_e(r.m_e),
        m_r(r.m_r)
    {}
    */
    // accesors
    constexpr operator R() const {
        return m_r;
    }
    constexpr operator exception_type () const {
        return m_e;
    }
    constexpr operator const char *() const {
        return m_msg;
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

#endif  // BOOST_NUMERIC_CHECKED_CHECKED_RESULT
