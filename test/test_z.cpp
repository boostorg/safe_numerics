
#if 0
auto val()
{
  return -0xFFFFFFFF;
}

#include <stdexcept>
#include <iostream>
#include "../include/safe_integer.hpp"
#include "../include/safe_integer_literal.hpp"

void val0(){
    const boost::numeric::safe<unsigned int> x{0};
    std::cout << x << std::endl;
    std::cout << -x << std::endl;
    auto y = -x;
    std::cout << y << std::endl;
}

constexpr boost::numeric::safe<unsigned int> val1()
{
    constexpr boost::numeric::safe<unsigned int> x = 0xFFFFFFFF;
    return -x;
}
constexpr boost::numeric::safe<unsigned int> val2()
{
    boost::numeric::safe<unsigned int> x = - boost::numeric::safe_unsigned_literal<0xFFFFFFFF>();
    return x;
}

constexpr boost::numeric::safe<unsigned int> val3()
{
    return - boost::numeric::safe_unsigned_literal<0xFFFFFFFF>();
}

int main(){
    val0();
    std::cout << val1() << std::endl;
    std::cout << val2() << std::endl;
    std::cout << val3() << std::endl;
    return 0;
}

// test utility
#include "../include/utility.hpp"

int main(){
    using namespace boost::numeric;
    using x = unsigned_stored_type<0, 42>;
    print_type<x> p1;

    return 0;
}

// test automatic type promotion
#include "../include/automatic.hpp"
#include "../include/safe_integer.hpp"
#include <type_traits>
#include <cstdint>
#include <iostream>

int main(){
    using namespace boost::numeric;
    using ar = automatic::addition_result<std::uint8_t, std::uint8_t>;
    static_assert(
        std::is_same<ar::type, std::uint16_t>::value,
        "sum of two 8 bit unsigned integers should fit in on 16 bit unsigned integer"
    );
    return 0;
}


// test automatic type promotion
#include "../include/safe_integer.hpp"
#include "../include/safe_integer_range.hpp"
#include "../include/safe_integer_literal.hpp"
#include "../include/automatic.hpp"
#include <type_traits>
#include <cstdint>
#include <iostream>

int main(){
    using namespace boost::numeric;
    unsigned char t1 = 1;
    constexpr const safe_unsigned_literal<42, automatic, default_exception_policy> v2;
    using result_type = decltype(t1 + v2);

    static_assert(
        std::is_same<
            result_type,
            safe_unsigned_range<42, 297, automatic, default_exception_policy>
        >::value,
        "result type should have a range 42-297"
    );
    return 0;
}
void f1(){
    using namespace boost::numeric;
    constexpr safe<int> j = 0;
    constexpr safe<int> k = 3;
    constexpr safe<int> l = j + k; // compile error
}

void f2(){
    using namespace boost::numeric;
    constexpr safe<int> j = boost::numeric::safe_signed_literal<0>();
    constexpr safe<int> k = boost::numeric::safe_signed_literal<3>();
    constexpr safe<int> l = j + k; // compile error
}

void f3(){
    using namespace boost::numeric;
    constexpr auto j = safe_signed_literal<0, native, loose_trap_policy>();
    constexpr auto k = safe_signed_literal<3>();
    constexpr const safe<int> l = j + k;
}

void f4(){
    using namespace boost::numeric;
    safe_signed_literal<0, native, loose_trap_policy> j;
    safe_signed_literal<3> k;
    constexpr auto l = safe_signed_literal<3>();
    constexpr const safe<int> l2 = j + k;
}

#include "../include/interval.hpp"

int main(){
    return 0;
}





#include "../include/utility.hpp"
#include "../include/cpp.hpp"
#include "../include/safe_common.hpp"

using pic16_promotion = boost::numeric::cpp<
    8,  // char
    8,  // short
    8,  // int
    16, // long
    32  // long long
>;

#include <type_traits>
#include "../include/safe_integer.hpp"
#include "../include/range_value.hpp"
#include <iostream>

int main(){
    using namespace boost::numeric;
    static_assert(
        std::is_literal_type<safe<int>>::value,
        "safe type is a literal type"
    );
    static_assert(
        std::is_literal_type<interval<int>>::value,
        "interval type is a literal type"
    );
    static_assert(
        std::is_literal_type<interval<
            safe<int>
        >>::value,
        "interval of safe types is a literal type"
    );
    static_assert(
        std::is_literal_type<range_value<
            safe<int>
        >>::value,
        "range_value of safe types is a literal type"
    );
    safe<int> x = 42;
    std::cout << make_range_value(x);
    return 0;
}

auto val()
{
  return -0xFFFFFFFF;
}

#include <stdexcept>
#include <iostream>
#include "../include/safe_integer.hpp"
#include "../include/safe_integer_literal.hpp"

void val0(){
    const boost::numeric::safe<unsigned int> x{0};
    std::cout << x << std::endl;
    std::cout << -x << std::endl;
    auto y = -x;
    std::cout << y << std::endl;
}

constexpr boost::numeric::safe<unsigned int> val1(){
    constexpr boost::numeric::safe<unsigned int> x = 0xFFFFFFFF;
    return -x;
}
constexpr boost::numeric::safe<unsigned int> val2(){
    const boost::numeric::safe<unsigned int> x
        = -boost::numeric::safe_unsigned_literal<0xFFFFFFFF>();
    return x;
}
constexpr boost::numeric::safe<unsigned int> val3(){
    return - boost::numeric::safe_unsigned_literal<0xFFFFFFFF>();
}

int main(){
    val0();
    std::cout << val1() << std::endl;
    std::cout << val2() << std::endl;
    std::cout << val3() << std::endl;
    return 0;
}

#endif

#include <interval.hpp>

int main(){
    return 0;
}

