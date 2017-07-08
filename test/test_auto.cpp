#include <iostream>
#include <cassert>
#include "../include/safe_integer.hpp"
#include "../include/safe_integer_range.hpp"
#include "../include/automatic.hpp"
#include "../include/utility.hpp"

int test_log(){
    using namespace boost::numeric::utility;
    assert(log(127u) == 6);
    assert(log(128u) == 7);
    assert(log(129u) == 7);
    assert(log(255u) == 7);
    assert(log(256u) == 8);

    assert(log(127) == 6);
    assert(log(128) == 7);
    assert(log(129) == 7);
    assert(log(255) == 7);
    assert(log(256) == 8);
    return 0;
}

template<class T>
void print_argument_type(const T & t){
    const std::type_info & ti = typeid(T);
    std::cout
        << boost::core::demangle(ti.name()) << ' ' << t << std::endl;
}

template<typename T, typename U>
int test_auto(const T & t, const U & u){
    using namespace boost::numeric;

    try{
        safe<T, automatic>(t) + u;
    }
    catch(std::exception e){
        safe<T, automatic>(t) + u;
    }

    try{
        t + safe<U, automatic>(u);
    }
    catch(std::exception e){
        t + safe<U, automatic>(u);
    }


    return 0;
}

int test_addition_result(){
    using namespace boost::numeric;
    automatic::addition_result<
        safe<std::int8_t, automatic>,
        safe<std::uint16_t, automatic>
    >::type r1;
    print_argument_type(r1);

    automatic::addition_result<
        safe_signed_range<-3, 8, automatic>,
        safe_signed_range<-4, 9, automatic>
    >::type r2;
    print_argument_type(r2);
    return 0;
}

int test_compare_result(){
    using namespace boost::numeric;

    automatic::comparison_result<
        safe<std::int8_t, automatic>,
        safe<std::uint64_t, automatic>
    >::type r1;
    print_argument_type(r1);

    safe<std::int8_t, automatic> x(0x01);
    safe<std::uint64_t, automatic> y(0x7fffffffffffffff);
    auto z = boost::numeric::safe_compare::less_than(x, y);

    print_argument_type(z);
    return 0;
}

int main(){
    using namespace boost::numeric;
    /*
    safe_signed_range<-3, 8, automatic> x = 5;
    safe_signed_range<-4, 9, automatic> y = 6;
    auto z = x + y;

    automatic::addition_result<
        safe_signed_range<-3, 8>,
        safe_signed_range<-4, 9>,
        automatic,
        default_exception_policy
    >::type x1;
    */

    test_log();
    test_auto<std::int8_t, std::int8_t>(1, -128);
    test_addition_result();
    test_compare_result();
    return 0;
}
