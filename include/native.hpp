#ifndef BOOST_NUMERIC_NATIVE_HPP
#define BOOST_NUMERIC_NATIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// we could have used decltype and auto for C++11 but we've decided
// to use boost/typeof to be compatible with older compilers
#include <boost/typeof/typeof.hpp>

// policy which creates results types equal to that of C++ promotions.
// Using the policy will permit the program to build and run in release
// mode which is identical to that in debug mode except for the fact
// that errors aren't trapped. 

namespace boost {
namespace numeric {
namespace promotion {

struct native {
    template<
        typename T,
        typename U
    >
    struct addition_result {
        typedef decltype(
            typename boost::numeric::base_type<T>::type()
          + typename boost::numeric::base_type<U>::type()
        ) type;
    };
};

} // promotion
} // numeric
} // boost

#endif // BOOST_NUMERIC_NATIVE_HPP
