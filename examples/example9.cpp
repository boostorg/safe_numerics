//////////////////////////////////////////////////////////////////
// test wrapper to permit compilation execution and debug of code
// intended for the PIC family of processors on the desktop
// development environment.
//
// Robert Ramey, 2015

#include <limits>
#include <iostream>

#include "../include/cpp.hpp"
#include "../include/automatic.hpp"
#include "../include/exception.hpp"
#include "../include/safe_integer.hpp"
#include "../include/safe_range.hpp"
#include "../include/safe_literal.hpp"

using pic16_promotion = boost::numeric::cpp<
    8,  // char
    8,  // short
    8,  // int
    16, // long
    32  // long long
>;

template <typename T> // T is char, int, etc data type
using safe_t = boost::numeric::safe<
    T,
    boost::numeric::automatic,
    boost::numeric::trap_exception // use for compiling and running tests
>;
using safe_bool_t = boost::numeric::safe_unsigned_range<
    0,
    1,
    pic16_promotion,
    boost::numeric::trap_exception // use for compiling and running tests
>;

using int8 = safe_t<std::uint8_t>;
using int16 = safe_t<std::uint16_t>;
using int32 = safe_t<std::uint32_t>;
using uint8 = safe_t<std::uint8_t>;
using uint16 = safe_t<std::uint16_t>;
using uint32 = safe_t<std::uint32_t>;
using signed_int16 = safe_t<std::int16_t>;

#define literal(x) boost::numeric::safe_literal<x>{}

std::uint8_t base[0xfff];
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

// now we can render
//#bit  TMR1ON  = T1CON.0
// as
bit<std::uint8_t, 0> TMR1ON(T1CON);
// and use expressions such as TMR1ON = 0

// make a 16 bit value from two 8 bit ones
int16 make16(int8 h, int8 l){
    return (h << literal(8)) | l;
}

#define disable_interrupts(x)
#define enable_interrupts(x)
#define output_c(x)
#define set_tris_c(x)
#define TRUE literal(1)
#define FALSE literal(0)

// note changes to original source code
// signed int16 <- signed_int16 note '-' added
// commented out the #byte and #bit statements
// commented out the #INT_CCP1
// void main() <- int main()
// added return 0 to main
// changed instances of x = 0 to x = literal(0)

//////////////////////////////////////////////////////////////////
// motor.c
// david austin
// http://www.embedded.com/design/mcus-processors-and-socs/4006438/Generate-stepper-motor-speed-profiles-in-real-time
// DECEMBER 30, 2004

// Demo program for stepper motor control with linear ramps
// Hardware: PIC18F252, L6219
// #include "18F252.h"

// PIC18F252 SFRs
/*
#byte TRISC   = 0xf94
#byte T3CON   = 0xfb1
#byte CCP2CON = 0xfba
#byte CCPR2L  = 0xfbb
#byte CCPR2H  = 0xfbc
#byte CCP1CON = 0xfbd
#byte CCPR1L  = 0xfbe
#byte CCPR1H  = 0xfbf
#byte T1CON   = 0xfcd
#byte TMR1L   = 0xfce
#byte TMR1H   = 0xfcf
#bit  TMR1ON  = T1CON.0
*/

// 1st step=50ms; max speed=120rpm (based on 1MHz timer, 1.8deg steps)
#define C0    literal(50000)
#define C_MIN  literal(2500)

// ramp state-machine states
#define ramp_idle literal(0)
#define ramp_up   literal(1)
#define ramp_max  literal(2)
#define ramp_down literal(3)
#define ramp_last literal(4)

// Types: int8,int16,int32=8,16,32bit integers, unsigned by default
int8  ramp_sts=ramp_idle;
signed_int16 motor_pos = literal(0); // absolute step number
signed_int16 pos_inc = literal(0);     // motor_pos increment
int16 phase=literal(0);     // ccpPhase[phase_ix]
int8  phase_ix=literal(0);  // index to ccpPhase[]
int8  phase_inc;            // phase_ix increment
int8  run_flg;              // true while motor is running
int16 ccpr;                 // copy of CCPR1&2
int16 c;                    // integer delay count
int16 step_no;              // progress of move
int16 step_down;            // start of down-ramp
int16 move;                 // total steps to move
int16 midpt;                // midpoint of move
int32 c32;                  // 24.8 fixed point delay count
signed_int16 denom; // 4.n+1 in ramp algo

// Config data to make CCP1&2 generate quadrature sequence on PHASE pins
// Action on CCP match: 8=set+irq; 9=clear+irq
int16 const ccpPhase[] = {
    literal(0x909),
    literal(0x908),
    literal(0x808),
    literal(0x809)
}; // 00,01,11,10

void current_on(){/* code as needed */}  // motor drive current
void current_off(){/* code as needed */} // reduce to holding value

// compiler-specific ISR declaration
// #INT_CCP1
void isr_motor_step() 
{ // CCP1 match -> step pulse + IRQ
  ccpr += c; // next comparator value: add step delay count
  switch (ramp_sts)
  {
  case ramp_up:   // accel
    if (step_no==midpt)
    { // midpoint: decel
      ramp_sts = ramp_down;
      denom = ((step_no - move) << literal(2) )+literal(1);

      if (!(move & literal(1)))
      { // even move: repeat last delay before decel
        denom +=literal(4);
        break;
      }
    }
    // no break: share code for ramp algo
  case ramp_down: // decel
    if (step_no == move-literal(1))
    { // next irq is cleanup (no step)
      ramp_sts = ramp_last;
      break;
    }
    denom+=4;
    c32 -= (c32 << literal(1)) / denom; // ramp algorithm
    // beware confict with foreground code if long div not reentrant
    c = (c32+literal(128))>>literal(8); // round 24.8format->int16
    if (c <= C_MIN)
    { // go to constant speed
      ramp_sts = ramp_max;
      step_down = move - step_no;
      c = C_MIN;
      break;
    }
    break;
  case ramp_max: // constant speed
    if (step_no == step_down)
    { // start decel
      ramp_sts = ramp_down;
      /*
      denom = ((step_no - move)<<literal(2))+literal(5);
      */
      auto x1 = step_no - move;
      auto x2 = x1 * literal(4);
      auto x3 = x2 + literal(5);
      denom = x3;
    }
    break;
  default: // last step: cleanup
    ramp_sts = ramp_idle;
    current_off(); // reduce motor current to holding value
    disable_interrupts(INT_CCP1);
    run_flg = FALSE; // move complete
    break;
  } // switch (ramp_sts)
  if (ramp_sts!=ramp_idle)
  {
    motor_pos += pos_inc;
    ++step_no;
    CCPR2H = CCPR1H = (ccpr >> literal(8)); // timer value at next CCP match
    CCPR2L = CCPR1L = (ccpr & literal(0xff));
    if (ramp_sts!=ramp_last) // else repeat last action: no step
	    phase_ix = (phase_ix + phase_inc) & literal(3);
    phase = ccpPhase[phase_ix];
    CCP1CON = phase & literal(0xff); // set CCP action on next match
    CCP2CON = phase >> literal(8);
  } // if (ramp_sts != ramp_idle)
} // isr_motor_step()


void motor_run(signed_int16 pos_new)
{ // set up to drive motor to pos_new (absolute step#)
  if (pos_new < motor_pos) // get direction & #steps
  {
    move = motor_pos-pos_new;
    pos_inc   = literal(-1);
    phase_inc = literal(0xff);
  } 
  else if (pos_new != motor_pos)
  { 
    move = pos_new-motor_pos;
    pos_inc   = literal(1);
    phase_inc = literal(1);
  }
  else return; // already there
  /*
  midpt = (move-1)>>1;
  */
  auto x1 = move  - 1;
  auto x2 = x1 >> 1;
  midpt = x2;
  c   = C0;
  c32 = c<<literal(8); // keep c in 24.8 fixed-point format for ramp calcs
  step_no  = literal(0); // step counter
  denom    = literal(1); // 4.n+1, n=0
  ramp_sts = ramp_up; // start ramp state-machine
  run_flg  = TRUE;
  TMR1ON   = 0; // stop timer1;
  ccpr = make16(TMR1H,TMR1L);  // 16bit value of Timer1
  ccpr += 1000; // 1st step + irq 1ms after timer1 restart
  CCPR2H = CCPR1H = (ccpr >> literal(8));
  CCPR2L = CCPR1L = (ccpr & literal(0xff));
  phase_ix = (phase_ix + phase_inc) & literal(3);
  phase = ccpPhase[phase_ix];
  CCP1CON = phase & literal(0xff); // sets action on match
  CCP2CON = phase >> literal(8);
  current_on(); // current in motor windings
  enable_interrupts(INT_CCP1); 
  TMR1ON=TRUE; // restart timer1;
} // motor_run()

void initialize()
{
  disable_interrupts(GLOBAL);
  disable_interrupts(INT_CCP1);
  disable_interrupts(INT_CCP2);
  output_c(0);
  set_tris_c(0);
  T3CON = 0;
  T1CON = 0x35;
  enable_interrupts(GLOBAL);
} // initialize()

// test program
int main()
{
    std::cout << "start test\n";
    try{
	initialize();
    motor_run(literal(100));

    // move motor to position 1000
  	motor_run(literal(1000));
  	while (run_flg){
        isr_motor_step();
    }
    // move back to position 0
  	motor_run(literal(0));
  	while (run_flg)
        isr_motor_step();
    }
    catch(...){
        std::cout << "test interrupted\n";
        return 1;
    }
    std::cout << "end test\n";
    return literal(0);
} // main()
// end of file motor.c
