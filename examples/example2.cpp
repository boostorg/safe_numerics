#include <cassert>
#include <exception>
#include <iostream>

#include "../include/safe_integer.hpp"

void detected_msg(bool detected){
    std::cout << (detected ? "error detected!" : "error NOT detected! ") << std::endl;
}

int main(int argc, const char * argv[]){
    std::cout << "example 2:";
    std::cout << "undetected overflow in data type" << std::endl;
    try{
        int x = INT_MAX;
        // the following silently produces an incorrect result
        ++x;
        std::cout << x << " != " << INT_MAX << " + 1" << std::endl;
        detected_msg(false);
    }
    catch(std::exception){
        assert(false); // never arrive here
    }
    // solution: replace int with safe<int>
    try{
        using namespace boost::numeric;
        safe<int> x = INT_MAX;
        // throws exception when result is past maximum possible 
        ++x;
        assert(false); // never arrive here
    }
    catch(std::exception & e){
        std::cout << e.what() << std::endl;
        detected_msg(true);
    }
    return 0;
}
