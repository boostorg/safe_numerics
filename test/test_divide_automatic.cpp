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

const char *test_division_result[boost::mp11::mp_size<test_values>::value] = {
//      0       0       0       0
//      012345670123456701234567012345670
//      012345678901234567890123456789012
/* 0*/ "..............................xxx",
/* 1*/ "..............................xxx",
/* 2*/ "..............................xxx",
/* 3*/ "..............................xxx",
/* 4*/ "..............................xxx",
/* 5*/ "..............................xxx",
/* 6*/ "..............................xxx",
/* 7*/ "..............................xxx",

/* 8*/ "..............................xxx",
/* 9*/ "..............................xxx",
/*10*/ "..............................xxx",
/*11*/ "..............................xxx",
/*12*/ "..............................xxx",
/*13*/ "..............................xxx",
/*14*/ "...x...x...x...x..............xxx",
/*15*/ "..............................xxx",

//      0       0       0       0
//      012345670123456701234567012345670
//      012345678901234567890123456789012
/*16*/ "................................x",
/*17*/ "................................x",
/*18*/ "................................x",
/*19*/ "................................x",
/*20*/ "................................x",
/*21*/ "................................x",
/*22*/ "................................x",
/*23*/ "................................x",

/*24*/ "................................x",
/*25*/ "................................x",
/*26*/ "................................x",
/*27*/ "................................x",
/*28*/ "................................x",
/*29*/ "................................x",
/*30*/ "xxxxxxxxxxxxxxxx................x",
/*31*/ "xxxxxxxxxxxxxxxx................x",
/*32*/ "..............................xxx"
};

#include <boost/mp11/algorithm.hpp>
#include <boost/core/demangle.hpp>

template <class T>
using safe_t = boost::safe_numerics::safe<
    T,
    boost::safe_numerics::automatic
>;
#include "test_divide.hpp"

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
        m_error &= test_divide<T1, T2>(
            boost::mp11::mp_at_c<L, i1>(), // value of first argument
            boost::mp11::mp_at_c<L, i2>(), // value of second argument
            boost::core::demangle(typeid(T1).name()).c_str(),
            boost::core::demangle(typeid(T2).name()).c_str(),
            test_division_result[i1][i2]
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
