#ifndef BOOST_NUMERIC_SAFE_COMPARE_HPP
#define BOOST_NUMERIC_SAFE_COMPARE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <type_traits>
#include <limits>

#include <boost/mpl/if.hpp>

namespace boost {
namespace numeric {
namespace safe_compare {

////////////////////////////////////////////////////
// safe comparison on primitive integral types
namespace safe_compare_detail {
    template<typename T>
    using make_unsigned = typename boost::mpl::if_c<
        std::is_signed<T>::value,
        std::make_unsigned<T>,
        T
    >::type;

    // both arguments unsigned or signed
    template<bool TS, bool US>
    struct less_than {
        template<class T, class U>
        constexpr static bool invoke(const T & t, const U & u){
            return t < u;
        }
    };

    // T unsigned, U signed
    template<>
    struct less_than<false, true> {
        template<class T, class U>
        constexpr static bool invoke(const T & t, const U & u){
            return
                (u < 0) ?
                    false
                :
                    less_than<false, false>::invoke(
                        t,
                        static_cast<const typename make_unsigned<U>::type &>(u)
                    )
                ;
        }
    };
    // T signed, U unsigned
    template<>
    struct less_than<true, false> {
        template<class T, class U>
        constexpr static bool invoke(const T & t, const U & u){
            return
                (t < 0) ?
                    true
                :
                    less_than<false, false>::invoke(
                        static_cast<const typename make_unsigned<T>::type &>(t),
                        u
                    )
                ;
        }
    };
} // safe_compare_detail

template<class T, class U>
constexpr bool less_than(const T & lhs, const U & rhs) {
    return safe_compare_detail::less_than<
        std::is_signed<T>::value,
        std::is_signed<U>::value
    >::template invoke(lhs, rhs);
}

template<class T, class U>
constexpr bool greater_than(const T & lhs, const U & rhs) {
    return less_than(rhs, lhs);
}

template<class T, class U>
constexpr bool less_than_equal(const T & lhs, const U & rhs) {
    return ! greater_than(lhs, rhs);
}

template<class T, class U>
constexpr bool greater_than_equal(const T & lhs, const U & rhs) {
    return ! less_than(lhs, rhs);
}

namespace safe_compare_detail {
    // both arguments unsigned or signed
    template<bool TS, bool US>
    struct equal {
        template<class T, class U>
        constexpr static bool invoke(const T & t, const U & u){
            return t == u;
        }
    };

    // T unsigned, U signed
    template<>
    struct equal<false, true> {
        template<class T, class U>
        constexpr static bool invoke(const T & t, const U & u){
            return
                (u < 0) ?
                    false
                :
                    equal<false, false>::invoke(
                        t,
                        static_cast<const typename make_unsigned<U>::type &>(u)
                    )
                ;
        }
    };
    // T signed, U unsigned
    template<>
    struct equal<true, false> {
        template<class T, class U>
        constexpr static bool invoke(const T & t, const U & u){
            return
                (t < 0) ?
                    false
                :
                    equal<false, false>::invoke(
                        static_cast<const typename make_unsigned<T>::type &>(t),
                        u
                    )
                ;
        }
    };
} // safe_compare_detail

template<class T, class U>
constexpr bool equal(const T & lhs, const U & rhs) {
    return safe_compare_detail::equal<
        std::numeric_limits<T>::is_signed,
        std::numeric_limits<U>::is_signed
    >::template invoke(lhs, rhs);
}

template<class T, class U>
constexpr bool not_equal(const T & lhs, const U & rhs) {
    return ! equal(lhs, rhs);
}

} // safe_compare
} // numeric
} // boost

#endif // BOOST_NUMERIC_SAFE_COMPARE_HPP
