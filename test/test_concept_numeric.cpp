//  Copyright (c) 2021 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// test Numeric concept - compile only test

#include <boost/safe_numerics/concept/numeric.hpp>
#include "test_checked_values.hpp"

using namespace boost::mp11;
static_assert(
    mp_all_of<
        signed_test_types,
        boost::safe_numerics::Numeric
    >(),
    "Numeric concept fails on at least one signed integer type"
);
static_assert(
    mp_all_of<
        unsigned_test_types,
        boost::safe_numerics::Numeric
    >(),
    "Numeric concept fails on at least one unsigned integer type"
);

#include <iostream>
#include <boost/safe_numerics/safe_integer.hpp>

int main(){
    boost::safe_numerics::safe<int> s;
    std::cout << s;
    return 0;
}
