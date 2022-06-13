#include <gtest/gtest.h>

#include <primitives/static_vector.hh>
using namespace RayTracer;

using testing::Eq;

TEST(StaicVector, default_constructor)
{
    StaticVector<int, 4> staticVector{1, 2, 3, 4};
    EXPECT_EQ(staticVector.size(), 4);
    EXPECT_EQ(staticVector.capacity(), 4);
    EXPECT_FALSE(staticVector.empty());
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(staticVector[i], i + 1);
        EXPECT_EQ(staticVector.data()[i], i + 1);
    }
    for (int i = 0; i < 4; i++) {
        staticVector[i] = i;
    }
    EXPECT_EQ(staticVector.back(), 3);
    int iterValue = 0;
    for (auto it = staticVector.begin(); it != staticVector.end(); it++) {
        EXPECT_EQ(*it, iterValue++);
    }
}

TEST(StaticVector, iterator_constructor)
{
    constexpr std::array<int, 4> arr{1, 2, 3, 4};
    constexpr auto staticVector = StaticVector<int, 4>(arr.begin(), arr.end());
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(staticVector[i], i + 1);
        EXPECT_EQ(staticVector.data()[i], i + 1);
    }
}
