//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <cstdlib> // EXIT_SUCCESS

#include <boost/cstdint.hpp>
#include <boost/limits.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/array/elem.hpp>
#include <boost/preprocessor/array/size.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <boost/type_traits/is_signed.hpp>

#include "../include/safe_cast.hpp"
#include "../include/safe_compare.hpp"
#include "../include/safe_integer.hpp"

#include "test_types.hpp"
#include "test_values.hpp"

// test conversion to T2 from different literal types
template<class T2, class T1>
bool test_cast(T1 v1, const char *t2_name, const char *t1_name){
    /* test conversion constructor to T1 */
    T2 v2;
    try{
        v2 = boost::numeric::safe_cast<T2>(v1);
        if(! boost::numeric::safe_compare::equal(v2, v1)){
            std::cout
                << "failed to detect error in construction "
                << t2_name << "<-" << t1_name
                << std::endl;
            try{
                v2 = boost::numeric::safe_cast<T2>(v1);
            }
            catch(...){}
            return false;
        }
    }
    catch(std::range_error e){
        if(boost::numeric::safe_compare::equal(v2, v1)){
            std::cout
                << "failed to detect error in construction "
                << t1_name << "<-" << t2_name
                << std::endl;
            try{
                v2 = boost::numeric::safe_cast<T2>(v1);
            }
            catch(...){}
            return false;
        }
    }
    return true; // passed test
}

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
