void example3(){
    // problem: implicit conversions change data values
    try{
        int x = -1;
        // the following silently produces an incorrect result
        char y = x;
        detected_msg(false);
    }
    catch(...){
        assert(false); // never arrive here
    }
    // solution: replace int with safe<int> and char with safe<char>
    try{
        using namespace boost::numeric;
        safe<int> x = -1;
        // throws exception when conversion change data value
        safe<char> y = x;
        assert(false); // never arrive here
    }
    catch(std::range_error & e){
        std::cout << e.what();
        detected_msg(true);
    }
}
