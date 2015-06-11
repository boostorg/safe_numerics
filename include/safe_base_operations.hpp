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

#include "safe_base.hpp"
#include "checked.hpp"
#include "checked_result.hpp"

#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/utility/enable_if.hpp>

#include <boost/static_assert.hpp>

namespace boost {
namespace numeric {

namespace detail {
    template<class T>
    struct safe_type {
        static_assert(
            is_safe<T>::value,
            "Should be safe type here!"
        );
        static SAFE_NUMERIC_CONSTEXPR const typename base_type<T>::type & get_stored_value(const T & t){
            return t.get_stored_value();
        }
    };
    template<class T>
    struct other_type {
        static SAFE_NUMERIC_CONSTEXPR const typename base_type<T>::type & get_stored_value(const T & t){
            return t;
        }
    };
} // detail

template<class T>
SAFE_NUMERIC_CONSTEXPR const typename base_type<T>::type & base_value(const T &  t) {
    typedef typename boost::mpl::if_<
        is_safe<T>,
        detail::safe_type<T>,
        detail::other_type<T>
    >::type invoke_operator;
    return invoke_operator::get_stored_value(t);
}

template<class T>
SAFE_NUMERIC_CONSTEXPR const typename base_type<T>::type & base_value(T && t) {
    typedef typename boost::mpl::if_<
        is_safe<T>,
        detail::safe_type<T>,
        detail::other_type<T>
    >::type invoke_operator;
    return invoke_operator::get_stored_value(t);
}

template<class T, class U>
struct common_policies {
    static_assert(
        boost::mpl::or_<
            is_safe<T>,
            is_safe<U>
        >::value,
        "at least one type must be a safe type"
    );

    // if both types are safe, the policies have to be the same!
    static_assert(
        boost::mpl::if_<
            boost::mpl::and_<
                is_safe<T>,
                is_safe<U>
            >,
            boost::mpl::and_<
                std::is_same<
                    typename boost::mpl::eval_if<
                        is_safe<T>,
                        get_promotion_policy<T>,
                        boost::mpl::identity<void>
                    >::type,
                    typename boost::mpl::eval_if<
                        is_safe<U>,
                        get_promotion_policy<U>,
                        boost::mpl::identity<void>
                    >::type
                >,
                std::is_same<
                    typename boost::mpl::eval_if<
                        is_safe<T>,
                        get_exception_policy<T>,
                        boost::mpl::identity<void>
                    >::type,
                    typename boost::mpl::eval_if<
                        is_safe<U>,
                        get_exception_policy<U>,
                        boost::mpl::identity<void>
                    >::type
                >
            >,
            boost::mpl::true_
        >::type::value,
        "if both types are safe, the policies have to be the same!"
    );

    // note: first attempt at the above fails - but I still can't figure out
    // why!!! I would think that the eval_if would inhibit evalution of it's
    // arguments which precipitates static assert inside of get_promotion_policy
    // if both types are safe, the policies have to be the same!
    /*
    static_assert(
        boost::mpl::eval_if<
            boost::mpl::and_<
                is_safe<T>,
                is_safe<U>
            >,
            std::is_same<
                typename get_promotion_policy<T>::type,
                typename get_promotion_policy<U>::type
            >,
            boost::mpl::identity<boost::mpl::true_>
        >::type::value,
        "if both types are safe, the policies have to be the same!"
    );
    */

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

    // filter out case were overflow cannot occur

    SAFE_NUMERIC_CONSTEXPR checked_result<result_base_type> maxx = checked::add(
        base_value(std::numeric_limits<result_type>::min()),
        base_value(std::numeric_limits<result_type>::max()),
        base_value(std::numeric_limits<T>::max()),
        base_value(std::numeric_limits<U>::max())
    );

    SAFE_NUMERIC_CONSTEXPR checked_result<result_base_type> minx = checked::add(
        base_value(std::numeric_limits<result_type>::min()),
        base_value(std::numeric_limits<result_type>::max()),
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<U>::min())
    );

    typedef typename checked_result<result_base_type>::exception_type exception_type;

    // if no over/under flow possible
    if(maxx == exception_type::no_exception
    && minx == exception_type::no_exception)
        // just do simple addition of base values
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

    // filter out case were overflow cannot occur

    SAFE_NUMERIC_CONSTEXPR checked_result<result_base_type> maxx = checked::subtract(
        base_value(std::numeric_limits<result_type>::min()),
        base_value(std::numeric_limits<result_type>::max()),
        base_value(std::numeric_limits<T>::max()),
        base_value(std::numeric_limits<U>::min())
    );

    SAFE_NUMERIC_CONSTEXPR checked_result<result_base_type> minx = checked::subtract(
        base_value(std::numeric_limits<result_type>::min()),
        base_value(std::numeric_limits<result_type>::max()),
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<U>::max())
    );

    typedef typename checked_result<result_base_type>::exception_type exception_type;

    // if no over/under flow possible
    if(maxx == exception_type::no_exception
    && minx == exception_type::no_exception)
        // just do simple subtraction of base values
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


/*
/////////////////////////////////////////////////////////////////
// subtraction
template<class T, class U>
struct subtraction_result {
    typedef typename get_common_policies<T, U>::type P;
    typedef typename get_promotion_policy<P>::type promotion_policy;
    typedef typename get_exception_policy<P>::type exception_policy;
    typedef typename promotion_policy::template subtraction_result<T, U, P>::type type;
};

template<class T, class U>
typename boost::enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    typename subtraction_result<T, U>::type
>::type
inline SAFE_NUMERIC_CONSTEXPR operator-(const T & t, const U & u){
    // argument dependent lookup should guarentee that we only get here
    // only if one of the types is a safe type. Verify this here
    static_assert(
        boost::mpl::or_<
            boost::numeric::is_safe<T>,
            boost::numeric::is_safe<U>
        >::value,
        "Neither type is a safe type"
    );
    typedef subtraction_result<T, U> sr;
    typedef typename sr::type result_type;
    typedef typename base_type<result_type>::type result_base_type;
    typedef typename sr::exception_policy exception_policy;
    return
        checked::subtract<result_base_type, exception_policy >(
            static_cast<const typename base_type<T>::type &>(t),
            static_cast<const typename base_type<U>::type &>(u)
        );
}
// comparison operators
template<class T, class Stored, class Derived, class Policies>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
operator<(const T & lhs, const safe_base<Stored, Derived, Policies> & rhs) {
    return rhs > lhs;
}
template<class T, class Stored, class Derived, class Policies>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
inline operator>(const T & lhs, const safe_base<Stored, Derived, Policies> & rhs) {
    return rhs < lhs;
}
template<class T, class Stored, class Derived, class Policies>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
inline operator==(const T & lhs, const safe_base<Stored, Derived, Policies> & rhs) {
    return rhs == lhs;
}
template<class T, class Stored, class Derived, class Policies>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
inline operator!=(const T & lhs, const safe_base<Stored, Derived, Policies> & rhs) {
    return rhs != rhs;
}
template<class T, class Stored, class Derived, class Policies>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
inline operator>=(const T & lhs, const safe_base<Stored, Derived, Policies> & rhs) {
    return rhs <= lhs;
}
template<class T, class Stored, class Derived, class Policies>
typename boost::enable_if<
    boost::is_integral<T>,
    bool
>::type
inline operator<=(const T & lhs, const safe_base<Stored, Derived, Policies> & rhs) {
    return  rhs >= lhs;
}

// multiplication
template<class T, class Stored, class Derived, class Policies>
typename boost::enable_if<
    boost::is_integral<T>,
    decltype(T() * Stored())
>::type
inline operator*(const T & lhs, const safe_base<Stored, Derived, Policies> & rhs) {
    return rhs * lhs;
}

// division
// special case - possible overflow
template<class T, class Stored, class Derived, class Policies>
typename boost::enable_if<
    boost::is_integral<T>,
    decltype(T() / Stored())
>::type
inline operator/(const T & lhs, const safe_base<Stored, Derived, Policies> & rhs) {
    if(safe_compare::equal(0, rhs))
        throw std::domain_error("Divide by zero");
    return static_cast<
        decltype(T() / Stored())
    >(lhs / static_cast<const Stored &>(rhs));
}

// modulus
template<class T, class Stored, class Derived, class Policies>
typename boost::enable_if<
    boost::is_integral<T>,
    decltype(T() % Stored())
>::type
inline operator%(const T & lhs, const safe_base<Stored, Derived, Policies> & rhs) {
    if(safe_compare::equal(0, rhs))
        throw std::domain_error("Divide by zero");
    return static_cast<
        decltype(T() % Stored())
    >(lhs % static_cast<const Stored &>(rhs));
}

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
    typedef typename get_exception_policy<T>::type exception_policy;
    is >> t.get_stored_value();
    if(std::cin.fail() || !t.validate())
        exception_policy::range_error("error in file input");
    return is;
}
} // numeric
} // boost

#endif // BOOST_NUMERIC_SAFE_BASE_OPERATIONS_HPP
