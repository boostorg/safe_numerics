#include <iostream>
#include <cassert>
#include "../include/safe_integer.hpp"
#include "../include/safe_range.hpp"
#include "../include/automatic.hpp"
#include "../include/utility.hpp"

int test_log(){
    using namespace boost::numeric::utility;
    assert(log(127u) == 7);
    assert(log(128u) == 8);
    assert(log(129u) == 8);
    assert(log(255u) == 8);
    assert(log(256u) == 9);

    assert(log(127) == 8);
    assert(log(128) == 9);
    assert(log(129) == 9);
    assert(log(255) == 9);
    assert(log(256) == 10);

    assert(log(-127) == 8);
    assert(log(-128) == 8);
    assert(log(-129) == 9);
    assert(log(-255) == 9);
    assert(log(-256) == 9);
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

    /*
    automatic::addition_result<
        safe<std::int8_t, automatic>,
        safe<std::uint16_t, automatic>
    >::type r1;
    print_argument_type(r1);

    automatic::addition_result<
        safe_signed_range<-3, 8, automatic>,
        safe_signed_range<-4, 9, automatic>
    >::type r1;
    print_argument_type(r1);
    */
    
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

    test_log();
    */
    test_auto<std::int8_t, std::int8_t>(1, -128);
    return 0;
}
