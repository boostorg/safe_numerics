#include <iostream>
#include "../include/safe_integer.hpp"
int main(){
    try{
        using namespace boost::numeric;
        safe<int> x;
        safe<char> y3 = x;
    }
    catch(std::exception & e){
        std::cout << e.what() << std::endl;
    }
    return 0;
}
