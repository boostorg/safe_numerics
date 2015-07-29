#include <climits>

#include "../include/cpp.hpp"
#include "../include/native.hpp"

using namespace boost::numeric;

// create custom policy which emulates native one
using custom = cpp<
    CHAR_BIT,
    CHAR_BIT * sizeof(short),
    CHAR_BIT * sizeof(int),
    CHAR_BIT * sizeof(long),
    CHAR_BIT * sizeof(long long)
>;

template<typename T, typename U>
using custom_result_type =
    typename custom::usual_arithmetic_conversions<
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

test_integral_promotion(char)
test_integral_promotion(signed char)
test_integral_promotion(unsigned char)
test_integral_promotion(signed short)
test_integral_promotion(unsigned short)


#define test_usual_arithmetic_conversions(T1, T2) \
void test_usual_arithmetic_conversions(T1, T2){   \
    static_assert(                       \
        std::is_same<                    \
            custom_result_type<T1, T2>,  \
            native::result_type<T1, T2>  \
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
#endif

int main(int argc, char *argv[]){
    // this is a compile only test - but since many build systems
    // can't handle a compile-only test - make sure it passes trivially.
    return 0;
}
