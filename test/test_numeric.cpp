//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "../include/numeric.hpp"

//#include <boost/limits.hpp>
#include <limits>
#include <boost/cstdint.hpp>
#include <boost/integer.hpp>
#include <boost/integer_traits.hpp>

#include <boost/mpl/assert.hpp>
#include <boost/mpl/print.hpp>
//#include <boost/mpl/integral_c.hpp>

BOOST_MPL_ASSERT(( boost::numeric::is_signed<int> ));
BOOST_MPL_ASSERT_NOT(( boost::numeric::is_signed<unsigned int> ));
BOOST_MPL_ASSERT_RELATION(8, ==, boost::numeric::bits<boost::int8_t>::value);
BOOST_MPL_ASSERT_RELATION(16, ==, (boost::numeric::max_bits<boost::int8_t, boost::int16_t>::value));
BOOST_MPL_ASSERT_RELATION(16, ==, (boost::numeric::max_bits<boost::int8_t, boost::int16_t>::value));

////////////////////////////////////////////////////////////
// addition of various types of integers

template<class T, class U, class R, int B>
struct addition_display {
    BOOST_MPL_ASSERT_RELATION(boost::numeric::bits<R>::value, >=, B);
};

// note we have to break this into two separate macros to avoid
// having gcc flag a compile error
#define TEST_ADDITION1(T, U)                                 \
    BOOST_MPL_ASSERT_RELATION(                               \
        (boost::integer_traits<                              \
            addition_result_type<T, U>::type                 \
        >::const_max),                                       \
        >=,                                                  \
        boost::integer_traits<T>::const_max                  \
        + boost::integer_traits<U>::const_max                \
    );                                                       \
    {                                                        \
        typedef boost::mpl::print<                           \
            addition_display<                                \
                T,                                           \
                U,                                           \
                addition_result_type<T, U>::type,            \
                addition_result_bits<T, U>::value            \
            >                                                \
        >::type tdef;                                        \
    }                                                        \
/**/
#define TEST_ADDITION2(T, U)                                 \
    BOOST_MPL_ASSERT_RELATION(                               \
        (boost::integer_traits<                              \
            addition_result_type<T, U>::type                 \
        >::const_min),                                       \
        <=,                                                  \
        boost::integer_traits<T>::const_min                  \
        + boost::integer_traits<U>::const_min                \
    );                                                       \
    {                                                        \
        typedef boost::mpl::print<                           \
            addition_display<                                \
                T,                                           \
                U,                                           \
                addition_result_type<T, U>::type,            \
                addition_result_bits<T, U>::value            \
            >                                                \
        >::type tdef;                                        \
    }                                                        \
/**/

void add(){
    using namespace boost::numeric;
    TEST_ADDITION1(boost::int8_t, boost::int8_t)
    TEST_ADDITION1(boost::int16_t, boost::int8_t)
    TEST_ADDITION1(boost::uint8_t, boost::uint8_t)
    TEST_ADDITION1(boost::uint16_t, boost::uint8_t)
    TEST_ADDITION1(boost::uint16_t, boost::int8_t)
    TEST_ADDITION1(boost::int16_t, boost::uint8_t)

    TEST_ADDITION2(boost::int8_t, boost::int8_t)
    TEST_ADDITION2(boost::int16_t, boost::int8_t)
    TEST_ADDITION2(boost::uint8_t, boost::uint8_t)
    TEST_ADDITION2(boost::uint16_t, boost::uint8_t)
    TEST_ADDITION2(boost::uint16_t, boost::int8_t)
    TEST_ADDITION2(boost::int16_t, boost::uint8_t)
}

////////////////////////////////////////////////////////////
// subtraction of various types of integers

template<class T, class U, class R, int B>
struct subtraction_display {};

#if 1
// note we have to break this into two separate macros to avoid
// having gcc flag a compile error
#define TEST_SUBTRACTION1(T, U)                              \
    BOOST_MPL_ASSERT_RELATION(                               \
        (boost::integer_traits<                              \
            subtraction_result_type<T, U>::type              \
        >::const_max),                                       \
        >=,                                                  \
        boost::integer_traits<T>::const_max                  \
        + boost::integer_traits<U>::const_max                \
    );                                                       \
    {                                                        \
        typedef boost::mpl::print<                           \
            subtraction_display<                             \
                T,                                           \
                U,                                           \
                subtraction_result_type<T, U>::type,         \
                subtraction_result_bits<T, U>::value         \
            >                                                \
        >::type tdef;                                        \
    }                                                        \
/**/
#define TEST_SUBTRACTION2(T, U)                              \
    BOOST_MPL_ASSERT_RELATION(                               \
        (boost::integer_traits<                              \
            subtraction_result_type<T, U>::type              \
        >::const_min),                                       \
        <=,                                                  \
        boost::integer_traits<T>::const_min                  \
        + boost::integer_traits<U>::const_min                \
    );                                                       \
    {                                                        \
        typedef boost::mpl::print<                           \
            subtraction_display<                             \
                T,                                           \
                U,                                           \
                subtraction_result_type<T, U>::type,         \
                subtraction_result_bits<T, U>::value         \
            >                                                \
        >::type tdef;                                        \
    }                                                        \
/**/

void subtract(){
    using namespace boost::numeric;
    TEST_SUBTRACTION1(boost::int8_t, boost::int8_t)
    TEST_SUBTRACTION1(boost::int16_t, boost::int8_t)
    TEST_SUBTRACTION1(boost::uint8_t, boost::uint8_t)
    TEST_SUBTRACTION1(boost::uint16_t, boost::uint8_t)
    TEST_SUBTRACTION1(boost::uint16_t, boost::int8_t)
    TEST_SUBTRACTION1(boost::int16_t, boost::uint8_t)

    TEST_SUBTRACTION2(boost::int8_t, boost::int8_t)
    TEST_SUBTRACTION2(boost::int16_t, boost::int8_t)
    TEST_SUBTRACTION2(boost::uint8_t, boost::uint8_t)
    TEST_SUBTRACTION2(boost::uint16_t, boost::uint8_t)
    TEST_SUBTRACTION2(boost::uint16_t, boost::int8_t)
    TEST_SUBTRACTION2(boost::int16_t, boost::uint8_t)
}

#endif

////////////////////////////////////////////////////////////
// multiplication of various types of integers

// note - this test is not really correct.  What is really needed
// is compile time integer interval arithmetic - which we don't
// have.  Addition is trivial - see above - but multiplication
// is more challanging. So, for now, this test fails to detect
// implementation errors in calculation of ranges of the result
// of multiplications

template<class T, class U, class R, int B>
struct multiply_display {};

#define TEST_MULTIPLICATION1(T, U)                           \
    BOOST_MPL_ASSERT_RELATION(                               \
        (boost::integer_traits<                              \
            multiply_result_type<T, U>::type                 \
        >::const_max),                                       \
        >=,                                                  \
        boost::integer_traits<T>::const_max                  \
        * boost::integer_traits<U>::const_max                \
    );                                                       \
    {                                                        \
        typedef boost::mpl::print<                           \
            multiply_display<                                \
                T,                                           \
                U,                                           \
                multiply_result_type<T, U>::type,            \
                multiply_result_bits<T, U>::value            \
            >                                                \
        >::type tdef;                                        \
    }                                                        \
/**/
#define TEST_MULTIPLICATION2(T, U)                           \
    BOOST_MPL_ASSERT_RELATION(                               \
        (boost::integer_traits<                              \
            multiply_result_type<T, U>::type                 \
        >::const_min),                                       \
        <=,                                                  \
        boost::integer_traits<T>::const_min                  \
        * boost::integer_traits<U>::const_min                \
    );                                                       \
    {                                                        \
        typedef boost::mpl::print<                           \
            multiply_display<                                \
                T,                                           \
                U,                                           \
                multiply_result_type<T, U>::type,            \
                multiply_result_bits<T, U>::value            \
            >                                                \
        >::type tdef;                                        \
    }                                                        \
/**/

void multiply(){
    using namespace boost::numeric;
    
    TEST_MULTIPLICATION1(boost::int8_t, boost::int8_t)
    TEST_MULTIPLICATION1(boost::int16_t, boost::int8_t)
    TEST_MULTIPLICATION1(boost::uint8_t, boost::uint8_t)
    TEST_MULTIPLICATION1(boost::uint16_t, boost::uint8_t)
    TEST_MULTIPLICATION1(boost::uint16_t, boost::int8_t)
    TEST_MULTIPLICATION1(boost::int16_t, boost::uint8_t)

    TEST_MULTIPLICATION2(boost::int8_t, boost::int8_t)
    TEST_MULTIPLICATION2(boost::int16_t, boost::int8_t)
    TEST_MULTIPLICATION2(boost::uint8_t, boost::uint8_t)
    TEST_MULTIPLICATION2(boost::uint16_t, boost::uint8_t)
    TEST_MULTIPLICATION2(boost::uint16_t, boost::int8_t)
    TEST_MULTIPLICATION2(boost::int16_t, boost::uint8_t)
}

const bool b = (boost::integer_traits<unsigned long>::const_max > boost::integer_traits<unsigned long>::const_max);

#include <iostream>

int main(int argc, char * argv[]){
    std::cerr << boost::integer_traits<uint16_t>::const_max << std::endl;
    std::cerr << boost::integer_traits<uint8_t>::const_max << std::endl;
    std::cerr <<
        boost::integer_traits<uint16_t>::const_max
        *boost::integer_traits<uint8_t>::const_max
        << std::endl;

    add();
    subtract();
    multiply();
    return 0;
}
