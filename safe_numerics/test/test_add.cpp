//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

bool test_add1();
bool test_add2();
bool test_add3();

int main(int argc, char * argv[]){
    bool result = true;
    result = test_add1();
    result &= test_add2();
    result &= test_add3();
    return ! result ;
}
