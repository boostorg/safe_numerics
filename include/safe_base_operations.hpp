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
#include <boost/mpl/not.hpp>
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

template<typename R, typename E>
struct validate_detail {
    struct exception_possible {
        template<typename T>
        constexpr static R return_value(
            const T & t,
            const interval<R> & this_interval
        ){
            // INT08-C
            if(! this_interval.includes(t))
                E::range_error("Value out of range for this safe type");
            checked_result<R> r = checked::cast<R>(t);
            assert(r.no_exception());
            return r;
        }
    };
    struct exception_not_possible {
        template<typename T>
        constexpr static R return_value(
            const T & t,
            const interval<R> & this_interval
        ){
            return static_cast<R>(t);
        }
    };
};

template<class Stored, Stored Min, Stored Max, class P, class E>
template<class T>
constexpr Stored safe_base<Stored, Min, Max, P, E>::
validated_cast(const T & t) const {
    using t_base_type = typename base_type<T>::type;
    constexpr const interval<t_base_type> t_interval{
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    };
    constexpr const interval<Stored> this_interval(Min, Max);
    // if static values don't overlap, the program can never function
    static_assert(
        indeterminate(t_interval < this_interval),
        "safe type cannot be constructed with this type"
    );
    return boost::mpl::if_c<
        this_interval.includes(t_interval),
        typename validate_detail<Stored, E>::exception_not_possible,
        typename validate_detail<Stored, E>::exception_possible
    >::type::template return_value(base_value(t), this_interval);
}

template<class Stored, Stored Min, Stored Max, class P, class E>
template<typename T, T N>
constexpr Stored safe_base<Stored, Min, Max, P, E>::
validated_cast(const safe_literal_impl<T, N> & t) const {
    constexpr const interval<Stored> this_interval{};
    // if static values don't overlap, the program can never function
    static_assert(
        this_interval.includes(N),
        "safe type cannot be constructed value"
    );
    return static_cast<Stored>(t);
}

/////////////////////////////////////////////////////////////////
// construction and assignment operators

template<class Stored, Stored Min, Stored Max, class P, class E>
constexpr safe_base<Stored, Min, Max, P, E>::
safe_base(const Stored & rhs, std::false_type) :
    m_t(rhs)
{}

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
    constexpr const interval<R> r_interval;
    constexpr const interval<Stored> this_interval(Min, Max);
    // if static values don't overlap, the program can never function
    static_assert(
        indeterminate(r_interval < this_interval),
        "safe type cannot be constructed with this type"
    );
    return boost::mpl::if_c<
        r_interval.includes(this_interval),
        typename validate_detail<R, E>::exception_not_possible,
        typename validate_detail<R, E>::exception_possible
    >::type::template return_value(m_t, r_interval);
}

template< class Stored, Stored Min, Stored Max, class P, class E>
constexpr safe_base<Stored, Min, Max, P, E>::
operator Stored () const {
    return m_t;
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

    using exception_policy = typename P::exception_policy;
    using promotion_policy = typename P::promotion_policy;
    using t_base_type = typename base_type<T>::type;
    using u_base_type = typename base_type<U>::type;
    using result_base_type =
        typename promotion_policy::template addition_result<
            t_base_type,
            u_base_type
        >::type;

    // filter out case were overflow cannot occur
    // note: subtle trickery.  Suppose t is safe_range<MIN, ..>.  Then
    // std::numeric_limits<T>::min() will be safe_range<MIN with a value of MIN
    // Use base_value(T) ( which equals MIN ) to create a new interval. Same
    // for MAX.  Now
    constexpr static const interval<t_base_type> t_interval{
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    };

    constexpr static const interval<u_base_type> u_interval{
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    };

    // when we add the temporary intervals above, we'll get a new interval
    // with the correct range for the sum !
    constexpr static const checked_result<interval<result_base_type>> r_interval = add<result_base_type>(t_interval, u_interval);

    constexpr static bool exception_possible() {
        return ! r_interval.no_exception();
    }

    constexpr static const interval<result_base_type> type_interval =
        exception_possible() ?
            interval<result_base_type>{}
        :
            static_cast<interval<result_base_type>>(r_interval)
        ;

    using type = safe_base<
            result_base_type,
            type_interval.l,
            type_interval.u,
            promotion_policy,
            exception_policy
        >;

    // exception possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        static_assert(exception_possible(), "implement runtime check");
        checked_result<result_base_type> r = checked::add<result_base_type>(
            base_value(t),
            base_value(u)
        );
        dispatch<exception_policy>(r);
        return static_cast<result_base_type>(r);
    }

    // exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        static_assert(! exception_possible(), "no runtime check");
        return
            static_cast<result_base_type>(base_value(t))
            + static_cast<result_base_type>(base_value(u))
        ;
    }
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
    return typename ar::type(
        ar::return_value(
            t,
            u,
            typename std::integral_constant<bool, ar::exception_possible()>()
        ),
        std::false_type() // don't need to revalidate
    );
}

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    boost::mpl::identity<T &>
>::type
constexpr inline operator+=(T & t, const U & u){
    t = static_cast<T>(t + u);
    return t;
}

/////////////////////////////////////////////////////////////////
// subtraction

template<class T, class U>
struct subtraction_result {
    using P = common_policies<T, U>;
    using exception_policy = typename P::exception_policy;
    using promotion_policy = typename P::promotion_policy;
    using t_base_type = typename base_type<T>::type;
    using u_base_type = typename base_type<U>::type;
    using result_base_type =
        typename promotion_policy::template subtraction_result<
            t_base_type,
            u_base_type
        >::type;

    constexpr static const interval<t_base_type> t_interval{
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    };

    constexpr static const interval<u_base_type> u_interval{
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    };

    // when we add the temporary intervals above, we'll get a new interval
    // with the correct range for the difference !
    constexpr static const checked_result<interval<result_base_type>> r_interval
        = subtract<result_base_type>(t_interval, u_interval);

    constexpr static bool exception_possible() {
        return ! r_interval.no_exception();
    }

    constexpr static const interval<result_base_type> type_interval =
        exception_possible() ?
            interval<result_base_type>{}
        :
            static_cast<interval<result_base_type>>(r_interval)
        ;

    using type = safe_base<
            result_base_type,
            type_interval.l,
            type_interval.u,
            promotion_policy,
            exception_policy
        >;

    // exception possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        static_assert(exception_possible(), "implement runtime check");
        checked_result<result_base_type> r = checked::subtract<result_base_type>(
            base_value(t),
            base_value(u)
        );
        dispatch<exception_policy, result_base_type>(r);
        return static_cast<result_base_type>(r);
    }

    // exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        static_assert(! exception_possible(), "no runtime check");
        return
            static_cast<result_base_type>(base_value(t))
            - static_cast<result_base_type>(base_value(u))
        ;
    }
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
    return typename sr::type(
        sr::return_value(
            t,
            u,
            typename std::integral_constant<bool, sr::exception_possible()>()
        ),
        std::false_type() // don't need to revalidate
    );
}

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    boost::mpl::identity<T &>
>::type
constexpr inline operator-=(T & t, const U & u){
    t = static_cast<T>(t - u);
    return t;
}

/////////////////////////////////////////////////////////////////
// multiplication

template<class T, class U>
struct multiplication_result {
    using P = common_policies<T, U>;
    using exception_policy = typename P::exception_policy;
    using promotion_policy = typename P::promotion_policy;
    using t_base_type = typename base_type<T>::type;
    using u_base_type = typename base_type<U>::type;
    using result_base_type =
        typename promotion_policy::template multiplication_result<
            t_base_type,
            u_base_type
        >::type;

    // filter out case were overflow cannot occur
    // note: subtle trickery.  Suppose t is safe_range<MIN, ..>.  Then
    // std::numeric_limits<T>::min() will be safe_range<MIN with a value of MIN
    // Use base_value(T) ( which equals MIN ) to create a new interval. Same
    // for MAX.  Now
    constexpr static const interval<t_base_type> t_interval{
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    };

    constexpr static const interval<u_base_type> u_interval{
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    };

    // when we add the temporary intervals above, we'll get a new interval
    // with the correct range for the sum !
    constexpr static const checked_result<interval<result_base_type>> r_interval
        = multiply<result_base_type>(t_interval, u_interval);

    constexpr static bool exception_possible() {
        return ! r_interval.no_exception();
    }

    constexpr static const interval<result_base_type> type_interval =
        exception_possible() ?
            interval<result_base_type>{}
        :
            static_cast<interval<result_base_type>>(r_interval)
        ;

    using type = safe_base<
            result_base_type,
            type_interval.l,
            type_interval.u,
            promotion_policy,
            exception_policy
        >;

    constexpr static bool no_exception() {
        return r_interval.no_exception();
    }

    // exception possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        static_assert(exception_possible(), "implement runtime check");
        checked_result<result_base_type> r = checked::multiply<result_base_type>(
            base_value(t),
            base_value(u)
        );
        boost::numeric::dispatch<exception_policy>(r);
        return static_cast<result_base_type>(r);
    }

    // exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        static_assert(! exception_possible(), "no runtime check");
        return
            static_cast<result_base_type>(base_value(t))
            * static_cast<result_base_type>(base_value(u))
        ;
    }
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
    return typename mr::type(
        mr::return_value(
            t,
            u,
            typename std::integral_constant<bool, mr::exception_possible()>()
        ),
        std::false_type() // don't need to revalidate
    );
}

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    boost::mpl::identity<T &>
>::type
constexpr inline operator*=(T & t, const U & u){
    t = static_cast<T>(t * u);
    return t;
}

/////////////////////////////////////////////////////////////////
// division

template<class T, class U>
struct division_result {
    using P = common_policies<T, U>;
    using exception_policy = typename P::exception_policy;
    using promotion_policy = typename P::promotion_policy;
    using t_base_type = typename base_type<T>::type;
    using u_base_type = typename base_type<U>::type;
    using result_base_type =
        typename promotion_policy::template division_result<
            t_base_type,
            u_base_type
        >::type;

    constexpr static const interval<t_base_type> t_interval{
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    };

    constexpr static const interval<u_base_type> u_interval{
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    };

    constexpr static const checked_result<interval<result_base_type>> r_interval
        = divide_nz<result_base_type>(t_interval, u_interval);

    constexpr static bool exception_possible() {
        return
            // if over/under flow or domain error possible
            ! r_interval.no_exception()
            // if the denominator can contain zero
            || (u_interval.l <= 0 && u_interval.u >=0 )
        ;
    }

    constexpr static const interval<result_base_type> type_interval =
        exception_possible() ?
            interval<result_base_type>{}
        :
            static_cast<interval<result_base_type>>(r_interval)
        ;


    using type = safe_base<
            result_base_type,
            type_interval.l,
            type_interval.u,
            promotion_policy,
            exception_policy
        >;

    // exception possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        static_assert(exception_possible(), "implement runtime check");
        checked_result<result_base_type> r =
        promotion_policy::template divide<result_base_type>(
            base_value(t),
            base_value(u)
        );
        dispatch<exception_policy>(r);
        return static_cast<result_base_type>(r);
    }

    // exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        static_assert(! exception_possible(), "no runtime check");
        return
            static_cast<result_base_type>(base_value(t))
            / static_cast<result_base_type>(base_value(u))
        ;
    }
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
    return typename dr::type(
        dr::return_value(
            t,
            u,
            typename std::integral_constant<bool, dr::exception_possible()>()
        ),
        std::false_type() // don't need to revalidate
    );
}

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    boost::mpl::identity<T &>
>::type
constexpr inline operator/=(T & t, const U & u){
    t = static_cast<T>(t / u);
    return t;
}

/////////////////////////////////////////////////////////////////
// modulus

template<class T, class U>
struct modulus_result {
    typedef common_policies<T, U> P;
    using exception_policy = typename P::exception_policy;
    using promotion_policy = typename P::promotion_policy;
    using t_base_type = typename base_type<T>::type;
    using u_base_type = typename base_type<U>::type;
    using result_base_type =
        typename promotion_policy::template modulus_result<
            t_base_type,
            u_base_type
        >::type;

    constexpr static const interval<t_base_type> t_interval{
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    };

    constexpr static const interval<u_base_type> u_interval{
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    };

    constexpr static const checked_result<interval<result_base_type>> r_interval
        = modulus_nz<result_base_type>(t_interval, u_interval);

    constexpr static bool exception_possible() {
        return
            // if over/under flow or domain error possible
            ! r_interval.no_exception()
            // if the denominator can contain zero
            || (u_interval.l <= 0 && u_interval.u >=0 )
        ;
    }

    constexpr static const interval<result_base_type> type_interval =
        exception_possible() ?
            interval<result_base_type>{}
        :
            static_cast<interval<result_base_type>>(r_interval)
        ;

    using type = safe_base<
            result_base_type,
            type_interval.l,
            type_interval.u,
            promotion_policy,
            exception_policy
        >;


    // exception possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        static_assert(exception_possible(), "implement runtime check");
        checked_result<result_base_type> r = checked::modulus<result_base_type>(
            base_value(t),
            base_value(u)
        );
        dispatch<exception_policy>(r);
        return static_cast<result_base_type>(r);
    }

    // exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        static_assert(! exception_possible(), "no runtime check");
        return
            static_cast<result_base_type>(base_value(t))
            % static_cast<result_base_type>(base_value(u))
        ;
    }
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
    using mr = modulus_result<T, U>;
    return typename mr::type(
        mr::return_value(
            t,
            u,
            typename std::integral_constant<bool, mr::exception_possible()>()
        ),
        std::false_type() // don't need to revalidate
    );
}

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    boost::mpl::identity<T &>
>::type
constexpr inline operator%=(T & t, const U & u){
    t = static_cast<T>(t % u);
    return t;
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
    return ! ( lhs > rhs );
}

/////////////////////////////////////////////////////////////////
// shift operators

// left shift
template<class T, class U>
struct left_shift_result {
    using P = common_policies<T, U>;
    using exception_policy = typename P::exception_policy;
    using promotion_policy = typename P::promotion_policy;
    using t_base_type = typename base_type<T>::type;
    using u_base_type = typename base_type<U>::type;
    using result_base_type =
        typename promotion_policy::template left_shift_result<
            t_base_type,
            u_base_type
        >::type;

    constexpr static const interval<t_base_type> t_interval{
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    };

    constexpr static const interval<u_base_type> u_interval{
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    };

    constexpr static const checked_result<interval<result_base_type>> r_interval {
        left_shift<result_base_type>(t_interval, u_interval)
    };

    constexpr static bool exception_possible() {
        return ! r_interval.no_exception();
    }

    constexpr static const interval<result_base_type> type_interval =
        exception_possible() ?
            interval<result_base_type>{}
        :
            static_cast<interval<result_base_type>>(r_interval)
        ;

    using type = safe_base<
            result_base_type,
            type_interval.l,
            type_interval.u,
            promotion_policy,
            exception_policy
        >;

    // exception possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        static_assert(exception_possible(), "implement runtime check");
        checked_result<result_base_type> r = checked::left_shift<result_base_type>(
            base_value(t),
            base_value(u)
        );
        dispatch<exception_policy>(r);
        return static_cast<result_base_type>(r);
    }

    // exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        static_assert(! exception_possible(), "no runtime check");
        return
            static_cast<result_base_type>(base_value(t))
            << static_cast<result_base_type>(base_value(u))
        ;
    }
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
    return typename lsr::type(
        lsr::return_value(
            t,
            u,
            typename std::integral_constant<bool, lsr::exception_possible()>()
        ),
        std::false_type() // don't need to revalidate
    );
}

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    boost::mpl::identity<T &>
>::type
constexpr inline operator<<=(T & t, const U & u){
    t = static_cast<T>(t << u);
    return t;
}

// right shift
template<class T, class U>
struct right_shift_result {
    using P = common_policies<T, U>;
    using exception_policy = typename P::exception_policy;
    using promotion_policy = typename P::promotion_policy;
    using t_base_type = typename base_type<T>::type;
    using u_base_type = typename base_type<U>::type;
    using result_base_type =
        typename promotion_policy::template right_shift_result<
            t_base_type,
            u_base_type
        >::type;

    constexpr static const interval<t_base_type> t_interval{
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    };

    constexpr static const interval<u_base_type> u_interval{
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    };

    constexpr static const checked_result<interval<result_base_type>> r_interval {
        right_shift<result_base_type>(t_interval, u_interval)
    };

    constexpr static bool exception_possible() {
        return ! r_interval.no_exception();
    }

    constexpr static const interval<result_base_type> type_interval =
        exception_possible() ?
            interval<result_base_type>{}
        :
            static_cast<interval<result_base_type>>(r_interval)
        ;

    using type = safe_base<
            result_base_type,
            type_interval.l,
            type_interval.u,
            promotion_policy,
            exception_policy
        >;

    // exception possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        static_assert(exception_possible(), "implement runtime check");
        checked_result<result_base_type> r = checked::right_shift<result_base_type>(
            base_value(t),
            base_value(u)
        );
        dispatch<exception_policy>(r);
        return static_cast<result_base_type>(r);
    }

    // exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        static_assert(! exception_possible(), "no runtime check");
        return
            static_cast<result_base_type>(base_value(t))
            >> static_cast<result_base_type>(base_value(u))
        ;
    }
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
    return typename rsr::type(
        rsr::return_value(
            t,
            u,
            typename std::integral_constant<bool, rsr::exception_possible()>()
        ),
        std::false_type() // don't need to revalidate
    );
}

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    boost::mpl::identity<T &>
>::type
constexpr inline operator>>=(T & t, const U & u){
    t = static_cast<T>(t >> u);
    return t;
}

/////////////////////////////////////////////////////////////////
// bitwise operators

// operator |
template<class T, class U>
struct bitwise_or_result {
    using P = common_policies<T, U>;
    using exception_policy = typename P::exception_policy;
    using promotion_policy = typename P::promotion_policy;
    using t_base_type = typename base_type<T>::type;
    using u_base_type = typename base_type<U>::type;
    using result_base_type =
        typename promotion_policy::template bitwise_result<
            t_base_type,
            u_base_type
        >::type;

    constexpr static result_base_type r =
        safe_compare::greater_than(
            base_value(std::numeric_limits<T>::max()),
            base_value(std::numeric_limits<U>::max())
        ) ?
            base_value(std::numeric_limits<T>::max())
        :
            base_value(std::numeric_limits<U>::max())
        ;

    using type = safe_base<
            result_base_type,
            0,
            r,
            promotion_policy,
            exception_policy
        >;
};

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    bitwise_or_result<T, U>
>::type
constexpr inline operator|(const T & t, const U & u){
    using bwr = bitwise_or_result<T, U>;
    using result_base_type = typename bwr::result_base_type;
    using exception_policy = typename bwr::exception_policy;

    const checked_result<result_base_type> r =
        checked::bitwise_or<result_base_type>(
            base_value(t),
            base_value(u)
        );
    assert(r.no_exception());
    return static_cast<result_base_type>(r);
}

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    boost::mpl::identity<T &>
>::type
constexpr inline operator|=(T & t, const U & u){
    t = static_cast<T>(t | u);
    return t;
}

// operator &
template<class T, class U>
struct bitwise_and_result {
    using P = common_policies<T, U>;
    using exception_policy = typename P::exception_policy;
    using promotion_policy = typename P::promotion_policy;
    using t_base_type = typename base_type<T>::type;
    using u_base_type = typename base_type<U>::type;
    using result_base_type =
        typename promotion_policy::template bitwise_result<
            t_base_type,
            u_base_type
        >::type;

    constexpr static result_base_type r =
        safe_compare::less_than(
            base_value(std::numeric_limits<T>::max()),
            base_value(std::numeric_limits<U>::max())
        ) ?
            base_value(std::numeric_limits<T>::max())
        :
            base_value(std::numeric_limits<U>::max())
        ;

    using type = safe_base<
            result_base_type,
            0,
            r,
            promotion_policy,
            exception_policy
        >;
};

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    bitwise_and_result<T, U>
>::type
constexpr inline operator&(const T & t, const U & u){
    using bwr = bitwise_and_result<T, U>;
    using result_base_type = typename bwr::result_base_type;
    using exception_policy = typename bwr::exception_policy;

    const checked_result<result_base_type> r =
        checked::bitwise_and<result_base_type>(
            base_value(t),
            base_value(u)
        );
    assert(r.no_exception());
    return static_cast<result_base_type>(r);
}

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    boost::mpl::identity<T &>
>::type
constexpr inline operator&=(T & t, const U & u){
    t = static_cast<T>(t & u);
    return t;
}

// operator ^
template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    bitwise_or_result<T, U>
>::type
constexpr inline operator^(const T & t, const U & u){
    using bwr = bitwise_or_result<T, U>;
    using result_base_type = typename bwr::result_base_type;
    using exception_policy = typename bwr::exception_policy;

    const checked_result<result_base_type> r =
        checked::bitwise_xor<result_base_type>(t, u);
    assert(r.no_exception());
    return static_cast<result_base_type>(r);
}

template<class T, class U>
typename boost::lazy_enable_if<
    boost::mpl::or_<
        boost::numeric::is_safe<T>,
        boost::numeric::is_safe<U>
    >,
    boost::mpl::identity<T &>
>::type
constexpr inline operator^=(T & t, const U & u){
    t = static_cast<T>(t ^ u);
    return t;
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

namespace {
    template<class T>
    struct Temp {
        T value;
    }; // primary template
    template<> struct Temp<signed char> {
        int value;
    }; // secondary template
    template<> struct Temp<unsigned char> {
        int value;
    }; // secondary template
    template<> struct Temp<signed wchar_t> {
        int value;
    }; // secondary template
    template<> struct Temp<unsigned wchar_t> {
        int value;
    }; // secondary template
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
    safe_base<T, Min, Max, P, E> & t
){
    Temp<T> tx;
    is >> tx.value;
    if(is.fail())
        E::range_error("error in file input");
    t = tx.value;
    return is;
}

} // numeric
} // boost

#endif // BOOST_NUMERIC_SAFE_BASE_OPERATIONS_HPP
