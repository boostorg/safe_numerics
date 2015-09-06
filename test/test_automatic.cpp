#include <iostream>
#include <cstdint>
#include <limits>
#include <cxxabi.h>

#include "../include/automatic.hpp"

/*
namespace boost { namespace numeric {

struct automatic {
    template<typename T, typename U, typename P, typename E>
    struct division_result {
        static const T t = '0';
        static const U u = 31;
    };
};

} // numeric
} // boost
*/

template<typename T, typename U>
bool test(){
    int status;
    const std::type_info & ti_t = typeid(T);
    const std::type_info & ti_u = typeid(U);
    std::cout
        << "test<"
        << abi::__cxa_demangle(ti_t.name(),0,0,&status)
        << ','
        << abi::__cxa_demangle(ti_u.name(),0,0,&status)
        << ">\n";
    using namespace boost::numeric;
    
    using division_result_alias = automatic::division_result<
        T, U, automatic, void
    >;

    const auto t = division_result_alias::t();
    const auto u = division_result_alias::u();

    std::cout << "t = " << t << '\n';
    std::cout << "u = " << u << '\n';

    if(std::numeric_limits<U>::is_signed)
        std::cout << "r_lower(u()) = " << division_result_alias::r_lower(u) << '\n';
    std::cout << "r_upper(u()) = " << division_result_alias::r_upper(u) << '\n';

    using max_t = typename division_result_alias::max_t;
    std::cout << division_result_alias::template r<max_t>(t, u) << '\n';
    return true;
}

#include "test.hpp"

#define VALUES (8, ( \
    std::int8_t,    \
    std::int16_t,   \
    std::int32_t,   \
    std::int64_t,   \
    std::uint8_t,   \
    std::uint16_t,  \
    std::uint32_t,  \
    std::uint64_t   \
))

const char *test_result[VALUE_ARRAY_SIZE] = {
//      01234567
/* 0*/ "........",
/* 1*/ "........",
/* 2*/ "........",
/* 3*/ "........",
/* 4*/ "........",
/* 5*/ "........",
/* 6*/ "........",
/* 7*/ "........"
};

#define TEST_IMPL(v1, v2, result) \
    rval &= test<v1, v2>();
/**/

#define TESTX(value_index1, value_index2)          \
    (std::cout << value_index1 << ',' << value_index2 << ','); \
    TEST_IMPL(                                     \
        BOOST_PP_ARRAY_ELEM(value_index1, VALUES), \
        BOOST_PP_ARRAY_ELEM(value_index2, VALUES), \
        test_result[value_index1][value_index2]() \
    )
/**/

#if 0
int main(){
    test<std::int8_t, std::uint8_t>();
    return 0;
}
#else
int main(){
    bool rval = true;
    TEST_EACH_VALUE_PAIR
    return ! rval ;
}
#endif

