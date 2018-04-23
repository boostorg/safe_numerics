#include <iostream>

#include "../include/safe_integer.hpp"
#include "../include/native.hpp"
#include "../include/exception.hpp"
#include "../include/safe_integer_literal.hpp"

using namespace boost::numeric;


template<std::intmax_t N>
using compile_time_value = safe_unsigned_literal<N, native, loose_trap_policy>;

int main(){

    constexpr const compile_time_value<1000> x;
    constexpr const compile_time_value<1> y;

    // should compile and execute without problem
    std::cout << x << '\n';

    // all the following statements should compile
    constexpr auto x_plus_y = x + y;
    constexpr auto x_minus_y = x - y;
    constexpr auto x_times_y = x * y;
    constexpr auto x_and_y = x & y;
    constexpr auto x_or_y = x | y;
    constexpr auto x_xor_y = x ^ y;
    constexpr auto x_divided_by_y = x / y;
    constexpr auto x_mod_y = x % y;

}
