
#include <iostream>
#include "safe_integer.hpp"

int main(){
    boost::numeric::safe<int32_t> t1(0x80000000);
    int8_t v2(0xff);

    std::cout << t1 / v2;
    return 0;
}
