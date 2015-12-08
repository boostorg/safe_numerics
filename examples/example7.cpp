#include <cassert>
#include <stdexcept>
#include <sstream>
#include <iostream>

#include "../include/safe_range.hpp"

// NOT using safe numerics - enforce program contract explicitly
// return total number of minutes
unsigned int convert(
    const unsigned int & hours,
    const unsigned int & minutes
) {
    // check that parameters are within required limits
    // invokes a runtime cost EVERYTIME the function is called
    // and the overhead of supporting an interrupt.
    // note high runtime cost!
    if(minutes > 59)
        throw std::domain_error("minutes exceeded 59");
    if(hours > 23)
        throw std::domain_error("hours exceeded 23");
    return hours * 60 + minutes;
}

// Use safe numeric to enforce program contract automatically
// define convient typenames for hours and minutes hh:mm
using hours_t = boost::numeric::safe_unsigned_range<0, 23>;
using minutes_t = boost::numeric::safe_unsigned_range<0, 59>;

// return total number of minutes
// type returned is safe_unsigned_range<0, 24*60 - 1>
auto safe_convert(const hours_t & hours, const minutes_t & minutes) {
    // no need for checking as parameters are guaranteed to be within limits
    // expression below cannot throw ! zero runtime overhead
    return hours * 60 + minutes;
}

int main(int argc, const char * argv[]){
    std::cout << "example 7: ";
    std::cout << "enforce contracts with zero runtime cost" << std::endl;
    std::cout << "Not using safe numerics" << std::endl;

    // problem: checking of externally produced value can be expensive
    try {
        convert(10, 83); // invalid parameters - detected - but at a heavy cost
    }
    catch(std::exception e){
        std::cout << "exception thrown for parameter error" << std::endl;
    }

    // solution: use safe range to restrict parameters
    std::cout << "Using safe numerics" << std::endl;

    try {
        // parameters are guarenteed to meet requirements
        hours_t hours(10);
        minutes_t minutes(83);  // interrupt thrown here
        // so the following will never throw
        safe_convert(hours, minutes);
    }
    catch(std::exception e){
        std::cout
            << "exception thrown when invalid arguments are constructed"
            << std::endl;
    }

    try {
        // parameters are guarenteed to meet requirements when
        // constructed on the stack
        safe_convert(hours_t(10), minutes_t(83));
    }
    catch(std::exception e){
        std::cout
            << "exception thrown when invalid arguments are constructed on the stack"
            << std::endl;
    }

    try {
        // parameters are guarenteed to meet requirements when
        // implicitly constructed to safe types to match function signature
        safe_convert(10, 83);
    }
    catch(std::exception e){
        std::cout
            << "exception thrown when invalid arguments are implicitly constructed"
            << std::endl;
    }

    try {
        // the following will never throw as the values meet requirements.
        const hours_t hours(10);
        const minutes_t minutes(17);

        // note zero runtime overhead once values are constructed

        // the following will never throw because it cannot be called with
        // invalid parameters
        safe_convert(hours, minutes); // zero runtime overhead

        // since safe types can be converted to their underlying unsafe types
        // we can still call an unsafe function with safe types
        convert(hours, minutes); // zero (almost) runtime overhead

        // since unsafe types can be implicitly converted to corresponding
        // safe types we can just pass the unsafe types.  checkin will occur
        // when the safe type is constructed.
        safe_convert(10, 17); // runtime cost in creating parameters

    }
    catch(std::exception e){
        std::cout << "error detected!" << std::endl;
    }

    return 0;
}
