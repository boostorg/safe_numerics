void example1(){
    std::cout << "example 1 ";
    std::cout << "(undetected erroneous expression evaluation)" << std::endl;
    try{
        char x = 127;
        char y = 2;
        char z;
        // this produces an invalid result !
        z = x + y;
        // it is the wrong result !!!
        assert(z != 129);
        // but assert fails to detect it since C++ implicitly
        // converts variables to int before evaluating he expression!
        assert(z != x + y);
        std::cout << static_cast<int>(z) << " != " << x + y;
        detected_msg(false);
    }
    catch(...){
        assert(false); // never arrive here
    }
    // solution: replace char with safe<char>
    try{
        using namespace boost::numeric;
        safe<char> x = 127;
        safe<char> y = 2;
        safe<char> z;
        // rather than producing and invalid result an exception is thrown
        z = x + y;
        assert(false); // never arrive here
    }
    catch(std::range_error & e){
        // which can catch here
        std::cout << e.what();
        detected_msg(true);
    }
}
