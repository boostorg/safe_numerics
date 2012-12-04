//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

bool test_multiply1();
bool test_multiply2();
bool test_multiply3();

int main(int argc, char * argv[]){
    bool result = true;
    result = test_multiply1();
    result &= test_multiply2();
    result &= test_multiply3();
    return ! result ;
}
