#include <cassert>
#include <iostream>
#include <ostream>
#include <cstdint>
#include <algorithm> // max, min
#include <limits>

#include <boost/core/demangle.hpp>

#include "../include/checked_result.hpp"
#include "../include/checked_integer.hpp"
#include "../include/interval.hpp"

// test simple interval addition
bool test1(){
    using namespace boost::numeric;
    std::cout << "test1" << std::endl;
    interval<std::int16_t> x = {-64, 63};
    std::cout << "x = " << x << std::endl;
    interval<std::int16_t> y(-128, 126);
    std::cout << "y = " << y << std::endl;
    assert(static_cast<interval<std::int16_t>>(add<std::int16_t>(x,x)) == y);
    if(add<std::int16_t>(x,x) != y)
        return false;
    std::cout << "x + x =" << add<std::int16_t>(x, x) << std::endl;
    std::cout << "x - x = " << subtract<std::int16_t>(x, x) << std::endl;
    return true;
}

// test simple interval equality
bool test2(){
    using namespace boost::numeric;
    std::cout << "test2" << std::endl;
    boost::numeric::interval<std::int16_t> x = {-64, 63};
    std::cout << "x = " << x << std::endl;
    std::cout << std::boolalpha << "(x == x) = " << (x == x) << std::endl;
    return true;
}

// test erroneous addition
bool test3(){
    using namespace boost::numeric;
    std::cout << "test3" << std::endl;
    interval<std::int8_t> t;
    std::cout << "t = " << t << std::endl;
    interval<std::uint64_t> u;
    std::cout << "u = " << u << std::endl;
    using max_t = unsigned long long;
    interval<checked_result<max_t>> r = add<max_t>(t, u);
    std::cout << "r = " << r << std::endl;
    return true;
}

// test simple interval addition
bool test6(){
    using namespace boost::numeric;
    std::cout << "test6" << std::endl;
    interval<std::uint8_t> x;
    std::cout << "x = " << x << std::endl;
    interval<std::int16_t> y(0, 510);
    std::cout << "y = " << y << std::endl;
    
    if(add<std::int16_t>(x,x) != y)
        return false;
    std::cout << "x + x =" << add<std::int16_t>(x, x) << std::endl;
    std::cout << "x - x = " << subtract<std::int16_t>(x, x) << std::endl;
    return true;
}

// test simple interval inclusion
template<bool ExpectedResult, typename T, typename U>
bool test5(){
    using namespace boost::numeric;
    std::cout << "test5" << std::endl;
    interval<T> t;
    std::cout << "t = " << t << std::endl;
    interval<U> u;
    std::cout << "u = " << u << std::endl;
    if(t.includes(u))
        std::cout << "t includes u\n";
    if(u.includes(t))
        std::cout << "u includes t\n";
    if(!t.includes(u) && ! u.includes(t))
        std::cout << "neither interval includes the other\n";
    return ExpectedResult == t.includes(u);
}

// test simple interval union
bool test7(){
    using namespace boost::numeric;
    std::cout << "test6" << std::endl;
    interval<std::int8_t> x(-23, 47);
    std::cout << "x = " << x << std::endl;
    interval<std::uint16_t> y(0, 510);
    std::cout << "y = " << y << std::endl;
    std::cout << "x U y =" << union_interval<std::int16_t>(x, y) << std::endl;
    return true;
}

namespace test4 {
    using namespace boost::numeric;
    using max_t = std::intmax_t;

    // utilities

    // figure the lower portion of range which includes zero
    template<typename Tx>
    constexpr static const interval<Tx> r_lower(const interval<Tx> & t){
        static_assert(
            std::is_literal_type< interval<Tx> >::value,
            "interval<Tx> is not literal type"
        );
        return interval<Tx>(
            t.l,
            std::min(Tx(-1), t.u)
        );
    }

    // figure the upper portion of range which includes zero
    template<typename Tx>
    constexpr static const interval<Tx> r_upper(const interval<Tx> & t){
        static_assert(
            std::is_literal_type< interval<Tx> >::value,
            "interval<Tx> is not literal type"
        );
        return interval<Tx>(
            std::max(Tx(1), t.l),
            t.u
        );
    }

    template<typename T, typename U>
    bool divide_result(){
        using namespace boost::numeric;
        std::cout << "divide result" << std::endl;
        const interval<T> t;
        std::cout
            << boost::core::demangle(typeid(t).name())
            << " t = "
            << t << std::endl;
        const interval<U> u;
        std::cout
            << boost::core::demangle(typeid(u).name())
            << " u = "
            << u << std::endl;
        const interval<checked_result<max_t>> rx = divide<max_t>(t, u); // r(t, u);
        std::cout
            << boost::core::demangle(typeid(rx).name())
            << " rx = "
            << rx
            << std::endl;
        return true;
    }

    template<typename T, typename U>
    bool test_multiply(){
        using namespace boost::numeric;
        std::cout << "test4::multiply" << std::endl;
        const interval<T> t_interval = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };
        std::cout
            << boost::core::demangle(typeid(t_interval).name())
            << " t_interval = "
            << t_interval
            << std::endl;
        const interval<U> u_interval = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };
        std::cout
            << boost::core::demangle(typeid(u_interval).name())
            << " u_interval = "
            << u_interval
            << std::endl;
        using R = decltype(U() * T());
        const interval<checked_result<max_t>> r_interval
            = multiply<R>(t_interval, u_interval);
        std::cout
            << boost::core::demangle(typeid(r_interval).name())
            << " r_interval = "
            << r_interval
            << std::endl;
        return true;
    }

    template<typename T, typename U>
    bool test_divide(){
        using namespace boost::numeric;
        std::cout << "test4::test4 divide" << std::endl;
        const interval<T> t_interval = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };
        std::cout
            << boost::core::demangle(typeid(t_interval).name())
            << " t_interval = "
            << t_interval
            << std::endl;
        const interval<U> u_interval = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };
        std::cout
            << boost::core::demangle(typeid(u_interval).name())
            << " u_interval = "
            << u_interval
            << std::endl;
        using R = decltype(U() * T());
        const interval<checked_result<max_t>> r_interval
            = divide<R>(t_interval, u_interval);
        std::cout
            << boost::core::demangle(typeid(r_interval).name())
            << " r_interval = "
            << r_interval
            << std::endl;
        return true;
    }

    template<typename T, typename U>
    bool test_add(
        const interval<T> & t_interval,
        const interval<U> & u_interval
    ){
        using namespace boost::numeric;
        std::cout << "test4::test4 add" << std::endl;
        std::cout
            << boost::core::demangle(typeid(t_interval).name())
            << " t_interval = "
            << t_interval
            << std::endl;
        std::cout
            << boost::core::demangle(typeid(u_interval).name())
            << " u_interval = "
            << u_interval
            << std::endl;
        using R = decltype(U() + T());
        const interval<checked_result<max_t>> r_interval
            = add<R>(t_interval, u_interval);
        std::cout
            << boost::core::demangle(typeid(r_interval).name())
            << " r_interval = "
            << r_interval
            << std::endl;
        return true;
    }
    template<typename T, typename U>
    bool test_add(){
        return test_add<T,U>(interval<T>(), interval<U>());
    }

    template<typename T, typename U>
    bool test_subtract(
        const interval<T> & t_interval,
        const interval<U> & u_interval
    ){
        using namespace boost::numeric;
        std::cout << "test4::test4 subtract" << std::endl;
        std::cout
            << boost::core::demangle(typeid(t_interval).name())
            << " t_interval = "
            << t_interval
            << std::endl;
        std::cout
            << boost::core::demangle(typeid(u_interval).name())
            << " u_interval = "
            << u_interval
            << std::endl;
        using R = decltype(U() * T());
        const interval<checked_result<max_t>> r_interval
            = subtract<R>(t_interval, u_interval);
        std::cout
            << boost::core::demangle(typeid(r_interval).name())
            << " r_interval = "
            << r_interval
            << std::endl;
        return true;
    }
    template<typename T, typename U>
    bool test_subtract(){
        return test_subtract<T,U>(interval<T>(), interval<U>());
    }

    template<typename T, typename U>
    bool test_left_shift(
        const interval<T> & t_interval,
        const interval<U> & u_interval
    ){
        using namespace boost::numeric;
        std::cout << "test4::test4 left shift" << std::endl;
        std::cout
            << boost::core::demangle(typeid(t_interval).name())
            << " t_interval = "
            << t_interval
            << std::endl;
        std::cout
            << boost::core::demangle(typeid(u_interval).name())
            << " u_interval = "
            << u_interval
            << std::endl;
        using R = decltype(T() << U());
        const interval<checked_result<R>> r_interval
            = left_shift<R>(t_interval, u_interval);
        std::cout
            << boost::core::demangle(typeid(r_interval).name())
            << " r_interval = "
            << r_interval
            << std::endl;
        return true;
    }
    template<typename T, typename U>
    bool test_left_shift(){
        return test_left_shift<T,U>(interval<T>(), interval<U>());
    }
    template<typename T, typename U>
    bool test_right_shift(
        const interval<T> & t_interval,
        const interval<U> & u_interval
    ){
        using namespace boost::numeric;
        std::cout << "test4::test4 right shift" << std::endl;
        std::cout
            << boost::core::demangle(typeid(t_interval).name())
            << " t_interval = "
            << t_interval
            << std::endl;
        std::cout
            << boost::core::demangle(typeid(u_interval).name())
            << " u_interval = "
            << u_interval
            << std::endl;
        using R = decltype(T() << U());
        const interval<checked_result<max_t>> r_interval
            = right_shift<R>(t_interval, u_interval);
        std::cout
            << boost::core::demangle(typeid(r_interval).name())
            << " r_interval = "
            << r_interval
            << std::endl;
        return true;
    }
    template<typename T, typename U>
    bool test_right_shift(){
        return test_right_shift<T,U>(interval<T>(), interval<U>());
    }
    template<typename T, typename U>
    bool test_mod(
        const interval<T> & t_interval,
        const interval<U> & u_interval
    ){
        using namespace boost::numeric;
        std::cout << "test4::test4 mod" << std::endl;
        std::cout
            << boost::core::demangle(typeid(t_interval).name())
            << " t_interval = "
            << t_interval
            << std::endl;
        std::cout
            << boost::core::demangle(typeid(u_interval).name())
            << " u_interval = "
            << u_interval
            << std::endl;
        using R = decltype(T() % U());
        const interval<checked_result<max_t>> r_interval
            = modulus<R>(t_interval, u_interval);
        std::cout
            << boost::core::demangle(typeid(r_interval).name())
            << " r_interval = "
            << r_interval
            << std::endl;
        return true;
    }
    template<typename T, typename U>
    bool test_mod(){
        return test_mod<T,U>(interval<T>(), interval<U>());
    }
    template<typename T, typename U>
    bool test(){
        return
            test_multiply<T, U>() &&
            test_divide<T, U>() &&
            test_add<T, U>() &&
            test_subtract<T, U>() &&
            test_left_shift<T, U>() &&
            test_right_shift<T, U>() &&
            test_mod<T, U>()
        ;
    }

    template<typename T, typename U>
    bool test_bitwise_or(
        const interval<T> & t_interval,
        const interval<U> & u_interval
    ){
        using namespace boost::numeric;
        std::cout << "test4::test4 bitwise or" << std::endl;
        std::cout
            << boost::core::demangle(typeid(t_interval).name())
            << " t_interval = "
            << t_interval
            << std::endl;
        std::cout
            << boost::core::demangle(typeid(u_interval).name())
            << " u_interval = "
            << u_interval
            << std::endl;
        using R = decltype(T() | U());
        const interval<checked_result<max_t>> r_interval
            = bitwise_or<R>(t_interval, u_interval);
        std::cout
            << boost::core::demangle(typeid(r_interval).name())
            << " r_interval = "
            << r_interval
            << std::endl;
        return true;
    }
    template<typename T, typename U>
    bool test_bitwise_or(){
        return test_bitwise_or<T,U>(interval<T>(), interval<U>());
    }

} // test4

int main(){
    using namespace boost::numeric;
    bool rval = (
        test1() &&
        test2() &&
        test3() &&
        test6() &&
        test7() &&

        test4::test_mod<std::int8_t, std::uint32_t>() &&
        test4::test_add<std::int8_t, std::uint16_t>() &&

        test4::test_bitwise_or<std::int8_t, std::uint16_t>() &&
        test4::test_bitwise_or<std::int8_t, std::int32_t>() &&
        test4::test_bitwise_or<unsigned char, unsigned char>() &&
        test4::test_right_shift<unsigned char, unsigned char>() &&
        test4::test_mod<unsigned char, unsigned char>() &&
        test4::test_left_shift<int, signed char>() &&
        test4::test_left_shift<unsigned char, unsigned char>() &&
        test4::test_left_shift(
            interval<unsigned char>(),
            interval<unsigned char>(8, 8)
        ) &&
        test4::test_right_shift<std::uint8_t, std::uint8_t>() &&
        test4::test_right_shift<std::int8_t, std::int8_t>() &&

        test5<true, std::int8_t, std::int8_t>() &&
        test5<false, std::int8_t, std::int16_t>() &&
        test5<true, std::int16_t, std::int8_t>() &&
        test5<true, std::int16_t, std::int16_t>() &&
        test5<true, std::uint8_t, std::uint8_t>() &&
        test5<false, std::uint8_t, std::uint16_t>() &&
        test5<true, std::uint16_t, std::uint8_t>() &&
        test5<true, std::uint16_t, std::uint16_t>() &&

        test4::divide_result<std::int8_t, std::int8_t>() &&
        test4::divide_result<std::int8_t, std::int16_t>() &&
        test4::divide_result<std::int16_t, std::int8_t>() &&
        test4::divide_result<std::int16_t, std::int16_t>() &&
        test4::divide_result<std::uint8_t, std::uint8_t>() &&
        test4::divide_result<std::uint8_t, std::uint16_t>() &&
        test4::divide_result<std::uint16_t, std::uint8_t>() &&
        test4::divide_result<std::uint16_t, std::uint16_t>() &&

        test4::test<std::int16_t, std::int8_t>() &&
        test4::test<std::int8_t, std::int8_t>() &&
        test4::test<std::int8_t, std::uint8_t>() &&
        test4::test<std::uint8_t, std::int16_t>() &&
        test4::test<std::int32_t, std::uint8_t>() &&
        test4::test<std::int32_t, std::int8_t>() &&
        test4::test<std::int64_t, std::uint8_t>()
    );
    std::cout << (rval ? "success!" : "failure") << std::endl;
    return rval ? EXIT_SUCCESS : EXIT_FAILURE;
}
