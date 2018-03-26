#include <exception>
#include <iostream>

#include "../include/checked_result.hpp"
#include "../include/checked_result_operations.hpp"

int main(){
    using ext_uint = boost::numeric::checked_result<unsigned int>;
    const ext_uint x{4};
    const ext_uint y{3};

    // operation is a success!
    std::cout << "success! x - y = " << x - y;

    // subtraction would result in -1, and invalid result for an unsigned value
    std::cout << "problem: y - x = " << y - x;

    const ext_uint z = y - x;
    std::cout << "z = " << z;
    // sum of two negative overflows is a negative overflow.
    std::cout << "z + z" << z + z;

    return 0;
}
