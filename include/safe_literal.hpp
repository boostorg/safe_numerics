#ifndef BOOST_NUMERIC_SAFE_LITERAL_HPP
#define BOOST_NUMERIC_SAFE_LITERAL_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdint> // for intmax_t/uintmax_t
#include <iosfwd>
#include <type_traits>
#include <boost/mpl/if.hpp>

#include "utility.hpp"
#include "safe_base.hpp"
#include "safe_base_operations.hpp"
#include "native.hpp"
#include "exception_policies.hpp"

namespace boost {
namespace numeric {

template<typename T, T N, class P, class E>
class safe_literal_impl;

template<typename T, T N, class P, class E>
struct is_safe<safe_literal_impl<T, N, P, E> > : public std::true_type
{};

template<typename T, T N, class P, class E>
struct get_promotion_policy<safe_literal_impl<T, N, P, E> > {
    using type = P;
};

template<typename T, T N, class P, class E>
struct get_exception_policy<safe_literal_impl<T, N, P, E> > {
    using type = E;
};
template<typename T, T N, class P, class E>
struct base_type<safe_literal_impl<T, N, P, E> > {
    using type = T;
};

template<typename T, T N, class P, class E>
constexpr T base_value(
    const safe_literal_impl<T, N, P, E>  & st
) {
    return N;
}

template<typename CharT, typename Traits, typename T, T N, class P, class E>
inline std::basic_ostream<CharT, Traits> & operator<<(
    std::basic_ostream<CharT, Traits> & os,
    const safe_literal_impl<T, N, P, E> & t
){
    return os << (
        (std::is_same<T, signed char>::value
        || std::is_same<T, unsigned char>::value
        ) ?
            static_cast<int>(N)
        :
            N
    );
}

template<typename T, T N, class P, class E>
class safe_literal_impl {

    template<
        class CharT,
        class Traits
    >
    friend std::basic_ostream<CharT, Traits> & operator<<(
        std::basic_ostream<CharT, Traits> & os,
        const safe_literal_impl & t
    ){
        return os << (
            (::std::is_same<T, signed char>::value
            || ::std::is_same<T, unsigned char>::value
            || ::std::is_same<T, wchar_t>::value
            ) ?
                static_cast<int>(N)
            :
                N
        );
    };

public:

    ////////////////////////////////////////////////////////////
    // constructors
    // default constructor
    constexpr safe_literal_impl(){}

    /////////////////////////////////////////////////////////////////
    // casting operators for intrinsic integers
    // convert to any type which is not safe.  safe types need to be
    // excluded to prevent ambiguous function selection which
    // would otherwise occur
    template<
        class R,
        typename std::enable_if<
            ! boost::numeric::is_safe<R>::value,
            int
        >::type = 0
    >
    constexpr operator R () const {
        return N;
    }
};

template<
    std::intmax_t N,
    class P = void,
    class E = void
>
using safe_signed_literal = safe_literal_impl<
    typename utility::signed_stored_type<N, N>,
    N,
    P,
    E
>;

template<
    std::uintmax_t N,
    class P = void,
    class E = void
>
using safe_unsigned_literal = safe_literal_impl<
    typename utility::unsigned_stored_type<N, N>,
    N,
    P,
    E
>;

#define safe_literal(n)                               \
    boost::mpl::if_c<                                 \
        std::numeric_limits<decltype<n>>::is_signed>. \
        safe_unsigned_literal<n, void, void>,         \
        safe_signed_literal<n, void, void>            \
    >::type

} // numeric
} // boost

/////////////////////////////////////////////////////////////////
// numeric limits for safe_literal etc.

#include <limits>

namespace std {

template<
    typename T,
    T N,
    class P,
    class E
>
class numeric_limits<boost::numeric::safe_literal_impl<T, N, P, E> >
    : public std::numeric_limits<T>
{
    using SL = boost::numeric::safe_literal_impl<T, N, P, E>;
public:
    constexpr static SL lowest() noexcept {
        return SL();
    }
    constexpr static SL min() noexcept {
        return SL();
    }
    constexpr static SL max() noexcept {
        return SL();
    }
};

} // std

#endif // BOOST_NUMERIC_SAFE_LITERAL_HPP
