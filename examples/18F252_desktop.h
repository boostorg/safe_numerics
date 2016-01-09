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
using signed_int8 = safe_t<std::int8_t>;
using signed_int16 = safe_t<std::int16_t>;
using signed_int32 = safe_t<std::int32_t>;

// define special type for unsigned modulo N integers
using mod16 = boost::uint_t<16>::exact;
using mod8 = boost::uint_t<8>::exact;

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

#if ! defined(literal)
// define a macro for literal types.  This may not be strictly necessary
// but it provides more information at compile time to the safe numerics
// library which may result in faster code.
//#define literal(x) boost::numeric::safe_literal<x>{}
#define literal(x) x
#endif

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
