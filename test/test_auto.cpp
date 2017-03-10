#include <iostream>
#include <cassert>
#include "../include/safe_integer.hpp"
#include "../include/safe_range.hpp"
#include "../include/automatic.hpp"

int test_log(){
    using namespace boost::numeric;
    assert(boost::numeric::log(127u) == 7);
    assert(boost::numeric::log(128u) == 8);
    assert(boost::numeric::log(129u) == 8);
    assert(boost::numeric::log(255u) == 8);
    assert(boost::numeric::log(256u) == 9);

    assert(boost::numeric::log(127) == 8);
    assert(boost::numeric::log(128) == 9);
    assert(boost::numeric::log(129) == 9);
    assert(boost::numeric::log(255) == 9);
    assert(boost::numeric::log(256) == 10);

    assert(boost::numeric::log(-127) == 8);
    assert(boost::numeric::log(-128) == 8);
    assert(boost::numeric::log(-129) == 9);
    assert(boost::numeric::log(-255) == 9);
    assert(boost::numeric::log(-256) == 9);
    return 0;
}

#ifdef __GNUC__
#include <cxxabi.h>

template<class T>
void print_argument_type(const T & t){
    const std::type_info & ti = typeid(T);
    int status;
    std::cout
        << abi::__cxa_demangle(ti.name(),0,0,&status) << ' ' << t << std::endl;
}

#else
template<class T>
void print_argument_type(const T & t){
    const std::type_info & ti = typeid(T);
    int status;
    std::cout
        << ti.name() << std::endl;
}

#endif

int test_auto(){
    using namespace boost::numeric;

    safe<char, automatic> s1 = 1;
    unsigned long long t1 = 1;
    try{
        decltype(s1 + t1) r1;
        //auto r1 = s1 + t1;
        print_argument_type(r1);
    }
    catch(std::exception e){
        decltype(s1 + t1) tx;
    }

    safe<std::int8_t, automatic> s2 = -128;
    std::int8_t t2 = 1;
    try{
        decltype(s2 + t2) r2;
        r2 = s2 + t2;
        print_argument_type(r2);
    }
    catch(std::exception e){
        decltype(s2 + t2) tx;
    }


    /*
    automatic::addition_result<
        safe<std::int8_t, automatic>,
        safe<std::uint64_t, automatic>,
        automatic,
        throw_exception
    >::type r1;
    print_argument_type(r1);

    automatic::addition_result<
        safe_signed_range<-3, 8, automatic>,
        safe_signed_range<-4, 9, automatic>,
        automatic,
        throw_exception
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
        throw_exception
    >::type x1;

    test_log();
    */
    test_auto();
}