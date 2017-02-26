#include <iostream>
#include <cstdint>

#include "../include/safe_integer.hpp"

using namespace std;
using namespace boost::numeric;

void f(const unsigned int & x, const int8_t & y){
    cout << x * y << endl;
}
void safe_f(
    const safe<unsigned int> & x,
    const safe<int8_t> & y
){
    cout << x * y << endl;
}

int main(){
    cout << "example 4: ";
    cout << "mixing types produces surprising results" << endl;
    try {
        std::cout << "Not using safe numerics" << std::endl;
        // problem: arithmetic operations can yield incorrect results.
        f(100, 100);  // works as expected
        f(100, -100); // wrong result - unnoticed
    }
    catch(std::exception){
        // never arrive here
        std::cout << "error detected!" << std::endl;
    }
    try {
        // solution: use safe types
        std::cout << "Using safe numerics" << std::endl;
        safe_f(100, 100);  // works as expected
        safe_f(100, -100); // throw error
    }
    catch(const exception & e){
        cout << "detected error:" << e.what() << endl;;
    }
    return 0;
}

