//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <exception>
#include <cstdlib> // EXIT_SUCCESS

#include "../include/checked.hpp"

// test conversion to T2 from different literal types
template<class T2, class T1>
bool test_cast(const T1 & v1, const char *t2_name, const char *t1_name){
    std::cout
        << "testing static_cast<safe<" << t2_name << ">(" << t1_name << ")"
        << std::endl;

    constexpr const T1 cev1 = 0;
    constexpr const boost::numeric::checked_result<T2> r2 = boost::numeric::checked::cast<T2>(cev1);

    if(r2 == boost::numeric::checked_result<T2>::exception_type::no_exception){
        if(! (r2 == v1)){
            std::cout
                << "failed to detect error in construction "
                << t2_name << "<-" << t1_name
                << std::endl;
            boost::numeric::checked::cast<T2>(v1);
            return false;
        }
    }
    else{
        if( r2 == v1 ){
            std::cout
                << "erroneously emitted error "
                << t1_name << "<-" << t2_name
                << std::endl;
            boost::numeric::checked::cast<T2>(v1);
        }
    }
    return true; // passed test
}

#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/array/elem.hpp>
#include <boost/preprocessor/array/size.hpp>
#include <boost/preprocessor/stringize.hpp>

#include "test_types.hpp"
#include "test_values.hpp"
#include "test.hpp"

#define TEST_CAST(T1, v)              \
    rval = rval && test_cast<T1>(     \
        v,                            \
        BOOST_PP_STRINGIZE(T1),       \
        BOOST_PP_STRINGIZE(v)         \
    );
/**/

#define EACH_VALUE(z, value_index, type_index)     \
    TEST_CAST(                               \
        BOOST_PP_ARRAY_ELEM(type_index, TYPES),    \
        BOOST_PP_ARRAY_ELEM(value_index, VALUES)   \
    )                                              \
/**/

#define EACH_TYPE1(z, type_index, nothing)         \
    BOOST_PP_REPEAT(                               \
        BOOST_PP_ARRAY_SIZE(VALUES),               \
        EACH_VALUE,                                \
        type_index                                 \
    )                                              \
/**/

int main(int argc, char *argv[]){
    bool rval = true;
    BOOST_PP_REPEAT(
        BOOST_PP_ARRAY_SIZE(TYPES),
        EACH_TYPE1,
        nothing
    )
    return rval ? EXIT_SUCCESS : EXIT_FAILURE;
}
