//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <boost/cstdint.hpp>
#include <boost/limits.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/array/elem.hpp>
#include <boost/preprocessor/array/size.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <boost/type_traits/is_signed.hpp>

#include "../include/safe_cast.hpp"
#include "../include/safe_integer.hpp"

#define TYPES (8, (    \
    boost::int8_t,     \
    boost::uint8_t,    \
    boost::int16_t,    \
    boost::uint16_t,   \
    boost::int32_t,    \
    boost::uint32_t,   \
    boost::int64_t,    \
    boost::uint64_t    \
))

#define VALUES (20, (  \
    0x10,              \
    0x20,              \
    0x7f,              \
    0x80,              \
    0xff,              \
    0x1000,            \
    0x2000,            \
    0x7fff,            \
    0x8000,            \
    0xffff,            \
    0x10000000,        \
    0x20000000,        \
    0x7fffffff,        \
    0x80000000,        \
    0xffffffff,        \
    0x100000000000,    \
    0x200000000000,    \
    0x7fffffffffff,    \
    0x80000000ffff,    \
    0xffffffffffff     \
))

// test conversion
template<class T1, class T2, class V>
bool test_conversion(V v, const char *a1, const char *a2, const char *a3){
    bool success;
    boost::numeric::safe<T2> t2;
    /* test conversion constructor */
    try{
        t2 = v;
        success = true;
    } 
    catch(std::range_error e){
        success = false;
    }
    if(success){
        if(t2 > std::numeric_limits<T2>::max()
        || t2 < std::numeric_limits<T2>::min()){
            std::cout
                << "failed to detect error in construction "
                << a1 << ' ' << a2
                << std::endl;
            return false;
        }
    }
    else{
        if(v <= std::numeric_limits<T2>::max()
        && v >= std::numeric_limits<T2>::min()){
            std::cout
                << "erroneasly detected error in construction "
                << a1 << ' ' << a2
                << std::endl;
            return false;
        }
        return true; // correctly detected error
    }

    T1 t1;
    try{
        //t1 = t2;
        t1 = boost::numeric::safe_cast<T1>(t2);
        success = true;
    }
    catch(std::range_error e){
        success = false;
    }
    if(success){
        if(t1 > std::numeric_limits<T1>::max()
        || t1 < std::numeric_limits<T1>::min()){
            std::cout 
                << "failed to detect error in conversion " 
                << a1 << ' ' << a2 << ' ' << a3
                << std::endl;
            return false;
        }
    }
    else{
        if(t2 <= std::numeric_limits<T1>::max()
        && t2 >= std::numeric_limits<T1>::min()){
            std::cout
                << "erroneasly detected error in conversion "
                << a1 << ' ' << a2 << ' ' << a3
                << std::endl;
            return false;
        }
        return true; // correctly detected error
    }
    return true; // passed test
}

template<class T>
struct test_value {
    typedef typename boost::mpl::if_<
        boost::is_signed<T>,
        boost::intmax_t,
        boost::uintmax_t
    >::type type;
}; 

#define TEST_CONVERSION(T1, T2, v)        \
    test_conversion<T1, T2>(              \
        static_cast<test_value<T2>::type>(v), \
        BOOST_PP_STRINGIZE(T1),           \
        BOOST_PP_STRINGIZE(T2),           \
        BOOST_PP_STRINGIZE(v)             \
    );                

#define TEST(z, k, ij)                    \
    TEST_CONVERSION(                      \
        BOOST_PP_ARRAY_ELEM(              \
            BOOST_PP_TUPLE_ELEM(2, 0, ij),\
            TYPES                         \
        ),                                \
        BOOST_PP_ARRAY_ELEM(              \
            BOOST_PP_TUPLE_ELEM(2, 1, ij),\
            TYPES                         \
        ),                                \
        BOOST_PP_ARRAY_ELEM(k, VALUES)    \
    )

#define EACH_VALUE(z, j, i)               \
    BOOST_PP_REPEAT(                      \
        BOOST_PP_ARRAY_SIZE(VALUES),      \
        TEST,                             \
        (i, j)                            \
    )                                     \
/**/

#define EACH_TYPE1(z, i, x)               \
    BOOST_PP_REPEAT(                      \
        BOOST_PP_ARRAY_SIZE(TYPES),       \
        EACH_VALUE,                       \
        i                                 \
    )                                     \
/**/

int main(int argc, char *argv[]){
    BOOST_PP_REPEAT(
        BOOST_PP_ARRAY_SIZE(TYPES),
        EACH_TYPE1,
        x
    )
    /*
    TEST(0, 0, (0, 1));
    TEST_CONVERSION(boost::uint8_t, boost::uint8_t, 0x80)  
    */
    return 0;
}
