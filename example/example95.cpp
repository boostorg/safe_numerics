#include <safe_integer.hpp>
#include <iostream>
#include <limits>
#include <type_traits>

#include <boost/rational.hpp>

int main(int argc, const char * argv[])
{
    using IntType = boost::numeric::safe<int>;
    //using IntType = int;
    using SafeRatio = boost::rational<IntType>;

    SafeRatio r {1, 2};
    SafeRatio q {-2, 4};
    std::cout << (r * q) << std::endl;
    auto a = r * q;
    assert ((a == SafeRatio{-1, 4}));
    SafeRatio x {1, INT_MAX};
    SafeRatio y {1, 2};
    try {
            x * y;
            assert (false);
    }
    catch (std::exception const& e) {
            assert (true);
            std::cout << e.what() << std::endl;
    }
    x *= 2;
    x /= 2;

    assert ((x == SafeRatio{1, INT_MAX}));

    return 0;
}
