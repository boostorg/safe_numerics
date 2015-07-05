#include <iostream>
#include <ostream>

#include "../include/interval.hpp"

/*
template<typename T>
std::ostream & operator<<(std::ostream & os, const boost::numeric::interval<T> & i){
    os << "[" << i.l << "," << i.u << "]" << std::endl;
    return os;
}
*/

bool test1(){
    boost::numeric::interval<std::int16_t> x = {-64, 63};
    std::cout << x;
    std::cout << boost::numeric::operator+<std::int16_t>(x, x);
    std::cout << boost::numeric::operator-<std::int16_t>(x, x);
    return true;
}

int main(){
    return (
        test1() /* &&
        test2() &&
        test3() &&
        test4() &&
        test5()
        */
    ) ? EXIT_SUCCESS : EXIT_FAILURE;
}