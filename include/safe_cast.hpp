#ifndef BOOST_NUMERIC_SAFE_CAST_HPP
#define BOOST_NUMERIC_SAFE_CAST_HPP

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
#include <boost/mpl/min_max.hpp>
#include <boost/mpl/plus.hpp>
#include "numeric.hpp"
#include "overflow.hpp"

namespace boost {
namespace numeric {
namespace detail {
    // simple case when signess are the same.
    template<bool TS, bool US>
    struct safe_cast {
        template<class T, class U>
        inline static T invoke(const U & u){
            if(u > std::numeric_limits<T>::max())
                overflow("safe range overflow");
            if(u < std::numeric_limits<T>::min())
                overflow("safe range underflow");
            return static_cast<T>(u);
        }
    };

    // T signed <- U unsigned
    template<>
    struct safe_cast<true, false> {
        template<class T>
        struct sbits : public
            boost::mpl::min<
                typename boost::mpl::integral_c<
                    int, 
                    std::numeric_limits<boost::intmax_t>::digits
                >,
                typename boost::mpl::plus<
                    typename boost::mpl::integral_c<
                        int, 
                        std::numeric_limits<T>::digits
                    >,
                    typename boost::mpl::integral_c<int, 1>
                >
            >::type
        {};
        template<class T, class U>
        inline static T invoke(const U & u){
            // figure # of bits in U
            // if that # == maximum supported
            if(bits<U>::value == bits<boost::uintmax_t>::value)
                // just cast to signed and
                // choke if it's negative
                if(static_cast<boost::intmax_t>(u) < 0)
                    overflow("safe range overflow");

            typedef typename boost::int_t<
                sbits<U>::value
            >::fast signed_u_type;

            return safe_cast<true, true>::invoke<T>(
                static_cast<signed_u_type>(u)
            );
        }
    };

    // T unsigned <- U signed
    template<>
    struct safe_cast<false, true> {
        template<class T, class U>
        inline static T invoke(const U & u){
            if(u < 0)
                overflow("safe range underflow");

            typedef typename boost::uint_t<
                boost::mpl::integral_c<
                    int, 
                    std::numeric_limits<U>::digits
                >::value
            >::fast unsigned_u_type;

            return safe_cast<false, false>::invoke<T>(
                static_cast<const unsigned_u_type &>(u)
            );
        }
    };

} // detail

template<class T, class U>
T safe_cast(const U & u) {
    T t = detail::safe_cast<
        boost::numeric::is_signed<T>::value,
        boost::numeric::is_signed<U>::value
    >::template invoke<T>(u);
    return t;
}

} // numeric
} // boost

#endif // BOOST_NUMERIC_SAFE_CAST_HPP
