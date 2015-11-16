#include <cassert>
#include <stdexcept>
#include <iostream>

#include "../include/safe_range.hpp"

void detected_msg(bool detected){
    std::cout << (detected ? "error detected!" : "error NOT detected! ") << std::endl;
}

int main(int argc, const char * argv[]){
    // problem: array index values can exceed array bounds
    std::cout << "example 5: ";
    std::cout << "array index values can exceed array bounds" << std::endl;
    std::cout << "Not using safe numerics" << std::endl;
    int i_array[37];

    // unsigned int i_index = 43;
    // the following corrupts memory.
    // This may or may not be detected at run time.
    // i_array[i_index] = 84; // comment this out so it can be tested!
    detected_msg(false);

    // solution: replace unsigned array index with safe_unsigned_range
    std::cout << "Using safe numerics" << std::endl;
    try{
        using namespace boost::numeric;
        safe_unsigned_range<0, sizeof(i_array)/sizeof(int) - 1> i_index;
        i_index = 36; // this works fine
        i_array[i_index] = 84;
        i_index = 37; // throw exception here!
        i_array[i_index] = 84; // so we never arrive here
        assert(false);
    }
    catch(std::exception & e){
        std::cout << e.what() << std::endl;
        detected_msg(true);
    }
    return 0;
}
