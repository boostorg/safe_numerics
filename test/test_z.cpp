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
