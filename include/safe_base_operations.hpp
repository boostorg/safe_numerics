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
#include <type_traits> // is_base_of, is_same, enable_if
#include <ostream>
#include <istream>

#include <boost/config.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/identity.hpp>

#include <boost/utility/enable_if.hpp>

#include "safe_base.hpp"
#include "safe_integer.hpp"
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
SAFE_NUMERIC_CONSTEXPR inline operator+(const T & t, const U & u){
    // argument dependent lookup should guarentee that we only get here
    // only if one of the types is a safe type. Verify this here
    typedef addition_result<T, U> ar;
    typedef typename ar::P::exception_policy exception_policy;
    typedef typename ar::type result_type;
    static_assert(
        boost::numeric::is_safe<result_type>::value,
        "Promotion failed to return safe type"
    );

    typedef typename base_type<result_type>::type result_base_type;
    typedef typename base_type<T>::type t_base_type;
    typedef typename base_type<U>::type u_base_type;

    // filter out case were overflow cannot occur
    // note: subtle trickery.  Suppose t is safe_range<MIN, ..>.  Then
    // std::numeric_limits<T>::min() will be safe_range<MIN with a value of MIN
    // Use base_value(T) ( which equals MIN ) to create a new interval. Same
    // for MAX.  Now
    SAFE_NUMERIC_CONSTEXPR const interval<t_base_type> t_interval(
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        );

    SAFE_NUMERIC_CONSTEXPR const interval<u_base_type> u_interval = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

    // when we add the temporary intervals above, we'll get a new interval
    // with the correct range for the sum !
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

    r.template dispatch<exception_policy>();

    return r;
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
SAFE_NUMERIC_CONSTEXPR operator-(const T & t, const U & u){
    // argument dependent lookup should guarentee that we only get here
    // only if one of the types is a safe type. Verify this here
    typedef subtraction_result<T, U> sr;
    typedef typename sr::P::exception_policy exception_policy;
    typedef typename sr::type result_type;
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

    r.template dispatch<exception_policy>();
    
    return result_type(r);
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
SAFE_NUMERIC_CONSTEXPR operator*(const T & t, const U & u){
    // argument dependent lookup should guarentee that we only get here
    // only if one of the types is a safe type. Verify this here
    typedef multiplication_result<T, U> mr;
    typedef typename mr::P::exception_policy exception_policy;
    typedef typename mr::type result_type;
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

    // when we multiply the temporary intervals above, we'll get a new interval
    // with the correct range for the  product!
    SAFE_NUMERIC_CONSTEXPR const interval<result_base_type> r_interval
        = operator*<result_base_type>(t_interval, u_interval);

    // if no over/under flow possible
    if(r_interval.no_exception())
        return checked_result<result_base_type>(base_value(t) * base_value(u));

    // otherwise do the multiplication checking for overflow
    checked_result<result_base_type>  r = checked::multiply(
        base_value(std::numeric_limits<result_type>::min()),
        base_value(std::numeric_limits<result_type>::max()),
        base_value(t),
        base_value(u)
    );

    r.template dispatch<exception_policy>();

    return r;
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
    typedef division_result<T, U> dr;
    typedef typename dr::P::exception_policy exception_policy;
    typedef typename dr::type result_type;
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
        return checked_result<result_base_type>(base_value(t) / base_value(u));

    // otherwise do the division checking for overflow
    checked_result<result_base_type>  r = checked::divide(
        base_value(std::numeric_limits<result_type>::min()),
        base_value(std::numeric_limits<result_type>::max()),
        base_value(t),
        base_value(u)
    );

    r.template dispatch<exception_policy>();

    return r;
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
    typedef modulus_result<T, U> mr;
    typedef typename mr::P::exception_policy exception_policy;
    typedef typename mr::type result_type;
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
        return checked_result<result_base_type>(base_value(t) % base_value(u));

    // otherwise do the modulus checking for overflow
    checked_result<result_base_type>  r = checked::modulus(
        base_value(std::numeric_limits<result_type>::min()),
        base_value(std::numeric_limits<result_type>::max()),
        base_value(t),
        base_value(u)
    );

    r.template dispatch<exception_policy>();

    return r;
}

/////////////////////////////////////////////////////////////////
// comparison

// note: not really correct.  Should subject both t and u to type
// promotion to R.  Then interval::operator< can be simpler
// as can checked::less_than.  This is a relic of the previous system
// still to do.
template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    boost::mpl::identity<bool>
>::type
SAFE_NUMERIC_CONSTEXPR operator<(const T & lhs, const U & rhs) {
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

    SAFE_NUMERIC_CONSTEXPR const boost::logic::tribool r =
        t_interval < u_interval;

    return
        (r != boost::logic::tribool::indeterminate_value) ?
            r
        :
            checked::less_than(base_value(lhs), base_value(rhs));
        ;
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

/////////////////////////////////////////////////////////////////
// shift operators

template<class T, class U>
struct left_shift_result {
    typedef common_policies<T, U> P;
    typedef typename P::promotion_policy::template left_shift_result<
        T,
        U,
        typename P::promotion_policy,
        typename P::exception_policy
    >::type type;
};

template<class T, class U>
typename boost::lazy_enable_if_c<
    // handle safe<T> << int, int << safe<U>, safe<T> << safe<U>
    // exclude std::ostream << ...
    (! std::is_base_of<std::ios_base, T>::value)
    && (
        boost::numeric::is_safe<T>::value
        ||boost::numeric::is_safe<U>::value
    ),
    left_shift_result<T, U>
>::type
SAFE_NUMERIC_CONSTEXPR inline operator<<(const T & t, const U & u){
    // INT13-CPP
    typedef left_shift_result<T, U> lsr;
    typedef typename lsr::P::exception_policy exception_policy;
    typedef typename lsr::type result_type;
    typedef typename base_type<result_type>::type result_base_type;
    static_assert(
        boost::numeric::is_safe<result_type>::value,
        "Promotion failed to return safe type"
    );

    // note: it's possible to trap some sitations at compile and skip
    // the error checking.  Maybe later - for now check all the time.

     const checked_result<result_base_type> r = {
        checked::left_shift<result_base_type>(base_value(t), base_value(u))
    };

    r.template dispatch<exception_policy>();
    return r;
}

template<class T, class U>
struct right_shift_result {
    typedef common_policies<T, U> P;
    typedef typename P::promotion_policy::template right_shift_result<
        T,
        U,
        typename P::promotion_policy,
        typename P::exception_policy
    >::type type;
};

template<class T, class U>
typename boost::lazy_enable_if_c<
    // handle safe<T> << int, int << safe<U>, safe<T> << safe<U>
    // exclude std::ostream << ...
    (! std::is_base_of<std::ios_base, T>::value)
    && (
        boost::numeric::is_safe<T>::value
        ||boost::numeric::is_safe<U>::value
    ),
    right_shift_result<T, U>
>::type
SAFE_NUMERIC_CONSTEXPR inline operator>>(const T & t, const U & u){
    // INT13-CPP
    typedef right_shift_result<T, U> lsr;
    typedef typename lsr::P::exception_policy exception_policy;
    typedef typename lsr::type result_type;
    typedef typename base_type<result_type>::type result_base_type;
    static_assert(
        boost::numeric::is_safe<result_type>::value,
        "Promotion failed to return safe type"
    );

    // note: it's possible to trap some sitations at compile and skip
    // the error checking.  May later - for now check all the time.

    const checked_result<result_base_type> r = {
        checked::right_shift<result_base_type>(base_value(t), base_value(u))
    };
    r.template dispatch<exception_policy>();
    return r;
}

/////////////////////////////////////////////////////////////////
// bitwise operators

// operator |
template<class T, class U>
struct or_result {
    typedef common_policies<T, U> P;
    typedef typename P::promotion_policy::template or_result<
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
    or_result<T, U>
>::type
SAFE_NUMERIC_CONSTEXPR inline operator|(const T & t, const U & u){
    // argument dependent lookup should guarentee that we only get here
    // only if one of the types is a safe type. Verify this here
    typedef or_result<T, U> or_;
    typedef typename or_::P::exception_policy exception_policy;
    typedef typename or_::type result_type;
    static_assert(
        boost::numeric::is_safe<result_type>::value,
        "Promotion failed to return safe type"
    );

    static_assert(
        std::numeric_limits<T>::is_integer
        && (! std::numeric_limits<T>::is_signed)
        && std::numeric_limits<U>::is_integer
        && (! std::numeric_limits<U>::is_signed),
        // INT13-C
        "bitwise operations are only applicable to unsigned integers"
    );

    typedef typename base_type<result_type>::type result_base_type;

    static_assert(
        std::numeric_limits<result_base_type>::is_integer
        && (! std::numeric_limits<result_base_type>::is_signed),
        // INT13-C
        "bitwise operations should return unsigned integers"
    );

    checked_result<result_base_type> r = checked::bitwise_or<result_base_type>(t, u);
    r.template dispatch<exception_policy>();
    return r;
}

// operator &
template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    or_result<T, U>
>::type
SAFE_NUMERIC_CONSTEXPR inline operator&(const T & t, const U & u){
    // argument dependent lookup should guarentee that we only get here
    // only if one of the types is a safe type. Verify this here
    typedef or_result<T, U> and_;
    typedef typename and_::P::exception_policy exception_policy;
    typedef typename and_::type result_type;
    static_assert(
        boost::numeric::is_safe<result_type>::value,
        "Promotion failed to return safe type"
    );

    static_assert(
        std::numeric_limits<T>::is_integer
        && (! std::numeric_limits<T>::is_signed)
        && std::numeric_limits<U>::is_integer
        && (! std::numeric_limits<U>::is_signed),
        // INT13-C
        "bitwise operations are only applicable to unsigned integers"
    );

    typedef typename base_type<result_type>::type result_base_type;

    static_assert(
        std::numeric_limits<result_base_type>::is_integer
        && (! std::numeric_limits<result_base_type>::is_signed),
        // INT13-C
        "bitwise operations should return unsigned integers"
    );

    checked_result<result_base_type> r = checked::bitwise_and<result_base_type>(t, u);
    r.template dispatch<exception_policy>();
    return r;
}

// operator ^
template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    or_result<T, U>
>::type
SAFE_NUMERIC_CONSTEXPR inline operator^(const T & t, const U & u){
    // argument dependent lookup should guarentee that we only get here
    // only if one of the types is a safe type. Verify this here
    typedef or_result<T, U> xor_;
    typedef typename xor_::P::exception_policy exception_policy;
    typedef typename xor_::type result_type;
    static_assert(
        boost::numeric::is_safe<result_type>::value,
        "Promotion failed to return safe type"
    );

    static_assert(
        std::numeric_limits<T>::is_integer
        && (! std::numeric_limits<T>::is_signed)
        && std::numeric_limits<U>::is_integer
        && (! std::numeric_limits<U>::is_signed),
        // INT13-C
        "bitwise operations are only applicable to unsigned integers"
    );

    typedef typename base_type<result_type>::type result_base_type;

    static_assert(
        std::numeric_limits<result_base_type>::is_integer
        && (! std::numeric_limits<result_base_type>::is_signed),
        // INT13-C
        "bitwise operations should return unsigned integers"
    );

    checked_result<result_base_type> r = checked::bitwise_xor<result_base_type>(t, u);
    r.template dispatch<exception_policy>();
    return r;
}

/////////////////////////////////////////////////////////////////
// stream operators

template<
    class T,
    T Min,
    T Max,
    class P, // promotion polic
    class E  // exception policy
>
std::ostream & operator<<(
    std::ostream & os,
    const boost::numeric::safe_base<T, Min, Max, P, E> & t
){
    return os << t.m_t;
}

template<
    class T,
    T Min,
    T Max,
    class P, // promotion polic
    class E  // exception policy
>
std::istream & operator>>(
    std::istream & is,
    boost::numeric::safe_base<T, Min, Max, P, E> & t
){
    T tx;
    is >> tx;
    if(is.fail() || !t.validate(tx))
        E::range_error("error in file input");
    t.m_t = tx;
    return is;
}

} // numeric
} // boost

#endif // BOOST_NUMERIC_SAFE_BASE_OPERATIONS_HPP
