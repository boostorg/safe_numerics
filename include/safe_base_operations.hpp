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
#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>

#include <boost/core/enable_if.hpp> // lazy_enable_if
#include <boost/integer.hpp>
#include <boost/logic/tribool.hpp>

#include "checked_result.hpp"
#include "safe_base.hpp"
#include "safe_compare.hpp"
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

template<typename R, typename E>
struct validate_detail {
    struct exception_possible {
        template<typename T>
        constexpr static R return_value(
            const T & t,
            const interval<R> & r_interval
        ){
            // INT08-C
            const checked_result<R> r = r_interval.includes(t) ?
                checked::cast<R>(t)
            :
                checked_result<R>(
                    safe_numerics_error::domain_error,
                    "Value out of range for this safe type"
                )
            ;
            dispatch<E>(r);
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

// convert to a builtin integer type from a safe type
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

    constexpr static const interval<typename base_type<T>::type> t_interval{
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    };

    constexpr static const interval<typename base_type<U>::type> u_interval{
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    };
    // when we add the temporary intervals above, we'll get a new interval
    // with the correct range for the sum !
    constexpr static const interval<checked_result<result_base_type>> r_interval =
        add<result_base_type>(t_interval, u_interval);

    constexpr static bool exception_possible() {
        return r_interval.l.exception() || r_interval.u.exception();
    }

    using exception_policy = typename common_exception_policy<T, U>::type;

    struct safe_type {
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
        constexpr static const type make(const result_base_type & t){
            return type(t, typename safe_type::type::skip_validation());
        }
    };

    struct unsafe_type {
        using type = result_base_type;
        constexpr static const type make(const result_base_type & t){
            return t;
        }
    };

    using type_helper = typename boost::mpl::if_c<
        std::numeric_limits<result_base_type>::is_integer,
        safe_type,
        unsafe_type
    >::type;

    constexpr static result_base_type
    add(const T & t, const U & u){
        return
            static_cast<result_base_type>(base_value(t))
            + static_cast<result_base_type>(base_value(u))
        ;
    }

    // exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        static_assert(! exception_possible(), "no runtime check");
        return add(t,u);
    }

    // exception possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        static_assert(exception_possible(), "implement runtime check");
        checked_result<result_base_type> r = checked::add<result_base_type>(
            base_value(t),
            base_value(u)
        );
        if(!r.exception())
            return static_cast<result_base_type>(r);
        // handle error condition
        dispatch<exception_policy>(r);
        // if we get here, the error has been ignored
        // just get the result the old fashioned way
        return add(t, u);
    }

public:
    using type = typename type_helper::type;

    constexpr static type return_value(const T & t, const U & u){
        return type_helper::make( return_value(
            t,
            u,
            typename std::integral_constant<bool, exception_possible()>()
        ));
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

    constexpr static const interval<typename base_type<T>::type> t_interval{
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    };

    constexpr static const interval<typename base_type<U>::type> u_interval{
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    };

    // when we subtract the temporary intervals above, we'll get a new interval
    // with the correct range for the difference !
    constexpr static const interval<checked_result<result_base_type>> r_interval =
        subtract<result_base_type>(t_interval, u_interval);

    constexpr static bool exception_possible() {
        return r_interval.l.exception() || r_interval.u.exception();
    }

    using exception_policy = typename common_exception_policy<T, U>::type;

    struct safe_type {
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
        constexpr static const type make(const result_base_type & t){
            return type(t, typename safe_type::type::skip_validation());
        }
    };
    struct unsafe_type {
        using type = result_base_type;
        constexpr static const type make(const result_base_type & t){
            return t;
        }
    };
    using type_helper = typename boost::mpl::if_c<
        std::numeric_limits<result_base_type>::is_integer,
        safe_type,
        unsafe_type
    >::type;

    constexpr static result_base_type
    subtract(const T & t, const U & u){
        return
            static_cast<result_base_type>(base_value(t))
            - static_cast<result_base_type>(base_value(u))
        ;
    }

    // exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        static_assert(! exception_possible(), "no runtime check");
        return subtract(t, u);
    }

    // exception possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        static_assert(exception_possible(), "implement runtime check");
        checked_result<result_base_type> r = checked::subtract<result_base_type>(
            base_value(t),
            base_value(u)
        );
        if(!r.exception())
            return static_cast<result_base_type>(r);
        dispatch<exception_policy>(r);
        return subtract(t, u);
    }
public:
    using type = typename type_helper::type;

    constexpr static type return_value(const T & t, const U & u){
        return type_helper::make( return_value(
            t,
            u,
            typename std::integral_constant<bool, exception_possible()>()
        ));
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

    using exception_policy = typename common_exception_policy<T, U>::type;
    struct safe_type {
        constexpr static const interval<typename base_type<T>::type> t_interval{
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };

        constexpr static const interval<typename base_type<U>::type> u_interval{
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

        constexpr static const interval<checked_result<result_base_type>> r_interval =
            multiply<result_base_type>(t_interval, u_interval);

        constexpr static bool exception_possible() {
            return r_interval.l.exception() || r_interval.u.exception();
        }

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
        constexpr static const type make(const result_base_type & t){
            return type(t, typename safe_type::type::skip_validation());
        }
    };
    struct unsafe_type {
        using type = result_base_type;
        constexpr static const type make(const result_base_type & t){
            return t;
        }
        constexpr static bool exception_possible() {
            return false;
        }
    };

    using type_helper = typename boost::mpl::if_c<
        std::numeric_limits<result_base_type>::is_integer,
        safe_type,
        unsafe_type
    >::type;

    constexpr static bool exception_possible() {
        return type_helper::exception_possible();
    }

    constexpr static result_base_type
    multiply(const T & t, const U & u){
        return
            static_cast<result_base_type>(base_value(t))
            * static_cast<result_base_type>(base_value(u))
        ;
    }

    // exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        static_assert(! exception_possible(), "no runtime check");
        return multiply(t, u);
    }

    // exception possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        static_assert(exception_possible(), "implement runtime check");
        checked_result<result_base_type> r = checked::multiply<result_base_type>(
            base_value(t),
            base_value(u)
        );
        if(!r.exception())
            return static_cast<result_base_type>(r);
        dispatch<exception_policy>(r);
        return multiply(t, u);
    }
public:
    using type = typename type_helper::type;

    constexpr static type return_value(const T & t, const U & u){
        return type_helper::make( return_value(
            t,
            u,
            typename std::integral_constant<bool, exception_possible()>()
        ));
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
    using result_base_type = typename promotion_policy::template division_result<T, U>::type;
    using t_base = typename base_type<T>::type;
    using u_base = typename base_type<U>::type;

    using exception_policy = typename common_exception_policy<T, U>::type;
    
    struct safe_type {
        constexpr static const interval<t_base> t_interval{
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };

        constexpr static const interval<u_base> u_interval{
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };
        static_assert(
            u_interval != interval<result_base_type>(0, 0),
            "Cannot divide by zero"
        );
        // we know that the denominator is not equal to zero

        constexpr static const interval<checked_result<result_base_type>> r_interval
            = divide<result_base_type>(t_interval, u_interval);

        static_assert(! r_interval.l.exception(), "unexpected negative overflow");
        static_assert(! r_interval.u.exception(), "unexpected positive overflow");
        // the result interval is definitely valid

        constexpr static bool exception_possible() {
            // section needs some work. issues:
            // cast 1) t = 1, u = -128
            //    we know the answer so we returned false.
            //    But when the answer is re-calculated at runtime
            //    we get an exception when we try to calculate
            //   -(-128) in preparate.
            /*
            if(r_interval.l == r_interval.u)
                // so it's not going to throw an exception
                return false;
            */
            return
                // these should really be handled in checked
                u_interval.includes(0)
                || (
                    u_interval.includes(-1)
                    && t_interval.includes(base_value(std::numeric_limits<T>::min()))
                )
            ;
        }
        using type = safe_base<
            result_base_type,
            static_cast<result_base_type>(r_interval.l),
            static_cast<result_base_type>(r_interval.u),
            promotion_policy,
            exception_policy
        >;
        constexpr static type make(const result_base_type & t){
            return type(t, typename safe_type::type::skip_validation());
        }
    };
    struct unsafe_type {
        using type = result_base_type;
        constexpr static const type make(const result_base_type & t){
            return t;
        }
        constexpr static bool exception_possible() {
            return false;
        }
    };

    using type_helper = typename boost::mpl::if_c<
        std::numeric_limits<result_base_type>::is_integer,
        safe_type,
        unsafe_type
    >::type;

    constexpr static bool exception_possible() {
        return type_helper::exception_possible();
    }

    constexpr static result_base_type
    divide(const T & t, const U & u){
        return
            static_cast<result_base_type>(base_value(t))
            / static_cast<result_base_type>(base_value(u))
        ;
    }

    constexpr static int bits = std::min(
        std::numeric_limits<std::uintmax_t>::digits,
        std::max(std::initializer_list<int>{
            std::numeric_limits<result_base_type>::digits,
            std::numeric_limits<t_base>::digits,
            std::numeric_limits<u_base>::digits
        }) + (std::numeric_limits<result_base_type>::is_signed ? 1 : 0)
    );

    using temp_base = typename boost::mpl::if_c<
        std::numeric_limits<result_base_type>::is_signed,
        typename boost::int_t<bits>::least,
        typename boost::uint_t<bits>::least
    >::type;

    // exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        return divide(t, u);
    }

    // exception possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        checked_result<temp_base> r = checked::divide<temp_base>(
            base_value(t),
            base_value(u)
        );
        if(!r.exception())
            return static_cast<result_base_type>(r);
        dispatch<exception_policy>(r);
        return divide(t, u);
    }
public:
    using type = typename type_helper::type;

    constexpr static type return_value(const T & t, const U & u){
        return type_helper::make( return_value(
            t,
            u,
            typename std::integral_constant<bool, exception_possible()>()
        ));
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

    using exception_policy = typename common_exception_policy<T, U>::type;
    struct safe_type {
        constexpr static const interval<typename base_type<T>::type> t_interval{
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };

        constexpr static const interval<typename base_type<U>::type> u_interval{
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

        static_assert(
            u_interval != interval<result_base_type>(0, 0),
            "Cannot divide by zero"
        );
        // we know that the denominator is not equal to zero

        constexpr static const interval<checked_result<result_base_type>> r_interval = modulus<result_base_type>(t_interval, u_interval);

        static_assert(! r_interval.l.exception(), "unexpected negative overflow");
        static_assert(! r_interval.u.exception(), "unexpected positive overflow");
        // the result interval is definitely valid

        constexpr static bool exception_possible() {
            #if 0 // see divide above
            // result is definitely known a compile time
            if(r_interval.l == r_interval.u)
                // so it's not going to throw an exception
                return false;
            #endif
            return
                u_interval.includes(0)
                || (
                    u_interval.includes(-1)
                    && t_interval.includes(base_value(std::numeric_limits<T>::min()))
                )
            ;
        }

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
        constexpr static const type make(const result_base_type & t){
            return type(t, typename safe_type::type::skip_validation());
        }
    };
    struct unsafe_type {
        using type = result_base_type;
        constexpr static const type make(const result_base_type & t){
            return t;
        }
        constexpr static bool exception_possible() {
            return false;
        }
    };
    using type_helper = typename boost::mpl::if_c<
        std::numeric_limits<result_base_type>::is_integer,
        safe_type,
        unsafe_type
    >::type;

    constexpr static bool exception_possible() {
        return type_helper::exception_possible();
    }

    constexpr static result_base_type
    modulus(const T & t, const U & u){
        return
            static_cast<result_base_type>(base_value(t))
            % static_cast<result_base_type>(base_value(u))
        ;
    }

    // exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        static_assert(! exception_possible(), "no runtime check");
        return modulus(t, u);
    }

    // exception possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        static_assert(exception_possible(), "implement runtime check");
        checked_result<result_base_type> r = checked::modulus<result_base_type>(
            base_value(t),
            base_value(u)
        );
        if(!r.exception())
            return static_cast<result_base_type>(r);
        dispatch<exception_policy>(r);
        return static_cast<result_base_type>(r);
    }
public:
    using type = typename type_helper::type;

    constexpr static type return_value(const T & t, const U & u){
        return type_helper::make( return_value(
            t,
            u,
            typename std::integral_constant<bool, exception_possible()>()
        ));
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

template<class T, class U>
struct less_than_result {
private:
    using promotion_policy = typename common_promotion_policy<T, U>::type;

    using result_base_type =
        typename promotion_policy::template comparison_result<T, U>::type;

   constexpr static const interval<typename base_type<T>::type> t_interval{
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    };

    constexpr static const interval<typename base_type<U>::type> u_interval{
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    };

    constexpr static const boost::tribool tb = t_interval < u_interval;

    using exception_policy = typename common_exception_policy<T, U>::type;

    constexpr static bool
    return_value(const T & t, const U & u, std::false_type){
        return false;
    }
    constexpr static bool
    return_value(const T & t, const U & u, std::true_type){
        return true;
    }
    constexpr static bool
    return_value(const T & t, const U & u, logic::detail::indeterminate_t){
        const checked_result<bool> r = checked::less_than<result_base_type>(
            base_value(t),
            base_value(u)
        );
        if(!r.exception())
            return r;
        dispatch<exception_policy>(r);
        return safe_compare::less_than(base_value(t), base_value(u));
    }

public:
    constexpr static bool
    return_value(const T & t, const U & u){
        return return_value(
            t,
            u,
            typename boost::mpl::if_c<
                static_cast<const bool>(!tb),
                std::false_type,
            typename boost::mpl::if_c<
                static_cast<const bool>(tb),
                std::true_type,
            logic::detail::indeterminate_t
            >::type
            >::type()
        );
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

template<class T, class U>
struct equal_result {
private:
    using promotion_policy = typename common_promotion_policy<T, U>::type;

    using result_base_type =
        typename promotion_policy::template comparison_result<T, U>::type;

   constexpr static const interval<typename base_type<T>::type> t_interval{
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    };

    constexpr static const interval<typename base_type<U>::type> u_interval{
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    };

    constexpr static const interval<checked_result<result_base_type>>
        r_interval = intersection<result_base_type>(t_interval, u_interval);

    constexpr static bool exception_possible() {
        // if intersection is not null, equality will have to evaluated
        // at runtime
        return  ! r_interval.l.exception() && ! r_interval.u.exception();
    }

    using exception_policy = typename common_exception_policy<T, U>::type;

    // exception not possible
    constexpr static bool
    return_value(const T & t, const U & u, std::false_type){
        return false;
    }
    // exception possible
    constexpr static bool return_value(const T & t, const U & u, std::true_type){
        const checked_result<bool> r = checked::equal<result_base_type>(
            base_value(t),
            base_value(u)
        );
        if(!r.exception())
            return r;
        dispatch<exception_policy>(r);
        return boost::numeric::safe_compare::equal(base_value(t), base_value(u));
    }

public:
    constexpr static bool return_value(const T & t, const U & u){
        return return_value(
            t,
            u,
            typename std::integral_constant<bool, exception_possible()>()
        );
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

    constexpr static const interval<typename base_type<T>::type> t_interval{
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    };

    constexpr static const interval<typename base_type<U>::type> u_interval{
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    };

    static_assert(t_interval.u >= 0, "negative values can't be shifted");
    static_assert(u_interval.u >= 0, "shift count can't be negative");
    static_assert(u_interval.l <= std::numeric_limits<T>::digits,
        "shift count can't exceed argument size"
    );

    constexpr static const interval<checked_result<result_base_type>> r_interval {
        left_shift<result_base_type>(t_interval, u_interval)
    };

    constexpr static bool exception_possible() {
        return r_interval.l.exception() || r_interval.u.exception();
    }

    using exception_policy = typename common_exception_policy<T, U>::type;

    constexpr static result_base_type
    left_shift(const T & t, const U & u){
        return
            static_cast<result_base_type>(base_value(t))
            << static_cast<result_base_type>(base_value(u))
        ;
    }

    // exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        static_assert(! exception_possible(), "no runtime check");
        return left_shift(t, u);
    }

    // exception possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        static_assert(exception_possible(), "implement runtime check");
        checked_result<result_base_type> r = checked::left_shift<result_base_type>(
            base_value(t),
            base_value(u)
        );
        if(!r.exception())
            return static_cast<result_base_type>(r);
        dispatch<exception_policy>(r);
        return left_shift(t, u);
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
        return return_value(
            t,
            u,
            typename std::integral_constant<bool, exception_possible()>()
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

    constexpr static const interval<typename base_type<T>::type> t_interval{
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    };

    constexpr static const interval<typename base_type<U>::type> u_interval{
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    };

    static_assert(t_interval.u >= 0, "negative values can't be shifted");
    static_assert(u_interval.l <= 0, "shift count can't be negative");
    static_assert(u_interval.l <= std::numeric_limits<T>::digits,
        "shift count can't exceed argument size"
    );

    constexpr static const interval<checked_result<result_base_type>> r_interval{
        right_shift<result_base_type>(t_interval, u_interval)
    };

    constexpr static bool exception_possible() {
        return r_interval.l.exception() || r_interval.u.exception();
    }

    using exception_policy = typename common_exception_policy<T, U>::type;

    constexpr static result_base_type
    right_shift(const T & t, const U & u){
        return
            static_cast<result_base_type>(base_value(t))
            >> static_cast<result_base_type>(base_value(u))
        ;
    }

    // exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        static_assert(! exception_possible(), "no runtime check");
        return right_shift(t, u);
    }

    // exception possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        static_assert(exception_possible(), "implement runtime check");
        checked_result<result_base_type> r = checked::right_shift<result_base_type>(
            base_value(t),
            base_value(u)
        );
        if(!r.exception())
            return static_cast<result_base_type>(r);
        dispatch<exception_policy>(r);
        return right_shift(t, u);
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
        return return_value(
            t,
            u,
            typename std::integral_constant<bool, exception_possible()>()
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

// operator |
template<class T, class U>
struct bitwise_or_result {
private:
    using promotion_policy = typename common_promotion_policy<T, U>::type;
    using result_base_type =
        typename promotion_policy::template bitwise_or_result<T, U>::type;

    constexpr static const interval<typename base_type<T>::type> t_interval{
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    };

    constexpr static const interval<typename base_type<U>::type> u_interval{
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    };
    // when we or the temporary intervals above, we'll get a new interval
    // with the correct range for the sum !
    constexpr static const interval<checked_result<result_base_type>> r_interval =
        bitwise_or<result_base_type>(t_interval, u_interval);

    constexpr static bool exception_possible() {
        return r_interval.l.exception() || r_interval.u.exception();
    }
    using exception_policy = typename common_exception_policy<T, U>::type;

    struct safe_type {
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
        constexpr static const type make(const result_base_type & t){
            return type(t, typename safe_type::type::skip_validation());
        }
    };
    struct unsafe_type {
        using type = result_base_type;
        constexpr static const type make(const result_base_type & t){
            return t;
        }
    };
    using type_helper = typename boost::mpl::if_c<
        std::numeric_limits<result_base_type>::is_integer,
        safe_type,
        unsafe_type
    >::type;

    constexpr static result_base_type
    bitwise_or(const T & t, const U & u){
        return
            static_cast<result_base_type>(base_value(t))
            | static_cast<result_base_type>(base_value(u));
    }

    // exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        static_assert(! exception_possible(), "no runtime check");
        return bitwise_or(t, u);
    }

    // exception possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        static_assert(exception_possible(), "implement runtime check");
        checked_result<result_base_type> r = checked::bitwise_or<result_base_type>(
            base_value(t),
            base_value(u)
        );
        if(!r.exception())
            return static_cast<result_base_type>(r);
        dispatch<exception_policy>(r);
        return bitwise_or(t, u);
    }
public:
    // lazy_enable_if_c depends on this
    using type = typename type_helper::type;

    constexpr static type return_value(const T & t, const U & u){
        return type_helper::make( return_value(
            t,
            u,
            typename std::integral_constant<bool, exception_possible()>()
        ));
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
    using promotion_policy = typename common_promotion_policy<T, U>::type;
    using result_base_type =
        typename promotion_policy::template bitwise_and_result<T, U>::type;

    constexpr static const interval<typename base_type<T>::type> t_interval{
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    };

    constexpr static const interval<typename base_type<U>::type> u_interval{
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    };
    // when we and the temporary intervals above, we'll get a new interval
    // with the correct range for the sum !
    constexpr static const interval<checked_result<result_base_type>> r_interval =
        bitwise_and<result_base_type>(t_interval, u_interval);

    constexpr static bool exception_possible() {
        return r_interval.l.exception() || r_interval.u.exception();
    }
    using exception_policy = typename common_exception_policy<T, U>::type;
    struct safe_type {
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
        constexpr static const type make(const result_base_type & t){
            return type(t, typename safe_type::type::skip_validation());
        }
    };
    struct unsafe_type {
        using type = result_base_type;
        constexpr static const type make(const result_base_type & t){
            return t;
        }
    };
    using type_helper = typename boost::mpl::if_c<
        std::numeric_limits<result_base_type>::is_integer,
        safe_type,
        unsafe_type
    >::type;

    constexpr static result_base_type
    bitwise_and(const T & t, const U & u){
        return
            static_cast<result_base_type>(base_value(t))
            & static_cast<result_base_type>(base_value(u))
        ;
    }
    // exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        static_assert(! exception_possible(), "no runtime check");
        return bitwise_and(t, u);
    }

    // exception possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        static_assert(exception_possible(), "implement runtime check");
        checked_result<result_base_type> r = checked::bitwise_and<result_base_type>(
            base_value(t),
            base_value(u)
        );
        dispatch<exception_policy>(r);
        return bitwise_and(t, u);
    }
public:
    using type = typename type_helper::type;

    constexpr static type return_value(const T & t, const U & u){
        return type_helper::make( return_value(
            t,
            u,
            typename std::integral_constant<bool, exception_possible()>()
        ));
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
        typename promotion_policy::template bitwise_xor_result<T, U>::type;

    constexpr static const interval<typename base_type<T>::type> t_interval{
        base_value(std::numeric_limits<T>::min()),
        base_value(std::numeric_limits<T>::max())
    };

    constexpr static const interval<typename base_type<U>::type> u_interval{
        base_value(std::numeric_limits<U>::min()),
        base_value(std::numeric_limits<U>::max())
    };
    // when we xor the temporary intervals above, we'll get a new interval
    // with the correct range for the sum !
    constexpr static const interval<checked_result<result_base_type>> r_interval =
        bitwise_xor<result_base_type>(t_interval, u_interval);

    constexpr static bool exception_possible() {
        return r_interval.l.exception() || r_interval.u.exception();
    }
    using exception_policy = typename common_exception_policy<T, U>::type;
    struct safe_type {
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
        constexpr static const type make(const result_base_type & t){
            return type(t, typename safe_type::type::skip_validation());
        }
    };

    struct unsafe_type {
        using type = result_base_type;
        constexpr static const type make(const result_base_type & t){
            return t;
        }
    };
    using type_helper = typename boost::mpl::if_c<
        std::numeric_limits<result_base_type>::is_integer,
        safe_type,
        unsafe_type
    >::type;

    constexpr static result_base_type
    bitwise_xor(const T & t, const U & u){
        return
            static_cast<result_base_type>(base_value(t))
            ^ static_cast<result_base_type>(base_value(u));
    }

    // exception possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::true_type){
        static_assert(exception_possible(), "implement runtime check");
        checked_result<result_base_type> r = checked::bitwise_xor<result_base_type>(
            base_value(t),
            base_value(u)
        );
        if(!r.exception())
            return static_cast<result_base_type>(r);
        dispatch<exception_policy>(r);
        return bitwise_xor(t, u);
    }

    // exception not possible
    constexpr static result_base_type
    return_value(const T & t, const U & u, std::false_type){
        static_assert(! exception_possible(), "no runtime check");
        return bitwise_xor(t, u);
    }
public:
    using type = typename type_helper::type;

    constexpr static type return_value(const T & t, const U & u){
        return type_helper::make( return_value(
            t,
            u,
            typename std::integral_constant<bool, exception_possible()>()
        ));
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
