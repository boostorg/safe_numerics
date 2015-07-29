#include <cassert>
#include <stdexcept>
#include <sstream>
#include <iostream>

#include "../include/safe_integer.hpp"
#include "../include/cpp.hpp"

//////////////////////////////////////////////////////////////
// Stepper Motor Control

// emululate evironment for pic162550

// data widths used by the CCS compiler for pic 16xxx series
using pic16_promotion = boost::numeric::cpp<
    8,  // char
    8,  // short - not used by pic 16xxxx
    8,  // int
    16, // long
    32  // long long
>;

template <typename T> // T is char, int, etc data type
using safe_t = boost::numeric::safe<
    T,
    pic16_promotion,
    boost::numeric::trap_exception  // use for running tests
>;

using int8 = safe_t<std::int8_t>;
using int16 = safe_t<std::int16_t>;
using int32 = safe_t<std::int32_t>;
using uint8 = safe_t<std::uint8_t>;
using uint16 = safe_t<std::uint16_t>;
using uint32 = safe_t<std::uint32_t>;

//////////////////////////////////////////////////////////////
// Mock defines, functions etc which are in he "real application
using BOOLEAN = bool;
#define TRUE true
#define FALSE false

using LEMPARAMETER = int16;

#define STEPS_PER_MM 200
#define STEP        0
#define STEP_LOW        0
#define STEP_HIGH       1
#define STEPPING_LIGHT   0 // Labeled D3

#define MAXIMUM_TIME 0xffff
#define INSTRUCTIONS_PER_SECOND ((uint32)MIPS * 0x100000)
// since we have a 48 mHz clock =>
#define MIPS 12

// stepper motor limit switches
#define LIMIT_OUT   0  // input
#define LIMIT_IN    1  // input

// switches are Normally/Closed
#define LIMIT_NOT_HIT   0
#define LIMIT_HIT       1

// gecko microstepper output
#define DIRECTION_IN    0
#define DIRECTION_OUT   1
#define DIRECTION  0
#define DIRECTION_LIGHT  0
#define LIGHT_OFF   0
#define LIGHT_ON    1

#define END_CLEARENCE 10
#define HOME_OUT ((SLIDE_LENGTH - END_CLEARENCE) * STEPS_PER_MM)
#define HOME_IN (END_CLEARENCE * STEPS_PER_MM)
#define SLIDE_LENGTH 155

BOOLEAN report_arrival = FALSE;
typedef enum {
    position_counter	= 0,	// in steps
    	// sets, initializes the position counter in steps
    	// response with new value when set and when stage stops
    fault               = 11,
} pcode_t;

// fault codes
typedef enum {
    response_queue_overflow     = 0,
    sample_queue_overflow       = 1,
    unanticipated_interrupt     = 2,
    oscillator_failure          = 3,
    low_voltage_detected        = 4,
    ad_max_rate_exceeded        = 5,
    unspecified_fault           = 9
} fcode_t;

BOOLEAN input(uint8){
    return TRUE;
}
void output_bit(uint8, BOOLEAN){
}
void delay_us(uint8){};
// just factor out macro expansion to save memory
void enqueue_response(
    pcode_t p, 
    LEMPARAMETER * v, 
    fcode_t f = unspecified_fault
){}

#define MAIL_BOX(name, type) type name
#define mail_box_put(name, value) (name = value)
#define mail_box_get(name, destination) (destination = name)
#define mail_box_isempty(name) false

MAIL_BOX(current_velocity, LEMPARAMETER);
MAIL_BOX(target_position, LEMPARAMETER);
MAIL_BOX(current_position, LEMPARAMETER);
MAIL_BOX(dt, uint16);

struct {
    // acceleration constant.  application of a signal to the controller
    // initiates movement according to the direction.  The acceleration
    // depends upon the voltage on the coil, current limiting and load on the
    // stage.  Generally this will be determined by experimentation.  If its 
    // determined that the stepper is skipping steps, we should lower this constant
    // to reflect the fact that things accelerate more slowly than we've assumed.
    // if we want to move the stage faster we should increase the voltage, 
    // increase the maximum current, and DEcrease the value of this constant.
    LEMPARAMETER acceleration;
        // (5 * 200) steps/sec/sec // => 1 sec to reach nominal 5 mm/sec speed
    
    LEMPARAMETER max_velocity;  // => 30 mm second => 5 sec for full travel ;
    LEMPARAMETER min_velocity;  // => 1 mm second => 150 sec for full travel;
        // nominal value would be (5 * STEPS_PER_MM) => 150 mm travel in 30 sec
    
    // current state of stage
        // velocity in steps / second 
    LEMPARAMETER current_velocity; // +/- mm/second depending on direction
    LEMPARAMETER current_position; // current position in steps.
    LEMPARAMETER target_position;
        // 200 steps/mm * 152 mm travel gives maximum 30480
    LEMPARAMETER sampling_on;
        // turn sampling on - turns light on also
    LEMPARAMETER min_sample_position;
    LEMPARAMETER max_sample_position;
        // define the range of positions between samples will 
        // be gathered
    LEMPARAMETER steps_per_sample;
        // a power of two 1, 2, 4, 8, ...
        // samples will be taken when the position counter modulo
        // steps_per_sample is zero

    // the following are dependent on the above.  They are updated whenever
    // one of the variables they depend upon ar updated.
    uint8 sample_mask;
        // save some time by pre-calculating
        // ~(lem.steps_per_sample - 1) which masks off the high
        // order bits
    uint8 sample_setup;
        // magic constant for the a/d conversion at the proper rate
} lem = {
    (5 * 1000),             // acceleration
    (30 * STEPS_PER_MM),    // max_velocity
    (1 * STEPS_PER_MM),     // min_velocity
    0,                      // current_velocity
    0,                      // position_counter
    0,                      // target_position
    0,                      // sampling on
    800,                    // min_sample_position
    29680,                  // max_sample_position
    32,                     // steps_per_sample
    0x1f, //~(32 - 1),      // sample mask};
    0
};

// return value in steps
/*
Use the formula:
    stopping dist = v **2 / a / 2
*/
uint16 get_stopping_distance(LEMPARAMETER velocity){
    int32 d;
    d = velocity;
    //d *= velocity;

    d = velocity * velocity;
    d /= lem.acceleration;
    d /= 2;
    return d;
}

int8 get_acceleration(
    LEMPARAMETER dp, 
    LEMPARAMETER velocity
){
    int8 a;
    if(dp > 0){
        // target is farther out than we are
        if(velocity > 0){
            // moving out
            LEMPARAMETER sd; // stopping distance
            sd = get_stopping_distance(velocity);
            if(dp > sd){
                // far from the destination
                if(velocity > lem.max_velocity)
                    a = -1;
                else
                if(velocity == lem.max_velocity)
                    a = 0;
                else
                    a = 1;
            }
            else{
                // close to the destination
                a = -1;
            }
        }
        else
        if(velocity < 0){
            // moving in
            a = 1; // turn around
        }
        else
            a = 1;
    }
    else
    if(dp < 0){
        // we're farther out than the target
        if(velocity < 0){
            // moving left
            LEMPARAMETER sd; // stopping distance
            sd = get_stopping_distance(velocity);
            if(dp < -sd){
                // far from the destination
                if(velocity < - lem.max_velocity)
                    a = 1;
                else
                if(velocity == - lem.max_velocity)
                    a = 0;
                else
                    a = -1;
            }
            else{
                // close to the destination
                a = 1;
            }
        }
        else
        if(velocity > 0){
            // moving right
            a = -1;
        }
        else
            a = -1;
    }
    else{
        // we're there
        if(velocity < - lem.min_velocity)
            a = 1;
        else
        if(velocity > lem.min_velocity)
            a = -1;
        else
            a = 0; // shouild never get here
    }
    return a;
}

// update velocity according to acceleration and
// distance to target postion
void motor_velocity_update(){
    int8 a;
    static uint16 dt = MAXIMUM_TIME;

    static struct {
        LEMPARAMETER current_position;
        LEMPARAMETER target_position;
    } shadow_lem; 
    LEMPARAMETER dp; // difference between target and current
    LEMPARAMETER velocity;
    LEMPARAMETER previous_position;

    if(mail_box_isempty(lem.current_position))
        return;

    mail_box_get(lem.current_position, shadow_lem.current_position);
    mail_box_get(lem.target_position, shadow_lem.target_position);

    velocity = lem.current_velocity;
    dp = shadow_lem.target_position - shadow_lem.current_position;
    a = get_acceleration(dp, velocity);
    
    if(0 != a){
        uint32 dv;
        int16 pcount;
        pcount = shadow_lem.current_position - previous_position;
        if(pcount < 0)
            pcount = - pcount;
        dv = (uint32)lem.acceleration * dt * pcount;
        dv /= INSTRUCTIONS_PER_SECOND;
        if(0 == dv)
            return;
        if(0 < a)
            velocity += dv;
        else
            velocity -= dv;
    }
    else
        if(0 == dp)
            velocity = 0;
    previous_position = shadow_lem.current_position;

    // figure new pulse width
    if(lem.min_velocity < velocity)
        dt = INSTRUCTIONS_PER_SECOND / velocity;
    else
    if(- lem.min_velocity < velocity)
        dt = MAXIMUM_TIME;
    else
        dt = - INSTRUCTIONS_PER_SECOND / velocity;

    mail_box_put(current_velocity, velocity);
    mail_box_put(dt, dt);

    // make sure input mail box is empty so that next time we get a fresh one
    mail_box_get(current_position, shadow_lem.current_position);    
}

///////////////////////////////////////////////////////////////
// invoked at main timer interrupt time

BOOLEAN check_collision(){
    static BOOLEAN collision_recovery = FALSE;

    if(collision_recovery){
        if(lem.target_position == lem.current_position)
            collision_recovery = FALSE;
        return TRUE;
    }
    if(LIMIT_HIT == input(LIMIT_IN)){
        if(LIMIT_HIT == input(LIMIT_IN)){
            lem.current_position = 0;
            lem.target_position = HOME_IN;
            lem.current_velocity = lem.min_velocity;
            collision_recovery = TRUE;
            mail_box_put(target_position, lem.target_position);
            return TRUE;
       }
    }
    else
    if(LIMIT_HIT == input(LIMIT_OUT)){
        if(LIMIT_HIT == input(LIMIT_OUT)){
            lem.current_position = SLIDE_LENGTH  * STEPS_PER_MM;
            lem.target_position = HOME_OUT;
            lem.current_velocity = - lem.min_velocity;
            collision_recovery = TRUE;
            mail_box_put(target_position, lem.target_position);
            return TRUE;
        }
    }
    return FALSE;
}

void motor_step(){
    output_bit(STEP, STEP_LOW);
    delay_us(4);
    output_bit(STEP, STEP_HIGH);
}

void motor_increment(){
    output_bit(DIRECTION, DIRECTION_OUT);
    #if(TARGET==DEVBOARD)
        output_bit(STEPPING_LIGHT, LIGHT_ON);
        output_bit(DIRECTION_LIGHT, DIRECTION_OUT);
    #endif
    ++lem.current_position;
    motor_step();
}

void motor_decrement(){
    output_bit(DIRECTION, DIRECTION_IN);
    #if(TARGET==DEVBOARD)
        output_bit(STEPPING_LIGHT, LIGHT_ON);
        output_bit(DIRECTION_LIGHT, DIRECTION_IN);
    #endif
    --lem.current_position;
    motor_step();
}

BOOLEAN check_arrival(){
    if(lem.current_position != lem.target_position)
        return FALSE;    
    #if(TARGET==DEVBOARD)
        output_bit(STEPPING_LIGHT, LIGHT_OFF);
    #endif
    if(report_arrival){
        report_arrival = FALSE;
        enqueue_response(position_counter, & lem.current_position);
    }
    return TRUE;
}

void motor_update(){
    mail_box_get(current_velocity, lem.current_velocity);
    check_collision();
    if(0 < lem.current_velocity){
        if(lem.min_velocity < lem.current_velocity){
            motor_increment();
        }
        else{
            if(!check_arrival())
                motor_increment();
        }
    }
    else
    if(0 > lem.current_velocity){
        if(-lem.min_velocity > lem.current_velocity){
            motor_decrement();
        }
        else{
            if(!check_arrival())
                motor_decrement();
        }
    }
    else{
        check_arrival();
    }
    mail_box_put(current_position, lem.current_position);
}

int main(int argc, const char * argv[]){
    // problem: testing against other architectures
    std::cout << "example 9: ";
    std::cout << "testing against other architectures"
    << std::endl;

    return 0;
}
