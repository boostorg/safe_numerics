#include <iostream>
#include <ostream>
#include <cassert>
#include <limits>

#include <boost/mp11/algorithm.hpp>

#include "../include/checked_result.hpp"
#include "../include/checked_result_operations.hpp"

#include "test.hpp"
#include <cstdint>

template<typename T>
using fptr = T (*)(const T &, const T &);


template<typename OP, typename T, unsigned int N>
const boost::numeric::checked_result<T> result[N][N];

// a single test for a specific operator
// for a apecific pair of values
template<typename OP, typename T>
bool test(const OP & op, const T & t1, const T & t2, const T & result){
    return result == op(t1, t2);
}

// given an array of values of particula
template<
    typename OP,
    typename T,
    unsigned int N
>
bool test_pairs(const OP & op, const T (&value)[N]) {
    using namespace boost::numeric;
    // for each pair of values p1, p2 (100)
    for(unsigned int i = 0; i < N; i++)
    for(unsigned int j = 0; j < N; j++){
        if(! test(op, value[i], value[j], result<T>[i][j]))
            return false;
    }
    return true;
}

// test all value pairs of a given collection#if 0
template<typename OP, typename T>
bool test_type(const OP & op){
    return test_pairs<OP, T>(
        op,
        std::numeric_limits<T>::is_signed)
        ? signed_value<T>
        : unsigned_value<T>
    ;
}

template<typename OP>
bool test_op(const OP & op){
    boost::mp11::mp_for_each<
        boost::mp11::mp_list<
            std::int8_t, std::int16_t, std::int32_t, std::int64_t,
            std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t
        >
    >(test_type::operator(op, );
}

// values
template<typename T, unsigned int N>
const boost::numeric::checked_result<T> signed_value[] = {
    boost::numeric::safe_numerics_error::negative_overflow_error,
    std::numeric_limits<T>::lowest(),
    -1,
    0,
    1,
    std::numeric_limits<T>::max(),
    boost::numeric::safe_numerics_error::positive_overflow_error,
    boost::numeric::safe_numerics_error::domain_error,
    boost::numeric::safe_numerics_error::range_error
};

template<typename T, unsigned int N>
const boost::numeric::checked_result<T> unsigned_value[] = {
    boost::numeric::safe_numerics_error::negative_overflow_error,
    0,
    1,
    std::numeric_limits<T>::max(),
    boost::numeric::safe_numerics_error::positive_overflow_error,
    boost::numeric::safe_numerics_error::domain_error,
    boost::numeric::safe_numerics_error::range_error
};

struct test {
    unsigned int m_error_count;
    test() :
        m_error_count(0)
    {}
    template<typename T>
    bool operator()(const T &){
        using namespace boost::numeric;
        std::cout
            << "** testing "
            << boost::core::demangle(typeid(T).name())
            << std::endl;

        using R = checked_result<T>;
        // pointers to operands for types T
        static const std::array<op<R>, 6> op_table{{
            {operator+, operator+, "+", false},
            {operator-, operator-, "-", false},
            {operator*, operator*, "*", false},
            {operator/, operator/, "/", true},
            {operator%, operator%, "%", true},
            {operator<<, operator<<, "<<", true},
            {operator>>, operator>>, ">>", true},
        }};

        //for(unsigned int i = 0; i < sizeof(op_table)/sizeof(op) / sizeof(fptr<R>); ++i){
        for(const op<R> & o : op_table){
            if(! test_type_operator(value<T>, o)){
                ++m_error_count;
                return false;
            }
        }
        /*
        test_type_operator(
            value<T>,
            operator/,
            operator/,
            true
        );
        test_type_operator(
            value<T>,
            operator%,
            operator%,
            true
        );
        */
        return true;
    }
};

#include <boost/mp11/list.hpp>
#include <boost/mp11/algorithm.hpp>

int main(int, char *[]){
    using namespace boost::mp11;
    // list of signed types
    using signed_types = mp_list<std::int8_t, std::int16_t, std::int32_t, std::int64_t>;
    // list of unsigned types
    using unsigned_types = mp_list<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;

    test t;
    mp_for_each<unsigned_types>(t);
    mp_for_each<signed_types>(t);

    std::cout << (t.m_error_count == 0 ? "success!" : "failure") << std::endl;
    return t.m_error_count ;
}

