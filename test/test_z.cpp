
#include "../include/automatic.hpp"

int main(){
    using namespace boost::numeric;

    std::uint64_t t = 0x7ffffffffffffffful;
    std::int8_t u = 0x01;

    typedef boost::numeric::automatic::subtraction_result<
        std::uint64_t,
        std::int8_t,
        automatic,
        void
    >::type result_type;

    //print<result_type> p_rt;

    return 0;
}
