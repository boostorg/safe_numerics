#include <iostream>
#include <cassert>
#include <limits>

#include "../include/safe_range.hpp"

template<typename T>
void display_ulog(T Max){
    std::cout
        << "ulog(" << Max << ") = "
        << boost::numeric::detail::ulog(Max) << std::endl;
}
template<typename T>
void display_log(T Max){
    std::cout
        << "log(" << Max << ") = "
        << boost::numeric::detail::log(Max) << std::endl;
}

void test_log(){
    using namespace boost::numeric::detail;
    assert(ulog(127) == 7); // 7 bits
    assert(ulog(127) == 7); // 7 bits
    assert(ulog(128) == 8); // 8 bits
    assert(ulog(129) == 8); // 8 bits
    assert(ulog(255) == 8); // 8 bits
    assert(ulog(256) == 9); // 9 bits

    assert(log(127) == 8); // 7 bits + 1 sign bit
    assert(log(128) == 9); // 8 bits + 1 sign bit
    assert(log(129) == 9); // 8 bits + 1 sign bit
    assert(log(255) == 9); // 8 bits + 1 sign bit
    assert(log(256) == 10); // 9 bits + 1 sign bit

    assert(log(-127) == 8); // 7 bits + 1 sign bit
    assert(log(-128) == 8); // 7 bits + 1 sign bit
    assert(log(-129) == 9); // 8 bits + 1 sign bit
    assert(log(-255) == 9); // 8 bits + 1 sign bit
    assert(log(-256) == 9); // 8 bits + 1 sign bit
}

template<typename Tx>
struct print {
    typedef typename Tx::error_message type;
};

int main(){
    using namespace boost::numeric;
    typedef detail::signed_stored_type<-256, 254> t1;

    safe_signed_range<-128, 127> s1(1);
    safe_signed_range<-256, 254> s2(2);

    typedef safe_unsigned_range<0u, 1000u> t2;
    static_assert(
        std::numeric_limits<t2>::is_signed == false,
        "this range should be unsigned"
    );
    //typedef ::print<t2>::type p_t2;

    return 0;
}