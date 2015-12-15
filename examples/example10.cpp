#include <cassert>
#include <stdexcept>
#include <sstream>
#include <iostream>

#include "../include/safe_integer.hpp"
#include "../include/cpp.hpp"

#include <iostream>
#include <cstdint>

using namespace std;

int main(){
    // problem: checking of externally produced value can be overlooked
    std::cout << "example 8: ";
    std::cout << "eliminate runtime overhead" << std::endl;

    int8_t x = 100;
    int y = x * x;
    cout << y << endl;

    uint32_t z1 = 100;
    int8_t z2 = -100;
    auto y2 = z1 * z2;
    cout << y2 << endl;

    return 0;
}

