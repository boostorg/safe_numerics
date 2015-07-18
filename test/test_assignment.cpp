//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// test construction assignments

#include <iostream>
#include <exception>

#include "../include/safe_compare.hpp"
#include "../include/safe_integer.hpp"

template<class T2, class T1>
bool test_assignment(T1 v1, const char *t2_name, const char *t1_name){
    std::cout
        << "testing assignments to " << t2_name << " from " << t1_name
        << std::endl;
{
    /* (1) test assignment to safe<T1> from T1 type */
    boost::numeric::safe<T1> s1;
    try{
        s1 = v1;
        // should always arrive here!
    }
    catch(std::exception){
        // should never, ever arrive here
        std::cout
            << "erroneously detected error in assignment "
            << "safe<" << t1_name << "> = " << t1_name
            << std::endl;
        try{
            s1 = v1; // try again for debugging
        }
        catch(std::exception){}
        return false;
    }
}
{
    /* (2) test assignment to safe<T2> from T1 type */
    boost::numeric::safe<T2> s2;
    try{
        s2 = v1;
        T2 v2 = s2;
        if(! (boost::numeric::safe_compare::equal(v1, v2))){
            std::cout
                << "failed to detect error in assignment "
                << "safe<" << t2_name << "> = " << t1_name
                << std::endl;
            s2 = v1;
            return false;
        }
    }
    catch(std::exception){
        T2 v2 = v1;
        if(boost::numeric::safe_compare::equal(v1, v2)){
            std::cout
                << "erroneously detected error in assignment "
                << "safe<" << t2_name << "> = " << t1_name
                << std::endl;
            try{
                s2 = v1; // try again for debugging
            }
            catch(std::exception){}
            return false;
        }
    }
}
{
    /* (3) test assignment to safe<T1> from safe<T1> type */
    boost::numeric::safe<T1> s1x(v1);
    boost::numeric::safe<T1> s1;
    try{
        s1 = s1x;
        if(! (s1 == s1x)){
            std::cout
                << "assignment altered value "
                << "safe<" << t1_name << "> = safe<" << t1_name << ">"
                << std::endl;
            s1 = s1x;
            return false;
        }
    }
    catch(std::exception){
        // should never arrive here
        std::cout
            << "erroneously detected error in assignment "
            << "safe<" << t1_name << "> = safe<" << t1_name << ">"
            << std::endl;
        try{
            s1 = s1x;
        }
        catch(std::exception){}
        return false;
    }
}
{
    /* (4) test assignment to safe<T2> from safe<T1> type */
    boost::numeric::safe<T1> s1(v1);
    boost::numeric::safe<T2> s2;
    T2 v2;
    try{
        s2 = s1;
        v2 = s2;
        if(! (boost::numeric::safe_compare::equal(v1, v2))){
            std::cout
                << "failed to detect error in assignment "
                << "safe<" << t2_name << "> = safe<" << t1_name << ">"
                << std::endl;
            s2 = s1;
            return false;
        }
    }
    catch(std::exception){
        if(boost::numeric::safe_compare::equal(v1, v2)){
            std::cout
                << "erroneously detected error in assignment "
                << "safe<" << t2_name << "> = safe<" << t1_name << ">"
                << std::endl;
            try{
                s2 = s1;
            }
            catch(std::exception){}
            return false;
        }
    }
}
{
    /* (5) test assignment to integer type T1 from safe<T1> type */
    boost::numeric::safe<T1> s1(v1);
    T1 t1;
    try{
        t1 = s1;
        if(! (t1 == s1)){
            std::cout
                << "failed to detect error in assignment "
                << t1_name << "=" << "safe<" << t1_name << ">"
                << std::endl;
            t1 = s1;
            return false;
        }
    }
    catch(std::exception){
        if(t1 == s1){
            std::cout
                << "erroneously detected error in assignment "
                << t1_name << "<-" << "safe<" << t1_name << ">"
                << std::endl;
            try{
                t1 = s1; // try again for debugging
            }
            catch(std::exception){}
            return false;
        }
    }
}
{
    /* (6) test assignment to integer type T2 from safe<T1> type */
    boost::numeric::safe<T1> s1(v1);
    T2 t2;
    try{
        t2 = s1;
        if(! (t2 == s1)){
            std::cout
                << "failed to detect error in assignment "
                << t2_name << "=" << "safe<" << t1_name << ">"
                << std::endl;
            try{
                t2 = s1;
            }
            catch(std::exception){}
            return false;
        }
    }
    catch(std::exception){
        if(t2 == s1){
            std::cout
                << "erroneously detected error in assignment "
                << t2_name << "=" << "safe<" << t1_name << ">"
                << std::endl;
            try{
                t2 = s1;
            }
            catch(std::exception){}
            return false;
        }
    }
}
    return true; // passed test
}

#include "test.hpp"
#include "test_types.hpp"
#include "test_values.hpp"

#define TEST_ASSIGNMENT(T1, v)        \
    test_assignment<T1>(              \
        v,                            \
        BOOST_PP_STRINGIZE(T1),       \
        BOOST_PP_STRINGIZE(v)         \
    );
/**/

#define EACH_VALUE(z, value_index, type_index)     \
    TEST_ASSIGNMENT(                               \
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

