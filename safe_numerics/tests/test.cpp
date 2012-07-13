#include "../include/safe_range.hpp"

void test1(){
    boost::numeric::safe_signed_range<-64, 63> x, y, z;
    x = 1;
    y = 2;
    z = 3;
    z = x + y;
    z = x - y;
    short int yi, zi;
    zi = x;
    typedef boost::mpl::print<
        boost::numeric::addition_result_bits<
            boost::numeric::safe_signed_range<-64, 63>,
            int
        >::type
    >::type t1;
    typedef boost::mpl::print<
        boost::numeric::bits<
            boost::numeric::safe_signed_range<-64, 63>
        >::type
    >::type t2;
    typedef boost::mpl::print<
        boost::numeric::bits<
            int
        >::type
    >::type t3;

    zi = x + yi;
    z = x + yi;
}

void test2(){
    boost::numeric::safe_unsigned_range<0, 64> x, y, z;
    x = 1;
    y = 2;
    z = 3;
    z = x + y;
    z = x - y;
    int yi, zi;
    zi = x;
    zi = x + yi;
    z = x + yi;
}

void test3(){
    boost::numeric::safe_int x, y, z;
    x = 1;
    y = 2;
    z = 3;
    z = x + y;
    z = x - y;
    int yi, zi;
    zi = x;
    zi = x + yi;
    z = x + yi;
}

void test4(){
    boost::numeric::safe_unsigned_int x, y, z;
    x = 1;
    y = 2;
    z = 3;
    z = x + y;
    z = x - y;
    unsigned int yi, zi;
    zi = x;
    zi = x + yi;
    z = x + yi;
    zi = x + y;
}

#include <boost/cstdint.hpp>

void test5(){
    boost::numeric::safe_integer<boost::uint64_t> x, y, z;
    x = 1;
    y = 2;
    z = 3;
    z = x + y;
    z = x - y;
    boost::uint64_t yi, zi;
    zi = x;
    zi = x + yi;
    z = x + yi;
    zi = x + y;
}
