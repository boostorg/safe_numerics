//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>

#include <boost/safe_numerics/safe_integer.hpp>
#include <boost/safe_numerics/native.hpp>

#include <boost/mp11/list.hpp>
#include <boost/mp11/algorithm.hpp>
#include "test_values.hpp"

// note: same test matrix as used in test_checked.  Here we test all combinations
// safe and unsafe integers.  in test_checked we test all combinations of
// integer primitives

const char *test_addition_result[boost::mp11::mp_size<test_values>::value] = {
//      0       0       0       0
//      012345670123456701234567012345670
//      012345678901234567890123456789012
/* 0*/ ".........x...x.............x...x.",
/* 1*/ ".........x...x.............x...x.",
/* 2*/ "..........x...x.........xxxxxxxx.",
/* 3*/ "..........x...x.........xxxxxxxx.",
/* 4*/ ".........x...x.............x...x.",
/* 5*/ ".........x...x.............x...x.",
/* 6*/ "..........x...x.........xxxxxxxx.",
/* 7*/ "..........x...x.........xxxxxxxx.",

/* 8*/ ".........x...x.............x...x.",
/* 9*/ "xx..xx..xx...x..xxxxxxxx...x...x.",
/*10*/ "..xx..xx..xx..x.........xxxxxxxx.",
/*11*/ "..........x...x.........xxxxxxxx.",
/*12*/ ".............x.................x.",
/*13*/ "xx..xx..xx..xx..xxxxxxxxxxxx...x.",
/*14*/ "..xx..xx..xx..xx............xxxx.",
/*15*/ "..............x.............xxxx.",

//      0       0       0       0
//      012345670123456701234567012345670
//      012345678901234567890123456789012
/*16*/ ".........x...x.............x...x.",
/*17*/ ".........x...x.............x...x.",
/*18*/ ".........x...x.............x...x.",
/*19*/ ".........x...x.............x...x.",
/*20*/ ".........x...x.............x...x.",
/*21*/ ".........x...x.............x...x.",
/*22*/ ".........x...x.............x...x.",
/*23*/ ".........x...x.............x...x.",

/*24*/ "..xx..xx..xx.x.............x...x.",
/*25*/ "..xx..xx..xx.x.............x...x.",
/*26*/ "..xx..xx..xx.x............xx...x.",
/*27*/ "xxxxxxxxxxxx.x..xxxxxxxxxxxx...x.",
/*28*/ "..xx..xx..xx..xx...............x.",
/*29*/ "..xx..xx..xx..xx...............x.",
/*30*/ "..xx..xx..xx..xx..............xx.",
/*31*/ "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.",
/*32*/ "................................."
};

#include <boost/mp11/algorithm.hpp>
#include <boost/core/demangle.hpp>

template <class T>
using safe_t = boost::safe_numerics::safe<
    T,
    boost::safe_numerics::native
>;
#include "test_add.hpp"

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
        m_error &= test_add<T1, T2>(
            boost::mp11::mp_at_c<L, i1>(), // value of first argument
            boost::mp11::mp_at_c<L, i2>(), // value of second argument
            boost::core::demangle(typeid(T1).name()).c_str(),
            boost::core::demangle(typeid(T2).name()).c_str(),
            test_addition_result[i1][i2]
        );
    }
};

#include "check_symmetry.hpp"

int main(){
    // sanity check on test matrix - should be symetrical
    check_symmetry(test_addition_result);

    //TEST_EACH_VALUE_PAIR
    test<test_values> rval(true);

    using value_indices = mp_iota_c<mp_size<test_values>::value>;
    mp_for_each<
        mp_product<mp_list, value_indices, value_indices>
    >(rval);

    std::cout << (rval ? "success!" : "failure") << std::endl;
    return ! rval ;
}
