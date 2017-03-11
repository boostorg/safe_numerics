/*
Copyright 2017 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#include <sstream>
#include <cassert>
#include "../include/safe_integer.hpp"

template<class T>
inline void test()
{
    {
        std::ostringstream output;
        output << boost::numeric::safe<T>(1);
        assert(output.str() == "1");
    }
    {
        std::istringstream input("1");
        boost::numeric::safe<T> value;
        input >> value;
        assert(value == 1);
    }
}

int main()
{
    test<int>();
    test<char>();
    test<signed char>();
    test<unsigned char>();
}
