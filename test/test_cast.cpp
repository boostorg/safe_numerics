//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <exception>
#include <cstdlib> // EXIT_SUCCESS

#include "../include/safe_compare.hpp"
#include "../include/safe_integer.hpp"

using namespace boost::numeric;

// test conversion to T2 from different literal types
template<class T2, class T1>
bool test_cast(T1 v1, const char *t2_name, const char *t1_name){
    std::cout
        << "testing static_cast<safe<" << t2_name << ">(" << t1_name << ")"
        << std::endl;
{

    /* test conversion constructor to safe<T2> from v1 */
    safe<T2> s2;
    try{
        s2 = static_cast<safe<T2>>(v1);
        if(! safe_compare::equal(base_value(s2), v1)){
            std::cout
                << "failed to detect error in construction "
                << t2_name << "<-" << t1_name
                << std::endl;
            try{
                s2 = static_cast<safe<T2> >(v1);
            }
            catch(std::exception){}
            return false;
        }
    }
    catch(std::exception){
        if( safe_compare::equal(base_value(s2), v1)){
            std::cout
                << "erroneously emitted error "
                << t1_name << "<-" << t2_name
                << std::endl;
            try{
                s2 = static_cast<safe<T2> >(v1);
            }
            catch(std::exception){}
            return false;
        }
    }
}
{
    /* test conversion to T1 from safe<T2>(v2) */
    safe<T1> s1(v1);
    T2 t2;
    try{
        t2 = static_cast<T2>(s1);
        if(! safe_compare::equal(t2, v1)){
            std::cout
                << "failed to detect error in construction "
                << t2_name << "<-" << t1_name
                << std::endl;
            try{
                t2 = static_cast<T2>(s1);
            }
            catch(std::exception){}
            return false;
        }
    }
    catch(std::exception){
        if(safe_compare::equal(t2, v1)){
            std::cout
                << "erroneously emitted error "
                << t1_name << "<-" << t2_name
                << std::endl;
            try{
                t2 = static_cast<T2>(s1);
            }
            catch(std::exception){}
            return false;
        }
    }
    return true; // passed test
}
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

int main(int, char *[]){
    bool rval = true;
    BOOST_PP_REPEAT(
        BOOST_PP_ARRAY_SIZE(TYPES),
        EACH_TYPE1,
        nothing
    )
    std::cout << (rval ? "success!" : "failure") << std::endl;
    return rval ? EXIT_SUCCESS : EXIT_FAILURE;
}
