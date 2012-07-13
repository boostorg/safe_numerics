#include <cassert>
#include <stdexcept>
#include <iostream>

#include "../include/safe_integer.hpp"
#include "../include/safe_compare.hpp"

void detected_msg(bool detected){
    std::cout << (detected ? ": error detected!" : ": error NOT detected!: ") << std::endl;
}

#include "example1.cpp"
#include "example2.cpp"
#include "example3.cpp"
#include "example4.cpp"

int main(int argc, const char * argv[]){
    example1();
    example2();
    example3();
    example4();
}