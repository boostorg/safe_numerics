//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// test construction conversions

#include <iostream>
#include <exception>

#include "../include/safe_compare.hpp"
#include "../include/safe_integer.hpp"

// test conversion to T2 from different literal types
template<class T2, class T1>
bool test_conversion(T1 v1, const char *t2_name, const char *t1_name){
    /* test conversion constructor to T1 */
    boost::numeric::safe<T2> t2;
    try{
        t2 = v1;
        if(! boost::numeric::safe_compare::equal(t2, v1)){
            std::cout
                << "failed to detect error in construction "
                << t2_name << "<-" << t1_name
                << std::endl;
            try{
                t2 = v1;
            }
            catch(...){}
            return false;
        }
    }
    catch(std::exception & e){
        if(boost::numeric::safe_compare::equal(t2, v1)){
            std::cout
                << "failed to detect error in construction "
                << t2_name << "<-" << t2_name
                << std::endl;
            try{
                t2 = v1;
            }
            catch(...){}
            return false;
        }
    }
    boost::numeric::safe<T1> t1 = v1;
    try{
        t2 = t1;
        if(! boost::numeric::safe_compare::equal(t2, t1)){
            std::cout
                << "failed to detect error in construction "
                << t2_name << "<-" << t1_name
                << std::endl;
            try{
                t2 = t1;
            }
            catch(...){}
            return false;
        }
    }
    catch(std::exception & e){
        if(boost::numeric::safe_compare::equal(t2, t1)){
            std::cout
                << "failed to detect error in construction "
                << t2_name << "<-" << t2_name
                << std::endl;
            try{
                t2 = t1;
            }
            catch(...){}
            return false;
        }
    }
    return true; // passed test
}

#include "test.hpp"
#include "test_types.hpp"
#include "test_values.hpp"

#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/array/elem.hpp>
#include <boost/preprocessor/array/size.hpp>
#include <boost/preprocessor/stringize.hpp>

#define TEST_CONVERSION(T1, v)        \
    test_conversion<T1>(              \
        v,                            \
        BOOST_PP_STRINGIZE(T1),       \
        BOOST_PP_STRINGIZE(v)         \
    );
/**/

#define EACH_VALUE(z, value_index, type_index)     \
    TEST_CONVERSION(                               \
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
    BOOST_PP_REPEAT(
        BOOST_PP_ARRAY_SIZE(TYPES),
        EACH_TYPE1,
        nothing
    )
    return 0;
}

