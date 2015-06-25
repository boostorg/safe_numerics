#ifndef BOOST_NUMERIC_NUMERIC_HPP
#define BOOST_NUMERIC_NUMERIC_HPP

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

#include <boost/config.hpp>
#include <boost/limits.hpp>
#include <boost/integer.hpp>
#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/min_max.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>

#include "concept/numeric.hpp"
#include "boost/concept/assert.hpp"

namespace boost {
namespace numeric {

namespace detail {
    template<boost::intmax_t MAX, int BASE>
    struct log;

    template<int BASE>
    struct log<0, BASE> : public
        boost::mpl::integral_c<
            boost::intmax_t ,
            0
        >
    {};
    template<int BASE>
    struct log<-1, BASE> : public
        boost::mpl::integral_c<
            boost::intmax_t ,
            0
        >
    {};

    template<boost::intmax_t MAX, int BASE>
    struct log : public 
        boost::mpl::integral_c<
            boost::intmax_t ,
            (1 + log<MAX / BASE, BASE>::value )
        >
    {};

    template<boost::uintmax_t MAX, int BASE>
    struct ulog;

    template<int BASE>
    struct ulog<0, BASE> : public
        boost::mpl::integral_c<
            boost::uintmax_t ,
            0
        >
    {};
    template<boost::uintmax_t MAX, int BASE>
    struct ulog : public   
        boost::mpl::integral_c<
            boost::uintmax_t ,
            (1 + ulog<MAX / BASE, BASE>::value)
        >
    {};

} //detail

// can't use is_signed type traits as these are defined ONLY for C++
// primitive types.  They are are not (by design) implemented in terms
// of numeric_limits so they aren't helpful to us.  We leave then in the
// boost/numeric namespace to distinguish them form the "official" type
// traits !
template<class T>
struct is_signed : public
    boost::mpl::integral_c<
        bool,
        std::numeric_limits<T>::is_signed
    >
{
    BOOST_CONCEPT_ASSERT((Numeric<T>));
};
template<class T>
struct is_unsigned : public
    boost::mpl::integral_c<
        bool,
        ! std::numeric_limits<T>::is_signed
    >
{
    BOOST_CONCEPT_ASSERT((Numeric<T>));
};

// return the number of bits in a type excluding any sign
template<class T>
struct digits : public
    boost::mpl::integral_c<int, std::numeric_limits<T>::digits>
{
    BOOST_CONCEPT_ASSERT((Numeric<T>));
};

// return maximum bits in types T and U
template<class T, class U>
struct max_digits : public
    boost::mpl::max<
        digits<T>,
        digits<U>
    >::type
{};

// return the number of bits in a type including the sign
template<class T>
struct bits : public 
    boost::mpl::plus<
        typename boost::mpl::integral_c<int, std::numeric_limits<T>::digits>,
        typename boost::mpl::integral_c<int, std::numeric_limits<T>::is_signed>
    > 
{};

// return maximum bits in types T and U
template<class T, class U>
struct max_bits : public
    boost::mpl::max<
        bits<T>,
        bits<U>
    >::type
{};


// return # of bit in the result of an addition of two types
template<class T, class U>
struct addition_result_bits : public
    boost::mpl::plus<
        max_digits<T, U>,
        // if either is signed
        typename boost::mpl::eval_if<
            typename boost::mpl::or_<
                is_signed<T>,
                is_signed<U>
            >,
            // add one guard bit and one sign bit
            boost::mpl::integral_c<int, 2>,
            // add one guard bit to detect overflow
            boost::mpl::integral_c<int, 1>
        >::type
    >
{};

// return smallest type that can hold the sum of types T and U
template<class T, class U>
struct addition_result_type {
    typedef typename boost::mpl::if_<
        typename boost::mpl::or_<
            is_signed<T>,
            is_signed<U>
        >,
        typename boost::int_t<
            boost::mpl::min<
                bits<boost::intmax_t>,
                addition_result_bits<T, U>
            >::type::value
        >::least,
        typename boost::uint_t<
            boost::mpl::min<
                bits<boost::uintmax_t>,
                addition_result_bits<T, U>
            >::type::value
        >::least
    >::type type;
};

// return # of bit in the result of an subtraction of two types
template<class T, class U>
struct subtraction_result_bits : public
    boost::mpl::plus<
        max_digits<T, U>,
        // add one guard bit and one sign bit
        boost::mpl::integral_c<int, 2>
    >
{};

// return smallest type that can hold the difference of types T and U
template<class T, class U>
struct subtraction_result_type {
    typedef typename boost::int_t<
        boost::mpl::min<
            bits<boost::intmax_t>,
            subtraction_result_bits<T, U>
        >::type::value
    >::least type;
};

// return # of bits in the result of an product of two types
template<class T, class U>
struct multiply_result_bits : public
    boost::mpl::plus<
        digits<T>,
        digits<U>,
        // if either is signed
        typename boost::mpl::eval_if<
            typename boost::mpl::or_<
                is_signed<T>,
                is_signed<U>
            >,
            // add one sign bit
            boost::mpl::integral_c<int, 1>,
            boost::mpl::integral_c<int, 0>
        >::type
    >
{};

// return smallest type that can hold the product of types T and U
template<class T, class U>
struct multiply_result_type {
    typedef typename boost::mpl::eval_if<
        typename boost::mpl::or_<
            is_signed<T>,
            is_signed<U>
        >,
        typename boost::mpl::identity<
        typename boost::int_t<
            boost::mpl::min<
                bits<boost::intmax_t>,
                multiply_result_bits<T, U>
            >::type::value
        >::least
        >,
        typename boost::mpl::identity<
        typename boost::uint_t<
            boost::mpl::min<
                bits<boost::uintmax_t>,
                multiply_result_bits<T, U>
            >::type::value
        >::least
        >
    >::type type;
};

// return # of bits in the result of an quotient of two types
template<class T, class U>
struct division_result_bits : public
    boost::mpl::plus<
        digits<T>,
        // if either is signed
        typename boost::mpl::eval_if<
            typename boost::mpl::or_<
                is_signed<T>,
                is_signed<U>
            >,
            // add one sign bit
            boost::mpl::integral_c<int, 1>,
            boost::mpl::integral_c<int, 0>
        >::type
    >
{};

// return smallest type that can hold the quotient of types T and U
template<class T, class U>
struct division_result_type {
    typedef typename boost::mpl::if_<
        typename boost::mpl::or_<
            is_signed<T>,
            is_signed<U>
        >,
        typename boost::int_t<
            boost::mpl::min<
                bits<boost::intmax_t>,
                division_result_bits<T, U>
            >::type::value
        >::least,
        typename boost::uint_t<
            boost::mpl::min<
                bits<boost::uintmax_t>,
                division_result_bits<T, U>
            >::type::value
        >::least
    >::type type;
};

// return the type which results from usage of one the
// operators &, |, ^, <<, >> 
template<class T, class U>
struct logical_result_type {
    typedef typename boost::mpl::if_<
        typename boost::mpl::or_<
            is_signed<T>,
            is_signed<U>
        >,
        typename boost::int_t<
            max_bits<T, U>::value
        >::least,
        typename boost::uint_t<
            max_bits<T, U>::value
        >::least
    >::type type;
};

} // numeric
} // boost

#endif // BOOST_NUMERIC_NUMERIC_HPP
