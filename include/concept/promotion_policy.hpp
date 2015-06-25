#ifndef BOOST_NUMERIC_CONCEPT_PROMOTION_POLICY_HPP
#define BOOST_NUMERIC_CONCEPT_PROMOTION_POLICY_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2015 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//#include "boost/concept/usage.hpp"

namespace boost {
namespace numeric {

template<class PP>
struct PromotionPolicy {
    typedef int T;
    typedef int U;
    typedef void E;
    typedef typename PP::template addition_result<T, U, PP, E> ar_type;
    typedef typename PP::template subtraction_result<T, U, PP, E> s_type;
};

} // numeric
} // boost

#endif // BOOST_NUMERIC_CONCEPT_EXCEPTION_POLICY_HPP
