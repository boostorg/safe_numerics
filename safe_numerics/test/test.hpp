#include <iostream>
#include <boost/cstdint.hpp>
#include <boost/limits.hpp>

#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/array/elem.hpp>
#include <boost/preprocessor/array/size.hpp>
#include <boost/preprocessor/list/adt.hpp>
#include <boost/preprocessor/list/to_tuple.hpp>
#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/if.hpp>

#include "../include/safe_integer.hpp"
#include "../include/safe_cast.hpp"

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
/**/

inline unsigned int 
count_bits(boost::uintmax_t t){
    unsigned int i = 0;
    while(t != 0){
        ++i;
        t >>= 1;
    }
    return i;
}

inline unsigned int 
count_bits(boost::intmax_t t){
    if(t < 0)
        t = -t;
    unsigned int i = 0;
    while(t != 0){
        ++i;
        t >>= 1;
    }
    return i;
}

#define EACH_VALUE2(z, l, list)                 \
    BOOST_PP_EXPAND(                            \
        TESTX                                   \
        BOOST_PP_LIST_TO_TUPLE(                 \
            BOOST_PP_LIST_CONS(                 \
                BOOST_PP_ARRAY_ELEM(l, VALUES), \
                list                            \
            )                                   \
        )                                       \
    )                                           \
/**/

#define EACH_VALUE1(z, k, types)                \
    BOOST_PP_REPEAT(                            \
        BOOST_PP_ARRAY_SIZE(VALUES),            \
        EACH_VALUE2,                            \
        BOOST_PP_LIST_CONS(                     \
            BOOST_PP_ARRAY_ELEM(k, VALUES),     \
            types                               \
        )                                       \
    )                                           \
/**/

#define EACH_TYPE2(i, j)                \
    BOOST_PP_REPEAT(                    \
        BOOST_PP_ARRAY_SIZE(VALUES),    \
        EACH_VALUE1,                    \
        (i, (j, BOOST_PP_NIL))          \
    )                                   \
/**/

#define EACH_TYPE1(i)               \
    EACH_TYPE2(i, boost::int8_t)    \
    EACH_TYPE2(i, boost::uint8_t)   \
    EACH_TYPE2(i, boost::int16_t)   \
    EACH_TYPE2(i, boost::uint16_t)  \
    EACH_TYPE2(i, boost::int32_t)   \
    EACH_TYPE2(i, boost::uint32_t)  \
    EACH_TYPE2(i, boost::int64_t)   \
    EACH_TYPE2(i, boost::uint64_t)  \
/**/

#define TEST                         \
    EACH_TYPE1(boost::int8_t)        \
    EACH_TYPE1(boost::uint8_t)       \
    EACH_TYPE1(boost::int16_t)       \
    EACH_TYPE1(boost::uint16_t)      \
    EACH_TYPE1(boost::int32_t)       \
    EACH_TYPE1(boost::uint32_t)      \
    EACH_TYPE1(boost::int64_t)       \
    EACH_TYPE1(boost::uint64_t)      \
/**/

