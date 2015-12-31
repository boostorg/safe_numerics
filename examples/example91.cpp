//////////////////////////////////////////////////////////////////
// example91.cpp
//
// Copyright (c) 2015 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <limits>

#include "../include/cpp.hpp"
#include "../include/automatic.hpp"
#include "../include/exception.hpp"
#include "../include/safe_integer.hpp"
#include "../include/safe_range.hpp"
#include "../include/safe_literal.hpp"

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

// define safe types used desktop version of the program.  In conjunction
// with the promotion policy above, this will permit us to guarantee that
// the resulting program will be free of arithmetic errors introduced by
// C expression syntax and type promotion with no runtime penalty
template <typename T> // T is char, int, etc data type
using safe_t = boost::numeric::safe<
    T,
    pic16_promotion,
    boost::numeric::throw_exception // use for compiling and running tests
>;
using safe_bool_t = boost::numeric::safe_unsigned_range<
    0,
    1,
    pic16_promotion,
    boost::numeric::throw_exception // use for compiling and running tests
>;

// define a macro for literal types.  This may not be strictly necessary
// but it provides more information at compile time to the safe numerics
// library which may result in faster code.
//#define literal(x) boost::numeric::safe_literal<x>{}
#define literal(x) x

#include "motor1.c"

#include <chrono>
#include <thread>

int main()
{
    std::cout << "start test\n";
    try{
        initialize();
        motor_run(literal(100));
        do{
            std::this_thread::sleep_for(std::chrono::microseconds(ccpr));
            isr_motor_step();
        }while (run_flg);

        // move motor to position 1000
        motor_run(literal(1000));
        do{
            std::this_thread::sleep_for(std::chrono::microseconds(ccpr));
            isr_motor_step();
        }while (run_flg);

        // move back to position 0
        motor_run(literal(0));
        do{
            std::this_thread::sleep_for(std::chrono::microseconds(ccpr));
            isr_motor_step();
        }while (run_flg);
    }
    catch(...){
        std::cout << "test interrupted\n";
        return 1;
    }
    std::cout << "end test\n";
    return literal(0);
} 