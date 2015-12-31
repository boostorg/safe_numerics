#ifndef PIC18F252_H
#define PIC18F252_H

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2015 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// any program which uses this is destined to be run on
// the desk top for debugging.  So set indicator here

#define DESKTOP

/*
#include <limits>
#include <iostream>

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
#define literal(x) boost::numeric::safe_literal<x>{}
*/

// define alias the same as used by the pic compiler as safe types for
// the desktop
// see the following comment in motor.c
// Types: int8,int16,int32=8,16,32bit integers, unsigned by default
using int8 = safe_t<std::uint8_t>;
using int16 = safe_t<std::uint16_t>;
using int32 = safe_t<std::uint32_t>;
using uint8 = safe_t<std::uint8_t>;
using uint16 = safe_t<std::uint16_t>;
using uint32 = safe_t<std::uint32_t>;
using signed_int16 = safe_t<std::int16_t>;

// implement equivalent to #bit in C++

// this types is meant to implement operations of naming bits
// which are part of a larger word.
// example
//  unsigned int x.
//  bit<unsigned int, 2> switch; // switch now refers to the
//  second bit from the right of the variable x.  So now can use:
//
//  switch = 1;
//  if(switch)
//      ...

template<typename T, std::int8_t N>
struct bit {
    T & m_word;
    bit(T & rhs) :
        m_word(rhs)
    {}
    bit & operator=(const safe_bool_t & b){
        if(b)
            m_word |= (1 << N);
        else
            m_word &= ~(1 << N);
        return *this;
    }
    bit & operator=(const boost::numeric::safe_literal<0>){
        m_word &= ~(1 << N);
        return *this;
    }
    bit & operator=(const boost::numeric::safe_literal<1>){
        m_word |= (1 << N);
        return *this;
    }
    operator safe_bool_t () const {
        return m_word >> N & 1;
    }
};

// make a 16 bit value from two 8 bit ones
int16 inline make16(int8 h, int8 l){
    return (h << literal(8)) | l;
}

#define disable_interrupts(x)
#define enable_interrupts(x)
#define output_c(x)
#define set_tris_c(x)
#define TRUE literal(1)
#define FALSE literal(0)

#endif // PIC18F252_H
