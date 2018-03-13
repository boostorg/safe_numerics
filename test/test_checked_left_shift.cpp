//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <exception>
#include <cstdlib>   // EXIT_SUCCESS
#include <iostream>
#include <cassert>
#include <typeinfo>
#include <boost/core/demangle.hpp>

#include "../include/checked_result.hpp"
#include "../include/checked_result_operations.hpp"
#include "../include/checked_integer.hpp"

template<class T>
bool test_checked_left_shift(
    boost::numeric::checked_result<T> v1,
    boost::numeric::checked_result<T> v2,
    char expected_result
){
    using namespace boost::numeric;

    std::cout << "testing " << v1 << " >> " << v2 << std::flush;
    const checked_result<T> result = v1 << v2;
    std::cout << " -> " << result << std::endl;

    switch(expected_result){
    case '.':
        if(result.exception()){
            std::cout
                << "erroneously detected error in left shift "
                << std::endl;
            v1 << v2;
            return false;
        }
        return true;
    case '-':
        if(safe_numerics_error::negative_overflow_error == result.m_e)
            return true;
        break;
    case '+':
        if(safe_numerics_error::positive_overflow_error == result.m_e)
            return true;
        break;
    case '!':
        if(result.exception())
            return true;
        break;
    case 'n':   // n negative_shift
        if(safe_numerics_error::negative_shift == result.m_e)
            return true;
        break;
    case 's':   // s negative_value_shift
        if(safe_numerics_error::negative_value_shift == result.m_e)
            return true;
        break;
    case 'l':   // l shift_too_large
        if(safe_numerics_error::shift_too_large == result.m_e)
            return true;
        break;
    default:
        assert(false);
    }
    std::cout
        << "failed to detect error in left shift "
        << std::hex << result << "(" << std::dec << result << ")"
        << " != "<< v1 << " << " << v2
        << std::endl;
    v1 << v2;
    return false;
}

// values
template<typename T>
const boost::numeric::checked_result<T> signed_value[] = {
    boost::numeric::safe_numerics_error::range_error,
    boost::numeric::safe_numerics_error::domain_error,
    boost::numeric::safe_numerics_error::positive_overflow_error,
    std::numeric_limits<T>::max(),
    1,
    0,
    -1,
    std::numeric_limits<T>::lowest(),
    boost::numeric::safe_numerics_error::negative_overflow_error,
};

template<typename T>
const boost::numeric::checked_result<T> unsigned_value[] = {
    boost::numeric::safe_numerics_error::range_error,
    boost::numeric::safe_numerics_error::domain_error,
    boost::numeric::safe_numerics_error::positive_overflow_error,
    std::numeric_limits<T>::max(),
    1,
    0,
    boost::numeric::safe_numerics_error::negative_overflow_error,
};

// test result matrices

// key
// . success
// - negative_overflow_error
// + positive_overflow_error
// ! range_error
// n negative_shift,             // negative value in shift operator
// s negative_value_shift,       // shift a negative value
// l shift_too_large,            // l/r shift exceeds variable size

constexpr char const * const signed_left_shift_results[] = {
//      012345678
/* 0*/ "!!!!!!!!!",
/* 1*/ "!!!!!!!!!",
/* 2*/ "!!++++++!",
/* 3*/ "!!+!+....",
/* 4*/ "!!++.....",
/* 5*/ ".........",
/* 6*/ "!!--.....",
/* 7*/ "!!-!!....",
/* 8*/ "!!------!",
};

constexpr char const * const unsigned_left_shift_results[] = {
//      0123456
/* 0*/ "!!!!!!!",
/* 1*/ "!!!!!!!",
/* 2*/ "!!++++!",
/* 3*/ "!!+++..",
/* 4*/ "!!++...",
/* 5*/ ".......",
/* 6*/ "!!----!",
};

// given an array of values of particula
// test all value pairs of a given collection
template<typename T, unsigned int N>
bool test_pairs(const T (&value)[N], const char * const (&results)[N]) {
    using namespace boost::numeric;
    bool result = true;
    // for each pair of values p1, p2 (100)
    for(unsigned int i = 0; i < N; i++)
    for(unsigned int j = 0; j < N; j++){
        std::cout << std::dec << i << ',' << j << ',';
        if(! test_checked_left_shift(value[i], value[j], results[i][j]))
            result = false;
    }
    return result;
}

#include <boost/mp11/algorithm.hpp>

struct t {
    static bool m_error;
    template<typename T>
    void operator()(const T &){
        std::cout
            << "** testing "
            << boost::core::demangle(typeid(T).name())
            << std::endl;
        m_error &=
            std::numeric_limits<T>::is_signed
            ? test_pairs(signed_value<T>, signed_left_shift_results)
            : test_pairs(unsigned_value<T>, unsigned_left_shift_results)
        ;
    }
};
bool t::m_error = true;

bool test_all_types(){
    t rval;
    boost::mp11::mp_for_each<
        boost::mp11::mp_list<
            std::int8_t, std::int16_t, std::int32_t, std::int64_t,
            std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t
        >
    >(rval);
    return rval.m_error;
}

int main(int , char *[]){
    // sanity check on test matrix - should be symetrical
    bool rval = test_all_types();
    std::cout << (rval ? "success!" : "failure") << std::endl;
    return ! rval ;
}
