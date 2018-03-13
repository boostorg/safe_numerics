#include <iostream>

#include "../include/safe_integer.hpp"
#include "../include/exception_policies.hpp"
#include "../include/automatic.hpp"
#include "safe_format.hpp" // prints out range and value of any type

using safe_t = boost::numeric::safe<
    int,
    boost::numeric::automatic, // note use of "automatic" policy!!!
    boost::numeric::loose_trap_policy
>;

int main(int, const char *[]){
    std::cout << "example 82:\n";
    safe_t x(INT_MAX);
    safe_t y = 2;
    std::cout << "x = " << safe_format(x) << std::endl;
    std::cout << "y = " << safe_format(y) << std::endl;
    std::cout << "z = " << safe_format(x + y) << std::endl;
    return 0;
}

