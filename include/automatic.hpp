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
#include <type_traits> // conditional
#include <boost/integer.hpp> //

#include "safe_common.hpp"
#include "checked_result.hpp"
#include "checked_default.hpp"
#include "checked_integer.hpp"
#include "interval.hpp"
#include "utility.hpp"

namespace boost {
namespace numeric {

struct automatic {
private:
    // the following returns the "true" type.  After calculating the new max and min
    // these return the minimum size type which can hold the expected result.
    struct defer_stored_signed_lazily {
        template<std::intmax_t Min, std::intmax_t Max>
        using type = utility::signed_stored_type<Min, Max>;
    };

    struct defer_stored_unsigned_lazily {
        template<std::uintmax_t Min, std::uintmax_t Max>
        using type = utility::unsigned_stored_type<Min, Max>;
    };

    template<typename T, T Min, T Max>
    struct result_type {
        using type = typename std::conditional<
            std::numeric_limits<T>::is_signed,
            defer_stored_signed_lazily,
            defer_stored_unsigned_lazily
        >::type::template type<Min, Max>;
    };

public:
    ///////////////////////////////////////////////////////////////////////
    template<typename T, typename U>
    struct addition_result {
        using temp_base_type = typename std::conditional<
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
        using temp_base_type = typename std::conditional<
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
        using temp_base_type = typename std::conditional<
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
        using temp_base_type = typename std::conditional<
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
        using temp_base_type = typename std::conditional<
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
        using temp_base_type = typename std::conditional<
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
        using temp_base_type = typename std::conditional<
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
        #if 0
        static const unsigned bits = std::min(
            std::initializer_list<unsigned>{
                    utility::significant_bits(base_value(std::numeric_limits<T>::min())),
                    utility::significant_bits(base_value(std::numeric_limits<U>::min())),
                    utility::significant_bits(base_value(std::numeric_limits<T>::max())),
                    utility::significant_bits(base_value(std::numeric_limits<U>::max()))
            }
        );

        static const unsigned bitsx = std::min(
            bits,
            64u
        );
        static_assert(bitsx <= 64, "too many bits");
        using type = typename std::conditional<
            std::numeric_limits<
                decltype(
                    typename base_type<T>::type()
                    ^ typename base_type<U>::type()
                )
            >::is_signed,
            // result is signed
            typename boost::int_t<bitsx>::least,
            // otherwise result is unsigned
            typename boost::uint_t<bitsx>::least
        >::type;
        #endif
        using type = decltype(
            typename base_type<T>::type()
            ^ typename base_type<U>::type()
        );
    };
    template<typename T, typename U>
    struct bitwise_or_result {
        #if 0
        static const unsigned bits = std::max(
            std::initializer_list<unsigned>{
                    utility::significant_bits(base_value(std::numeric_limits<T>::min())),
                    utility::significant_bits(base_value(std::numeric_limits<U>::min())),
                    utility::significant_bits(base_value(std::numeric_limits<T>::max())),
                    utility::significant_bits(base_value(std::numeric_limits<U>::max()))
            }
        );

        static const unsigned bitsx = std::min(
            bits,
            64u
        );
        static_assert(bitsx <= 64, "too many bits");
        using type = typename std::conditional<
            std::numeric_limits<
                decltype(
                    typename base_type<T>::type()
                    ^ typename base_type<U>::type()
                )
            >::is_signed,
            // result is signed
            typename boost::int_t<bitsx>::least,
            // otherwise result is unsigned
            typename boost::uint_t<bitsx>::least
        >::type;
        #endif
        using type = decltype(
            typename base_type<T>::type()
            ^ typename base_type<U>::type()
        );
    };
    template<typename T, typename U>
    struct bitwise_xor_result {
        #if 0
        static const unsigned bits = std::max(
            std::initializer_list<unsigned>{
                    utility::significant_bits(base_value(std::numeric_limits<T>::min())),
                    utility::significant_bits(base_value(std::numeric_limits<U>::min())),
                    utility::significant_bits(base_value(std::numeric_limits<T>::max())),
                    utility::significant_bits(base_value(std::numeric_limits<U>::max()))
            }
        );

        static const unsigned bitsx = std::min(
            bits,
            64u
        );
        static_assert(bitsx <= 64, "too many bits");
        using type = typename std::conditional<
            std::numeric_limits<
                decltype(
                    typename base_type<T>::type()
                    ^ typename base_type<U>::type()
                )
            >::is_signed,
            // result is signed
            typename boost::int_t<bitsx>::least,
            // otherwise result is unsigned
            typename boost::uint_t<bitsx>::least
        >::type;
        #endif
        using type = decltype(
            typename base_type<T>::type()
            ^ typename base_type<U>::type()
        );
    };
};

} // numeric
} // boost

#endif // BOOST_NUMERIC_AUTOMATIC_HPP
