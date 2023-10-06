#include <gtest/gtest.h>
#include <limits>
#include <primitives.hh>
using namespace RayTracer;
using testing::Eq;

TEST(SphereIntersection, encapsulates) {
  // An intersection encapsulates the distance from the ray origin and the object intersected.
  constexpr Sphere sphere;
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr auto I = Intersection(3.5, &shapeWrapper);
  EXPECT_EQ(I.GetIntersectDistance(), 3.5);
  EXPECT_EQ(I.GetShapeType(), ShapeType::SphereTag);
}

TEST(SphereIntersection, aggregations) {
  constexpr Sphere sphere;
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr auto I1 = Intersection(2, &shapeWrapper);
  constexpr auto I2 = Intersection(1, &shapeWrapper);
  constexpr auto xs = IntersectionUtils::AggregateIntersections(I1, I2);
  EXPECT_EQ(xs.size(), 2);
  EXPECT_EQ(xs[0], I2);
  EXPECT_EQ(xs[1], I1);
}

TEST(SphereIntersection, intersection_object) {
  // Intersect sets the object on the intersection
  // intersect with sphere should give us 2 intersection point
  constexpr Sphere sphere;
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr Tuple origin = MakePoint(0, 0, -5);
  constexpr Tuple direction = MakeVector(0, 0, 1);
  constexpr Ray ray{origin, direction};
  constexpr auto xsVariant = sphere.IntersectWith(ray, &shapeWrapper);
  const auto xs = std::get<StaticVector<Intersection, 2>>(xsVariant);
  EXPECT_EQ(xs.size(), 2);
  EXPECT_EQ(xs[0].GetShapeType(), ShapeType::SphereTag);
  EXPECT_EQ(xs[1].GetShapeType(), ShapeType::SphereTag);
}

TEST(SphereIntersection, all_positive_distance) {
  constexpr Sphere sphere;
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr auto I1 = Intersection(2, &shapeWrapper);
  constexpr auto I2 = Intersection(1, &shapeWrapper);
  constexpr auto xs = IntersectionUtils::AggregateIntersections(I1, I2);
  constexpr auto I = IntersectionUtils::VisibleHit(xs);
  EXPECT_EQ(I, I2);
}

TEST(SphereIntersection, some_negative_distance) {
  constexpr Sphere sphere;
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr auto I1 = Intersection(-2, &shapeWrapper);
  constexpr auto I2 = Intersection(4, &shapeWrapper);
  constexpr auto I3 = Intersection(-1, &shapeWrapper);
  constexpr auto I4 = Intersection(2, &shapeWrapper);
  constexpr auto xs = IntersectionUtils::AggregateIntersections(I1, I2, I3, I4);
  EXPECT_EQ(xs[0], I4);
  EXPECT_EQ(xs[1], I2);
  EXPECT_EQ(xs[2], I1);
  EXPECT_EQ(xs[3], I3);
  constexpr auto I = IntersectionUtils::VisibleHit(xs);
  EXPECT_EQ(I, I4);
}

TEST(SphereIntersection, all_negative_distance) {
  constexpr Sphere sphere;
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr auto I1 = Intersection(-1, &shapeWrapper);
  constexpr auto I2 = Intersection(-4, &shapeWrapper);
  constexpr auto I3 = Intersection(-2, &shapeWrapper);
  constexpr auto I4 = Intersection(-3, &shapeWrapper);
  constexpr auto xs = IntersectionUtils::AggregateIntersections(I1, I2, I3, I4);
  EXPECT_EQ(xs[0], I1);
  EXPECT_EQ(xs[1], I2);
  EXPECT_EQ(xs[2], I3);
  EXPECT_EQ(xs[3], I4);
  constexpr auto I = IntersectionUtils::VisibleHit(xs);
  EXPECT_EQ(I, std::nullopt);
}

TEST(SphereIntersection, intersect_with_scaled_sphere) {
  constexpr auto scale = MatrixUtils::Scale(2, 2, 2);
  constexpr Sphere sphere{scale};
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr Tuple origin = MakePoint(0, 0, -5);
  constexpr Tuple direction = MakeVector(0, 0, 1);
  constexpr Ray ray{origin, direction};
  constexpr auto xsVariant = sphere.IntersectWith(ray, &shapeWrapper);
  const auto xs = std::get<StaticVector<Intersection, 2>>(xsVariant);
  EXPECT_EQ(xs.size(), 2);
  EXPECT_EQ(xs[0].GetIntersectDistance(), 3);
  EXPECT_EQ(xs[1].GetIntersectDistance(), 7);
}

TEST(SphereIntersection, intersect_with_translated_sphere) {
  constexpr auto translation = MatrixUtils::Translation(5, 0, 0);
  constexpr Sphere sphere{translation};
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr Tuple origin = MakePoint(0, 0, -5);
  constexpr Tuple direction = MakeVector(0, 0, 1);
  constexpr Ray ray{origin, direction};
  constexpr auto xsVariant = sphere.IntersectWith(ray, &shapeWrapper);
  const auto xs = std::get<StaticVector<Intersection, 2>>(xsVariant);
  EXPECT_EQ(xs.size(), 2);
  // TODO: a workaround for non-hitting : see shape.h for more details
  // This is different from book's illustrations
  constexpr auto I = IntersectionUtils::VisibleHitFromVariant(xsVariant);
  EXPECT_EQ(I, std::nullopt);
}

TEST(SphereIntersection, hit_should_fall_above_point) {
  constexpr Tuple origin = MakePoint(0, 0, -5);
  constexpr Tuple direction = MakeVector(0, 0, 1);
  constexpr Ray ray{origin, direction};
  constexpr Transform translation = MatrixUtils::Translation(0.0, 0.0, 1.0);
  constexpr Sphere s = Sphere{translation};
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(s);
  constexpr auto I = Intersection(5, &shapeWrapper);
  constexpr auto hitRecord = I.PrepareComputation(ray);
  EXPECT_LE(hitRecord.pointOverSurface[TupleConstants::z], -(EPSILON / 2.0));
  EXPECT_GE(hitRecord.point[TupleConstants::z],
            hitRecord.pointOverSurface[TupleConstants::z]);
}

TEST(PlaneIntersection, intersect_with_ray_parallel_to_plane) {
  constexpr Tuple origin = MakePoint(0, 10, 0);
  constexpr Tuple direction = MakeVector(0, 0, 1);
  constexpr Ray ray{origin, direction};
  constexpr Plane plane;
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(plane);
  constexpr auto xsVariant = plane.IntersectWith(ray, &shapeWrapper);
  const auto xs = std::get<StaticVector<Intersection, 1>>(xsVariant);
  EXPECT_EQ(xs.size(), 1);
  // TODO: a workaround for non-hitting : see shape.h for more details
  // This is different from book's illustrations
  constexpr auto I = IntersectionUtils::VisibleHitFromVariant(xsVariant);
  EXPECT_EQ(I, std::nullopt);
}

TEST(PlaneIntersection, intersect_with_ray_coplaner_to_plane) {
  constexpr Tuple origin = MakePoint(0, 0, 0);
  constexpr Tuple direction = MakeVector(0, 0, 1);
  constexpr Ray ray{origin, direction};
  constexpr Plane plane;
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(plane);
  constexpr auto xsVariant = plane.IntersectWith(ray, &shapeWrapper);
  const auto xs = std::get<StaticVector<Intersection, 1>>(xsVariant);
  EXPECT_EQ(xs.size(), 1);
  // TODO: a workaround for non-hitting : see shape.h for more details
  // This is different from book's illustrations
  constexpr auto I = IntersectionUtils::VisibleHitFromVariant(xsVariant);
  EXPECT_EQ(I, std::nullopt);
}

TEST(PlaneIntersection, intersect_with_ray_from_above) {
  constexpr Tuple origin = MakePoint(0, 1, 0);
  constexpr Tuple direction = MakeVector(0, -1, 0);
  constexpr Ray ray{origin, direction};
  constexpr Plane plane;
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(plane);
  constexpr auto xsVariant = plane.IntersectWith(ray, &shapeWrapper);
  const auto xs = std::get<StaticVector<Intersection, 1>>(xsVariant);
  EXPECT_EQ(xs.size(), 1);
  EXPECT_EQ(xs[0].GetIntersectDistance(), 1);
}

TEST(PlaneIntersection, intersect_with_ray_from_below) {
  constexpr Tuple origin = MakePoint(0, -1, 0);
  constexpr Tuple direction = MakeVector(0, 1, 0);
  constexpr Ray ray{origin, direction};
  constexpr Plane plane;
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(plane);
  constexpr auto xsVariant = plane.IntersectWith(ray, &shapeWrapper);
  const auto xs = std::get<StaticVector<Intersection, 1>>(xsVariant);
  EXPECT_EQ(xs.size(), 1);
  EXPECT_EQ(xs[0].GetIntersectDistance(), 1);
}