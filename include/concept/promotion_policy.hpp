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

#include "../checked_result.hpp"

namespace boost {
namespace numeric {

template<class PP>
struct PromotionPolicy {
    using T = int;
    using U = int;
    using a_type = typename PP::template addition_result<T, U>;
    using s_type = typename PP::template subtraction_result<T, U>;
    using m_type = typename PP::template multiplication_result<T, U>;
    using d_type = typename PP::template division_result<T, U>;
    using mod_type = typename PP::template modulus_result<T, U>;
    using ls_type = typename PP::template left_shift_result<T, U>;
    using rs_type = typename PP::template right_shift_result<T, U>;
    using bw_type = typename PP::template bitwise_result<T, U>;

    checked_result<typename base_type<d_type>::type> divide(){
        return PP::template divide<typename base_type<d_type>::type>(0, 0);
    }
    checked_result<typename base_type<d_type>::type> modulus(){
        return PP::template modulus<typename base_type<d_type>::type>(0, 0);
    }

};

} // numeric
} // boost

#endif // BOOST_NUMERIC_CONCEPT_EXCEPTION_POLICY_HPP
