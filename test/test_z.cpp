
#include "../include/safe_integer.hpp"
#include "../include/native.hpp"

int main(){
    using namespace boost::numeric;
/*

    safe<uint16_t> x = 0xffff;
    uint16_t y = 0xffff;

    typedef boost::numeric::native::multiplication_result<
        decltype(x),
        decltype(y),
        native,
        void
    >::type result_type;
    print<result_type> p_rt;
    print<decltype(y + y)> p_yy;

    x * y;
*/
    return 0;
}
