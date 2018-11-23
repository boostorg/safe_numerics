//
//  test_values.hpp
//
//  Created by Robert Ramey on 3/27/14.
//
//

#ifndef BOOST_SAFE_NUMERICS_TEST_VALUES_HPP
#define BOOST_SAFE_NUMERICS_TEST_VALUES_HPP

#include <cstdint>
#include <type_traits> // integral_constant
#include <boost/mp11/list.hpp> // mp11_list

#define TEST_VALUE(type, value) std::integral_constant<type,(type)value>

using test_values = boost::mp11::mp_list<
    TEST_VALUE(std::int8_t,0x01),
    TEST_VALUE(std::int8_t,0x7f),
    TEST_VALUE(std::int8_t,0x80),
    TEST_VALUE(std::int8_t,0xff),
    TEST_VALUE(std::int16_t,0x0001),
    TEST_VALUE(std::int16_t,0x7fff),
    TEST_VALUE(std::int16_t,0x8000),
    TEST_VALUE(std::int16_t,0xffff),
    TEST_VALUE(std::int32_t,0x00000001),
    TEST_VALUE(std::int32_t,0x7fffffff),
    TEST_VALUE(std::int32_t,0x80000000),
    TEST_VALUE(std::int32_t,0xffffffff),
    TEST_VALUE(std::int64_t,0x0000000000000001),
    TEST_VALUE(std::int64_t,0x7fffffffffffffff),
    TEST_VALUE(std::int64_t,0x8000000000000000),
    TEST_VALUE(std::int64_t,0xffffffffffffffff),
    TEST_VALUE(std::uint8_t,0x01),
    TEST_VALUE(std::uint8_t,0x7f),
    TEST_VALUE(std::uint8_t,0x80),
    TEST_VALUE(std::uint8_t,0xff),
    TEST_VALUE(std::uint16_t,0x0001),
    TEST_VALUE(std::uint16_t,0x7fff),
    TEST_VALUE(std::uint16_t,0x8000),
    TEST_VALUE(std::uint16_t,0xffff),
    TEST_VALUE(std::uint32_t,0x00000001),
    TEST_VALUE(std::uint32_t,0x7fffffff),
    TEST_VALUE(std::uint32_t,0x80000000),
    TEST_VALUE(std::uint32_t,0xffffffff),
    TEST_VALUE(std::uint64_t,0x0000000000000001),
    TEST_VALUE(std::uint64_t,0x7fffffffffffffff),
    TEST_VALUE(std::uint64_t,0x8000000000000000),
    TEST_VALUE(std::uint64_t,0xffffffffffffffff),
    TEST_VALUE(std::int8_t,0x0)
>;

#endif
