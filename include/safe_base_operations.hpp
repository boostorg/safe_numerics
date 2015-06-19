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

#include <limits>
#include <type_traits> // is_convertible, is_same, enable_if

#include <boost/config.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/or.hpp>
//#include <boost/mpl/print.hpp>

#include <boost/utility/enable_if.hpp>

#include "safe_base.hpp"
#include "checked.hpp"
#include "checked_result.hpp"
#include "interval.hpp"

namespace boost {
namespace numeric {

template<class T, class U>
struct common_policies {
    static_assert(
        boost::mpl::or_<
            is_safe<T>,
            is_safe<U>
        >::value,
        "at least one type must be a safe type"
    );

    static_assert(
        boost::mpl::or_<
            std::is_same<
                typename get_promotion_policy<T>::type,
                typename get_promotion_policy<U>::type
            >,
            std::is_same<
                typename get_promotion_policy<T>::type,
                void
            >,
            std::is_same<
                void,
                typename get_promotion_policy<U>::type
            >
        >::value,
        "if the promotion policies are different, one must be void!"
    );

    static_assert(
        boost::mpl::or_<
            std::is_same<
                typename get_exception_policy<T>::type,
                typename get_exception_policy<U>::type
            >,
            std::is_same<
                typename get_exception_policy<T>::type,
                void
            >,
            std::is_same<
                void,
                typename get_exception_policy<U>::type
            >
        >::value,
        "if the exception policies are different, one must be void!"
    );

    // now we've verified that there is no conflict between policies
    // return the one from the first safe type
    typedef typename boost::mpl::if_<
        is_safe<T>,
        T,
    typename boost::mpl::if_<
        is_safe<U>,
        U,
    //
        boost::mpl::void_
    >
    >::type safe_type;

    typedef typename get_promotion_policy<safe_type>::type promotion_policy;
    typedef typename get_exception_policy<safe_type>::type exception_policy;
};

/////////////////////////////////////////////////////////////////
// binary operators

// Note: the following global operators will be only found via
// argument dependent lookup.  So they won't conflict any
// other global operators for types in namespaces other than
// boost::numeric

// These should catch things like U < safe_base<...> and implement them
// as safe_base<...> >= U which should be handled above.

/////////////////////////////////////////////////////////////////
// cast

/*
template<class R, class U>
T safe_cast(const U & u) {

    typedef typename base_type<R>::type result_base_type;

    // filter out case were overflow cannot occur
    SAFE_NUMERIC_CONSTEXPR const interval<R> r_interval;
    SAFE_NUMERIC_CONSTEXPR const interval<U> u_interval(
        base_value<std::numeric_limits<U>::min()>,
        base_value<std::numeric_limits<U>::max()>
    );


                    = operator*<result_base_type>(
            interval<typename base_type<T>::type>(),
            interval<typename base_type<U>::type>()
        );


    if(std::numeric_limits<T>::is_unsigned)
        if(u < 0)
            overflow("casting alters value");
    if(safe_compare::greater_than(u, std::numeric_limits<T>::max()))
        overflow("safe range overflow");
    if(safe_compare::less_than(u, std::numeric_limits<T>::min()))
        overflow("safe range underflow");
    return static_cast<T>(u);
}
*/

/////////////////////////////////////////////////////////////////
// addition

template<class T, class U>
struct addition_result {
    typedef common_policies<T, U> P;
    typedef typename P::promotion_policy::template addition_result<
        T,
        U,
        typename P::promotion_policy,
        typename P::exception_policy
    >::type type;
};

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    addition_result<T, U>
>::type
inline operator+(const T & t, const U & u){
    // argument dependent lookup should guarentee that we only get here
    // only if one of the types is a safe type. Verify this here
    typedef addition_result<T, U> ar;
    typedef typename ar::type result_type;
    static_assert(
        boost::numeric::is_safe<result_type>::value,
        "Promotion failed to return safe type"
    );

    typedef typename base_type<result_type>::type result_base_type;
    typedef typename base_type<T>::type t_base_type;
    typedef typename base_type<U>::type u_base_type;

    // filter out case were overflow cannot occur
    // note: subtle trickery.  Suppose is safe_range<MIN, ..>.  Then
    // std::numeric_limits<T>::min() will be safe_range<MIN with a value of MIN
    // Use base_value(T) ( which equals MIN ) to create a new interval. Same
    // for MAX.  Now
    SAFE_NUMERIC_CONSTEXPR const interval<t_base_type> t_interval = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };
    SAFE_NUMERIC_CONSTEXPR const interval<u_base_type> u_interval = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

    // when we add the temporary intervals above, we'll get a new interva
    // with the correct range for the sum !  Same goes for all the operations
    // defined below.
    SAFE_NUMERIC_CONSTEXPR const interval<result_base_type> r_interval
        = operator+<result_base_type>(t_interval, u_interval);

    // if no over/under flow possible
    if(r_interval.no_exception())
        return result_type(base_value(t) + base_value(u));

    // otherwise do the addition checking for overflow
    checked_result<result_base_type> r = checked::add(
        base_value(std::numeric_limits<result_base_type>::min()),
        base_value(std::numeric_limits<result_base_type>::max()),
        base_value(t),
        base_value(u)
    );

    r.template dispatch<typename ar::P::exception_policy>();
    
    return static_cast<result_type>(r);
}

/////////////////////////////////////////////////////////////////
// subtraction
template<class T, class U>
struct subtraction_result {
    typedef common_policies<T, U> P;
    typedef typename P::promotion_policy::template subtraction_result<
        T,
        U,
        typename P::promotion_policy,
        typename P::exception_policy
    >::type type;
};

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    subtraction_result<T, U>
>::type
inline operator-(const T & t, const U & u){
    // argument dependent lookup should guarentee that we only get here
    // only if one of the types is a safe type. Verify this here
    typedef subtraction_result<T, U> ar;
    typedef typename ar::type result_type;
    static_assert(
        boost::numeric::is_safe<result_type>::value,
        "Promotion failed to return safe type"
    );

    typedef typename base_type<result_type>::type result_base_type;
    typedef typename base_type<T>::type t_base_type;
    typedef typename base_type<U>::type u_base_type;

    // filter out case were overflow cannot occur
    SAFE_NUMERIC_CONSTEXPR const interval<t_base_type> t_interval = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };
    SAFE_NUMERIC_CONSTEXPR const interval<u_base_type> u_interval = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

    SAFE_NUMERIC_CONSTEXPR const interval<result_base_type> r_interval
        = operator-<result_base_type>(t_interval, u_interval);

    // if no over/under flow possible
    if(r_interval.no_exception())
        return result_type(base_value(t) - base_value(u));

    // otherwise do the subtraction checking for overflow
    checked_result<result_base_type> r = checked::subtract(
        base_value(std::numeric_limits<result_base_type>::min()),
        base_value(std::numeric_limits<result_base_type>::max()),
        base_value(t),
        base_value(u)
    );

    r.template dispatch<typename ar::P::exception_policy>();
    
    return static_cast<result_type>(r);
}

/////////////////////////////////////////////////////////////////
// multiplication
template<class T, class U>
struct multiplication_result {
    typedef common_policies<T, U> P;
    typedef typename P::promotion_policy::template multiplication_result<
        T,
        U,
        typename P::promotion_policy,
        typename P::exception_policy
    >::type type;
};

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    multiplication_result<T, U>
>::type
inline operator*(const T & t, const U & u){
    // argument dependent lookup should guarentee that we only get here
    // only if one of the types is a safe type. Verify this here
    typedef multiplication_result<T, U> ar;
    typedef typename ar::type result_type;
    static_assert(
        boost::numeric::is_safe<result_type>::value,
        "Promotion failed to return safe type"
    );

    typedef typename base_type<result_type>::type result_base_type;
    typedef typename base_type<T>::type t_base_type;
    typedef typename base_type<U>::type u_base_type;

    // filter out case were overflow cannot occur
    SAFE_NUMERIC_CONSTEXPR const interval<t_base_type> t_interval = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };
    SAFE_NUMERIC_CONSTEXPR const interval<u_base_type> u_interval = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

    SAFE_NUMERIC_CONSTEXPR const interval<result_base_type> r_interval
        = operator*<result_base_type>(t_interval, u_interval);

    // if no over/under flow possible
    if(r_interval.no_exception())
        return result_type(base_value(t) * base_value(u));

    // otherwise do the multiplication checking for overflow
    checked_result<result_base_type>  r = checked::multiply(
        base_value(std::numeric_limits<result_type>::min()),
        base_value(std::numeric_limits<result_type>::max()),
        base_value(t),
        base_value(u)
    );

    r.template dispatch<typename ar::P::exception_policy>();
    
    return static_cast<result_type>(r);
}

/////////////////////////////////////////////////////////////////
// division

template<class T, class U>
struct division_result {
    typedef common_policies<T, U> P;
    typedef typename P::promotion_policy::template division_result<
        T,
        U,
        typename P::promotion_policy,
        typename P::exception_policy
    >::type type;
};

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    division_result<T, U>
>::type
inline operator/(const T & t, const U & u){
    // argument dependent lookup should guarentee that we only get here
    // only if one of the types is a safe type. Verify this here
    typedef division_result<T, U> ar;
    typedef typename ar::type result_type;
    static_assert(
        boost::numeric::is_safe<result_type>::value,
        "Promotion failed to return safe type"
    );

    typedef typename base_type<result_type>::type result_base_type;
    typedef typename base_type<T>::type t_base_type;
    typedef typename base_type<U>::type u_base_type;

    // filter out case were overflow cannot occur
    SAFE_NUMERIC_CONSTEXPR const interval<t_base_type> t_interval = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };
    SAFE_NUMERIC_CONSTEXPR const interval<u_base_type> u_interval = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

    SAFE_NUMERIC_CONSTEXPR const interval<result_base_type> r_interval
        = operator/<result_base_type>(t_interval, u_interval);

    // if no over/under flow possible
    if(r_interval.no_exception())
        return result_type(base_value(t) / base_value(u));

    // otherwise do the multiplication checking for overflow
    checked_result<result_base_type>  r = checked::divide(
        base_value(std::numeric_limits<result_type>::min()),
        base_value(std::numeric_limits<result_type>::max()),
        base_value(t),
        base_value(u)
    );

    r.template dispatch<typename ar::P::exception_policy>();
    
    return static_cast<result_type>(r);
}

/////////////////////////////////////////////////////////////////
// modulus

template<class T, class U>
struct modulus_result {
    typedef common_policies<T, U> P;
    typedef typename P::promotion_policy::template modulus_result<
        T,
        U,
        typename P::promotion_policy,
        typename P::exception_policy
    >::type type;
};

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    modulus_result<T, U>
>::type
inline operator%(const T & t, const U & u){
    // argument dependent lookup should guarentee that we only get here
    // only if one of the types is a safe type. Verify this here
    typedef modulus_result<T, U> ar;
    typedef typename ar::type result_type;
    static_assert(
        boost::numeric::is_safe<result_type>::value,
        "Promotion failed to return safe type"
    );

    typedef typename base_type<result_type>::type result_base_type;
    typedef typename base_type<T>::type t_base_type;
    typedef typename base_type<U>::type u_base_type;

    // filter out case were overflow cannot occur
    SAFE_NUMERIC_CONSTEXPR const interval<t_base_type> t_interval = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };
    SAFE_NUMERIC_CONSTEXPR const interval<u_base_type> u_interval = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

    SAFE_NUMERIC_CONSTEXPR const interval<result_base_type> r_interval
        = operator%<result_base_type>(t_interval, u_interval);

    // if no over/under flow possible
    if(r_interval.no_exception())
        return result_type(base_value(t) % base_value(u));

    // otherwise do the multiplication checking for overflow
    checked_result<result_base_type>  r = checked::modulus(
        base_value(std::numeric_limits<result_type>::min()),
        base_value(std::numeric_limits<result_type>::max()),
        base_value(t),
        base_value(u)
    );

    r.template dispatch<typename ar::P::exception_policy>();
    
    return static_cast<result_type>(r);
}

/////////////////////////////////////////////////////////////////
// comparison

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    boost::mpl::identity<bool>
>::type
SAFE_NUMERIC_CONSTEXPR operator<(const T & lhs, const U & rhs) {
    return checked::less_than(base_value(lhs), base_value(rhs));
}

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    boost::mpl::identity<bool>
>::type
SAFE_NUMERIC_CONSTEXPR operator>(const T & lhs, const U & rhs) {
    return checked::greater_than(base_value(lhs), base_value(rhs));
}

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    boost::mpl::identity<bool>
>::type
SAFE_NUMERIC_CONSTEXPR operator==(const T & lhs, const U & rhs) {
    return checked::equal(base_value(lhs), base_value(rhs));
}

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    boost::mpl::identity<bool>
>::type
SAFE_NUMERIC_CONSTEXPR operator!=(const T & lhs, const U & rhs) {
    return checked::not_equal(base_value(lhs), base_value(rhs));
}

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    boost::mpl::identity<bool>
>::type
SAFE_NUMERIC_CONSTEXPR operator>=(const T & lhs, const U & rhs) {
    return checked::greater_than_equal(base_value(lhs), base_value(rhs));
}

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    boost::mpl::identity<bool>
>::type
SAFE_NUMERIC_CONSTEXPR operator<=(const T & lhs, const U & rhs) {
    return checked::less_than_equal(base_value(lhs), base_value(rhs));
}

/*

// logical operators
template<class T, class Stored, class Derived, class Policies>
typename boost::enable_if<
    boost::is_integral<T>,
    typename multiply_result_type<T, Stored>::type
>::type
inline operator|(const T & lhs, const safe_base<Stored, Derived, Policies> & rhs) {
    return rhs | lhs;
}
template<class T, class Stored, class Derived, class Policies>
typename boost::enable_if<
    boost::is_integral<T>,
    decltype(T() & Stored())
>::type
inline operator&(const T & lhs, const safe_base<Stored, Derived, Policies> & rhs) {
    return rhs & lhs;
}
template<class T, class Stored, class Derived, class Policies>
typename boost::enable_if<
    boost::is_integral<T>,
    decltype(T() ^ Stored())
>::type
inline operator^(const T & lhs, const safe_base<Stored, Derived, Policies> & rhs) {
    return rhs ^ lhs;
}
*/

} // numeric
} // boost

template<class T>
typename std::enable_if<
    boost::numeric::is_safe<T>::value,
    std::ostream &
>::type
operator<<(
    std::ostream & os,
    const T & t
){
    return os << t.get_stored_value();
}

template<class T>
typename std::enable_if<
    boost::numeric::is_safe<T>::value,
    std::ostream &
>::type
operator>>(
    std::istream & is,
    T & t
){
    typedef typename boost::numeric::get_exception_policy<T>::type exception_policy;
    is >> t.get_stored_value();
    if(is.fail() || !t.validate())
        exception_policy::range_error("error in file input");
    return is;
}

#endif // BOOST_NUMERIC_SAFE_BASE_OPERATIONS_HPP
