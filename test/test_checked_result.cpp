#include <iostream>
#include <ostream>
#include <cassert>

#include "../include/checked_result.hpp"

bool test1(){
    using namespace boost::numeric;
    checked_result<int> x_min = {
        std::numeric_limits<checked_result<int> >::min()
    };
    std::cout << "checked_result<int> min = " << x_min << std::endl;

    checked_result<int> x_max = {
        std::numeric_limits<checked_result<int>>::max()
    };
    std::cout << "checked_result<int> max = " << x_max << std::endl;
    return true;
}

bool test2(){
    using namespace boost::numeric;
    checked_result<int> x1(0);
    assert(0 == x1);
    checked_result<int> x2(exception_type::overflow_error);
    assert(! x2.no_exception());
    return true;
}

int main(){
    return (
        test1() &&
        test2() /* &&
        test3() &&
        test4() &&
        test5()
        */
    ) ? 0 : 1;
}