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
template<typename R>
struct checked_result {
    enum class exception_type {
        no_exception,
        overflow_error,
        underflow_error,
        range_error
    } m_e;
    union {
        R m_r;
        const char * m_msg;
    };
    constexpr explicit checked_result(const R & r) :
        m_e(exception_type::no_exception),
        m_r(r)
    {}
    constexpr explicit checked_result(exception_type e, const char * msg) :
        m_e(e),
        m_msg(msg)
    {}
    constexpr checked_result(const checked_result && r) :
        m_e(r.m_e),
        m_r(r.m_r)
    {}
    constexpr checked_result(const checked_result & r) :
        m_e(r.m_e),
        m_r(r.m_r)
    {}
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
    dispatch(const EP & ep) const {
        BOOST_CONCEPT_ASSERT((ExceptionPolicy<EP>));
        switch(m_e){
        case exception_type::overflow_error:
            ep.overflow_error(m_msg);
            break;
        case checked_result<R>::exception_type::underflow_error:
            ep.underflow_error(m_msg);
            break;
        case checked_result<R>::exception_type::range_error:
            ep.range_error(m_msg);
            break;
        case checked_result<R>::exception_type::no_exception:
            break;
        }
    }
};

} // numeric
} // boost

#endif  // BOOST_NUMERIC_CHECKED_CHECKED_RESULT
