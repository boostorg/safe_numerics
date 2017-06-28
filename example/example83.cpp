#include <iostream>

#include "../include/safe_range.hpp"
#include "../include/safe_literal.hpp"
#include "../include/exception.hpp"
#include "../include/native.hpp"
#include "safe_format.hpp" // prints out range and value of any type

using namespace boost::numeric; // for safe_literal

// create a type for holding small integers in a specific range
using safe_t = safe_signed_range<
    -24,
    82,
    native,           // C++ type promotion rules work OK for this example
    loose_trap_policy // catch problems at compile time
>;

// create a type to hold one specific value
template<int I>
using const_safe_t = safe_signed_literal<I, native, loose_trap_policy>;

// We "know" that C++ type promotion rules will work such that
// addition will never overflow. If we change the program to break this,
// the usage of the loose_trap_policy promotion policy will prevent compilation.
int main(int argc, const char * argv[]){
    std::cout << "example 83:\n";

    constexpr const const_safe_t<10> x;
    std::cout << "x = " << safe_format(x) << std::endl;
    constexpr const const_safe_t<67> y;
    std::cout << "y = " << safe_format(y) << std::endl;

    constexpr const safe_t z = x + y;
    std::cout << "z = " << safe_format(z) << std::endl;

    const auto za = x + y;
    std::cout << "za = " << safe_format(za) << std::endl;
    return 0;
}
