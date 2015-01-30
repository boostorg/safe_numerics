#include <cassert>
#include <stdexcept>
#include <iostream>

#include "../include/safe_integer.hpp"

void detected_msg(bool detected){
    std::cout << (detected ? "error detected!" : "error NOT detected! ") << std::endl;
}

int main(int argc, const char * argv[]){
    // problem: undetected erroneous expression evaluation
    std::cout << "example 1:";
    std::cout << "undetected erroneous expression evaluation" << std::endl;
    std::cout << "Not using safe numerics" << std::endl;
    try{
        signed char x = 127;
        signed char y = 2;
        signed char z;
        // this produces an invalid result !
        z = x + y;
        assert(z != 129);
        // but assert fails to detect it since C++ implicitly
        // converts variables to int before evaluating he expression!
        assert(z != x + y);
        std::cout << static_cast<int>(z) << " != " << x + y << std::endl;
        detected_msg(false);
    }
    catch(...){
        assert(false); // we never arrive here
    }
    // solution: replace char with safe<char>
    std::cout << "Using safe numerics" << std::endl;
    try{
        using namespace boost::numeric;
        safe<signed char> x = 127;
        safe<signed char> y = 2;
        safe<signed char> z;
        // rather than producing an invalid result an exception is thrown
        z = x + y;
        assert(false); // we never arrive here
    }
    catch(std::range_error & e){
        // which can catch here
        std::cout << e.what() << std::endl;
        detected_msg(true);
    }
    return 0;
}
