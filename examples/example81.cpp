#include <iostream>

#include "../include/safe_integer.hpp"
#include "../include/exception.hpp" // include exception policies

using safe_t = boost::numeric::safe<
    int,
    boost::numeric::native,
    boost::numeric::trap_exception  // note use of "trap_exception" policy!
>;

int main(int argc, const char * argv[]){
    std::cout << "example 81:\n";
    safe_t x(INT_MAX);
    safe_t y(2);
    safe_t z = x + y; // will fail to compile !
    return 0;
}
