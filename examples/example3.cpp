#include <cassert>
#include <stdexcept>
#include <iostream>

#include "../include/safe_integer.hpp"
#include "../include/safe_compare.hpp"

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

        // however, safe_compare does detect the error
        assert(! boost::numeric::safe_compare::equal(x, -1));
        std::cout << x << " != " << -1;
        detected_msg(false);
    }
    catch(...){
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
    catch(std::range_error & e){
        std::cout << e.what();
        detected_msg(true);
    }
    return 0;
}
