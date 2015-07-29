#include <cassert>
#include <stdexcept>
#include <ostream>
#include <iostream>
#include <cxxabi.h>

#include "../include/safe_range.hpp"
#include "../include/automatic.hpp"

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

// I can't figure out how to overload os << for safe_t
// we use the following workaround there

// wrap a safe_t in a "formatted" wrapper
template<typename T>
struct formatted {
    using wrapped_type = T;
    const T & m_t;
    formatted(const T & t) :
        m_t(t)
    {}
};

template<typename T>
auto make_formatted(const T & t){
    return formatted<T>(t);
}

// now (fully) specialize output of safe types wrapped in formatted
template<
    class T,
    T Min,
    T Max,
    class P, // promotion polic
    class E  // exception policy
>
std::ostream & operator<<(
    std::ostream & os,
    const formatted<boost::numeric::safe_base<T, Min, Max, P, E>> & f
){
    using safe_type = typename formatted<boost::numeric::safe_base<T, Min, Max, P, E> >::wrapped_type;
    return os
        << "["
        << std::numeric_limits<safe_type>::min() << ","
        << std::numeric_limits<safe_type>::max() << "] = "
        << f.m_t;
}

int main(int argc, const char * argv[]){
    // problem: checking of externally produced value can be overlooked
    std::cout << "example 8: ";
    std::cout << "eliminate runtime overhead"
    << std::endl;

    try{
        int status;
        const safe_t<-64, 63> x(1);
        std::cout << abi::__cxa_demangle(typeid(x).name(),0,0,&status) << '\n';

        std::cout << "x" << make_formatted(x) << std::endl;
        safe_t<-64, 63> y;
        y = 2;
        std::cout << "y" << make_formatted(y) << std::endl;
        auto z = x + y;
        std::cout << "(x + y)" << make_formatted(z) << std::endl;

        std::cout << "(x - y)" << make_formatted(x - y) << std::endl;
    }
    catch(std::exception e){
        // none of the above should trap. Mark failure if they do
        std::cout << e.what() << std::endl;
        return false;
    }

    return 0;
}
