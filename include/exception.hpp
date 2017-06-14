#ifndef BOOST_NUMERIC_EXCEPTION
#define BOOST_NUMERIC_EXCEPTION

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// contains error indicators for results of doing checked
// arithmetic on native C++ types

#include <algorithm>
#include <system_error> // error_code, system_error
#include <string>

// Using the system_error code facility.  This facility is more complex
// than meets the eye.  To fully understand what out intent here is,
// review http://blog.think-async.com/2010/04/system-error-support-in-c0x-part-5.html
// "Giving context-specific meaning to generic error codes"

namespace boost {
namespace numeric {

// errors codes for safe numerics

// in spite of the similarity, this list is distinct from the exceptions
// listed in documentation for std::exception.
enum class safe_numerics_error {
    success = 0,
    positive_overflow_error,
    negative_overflow_error,
    underflow_error,
    range_error,
    domain_error,
    implementation_defined_behavior,
    undefined_behavior,
    uninitialized_value
};

} // numeric
} // boost

namespace std {
    template <>
    struct is_error_code_enum<boost::numeric::safe_numerics_error>
        : public true_type {};
};

namespace boost {
namespace numeric {

const class : public std::error_category {
public:
    virtual const char* name() const noexcept{
        return "safe numerics error";
    }
    virtual std::string message(int ev) const {
        switch(static_cast<safe_numerics_error>(ev)){
        case safe_numerics_error::success:
            return "success";
        case safe_numerics_error::positive_overflow_error:
            return "positive overflow error";
        case safe_numerics_error::negative_overflow_error:
            return "negative overflow error";
        case safe_numerics_error::underflow_error:
            return "underflow error";
        case safe_numerics_error::range_error:
            return "range error";
        case safe_numerics_error::domain_error:
            return "domain error";
        case safe_numerics_error::implementation_defined_behavior:
            return "implementation defined behavior";
        case safe_numerics_error::undefined_behavior:
            return "undefined behavior";
        case safe_numerics_error::uninitialized_value:
            return "uninitialized value";
        default:
            assert(false);
        }
    }
} safe_numerics_error_category ;

// constexpr - damn, can't use constexpr due to std::error_code
std::error_code make_error_code(safe_numerics_error e){
    return std::error_code(static_cast<int>(e), safe_numerics_error_category);
}

// actions for error_codes for safe numerics.  I've leveraged on
// error_condition in order to do this.  I'm not sure this is a good
// idea or not.

enum class safe_numerics_actions {
    no_action = 0,
    uninitialized_value,
    arithmetic_error,
    implementation_defined_behavior,
    undefined_behavior
};

} // numeric
} // boost

namespace std {
    template <>
    struct is_error_condition_enum<boost::numeric::safe_numerics_actions>
        : public true_type {};
};

namespace boost {
namespace numeric {

const class : public std::error_category {
public:
    virtual const char* name() const noexcept {
        return "safe numerics error group";
    }
    virtual std::string message(int ev) const {
        return "safe numerics error group";
    }
    // return true if a given error code corresponds to a
    // given safe numeric action
    virtual bool equivalent(
        const std::error_code & code,
        int condition
    ) const noexcept {
        if(code.category() != safe_numerics_error_category)
            return false;
        switch (static_cast<safe_numerics_actions>(condition)){
        case safe_numerics_actions::no_action:
            return code == safe_numerics_error::success;
        case safe_numerics_actions::uninitialized_value:
            return code == safe_numerics_error::uninitialized_value;
        case safe_numerics_actions::arithmetic_error:
            return code == safe_numerics_error::positive_overflow_error
                || code == safe_numerics_error::negative_overflow_error
                || code == safe_numerics_error::underflow_error
                || code == safe_numerics_error::range_error
                || code == safe_numerics_error::domain_error;
        case safe_numerics_actions::implementation_defined_behavior:
            return code == safe_numerics_error::implementation_defined_behavior;
        case safe_numerics_actions::undefined_behavior:
            return code == safe_numerics_error::undefined_behavior;
        default:
            assert(false);
        }
    }
} safe_numerics_actions_category ;

std::error_condition
make_error_condition(safe_numerics_actions e){
    return std::error_condition(
        static_cast<int>(e),
        safe_numerics_actions_category
    );
}

// given an error code - return the action code which it corresponds to.
constexpr enum  safe_numerics_actions
make_safe_numerics_action(const safe_numerics_error & e){
    // we can't use standard algorithms since we want this to be constexpr
    // this brute force solution is simple and pretty fast anyway
    switch(e){
    case safe_numerics_error::negative_overflow_error:
    case safe_numerics_error::underflow_error:
    case safe_numerics_error::range_error:
    case safe_numerics_error::domain_error:
    case safe_numerics_error::positive_overflow_error:
        return safe_numerics_actions::arithmetic_error;
    case safe_numerics_error::undefined_behavior:
        return safe_numerics_actions::undefined_behavior;
    case safe_numerics_error::implementation_defined_behavior:
        return safe_numerics_actions::implementation_defined_behavior;
    case safe_numerics_error::uninitialized_value:
        return safe_numerics_actions::uninitialized_value;
    case safe_numerics_error::success:
        return safe_numerics_actions::no_action;
    default:
        assert(false);
    }
}

// given an error code - return the error_condition which it corresponds to.
std::error_condition
make_error_condition(safe_numerics_error e){
    return std::error_condition(make_safe_numerics_action(e));
}

} // numeric
} // boost

#endif // BOOST_NUMERIC_CHECKED_RESULT
