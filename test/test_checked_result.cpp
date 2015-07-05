#include <iostream>
#include <ostream>

#include "../include/checked_result.hpp"

bool test1(){
    boost::numeric::checked_result<int> x_min = {std::numeric_limits<
            boost::numeric::checked_result<int>
        >::min()
        };
    std::cout << "checked_result<int> min = " << x_min << std::endl;

    boost::numeric::checked_result<int> x_max = {std::numeric_limits<
            boost::numeric::checked_result<int>
        >::max()
        };
    std::cout << "checked_result<int> max = " << x_max << std::endl;
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