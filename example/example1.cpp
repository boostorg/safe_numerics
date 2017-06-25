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
        std::cout << z << " != " << x << " + " << y << std::endl;
        std::cout << "error NOT detected!" << std::endl;
    }
    catch(std::exception){
        std::cout << "error detected!" << std::endl;
    }
    // solution: replace int with safe<int>
    std::cout << "Using safe numerics" << std::endl;
    try{
        using namespace boost::numeric;
        safe<int> x = INT_MAX;
        safe<int> y = 2;
        safe<int> z;
        // rather than producing an invalid result an exception is thrown
        z = x + y;
    }
    catch(std::exception & e){
        // which we can catch here
        std::cout << "error detected:" << e.what() << std::endl;
    }
    return 0;
}
