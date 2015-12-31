#if 0
#include <iostream>
#include <cstdint>
#include <type_traits>

#include "../include/safe_integer.hpp"

template <class T>
using safe_t = boost::numeric::safe<
    T,
    boost::numeric::native
>;

int main(){
    using namespace boost::numeric;
    {
        const safe_t<std::int8_t> t = 1;
        const std::uint32_t u = 1;
        auto z = t / u;
        std::cout << z << std::endl;
    }
    return 0;
}

#include <iostream>
#include <cstdint>

template<typename T>
struct division_result {
    static const T t = '0';
};

template<class T>
int test(){
    std::cout << "t = " << division_result<T>::t << std::endl;
    return 0;
}
int main(){
    test<std::int8_t>();
    return 0;
}
#endif

#if 0
#include <iostream>
#include <cstdint>
#include <type_traits>

#include "../include/safe_integer.hpp"
#include "../include/automatic.hpp"

template <class T>
using safe_t = boost::numeric::safe<
    T,
    boost::numeric::automatic
>;

int main(){
    using namespace boost::numeric;
    {
        const safe_t<std::int64_t> t = (std::int64_t)0x8000000000000000;
        const std::uint8_t u = (std::uint8_t)0x01;
        auto z = t * u;
        std::cout << z << std::endl;
    }
    /*
    {
        const safe_t<std::int64_t> t = (std::int64_t)0x8000000000000000;
        const safe_t<std::uint64_t> u = (std::uint64_t)0x8000000000000000;
        auto z = t / u;
        std::cout << z << std::endl;
    }
    {
        const safe_t<std::uint8_t> t = 10;
        const std::uint8_t u = 2;
        auto z = t / u;
        std::cout << z << std::endl;
    }
    {
        const safe_t<std::uint8_t> t = std::numeric_limits<safe_t<std::uint8_t>>::max();
        const std::uint8_t u = 0;
        try {
            auto z = t / u;
            std::cout << z << std::endl;
        }
        catch(const std::exception & e){}
    }
    {
        const safe_t<std::uint8_t> t = 10;
        const std::uint8_t u = 2;
        auto z = t / u;
        std::cout << z << std::endl;
    }
    {
        const safe_t<std::uint8_t> t = 10;
        const std::int8_t u = -1;
        auto z = t / u;
        std::cout << z << std::endl;
    }
    {
        const safe_t<std::int8_t> t = -10;
        const std::uint8_t u = 2;
        auto z = t / u;
        std::cout << z << std::endl;
    }
    {
        const safe_t<std::int8_t> t = 10;
        const std::int8_t u = -1;
        auto z = t / u;
        std::cout << z << std::endl;
    }
    {
        safe_t<std::int8_t> t = 1;
        std::int32_t u = 0x7fffffff;
        auto z = t / u;
        std::cout << z << std::endl;
    }
    {
        const safe_t<std::int8_t> t = 10;
        const std::int64_t u = 2;
        auto z = t / u;
        std::cout << z << std::endl;
    }
    {
        const safe_t<std::uint32_t> t = std::numeric_limits<safe_t<std::uint8_t>>::max();
        const std::int8_t u = -128;
        try {
            auto z = t / u;
            std::cout << z << std::endl;
        }
        catch(const std::exception & e){}
    }
    {
        const safe_t<std::int32_t> t = 10;
        const std::int8_t u = -1;
        auto z = t / u;
        std::cout << z << std::endl;
    }
    {
        const safe_t<std::int64_t> t = 10;
        const std::int8_t u = -1;
        auto z = t / u;
        std::cout << z << std::endl;
    }
    */
    return 0;
}

#include "../include/cpp.hpp"
#include "../include/safe_common.hpp"

using pic16_promotion = boost::numeric::cpp<
    8,  // char
    8,  // short
    8,  // int
    16, // long
    32  // long long
>;

typedef pic16_promotion::rank<short> pr;

int main(){
    return 0;
}

#include "../include/cpp.hpp"
#include "../include/safe_common.hpp"

using namespace boost::numeric;

// create custom policy which emulates native one
using custom = cpp<
    CHAR_BIT,
    CHAR_BIT * sizeof(short),
    CHAR_BIT * sizeof(int),
    CHAR_BIT * sizeof(long),
    CHAR_BIT * sizeof(long long)
>;

template<typename T>
using test = custom::rank<T>;

print_value<sizeof(int)> pv_int;
typedef print_type<test<int>> pt_r_int;

print_value<sizeof(long long)> pv_long_long;
typedef print_type<long long> pt_long_long;
typedef print_type<test<long long>> p_r_long_long;

print_value<sizeof(custom::local_long_long_type)> pv_local_long_long_type;
typedef print_type<custom::local_long_long_type> pt_local_long_long;
typedef print_type<test<custom::local_long_long_type>> p_r_local_long_long;

typedef print_type<
    std::is_same<
        long long,
        custom::local_long_long_type
    >::type
> pt_same;


// testing trap
#include "../include/exception_policies.hpp"
#include "../include/safe_integer.hpp"

using namespace boost::numeric;
template <typename T> // T is char, int, etc data type
using safe_t = safe<
    T,
    native,
    trap_exception // use for compiling and running tests
>;

template<typename T, typename U>
void test(){
    safe_t<T> t;
    safe_t<U> u;
    t + u;
    t - u;
    t * u;
    t / u;
    t % u;
    t << u;
    t >> u;
    t | u;
    t & u;
    t ^ u;
}
int main(int argc, char *argv[]){
    test<std::int8_t, std::int8_t>();
    test<std::int16_t, std::int16_t>();
    test<std::int32_t, std::int32_t>();
    test<std::int64_t, std::int64_t>();
    // this is a compile only test - but since many build systems
    // can't handle a compile-only test - make sure it passes trivially.
    return 0;
}

#endif

#include <iostream>
#include "../include/interval.hpp"

using namespace boost::numeric;

int main(){
    interval<std::int8_t> t;
    interval<std::int8_t> u;
    auto r = right_shift_positive<std::int8_t>(t, u);
    std::cout << r << '\n';
    return 0;
}

