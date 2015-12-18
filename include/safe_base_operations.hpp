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

#include <boost/utility/enable_if.hpp> // lazy_enable_if

#include "safe_base.hpp"
#include "safe_literal.hpp"
#include "safe_compare.hpp"
#include "checked_result.hpp"
#include "interval.hpp"

namespace boost {
namespace numeric {

/////////////////////////////////////////////////////////////////
// validation

template<typename E>
struct validate_detail {
    struct exception_possible {
        template<typename Min, typename Max, typename T>
        constexpr static bool check_value(
            const Min & min,
            const Max & max,
            const T & t
        ){
            // INT08-C
            bool validated =
                ! safe_compare::less_than(
                    max,
                    base_value(t)
                )
                &&
                ! safe_compare::less_than(
                    base_value(t),
                    min
                );
            if(! validated)
                E::range_error("Value out of range for this safe type");
            return validated;
        }
        constexpr static void domain_error(){
            E::domain_error("Stored type cannot hold argument");
        }

    };
    struct exception_not_possible {
        template<typename Min, typename Max, typename T>
        constexpr static bool check_value(
            const Min & min,
            const Max & max,
            const T & t
        ){
            return true;
        }
        constexpr static void domain_error(){}
    };
};

template<class Stored, Stored Min, Stored Max, class P, class E>
template<class T>
constexpr bool safe_base<Stored, Min, Max, P, E>::
validate(const T & t) const {
    constexpr const interval<typename base_type<T>::type> t_interval(
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    );
    constexpr const interval<Stored> this_interval(Min, Max);
    // if static values don't overlap, the program can never function
    static_assert(
        indeterminate(t_interval < this_interval),
        "safe type cannot be constructed with this type"
    );

    return boost::mpl::if_c<
        this_interval.includes(t_interval),
        typename validate_detail<E>::exception_not_possible,
        typename validate_detail<E>::exception_possible
    >::type::check_value(Min, Max, t);
}

template<class Stored, Stored Min, Stored Max, class P, class E>
template<class T>
constexpr Stored safe_base<Stored, Min, Max, P, E>::
validated_cast(const T & t) const {
    if(validate(t)){
        checked_result<Stored> r = checked::cast<Stored>(base_value(t));
        if(!r.no_exception()){
            E::domain_error("Stored type cannot hold argument");
        }
    }
    return static_cast<Stored>(t);
}

template<class Stored, Stored Min, Stored Max, class P, class E>
template<std::intmax_t N>
constexpr Stored safe_base<Stored, Min, Max, P, E>::
validated_cast(const safe_literal<N> & t) const {
    if(validate(t)){
        constexpr checked_result<Stored> r = checked::cast<Stored>(N);
        boost::mpl::if_c<
            r.no_exception(),
            typename validate_detail<E>::exception_not_possible,
            typename validate_detail<E>::exception_possible
        >::type::domain_error();
    }
    return static_cast<Stored>(N);
}

/////////////////////////////////////////////////////////////////
// construction and assignment operators

// construction from some arbitrary type T
template<class Stored, Stored Min, Stored Max, class P, class E>
template<class T>
constexpr safe_base<Stored, Min, Max, P, E>::
safe_base(const T & rhs) :
    m_t(validated_cast(rhs))
{}

// assignment from some type T
template<class Stored, Stored Min, Stored Max, class P, class E>
template<class T, T MinT, T MaxT, class PT, class ET>
constexpr safe_base<Stored, Min, Max, P, E> &
safe_base<Stored, Min, Max, P, E>::
operator=(const safe_base<T, MinT, MaxT, PT, ET> & rhs){
    m_t = validated_cast(rhs);
    return *this;
}

/////////////////////////////////////////////////////////////////
// casting operators

template<typename R, typename E>
struct cast_detail {
    struct exception_possible {
        template<typename T>
        constexpr static R
        return_value(const T & t){
            checked_result<R> r = checked::cast<R>(t);
            if(! r.no_exception())
                E::range_error(r);
            return r;
        }
    };

    struct exception_not_possible {
        template<typename T>
        constexpr static R
        return_value(const T & t){
            return t;
        }
    };
};

template< class Stored, Stored Min, Stored Max, class P, class E>
template<
    class R,
    typename std::enable_if<
        !boost::numeric::is_safe<R>::value,
        int
    >::type
>
constexpr safe_base<Stored, Min, Max, P, E>::
operator R () const {
    constexpr interval<R> r_interval;
    constexpr interval<Stored> this_interval(Min, Max);

    return boost::mpl::if_c<
        r_interval.includes(this_interval),
        typename cast_detail<R, E>::exception_not_possible,
        typename cast_detail<R, E>::exception_possible
    >::type::return_value(m_t);
}

/////////////////////////////////////////////////////////////////
// binary operators

template<class T, class U>
struct common_policies {
    static_assert(is_safe<T>::value || is_safe<U>::value,
        "at least one type must be a safe type"
    );

    using t_promotion_policy = typename get_promotion_policy<T>::type;
    using u_promotion_policy = typename get_promotion_policy<U>::type;
    using t_exception_policy = typename get_exception_policy<T>::type;
    using u_exception_policy = typename get_exception_policy<U>::type;

    static_assert(
        std::is_same<t_promotion_policy, u_promotion_policy>::value
        ||std::is_same<t_promotion_policy, void>::value
        ||std::is_same<void, u_promotion_policy>::value,
        "if the promotion policies are different, one must be void!"
    );

    static_assert(
        ! std::is_same<t_promotion_policy, void>::value
        || !std::is_same<void, u_promotion_policy>::value,
        "at least one promotion polcy must be non void"
    );

    static_assert(
        ! (std::is_same<t_promotion_policy, void>::value
        && std::is_same<void, u_promotion_policy>::value),
        "at least one promotion policy must not be void"
    );

    static_assert(
        std::is_same<t_exception_policy, u_exception_policy>::value
        || std::is_same<t_exception_policy, void>::value
        || std::is_same<void, u_exception_policy>::value,
        "if the exception policies are different, one must be void!"
    );

    static_assert(
        ! (std::is_same<t_exception_policy, void>::value
        && std::is_same<void, u_exception_policy>::value),
        "at least one promotion policy must not be void"
    );

    using promotion_policy =
        typename boost::mpl::if_c<
            ! std::is_same<void, u_promotion_policy>::value,
            u_promotion_policy,
        typename boost::mpl::if_c<
            ! std::is_same<void, t_promotion_policy>::value,
            t_promotion_policy,
        //
            void
        >::type >::type;

    static_assert(
        ! std::is_same<void, promotion_policy>::value,
        "promotion_policy is void"
    );

    using exception_policy =
        typename boost::mpl::if_c<
            !std::is_same<void, u_exception_policy>::value,
            u_exception_policy,
        typename boost::mpl::if_c<
            !std::is_same<void, t_exception_policy>::value,
            t_exception_policy,
        //
            void
        >::type >::type;

    static_assert(
        !std::is_same<void, exception_policy>::value,
        "exception_policy is void"
    );
};

// Note: the following global operators will be only found via
// argument dependent lookup.  So they won't conflict any
// other global operators for types in namespaces other than
// boost::numeric

/////////////////////////////////////////////////////////////////
// addition

template<class T, class U>
struct addition_result {
    using P = common_policies<T, U>;

    using type = typename P::promotion_policy::template addition_result<
        T,
        U,
        typename P::promotion_policy,
        typename P::exception_policy
    >::type;
    using result_type = type;
    using result_base_type = typename base_type<result_type>::type;
    using exception_policy = typename P::exception_policy;

    static_assert(
        boost::numeric::is_safe<result_type>::value,
        "Promotion failed to return safe type"
    );

    struct exception_possible {
        static result_type
        constexpr return_value(const T & t, const U & u){
            checked_result<result_base_type> r = checked::add<result_base_type>(
                base_value(t),
                base_value(u)
            );
            dispatch<exception_policy>(r);
            return result_type(static_cast<result_base_type>(r));
        }
    };

    struct exception_not_possible {
        static result_type
        constexpr return_value(const T & t, const U & u){
            return result_type(
                static_cast<result_base_type>(base_value(t))
                + static_cast<result_base_type>(base_value(u))
            );
        }
    };
};

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    addition_result<T, U>
>::type
constexpr inline operator+(const T & t, const U & u){
    // argument dependent lookup should guarentee that we only get here
    using ar = addition_result<T, U>;
    using result_base_type = typename ar::result_base_type;
    using t_base_type = typename base_type<T>::type;
    using u_base_type = typename base_type<U>::type;

    // filter out case were overflow cannot occur
    // note: subtle trickery.  Suppose t is safe_range<MIN, ..>.  Then
    // std::numeric_limits<T>::min() will be safe_range<MIN with a value of MIN
    // Use base_value(T) ( which equals MIN ) to create a new interval. Same
    // for MAX.  Now
    constexpr const interval<t_base_type> t_interval(
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    );

    constexpr const interval<u_base_type> u_interval(
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    );

    // when we add the temporary intervals above, we'll get a new interval
    // with the correct range for the sum !
    constexpr const checked_result<interval<result_base_type>> r_interval
        = add<result_base_type>(t_interval, u_interval);

    return boost::mpl::if_c<
        r_interval.no_exception(),
        typename ar::exception_not_possible,
        typename ar::exception_possible
    >::type::return_value(t, u);
}

/////////////////////////////////////////////////////////////////
// subtraction
template<class T, class U>
struct subtraction_result {
    using P = common_policies<T, U>;
    using type = typename P::promotion_policy::template subtraction_result<
        T,
        U,
        typename P::promotion_policy,
        typename P::exception_policy
    >::type;
    using result_type = type;
    using result_base_type = typename base_type<result_type>::type;
    using exception_policy = typename P::exception_policy;

    static_assert(
        boost::numeric::is_safe<result_type>::value,
        "Promotion failed to return safe type"
    );

    struct exception_possible {
        static result_type
        constexpr return_value(const T & t, const U & u){
            checked_result<result_base_type> r = checked::subtract<result_base_type>(
                base_value(t),
                base_value(u)
            );
            dispatch<exception_policy>(r);
            return result_type(static_cast<result_base_type>(r));
        }
    };

    struct exception_not_possible {
        static result_type
        constexpr return_value(const T & t, const U & u){
            return static_cast<result_base_type>(result_type(
                static_cast<result_base_type>(base_value(t))
                - static_cast<result_base_type>(base_value(u))
            ));
        }
    };
};

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    subtraction_result<T, U>
>::type
constexpr operator-(const T & t, const U & u){
    using sr = subtraction_result<T, U>;
    using result_base_type = typename sr::result_base_type;

    using t_base_type = typename base_type<T>::type;
    using u_base_type = typename base_type<U>::type;

    // filter out case were overflow cannot occur
    constexpr const interval<t_base_type> t_interval(
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    );
    constexpr const interval<u_base_type> u_interval(
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    );

    constexpr const checked_result<interval<result_base_type>> r_interval
        = subtract<result_base_type>(t_interval, u_interval);

    return boost::mpl::if_c<
        r_interval.no_exception(),
        typename sr::exception_not_possible,
        typename sr::exception_possible
    >::type::return_value(t, u);
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
    using result_type = type;
    using result_base_type = typename base_type<result_type>::type;
    using exception_policy = typename P::exception_policy;

    static_assert(
        boost::numeric::is_safe<result_type>::value,
        "Promotion failed to return safe type"
    );

    struct exception_possible {
        static result_type
        constexpr return_value(const T & t, const U & u){
            checked_result<result_base_type> r = checked::multiply<result_base_type>(
                base_value(t),
                base_value(u)
            );
            dispatch<exception_policy>(r);
            return result_type(static_cast<result_base_type>(r));
        }
    };

    struct exception_not_possible {
        static result_type
        constexpr return_value(const T & t, const U & u){
            return result_type(
                static_cast<result_base_type>(base_value(t))
                * static_cast<result_base_type>(base_value(u))
            );
        }
    };
};

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    multiplication_result<T, U>
>::type
constexpr operator*(const T & t, const U & u){
    // argument dependent lookup should guarentee that we only get here
    using mr = multiplication_result<T, U>;
    using result_base_type = typename mr::result_base_type;
    using t_base_type = typename base_type<T>::type;
    using u_base_type = typename base_type<U>::type;

    constexpr const interval<t_base_type> t_interval(
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    );

    constexpr const interval<u_base_type> u_interval(
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    );

    // when we multiply the temporary intervals above, we'll get a new interval
    // with the correct range for the  product!
    constexpr const checked_result<interval<result_base_type>> r_interval
        = multiply<result_base_type>(t_interval, u_interval);

    return boost::mpl::if_c<
        r_interval.no_exception(),
        typename mr::exception_not_possible,
        typename mr::exception_possible
    >::type::return_value(t, u);
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
    using result_type = type;
    using result_base_type = typename base_type<result_type>::type;
    using exception_policy = typename P::exception_policy;
    using promotion_policy = typename P::promotion_policy;

    static_assert(
        boost::numeric::is_safe<result_type>::value,
        "Promotion failed to return safe type"
    );

    struct exception_possible {
        static result_type
        constexpr return_value(const T & t, const U & u){
            checked_result<result_base_type> r =
            promotion_policy::template divide<result_base_type>(
                base_value(t),
                base_value(u)
            );
            dispatch<exception_policy>(r);
            return result_type(static_cast<result_base_type>(r));
        }
    };

    struct exception_not_possible {
        static result_type
        constexpr return_value(const T & t, const U & u){
            return result_type(
                static_cast<result_base_type>(base_value(t))
                / static_cast<result_base_type>(base_value(u))
            );
        }
    };
};

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    division_result<T, U>
>::type
constexpr operator/(const T & t, const U & u){
    // argument dependent lookup should guarentee that we only get here
    using dr = division_result<T, U>;
    using result_base_type = typename dr::result_base_type;
    using t_base_type = typename base_type<T>::type;
    using u_base_type = typename base_type<U>::type;

    constexpr const interval<t_base_type> t_interval(
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    );

    constexpr const interval<u_base_type> u_interval(
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    );

    // when we divide the temporary intervals above, we'll get a new interval
    // with the correct range for the result!
    constexpr const checked_result<interval<result_base_type>> r_interval =
        divide<result_base_type>(t_interval, u_interval);

    return boost::mpl::if_c<
        // if no over/under flow or domain error possible
        r_interval.no_exception()
        // and if the denominator cannot contain zero
        && (u_interval.l <= 0 && u_interval.u >=0 ),
        typename dr::exception_not_possible,
        typename dr::exception_possible
    >::type::return_value(t, u);
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
    using result_type = type;
    using result_base_type = typename base_type<result_type>::type;
    using exception_policy = typename P::exception_policy;
    using promotion_policy = typename P::promotion_policy;

    static_assert(
        boost::numeric::is_safe<result_type>::value,
        "Promotion failed to return safe type"
    );

    struct exception_possible {
        static result_type
        constexpr return_value(const T & t, const U & u){
            using t_type = decltype(base_value(t) % base_value(u));
            checked_result<t_type>  r =
                promotion_policy::template modulus<t_type>(
                    base_value(t),
                    base_value(u)
                );

            dispatch<exception_policy>(r);
            return result_type(static_cast<result_base_type>(r));
        }
    };

    struct exception_not_possible {
        static result_type
        constexpr return_value(const T & t, const U & u){
            return result_type(
                static_cast<result_base_type>(base_value(t))
                % static_cast<result_base_type>(base_value(u))
            );
        }
    };
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
    using mr = modulus_result<T, U>;
    using result_base_type = typename mr::result_base_type;
    using u_base_type = typename base_type<U>::type;

    // filter out case were overflow cannot occur
    constexpr const interval<u_base_type> u_interval(
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    );

    return boost::mpl::if_c<
        // if no over/under flow or domain error possible
        ((u_interval.l > 0 || 0 > u_interval.u)
        // and if the denominator cannot contain zero
        && (u_interval.l <= 0 && u_interval.u >=0 )),
        typename mr::exception_not_possible,
        typename mr::exception_possible
    >::type::return_value(t, u);
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
constexpr operator<(const T & lhs, const U & rhs) {
    using t_base_type = typename base_type<T>::type;
    using u_base_type = typename base_type<U>::type;

    // filter out case were overflow cannot occur
    constexpr const interval<t_base_type> t_interval(
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    );

    constexpr const interval<u_base_type> u_interval(
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    );

    constexpr const boost::logic::tribool r =
        t_interval < u_interval;

    return
        // if the ranges don't overlap
        (! boost::logic::indeterminate(r)) ?
            // values in those ranges can't be equal
            false
        :
            // otherwise we have to check
            safe_compare::less_than(base_value(lhs), base_value(rhs));
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
constexpr operator>(const T & lhs, const U & rhs) {
    using t_base_type = typename base_type<T>::type;
    using u_base_type = typename base_type<U>::type;

    constexpr const interval<t_base_type> t_interval(
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    );

    constexpr const interval<u_base_type> u_interval(
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    );

    constexpr const boost::logic::tribool r =
        t_interval > u_interval;

    return
        // if the ranges don't overlap
        (! boost::logic::indeterminate(r)) ?
            // values in those ranges can't be equal
            false
        :
            // otherwise we have to check
            safe_compare::greater_than(base_value(lhs), base_value(rhs));
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
constexpr operator==(const T & lhs, const U & rhs) {
    using t_base_type = typename base_type<T>::type;
    using u_base_type = typename base_type<U>::type;

    constexpr const interval<t_base_type> t_interval(
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    );

    constexpr const interval<u_base_type> u_interval(
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    );

    return
        // if the ranges don't overlap
        ( t_interval < u_interval || t_interval > u_interval) ?
            // values in those ranges can't be equal
            false
        :
            // otherwise we have to check
            safe_compare::equal(base_value(lhs), base_value(rhs));
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
constexpr operator!=(const T & lhs, const U & rhs) {
    return ! (lhs == rhs);
}

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    boost::mpl::identity<bool>
>::type
constexpr operator>=(const T & lhs, const U & rhs) {
    return ! ( rhs < lhs );
}

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    boost::mpl::identity<bool>
>::type
constexpr operator<=(const T & lhs, const U & rhs) {
    return ! ( rhs > lhs );
}

/////////////////////////////////////////////////////////////////
// shift operators

// left shift
template<class T, class U>
struct left_shift_result {
    typedef common_policies<T, U> P;
    typedef typename P::promotion_policy::template left_shift_result<
        T,
        U,
        typename P::promotion_policy,
        typename P::exception_policy
    >::type type;
    using result_type = type;
    using result_base_type = typename base_type<result_type>::type;
    using exception_policy = typename P::exception_policy;

    static_assert(
        boost::numeric::is_safe<result_type>::value,
        "Promotion failed to return safe type"
    );
    struct exception_possible {
        static result_type
        constexpr return_value(const T & t, const U & u){
            const checked_result<result_base_type> r = checked::left_shift<result_base_type>(
                base_value(t),
                base_value(u)
            );
            dispatch<exception_policy>(r);
            return result_type(static_cast<result_base_type>(r));
        }
    };

    struct exception_not_possible {
        static result_type
        constexpr return_value(const T & t, const U & u){
            // just return the normal calcuation
            return result_type(t << u);
        }
    };
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
constexpr inline operator<<(const T & t, const U & u){
    // INT13-CPP
    using lsr = left_shift_result<T, U>;
    using result_base_type = typename lsr::result_base_type;
    using t_base_type = typename base_type<T>::type;
    using u_base_type = typename base_type<U>::type;

    constexpr const interval<t_base_type> t_interval(
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    );

    constexpr const interval<u_base_type> u_interval(
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    );

    constexpr const checked_result<interval<result_base_type>> r_interval {
        left_shift<result_base_type>(t_interval, u_interval)
    };

    return boost::mpl::if_c<
        // if no over/under flow or domain error possible
        r_interval.no_exception(),
        typename lsr::exception_not_possible,
        typename lsr::exception_possible
    >::type::return_value(t, u);
}

// right shift
template<class T, class U>
struct right_shift_result {
    typedef common_policies<T, U> P;
    typedef typename P::promotion_policy::template right_shift_result<
        T,
        U,
        typename P::promotion_policy,
        typename P::exception_policy
    >::type type;
    using result_type = type;
    using result_base_type = typename base_type<result_type>::type;
    using exception_policy = typename P::exception_policy;

    static_assert(
        boost::numeric::is_safe<result_type>::value,
        "Promotion failed to return safe type"
    );
    struct exception_possible {
        static result_type
        constexpr return_value(const T & t, const U & u){
            const checked_result<result_base_type> r = checked::right_shift<result_base_type>(
                base_value(t),
                base_value(u)
            );
            dispatch<exception_policy>(r);
            return result_type(static_cast<result_base_type>(r));
        }
    };
    struct exception_not_possible {
        static result_type
        constexpr return_value(const T & t, const U & u){
            // just return the normal calcuation
            return result_type(t >> u);
        }
    };
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
constexpr inline operator>>(const T & t, const U & u){
    // INT13-CPP
    using rsr = right_shift_result<T, U>;
    using result_base_type = typename rsr::result_base_type;
    using t_base_type = typename base_type<T>::type;
    using u_base_type = typename base_type<U>::type;

    constexpr const interval<t_base_type> t_interval(
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    );

    constexpr const interval<u_base_type> u_interval(
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    );

    constexpr const checked_result<interval<result_base_type>> r_interval {
        right_shift<result_base_type>(t_interval, u_interval)
    };

    return boost::mpl::if_c<
        // if no over/under flow or domain error possible
        r_interval.no_exception(),
        typename rsr::exception_not_possible,
        typename rsr::exception_possible
    >::type::return_value(t, u);
}

/////////////////////////////////////////////////////////////////
// bitwise operators

// operator |
template<class T, class U>
struct bitwise_result {
    typedef common_policies<T, U> P;
    typedef typename P::promotion_policy::template or_result<
        T,
        U,
        typename P::promotion_policy,
        typename P::exception_policy
    >::type type;
    using result_type = type;
    using result_base_type = typename base_type<result_type>::type;
    using exception_policy = typename P::exception_policy;
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
    static_assert(
        std::numeric_limits<result_base_type>::is_integer
        && (! std::numeric_limits<result_base_type>::is_signed),
        // INT13-C
        "bitwise operations should return unsigned integers"
    );
};

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    bitwise_result<T, U>
>::type
constexpr inline operator|(const T & t, const U & u){
    using bwr = bitwise_result<T, U>;
    using result_base_type = typename bwr::result_base_type;
    using result_type = typename bwr::result_type;
    using exception_policy = typename bwr::exception_policy;

    checked_result<result_base_type> r =
        checked::bitwise_or<result_base_type>(t, u);
    dispatch<exception_policy>(r);
    return static_cast<result_type>(static_cast<result_base_type>(r));
}

// operator &
template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    bitwise_result<T, U>
>::type
constexpr inline operator&(const T & t, const U & u){
    using bwr = bitwise_result<T, U>;
    using result_base_type = typename bwr::result_base_type;
    using result_type = typename bwr::result_type;
    using exception_policy = typename bwr::exception_policy;

    checked_result<result_base_type> r =
        checked::bitwise_and<result_base_type>(t, u);
    dispatch<exception_policy>(r);
    return static_cast<result_type>(static_cast<result_base_type>(r));
}

// operator ^
template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    bitwise_result<T, U>
>::type
constexpr inline operator^(const T & t, const U & u){
    using bwr = bitwise_result<T, U>;
    using result_base_type = typename bwr::result_base_type;
    using result_type = typename bwr::result_type;
    using exception_policy = typename bwr::exception_policy;

    checked_result<result_base_type> r =
        checked::bitwise_xor<result_base_type>(t, u);
    dispatch<exception_policy>(r);
    return static_cast<result_type>(static_cast<result_base_type>(r));
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
    const safe_base<T, Min, Max, P, E> & t
){
    os << (
        (std::is_same<T, signed char>::value
        || std::is_same<T, unsigned char>::value
        ) ?
            static_cast<int>(t.m_t)
        :
            t.m_t
    );
    return os;
}

namespace detail {

    template<class T, class Enable = void>
    struct Temp {
        T value;
    }; // primary template
     
    template<class T>
    struct Temp<
        T,
        typename std::enable_if<
            (std::is_same<T, signed char>::value
            || std::is_same<T, unsigned char>::value)
        >
    >
    {
        int value;
    };

} // detail

template<
    class T,
    T Min,
    T Max,
    class P, // promotion polic
    class E  // exception policy
>
std::istream & operator>>(
    std::istream & is,
    safe_base<T, Min, Max, P, E> & t
){

    detail::Temp<T> tx;
    is >> tx.value;
    if(is.fail())
        E::range_error("error in file input");
    t = tx.value;
    return is;
}

} // numeric
} // boost

#endif // BOOST_NUMERIC_SAFE_BASE_OPERATIONS_HPP
