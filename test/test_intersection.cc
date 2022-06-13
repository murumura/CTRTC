#include <gtest/gtest.h>
#include <limits>
#include <shape.hh>
using namespace RayTracer;
using testing::Eq;

TEST(Intersection, encapsulates)
{
    // An intersection encapsulates the distance from the ray origin and the object intersected.
    constexpr Sphere sphere;
    static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
    constexpr auto I = Intersection(3.5, &shapeWrapper);
    EXPECT_EQ(I.GetIntersectDistance(), 3.5);
    EXPECT_EQ(I.GetShapeType(), ShapeType::SphereTag);
}

TEST(Intersection, aggregations)
{
    constexpr Sphere sphere;
    static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
    constexpr auto I1 = Intersection(2, &shapeWrapper);
    constexpr auto I2 = Intersection(1, &shapeWrapper);
    constexpr auto xs = IntersectionUtils::AggregateIntersection(I1, I2);
    EXPECT_EQ(xs.size(), 2);
    EXPECT_EQ(xs[0], I2);
    EXPECT_EQ(xs[1], I1);
}

TEST(Intersection, Intersection_object)
{
    // Intersect sets the object on the intersection
    // intersect with sphere should give us 2 intersection point
    constexpr Sphere sphere;
    static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
    constexpr Tuple origin = MakePoint(0, 0, -5);
    constexpr Tuple direction = MakeVector(0, 0, 1);
    constexpr Ray ray{origin, direction};
    constexpr auto xs = sphere.IntersectWith(ray, &shapeWrapper);
    EXPECT_EQ(xs.size(), 2);
    EXPECT_EQ(xs[0].GetShapeType(), ShapeType::SphereTag);
    EXPECT_EQ(xs[1].GetShapeType(), ShapeType::SphereTag);
}

TEST(Intersection, all_positive_distance)
{
    constexpr Sphere sphere;
    static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
    constexpr auto I1 = Intersection(2, &shapeWrapper);
    constexpr auto I2 = Intersection(1, &shapeWrapper);
    constexpr auto xs = IntersectionUtils::AggregateIntersection(I1, I2);
    constexpr auto I = IntersectionUtils::VisibleHit(xs);
    EXPECT_EQ(I, I2);
}

TEST(Intersection, some_negative_distance)
{
    constexpr Sphere sphere;
    static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
    constexpr auto I1 = Intersection(-2, &shapeWrapper);
    constexpr auto I2 = Intersection(4, &shapeWrapper);
    constexpr auto I3 = Intersection(-1, &shapeWrapper);
    constexpr auto I4 = Intersection(2, &shapeWrapper);
    constexpr auto xs = IntersectionUtils::AggregateIntersection(I1, I2, I3, I4);
    EXPECT_EQ(xs[0], I4);
    EXPECT_EQ(xs[1], I2);
    EXPECT_EQ(xs[2], I1);
    EXPECT_EQ(xs[3], I3);
    constexpr auto I = IntersectionUtils::VisibleHit(xs);
    EXPECT_EQ(I, I4);
}

TEST(Intersection, all_negative_distance)
{
    constexpr Sphere sphere;
    static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
    constexpr auto I1 = Intersection(-1, &shapeWrapper);
    constexpr auto I2 = Intersection(-4, &shapeWrapper);
    constexpr auto I3 = Intersection(-2, &shapeWrapper);
    constexpr auto I4 = Intersection(-3, &shapeWrapper);
    constexpr auto xs = IntersectionUtils::AggregateIntersection(I1, I2, I3, I4);
    EXPECT_EQ(xs[0], I1);
    EXPECT_EQ(xs[1], I2);
    EXPECT_EQ(xs[2], I3);
    EXPECT_EQ(xs[3], I4);
    constexpr auto I = IntersectionUtils::VisibleHit(xs);
    EXPECT_EQ(I, std::nullopt);
}

TEST(Intersection, Intersect_with_scaled_sphere)
{
    constexpr auto scale = MatrixUtils::Scale(2, 2, 2);
    constexpr Sphere sphere{scale};
    static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
    constexpr Tuple origin = MakePoint(0, 0, -5);
    constexpr Tuple direction = MakeVector(0, 0, 1);
    constexpr Ray ray{origin, direction};
    constexpr auto xs = sphere.IntersectWith(ray, &shapeWrapper);
    EXPECT_EQ(xs.size(), 2);
    EXPECT_EQ(xs[0].GetIntersectDistance(), 3);
    EXPECT_EQ(xs[1].GetIntersectDistance(), 7);
}

TEST(Intersection, Intersect_with_translated_sphere)
{
    constexpr auto translation = MatrixUtils::Translation(5, 0, 0);
    constexpr Sphere sphere{translation};
    static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
    constexpr Tuple origin = MakePoint(0, 0, -5);
    constexpr Tuple direction = MakeVector(0, 0, 1);
    constexpr Ray ray{origin, direction};
    constexpr auto xs = sphere.IntersectWith(ray, &shapeWrapper);
    EXPECT_EQ(xs.size(), 0);
}
