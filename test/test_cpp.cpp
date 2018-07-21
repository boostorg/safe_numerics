#include <boost/integer.hpp>
#include "../include/utility.hpp"

// include headers to support safe integers
#include "../include/cpp.hpp"
//#include "../include/exception.hpp"

using promotion_policy = boost::numeric::cpp<
    8,  // char      8 bits
    16, // short     16 bits
    16, // int       16 bits
    16, // long      32 bits
    32  // long long 32 bits
>;

template<typename R, typename T, typename U>
struct test {
    using ResultType = promotion_policy::result_type<T,U>;
    //boost::numeric::utility::print_type<ResultType> pt;
    static_assert(
        std::is_same<R, ResultType>::value,
        "is_same<R, ResultType>"
    );
};

test<std::uint16_t, std::uint8_t, std::uint8_t> t1;

int main(){
    return 0;
}

