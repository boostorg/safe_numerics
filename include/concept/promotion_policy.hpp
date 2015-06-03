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

#include "boost/concept/usage.hpp"

namespace boost {
namespace numeric {

template<class PP>
struct PromotionPolicy {
    const char * message;
    BOOST_CONCEPT_USAGE(PromotionPolicy){
        EP::overflow_error(message);
        EP::underflow_error(message);
        EP::range_error(message);
    }
    template<
        typename T,
        typename U,
        typename P
    >
    struct addition_result {
        typedef typename boost::numeric::base_type<T>::type base_type_t;
        typedef typename boost::numeric::base_type<U>::type base_type_u;
        typedef decltype(base_type_t() + base_type_u()) result_base_type;
        typedef safe<result_base_type, P> type;
    };
    template<
        typename T,
        typename U,
        typename P
    >
    struct subtraction_result {
        typedef typename boost::numeric::base_type<T>::type base_type_t;
        typedef typename boost::numeric::base_type<U>::type base_type_u;
        typedef decltype(base_type_t() - base_type_u()) result_base_type;
        typedef safe<result_base_type, P> type;
    };
};

} // numeric
} // boost

#endif // BOOST_NUMERIC_CONCEPT_EXCEPTION_POLICY_HPP
