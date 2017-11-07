#include <iostream>
#include <ostream>
#include <cassert>

#include "../include/checked_result.hpp"
#include "../include/checked_result_operations.hpp"

bool test1(){
    using namespace boost::numeric;
    checked_result<int> x_min =
        std::numeric_limits<checked_result<int> >::min();
    std::cout << "checked_result<int> min = " << x_min << std::endl;

    checked_result<int> x_max =
        std::numeric_limits<checked_result<int>>::max();
    std::cout << "checked_result<int> max = " << x_max << std::endl;
    return true;
}

bool test2(){
    using namespace boost::numeric;
    checked_result<int> x1(0);
    assert(0 == x1);
    checked_result<int> x2(
        safe_numerics_error::positive_overflow_error,
        "positive_overflow_error"
    );
    assert(x2.exception());
    std::cout << "checked_result<int> x2 = " << x2 << std::endl;
    return true;
}

int main(){
    return (
        test1() &&
        test2()
    ) ? 0 : 1;
}
