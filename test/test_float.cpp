//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// testing floating point

// this is a compile only test - but since many build systems
// can't handle a compile-only test - make sure it passes trivially.
#include <cassert>
#include "../include/safe_integer.hpp"

template<typename T, typename U>
void test(){
    T t;
    U u;
    t + u;
    t - u;
    t * u;
    t / u;
    // the operators below are restricted to integral types
}
int main(int argc, char *argv[]){
    using namespace boost::numeric;
    // compiles OK but seems to throw with the
    // following output (Ubuntu 16.06 / clang 3.8):
    // john macfarland
    // fixed now
    boost::numeric::safe<int8_t> i = 1;
    float f = i;
    int8_t j = i;
    assert(j == i);
    test<safe<std::int8_t>, float>();
    test<safe<std::int16_t>,float>();
    test<safe<std::int32_t>, float>();
    test<safe<std::int64_t>, float>();
    return 0;
}
