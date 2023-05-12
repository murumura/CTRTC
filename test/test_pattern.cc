#include <gtest/gtest.h>
#include <primitives.hh>
using namespace RayTracer;
using testing::Eq;
TEST(Pattern, stride_pattern) {
  constexpr StridePattern stridePat;
  EXPECT_EQ(stridePat.colourA, PredefinedColours::WHITE);
  EXPECT_EQ(stridePat.colourB, PredefinedColours::BLACK);
}

TEST(Pattern, stride_pattern_const_Y) {
  // A stride pattern is constant in Y
  constexpr StridePattern stridePat;
  constexpr Tuple p1 = MakePoint(0, 0, 0);
  constexpr Tuple p2 = MakePoint(0, 1, 0);
  constexpr Tuple p3 = MakePoint(0, 2, 0);
  EXPECT_EQ(stridePat.StrideAt(p1), PredefinedColours::WHITE);
  EXPECT_EQ(stridePat.StrideAt(p2), PredefinedColours::WHITE);
  EXPECT_EQ(stridePat.StrideAt(p3), PredefinedColours::WHITE);
}

TEST(Pattern, stride_pattern_const_Z) {
  // A stride pattern is constant in Z
  constexpr StridePattern stridePat;
  constexpr Tuple p1 = MakePoint(0, 0, 0);
  constexpr Tuple p2 = MakePoint(0, 0, 1);
  constexpr Tuple p3 = MakePoint(0, 0, 2);
  EXPECT_EQ(stridePat.StrideAt(p1), PredefinedColours::WHITE);
  EXPECT_EQ(stridePat.StrideAt(p2), PredefinedColours::WHITE);
  EXPECT_EQ(stridePat.StrideAt(p3), PredefinedColours::WHITE);
}

TEST(Pattern, stride_pattern_alternate_X) {
  // A stride pattern is alternates in X
  constexpr StridePattern stridePat;
  constexpr Tuple p1 = MakePoint(0, 0, 0);
  constexpr Tuple p2 = MakePoint(0.9, 0, 0);
  constexpr Tuple p3 = MakePoint(1.0, 0, 0);
  constexpr Tuple p4 = MakePoint(-0.1, 0, 0);
  constexpr Tuple p5 = MakePoint(-1, 0, 0);
  constexpr Tuple p6 = MakePoint(-1.1, 0, 0);
  EXPECT_EQ(stridePat.StrideAt(p1), PredefinedColours::WHITE);
  EXPECT_EQ(stridePat.StrideAt(p2), PredefinedColours::WHITE);
  EXPECT_EQ(stridePat.StrideAt(p3), PredefinedColours::BLACK);
  EXPECT_EQ(stridePat.StrideAt(p4), PredefinedColours::BLACK);
  EXPECT_EQ(stridePat.StrideAt(p5), PredefinedColours::BLACK);
  EXPECT_EQ(stridePat.StrideAt(p6), PredefinedColours::WHITE);
}

TEST(Pattern, stride_object_transformation) {
  constexpr auto scale = MatrixUtils::Scale(2, 2, 2);
  constexpr Sphere sphere{scale};
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr StridePattern stridePat;
  constexpr Tuple p = MakePoint(1.5, 0, 0);
  EXPECT_EQ(stridePat.StrideAtObject(shapeWrapper, p),
            PredefinedColours::WHITE);
}

TEST(Pattern, stride_pattern_transformation) {
  constexpr auto scale = MatrixUtils::Scale(2, 2, 2);
  constexpr Sphere sphere;
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr StridePattern stridePat{scale};
  constexpr Tuple p = MakePoint(1.5, 0, 0);
  EXPECT_EQ(stridePat.StrideAtObject(shapeWrapper, p),
            PredefinedColours::WHITE);
}

TEST(Pattern, stride_object_and_pattern_transformation) {
  constexpr auto scale = MatrixUtils::Scale(2, 2, 2);
  constexpr auto translation = MatrixUtils::Translation(0.5, 0, 0);
  constexpr Sphere sphere{scale};
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr StridePattern stridePat{translation};
  constexpr Tuple p = MakePoint(2.5, 0, 0);
  EXPECT_EQ(stridePat.StrideAtObject(shapeWrapper, p),
            PredefinedColours::WHITE);
}