void example3(){
    std::cout << "example 3 ";
    std::cout << "(implicit conversions change data values" << std::endl;
    try{
        int x = -1000;
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
        safe<int> x = -1000;
        // throws exception when conversion change data value
        safe<char> y = x;
        assert(false); // never arrive here
    }
    catch(std::range_error & e){
        std::cout << e.what();
        detected_msg(true);
    }
}
