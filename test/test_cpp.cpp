#include <climits>

#include "../include/cpp.hpp"

using namespace boost::numeric;

// create custom policy which emulates native one
// so we can compare the results
using custom = cpp<
    CHAR_BIT,
    CHAR_BIT * sizeof(short),
    CHAR_BIT * sizeof(int),
    CHAR_BIT * sizeof(long),
    CHAR_BIT * sizeof(long long)
>;

template<typename T, typename U>
using custom_result_type =
    custom::usual_arithmetic_conversions<
        custom::integral_promotion<T>,
        custom::integral_promotion<U>
    >;

#include <boost/preprocessor/cat.hpp>
#include "test_types.hpp"
#include "test.hpp"

#define test_integral_promotion(T)        \
void test_integral_promotion(T){          \
    static_assert(                        \
        std::is_same<                     \
            custom::integral_promotion<T>,\
            custom::local_int_type        \
        >::value,                         \
        BOOST_PP_STRINGIZE(failed integral promotion T) \
    );                                   \
}

static_assert(
    custom::rank<custom::local_char_type>::value == 1,
    "rank char != 1"
);
static_assert(
    custom::rank<std::make_signed<custom::local_char_type>::type>::value == 1,
    "rank signed char != 1"
);
static_assert(
    custom::rank<std::make_unsigned<custom::local_char_type>::type>::value == 1,
    "rank unsigned char != 1"
);
static_assert(
    custom::rank<custom::local_short_type>::value == 2,
    "rank short != 2"
);
static_assert(
    custom::rank<std::make_unsigned<custom::local_short_type>::type>::value == 2,
    "rank unsigned short != 2"
);
static_assert(
    custom::rank<custom::local_int_type>::value == 3,
    "rank int != 3"
);
static_assert(
    custom::rank<std::make_unsigned<custom::local_int_type>::type>::value == 3,
    "rank unsigned int != 3"
);

test_integral_promotion(char)
test_integral_promotion(signed char)
test_integral_promotion(unsigned char)
test_integral_promotion(signed short)
test_integral_promotion(unsigned short)

// compare our custom promotion policy with the native
// one.  They should be the same.
#define test_usual_arithmetic_conversions(T1, T2) \
void test_usual_arithmetic_conversions(T1, T2){   \
    static_assert(                       \
        std::is_same<                    \
            custom_result_type<T1, T2>,  \
            decltype(T1() + T2())        \
        >::value,                        \
        BOOST_PP_STRINGIZE(conversion fails T1 T2) \
    );                                   \
}

#define TEST(T1, T2)              \
    test_usual_arithmetic_conversions(T1, T2);
/**/

#define EACH_TYPE2(z, type_index1, type_index2)    \
    TEST(                                          \
        BOOST_PP_ARRAY_ELEM(type_index1, TYPES),   \
        BOOST_PP_ARRAY_ELEM(type_index2, TYPES)    \
    )                                              \
/**/

#define EACH_TYPE1(z, type_index1, nothing)        \
    BOOST_PP_REPEAT(                               \
        BOOST_PP_ARRAY_SIZE(TYPES),                \
        EACH_TYPE2,                                \
        type_index1                                \
    )                                              \
/**/

BOOST_PP_REPEAT(
    BOOST_PP_ARRAY_SIZE(TYPES),
    EACH_TYPE1,
    nothing
)

int main(int argc, char *argv[]){
    // this is a compile only test - but since many build systems
    // can't handle a compile-only test - make sure it passes trivially.
    return 0;
}
