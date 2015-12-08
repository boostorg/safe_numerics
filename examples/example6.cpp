#include <stdexcept>
#include <sstream>
#include <iostream>

#include "../include/safe_integer.hpp"

int main(int argc, const char * argv[]){
    // problem: checking of externally produced value can be overlooked
    std::cout << "example 6: ";
    std::cout << "checking of externally produced value can be overlooked" << std::endl;
    std::cout << "Not using safe numerics" << std::endl;

    std::istringstream is("12317289372189 1231287389217389217893");

    try{
        int x, y;
        is >> x >> y; // get integer values from the user
        std::cout << x << ' ' << y << std::endl;
        std::cout << "error NOT detected!" << std::endl;
    }
    catch(std::exception){
        std::cout << "error detected!" << std::endl;
    }

    // solution: asign externally retrieved values to safe equivalents
    std::cout << "Using safe numerics" << std::endl;
    {
        using namespace boost::numeric;
        safe<int> x, y;
        is.seekg(0);
        try{
            is >> x >> y; // get integer values from the user
            std::cout << x << ' ' << y << std::endl;
            std::cout << "error NOT detected!" << std::endl;
        }
        catch(std::exception & e){
            std::cout << e.what() << std::endl;
            std::cout << "error detected!" << std::endl;
        }
    }
    return 0;
}
