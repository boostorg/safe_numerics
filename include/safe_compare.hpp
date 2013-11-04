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

#include <boost/limits.hpp>
#include <boost/integer.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/make_unsigned.hpp>

#include "numeric.hpp"

namespace boost {
namespace numeric {
namespace safe_compare {
    namespace detail {
        // note make_unsigned needs more research.  We use the one
        // from boost type_traits.  BUT boost type_traits only handles
        // C primitives.  What we really want is something which works
        // for any type T such that std::numeric_limits<T>::is_signed is true.
        template<typename T>
        struct make_unsigned {
            typedef typename boost::mpl::if_<
                boost::is_integral<T>,
                typename boost::make_unsigned<T>,
                typename boost::mpl::identity<T>
            >::type::type type;
        };
        // both arguments unsigned or signed
        template<bool TS, bool US>
        struct less_than {
            template<class T, class U>
            static bool invoke(const T & t, const U & u){
                return t < u;
            }
        };

        // T unsigned, U signed
        template<>
        struct less_than<false, true> {
            template<class T, class U>
            static bool invoke(const T & t, const U & u){
                if(u < 0)
                    return false;
                return less_than<false, false>::invoke(
                    t, 
                    static_cast<const typename make_unsigned<U>::type &>(u)
                );
            }
        };
        // T signed, U unsigned
        template<>
        struct less_than<true, false> {
            template<class T, class U>
            static bool invoke(const T & t, const U & u){
                if(t < 0)
                    return true;
                return less_than<false, false>::invoke(
                    static_cast<const typename make_unsigned<T>::type &>(t),
                    u
                );
            }
        };
    } // detail

    template<class T, class U>
    bool less_than(const T & lhs, const U & rhs) {
        return detail::less_than<
            boost::numeric::is_signed<T>::value,
            boost::numeric::is_signed<U>::value
        >::template invoke(lhs, rhs);
    }

    template<class T, class U>
    bool greater_than_equal(const T & lhs, const U & rhs) {
        return less_than(rhs, lhs);
    }

    namespace detail { 
        // both arguments unsigned or signed
        template<bool TS, bool US>
        struct greater_than {
            template<class T, class U>
            static bool invoke(const T & t, const U & u){
                return t > u;
            }
        };

        // T unsigned, U signed
        template<>
        struct greater_than<false, true> {
            template<class T, class U>
            static bool invoke(const T & t, const U & u){
                if(u < 0)
                    return true;
                return greater_than<false, false>::invoke(
                    t, 
                    static_cast<const typename make_unsigned<U>::type &>(u)
                );
            }
        };
        // T signed, U unsigned
        template<>
        struct greater_than<true, false> {
            template<class T, class U>
            static bool invoke(const T & t, const U & u){
                if(t < 0)
                    return false;
                return greater_than<false, false>::invoke(
                    static_cast<const typename make_unsigned<T>::type &>(t),
                    u
                );
            }
        };
    } // detail

    template<class T, class U>
    bool greater_than(const T & lhs, const U & rhs) {
        return detail::greater_than<
            boost::numeric::is_signed<T>::value,
            boost::numeric::is_signed<U>::value
        >::template invoke(lhs, rhs);
    }

    template<class T, class U>
    bool less_than_equal(const T & lhs, const U & rhs) {
        return greater_than(rhs, lhs);
    }

    template<class T, class U>
    bool equal(const T & lhs, const U & rhs) {
        return ! less_than(lhs, rhs) && ! greater_than(lhs, rhs);
    }

    template<class T, class U>
    bool not_equal(const T & lhs, const U & rhs) {
        return ! equal(lhs, rhs);
    }

} // safe_compare
} // numeric
} // boost

#endif // BOOST_NUMERIC_SAFE_COMPARE_HPP
