#include <iostream>
#include <limits>

#include <boost/rational.hpp>
#include <safe_integer.hpp>

int main(int argc, const char * argv[])
{
    using namespace boost::numeric;
    constexpr const safe<int> x = INT_MAX;
    constexpr const safe<int> y = 2;
    safe<int> z;
    // rather than producing an invalid result an exception is thrown
    z = x + y;
    return 0;
}
