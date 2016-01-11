#include <cassert>
#include <iostream>
#include <ostream>
#include <cstdint>
#include <cxxabi.h>
#include <algorithm> // max, min

#include <boost/logic/tribool_io.hpp>

#include "../include/checked_result.hpp"
#include "../include/interval.hpp"

bool test1(){
    using namespace boost::numeric;
    std::cout << "test1" << std::endl;
    interval<std::int16_t> x = {-64, 63};
    std::cout << "x = " << x << std::endl;
    interval<std::int16_t> y(-128, 126);
    std::cout << "y = " << y << std::endl;
    assert(static_cast<interval<std::int16_t>>(add<std::int16_t>(x,x)) == y);
    std::cout << "x + x =" << add<std::int16_t>(x, x) << std::endl;
    std::cout << "x - x = " << subtract<std::int16_t>(x, x) << std::endl;
    return true;
}

bool test2(){
    using namespace boost::numeric;
    std::cout << "test2" << std::endl;
    boost::numeric::interval<std::int16_t> x = {-64, 63};
    std::cout << "x = " << x << std::endl;
    std::cout << std::boolalpha << "(x == x) = " << (x == x) << std::endl;
    return true;
}

bool test3(){
    using namespace boost::numeric;
    std::cout << "test3" << std::endl;
    interval<std::int8_t> t;
    std::cout << "t = " << t << std::endl;
    interval<std::uint64_t> u;
    std::cout << "u = " << u << std::endl;
    using max_t = unsigned long long;
    checked_result<interval< max_t>> r = add<max_t>(t, u);
    std::cout << "r = " << r << std::endl;
    return true;
}

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

#include <limits>

namespace test4 {
    using namespace boost::numeric;
    using max_t = std::intmax_t;
    // typedef typename print_type<max_t>::type p_max_t;

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

    template<typename Tx, typename Ux>
    constexpr static const checked_result<interval<max_t>> r(
        const interval<Tx> & t,
        const interval<Ux> & u
    ){
        if(std::numeric_limits<Ux>::is_signed){
            if(u.l > 0 || u.u < 0){
                return divide<max_t>(t, u);
            }
            else{
                checked_result<interval<max_t>> lower = divide<max_t>(t,r_lower(u));
                if(! lower.no_exception())
                    return lower;
                checked_result<interval<max_t>> upper = divide<max_t>(t,r_upper(u));
                if(! upper.no_exception())
                    return upper;
                const interval< max_t> & il = lower;
                const interval< max_t> & iu = upper;
                return
                    interval< max_t>(
                        std::min(il.l, iu.l),
                        std::max(il.u, iu.u)
                    );
            }
        }
        else{ // u is unsigned
            if(u.l > 0)
                return divide<max_t>(t, u);
            else
                return divide<max_t>(t, r_upper(u)) ;
        };
    };

    bool test1(){
        using namespace boost::numeric;
        int status;
        std::cout << "test4::test1 int8_t / int8_t new range ignoring zero" << std::endl;
        const interval<std::int8_t> t;
        std::cout
            << abi::__cxa_demangle(typeid(t).name(),0,0,&status)
            << " t = "
            << t << std::endl;
        const interval<std::int8_t> u;
        std::cout
            << abi::__cxa_demangle(typeid(u).name(),0,0,&status)
            << " u = "
            << u << std::endl;
        const interval<max_t> rx = r(t, u);
        std::cout
            << abi::__cxa_demangle(typeid(rx).name(),0,0,&status)
            << " rx = "
            << rx
            << std::endl;
        return true;
    }
    bool test2(){
        using namespace boost::numeric;
        int status;
        std::cout << "test4::test2 int8_t / int64_t new range ignoring zero" << std::endl;
        const interval<std::int8_t> t;
        std::cout
            << abi::__cxa_demangle(typeid(t).name(),0,0,&status)
            << " t = "
            << t << std::endl;
        const interval<std::int64_t> u;
        std::cout
            << abi::__cxa_demangle(typeid(u).name(),0,0,&status)
            << " u = "
            << u << std::endl;
        const interval<max_t> rx = r(t, u);
        std::cout
            << abi::__cxa_demangle(typeid(rx).name(),0,0,&status)
            << " rx = "
            << rx
            << std::endl;
        return true;
    }
    bool test3(){
        using namespace boost::numeric;
        int status;
        std::cout << "test4::test3  int8_t / int64_t new range ignoring zer0" << std::endl;
        const interval<std::int8_t> t;
        std::cout
            << abi::__cxa_demangle(typeid(t).name(),0,0,&status)
            << " t = "
            << t << std::endl;
        const interval<std::uint64_t> u;
        std::cout
            << abi::__cxa_demangle(typeid(u).name(),0,0,&status)
            << " u = "
            << u << std::endl;
        const checked_result<interval<max_t>> rx = r(t, u);
        std::cout
            << abi::__cxa_demangle(typeid(rx).name(),0,0,&status)
            << " rx = "
            << rx
            << std::endl;
        return true;
    }
    template<typename T, typename U>
    bool test4(){
        using namespace boost::numeric;
        int status;
        std::cout << "test4::test4 multiply" << std::endl;
        const interval<T> t_interval = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };
        std::cout
            << abi::__cxa_demangle(typeid(t_interval).name(),0,0,&status)
            << " t_interval = "
            << t_interval
            << std::endl;
        const interval<U> u_interval = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };
        std::cout
            << abi::__cxa_demangle(typeid(u_interval).name(),0,0,&status)
            << " u_interval = "
            << u_interval
            << std::endl;
        using R = decltype(U() * T());
        const checked_result<interval<R> > r_interval
            = multiply<R>(t_interval, u_interval);
        std::cout
            << abi::__cxa_demangle(typeid(r_interval).name(),0,0,&status)
            << " r_interval = "
            << r_interval
            << std::endl;
        return true;
    }
    template<typename T, typename U>
    bool test5(){
        using namespace boost::numeric;
        int status;
        std::cout << "test4::test4 divide_nz" << std::endl;
        const interval<T> t_interval = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };
        std::cout
            << abi::__cxa_demangle(typeid(t_interval).name(),0,0,&status)
            << " t_interval = "
            << t_interval
            << std::endl;
        const interval<U> u_interval = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };
        std::cout
            << abi::__cxa_demangle(typeid(u_interval).name(),0,0,&status)
            << " u_interval = "
            << u_interval
            << std::endl;
        using R = decltype(U() * T());
        const checked_result<interval<R> > r_interval
            = divide_nz<R>(t_interval, u_interval);
        std::cout
            << abi::__cxa_demangle(typeid(r_interval).name(),0,0,&status)
            << " r_interval = "
            << r_interval
            << std::endl;
        return true;
    }
    template<typename T, typename U>
    bool test6(){
        using namespace boost::numeric;
        int status;
        std::cout << "test4::test4 subtract" << std::endl;
        const interval<T> t_interval = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };
        std::cout
            << abi::__cxa_demangle(typeid(t_interval).name(),0,0,&status)
            << " t_interval = "
            << t_interval
            << std::endl;
        const interval<U> u_interval = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };
        std::cout
            << abi::__cxa_demangle(typeid(u_interval).name(),0,0,&status)
            << " u_interval = "
            << u_interval
            << std::endl;
        using R = decltype(U() * T());
        const checked_result<interval<R> > r_interval
            = subtract<R>(t_interval, u_interval);
        std::cout
            << abi::__cxa_demangle(typeid(r_interval).name(),0,0,&status)
            << " r_interval = "
            << r_interval
            << std::endl;
        return true;
    }
    template<typename T, typename U>
    bool test7(){
        using namespace boost::numeric;
        int status;
        std::cout << "test4::test4 left shift" << std::endl;
        const interval<T> t_interval = {
            base_value(std::numeric_limits<T>::min()),
            base_value(std::numeric_limits<T>::max())
        };
        std::cout
            << abi::__cxa_demangle(typeid(t_interval).name(),0,0,&status)
            << " t_interval = "
            << t_interval
            << std::endl;
        const interval<U> u_interval = {
            base_value(std::numeric_limits<U>::min()),
            base_value(std::numeric_limits<U>::max())
        };
        std::cout
            << abi::__cxa_demangle(typeid(u_interval).name(),0,0,&status)
            << " u_interval = "
            << u_interval
            << std::endl;
        using R = T;
        const checked_result<interval<R> > r_interval
            = left_shift<R>(t_interval, u_interval);
        std::cout
            << abi::__cxa_demangle(typeid(r_interval).name(),0,0,&status)
            << " r_interval = "
            << r_interval
            << std::endl;
        return true;
    }
    template<typename T, typename U>
    bool test(){
        return
            test4<T, U>() &&
            test5<T, U>() &&
            test6<T, U>() &&
            test7<T, U>()
        ;
    }
} // test4

int main(){
    bool rval = (
        test1() &&
        test2() &&
        test3() &&
        test5<true, std::int8_t, std::int8_t>() &&
        test5<false, std::int8_t, std::int16_t>() &&
        test5<true, std::int16_t, std::int8_t>() &&
        test5<true, std::int16_t, std::int16_t>() &&
        test5<true, std::uint8_t, std::uint8_t>() &&
        test5<false, std::uint8_t, std::uint16_t>() &&
        test5<true, std::uint16_t, std::uint8_t>() &&
        test5<true, std::uint16_t, std::uint16_t>() &&
        test4::test1() &&
        test4::test2() &&
        test4::test3() &&
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