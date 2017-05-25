#include <iostream>

#include "../include/safe_range.hpp"
#include "../include/safe_literal.hpp"
#include "../include/exception.hpp"
#include "../include/native.hpp"
#include "safe_format.hpp" // prints out range and value of any type

using namespace boost::numeric; // for safe_literal

// create a type for holding small integers.  We "know" that C++
// type promotion rules will work such that addition will never
// overflow. If we change the program to break this, the usage
// of the no_exceptions_policy will prevent compilation.
using safe_t = safe_signed_range<
    -24,
    82,
    native,         // C++ type promotion rules work OK for this example
    no_exceptions_policy  // catch problems at compile time
>;

int main(int argc, const char * argv[]){
    std::cout << "example 83:\n";
    // the following would result in a compile time error
    // since the sum of x and y wouldn't be in the legal
    // range for z.
    // const safe_signed_literal<20> x;
    const safe_signed_literal<10, native, no_exceptions_policy> x;  // no problem
    const safe_signed_literal<67, native, no_exceptions_policy> y;

    const safe_t z = x + y;
    std::cout << "x = " << safe_format(x) << std::endl;
    std::cout << "y = " << safe_format(y) << std::endl;
    std::cout << "z = " << safe_format(z) << std::endl;
    return 0;
}
