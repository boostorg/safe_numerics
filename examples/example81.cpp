#include <iostream>

#include "../include/safe_integer.hpp"
#include "../include/exception.hpp" // include exception policies
#include "safe_format.hpp" // prints out range and value of any type

using safe_t = boost::numeric::safe<
    int,
    boost::numeric::native,
    boost::numeric::trap_exception  // note use of "trap_exception" policy!
>;

safe_t f(const safe_t & x, const safe_t & y){
    // each statement below will fail to compile !
    safe_t z = x + y;
    std::cout << "(x + y)" << safe_format(x + y) << std::endl;
    std::cout << "(x - y)" << safe_format(x - y) << std::endl;
    return z;
}

int main(int argc, const char * argv[]){
    std::cout << "example 81:\n";
    safe_t x(INT_MAX);  // will fail to compile
    safe_t y(2);        // will fail to compile
    std::cout << "x" << safe_format(x) << std::endl;
    std::cout << "y" << safe_format(y) << std::endl;
    std::cout << "z" << safe_format(f(x, y)) << std::endl;
    return 0;
}
