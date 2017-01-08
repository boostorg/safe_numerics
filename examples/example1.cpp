#include <cassert>
#include <exception>
#include <iostream>
#include <cstdint>

#include "../include/safe_integer.hpp"

int main(int argc, const char * argv[]){
    std::cout << "example 1:";
    std::cout << "undetected erroneous expression evaluation" << std::endl;
    std::cout << "Not using safe numerics" << std::endl;
    try{
        int x = INT_MAX;
        int y = 2;
        int z;
        // this produces an invalid result !
        z = x + y;
        // but assert fails to detect it since C++ implicitly
        // converts variables to int before evaluating he expression!
        std::cout << "x == " << x << '\n';
        std::cout << "y == " << y << '\n';
        std::cout << "z == " << z << '\n';
        std::cout << std::boolalpha;
        std::cout << "does z == x + y? " << (z == x + y) << std::endl;

        assert(z == x + y);
        std::cout << z << " != " << x + y << std::endl;
        std::cout << "error NOT detected!" << std::endl;
    }
    catch(std::exception){
        std::cout << "error detected!" << std::endl;
    }
    // problem: arithmetic operations can yield incorrect results.
    try{
        std::int8_t x = 127;
        std::int8_t y = 2;
        std::int8_t z;
        // this produces an invalid result !
        z = x + y;
        // but assert fails to detect it since C++ implicitly
        // converts variables to int before evaluating he expression!
        std::cout << "x == " << static_cast<int>(x) << '\n';
        std::cout << "y == " << static_cast<int>(y) << '\n';
        std::cout << "z == " << static_cast<int>(z) << '\n';
        std::cout << std::boolalpha;
        std::cout << "does z == x + y? " << (z == x + y) << std::endl;

        // assert(z == x + y);
        std::cout << static_cast<int>(z) << " != " << x + y << std::endl;
        std::cout << "error NOT detected!" << std::endl;
    }
    catch(std::exception){
        std::cout << "error detected!" << std::endl;
    }
    // solution: replace std::int8_t with safe<std::int8_t>
    std::cout << "Using safe numerics" << std::endl;
    try{
        using namespace boost::numeric;
        safe<std::int8_t> x = 127;
        safe<std::int8_t> y = 2;
        safe<std::int8_t> z;
        // rather than producing and invalid result an exception is thrown
        z = x + y;
    }
    catch(std::exception & e){
        // which can catch here
        std::cout << e.what() << std::endl;
    }
    return 0;
}
