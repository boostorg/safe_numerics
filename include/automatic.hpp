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

// policy which creates expanded results types designed
// to avoid overflows.

#include <limits>
#include <cstdint>     // (u)intmax_t,
#include <boost/mpl/if.hpp>

#include "safe_common.hpp"
#include "checked_result.hpp"
#include "interval.hpp"
#include "utility.hpp"

namespace boost {
namespace numeric {

struct automatic {
private:
    // the following returns the "true" type.  After calculating the new max and min
    // these return the minimum size type which can hold the expected result.
    template<typename T, T Min, T Max>
    struct defer_stored_signed_lazily {
        using type = utility::signed_stored_type<Min, Max>;
    };

    template<typename T, T Min, T Max>
    struct defer_stored_unsigned_lazily {
        using type = utility::unsigned_stored_type<Min, Max>;
    };

    template<typename T, T Min, T Max>
    using result_type =
        typename boost::mpl::if_c<
            std::numeric_limits<T>::is_signed,
            defer_stored_signed_lazily<T, Min, Max>,
            defer_stored_unsigned_lazily<T, Min, Max>
        >::type;

public:
    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct addition_result {
        using temp_base_type = typename boost::mpl::if_c<
            // if both arguments are unsigned
            ! std::numeric_limits<T>::is_signed
            && ! std::numeric_limits<U>::is_signed,
            // result is unsigned
            std::uintmax_t,
            // otherwise result is signed
            std::intmax_t
        >::type;

        using r_type = checked_result<temp_base_type>;
        using r_interval_type = interval<r_type>;

        constexpr static const r_interval_type t_interval{
            checked::cast<temp_base_type>(base_value(std::numeric_limits<T>::min())),
            checked::cast<temp_base_type>(base_value(std::numeric_limits<T>::max()))
        };

        constexpr static const r_interval_type u_interval{
            checked::cast<temp_base_type>(base_value(std::numeric_limits<U>::min())),
            checked::cast<temp_base_type>(base_value(std::numeric_limits<U>::max()))
        };

        constexpr static const r_interval_type r_interval = t_interval + u_interval;

        using type = typename result_type<
            temp_base_type,
            r_interval.l.exception()
                ? std::numeric_limits<temp_base_type>::min()
                : static_cast<temp_base_type>(r_interval.l),
            r_interval.u.exception()
                ? std::numeric_limits<temp_base_type>::max()
                : static_cast<temp_base_type>(r_interval.u)
        >::type;
    };

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct subtraction_result {
        // result of subtraction are always signed.
        using temp_base_type = intmax_t;

        using r_type = checked_result<temp_base_type>;
        using r_interval_type = interval<r_type>;

        constexpr static const r_interval_type t_interval{
            checked::cast<temp_base_type>(base_value(std::numeric_limits<T>::min())),
            checked::cast<temp_base_type>(base_value(std::numeric_limits<T>::max()))
        };

        constexpr static const r_interval_type u_interval{
            checked::cast<temp_base_type>(base_value(std::numeric_limits<U>::min())),
            checked::cast<temp_base_type>(base_value(std::numeric_limits<U>::max()))
        };

        constexpr static const r_interval_type r_interval = t_interval - u_interval;

        using type = typename result_type<
            temp_base_type,
            r_interval.l.exception()
                ? std::numeric_limits<temp_base_type>::min()
                : static_cast<temp_base_type>(r_interval.l),
            r_interval.u.exception()
                ? std::numeric_limits<temp_base_type>::max()
                : static_cast<temp_base_type>(r_interval.u)
        >::type;
    };

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct multiplication_result {
        using temp_base_type = typename boost::mpl::if_c<
            // if both arguments are unsigned
            ! std::numeric_limits<T>::is_signed
            && ! std::numeric_limits<U>::is_signed,
            // result is unsigned
            std::uintmax_t,
            // otherwise result is signed
            std::intmax_t
        >::type;

        using r_type = checked_result<temp_base_type>;
        using r_interval_type = interval<r_type>;

        constexpr static const r_interval_type t_interval{
            checked::cast<temp_base_type>(base_value(std::numeric_limits<T>::min())),
            checked::cast<temp_base_type>(base_value(std::numeric_limits<T>::max()))
        };

        constexpr static const r_interval_type u_interval{
            checked::cast<temp_base_type>(base_value(std::numeric_limits<U>::min())),
            checked::cast<temp_base_type>(base_value(std::numeric_limits<U>::max()))
        };

        constexpr static const r_interval_type r_interval = t_interval * u_interval;

        using type = typename result_type<
            temp_base_type,
            r_interval.l.exception()
                ? std::numeric_limits<temp_base_type>::min()
                : static_cast<temp_base_type>(r_interval.l),
            r_interval.u.exception()
                ? std::numeric_limits<temp_base_type>::max()
                : static_cast<temp_base_type>(r_interval.u)
        >::type;
    };

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct division_result {
        using temp_base_type = typename boost::mpl::if_c<
            // if both arguments are unsigned
            ! std::numeric_limits<T>::is_signed
            && ! std::numeric_limits<U>::is_signed,
            // result is unsigned
            std::uintmax_t,
            // otherwise result is signed
            std::intmax_t
        >::type;

        using r_type = checked_result<temp_base_type>;
        using r_interval_type = interval<r_type>;

        constexpr static const r_interval_type t_interval{
            checked::cast<temp_base_type>(base_value(std::numeric_limits<T>::min())),
            checked::cast<temp_base_type>(base_value(std::numeric_limits<T>::max()))
        };

        constexpr static const r_interval_type u_interval{
            checked::cast<temp_base_type>(base_value(std::numeric_limits<U>::min())),
            checked::cast<temp_base_type>(base_value(std::numeric_limits<U>::max()))
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

        using type = typename result_type<
            temp_base_type,
            r_interval.l.exception()
                ? std::numeric_limits<temp_base_type>::min()
                : static_cast<temp_base_type>(r_interval.l),
            r_interval.u.exception()
                ? std::numeric_limits<temp_base_type>::max()
                : static_cast<temp_base_type>(r_interval.u)
        >::type;
    };

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct modulus_result {
        using temp_base_type = typename boost::mpl::if_c<
            // if both arguments are unsigned
            ! std::numeric_limits<T>::is_signed
            && ! std::numeric_limits<U>::is_signed,
            // result is unsigned
            std::uintmax_t,
            // otherwise result is signed
            std::intmax_t
        >::type;

        using r_type = checked_result<temp_base_type>;
        using r_interval_type = interval<r_type>;

        constexpr static const r_interval_type t_interval{
            checked::cast<temp_base_type>(base_value(std::numeric_limits<T>::min())),
            checked::cast<temp_base_type>(base_value(std::numeric_limits<T>::max()))
        };

        constexpr static const r_interval_type u_interval{
            checked::cast<temp_base_type>(base_value(std::numeric_limits<U>::min())),
            checked::cast<temp_base_type>(base_value(std::numeric_limits<U>::max()))
        };

        constexpr static const r_interval_type rx(){
            if(u_interval.u < r_type(0)
            || u_interval.l > r_type(0))
                return t_interval / u_interval;
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

        using type = typename result_type<
            temp_base_type,
            r_interval.l.exception()
                ? std::numeric_limits<temp_base_type>::min()
                : static_cast<temp_base_type>(r_interval.l),
            r_interval.u.exception()
                ? std::numeric_limits<temp_base_type>::max()
                : static_cast<temp_base_type>(r_interval.u)
        >::type;
    };

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct comparison_result {
        using temp_base_type = typename boost::mpl::if_c<
            // if both arguments are unsigned
            ! std::numeric_limits<T>::is_signed
            && ! std::numeric_limits<U>::is_signed,
            // result is unsigned
            std::uintmax_t,
            // otherwise result is signed
            std::intmax_t
        >::type;

        using r_type = checked_result<temp_base_type>;
        using r_interval_type = interval<r_type>;

        constexpr static const r_interval_type t_interval{
            checked::cast<temp_base_type>(base_value(std::numeric_limits<T>::min())),
            checked::cast<temp_base_type>(base_value(std::numeric_limits<T>::max()))
        };

        constexpr static const r_interval_type u_interval{
            checked::cast<temp_base_type>(base_value(std::numeric_limits<U>::min())),
            checked::cast<temp_base_type>(base_value(std::numeric_limits<U>::max()))
        };

        constexpr static const r_interval_type r_interval =
            t_interval | u_interval;

        using type = typename result_type<
            temp_base_type,
            r_interval.l.exception()
                ? std::numeric_limits<temp_base_type>::min()
                : static_cast<temp_base_type>(r_interval.l),
            r_interval.u.exception()
                ? std::numeric_limits<temp_base_type>::max()
                : static_cast<temp_base_type>(r_interval.u)
        >::type;
    };

    ///////////////////////////////////////////////////////////////////////
    // shift operations
    template<typename T, typename U>
    struct left_shift_result {
        using temp_base_type = typename boost::mpl::if_c<
            std::numeric_limits<T>::is_signed,
            std::intmax_t,
            std::uintmax_t
        >::type;

        using r_type = checked_result<temp_base_type>;
        using r_interval_type = interval<r_type>;

        constexpr static const r_interval_type t_interval{
            checked::cast<temp_base_type>(base_value(std::numeric_limits<T>::min())),
            checked::cast<temp_base_type>(base_value(std::numeric_limits<T>::max()))
        };

        constexpr static const r_interval_type u_interval{
            checked::cast<temp_base_type>(base_value(std::numeric_limits<U>::min())),
            checked::cast<temp_base_type>(base_value(std::numeric_limits<U>::max()))
        };

        constexpr static const r_interval_type r_interval =
            t_interval << u_interval;

        using type = typename result_type<
            temp_base_type,
            r_interval.l.exception()
                ? std::numeric_limits<temp_base_type>::min()
                : static_cast<temp_base_type>(r_interval.l),
            r_interval.u.exception()
                ? std::numeric_limits<temp_base_type>::max()
                : static_cast<temp_base_type>(r_interval.u)
        >::type;
    };

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct right_shift_result {
        using temp_base_type = typename boost::mpl::if_c<
            std::numeric_limits<T>::is_signed,
            std::intmax_t,
            std::uintmax_t
        >::type;

        using r_type = checked_result<temp_base_type>;
        using r_interval_type = interval<r_type>;

        constexpr static const r_interval_type t_interval{
            checked::cast<temp_base_type>(base_value(std::numeric_limits<T>::min())),
            checked::cast<temp_base_type>(base_value(std::numeric_limits<T>::max()))
        };

        constexpr static const r_type u_min
            = checked::cast<temp_base_type>(base_value(std::numeric_limits<U>::min()));

        constexpr static const r_interval_type u_interval{
            u_min.exception()
            ? r_type(0)
            : u_min,
            checked::cast<temp_base_type>(base_value(std::numeric_limits<U>::max()))
        };

        constexpr static const r_interval_type r_interval = t_interval >> u_interval;

        //const utility::print_value<r_interval.l> pv_r_interval_l;
        //const utility::print_value<r_interval.u> pv_r_interval_u;

        using type = typename result_type<
            temp_base_type,
            r_interval.l.exception()
                ? std::numeric_limits<temp_base_type>::min()
                : static_cast<temp_base_type>(r_interval.l),
            r_interval.u.exception()
                ? std::numeric_limits<temp_base_type>::max()
                : static_cast<temp_base_type>(r_interval.u)
        >::type;

    };

    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct bitwise_and_result {
        using t_base_type = typename base_type<T>::type;
        using u_base_type = typename base_type<U>::type;
        using type = typename boost::mpl::if_c<
            (sizeof(t_base_type) > sizeof(u_base_type)),
            u_base_type,
            t_base_type
        >::type;
    };
    template<typename T, typename U>
    struct bitwise_or_result {
        using t_base_type = typename base_type<T>::type;
        using u_base_type = typename base_type<U>::type;
        using type = typename boost::mpl::if_c<
            (sizeof(t_base_type) > sizeof(u_base_type)),
            t_base_type,
            u_base_type
        >::type;
    };
    template<typename T, typename U>
    struct bitwise_xor_result {
        using t_base_type = typename base_type<T>::type;
        using u_base_type = typename base_type<U>::type;
        using type = typename boost::mpl::if_c<
            (sizeof(t_base_type) > sizeof(u_base_type)),
            t_base_type,
            u_base_type
        >::type;
    };
};

} // numeric
} // boost

#endif // BOOST_NUMERIC_AUTOMATIC_HPP
