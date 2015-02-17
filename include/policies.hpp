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
#include <type_traits>
#include <boost/config.hpp>

namespace boost {
namespace numeric {

// run time trap handlers

// this would emulate the normal C/C++ behavior of permitting overflows
// and the like.
struct ignore_exception {
    void operator()(const std::exception & e) const {}
};

// If an exceptional condition is detected at runtime throw the exception.
struct throw_exception {
    void operator()(const std::exception & e) const {
        throw e;
    }
};

// example - if you don't want exceptions thrown, used an exception
// policy like this one. Substitute you're own implementation to taste
template<class F>
struct no_exception_support {
    static F m_f;
    void operator()(const std::exception & e) {
        m_f();
    }
};

// use this policy to trap at compile time any operation which
// would otherwise trap at runtime.  Hence expressions such as i/j
// will trap at compile time unless j can be guarenteed to not be zero.
typedef void no_exception_permitted;

// policies structure
// note: if there's no exception policy then any operation which
// could result in a runtime exception will trap during compile
// time.  For example i = i / j will not be permited because we
// can't know at compiler time that j can never be zero

struct policies_tag {};
template<class P, class E = throw_exception>
struct policies : public policies_tag {
    typedef P t_p; // promotion policy
    typedef E t_e; // exception policy
};

template<class P>
struct is_policies : public
    std::is_convertible<P, boost::numeric::policies_tag>
{};

template<class P>
struct get_promotion_policy {
    static_assert(boost::numeric::is_policies<P>::value, "P is not a pair of policies");
    typedef typename P::t_p type;
};
template<class P>
struct get_error_policy {
    static_assert(boost::numeric::is_policies<P>::value, "P is not a pair of policies");
    typedef typename P::t_e type;
};

} // namespace numeric
} // namespace boost

#endif // BOOST_NUMERIC_POLICIES_HPP
