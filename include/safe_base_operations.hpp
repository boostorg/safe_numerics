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
#include <type_traits> // is_base_of, is_same, is_floating_point
#include <algorithm>   // max
#include <cassert>

#include <boost/config.hpp>
//#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>

#include <boost/core/enable_if.hpp> // lazy_enable_if
#include <boost/integer.hpp>
#include <boost/logic/tribool.hpp>

#include "checked_integer.hpp"
#include "checked_result.hpp"
#include "checked_result_operations.hpp"
#include "safe_base.hpp"

#include "interval.hpp"
#include "utility.hpp"
#include "safe_integer_literal.hpp"

namespace boost {
namespace numeric {

// invoke error handling
template<class EP, typename R>
constexpr void
dispatch(const checked_result<R> & cr){
    // if the result contains an error condition
    if(cr.exception())
        // dispatch to the appropriate function
        dispatch<EP>(cr.m_e, cr.m_msg);
    // otherwise just do a simple return
}

/////////////////////////////////////////////////////////////////
// validation

template<typename R, R Min, R Max, typename T, typename E>
struct validate_detail {
    constexpr static const interval<checked_result<R>> t_interval{
        checked::cast<R>(base_value(std::numeric_limits<T>::min())),
        checked::cast<R>(base_value(std::numeric_limits<T>::max()))
    };
    constexpr static const interval<checked_result<R>> r_interval{Min, Max};

/*
    static_assert(
        true != r_interval.excludes(t_interval),
        "ranges don't overlap: can't cast"
    );
*/

    struct exception_possible {
        constexpr static R return_value(
            const T & t
        ){
            // INT08-C
            const checked_result<R> r = checked::cast<R>(t);
            dispatch<E>(r);
            return base_value(r);
        }
    };
    struct exception_not_possible {
        constexpr static R return_value(
            const T & t
        ){
            static_assert(
                static_cast<bool>(r_interval.includes(t_interval)),
                "exeption not possible"
            );
            return static_cast<R>(base_value(t));
        }
    };

    constexpr static R return_value(const T & t){
        return boost::mpl::if_c<
            static_cast<bool>(r_interval.includes(t_interval)),
            exception_not_possible,
            exception_possible
        >::type::return_value(t);
    }
};

template<class Stored, Stored Min, Stored Max, class P, class E>
template<class T>
constexpr Stored safe_base<Stored, Min, Max, P, E>::
validated_cast(const T & t) const {
    return validate_detail<Stored,Min,Max,T,E>::return_value(t);
}

template<class Stored, Stored Min, Stored Max, class P, class E>
template<typename T, T N, class P1, class E1>
constexpr Stored safe_base<Stored, Min, Max, P, E>::
validated_cast(const safe_literal_impl<T, N, P1, E1> &) const {
    constexpr const interval<Stored> this_interval{};
    // if static values don't overlap, the program can never function
    static_assert(
        this_interval.includes(N),
        "safe type cannot be constructed from this value"
    );
    return static_cast<Stored>(N);
}

/////////////////////////////////////////////////////////////////
// casting operators

// cast to a builtin type from a safe type
template< class Stored, Stored Min, Stored Max, class P, class E>
#if 1
template<
    class R,
    typename std::enable_if<
        ! boost::numeric::is_safe<R>::value,
        int
    >::type
>
#else
template<class R>
#endif
constexpr safe_base<Stored, Min, Max, P, E>::
operator R () const {

    // if static values don't overlap, the program can never function
    constexpr const interval<R> r_interval;
    constexpr const interval<Stored> this_interval(Min, Max);
    static_assert(
        ! r_interval.excludes(this_interval),
        "safe type cannot be constructed with this type"
    );

    return validate_detail<
        R,
        std::numeric_limits<R>::min(),
        std::numeric_limits<R>::max(),
        Stored,
        E
    >::return_value(m_t);

}

// cast to the underlying builtin type from a safe type
template< class Stored, Stored Min, Stored Max, class P, class E>
constexpr safe_base<Stored, Min, Max, P, E>::
operator Stored () const {
    return m_t;
}


/////////////////////////////////////////////////////////////////
// binary operators

template<class T, class U>
struct common_exception_policy {
    static_assert(is_safe<T>::value || is_safe<U>::value,
        "at least one type must be a safe type"
    );

    using t_exception_policy = typename get_exception_policy<T>::type;
    using u_exception_policy = typename get_exception_policy<U>::type;

    static_assert(
        std::is_same<t_exception_policy, u_exception_policy>::value
        || std::is_same<t_exception_policy, void>::value
        || std::is_same<void, u_exception_policy>::value,
        "if the exception policies are different, one must be void!"
    );

    static_assert(
        ! (std::is_same<t_exception_policy, void>::value
        && std::is_same<void, u_exception_policy>::value),
        "at least one exception policy must not be void"
    );

    using type =
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
        !std::is_same<void, type>::value,
        "exception_policy is void"
    );
};

template<class T, class U>
struct common_promotion_policy {
    static_assert(is_safe<T>::value || is_safe<U>::value,
        "at least one type must be a safe type"
    );
    using t_promotion_policy = typename get_promotion_policy<T>::type;
    using u_promotion_policy = typename get_promotion_policy<U>::type;
    static_assert(
        std::is_same<t_promotion_policy, u_promotion_policy>::value
        ||std::is_same<t_promotion_policy, void>::value
        ||std::is_same<void, u_promotion_policy>::value,
        "if the promotion policies are different, one must be void!"
    );
    static_assert(
        ! (std::is_same<t_promotion_policy, void>::value
        && std::is_same<void, u_promotion_policy>::value),
        "at least one promotion policy must not be void"
    );

    using type =
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
        ! std::is_same<void, type>::value,
        "promotion_policy is void"
    );

};

// give the resultant base type, figure out what the final result
// type will be.  Note we currently need this because we support
// return of only safe integer types. Someday ..., we'll support
// all other safe types including float and user defined ones.
//

// Note: the following global operators will be found via
// argument dependent lookup.

/////////////////////////////////////////////////////////////////
// addition

template<class T, class U>
struct addition_result {
private:
    using promotion_policy = typename common_promotion_policy<T, U>::type;
    using result_base_type =
        typename promotion_policy::template addition_result<T,U>::type;

    using r_type = checked_result<result_base_type>;
    using r_interval_type = interval<r_type>;

    constexpr static const r_interval_type t_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::max()))
    };

    constexpr static const r_interval_type u_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::max()))
    };

    constexpr static const r_interval_type r_interval = t_interval + u_interval;

    constexpr static bool exception_possible(){
        return r_interval.l.exception() || r_interval.u.exception();
    }

    // if exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        return
            static_cast<result_base_type>(base_value(t))
            + static_cast<result_base_type>(base_value(u));
    }

    // if exception possible
    using exception_policy = typename common_exception_policy<T, U>::type;

    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        const r_type tx = checked::cast<result_base_type>(base_value(t));
        if(tx.exception())
            dispatch<exception_policy>(tx);
        const r_type ux = checked::cast<result_base_type>(base_value(u));
        if(ux.exception())
            dispatch<exception_policy>(ux);
        // the following will use checked arithmetic
        const r_type r = (tx + ux);
        if(!r.exception()){
            return static_cast<result_base_type>(r);
        }
        // handle error condition
        dispatch<exception_policy>(r);
        // if we get here, the error has been ignored
        // just get the result the old fashioned way
        return
            static_cast<result_base_type>(tx)
            + static_cast<result_base_type>(ux);
    }

public:
    using type = safe_base<
        result_base_type,
        r_interval.l.exception()
            ? std::numeric_limits<result_base_type>::min()
            : static_cast<result_base_type>(r_interval.l),
        r_interval.u.exception()
            ? std::numeric_limits<result_base_type>::max()
            : static_cast<result_base_type>(r_interval.u),
        promotion_policy,
        exception_policy
    >;

    constexpr static type return_value(const T & t, const U & u){
        return type(
            return_value(
                t,
                u,
                std::integral_constant<bool, exception_possible()>()
            ),
            typename type::skip_validation()
        );
    }
};

template<class T, class U>
typename boost::lazy_enable_if_c<
    is_safe<T>::value || is_safe<U>::value,
    addition_result<T, U>
>::type
constexpr operator+(const T & t, const U & u){
    return addition_result<T, U>::return_value(t, u);
}

template<class T, class U>
typename std::enable_if<
    is_safe<T>::value || is_safe<U>::value,
    T
>::type
constexpr operator+=(T & t, const U & u){
    t = static_cast<T>(t + u);
    return t;
}

/////////////////////////////////////////////////////////////////
// subtraction

template<class T, class U>
struct subtraction_result {
private:
    using promotion_policy = typename common_promotion_policy<T, U>::type;
    using result_base_type =
        typename promotion_policy::template subtraction_result<T, U>::type;

    using r_type = checked_result<result_base_type>;
    using r_interval_type = interval<r_type>;

    constexpr static const r_interval_type t_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::max()))
    };

    constexpr static const r_interval_type u_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::max()))
    };

    constexpr static const r_interval_type r_interval = t_interval - u_interval;

    constexpr static bool exception_possible(){
        return r_interval.l.exception() || r_interval.u.exception();
    }

    // if exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        return
            static_cast<result_base_type>(base_value(t))
            - static_cast<result_base_type>(base_value(u));
    }

    // if exception possible
    using exception_policy = typename common_exception_policy<T, U>::type;

    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        const r_type tx = checked::cast<result_base_type>(base_value(t));
        if(tx.exception())
            dispatch<exception_policy>(tx);
        const r_type ux = checked::cast<result_base_type>(base_value(u));
        if(ux.exception())
            dispatch<exception_policy>(ux);
        // the following will use checked arithmetic
        r_type r = (tx - ux);
        if(!r.exception())
            return static_cast<result_base_type>(r);
        // handle error condition
        dispatch<exception_policy>(r);
        // if we get here, the error has been ignored
        // just get the result the old fashioned way
        return
            static_cast<result_base_type>(tx)
            - static_cast<result_base_type>(ux);
    }

public:
    using type = safe_base<
        result_base_type,
        r_interval.l.exception()
            ? std::numeric_limits<result_base_type>::min()
            : static_cast<result_base_type>(r_interval.l),
        r_interval.u.exception()
            ? std::numeric_limits<result_base_type>::max()
            : static_cast<result_base_type>(r_interval.u),
        promotion_policy,
        exception_policy
    >;

    constexpr static type return_value(const T & t, const U & u){
        return type(
            return_value(
                t,
                u,
                std::integral_constant<bool, exception_possible()>()
            ),
            typename type::skip_validation()
        );
    }
};

template<class T, class U>
typename boost::lazy_enable_if_c<
    is_safe<T>::value || is_safe<U>::value,
    subtraction_result<T, U>
>::type
constexpr operator-(const T & t, const U & u){
    return subtraction_result<T, U>::return_value(t, u);
}

template<class T, class U>
typename std::enable_if<
    is_safe<T>::value || is_safe<U>::value,
    T
>::type
constexpr operator-=(T & t, const U & u){
    t = static_cast<T>(t - u);
    return t;
}

/////////////////////////////////////////////////////////////////
// multiplication

template<class T, class U>
struct multiplication_result {
private:
    using promotion_policy = typename common_promotion_policy<T, U>::type;
    using result_base_type =
        typename promotion_policy::template multiplication_result<T, U>::type;

    using r_type = checked_result<result_base_type>;
    using r_interval_type = interval<checked_result<result_base_type>>;
    
    constexpr static const r_interval_type t_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::max()))
    };

    constexpr static const r_interval_type u_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::max()))
    };

    constexpr static const r_interval_type r_interval = t_interval * u_interval;


    constexpr static bool exception_possible(){
        return r_interval.l.exception() || r_interval.u.exception();
    }

    // if exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        return
            static_cast<result_base_type>(base_value(t))
            * static_cast<result_base_type>(base_value(u));
    }

    // if exception possible
    using exception_policy = typename common_exception_policy<T, U>::type;
    
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        const r_type tx = checked::cast<result_base_type>(base_value(t));
        if(tx.exception())
            dispatch<exception_policy>(tx);
        const r_type ux = checked::cast<result_base_type>(base_value(u));
        if(ux.exception())
            dispatch<exception_policy>(ux);
        r_type r = (tx * ux);
        if(!r.exception())
            return static_cast<result_base_type>(r);
        // handle error condition
        dispatch<exception_policy>(r);
        // if we get here, the error has been ignored
        // just get the result the old fashioned way
        return
            static_cast<result_base_type>(tx)
            * static_cast<result_base_type>(ux);
    }

public:
    using type = safe_base<
        result_base_type,
        r_interval.l.exception()
            ? std::numeric_limits<result_base_type>::min()
            : static_cast<result_base_type>(r_interval.l),
        r_interval.u.exception()
            ? std::numeric_limits<result_base_type>::max()
            : static_cast<result_base_type>(r_interval.u),
        promotion_policy,
        exception_policy
    >;

    constexpr static type return_value(const T & t, const U & u){
        return type(
            return_value(
                t,
                u,
                std::integral_constant<bool, exception_possible()>()
            ),
            typename type::skip_validation()
        );
    }
};

template<class T, class U>
typename boost::lazy_enable_if_c<
    is_safe<T>::value || is_safe<U>::value,
    multiplication_result<T, U>
>::type
constexpr operator*(const T & t, const U & u){
    // argument dependent lookup should guarentee that we only get here
    return multiplication_result<T, U>::return_value(t, u);
}

template<class T, class U>
typename std::enable_if<
    is_safe<T>::value || is_safe<U>::value,
    T
>::type
constexpr operator*=(T & t, const U & u){
    t = static_cast<T>(t * u);
    return t;
}

/////////////////////////////////////////////////////////////////
// division

// key idea here - result will never be larger than T
template<class T, class U>
struct division_result {
private:
    using promotion_policy = typename common_promotion_policy<T, U>::type;
    using result_base_type =
        typename promotion_policy::template division_result<T, U>::type;

    using r_type = checked_result<result_base_type>;
    using r_interval_type = interval<r_type>;

    constexpr static const r_interval_type t_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::max()))
    };

    constexpr static const r_interval_type u_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::max()))
    };
    constexpr static const r_interval_type rx(){
        if(u_interval.u < r_type(0)
        || u_interval.l > r_type(0))
            return t_interval / u_interval;
        return utility::minmax(
            std::initializer_list<r_type> {
                t_interval.l / u_interval.l,
                t_interval.l / r_type(-1),
                t_interval.l / r_type(1),
                t_interval.l / u_interval.u,
                t_interval.u / u_interval.l,
                t_interval.u / r_type(-1),
                t_interval.u / r_type(1),
                t_interval.u / u_interval.u,
            }
        );
    }

    constexpr static const r_interval_type r_interval = rx();

    constexpr static bool exception_possible() {
        return u_interval.includes(r_type(0))
        || r_interval.l.exception()
        || r_interval.u.exception();
    }

    // if exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        return
            static_cast<result_base_type>(base_value(t))
            / static_cast<result_base_type>(base_value(u));
    }

    // if exception possible
    using exception_policy = typename common_exception_policy<T, U>::type;

    constexpr static int bits = std::min(
        std::numeric_limits<std::uintmax_t>::digits,
        std::max(std::initializer_list<int>{
            std::numeric_limits<result_base_type>::digits,
            std::numeric_limits<typename base_type<T>::type>::digits,
            std::numeric_limits<typename base_type<U>::type>::digits
        }) + (std::numeric_limits<result_base_type>::is_signed ? 1 : 0)
    );

    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        using temp_base = typename boost::mpl::if_c<
            std::numeric_limits<result_base_type>::is_signed,
            typename boost::int_t<bits>::least,
            typename boost::uint_t<bits>::least
        >::type;
        using t_type = checked_result<temp_base>;

        const t_type tx = checked::cast<temp_base>(base_value(t));
        if(tx.exception())
            dispatch<exception_policy>(tx);
        const t_type ux = checked::cast<temp_base>(base_value(u));
        if(ux.exception())
            dispatch<exception_policy>(ux);
        const t_type r = (tx / ux);
        if(!r.exception())
            return static_cast<result_base_type>(r);
        // handle error condition
        dispatch<exception_policy>(r);
        // if we get here, the error has been ignored
        // just get the result the old fashioned way
        return
            static_cast<result_base_type>(tx)
            / static_cast<result_base_type>(ux);
    }

public:
    using type = safe_base<
        result_base_type,
        r_interval.l.exception()
            ? std::numeric_limits<result_base_type>::min()
            : static_cast<result_base_type>(r_interval.l),
        r_interval.u.exception()
            ? std::numeric_limits<result_base_type>::max()
            : static_cast<result_base_type>(r_interval.u),
        promotion_policy,
        exception_policy
    >;

    constexpr static type return_value(const T & t, const U & u){
        return type(
            return_value(
                t,
                u,
                std::integral_constant<bool, exception_possible()>()
            ),
            typename type::skip_validation()
        );
    }
};

template<class T, class U>
typename boost::lazy_enable_if_c<
    is_safe<T>::value || is_safe<U>::value,
    division_result<T, U>
>::type
constexpr operator/(const T & t, const U & u){
    return division_result<T, U>::return_value(t, u);
}

template<class T, class U>
typename std::enable_if<
    is_safe<T>::value || is_safe<U>::value,
    T
>::type
constexpr operator/=(T & t, const U & u){
    t = static_cast<T>(t / u);
    return t;
}

/////////////////////////////////////////////////////////////////
// modulus

template<class T, class U>
struct modulus_result {
private:
    using promotion_policy = typename common_promotion_policy<T, U>::type;
    using result_base_type = typename promotion_policy::template modulus_result<T, U>::type;

    using r_type = checked_result<result_base_type>;
    using r_interval_type = interval<r_type>;

    constexpr static const r_interval_type t_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::max()))
    };

    constexpr static const r_interval_type u_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::max()))
    };

    constexpr static const r_interval_type rx(){
        if(u_interval.u < r_type(0)
        || u_interval.l > r_type(0))
            return t_interval % u_interval;
        return utility::minmax(
            std::initializer_list<r_type> {
                t_interval.l % u_interval.l,
                t_interval.l % r_type(-1),
                t_interval.l % r_type(1),
                t_interval.l % u_interval.u,
                t_interval.u % u_interval.l,
                t_interval.u % r_type(-1),
                t_interval.u % r_type(1),
                t_interval.u % u_interval.u,
            }
        );
    }

    constexpr static const r_interval_type r_interval = rx();

    constexpr static bool exception_possible() {
        return u_interval.includes(r_type(0))
        || r_interval.l.exception()
        || r_interval.u.exception();
    }

    // if exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        return
            static_cast<result_base_type>(base_value(t))
            % static_cast<result_base_type>(base_value(u));
    }

    // if exception possible
    using exception_policy = typename common_exception_policy<T, U>::type;

    constexpr static int bits = std::min(
        std::numeric_limits<std::uintmax_t>::digits,
        std::max(std::initializer_list<int>{
            std::numeric_limits<result_base_type>::digits,
            std::numeric_limits<typename base_type<T>::type>::digits,
            std::numeric_limits<typename base_type<U>::type>::digits
        }) + (std::numeric_limits<result_base_type>::is_signed ? 1 : 0)
    );

    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        using temp_base = typename boost::mpl::if_c<
            std::numeric_limits<result_base_type>::is_signed,
            typename boost::int_t<bits>::least,
            typename boost::uint_t<bits>::least
        >::type;
        using t_type = checked_result<temp_base>;
        const t_type tx = checked::cast<temp_base>(base_value(t));
        if(tx.exception())
            dispatch<exception_policy>(tx);
        const t_type ux = checked::cast<temp_base>(base_value(u));
        if(ux.exception())
            dispatch<exception_policy>(ux);
        t_type r = (tx % ux);

        if(!r.exception())
            return static_cast<result_base_type>(r);
        // handle error condition
        dispatch<exception_policy>(r);
        // if we get here, the error has been ignored
        // just get the result the old fashioned way
        return
            static_cast<result_base_type>(tx)
            % static_cast<result_base_type>(ux);
    }

public:
    using type = safe_base<
        result_base_type,
        r_interval.l.exception()
            ? std::numeric_limits<result_base_type>::min()
            : static_cast<result_base_type>(r_interval.l),
        r_interval.u.exception()
            ? std::numeric_limits<result_base_type>::max()
            : static_cast<result_base_type>(r_interval.u),
        promotion_policy,
        exception_policy
    >;

    constexpr static type return_value(const T & t, const U & u){
        return type(
            return_value(
                t,
                u,
                std::integral_constant<bool, exception_possible()>()
            ),
            typename type::skip_validation()
        );
    }
};

template<class T, class U>
typename boost::lazy_enable_if_c<
   is_safe<T>::value || is_safe<U>::value,
    modulus_result<T, U>
>::type
constexpr operator%(const T & t, const U & u){
    // see https://en.wikipedia.org/wiki/Modulo_operation
    return modulus_result<T, U>::return_value(t, u);
}

template<class T, class U>
typename std::enable_if<
    is_safe<T>::value || is_safe<U>::value,
    T
>::type
constexpr operator%=(T & t, const U & u){
    t = static_cast<T>(t % u);
    return t;
}

/////////////////////////////////////////////////////////////////
// comparison

// less than

template<class T, class U>
struct less_than_result {
private:
    using promotion_policy = typename common_promotion_policy<T, U>::type;

    using result_base_type =
        typename promotion_policy::template comparison_result<T, U>::type;

    using r_type = checked_result<result_base_type>;
    using r_interval_type = interval<r_type>;

    constexpr static const r_interval_type t_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::max()))
    };

    constexpr static const r_interval_type u_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::max()))
    };

    using exception_policy = typename common_exception_policy<T, U>::type;

    // if exception possible
    constexpr static bool
    return_valuex(const T & t, const U & u){
        const r_type tx = checked::cast<result_base_type>(base_value(t));
        if(tx.exception())
            dispatch<exception_policy>(tx);
        const r_type ux = checked::cast<result_base_type>(base_value(u));
        if(ux.exception())
            dispatch<exception_policy>(ux);
        const boost::logic::tribool r = (tx < ux);
        if(r || !r) // answer is not indeterminate
            return r;
        dispatch<exception_policy>(
            checked_result<bool>(safe_numerics_error::domain_error)
        );
        return safe_compare::less_than(base_value(t), base_value(u));
    }

public:
    constexpr static bool
    return_value(const T & t, const U & u){
        return
            t_interval < u_interval
            ? true
            : t_interval > u_interval
            ? false
            : return_valuex(t, u);
    }
};

template<class T, class U>
typename std::enable_if<
    is_safe<T>::value || is_safe<U>::value,
    bool
>::type
constexpr operator<(const T & lhs, const U & rhs) {
    return less_than_result<T, U>::return_value(lhs, rhs);
}

template<class T, class U>
typename std::enable_if<
    is_safe<T>::value || is_safe<U>::value,
    bool
>::type
constexpr operator>(const T & lhs, const U & rhs) {
    return rhs < lhs;
}

template<class T, class U>
typename std::enable_if<
    is_safe<T>::value || is_safe<U>::value,
    bool
>::type
constexpr operator>=(const T & lhs, const U & rhs) {
    return ! ( lhs < rhs );
}

template<class T, class U>
typename std::enable_if<
    is_safe<T>::value || is_safe<U>::value,
    bool
>::type
constexpr operator<=(const T & lhs, const U & rhs) {
    return ! ( lhs > rhs );
}

// equal

template<class T, class U>
struct equal_result {
private:
    using promotion_policy = typename common_promotion_policy<T, U>::type;

    using result_base_type =
        typename promotion_policy::template comparison_result<T, U>::type;

    using r_type = checked_result<result_base_type>;
    using r_interval_type = interval<r_type>;

    constexpr static const r_interval_type t_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::max()))
    };

    constexpr static const r_interval_type u_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::max()))
    };

    using exception_policy = typename common_exception_policy<T, U>::type;

    // exception possible
    constexpr static bool return_valuex(const T & t, const U & u){
        const r_type tx = checked::cast<result_base_type>(base_value(t));
        if(tx.exception())
            dispatch<exception_policy>(tx);
        const r_type ux = checked::cast<result_base_type>(base_value(u));
        if(ux.exception())
            dispatch<exception_policy>(ux);
        const boost::logic::tribool r = (tx == ux);
        if(r || !r) // answer is not indeterminate
            return r;
        dispatch<exception_policy>(
            checked_result<bool>(safe_numerics_error::domain_error)
        );
        return safe_compare::equal(base_value(t), base_value(u));
    }

public:
    constexpr static bool
    return_value(const T & t, const U & u){
        return
            t_interval < u_interval
            ? false  // cannot be equal
            : t_interval > u_interval
            ? false  // cannot be equal
            : return_valuex(t, u);
    }
};

template<class T, class U>
typename std::enable_if<
    is_safe<T>::value || is_safe<U>::value,
    bool
>::type
constexpr operator==(const T & lhs, const U & rhs) {
    return equal_result<T, U>::return_value(lhs, rhs);
}

template<class T, class U>
typename std::enable_if<
    is_safe<T>::value || is_safe<U>::value,
    bool
>::type
constexpr operator!=(const T & lhs, const U & rhs) {
    return ! (lhs == rhs);
}

/////////////////////////////////////////////////////////////////
// shift operators

// left shift
template<class T, class U>
struct left_shift_result {
private:
    using promotion_policy = typename common_promotion_policy<T, U>::type;
    using result_base_type =
        typename promotion_policy::template left_shift_result<T, U>::type;

    using r_type = checked_result<result_base_type>;
    using r_interval_type = interval<r_type>;

    constexpr static const r_interval_type t_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::max()))
    };

    constexpr static const r_interval_type u_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::max()))
    };

/*
    static_assert(t_interval.u >= 0, "negative values can't be shifted");
    static_assert(u_interval.u >= 0, "shift count can't be negative");
    static_assert(u_interval.l <= std::numeric_limits<T>::digits,
        "shift count can't exceed argument size"
    );
*/
    constexpr static const r_interval_type r_interval = (t_interval << u_interval);

    constexpr static bool exception_possible(){
        return r_interval.l.exception() || r_interval.u.exception();
    }

    // if exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        return
            static_cast<result_base_type>(base_value(t))
            << static_cast<result_base_type>(base_value(u));
    }

    // exception possible
    using exception_policy = typename common_exception_policy<T, U>::type;

    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        const r_type tx = checked::cast<result_base_type>(base_value(t));
        if(tx.exception())
            dispatch<exception_policy>(tx);
        const r_type ux = checked::cast<result_base_type>(base_value(u));
        if(ux.exception())
            dispatch<exception_policy>(ux);
        const r_type r = checked::left_shift(tx.m_r, ux.m_r);
        if(r.exception())
            dispatch<exception_policy>(r);
        return r;
    }
public:
    using type = safe_base<
            result_base_type,
            r_interval.l.exception()
                ? std::numeric_limits<result_base_type>::min()
                : static_cast<result_base_type>(r_interval.l),
            r_interval.u.exception()
                ? std::numeric_limits<result_base_type>::max()
                : static_cast<result_base_type>(r_interval.u),
            promotion_policy,
            exception_policy
        >;

    constexpr static type return_value(const T & t, const U & u){
        return type(
            return_value(
                t,
                u,
                std::integral_constant<bool, exception_possible()>()
            ),
            typename type::skip_validation()
        );
    }
};

template<class T, class U>
typename boost::lazy_enable_if_c<
    // handle safe<T> << int, int << safe<U>, safe<T> << safe<U>
    // exclude std::ostream << ...
    (! std::is_base_of<std::ios_base, T>::value)
    && (is_safe<T>::value || is_safe<U>::value),
    left_shift_result<T, U>
>::type
constexpr operator<<(const T & t, const U & u){
    // INT13-CPP
    // C++ standards document N4618 & 5.8.2
    static_assert(
        std::numeric_limits<T>::is_integer, "shifted value must be an integer"
    );
    static_assert(
        std::numeric_limits<U>::is_integer, "shift amount must be an integer"
    );
    return left_shift_result<T, U>::return_value(t, u);
}

template<class T, class U>
typename std::enable_if<
    is_safe<T>::value || is_safe<U>::value,
    T
>::type
constexpr operator<<=(T & t, const U & u){
    t = static_cast<T>(t << u);
    return t;
}

// right shift
template<class T, class U>
struct right_shift_result {
    using promotion_policy = typename common_promotion_policy<T, U>::type;
    using result_base_type =
        typename promotion_policy::template right_shift_result<T, U>::type;

    using r_type = checked_result<result_base_type>;
    using r_interval_type = interval<r_type>;

    constexpr static const r_interval_type t_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::max()))
    };

    constexpr static const r_interval_type u_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::max()))
    };

/*
    static_assert(t_interval.u >= 0, "negative values can't be shifted");
    static_assert(u_interval.l <= 0, "shift count can't be negative");
    static_assert(u_interval.l <= std::numeric_limits<T>::digits,
        "shift count can't exceed argument size"
    );
*/

    constexpr static const r_interval_type r_interval = (t_interval >> u_interval);

    constexpr static bool exception_possible(){
        return
            // note undesirable coupling with checked::shift right here !
            u_interval.u > checked_result<result_base_type>(
                    std::numeric_limits<result_base_type>::digits
                )
            || t_interval.l < checked_result<result_base_type>(0)
            || u_interval.l < checked_result<result_base_type>(0)
            || r_interval.l.exception()
            || r_interval.u.exception();
    }

    // if exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        return
            static_cast<result_base_type>(base_value(t))
            >> static_cast<result_base_type>(base_value(u));
    }

    // exception possible
    using exception_policy = typename common_exception_policy<T, U>::type;

    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        const r_type tx = checked::cast<result_base_type>(base_value(t));
        if(tx.exception())
            dispatch<exception_policy>(tx);
        const r_type ux = checked::cast<result_base_type>(base_value(u));
        if(ux.exception())
            dispatch<exception_policy>(ux);
        const r_type r = checked::right_shift(tx.m_r, ux.m_r);
        if(r.exception())
            dispatch<exception_policy>(r);
        return r;
    }
public:
    using type = safe_base<
            result_base_type,
            r_interval.l.exception()
                ? std::numeric_limits<result_base_type>::min()
                : static_cast<result_base_type>(r_interval.l),
            r_interval.u.exception()
                ? std::numeric_limits<result_base_type>::max()
                : static_cast<result_base_type>(r_interval.u),
            promotion_policy,
            exception_policy
        >;

    constexpr static type return_value(const T & t, const U & u){
        return type(
            return_value(
                t,
                u,
                std::integral_constant<bool, exception_possible()>()
            ),
            typename type::skip_validation()
        );
    }
};

template<class T, class U>
typename boost::lazy_enable_if_c<
    (! std::is_base_of<std::ios_base, T>::value)
    && (is_safe<T>::value || is_safe<U>::value),
    right_shift_result<T, U>
>::type
constexpr operator>>(const T & t, const U & u){
    // INT13-CPP
    static_assert(
        std::numeric_limits<T>::is_integer, "shifted value must be an integer"
    );
    static_assert(
        std::numeric_limits<U>::is_integer, "shift amount must be an integer"
    );
    return right_shift_result<T, U>::return_value(t, u);
}

template<class T, class U>
typename std::enable_if<
    is_safe<T>::value || is_safe<U>::value,
    T
>::type
constexpr operator>>=(T & t, const U & u){
    t = static_cast<T>(t >> u);
    return t;
}

/////////////////////////////////////////////////////////////////
// bitwise operators

template<typename T>
constexpr T round_out(const T & t){
    if(t >= 0){
        const std::uint8_t sb = utility::significant_bits(t);
        return (sb < sizeof(T) * 8)
            ? (1ul << sb) - 1
            : std::numeric_limits<T>::max();
    }
    else{
        const std::uint8_t sb = utility::significant_bits(~t);
        return (sb < sizeof(T) * 8)
            ? ~((1ul << sb) - 1)
            : std::numeric_limits<T>::min();
    }
}

// operator |
template<class T, class U>
struct bitwise_or_result {
private:
    using promotion_policy = typename common_promotion_policy<T, U>::type;
    using result_base_type =
        typename promotion_policy::template bitwise_or_result<T, U>::type;

    using r_type = checked_result<result_base_type>;
    using r_interval_type = interval<r_type>;

    constexpr static const r_interval_type t_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::max()))
    };

    constexpr static const r_interval_type u_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::max()))
    };

    // get the union of the intervals.
    // note unusual logic here.  This makes use of the fact that the results
    // of the comparison are not booleans but rather boost::logic::tribool.
    // for more information see documenation on boost.tribool
    constexpr static const r_interval_type r1{
        // if comparison is defined and true
          t_interval.l < u_interval.l
        ? t_interval.l
        // if comparison is defined and false
        : !(t_interval.l < u_interval.l)
        ? u_interval.l
        // if comparison is undefined, note this fact
        : r_type(safe_numerics_error::domain_error),
          t_interval.u < u_interval.u
        ? t_interval.u
        : u_interval.u < t_interval.u
        ? u_interval.u
        : r_type(safe_numerics_error::domain_error)
    };

    // extend them to include all bit set
    constexpr static const r_interval_type r_interval{
        (r1.l.exception())
            ? r1.l
            : r_type(round_out(static_cast<result_base_type>(r1.l))),
        (r1.u.exception())
            ? r1.u
            : r_type(round_out(static_cast<result_base_type>(r1.u)))
    };

    constexpr static bool exception_possible(){
        return r_interval.l.exception() || r_interval.u.exception();
    }

    // if exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        return
            static_cast<result_base_type>(base_value(t))
            | static_cast<result_base_type>(base_value(u));
    }

    // if exception possible
    using exception_policy = typename common_exception_policy<T, U>::type;

    // exception possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        const r_type tx = checked::cast<result_base_type>(base_value(t));
        if(tx.exception())
            dispatch<exception_policy>(tx);
        const r_type ux = checked::cast<result_base_type>(base_value(u));
        if(ux.exception())
            dispatch<exception_policy>(ux);
        const r_type r = (tx | ux);
        if(!r.exception())
            return static_cast<result_base_type>(r);
        // handle error condition
        dispatch<exception_policy>(r);
        // if we get here, the error has been ignored
        // just get the result the old fashioned way
        return
            static_cast<result_base_type>(tx)
            | static_cast<result_base_type>(ux);
    }
public:
    // lazy_enable_if_c depends on this
    using type = safe_base<
            result_base_type,
            r_interval.l.exception()
                ? std::numeric_limits<result_base_type>::min()
                : static_cast<result_base_type>(r_interval.l),
            r_interval.u.exception()
                ? std::numeric_limits<result_base_type>::max()
                : static_cast<result_base_type>(r_interval.u),
            promotion_policy,
            exception_policy
        >;

    constexpr static type return_value(const T & t, const U & u){
        return type(
            return_value(
                t,
                u,
                std::integral_constant<bool, exception_possible()>()
            ),
            typename type::skip_validation()
        );
    }
};

template<class T, class U>
typename boost::lazy_enable_if_c<
    is_safe<T>::value || is_safe<U>::value,
    bitwise_or_result<T, U>
>::type
constexpr operator|(const T & t, const U & u){
    return bitwise_or_result<T, U>::return_value(t, u);
}

template<class T, class U>
typename std::enable_if<
    is_safe<T>::value || is_safe<U>::value,
    T
>::type
constexpr operator|=(T & t, const U & u){
    t = static_cast<T>(t | u);
    return t;
}

// operator &
template<class T, class U>
struct bitwise_and_result {
private:
    using promotion_policy = typename common_promotion_policy<T, U>::type;
    using result_base_type =
        typename promotion_policy::template bitwise_and_result<T, U>::type;

    using r_type = checked_result<result_base_type>;
    using r_interval_type = interval<r_type>;
    
    constexpr static const r_interval_type t_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::max()))
    };

    constexpr static const r_interval_type u_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::max()))
    };

    // get the intersection of the intervals.
    // note unusual logic here.  This makes use of the fact that the results
    // of the comparison are not booleans but rather boost::logic::tribool.
    // for more information see documenation on boost.tribool
    constexpr static const r_interval_type r1{
        // if comparison is defined and true
          t_interval.l < u_interval.l
        ? u_interval.l
        // if comparison is defined and false
        : !(t_interval.l < u_interval.l)
        ? t_interval.l
        // if comparison is undefined, note this fact
        : r_type(safe_numerics_error::domain_error),
          t_interval.u < u_interval.u
        ? t_interval.u
        : u_interval.u < t_interval.u
        ? u_interval.u
        : r_type(safe_numerics_error::domain_error)
    };

    // extend them to include all bit set
    constexpr static const r_interval_type r_interval{
        (r1.l.exception())
            ? r1.l
            : r_type(round_out(static_cast<result_base_type>(r1.l))),
        (r1.u.exception())
            ? r1.u
            : r_type(round_out(static_cast<result_base_type>(r1.u)))
    };

    constexpr static bool exception_possible(){
        return r_interval.l.exception() || r_interval.u.exception();
    }

    // if exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        return
            static_cast<result_base_type>(base_value(t))
            & static_cast<result_base_type>(base_value(u));
    }

    // exception possible
    using exception_policy = typename common_exception_policy<T, U>::type;
    
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
    #if 0
        const r_type tx = checked::cast<result_base_type>(base_value(t));
        if(tx.exception())
            dispatch<exception_policy>(tx);
        const r_type ux = checked::cast<result_base_type>(base_value(u));
        if(ux.exception())
            dispatch<exception_policy>(ux);
        const r_type r = (tx & ux);
    #endif
        const r_type r = (tx & ux);
        if(!r.exception())
            return static_cast<result_base_type>(r);
        // handle error condition
        dispatch<exception_policy>(r);
        // if we get here, the error has been ignored
        // just get the result the old fashioned way
        return
            static_cast<result_base_type>(tx)
            & static_cast<result_base_type>(ux)
        ;
    }

public:
    // lazy_enable_if_c depends on this
    using type = safe_base<
        result_base_type,
        r_interval.l.exception()
            ? std::numeric_limits<result_base_type>::min()
            : static_cast<result_base_type>(r_interval.l),
        r_interval.u.exception()
            ? std::numeric_limits<result_base_type>::max()
            : static_cast<result_base_type>(r_interval.u),
        promotion_policy,
        exception_policy
    >;
    
    constexpr static type return_value(const T & t, const U & u){
        return type(
            return_value(
                t,
                u,
                std::integral_constant<bool, exception_possible()>()
            ),
            typename type::skip_validation()
        );
    }
};
    
template<class T, class U>
typename boost::lazy_enable_if_c<
    is_safe<T>::value || is_safe<U>::value,
    bitwise_and_result<T, U>
>::type
constexpr operator&(const T & t, const U & u){
    return bitwise_and_result<T, U>::return_value(t, u);
}

template<class T, class U>
typename std::enable_if<
    is_safe<T>::value || is_safe<U>::value,
    T
>::type
constexpr operator&=(T & t, const U & u){
    t = static_cast<T>(t & u);
    return t;
}

// operator ^
template<class T, class U>
struct bitwise_xor_result {
private:
    using promotion_policy = typename common_promotion_policy<T, U>::type;
    using result_base_type =
        typename promotion_policy::template bitwise_or_result<T, U>::type;

    using r_type = checked_result<result_base_type>;
    using r_interval_type = interval<r_type>;

    constexpr static const r_interval_type t_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<T>::max()))
    };

    constexpr static const r_interval_type u_interval{
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::min())),
        checked::cast<result_base_type>(base_value(std::numeric_limits<U>::max()))
    };

    // get the union of the intervals.
    // note unusual logic here.  This makes use of the fact that the results
    // of the comparison are not booleans but rather boost::logic::tribool.
    // for more information see documenation on boost.tribool
    constexpr static const r_interval_type r1{
        // if comparison is defined and true
          t_interval.l < u_interval.l
        ? t_interval.l
        // if comparison is defined and false
        : !(t_interval.l < u_interval.l)
        ? u_interval.l
        // if comparison is undefined, note this fact
        : r_type(safe_numerics_error::domain_error),
          t_interval.u < u_interval.u
        ? t_interval.u
        : u_interval.u < t_interval.u
        ? u_interval.u
        : r_type(safe_numerics_error::domain_error)
    };

    // extend them to include all bit set
    constexpr static const r_interval_type r_interval{
        (r1.l.exception())
            ? r1.l
            : r_type(round_out(static_cast<result_base_type>(r1.l))),
        (r1.u.exception())
            ? r1.u
            : r_type(round_out(static_cast<result_base_type>(r1.u)))
    };

    constexpr static bool exception_possible(){
        return r_interval.l.exception() || r_interval.u.exception();
    }

    // if exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        return
            static_cast<result_base_type>(base_value(t))
            ^ static_cast<result_base_type>(base_value(u));
    }

    // if exception possible
    using exception_policy = typename common_exception_policy<T, U>::type;

    // exception possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        const r_type tx = checked::cast<result_base_type>(base_value(t));
        const r_type ux = checked::cast<result_base_type>(base_value(u));
        const r_type r = (tx | ux);
        if(!r.exception())
            return static_cast<result_base_type>(r);
        // handle error condition
        dispatch<exception_policy>(r);
        // if we get here, the error has been ignored
        // just get the result the old fashioned way
        return
            static_cast<result_base_type>(tx)
            ^ static_cast<result_base_type>(ux);
    }
public:
    // lazy_enable_if_c depends on this
    using type = safe_base<
            result_base_type,
            r_interval.l.exception()
                ? std::numeric_limits<result_base_type>::min()
                : static_cast<result_base_type>(r_interval.l),
            r_interval.u.exception()
                ? std::numeric_limits<result_base_type>::max()
                : static_cast<result_base_type>(r_interval.u),
            promotion_policy,
            exception_policy
        >;

    constexpr static type return_value(const T & t, const U & u){
        return type(
            return_value(
                t,
                u,
                std::integral_constant<bool, exception_possible()>()
            ),
            typename type::skip_validation()
        );
    }
};

template<class T, class U>
typename boost::lazy_enable_if_c<
    is_safe<T>::value || is_safe<U>::value,
    bitwise_xor_result<T, U>
>::type
constexpr operator^(const T & t, const U & u){
    return bitwise_xor_result<T, U>::return_value(t, u);
}

template<class T, class U>
typename std::enable_if<
    is_safe<T>::value || is_safe<U>::value,
    T
>::type
constexpr operator^=(T & t, const U & u){
    t = static_cast<T>(t ^ u);
    return t;
}

/////////////////////////////////////////////////////////////////
// stream helpers

template<
    class T,
    T Min,
    T Max,
    class P, // promotion polic
    class E  // exception policy
>
template<
    class CharT,
    class Traits
>
void safe_base<T, Min, Max, P, E>::output(
    std::basic_ostream<CharT, Traits> & os
) const {
    os << (
        (std::is_same<T, signed char>::value
        || std::is_same<T, unsigned char>::value
        || std::is_same<T, wchar_t>::value
        ) ?
            static_cast<int>(m_t)
        :
            m_t
    );
}

template<
    class T,
    T Min,
    T Max,
    class P, // promotion polic
    class E  // exception policy
>
template<
    class CharT,
    class Traits
>
void safe_base<T, Min, Max, P, E>::input(
    std::basic_istream<CharT, Traits> & is
){
    if(std::is_same<T, signed char>::value
    || std::is_same<T, unsigned char>::value
    || std::is_same<T, wchar_t>::value
    ){
        int x;
        is >> x;
        m_t = validated_cast(x);
    }
    else{
        is >> m_t;
        validated_cast(m_t);
    }
    if(is.fail()){
        boost::numeric::dispatch<E>(
            boost::numeric::safe_numerics_error::domain_error,
            "error in file input"
        );
    }
}

} // numeric
} // boost

#endif // BOOST_NUMERIC_SAFE_BASE_OPERATIONS_HPP
