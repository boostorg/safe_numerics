#include <cassert>
#include <exception>
#include <iostream>

#include "../include/safe_integer.hpp"

int main(int argc, const char * argv[]){
    std::cout << "example 1:";
    std::cout << "undetected erroneous expression evaluation" << std::endl;
    std::cout << "Not using safe numerics" << std::endl;
    // problem: arithmetic operations can yield incorrect results.
    try{
        char x = 127;
        char y = 2;
        char z;
        // this produces an invalid result !
        z = x + y;
        // but assert fails to detect it since C++ implicitly
        // converts variables to int before evaluating he expression!
        assert(z != x + y);
        std::cout << static_cast<int>(z) << " != " << x + y << std::endl;
        std::cout << "error NOT detected!" << std::endl;
    }
    catch(std::exception){
        std::cout << "error detected!" << std::endl;
    }
    // solution: replace char with safe<char>
    std::cout << "Using safe numerics" << std::endl;
    try{
        using namespace boost::numeric;
        safe<char> x = 127;
        safe<char> y = 2;
        safe<char> z;
        // rather than producing and invalid result an exception is thrown
        z = x + y;
    }
    catch(std::exception & e){
        // which can catch here
        std::cout << e.what() << std::endl;
    }
    return 0;
}
