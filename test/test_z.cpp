#include "../include/safe_integer.hpp"

int main(){
    using namespace boost::numeric;
    //safe_signed_literal2<100> one_hundred;
    //one_hundred = 200;

    int i = 1;
    safe<int> j = 2;
    j + i;
    //i + j;
    return 0;
}
