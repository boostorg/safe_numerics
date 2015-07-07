#include <iostream>
#include <boost/mpl/print.hpp>

#include "../include/automatic.hpp"
#include "../include/safe_range.hpp"

#include "../include/interval.hpp"

int main(){}

#if 0
template <
    std::intmax_t Min,
    std::intmax_t Max
>
using safe_t = boost::numeric::safe_signed_range<
    Min,
    Max,
    boost::numeric::automatic,
    boost::numeric::trap_exception
>;

bool test1(){
    std::cout << "test1" << std::endl;
    try{
        constexpr const int xi = 1;
        constexpr const safe_t<-64, 63> x(xi);
/*
        safe_t<-64, 63> y;
        y = 2;
        std::cout << "x = " << x << std::endl;
        std::cout << "y = " << y << std::endl;
        auto z = x + y;
        std::cout << "x + y = ["
            << std::numeric_limits<decltype(z)>::min() << ","
            << std::numeric_limits<decltype(z)>::max() << "] = "
            << z << std::endl;

        auto z2 = x - y;
        std::cout << "x - y = ["
            << std::numeric_limits<decltype(z2)>::min() << ","
            << std::numeric_limits<decltype(z2)>::max() << "] = "
            << z2 << std::endl;

            short int yi, zi;
            yi = y;
            zi = x + yi;
*/
    }
    catch(std::exception e){
        // none of the above should trap. Mark failure if they do
        std::cout << e.what() << std::endl;
        return false;
    }
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
#endif
