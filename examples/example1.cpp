#include <cassert>
#include <stdexcept>
#include <iostream>

#include "../include/safe_integer.hpp"
//#include "../include/safe_compare.hpp"

void detected_msg(bool detected){
    std::cout << (detected ? "error detected!" : "error NOT detected! ") << std::endl;
}

int main(int argc, const char * argv[]){
    std::cout << "example 1:";
    std::cout << "undetected erroneous expression evaluation" << std::endl;
    std::cout << "Not using safe numerics" << std::endl;
    try{
        char x = 127;
        char y = 2;
        char z;
        // this produces an invalid result !
        z = x + y;
        // it is the wrong result !!!
        assert(z != 129);
        // but assert fails to detect it since C++ implicitly
        // converts variables to int before evaluating he expression!
        assert(z != x + y);
        std::cout << static_cast<int>(z) << " != " << x + y << std::endl;
        detected_msg(false);
    }
    catch(...){
        assert(false); // never arrive here
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
        assert(false); // never arrive here
    }
    catch(std::range_error & e){
        // which can catch here
        std::cout << e.what() << std::endl;
        detected_msg(true);
    }
    return 0;
}
