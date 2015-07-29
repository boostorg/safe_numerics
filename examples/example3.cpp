#include <cassert>
#include <exception>
#include <iostream>

#include "../include/safe_integer.hpp"

void detected_msg(bool detected){
    std::cout << (detected ? "error detected!" : "error NOT detected! ") << std::endl;
}

int main(int argc, const char * argv[]){
    std::cout << "example 3:";
    std::cout << "undetected underflow in data type" << std::endl;
    std::cout << "Not using safe numerics" << std::endl;
    try{
        unsigned int x = 0;
        // the following silently produces an incorrect result
        --x;
        // because C/C++ implicitly converts mis-matched arguments to int
        // suggests that the operation is correct
        assert(x == -1);
        // even though it's not !!!

        // so the error is not detected!
        std::cout << x << " != " << -1 << std::endl;
        detected_msg(false);
    }
    catch(std::exception){
        assert(false); // never arrive here
    }
    // solution: replace unsigned int with safe<unsigned int>
    std::cout << "Using safe numerics" << std::endl;
    try{
        using namespace boost::numeric;
        safe<unsigned int> x = 0;
        // decrement unsigned to less than zero throws exception
        --x;
        assert(false); // never arrive here
    }
    catch(std::exception & e){
        std::cout << e.what() << std::endl;
        detected_msg(true);
    }
    return 0;
}
