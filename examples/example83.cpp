#include <iostream>

#include "../include/safe_range.hpp"
#include "../include/safe_literal.hpp"
#include "../include/native.hpp"
#include "../include/exception.hpp"

#include "safe_format.hpp" // prints out range and value of any type

using namespace boost::numeric; // for safe_literal

// create a type for holding small integers.  We "know" that C++ type
// promotion rules will work such that operations on this type
// will never overflow. If change the program to break this, the
// trap_exception will prevent compilation
using safe_t = safe_signed_range<
    -24,
    82,
    native, // we don't need automatic in this case
    trap_exception
>;

int f(const safe_t & x, const safe_t & y){
    int z = x + y;  // we know that this cannot fail
    // std::int8_t z = x + y; // but this COULD fail. So we get a compile error
    std::cout << "(x + y)" << safe_format(x + y) << std::endl;
    std::cout << "(x - y)" << safe_format(x - y) << std::endl;
    return z;
}

int main(int argc, const char * argv[]){
    std::cout << "example 83:\n";
    safe_t x(safe_literal<1>{});  // note special type for initialization needed
    safe_t y(safe_literal<2>{});  // to avoid runtime penalty
    std::cout << "x" << safe_format(x) << std::endl;
    std::cout << "y" << safe_format(y) << std::endl;
    std::cout << "z" << safe_format(f(x, y)) << std::endl;
    return 0;
}
