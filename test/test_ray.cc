#include <gmock/gmock.h>
#include <ray.hh>
using namespace RayTracer;

TEST(Ray, default_constructor)
{
    constexpr Ray ray;
    ASSERT_EQ(ray.GetOrigin(), PredefinedTuples::ZeroPoint);
    ASSERT_EQ(ray.GetDirection(), PredefinedTuples::ZeroVector);
}

TEST(Ray, constructor)
{
    constexpr Tuple origin = MakePoint(2, 3, 4);
    constexpr Tuple direction = MakeVector(1, 0, 0);
    constexpr Ray ray{origin, direction};
    ASSERT_EQ(ray.GetOrigin(), origin);
    ASSERT_EQ(ray.GetDirection(), direction);
}

TEST(Ray, translation)
{
    constexpr Tuple origin = MakePoint(1, 2, 3);
    constexpr Tuple direction = MakeVector(0, 1, 0);
    constexpr auto m = MatrixUtils::Translation(3, 4, 5);
    constexpr Ray ray{origin, direction};
    constexpr Ray ray2 = ray.Transform(m);
    ASSERT_EQ(ray2.GetOrigin(), MakePoint(4, 6, 8));
    ASSERT_EQ(ray2.GetDirection(), MakeVector(0, 1, 0));
}

TEST(Ray, Scaling)
{
    constexpr Tuple origin = MakePoint(1, 2, 3);
    constexpr Tuple direction = MakeVector(0, 1, 0);
    constexpr auto m = MatrixUtils::Scale(2, 3, 4);
    constexpr Ray ray{origin, direction};
    constexpr Ray rayT = ray.Transform(m);
    ASSERT_EQ(rayT.GetOrigin(), MakePoint(2, 6, 12));
    ASSERT_EQ(rayT.GetDirection(), MakeVector(0, 3, 0));
}
