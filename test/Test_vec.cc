#include <gmock/gmock.h>

#include <vec.hh>
using namespace RayTracer;

using testing::Eq;

TEST(plain_vec_initialization, list_constructor)
{
    /* Regular vector initialization from list constructor */
    constexpr Vec<4, double> v1{0, 1, 2, 3};
    constexpr Vec<4, double> v2{0, 1, 2, 3};
    ASSERT_THAT(v1.size(), Eq(4));
    ASSERT_EQ(v1.size(), v2.size());
}