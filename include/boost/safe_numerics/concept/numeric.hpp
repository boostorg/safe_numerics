#ifndef BOOST_NUMERIC_CONCEPT_NUMERIC_HPP
#define BOOST_NUMERIC_CONCEPT_NUMERIC_HPP

//  Copyright (c) 2021 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <limits>
#include <cstdint>
#include <boost/type_traits/is_detected_exact.hpp> // is_detected_exact
#include <boost/mp11/function.hpp> // mp_and
#include <boost/safe_numerics/utility.hpp> // print_types


namespace boost {
namespace safe_numerics {

// Note: the following global operators will be found via
// argument dependent lookup.
namespace {

using namespace boost::mp11;

template<class T>
using lowest_t = decltype(std::declval<std::numeric_limits<T>>().lowest());
template<class T>
using min_t = decltype(std::declval<std::numeric_limits<T>>().min());
template<class T>
using max_t = decltype(std::declval<std::numeric_limits<T>>().max());
template<class T>
using digits_t = decltype(std::declval<std::numeric_limits<T>>().digits);
template<class T>
using is_signed_t = decltype(std::declval<std::numeric_limits<T>>().is_signed);

} // anon

template<class T>
using Numeric = boost::mp11::mp_and<
    boost::is_detected_exact<T, lowest_t, T>,
    boost::is_detected_exact<T, min_t, T>,
    boost::is_detected_exact<T, max_t, T>,
    boost::is_detected_exact<const int, digits_t, T>,
    boost::is_detected_exact<const bool, is_signed_t, T>
>;

} // safe_numerics
} // boost

boost::safe_numerics::utility::print_types<
    boost::is_detected_exact<
        std::int16_t,
        boost::safe_numerics::lowest_t,
        std::int16_t
    >,
    boost::safe_numerics::lowest_t<std::int16_t>,
    boost::safe_numerics::lowest_t<std::int16_t>(),
    boost::safe_numerics::Numeric<std::int16_t>
> x;

boost::safe_numerics::utility::print_types<
    boost::is_detected_exact<
        std::int16_t,
        boost::safe_numerics::digits_t,
        std::int16_t
    >,
    boost::safe_numerics::digits_t<std::int16_t>,
    boost::safe_numerics::digits_t<std::int16_t>(),
    boost::safe_numerics::Numeric<std::int16_t>
> x1;

static_assert(boost::safe_numerics::Numeric<std::int16_t>::value, "asdfasdfasdf");
#endif // BOOST_NUMERIC_CONCEPT_NUMERIC_HPP
