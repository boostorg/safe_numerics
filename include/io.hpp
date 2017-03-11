/*
Copyright 2017 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef BOOST_NUMERIC_IO_HPP
#define BOOST_NUMERIC_IO_HPP

namespace boost {
namespace numeric {
namespace detail {

template<class T>
inline const T& output(const T& o)
{
    return o;
}

template<class T>
inline T output(const T&& o)
{
    return o;
}

inline auto output(char c)
{
    return static_cast<int>(c);
}

inline auto output(signed char c)
{
    return static_cast<int>(c);
}

inline auto output(unsigned char c)
{
    return static_cast<int>(c);
}

template<class T>
struct input {
    using type = T;
};

template<>
struct input<char> {
    using type = int;
};

template<>
struct input<signed char> {
    using type = int;
};

template<>
struct input<unsigned char> {
    using type = int;
};

template<class T>
using input_t = typename input<T>::type;

} /* detail */
} /* numeric */
} /* boost */

#endif
