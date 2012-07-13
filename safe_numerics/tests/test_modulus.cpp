//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

bool test_modulus1();
bool test_modulus2();
bool test_modulus3();

int main(int argc, char * argv[]){
    bool result = true;
    result = test_modulus1();
    result &= test_modulus2();
    result &= test_modulus3();
    return ! result ;
}
