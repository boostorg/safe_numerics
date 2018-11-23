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

const char *test_subtraction_result[boost::mp11::mp_size<test_values>::value] = {
//      0       0       0       0
//      012345670123456701234567012345670
//      012345678901234567890123456789012
/* 0*/ "..........x...x..........xxx.xxx.",
/* 1*/ "..........x...x..........xxx.xxx.",
/* 2*/ ".........x...x..........xxxxxxxx.",
/* 3*/ "........................xxxxxxxx.",
/* 4*/ "..........x...x..........xxx.xxx.",
/* 5*/ "..........x...x..........xxx.xxx.",
/* 6*/ ".........x...x..........xxxxxxxx.",
/* 7*/ "........................xxxxxxxx.",

/* 8*/ "..........x...x..........xxx.xxx.",
/* 9*/ "..xx..xx..xx..x...........xx.xxx.",
/*10*/ "xx..xx..xx...x..xxxxxxxxxxxxxxxx.",
/*11*/ "........................xxxxxxxx.",
/*12*/ "..............x..............xxx.",
/*13*/ "..xx..xx..xx..xx..............xx.",
/*14*/ "xx..xx..xx..xx..xxxxxxxxxxxxxxxx.",
/*15*/ "............................xxxx.",

//      0       0       0       0
//      012345670123456701234567012345670
//      012345678901234567890123456789012
/*16*/ "..........x...x..........xxx.xxx.",
/*17*/ "..........x...x..........xxx.xxx.",
/*18*/ "..........x...x..........xxx.xxx.",
/*19*/ "..........x...x..........xxx.xxx.",
/*20*/ "..........x...x..........xxx.xxx.",
/*21*/ "..........x...x..........xxx.xxx.",
/*22*/ "..........x...x..........xxx.xxx.",
/*23*/ "..........x...x..........xxx.xxx.",

/*24*/ ".xxx.xxx.xxx..x..xxx.xxx.xxx.xxx.",
/*25*/ "..xx..xx..xx..x...........xx.xxx.",
/*26*/ "..xx..xx..xx..x............x.xxx.",
/*27*/ "..xx..xx..xx..x..............xxx.",
/*28*/ ".xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx.",
/*29*/ "..xx..xx..xx..xx..............xx.",
/*30*/ "..xx..xx..xx..xx...............x.",
/*31*/ "..xx..xx..xx..xx.................",
/*32*/ "..........x...x.........xxxxxxxx."
};

#include <boost/mp11/algorithm.hpp>
#include <boost/core/demangle.hpp>

template <class T>
using safe_t = boost::safe_numerics::safe<
    T,
    boost::safe_numerics::native
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
