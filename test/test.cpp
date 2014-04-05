//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <exception>
#include <cstdlib>   // EXIT_SUCCESS

#include <boost/mpl/print.hpp>

#include "../include/safe_range.hpp"
#include "../include/safe_integer.hpp"

bool test1(){
    boost::numeric::safe_signed_range<-64, 63> x, y, z;
    x = 1;
    y = 2;
    z = 3;
    z = x + y;
    z = x - y;
    short int yi, zi;
    zi = x;
    typedef boost::mpl::print<
        boost::numeric::addition_result_bits<
            boost::numeric::safe_signed_range<-64, 63>,
            int
        >::type
    >::type t1;
    typedef boost::mpl::print<
        boost::numeric::bits<
            boost::numeric::safe_signed_range<-64, 63>
        >::type
    >::type t2;
    typedef boost::mpl::print<
        boost::numeric::bits<
            int
        >::type
    >::type t3;
    zi = x + yi;
    
    bool success = false;
    try{
        z = x + yi; // should trap here
    }
    catch(std::exception e){
        success = true;
    }
    return success;
}

bool test2(){
    boost::numeric::safe_unsigned_range<0, 64> x, y, z;
    x = 1;
    y = 2;
    z = 3;

    bool success;
    success = false;
    try{
        z = x - y; // should trap here
    }
    catch(std::exception e){
        success = true;
    }
    if(success == false)
        return false;
    
    return success;
    int yi, zi;
    zi = x;
    zi = x + yi;
    success = false;
    try{
        z = x + yi; // should trap here
    }
    catch(std::exception e){
        success = true;
    }
    return success;
}

bool test3(){
    boost::numeric::safe<int> x, y, z;
    x = 1;
    y = 2;
    z = 3;
    z = x + y;
    z = x - y;
    int yi, zi;
    zi = x;
    zi = x + yi;
    z = x + yi;
    return true;
}

bool test4(){
    boost::numeric::safe<unsigned int> x, y, z;
    x = 1;
    y = 2;
    z = 3;
    z = x + y;
    bool success;
    success = false;
    try{
        z = x - y; // should trap here
    }
    catch(std::exception e){
        success = true;
    }
    if(success == false)
        return false;
    unsigned int yi, zi;
    zi = x;
    zi = x + yi;
    z = x + yi;
    zi = x + y;
    return true;
}

#include <boost/cstdint.hpp>

bool test5(){
    boost::numeric::safe<boost::uint64_t> x, y, z;
    x = 1;
    y = 2;
    z = 3;
    z = x + y;
    z = x - y;
    boost::uint64_t yi, zi;
    zi = x;
    zi = x + yi;
    z = x + yi;
    zi = x + y;
    return true;
}

int main(int argc, char *argv[]){
    return (
        test1() &&
        test2() &&
        test3() &&
        test4() &&
        test5()
    ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
