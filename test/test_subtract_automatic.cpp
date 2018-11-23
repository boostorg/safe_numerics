//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>

#include <boost/safe_numerics/safe_integer.hpp>
#include <boost/safe_numerics/automatic.hpp>

#include <boost/mp11/list.hpp>
#include <boost/mp11/algorithm.hpp>
#include "test_values.hpp"

// note: same test matrix as used in test_checked.  Here we test all combinations
// safe and unsafe integers.  in test_checked we test all combinations of
// integer primitives

const char *test_subtraction_result[boost::mp11::mp_size<test_values>::value] = {
//      0       0       0       0
//      012345670123456701234567012345670
//      012345678901234567890123456789012
/* 0*/ "..............x...............xx.",
/* 1*/ "..............x...............xx.",
/* 2*/ ".............x...............xxx.",
/* 3*/ "..............................xx.",
/* 4*/ "..............x...............xx.",
/* 5*/ "..............x...............xx.",
/* 6*/ ".............x...............xxx.",
/* 7*/ "..............................xx.",

/* 8*/ "..............x...............xx.",
/* 9*/ "..............x...............xx.",
/*10*/ ".............x...............xxx.",
/*11*/ "..............................xx.",
/*12*/ "..............x...............xx.",
/*13*/ "..xx..xx..xx..xx..............xx.",
/*14*/ "xx..xx..xx..xx..xxxxxxxxxxxxxxxx.",
/*15*/ "..............................xx.",

//      0       0       0       0
//      012345670123456701234567012345670
//      012345678901234567890123456789012
/*16*/ "..............x...............xx.",
/*17*/ "..............x...............xx.",
/*18*/ "..............x...............xx.",
/*19*/ "..............x...............xx.",
/*20*/ "..............x...............xx.",
/*21*/ "..............x...............xx.",
/*22*/ "..............x...............xx.",
/*23*/ "..............x...............xx.",

/*24*/ "..............x...............xx.",
/*25*/ "..............x...............xx.",
/*26*/ "..............x...............xx.",
/*27*/ "..............x...............xx.",
/*28*/ "..............x...............xx.",
/*29*/ "..xx..xx..xx..xx..............xx.",
/*30*/ "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
/*31*/ "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
/*32*/ "..............x...............xx."
};

#include <boost/mp11/algorithm.hpp>
#include <boost/core/demangle.hpp>

template <class T>
using safe_t = boost::safe_numerics::safe<
    T,
    boost::safe_numerics::automatic
>;
#include "test_subtract.hpp"

using namespace boost::mp11;

template<typename L>
struct test {
    static_assert(mp_is_list<L>(), "must be a list of integral constants");
    bool m_error;
    test(bool b = true) : m_error(b) {}
    operator bool(){
        return m_error;
    }
    template<typename T>
    void operator()(const T &){
        static_assert(mp_is_list<T>(), "must be a list of two integral constants");
        constexpr size_t i1 = mp_first<T>(); // index of first argument
        constexpr size_t i2 = mp_second<T>();// index of second argument
        std::cout << i1 << ',' << i2 << ',';
        using T1 = typename boost::mp11::mp_at_c<L, i1>::value_type;
        using T2 = typename boost::mp11::mp_at_c<L, i2>::value_type;
        m_error &= test_subtract<T1, T2>(
            boost::mp11::mp_at_c<L, i1>(), // value of first argument
            boost::mp11::mp_at_c<L, i2>(), // value of second argument
            boost::core::demangle(typeid(T1).name()).c_str(),
            boost::core::demangle(typeid(T2).name()).c_str(),
            test_subtraction_result[i1][i2]
        );
    }
};

int main(){
    //TEST_EACH_VALUE_PAIR
    test<test_values> rval(true);

    using value_indices = mp_iota_c<mp_size<test_values>::value>;
    mp_for_each<
        mp_product<mp_list, value_indices, value_indices>
    >(rval);

    std::cout << (rval ? "success!" : "failure") << std::endl;
    return ! rval ;
}
