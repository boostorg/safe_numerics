#include <cassert>
#include <stdexcept>
#include <ostream>
#include <iostream>
#include <cxxabi.h>
#include <typeinfo>

#include "../include/safe_range.hpp"
#include "../include/automatic.hpp"

// create an output manipulator which prints variable type and limits
// as well as value
template<typename T>
struct formatted_impl {
    const T & m_t;
    formatted_impl(const T & t) :
        m_t(t)
    {}
    template <class charT, class Traits>
    friend std::basic_ostream<charT,Traits> &
    operator<<(
        std::basic_ostream<charT,Traits> & os,
        const formatted_impl<T> & f
    ){
        int status;
        return os
            << "<"
            << abi::__cxa_demangle(
                typeid(boost::numeric::base_value(m_t)).name(),0,0,&status
            )
            << ">["
            << std::numeric_limits<T>::min() << ","
            << std::numeric_limits<T>::max() << "] = "
            << f.m_t;
    }
};

template<typename T>
auto formatted(const T & t){
    return formatted_impl<T>(t);
}

// create a type for holding small integers which implement automatic
// type promotion to larger types to guarentee correct results with
// zero runtime overhead !
template <
    std::intmax_t Min,
    std::intmax_t Max
>
using safe_t = boost::numeric::safe_signed_range<
    Min,
    Max,
    boost::numeric::automatic,
    boost::numeric::throw_exception
>;
using small_integer_t = safe_t<-24, 82>;

int main(int argc, const char * argv[]){
    // problem: checking of externally produced value can be overlooked
    std::cout << "example 8: ";
    std::cout << "eliminate runtime overhead" << std::endl;

    try{
        const small_integer_t x(1);
        std::cout << "x" << formatted(x) << std::endl;
        small_integer_t y = 2;
        std::cout << "y" << formatted(y) << std::endl;
        auto z = x + y;  // zero runtime overhead !
        std::cout << "(x + y)" << formatted(z) << std::endl;
        std::cout << "(x - y)" << formatted(x - y) << std::endl;
    }
    catch(std::exception e){
        // none of the above should trap. Mark failure if they do
        std::cout << e.what() << std::endl;
        return false;
    }

    return 0;
}
