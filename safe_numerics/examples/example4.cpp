#include "../include/safe_compare.hpp"

void example4(){
    // problem: undetected underflow in data type
    try{
        unsigned int x = 0;
        // the following silently produces an incorrect result
        --x;
        // because C implicitly converts mis-matched arguments to int
        // the following assert suggests the result is correct
        assert(boost::numeric::safe_compare::equal(x, -1));
        // even though it's not !!!
        std::cout << x << " != " << -1;
        detected_msg(false);
    }
    catch(...){
        assert(false); // never arrive here
    }
    // solution: replace unsigned int with safe<unsigned int>
    try{
        using namespace boost::numeric;
        safe<unsigned int> x = 0;
        // decrement unsigned to less than zero throws exception
        --x;
        assert(false); // never arrive here
    }
    catch(std::range_error & e){
        std::cout << e.what();
        detected_msg(true);
    }
}
