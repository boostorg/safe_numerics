//////////////////////////////////////////////////////////////////
// example93.cpp
//
// Copyright (c) 2015 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <limits>
#include <boost/integer.hpp>

#include "../include/cpp.hpp"
#include "../include/safe_integer.hpp"
#include "../include/safe_range.hpp"

// use same type promotion as used by the pic compiler
// see the following comment in motor.c
// Types: int8,int16,int32=8,16,32bit integers, unsigned by default

using pic16_promotion = boost::numeric::cpp<
    8,  // char
    8,  // short
    8,  // int
    16, // long
    32  // long long
>;

using exception_policy = boost::numeric::loose_trap_policy;

// define safe types used desktop version of the program.  In conjunction
// with the promotion policy above, this will permit us to guarantee that
// the resulting program will be free of arithmetic errors introduced by
// C expression syntax and type promotion with no runtime penalty
template <typename T> // T is char, int, etc data type
using safe_t = boost::numeric::safe<
    T,
    pic16_promotion,
    exception_policy
>;
using safe_bool_t = boost::numeric::safe_unsigned_range<
    0,
    1,
    pic16_promotion,
    exception_policy
>;

using step_t = boost::numeric::safe_signed_range<
    0,
    1000,
    pic16_promotion,
    exception_policy
>;
using denom_t = boost::numeric::safe_signed_range<
    -4005,
    4005,
    pic16_promotion,
    exception_policy
>;
using phase_ix_t = boost::numeric::safe_unsigned_range<
    0,
    3,
    pic16_promotion,
    exception_policy
>;
using c24_t = boost::numeric::safe_signed_range<
    -0x8000000,
    0x7ffffff,
    pic16_promotion,
    exception_policy
>;

#define DESKTOP
#include "motor3.c"

#include <chrono>
#include <thread>

void test(step_t m){
    std::cout << "move motor to " << m << '\n';
    motor_run(m);
    while(run_flg){
        isr_motor_step();
        std::cout << motor_pos << ' ' << c32 << ' ' << c << '\n';
        std::this_thread::sleep_for(std::chrono::microseconds(ccpr));
    }
}

int main()
{
    std::cout << "start test\n";
    try{
        initialize();
        // move motor to position 100
        test(literal(100));
        // move motor to position 1000
        //test(1000);
        // move back to position 0
        test(literal(0));
    }
    catch(std::exception & e){
        std::cout << e.what() << '\n';
        return 1;
    }
    catch(...){
        std::cout << "test interrupted\n";
        return 1;
    }
    std::cout << "end test\n";
    return 0;
} 
