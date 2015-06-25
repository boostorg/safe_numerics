#include <cassert>
#include <stdexcept>
#include <sstream>
#include <iostream>

#include "../include/safe_integer.hpp"

void detected_msg(bool detected){
    std::cout << (detected ? "error detected!" : "error NOT detected! ") << std::endl;
}

int main(int argc, const char * argv[]){
    // problem: checking of externally produced value can be overlooked
    std::cout << "example 6: ";
    std::cout << "checking of externally produced value can be overlooked" << std::endl;
    std::cout << "Not using safe numerics" << std::endl;

    std::istringstream is("12317289372189 1231287389217389217893");

    int x, y, z;
    is >> x >> y; // get integer values from the user
    z = x + y;
    std::cout << z << std::endl;  // display sum of the values
    detected_msg(false);
    
    // solution: asign externally retrieved values to safe equivalents
    std::cout << "Using safe numerics" << std::endl;
    {
        using namespace boost::numeric;
        safe<int> x, y, z;
        is.seekg(0);
        try{
            is >> x >> y; // get integer values from the user
            detected_msg(false);
        }
        catch(std::exception e){
            std::cout << e.what() << std::endl;
            detected_msg(true);
        }
    }
    return 0;
}
