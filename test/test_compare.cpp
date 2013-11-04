// Copyright (c) 2013 Ruslan Baratov
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "../include/safe_integer.hpp"
#include <cassert>

template <class T>
void test_equal(T test_val) {
  namespace num = boost::numeric;
  num::safe<T> x = test_val;
  num::safe<T> y = test_val;
  assert(x == y);
  assert(x >= y);
  assert(x <= y);
  assert(!(x < y));
  assert(!(x > y));
}

template <class T>
void test_non_equal(T test_val) {
  namespace num = boost::numeric;
  num::safe<T> x(test_val);
  num::safe<T> y(test_val + 1);
  assert(x != y);
  assert(!(x >= y));
  assert(x <= y);
  assert(x < y);
  assert(!(x > y));
}

int main() {
  test_equal<unsigned>(0);
  test_equal<unsigned>(3);
  test_equal<unsigned short>(14);

  test_equal<int>(3);

  test_equal<uint64_t>(45);

  test_non_equal<unsigned>(342);
  test_non_equal<signed char>(33);
  test_non_equal<int32_t>(-988);
}
