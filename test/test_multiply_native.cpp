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

// note: These tables presume that the the size of an int is 32 bits.
// This should be changed for a different architecture or better yet
// be dynamically adjusted depending on the indicated architecture

const char *test_multiplication_result[boost::mp11::mp_size<test_values>::value] = {
//      0       0       0       0
//      012345670123456701234567012345670
//      012345678901234567890123456789012
/* 0*/ ".................................",
/* 1*/ ".........xx..xx..........xxx.xxx.",
/* 2*/ ".........xx..xx.........xxxxxxxx.",
/* 3*/ "..........x...x.........xxxxxxxx.",
/* 4*/ ".................................",
/* 5*/ ".........xx..xx..........xxx.xxx.",
/* 6*/ ".........xx..xx.........xxxxxxxx.",
/* 7*/ "..........x...x.........xxxxxxxx.",

/* 8*/ ".................................",
/* 9*/ ".xx..xx..xx..xx..xxx.xxx.xxx.xxx.",
/*10*/ ".xxx.xxx.xxx.xx..xxx.xxxxxxxxxxx.",
/*11*/ "..........x...x.........xxxxxxxx.",
/*12*/ ".................................",
/*13*/ ".xx..xx..xx..xx..xxx.xxx.xxx.xxx.",
/*14*/ ".xxx.xxx.xxx.xxx.xxx.xxx.xxxxxxx.",
/*15*/ "..............x.............xxxx.",

//      0       0       0       0
//      012345670123456701234567012345670
//      012345678901234567890123456789012
/*16*/ ".................................",
/*17*/ ".........xx..xx..........xxx.xxx.",
/*18*/ ".........xx..xx..........xxx.xxx.",
/*19*/ ".........xx..xx..........xxx.xxx.",
/*20*/ ".................................",
/*21*/ ".........xx..xx..........xxx.xxx.",
/*22*/ ".........xx..xx..........xxx.xxx.",
/*23*/ ".........xx..xx........x.xxx.xxx.",

/*24*/ "..xx..xx..xx.....................",
/*25*/ ".xxx.xxx.xxx.xx..xxx.xxx.xxx.xxx.",
/*26*/ ".xxx.xxx.xxx.xx..xxx.xxx.xxx.xxx.",
/*27*/ ".xxx.xxx.xxx.xx..xxx.xxx.xxx.xxx.",
/*28*/ "..xx..xx..xx..xx.................",
/*29*/ ".xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx.",
/*30*/ ".xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx.",
/*31*/ ".xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx.",
/*31*/ "................................."
};

#include <boost/mp11/algorithm.hpp>
#include <boost/core/demangle.hpp>

template <class T>
using safe_t = boost::safe_numerics::safe<
    T,
    boost::safe_numerics::native
>;
#include "test_multiply.hpp"

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
        m_error &= test_multiply<T1, T2>(
            boost::mp11::mp_at_c<L, i1>(), // value of first argument
            boost::mp11::mp_at_c<L, i2>(), // value of second argument
            boost::core::demangle(typeid(T1).name()).c_str(),
            boost::core::demangle(typeid(T2).name()).c_str(),
            test_multiplication_result[i1][i2]
        );
    }
};

#include "check_symmetry.hpp"

int main(){
    // sanity check on test matrix - should be symetrical
    check_symmetry(test_multiplication_result);
    //TEST_EACH_VALUE_PAIR
    test<test_values> rval(true);

    using value_indices = mp_iota_c<mp_size<test_values>::value>;
    mp_for_each<
        mp_product<mp_list, value_indices, value_indices>
    >(rval);

    std::cout << (rval ? "success!" : "failure") << std::endl;
    return ! rval ;
}
