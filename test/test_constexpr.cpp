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
using compile_time_value = safe_signed_literal<N, native, loose_trap_policy>;

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

