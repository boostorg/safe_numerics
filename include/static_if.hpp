#ifndef BOOST_STATIC_IF_HPP
#define BOOST_STATIC_IF_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2012 Robert Ramey
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// due To Paul Fultz II

namespace boost {
namespace utility {
    namespace detail {
    struct identity
    { 
        template<class T>
        T operator()(T&& x) const 
        { 
            return std::forward<T>(x); 
        } 
    }; 

    template< bool Condition > 
    struct static_if_statement 
    { 
        template< typename F > constexpr void then ( F const& f ) { f(identity()); }
        template< typename F > constexpr void else_ ( F const& ) { }
    }; 

    template< > 
    struct static_if_statement<false>  
    { 
        template< typename F > constexpr void then ( F const& ) { }
        template< typename F > constexpr void else_ ( F const& f ) { f(identity()); }
    }; 
    } // detail

    template< bool Condition, typename F > 
    detail::static_if_statement<Condition> constexpr static_if ( F const& f )
    { 
        detail::static_if_statement<Condition> if_;
        if_.then(f); 
        return if_; 
    }
} // utility
} // boost

/*
    Then you can call it like this: 

    template< typename T > 
    void assign ( T& x, T const& y ) 
    { 
        x = y; 

        static_if<boost::has_equal_to<T>::value>([](auto f) 
        { 
            assert(f(x) == f(y)); 
            std::cout << "asserted for: " << typeid(T).name() << std::endl; 
        }) 
        .else_([] (auto) 
        { 
            std::cout << "cannot assert for: " << typeid(T).name() << std::endl; 
        }); 
    } 
*/

#define BOOST_STATIC_IF(condition) boost::utility::static_if<r_interval.no_exception()>([&](auto f)
#define BOOST_STATIC_ELSE .else_([&] (auto)

#endif // BOOST_STATIC_IF_HPP
