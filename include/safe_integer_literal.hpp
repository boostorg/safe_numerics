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

    // non mutating unary operators
    constexpr safe_literal_impl & operator+() const { // unary plus
        return *this;
    }
    // after much consideration, I've permited the resulting value of a unary
    // - to change the type.  The C++ standard does invoke integral promotions
    // so it's changing the type as well.

    /*  section 5.3.1 &8 of the C++ standard
    The operand of the unary - operator shall have arithmetic or unscoped
    enumeration type and the result is the negation of its operand. Integral
    promotion is performed on integral or enumeration operands. The negative
    of an unsigned quantity is computed by subtracting its value from 2n,
    where n is the number of bits in the promoted operand. The type of the
    result is the type of the promoted operand.
    */
    constexpr safe_literal_impl<T, -N, P, E> operator-() const { // unary minus
        return 0 - *this;
        //return safe_literal_impl<T, -N, P, E>();
    }
    /*   section 5.3.1 &10 of the C++ standard
    The operand of ~ shall have integral or unscoped enumeration type; 
    the result is the ones’ complement of its operand. Integral promotions 
    are performed. The type of the result is the type of the promoted operand.
    */
    constexpr safe_literal_impl<T, ~N, P, E> operator~() const { // complement
        return safe_literal_impl<T, ~N, P, E>();
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

#if 0
// the following is what I'd like to do to deal with the
// annoying signed/unsigned selection.  Unfortunately, for now
// the parameter N is not known to be constexpr so one gets
// a syntax error when trying to use this.  Leave this in as
// a reminder that I spent a lot of time and failed to
// find a solution
template<
    typename T,
    class P = void,
    class E = void
>
auto constexpr make_safe_literal(T N){
    return boost::mpl::eval_if_c<
        std::is_signed<T>::value,
        safe_signed_literal<N, P, E>,
        safe_signed_literal<N, P, E>
    >::type();
}
#endif

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
