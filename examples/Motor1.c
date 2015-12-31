//////////////////////////////////////////////////////////////////
// motor.c
// david austin
// http://www.embedded.com/design/mcus-processors-and-socs/4006438/Generate-stepper-motor-speed-profiles-in-real-time
// DECEMBER 30, 2004

// Demo program for stepper motor control with linear ramps
// Hardware: PIC18F252, L6219

//////////////////////////////////////////////////////////////////
// Note:
// changes to original source code to permit desktop compile, test
// and debug while remaining compatible with pic compiler.  These
// changes to the original code are marked with RR below

// Robert Ramey, 2015

// note changes to original source code
//
// signed int16 <- signed_int16 note '-' added
// commented out the #byte and #bit statements
// commented out the #INT_CCP1
// changed instances of x = 0 to x = literal(0)

// ***********
// RR factor out motor.c so motor code can be separated from tests
// and other user code.
#include "motor.h"

#if !defined(DESKTOP)
    #include "picsfr.h"
#else
    // define a memory map to emulate the on in the pic.  This permits all
    // same pic code to work on the desktop with no alteration
    std::uint8_t base[0xfff];
    // now we can render
    //#bit  TMR1ON  = T1CON.0 as
    bit<std::uint8_t, 0> TMR1ON(T1CON);
    // and use expressions such as TMR1ON = 0
#endif

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

// RR compiler-specific ISR declaration
// #if ! defined(DESKTOP)
// #INT_CCP1
// #endif
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
      denom = ((step_no - move)<<literal(2))+literal(5);
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
  midpt = (move-1)>>1;
  c   = C0;
  c32 = c<<literal(8); // keep c in 24.8 fixed-point format for ramp calcs
  step_no  = literal(0); // step counter
  denom    = literal(1); // 4.n+1, n=0
  ramp_sts = ramp_up; // start ramp state-machine
  run_flg  = TRUE;
  TMR1ON   = literal(0); // stop timer1;
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
  TMR1ON = literal(0); // restart timer1;
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
}

// end of file motor.c
