#ifndef BOOST_NUMERIC_POLICIES_HPP
#define BOOST_NUMERIC_POLICIES_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <stdexcept>
#include <boost/config.hpp>
#include <boost/type_traits/is_convertible.hpp>

namespace boost {
namespace numeric {

// compile time trap handling.  Not that some static asserts are
// never disabled.
enum compiletime_trap {
    strict,     // static assert on all possible runtime overflows
    relaxed,    // overflows can occur at runtime and maybe trapped.
};

// run time trap handlers
struct ignore_exception {
    void operator()(const std::exception & e) const {}
};

struct throw_exception {
    void operator()(const std::exception & e) const {
        throw e;
    }
};

struct policies_tag {};
template<class P, enum compiletime_trap CT, class E>
struct policies : public policies_tag {
    typedef P t_p;
    typedef E t_e;
    const enum compiletime_trap m_ct = CT;
};

template<class T>
struct is_policies : public
    boost::is_convertible<T, boost::numeric::policies_tag>
{};

template<class T>
struct get_promotion_policy {
    BOOST_STATIC_ASSERT((boost::numeric::is_policies<T>::value));
    typedef typename T::t_p type;
};
template<class T>
struct get_error_policy {
    BOOST_STATIC_ASSERT((boost::numeric::is_policies<T>::value));
    typedef typename T::t_e type;
};
template<class T>
enum compiletime_trap get_compile_time_trap_policy() {
    BOOST_STATIC_ASSERT((boost::numeric::is_policies<T>::value));
    return T::m_ct;
};

} // namespace numeric
} // namespace boost

#endif // BOOST_NUMERIC_POLICIES_HPP
