void example2(){
    // problem: undetected overflow in data type
    try{
        int x = INT_MAX;
        // the following silently produces an incorrect result
        ++x;
        //std::cout << x << " != " << -1;
        detected_msg(false);
    }
    catch(...){
        assert(false); // never arrive here
    }
    // solution: replace int with safe<int>
    try{
        using namespace boost::numeric;
        safe<int> x = INT_MAX;
        // throws exception when result is past maximum possible 
        ++x;
        assert(false); // never arrive here
    }
    catch(std::range_error & e){
        std::cout << e.what();
        detected_msg(true);
    }
}
