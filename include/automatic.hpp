#ifndef BOOST_NUMERIC_AUTOMATIC_HPP
#define BOOST_NUMERIC_AUTOMATIC_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// policy which creates results types equal to that of C++ promotions.
// Using the policy will permit the program to build and run in release
// mode which is identical to that in debug mode except for the fact
// that errors aren't trapped.

#include <limits>
#include <cstdint>     // (u)intmax_t,
#include <type_traits> // true_type, false_type, is_same
#include <boost/mpl/if.hpp>

#include "safe_common.hpp"
#include "checked_result.hpp"
#include "interval.hpp"
#include "safe_range.hpp"

namespace boost {
namespace numeric {

struct automatic {
    // section 4.13 integer conversion rank
    template<class T>
    using rank =
        typename boost::mpl::if_c<
            sizeof(char) == sizeof(T),
            std::integral_constant<int, 1>,
        typename boost::mpl::if_c<
            sizeof(short) == sizeof(T),
            std::integral_constant<int, 2>,
        typename boost::mpl::if_c<
            sizeof(int) == sizeof(T),
            std::integral_constant<int, 3>,
        typename boost::mpl::if_c<
            sizeof(long) == sizeof(T),
            std::integral_constant<int, 4>,
        typename boost::mpl::if_c<
            sizeof(long long) == sizeof(T),
            std::integral_constant<int, 5>,
            void
        >::type >::type >::type >::type >::type;

    // note presumption that T & U don't have he same sign
    // if that's not true, these won't work
    template<class T, class U>
    using select_signed = typename boost::mpl::if_c<
        std::numeric_limits<T>::is_signed,
        T,
        U
    >::type;

    template<class T, class U>
    using select_unsigned = typename boost::mpl::if_c<
        std::numeric_limits<T>::is_signed,
        U,
        T
    >::type;

    template<class T, class U>
    using calculate_max_t =
        typename boost::mpl::if_c<
            // clause 1 - if both operands have the same sign
            std::numeric_limits<T>::is_signed
            == std::numeric_limits<U>::is_signed,
            // use that sign
            typename boost::mpl::if_c<
                std::numeric_limits<T>::is_signed,
                std::intmax_t,
                std::uintmax_t
            >::type,
        // clause 2 - otherwise if the rank of the unsigned type exceeds
        // the rank of the of the maximum signed type
        typename boost::mpl::if_c<
            (rank< select_unsigned<T, U>>::value
            > rank< std::intmax_t >::value),
            // use unsigned type
            std::uintmax_t,
        // clause 3 - otherwise if the type of the signed integer type can
        // represent all the values of the unsigned type
        typename boost::mpl::if_c<
            std::numeric_limits< std::intmax_t >::digits >=
            std::numeric_limits< select_unsigned<T, U> >::digits,
            // use signed type
            std::intmax_t,
        // clause 4 - otherwise use unsigned version of the signed type
            std::uintmax_t
        >::type >::type >::type;

    template<typename T, T Min, T Max, class P, class E>
    struct defer_signed_lazily {
        using type = boost::numeric::safe_signed_range<Min, Max, P, E>;
    };

    template<typename T, T Min, T Max, class P, class E>
    struct defer_unsigned_lazily {
        using type = boost::numeric::safe_unsigned_range<Min, Max, P, E>;
    };

    template<typename T, T Min, T Max, class P, class E>
    using safe_range =
        typename boost::mpl::if_<
            std::is_signed<T>,
            defer_signed_lazily<T, Min, Max, P, E>,
            defer_unsigned_lazily<T, Min, Max, P, E>
        >::type;

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U, typename P, typename E>
    struct addition_result {
        typedef typename base_type<T>::type base_type_t;
        typedef typename base_type<U>::type base_type_u;

        SAFE_NUMERIC_CONSTEXPR static const interval<base_type_t> t = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };

        SAFE_NUMERIC_CONSTEXPR static const interval<base_type_u> u = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

        typedef  calculate_max_t<T, U> max_t;

        SAFE_NUMERIC_CONSTEXPR static const interval< max_t> r
            = operator+<max_t>(t, u);

        SAFE_NUMERIC_CONSTEXPR static const max_t newmin = r.l.no_exception() ?
            static_cast<max_t>(r.l)
        :
            std::numeric_limits<max_t>::min()
        ;

        SAFE_NUMERIC_CONSTEXPR static const max_t newmax = r.u.no_exception() ?
            static_cast<max_t>(r.u)
        :
            std::numeric_limits<max_t>::max()
        ;

        // typedef typename print<max_t>::type p_max_t;
        // typedef typename print<std::integral_constant<max_t, newmin>>::type p_newmin;
        // typedef typename print<std::integral_constant<max_t, newmax>>::type p_newmax;

        static_assert(
            newmin < newmax,
            "new minimumum must be less than new maximum"
        );

        static_assert(
            std::numeric_limits<max_t>::min() >= 0 || std::is_signed<max_t>::value,
            "newmin < 0 and unsigned can't happen"
        );

        typedef typename safe_range<
            max_t,
            newmin,
            newmax,
            P,
            E
        >::type type;
    };

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U, typename P, typename E>
    struct subtraction_result {
        typedef typename base_type<T>::type base_type_t;
        typedef typename base_type<U>::type base_type_u;

        SAFE_NUMERIC_CONSTEXPR static const interval<base_type_t> t = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };

        SAFE_NUMERIC_CONSTEXPR static const interval<base_type_u> u = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

        typedef calculate_max_t<T, U> max_t;
        // typedef typename print<max_t>::type p_max_t;

        SAFE_NUMERIC_CONSTEXPR static const interval< max_t> r
            = operator-<max_t>(t, u);

        SAFE_NUMERIC_CONSTEXPR static const max_t newmin = r.l.no_exception() ?
            static_cast<max_t>(r.l)
        :
            std::numeric_limits<max_t>::min()
        ;
        // typedef print<std::integral_constant<max_t, newmin>> p_newmin;

        SAFE_NUMERIC_CONSTEXPR static const max_t newmax = r.u.no_exception() ?
            static_cast<max_t>(r.u)
        :
            std::numeric_limits<max_t>::max()
        ;
        // typedef print<std::integral_constant<max_t, newmax>> p_newmax;

        static_assert(
            newmin < newmax,
            "new minimumum must be less than new maximum"
        );

        static_assert(
            std::numeric_limits<max_t>::min() >= 0 || std::is_signed<max_t>::value,
            "newmin < 0 and unsigned can't happen"
        );

        typedef typename safe_range<
            max_t,
            newmin,
            newmax,
            P,
            E
        >::type type;
    };

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U, typename P, typename E>
    struct multiplication_result {
        typedef typename base_type<T>::type base_type_t;
        typedef typename base_type<U>::type base_type_u;

        SAFE_NUMERIC_CONSTEXPR static const interval<base_type_t> t = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };

        SAFE_NUMERIC_CONSTEXPR static const interval<base_type_u> u = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };

        typedef calculate_max_t<T, U> max_t;
        // typedef print<max_t> p_max_t;

        SAFE_NUMERIC_CONSTEXPR static const interval< max_t> r
            = operator*<max_t>(t, u);

        // typedef print<std::integral_constant<max_t, r.l>> p_r_l;
        // typedef print<std::integral_constant<max_t, r.u>> p_r_u;

        SAFE_NUMERIC_CONSTEXPR static const max_t newmin = r.l.no_exception() ?
            static_cast<max_t>(r.l)
        :
            std::numeric_limits<max_t>::min()
        ;

        SAFE_NUMERIC_CONSTEXPR static const max_t newmax = r.u.no_exception() ?
            static_cast<max_t>(r.u)
        :
            std::numeric_limits<max_t>::max()
        ;

        // typedef print<std::integral_constant<max_t, newmin>> p_newmin;
        // typedef print<std::integral_constant<max_t, newmax>> p_newmax;

        static_assert(
            newmin < newmax,
            "new minimumum must be less than new maximum"
        );

        static_assert(
            std::numeric_limits<max_t>::min() >= 0 || std::is_signed<max_t>::value,
            "newmin < 0 and unsigned can't happen"
        );

        typedef typename safe_range<
            max_t,
            newmin,
            newmax,
            P,
            E
        >::type type;
    };

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U, typename P, typename E>
    struct division_result {
        typedef typename base_type<T>::type base_type_t;
        static_assert(
            std::is_literal_type< interval<base_type_t> >::value,
            "interval<base_type_t> is not literal type"
        );
        typedef typename base_type<U>::type base_type_u;
        static_assert(
            std::is_literal_type< interval<base_type_u> >::value,
            "interval<base_type_u> is not tliteral type"
        );

        constexpr static interval<base_type_t> t(){
            return interval<base_type_t>(
                base_value(std::numeric_limits<T>::min()),
                base_value(std::numeric_limits<T>::max())
            );
        };
        constexpr static interval<base_type_u> u(){
            return interval<base_type_u>(
                base_value(std::numeric_limits<U>::min()),
                base_value(std::numeric_limits<U>::max())
            );
        };

        using max_t = typename boost::mpl::if_c<
            std::numeric_limits<base_type_t>::is_signed
            || std::numeric_limits<base_type_u>::is_signed,
            std::intmax_t,
            std::uintmax_t
        >::type;

        template<typename Tx>
        SAFE_NUMERIC_CONSTEXPR static const interval<Tx> r_upper(const interval<Tx> & t){
            static_assert(
                std::is_literal_type< interval<Tx> >::value,
                "interval<Tx> is not literal type"
            );
            return interval<Tx>(
                vmax(checked_result<Tx>(1), t.l),
                t.u
            );
        }
        template<typename Tx>
        SAFE_NUMERIC_CONSTEXPR static const interval<Tx> r_lower(const interval<Tx> & t){
            static_assert(
                std::is_literal_type< interval<Tx> >::value,
                "interval<Tx> is not literal type"
            );
            return interval<Tx>(
                t.l,
                vmin(checked_result<Tx>(-1), t.u)
            );
        }

        // result unsigned
        template<class R, typename Tx, typename Ux>
        static typename boost::enable_if_c<
            ! std::numeric_limits<Ux>::is_signed,
            const interval<R>
        >::type
        SAFE_NUMERIC_CONSTEXPR r(
            const interval<Tx> & t,
            const interval<Ux> & u
        ){
            if(u.l > 0)
                return operator/<R>(t, u);
            else
                return operator/<R>(t, r_upper(u)) ;
        };

        // result signed
        template<class R, typename Tx, typename Ux>
        static typename boost::enable_if_c<
            std::numeric_limits<Ux>::is_signed,
            const interval<R>
        >::type
        SAFE_NUMERIC_CONSTEXPR r(
            const interval<Tx> & t,
            const interval<Ux> & u
        ){
            if(u.l > 0 || u.u < 0){
                return operator/<R>(t, u);
            }
            else{
                auto lower = operator/<R>(t,r_lower(u));
                auto upper = operator/<R>(t,r_upper(u));
                return
                    interval<R>(
                        vmin(lower.l, upper.l),
                        vmax(lower.u, upper.u)
                    );
            }
        }

        //typedef print<std::integral_constant<bool,  (u.l > 0) >> p_ulgt0;
        //typedef print<std::integral_constant<bool,  (u.u < 0) >> p_uult0;

        constexpr const static interval<max_t> ni = r<max_t>(t(), u());

        //static_assert(ni.l.no_exception(), "invalid lower bound");
        //static_assert(ni.u.no_exception(), "invalid upper bound");

        SAFE_NUMERIC_CONSTEXPR static const max_t newmin = ni.l.no_exception() ?
            static_cast<max_t>(ni.l)
        :
            std::numeric_limits<max_t>::min()
        ;
        //typedef print<std::integral_constant<max_t, newmin>> p_newmin;

        SAFE_NUMERIC_CONSTEXPR static const max_t newmax = ni.u.no_exception() ?
            static_cast<max_t>(ni.u)
        :
            std::numeric_limits<max_t>::max()
        ;
        //typedef print<std::integral_constant<max_t, newmax>> p_newmax;

        static_assert(
            newmin <= newmax,
            "new minimumum must be less than new maximum"
        );

        static_assert(
            std::numeric_limits<max_t>::min() >= 0 || std::is_signed<max_t>::value,
            "newmin < 0 and unsigned can't happen"
        );

        typedef typename safe_range<
            max_t,
            newmin,
            newmax,
            P,
            E
        >::type type;
    };

    // forward to correct divide implementation
    template<class R, class T, class U>
    checked_result<R>
    static SAFE_NUMERIC_CONSTEXPR divide(
        const T & t,
        const U & u
    ){
        return checked::divide_automatic<R>(t, u);
    }

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U, typename P, typename E>
    struct modulus_result {
        typedef typename base_type<T>::type base_type_t;
        static_assert(
            std::is_literal_type< interval<base_type_t> >::value,
            "interval<base_type_t> is not literal type"
        );
        typedef typename base_type<U>::type base_type_u;
        static_assert(
            std::is_literal_type< interval<base_type_u> >::value,
            "interval<base_type_u> is not tliteral type"
        );
        using max_t = typename boost::mpl::if_c<
            std::numeric_limits<base_type_t>::is_signed
            || std::numeric_limits<base_type_u>::is_signed,
            std::intmax_t,
            std::uintmax_t
        >::type;

        //typedef print<max_t> p_max_t;
        //typedef print<typename base_type<U>::type > p_U_t;

        typedef typename safe_range<
            max_t,
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max()),
            P,
            E
        >::type type;
    };

    // forward to correct modulus implementation
    template<class R, class T, class U>
    checked_result<R>
    static SAFE_NUMERIC_CONSTEXPR modulus(
        const T & t,
        const U & u
    ){
        return checked::modulus<R>(t, u);
    }

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U, typename P, typename E>
    struct left_shift_result {
        typedef typename base_type<T>::type t_base_type;
        typedef typename base_type<U>::type u_base_type;
        SAFE_NUMERIC_CONSTEXPR static const interval<t_base_type> t = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };
        SAFE_NUMERIC_CONSTEXPR static const interval<u_base_type> u = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };
        typedef decltype(t_base_type() << u_base_type()) r_base_type;

        SAFE_NUMERIC_CONSTEXPR static const interval<r_base_type> r
            = operator<<<r_base_type>(t, u);

        typedef safe_base<r_base_type, r.l, r.u, P, E> type;
    };
    template<typename T, typename U, typename P, typename E>    
    struct right_shift_result {
        typedef typename base_type<T>::type t_base_type;
        typedef typename base_type<U>::type u_base_type;
        SAFE_NUMERIC_CONSTEXPR static const interval<t_base_type> t = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };
        SAFE_NUMERIC_CONSTEXPR static const interval<u_base_type> u = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };
        typedef decltype(t_base_type() >> u_base_type()) r_base_type;

        SAFE_NUMERIC_CONSTEXPR static const interval<r_base_type> r
            = operator>><r_base_type>(t, u);

        typedef safe_base<r_base_type, r.l, r.u, P, E> type;
    };
};

} // numeric
} // boost

#endif // BOOST_NUMERIC_AUTOMATIC_HPP
