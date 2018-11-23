//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <cstdlib> // EXIT_SUCCESS

#include <boost/safe_numerics/checked_integer.hpp>

// test conversion to T2 from different literal types
template<class T2, class T1>
bool test_cast(
    const T1 & v1,
    const char *t2_name,
    const char *t1_name,
    char expected_result
){
    std::cout
        << "testing static_cast<" << t2_name << ">(" << t1_name << ")"
        << std::endl;

    boost::safe_numerics::checked_result<T2> r2 = boost::safe_numerics::checked::cast<T2>(v1);

    if(expected_result == 'x' && ! r2.exception()){
        std::cout
            << "failed to detect error in construction "
            << t2_name << "<-" << t1_name
            << std::endl;
        boost::safe_numerics::checked::cast<T2>(v1);
        return false;
    }
    if(expected_result == '.' && r2.exception()){
        std::cout
            << "erroneously emitted error "
            << t2_name << "<-" << t1_name
            << std::endl;
        boost::safe_numerics::checked::cast<T2>(v1);
        return false;
    }
    return true; // passed test
}

#include <boost/mp11/list.hpp>
#include <boost/mp11/algorithm.hpp>
#include "test_values.hpp"

// note: the types indexed on the left side of the table are gathered
// by filtering the test_values list.  So the types are in the same
// sequence

const char *test_result[boost::mp11::mp_size<test_values>::value] = {
//      0       0       0       0
//      01234567012345670123456701234567
//      01234567890123456789012345678901
/* 0*/ ".....xx..xx..xx...xx.xxx.xxx.xxx",
/* 1*/ ".........xx..xx.......xx.xxx.xxx",
/* 2*/ ".............xx...........xx.xxx",
/* 3*/ "..............................xx",
/* 4*/ "..xx.xxx.xxx.xxx.....xxx.xxx.xxx",
/* 5*/ "..xx..xx.xxx.xxx.........xxx.xxx",
/* 6*/ "..xx..xx..xx.xxx.............xxx",
/* 7*/ "..xx..xx..xx..xx................"
};

#include <boost/mp11/algorithm.hpp>
#include <boost/core/demangle.hpp>

using namespace boost::mp11;

template<typename T>
using extract_value_type = typename T::value_type;
using test_types = mp_unique<
    mp_transform<
        extract_value_type,
        test_values
    >
>;

struct test {
    bool m_error;
    test(bool b = true) : m_error(b) {}
    operator bool(){
        return m_error;
    }
    template<typename T>
    void operator()(const T &){
        static_assert(mp_is_list<T>(), "must be a list of two types");
        constexpr size_t i1 = mp_first<T>(); // index of first argument
        constexpr size_t i2 = mp_second<T>();// index of second argument
        std::cout << i1 << ',' << i2 << ',';
        using T1 = boost::mp11::mp_at_c<test_types, i1>;
        using T2 = typename boost::mp11::mp_at_c<test_values, i2>::value_type;
        constexpr T2 v2 = typename boost::mp11::mp_at_c<test_values, i2>();
        m_error &= test_cast<T1>(
            v2,
            boost::core::demangle(typeid(T1).name()).c_str(),
            boost::core::demangle(typeid(T2).name()).c_str(),
            test_result[i1][i2]
        );
    }
};


int main(){
    test rval(true);

    using value_indices = mp_iota_c<mp_size<test_values>::value>;
    using type_indices = mp_iota_c<mp_size<test_types>::value>;
    mp_for_each<
        mp_product<mp_list, type_indices, value_indices>
    >(rval);

    std::cout << (rval ? "success!" : "failure") << std::endl;
    return ! rval ;
}
