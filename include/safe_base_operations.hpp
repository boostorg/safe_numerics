#ifndef BOOST_NUMERIC_SAFE_BASE_OPERATIONS_HPP
#define BOOST_NUMERIC_SAFE_BASE_OPERATIONS_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "safe_base.hpp"
#include "policies.hpp"

#include <boost/type_traits/is_convertible.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/utility/enable_if.hpp>
//#include <boost/mpl/print.hpp>

namespace boost {
namespace numeric {

template<class T>
struct get_policies {
    BOOST_STATIC_ASSERT((boost::numeric::is_safe<T>::value));
    typedef typename T::Policies type;
};

template<class T, class U>
struct get_common_policies {
    // verify that at least one is a safe type
    BOOST_STATIC_ASSERT((
        boost::mpl::or_<
            is_safe<T>,
            is_safe<U>
        >::type::value
    ));

    typedef typename boost::mpl::eval_if<
        is_safe<T>,
        get_policies<T>,
        boost::mpl::identity<boost::mpl::void_>
    >::type::type policies_t;

    // typedef typename boost::mpl::print<policies_t>::type t0;

    typedef typename boost::mpl::eval_if<
        is_safe<U>,
        get_policies<U>,
        boost::mpl::identity<boost::mpl::void_>
    >::type::type policies_u;

    // typedef typename boost::mpl::print<policies_u>::type t1;

    // if both types are safe, the policies have to be the same!
    BOOST_STATIC_ASSERT((
        boost::mpl::if_<
            boost::mpl::and_<
                is_safe<T>,
                is_safe<U>
            >,
            typename boost::is_same<policies_t, policies_u>,
            boost::mpl::true_
        >::type::value
    ));

    // now we've verified that there is no conflict between policies
    // return the one from the first safe type
    typedef typename boost::mpl::if_<
        is_safe<T>,
        policies_t,
    typename boost::mpl::if_<
        is_safe<U>,
        policies_u,
    boost::mpl::void_
    >::type >::type type;

    // quadriple check all of the above
    BOOST_STATIC_ASSERT((
        boost::mpl::not_<boost::is_same<boost::mpl::void_, type> >::value
    ));

    // typedef typename boost::mpl::print<type>::type t2;
};

/////////////////////////////////////////////////////////////////
// Note: the following global operators will be only found via 
// argument dependent lookup.  So they won't conflict any
// other global operators for types in namespaces other than
// boost::numeric

// These should catch things like U < safe_base<...> and implement them
// as safe_base<...> >= U which should be handled above.

template<class T, class U>
struct addition_result {
    typedef typename get_common_policies<T, U>::type P;
    typedef typename get_promotion_policy<P>::type promotion_policy;
    typedef typename promotion_policy::template addition_result<T, U, P>::type type;
};

template<class T, class U>
typename boost::enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    typename addition_result<T, U>::type
>::type
inline constexpr operator+(const T & t, const U & u){
    // argument dependent lookup should guarentee that we only get here
    // if one of the types is a safe type. Verify this here
    BOOST_STATIC_ASSERT((boost::mpl::or_<
            boost::numeric::is_safe<T>,
            boost::numeric::is_safe<U>
        >::value
    ));
    typedef typename addition_result<T, U>::type result_type;
    typedef typename base_type<result_type>::type result_base_type;

    return
        checked::add<result_base_type >(
            static_cast<const typename base_type<T>::type &>(t),
            static_cast<const typename base_type<U>::type &>(u)
        );
}

/////////////////////////////////////////////////////////////////
// binary operators

// comparison operators
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
operator<(const T & lhs, const safe_base<Stored, Derived> & rhs) {
    return rhs > lhs;
}
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
inline operator>(const T & lhs, const safe_base<Stored, Derived> & rhs) {
    return rhs < lhs;
}
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
inline operator==(const T & lhs, const safe_base<Stored, Derived> & rhs) {
    return rhs == lhs;
}
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
inline operator!=(const T & lhs, const safe_base<Stored, Derived> & rhs) {
    return rhs != rhs;
}
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
inline operator>=(const T & lhs, const safe_base<Stored, Derived> & rhs) {
    return rhs <= lhs;
}
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
inline operator<=(const T & lhs, const safe_base<Stored, Derived> & rhs) {
    return  rhs >= lhs;
}

#if 0
// addition
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>, // native integer type
    //BOOST_TYPEOF_TPL(T() + Stored())
    typename PromotionPolicy::template addition_result<T, Stored>::type
>::type
inline operator+(
    const T & lhs,
    const safe_base<Stored, Derived> & rhs
){
    return rhs + lhs;
}
#endif

// subtraction
template<class T, class Stored, class Derived, class PromotionPolicy>
typename boost::enable_if<
    boost::is_integral<T>,  // native integer type
    BOOST_TYPEOF_TPL(T() - Stored())
>::type
inline operator-(const T & lhs, const safe_base<Stored, Derived> & rhs) {
    BOOST_TYPEOF_TPL(T() - Stored()) tmp = rhs - lhs;
    return - tmp;
}

// multiplication
template<class T, class Stored, class Derived, class PromotionPolicy>
typename boost::enable_if<
    boost::is_integral<T>,
    BOOST_TYPEOF_TPL(T() * Stored())
>::type
inline operator*(const T & lhs, const safe_base<Stored, Derived> & rhs) {
    return rhs * lhs;
}

// division
// special case - possible overflow
template<class T, class Stored, class Derived, class PromotionPolicy>
typename boost::enable_if<
    boost::is_integral<T>,
    BOOST_TYPEOF_TPL(T() / Stored())
>::type
inline operator/(const T & lhs, const safe_base<Stored, Derived> & rhs) {
    if(safe_compare::equal(0, rhs))
        throw std::domain_error("Divide by zero");
    return static_cast<
        BOOST_TYPEOF_TPL(T() / Stored())
    >(lhs / static_cast<const Stored &>(rhs));
}

// modulus
template<class T, class Stored, class Derived, class PromotionPolicy>
typename boost::enable_if<
    boost::is_integral<T>,
    BOOST_TYPEOF_TPL(T() % Stored())
>::type
inline operator%(const T & lhs, const safe_base<Stored, Derived> & rhs) {
    if(safe_compare::equal(0, rhs))
        throw std::domain_error("Divide by zero");
    return static_cast<
        BOOST_TYPEOF_TPL(T() % Stored())
    >(lhs % static_cast<const Stored &>(rhs));
}

// logical operators
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    typename multiply_result_type<T, Stored>::type
>::type
inline operator|(const T & lhs, const safe_base<Stored, Derived> & rhs) {
    return rhs | lhs;
}
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    BOOST_TYPEOF_TPL(T() & Stored())
>::type
inline operator&(const T & lhs, const safe_base<Stored, Derived> & rhs) {
    return rhs & lhs;
}
template<class T, class Stored, class Derived>
typename boost::enable_if<
    boost::is_integral<T>,
    BOOST_TYPEOF_TPL(T() ^ Stored())
>::type
inline operator^(const T & lhs, const safe_base<Stored, Derived> & rhs) {
    return rhs ^ lhs;
}

} // numeric
} // boost

#endif // BOOST_NUMERIC_SAFE_BASE_OPERATIONS_HPP
