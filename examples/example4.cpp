#include <exception>
#include <iostream>

#include "../include/safe_integer.hpp"

int main(){
    std::cout << "example 3: ";
    std::cout << "implicit conversions change data values" << std::endl;
    std::cout << "Not using safe numerics" << std::endl;
    
    // problem: implicit conversions change data values
    try{
        signed int   a{-1};
        unsigned int b{1};
        if(a < b){
            std::cout << "a is less than b\n";
        }
        else{
            std::cout << "b is less than a\n";
        }
    }
    catch(std::exception){
        // never arrive here - just produce the wrong answer!
        std::cout << "error detected!" << std::endl;
    }

    // solution: replace int with safe<int> and char with safe<char>
    std::cout << "Using safe numerics" << std::endl;
    try{
        using namespace boost::numeric;
        safe<signed int>   a{-1};
        safe<unsigned int> b{1};
        if(a < b){
            std::cout << "a is less than b\n";
        }
        else{
            std::cout << "b is less than a\n";
        }
    }
    catch(std::exception & e){
        // never arrive here - just produce the correct answer!
        std::cout << e.what() << std::endl;
        std::cout << "error detected!" << std::endl;
    }
    return 0;
}
