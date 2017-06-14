//////////////////////////////////////////////////////////////////
// motor.h
// Copyright (c) 2015 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MOTOR_H
#define MOTOR_H

// RR on desktop - this resolves to special file for usage in that
// environment

#if ! defined(DESKTOP)
    #include "18F252.h"
    // include pic sfr map only on pic compilers which support the
    // #byte preprocessor statement.  In other environments, this
    // provokes unsuppressable syntax error.
    #include "picsfr.h"
#else
    #include "18F252_desktop.h"

    extern std::uint8_t base[0xfff];
    extern bit<std::uint8_t, 0> TMR1ON;

    #define TRISC   base[0xf94]
    #define T3CON   base[0xfb1]
    #define CCP2CON base[0xfba]
    #define CCPR2L  base[0xfbb]
    #define CCPR2H  base[0xfbc]
    #define CCP1CON base[0xfbd]
    #define CCPR1L  base[0xfbe]
    #define CCPR1H  base[0xfbf]
    #define T1CON   base[0xfcd]
    #define TMR1L   base[0xfce]
    #define TMR1H   base[0xfcf]
#endif

extern "C" void isr_motor_step();
extern "C" void motor_run(int16 pos_new);
extern "C" void initialize();
int8 get_run_flg();

#endif // MOTOR_H
