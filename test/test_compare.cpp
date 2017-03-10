// Copyright (c) 2014 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <cassert>
#include <typeinfo>

#include "../include/safe_integer.hpp"

#ifdef __GNU_C__

#include <cxxabi.h> 

template<class T1, class T2>
void print_argument_types(
    T1 v1,
    T2 v2
){
    const std::type_info & ti1 = typeid(v1);
    const std::type_info & ti2 = typeid(v2);
    int status;

    std::cout
        << abi::__cxa_demangle(ti1.name(),0,0,&status) << ','
        << abi::__cxa_demangle(ti2.name(),0,0,&status) << ',';
}
#else

template<class T1, class T2>
void print_argument_types(
    T1 v1,
    T2 v2
){
    const std::type_info & ti1 = typeid(v1);
    const std::type_info & ti2 = typeid(v2);
    int status;

    std::cout
        << ti1.name() << ','
        << ti2.name() << ',';
}
#endif

template<class T1, class T2>
bool test_compare_detail(
    T1 v1,
    T2 v2,
    char expected_result
){
    print_argument_types(v1, v2);
    switch(expected_result){
    case '=': {
        if(! (v1 == v2))
            return false;
        if(v1 < v2)
            return false;
        if(v1 > v2)
            return false;
        break;
    }
    case '<': {
        if(! (v1 < v2))
            return false;
        if(v1 > v2)
            return false;
        if(v1 == v2)
            return false;
        break;
    }
    case '>':{
        if(! (v1 > v2))
            return false;
        if(v1 < v2)
            return false;
        if(v1 == v2)
            return false;
        break;
    }
    }
    return true;
}


template<class T1, class T2>
bool test_compare(
    T1 v1,
    T2 v2,
    const char *av1,
    const char *av2,
    char expected_result
){
    std::cout
        << "testing  "
        << av1 << ' ' << expected_result << ' ' << av2
        << std::endl;

    boost::numeric::safe<T1> t1 = v1;
    if(!test_compare_detail(t1, v2, expected_result)){
        std::cout
            << "error "
            << "safe(" << av1 << ')' << ' ' << expected_result << ' ' << av2
            << std::endl;
        test_compare_detail(t1, v2, expected_result);
        return false;
    }

    boost::numeric::safe<T2> t2 = v2;
    if(!test_compare_detail(v1, t2, expected_result)){
        std::cout
            << "error "
            << "safe(" << av1 << ')' << ' ' << expected_result << ' ' << av2
            << std::endl;
        test_compare_detail(v1, t2, expected_result);
        return false;
    }

    if(!test_compare_detail(t1, t2, expected_result)){
        std::cout
            << "error "
            << "safe(" << av1 << ')' << ' ' << expected_result << ' ' << av2
            << std::endl;
        test_compare_detail(t1, t2, expected_result);
        return false;
    }
    std::cout << std::endl;
    return true; // correct result
}

#include "test.hpp"
#include "test_values.hpp"

const char *test_compare_result[VALUE_ARRAY_SIZE] = {
//      0       0       0       0
//      01234567012345670123456701234567
//      01234567890123456789012345678901
/* 0*/ "=<>>=<>>=<>>=<>>=<<<=<<<=<<<=<<<",
/* 1*/ ">=>>><>>><>>><>>>=<<><<<><<<><<<",
/* 2*/ "<<=<<<><<<><<<><<<<<<<<<<<<<<<<<",
/* 3*/ "<<>=<<>=<<>=<<>=<<<<<<<<<<<<<<<<",
/* 4*/ "=<>>=<>>=<>>=<>>=<<<=<<<=<<<=<<<",
/* 5*/ ">>>>>=>>><>>><>>>>>>>=<<><<<><<<",
/* 6*/ "<<<<<<=<<<><<<><<<<<<<<<<<<<<<<<",
/* 7*/ "<<>=<<>=<<>=<<>=<<<<<<<<<<<<<<<<",

/* 8*/ "=<>>=<>>=<>>=<>>=<<<=<<<=<<<=<<<",
/* 9*/ ">>>>>>>>>=>>><>>>>>>>>>>>=<<><<<",
/*10*/ "<<<<<<<<<<=<<<><<<<<<<<<<<<<<<<<",
/*11*/ "<<>=<<>=<<>=<<>=<<<<<<<<<<<<<<<<",
/*12*/ "=<>>=<>>=<>>=<>>=<<<=<<<=<<<=<<<",
/*13*/ ">>>>>>>>>>>>>=>>>>>>>>>>>>>>>=<<",
/*14*/ "<<<<<<<<<<<<<<=<<<<<<<<<<<<<<<<<",
/*15*/ "<<>=<<>=<<>=<<>=<<<<<<<<<<<<<<<<",

//      0       0       0       0
//      01234567012345670123456701234567
//      01234567890123456789012345678901
/*16*/ "=<>>=<>>=<>>=<>>=<<<=<<<=<<<=<<<",
/*17*/ ">=>>><>>><>>><>>>=<<><<<><<<><<<",
/*18*/ ">>>>><>>><>>><>>>>=<><<<><<<><<<",
/*19*/ ">>>>><>>><>>><>>>>>=><<<><<<><<<",
/*20*/ "=<>>=<>>=<>>=<>>=<<<=<<<=<<<=<<<",
/*21*/ ">>>>>=>>><>>><>>>>>>>=<<><<<><<<",
/*22*/ ">>>>>>>>><>>><>>>>>>>>=<><<<><<<",
/*23*/ ">>>>>>>>><>>><>>>>>>>>>=><<<><<<",

/*24*/ "=<>>=<>>=<>>=<>>=<<<=<<<=<<<=<<<",
/*25*/ ">>>>>>>>>=>>><>>>>>>>>>>>=<<><<<",
/*26*/ ">>>>>>>>>>>>><>>>>>>>>>>>>=<><<<",
/*27*/ ">>>>>>>>>>>>><>>>>>>>>>>>>>=><<<",
/*28*/ "=<>>=<>>=<>>=<>>=<<<=<<<=<<<=<<<",
/*29*/ ">>>>>>>>>>>>>=>>>>>>>>>>>>>>>=<<",
/*30*/ ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>=<",
/*31*/ ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>="
};

#define TEST_IMPL(v1, v2, result) \
    rval &= test_compare(         \
        v1,                       \
        v2,                       \
        BOOST_PP_STRINGIZE(v1),   \
        BOOST_PP_STRINGIZE(v2),   \
        result                    \
    );
/**/

void break_check(unsigned int i, unsigned int j){
    std::cout << i << ',' << j << ',';
}

#define TESTX(value_index1, value_index2)          \
    break_check(value_index1, value_index2);       \
    TEST_IMPL(                                     \
        BOOST_PP_ARRAY_ELEM(value_index1, VALUES), \
        BOOST_PP_ARRAY_ELEM(value_index2, VALUES), \
        test_compare_result[value_index1][value_index2] \
    )
/**/

int main(int argc, char * argv[]){
    bool rval = true;
    TEST_EACH_VALUE_PAIR
    std::cout << (rval ? "success!" : "failure") << std::endl;
   return ! rval ;
}
