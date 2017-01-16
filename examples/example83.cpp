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
// usage of the trap_exception promotion policy will prevent compilation.
using safe_t = safe_signed_range<
    -24,
    82,
    native,         // C++ type promotion rules work OK for this example
    trap_exception  // catch problems at compile time
>;

auto f(const safe_t & x,    const safe_t & y){
    //safe_t z = x + y; // depending on values of x & y COULD fail
    auto z = x + y;    // due to C++ type promotion rules,
                       // we know that this cannot fail
    std::cout << "(x + y) = " << safe_format(x + y) << std::endl;
    std::cout << "(x - y) = " << safe_format(x - y) << std::endl;
    return z;
}

int main(int argc, const char * argv[]){
    std::cout << "example 83:\n";
    // constexpr const safe_t z = 3; // fails to compile
    const safe_t x(safe_signed_literal<2>{});
    const safe_t y = safe_signed_literal<2>();  // to avoid runtime penalty
    std::cout << "x = " << safe_format(x) << std::endl;
    std::cout << "y = " << safe_format(y) << std::endl;
    std::cout << "z = " << safe_format(f(x, y)) << std::endl;
    return 0;
}
