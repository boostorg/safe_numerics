#ifndef BOOST_NUMERIC_CPP_HPP
#define BOOST_NUMERIC_CPP_HPP

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

#include <boost/integer.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/or.hpp>

#include <boost/integer_traits.hpp>
#include <boost/static_assert.hpp>

#include "promotion.hpp"

namespace boost {
namespace numeric {
namespace promotion {

// in C++ the following rules govern integer arithmetic

// Follow section 5 of the standard.
template<class T, class U>
struct standard_promotion {
    // rank is (more or less) proportional to size of operand
    template<class T1>
    struct rank {
        BOOST_STATIC_CONSTANT(int, value =
            boost::integer_traits<T1>::digits 
            + boost::integer_traits<T1>::is_signed
        );
    };

    template<class T1, class U1>
    struct higher_ranked_type {
        // convert smaller of two types to the size of the larger
        typedef typename boost::mpl::if_c<
            (rank<T1>::value < rank<U1>::value),
            U1,
            T1
        >::type type;
    };

    template<class TSigned, class TUnsigned>
    struct clause2 {
        typedef typename boost::mpl::if_c<
            // if the type of the operand with signed integer type can represent all of the values of
            // the type of the operand with unsigned integer type, 
            (boost::integer_traits<TSigned>::digits
            >= boost::integer_traits<TUnsigned>::digits),
            // the operand with unsigned integer type shall
            // be converted to the type of the operand with signed integer type.
            TSigned,
            // both operands shall be converted to the unsigned integer type corresponding to the
            // type of the operand with signed integer type.
            TUnsigned
        >::type type;
    };

    template<class TSigned, class TUnsigned>
    struct clause1 {
        typedef typename boost::mpl::if_c<
            // if the operand that has unsigned integer type has rank greater than or equal to 
            // the rank of the type of the other operand,
            rank<TUnsigned>::value >= rank<TSigned>::value,
            // the operand with signed integer type shall be 
            // converted to the type of the operand with unsigned integer type.
            TUnsigned,
            // otherwise
            typename clause2<TSigned, TUnsigned>::type
        >::type type;
    };

    // if both operands have signed integer types or both have unsigned integer types, the
    // operand with the type of lesser integer conversion rank shall be converted to the type of the
    // operand with greater rank
    template<class T1, class U1>
    struct clause0 {
        typedef typename boost::mpl::if_c<
            // if types have the same sign
            ( boost::integer_traits<T>::is_signed
            == boost::integer_traits<U>::is_signed),
            // convert the smaller type to the larger
            higher_ranked_type<T, U>,
            // otherwise consider converting the signed type
            typename boost::mpl::if_c<
                boost::integer_traits<T>::is_signed,
                typename clause1<T, U>::type,
                typename clause1<U, T>::type
            >::type
        >::type type;
    };

    typedef typename clause0<T, U>::type type;
};

struct cpp {
    template<
        class T, T TMin, T TMax,
        class U, U UMin, U UMax
    >
    struct add_result {
        typedef typename standard_promotion<T, U>::type type;
        BOOST_STATIC_CONSTANT(bool, underflow =
            (promotion::sum_underflow<
                T,
                U,
                type,
                TMin,
                UMin,
                (boost::integer_traits<type>::const_min)
            >::value)
        );
        BOOST_STATIC_CONSTANT(bool, overflow =
            (promotion::sum_overflow<
                T,
                U,
                type,
                TMax,
                UMax,
                (boost::integer_traits<type>::const_max)
            >::value)
        );
        BOOST_STATIC_CONSTANT(type, min = (underflow ?
            (boost::integer_traits<type>::const_min)
            :
            TMin + UMin
        ));
        BOOST_STATIC_CONSTANT(type, max = (overflow ?
            (boost::integer_traits<type>::const_max)
            :
            TMax + UMax 
        ));
    };
    template<class T, class U>
    struct subtract_result {
        typedef typename standard_promotion<T, U>::type type;
    };
    template<class T, class U>
    struct multiply_result {
        typedef typename standard_promotion<T, U>::type type;
    };
    template<class T, class U>
    struct divide_result {
        typedef typename standard_promotion<T, U>::type type;
    };
    template<class T, class U>
    struct modulus_result {
        typedef typename standard_promotion<T, U>::type type;
    };

};  // cpp
}   // promotion
}   // numeric
}   // boost

#endif // BOOST_NUMERIC_cpp_HPP
