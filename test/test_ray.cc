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
    
}
