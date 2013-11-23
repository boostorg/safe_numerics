#ifndef BOOST_NUMERIC_CONCEPT_NUMERIC_HPP
#define BOOST_NUMERIC_CONCEPT_NUMERIC_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/limits.hpp>
#include <boost/static_assert.hpp>

template<class T>
struct Numeric {
    // if your program traps here, you need to create a
    // std::numeric_limits class for your type T.  see
    // see C++ standard 18.3.2.2
    BOOST_STATIC_ASSERT(std::numeric_limits<T>::is_specialized == true);
    BOOST_STATIC_ASSERT(std::numeric_limits<T>::is_bounded == true);
};

template <class T>
class Integer : public Numeric<T> {
    // integer types must have the corresponding numeric trait.
    BOOST_STATIC_ASSERT(std::numeric_limits<T>::is_integer == true);
};

#endif // BOOST_NUMERIC_CONCEPT_NUMERIC_HPP
