//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// test constructors

#include <iostream>
#include <exception>

#include "../include/safe_compare.hpp"
#include "../include/safe_integer.hpp"

template<class T2, class T1>
bool test_construction(T1 v1, const char *t2_name, const char *t1_name){
    std::cout
        << "testing constructions to " << t2_name << " from " << t1_name
        << std::endl;
{
    /* (1) test construction of safe<T1> from T1 type */
    try{
        boost::numeric::safe<T1> s1(v1);
        // should always arrive here!
    }
    catch(std::exception){
        // should never, ever arrive here
        std::cout
            << "erroneously detected error in construction "
            << "safe<" << t1_name << "> (" << t1_name << ")"
            << std::endl;
        try{
            boost::numeric::safe<T1> s1(v1); // try again for debugging
        }
        catch(std::exception){}
        return false;
    }
}
{
    /* (2) test construction of safe<T2> from T1 type */
    try{
        boost::numeric::safe<T2> s2(v1);
        T2 v2 = v1;
        if(! boost::numeric::safe_compare::equal(v1, v2)){
            std::cout
                << "failed to detect error in construction "
                << "safe<" << t2_name << "> (" << t1_name << ")"
                << std::endl;
            boost::numeric::safe<T2> s2(v1);
            return false;
        }
    }
    catch(std::exception){
        T2 v2 = v1;
        if(boost::numeric::safe_compare::equal(v1, v2)){
            std::cout
                << "erroneously detected error in construction "
                << "safe<" << t2_name << "> (" << t1_name << ")"
                << std::endl;
            try{
                boost::numeric::safe<T2> s2(v1); // try again for debugging
            }
            catch(std::exception){}
            return false;
        }
    }
}
{
    /* (3) test construction of safe<T1> from safe<T1> type */
    boost::numeric::safe<T1> s1x(v1);
    try{
        boost::numeric::safe<T1> s1(s1x);
        if(! (s1 == s1x)){
            std::cout
                << "copy constructor altered value "
                << "safe<" << t1_name << "> (safe<" << t1_name << ">(" << v1 << "))"
                << std::endl;
            //boost::numeric::safe<T1> s1(s1x);
            return false;
        }
    }
    catch(std::exception){
        // should never arrive here
        std::cout
            << "erroneously detected error in construction "
            << "safe<" << t1_name << "> (safe<" << t1_name << ">(" << v1 << "))"
            << std::endl;
        try{
            //boost::numeric::safe<T1> s1(s1x);
        }
        catch(std::exception){}
        return false;
    }
}
{
    /* (4) test construction of safe<T2> from safe<T1> type */
    T2 v2;
    try{
        boost::numeric::safe<T1> s1(v1);
        boost::numeric::safe<T2> s2 = s1;
        v2 = s2;
        if(! (boost::numeric::safe_compare::equal(v1, v2))){
            std::cout
                << "failed to detect error in construction "
                << "safe<" << t1_name << "> (safe<" << t1_name << ">(" << v1 << "))"
                << std::endl;
            boost::numeric::safe<T2> s1(v1);
            return false;
        }
    }
    catch(std::exception){
        if(boost::numeric::safe_compare::equal(v1, v2)){
            std::cout
                << "erroneously detected error in construction "
                << "safe<" << t2_name << "> (safe<" << t1_name << ">(" << v1 << "))"
                << std::endl;
            try{
                boost::numeric::safe<T2> s1(v1);
            }
            catch(std::exception){}
            return false;
        }
    }
}
    return true;
}

#include "test.hpp"
#include "test_types.hpp"
#include "test_values.hpp"

#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/array/elem.hpp>
#include <boost/preprocessor/array/size.hpp>
#include <boost/preprocessor/stringize.hpp>

#define TEST_CONSTRUCTION(T1, v)      \
    test_construction<T1>(            \
        v,                            \
        BOOST_PP_STRINGIZE(T1),       \
        BOOST_PP_STRINGIZE(v)         \
    );
/**/

#define EACH_VALUE(z, value_index, type_index)     \
    TEST_CONSTRUCTION(                             \
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

