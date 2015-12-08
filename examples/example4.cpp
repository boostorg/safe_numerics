#include <exception>
#include <iostream>

#include "../include/safe_integer.hpp"

int main(int argc, const char * argv[]){
    std::cout << "example 4: ";
    std::cout << "implicit conversions change data values" << std::endl;
    std::cout << "Not using safe numerics" << std::endl;
    // problem: implicit conversions change data values
    try{
        int x = -1000;
        // the following silently produces an incorrect result
        char y = x;
        std::cout << x << " != " << (int)y << std::endl;
        std::cout << "error NOT detected!" << std::endl;
    }
    catch(std::exception){
        std::cout << "error detected!" << std::endl; // never arrive here
    }
    // solution: replace int with safe<int> and char with safe<char>
    std::cout << "Using safe numerics" << std::endl;
    try{
        using namespace boost::numeric;
        safe<int> x = -1000;
        // throws exception when conversion change data value
        safe<char> y1(x);
        safe<char> y2 = x;
        safe<char> y3 = {x};
        std::cout << "error NOT detected!" << std::endl;
    }
    catch(std::exception & e){
        std::cout << e.what() << std::endl;
        std::cout << "error detected!" << std::endl;
    }
    return 0;
}
