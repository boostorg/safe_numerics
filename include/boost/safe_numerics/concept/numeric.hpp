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

template<class T>
using Numeric = std::integral_constant<bool, std::numeric_limits<T>::is_specialized>;

} // safe_numerics
} // boost


static_assert(boost::safe_numerics::Numeric<std::int16_t>::value, "asdfasdfasdf");
#endif // BOOST_NUMERIC_CONCEPT_NUMERIC_HPP
