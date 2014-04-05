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
#include "../include/safe_compare.hpp"
#include "../include/safe_integer.hpp"

#include "test_types.hpp"
#include "test_values.hpp"

// test conversion to T2 from different literal types
template<class T1, class V>
bool test_cast(V v, const char *t_name, const char *v_name){
    /* test conversion constructor to T1 */
    T1 t1;
    try{
        t1 = boost::numeric::safe_cast<T1>(v);
        if(! boost::numeric::safe_compare::equal(t1, v)){
            std::cout
                << "failed to detect error in construction "
                << t_name << "<-" << v_name
                << std::endl;
            return false;
        }
    }
    catch(std::range_error e){
        if(boost::numeric::safe_compare::equal(t1, v)){
            std::cout
                << "failed to detect error in construction "
                << t_name << "<-" << v_name
                << std::endl;
            return false;
        }
    }
    return true; // passed test
}

#define TEST_CAST(T1, v)        \
    test_cast<T1>(              \
        v,                      \
        BOOST_PP_STRINGIZE(T1), \
        BOOST_PP_STRINGIZE(v)   \
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
    BOOST_PP_REPEAT(
        BOOST_PP_ARRAY_SIZE(TYPES),
        EACH_TYPE1,
        nothing
    )
    return 0;
}
