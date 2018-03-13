//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// testing constexpr and safe_literal.
#include <iostream>

#include "../include/safe_integer.hpp"
#include "../include/safe_integer_literal.hpp"
#include "../include/native.hpp"
#include "../include/exception.hpp"

using namespace boost::numeric;

template<std::intmax_t N>
//using compile_time_value = safe_signed_literal<N, native, loose_exception_policy>;
using compile_time_value = safe_signed_literal<N, native, loose_trap_policy>;

template<std::intmax_t N>
using compile_time_result = safe_base<int, N, N, native, loose_trap_policy>;

int main(){

    constexpr const compile_time_value<1000> x;
    constexpr const compile_time_value<2> y;

    // should compile and execute without problem
    std::cout << "x = " << x << '\n';
    std::cout << "y = " << y << '\n';

    constexpr const compile_time_result<1002> x_plus_y = x + y;
    std::cout << "x + y = " << x_plus_y << '\n';
    
    constexpr const compile_time_result<998> x_minus_y = x - y;
    std::cout << "x - y = " << x_minus_y << '\n';

    constexpr const compile_time_result<2000> x_times_y = x * y;
    std::cout << "x * y = " << x_times_y << '\n';

    constexpr const compile_time_result<500> x_divided_by_y = x / y;
    std::cout << "x / y = " << x_divided_by_y << '\n';

    constexpr const compile_time_result<250> x_right_shift_y = x >> y;
    std::cout << "x >> y = " << x_right_shift_y << '\n';

    constexpr const auto /*compile_time_result<4000>*/ x_left_shift_y = x << y;
    std::cout << "x << y = " << x_left_shift_y << '\n';
#if 0

    constexpr const compile_time_result<0> x_mod_y = x % y;
    std::cout << "x % y = " << x_mod_y << '\n';

    constexpr const compile_time_result<0> x_and_y = x & y;
    std::cout << "x & y = " << x_and_y << '\n';
    
    constexpr const compile_time_result<1002> x_or_y = x | y;
    std::cout << "x | y = " << x_or_y << '\n';
#endif

    constexpr const auto x_xor_y = x ^ y;
    std::cout << "x ^ y = " << x_xor_y << '\n';
    /*
    */

    // this is a compile only test - but since many build systems
    // can't handle a compile-only test - make sure it passes trivially.

    // all the following statements should fail to compile
    /*
    y++;
    y--;
    ++y;
    --y;
    y = 1;
    y += 1;
    y -= 1;
    y *= 1;
    y /= 1;
    */
    return 0;
}

