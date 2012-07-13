#include <boost/integer.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_integral.hpp>
#include "../include/safe_integer.hpp"

BOOST_STATIC_ASSERT(boost::is_integral<boost::int8_t>::value);
BOOST_STATIC_ASSERT(boost::is_integral<boost::numeric::safe<boost::int8_t> >::value);
