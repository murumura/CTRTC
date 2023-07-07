#include <gtest/gtest.h>
#include <primitives.hh>
using namespace RayTracer;
using testing::Eq;
TEST(Pattern, stride_pattern) {
  constexpr StridePattern stridePatn;
  EXPECT_EQ(stridePatn.colourA, PredefinedColours::WHITE);
  EXPECT_EQ(stridePatn.colourB, PredefinedColours::BLACK);
}

TEST(Pattern, stride_pattern_const_Y) {
  // A stride pattern is constant in Y
  constexpr StridePattern stridePatn;
  constexpr Tuple p1 = MakePoint(0, 0, 0);
  constexpr Tuple p2 = MakePoint(0, 1, 0);
  constexpr Tuple p3 = MakePoint(0, 2, 0);
  EXPECT_EQ(stridePatn.StrideAt(p1), PredefinedColours::WHITE);
  EXPECT_EQ(stridePatn.StrideAt(p2), PredefinedColours::WHITE);
  EXPECT_EQ(stridePatn.StrideAt(p3), PredefinedColours::WHITE);
}

TEST(Pattern, stride_pattern_const_Z) {
  // A stride pattern is constant in Z
  constexpr StridePattern stridePatn;
  constexpr Tuple p1 = MakePoint(0, 0, 0);
  constexpr Tuple p2 = MakePoint(0, 0, 1);
  constexpr Tuple p3 = MakePoint(0, 0, 2);
  EXPECT_EQ(stridePatn.StrideAt(p1), PredefinedColours::WHITE);
  EXPECT_EQ(stridePatn.StrideAt(p2), PredefinedColours::WHITE);
  EXPECT_EQ(stridePatn.StrideAt(p3), PredefinedColours::WHITE);
}

TEST(Pattern, stride_pattern_alternate_X) {
  // A stride pattern is alternates in X
  constexpr StridePattern stridePatn;
  constexpr Tuple p1 = MakePoint(0, 0, 0);
  constexpr Tuple p2 = MakePoint(0.9, 0, 0);
  constexpr Tuple p3 = MakePoint(1.0, 0, 0);
  constexpr Tuple p4 = MakePoint(-0.1, 0, 0);
  constexpr Tuple p5 = MakePoint(-1, 0, 0);
  constexpr Tuple p6 = MakePoint(-1.1, 0, 0);
  EXPECT_EQ(stridePatn.StrideAt(p1), PredefinedColours::WHITE);
  EXPECT_EQ(stridePatn.StrideAt(p2), PredefinedColours::WHITE);
  EXPECT_EQ(stridePatn.StrideAt(p3), PredefinedColours::BLACK);
  EXPECT_EQ(stridePatn.StrideAt(p4), PredefinedColours::BLACK);
  EXPECT_EQ(stridePatn.StrideAt(p5), PredefinedColours::BLACK);
  EXPECT_EQ(stridePatn.StrideAt(p6), PredefinedColours::WHITE);
}

TEST(Pattern, stride_object_transformation) {
  constexpr auto scale = MatrixUtils::Scale(2, 2, 2);
  constexpr Sphere sphere{scale};
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr StridePattern stridePatn;
  constexpr Tuple p = MakePoint(1.5, 0, 0);
  EXPECT_EQ(stridePatn.StrideAtObject(shapeWrapper, p),
            PredefinedColours::WHITE);
}

TEST(Pattern, stride_pattern_transformation) {
  constexpr auto scale = MatrixUtils::Scale(2, 2, 2);
  constexpr Sphere sphere;
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr StridePattern stridePatn{scale};
  constexpr Tuple p = MakePoint(1.5, 0, 0);
  EXPECT_EQ(stridePatn.StrideAtObject(shapeWrapper, p),
            PredefinedColours::WHITE);
}

TEST(Pattern, stride_object_and_pattern_transformation) {
  constexpr auto scale = MatrixUtils::Scale(2, 2, 2);
  constexpr auto translation = MatrixUtils::Translation(0.5, 0, 0);
  constexpr Sphere sphere{scale};
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr StridePattern stridePatn{translation};
  constexpr Tuple p = MakePoint(2.5, 0, 0);
  EXPECT_EQ(stridePatn.StrideAtObject(shapeWrapper, p),
            PredefinedColours::WHITE);
}

TEST(Pattern, default_pattern_translation) {
  constexpr TestPattern testPatn;
  constexpr PatternWrapper patternWrapper = PatternWrapper(testPatn);
  constexpr auto I4 = PredefinedMatrices::I<double, 4>;
  EXPECT_EQ(I4, patternWrapper.GetTransform());
}

TEST(Pattern, assigning_a_transformation) {
  constexpr auto translation = MatrixUtils::Translation(1, 2, 3);
  constexpr TestPattern testPatn{translation};
  constexpr PatternWrapper patternWrapper = PatternWrapper(testPatn);
  EXPECT_EQ(translation, patternWrapper.GetTransform());
}

TEST(Pattern, pattern_with_object_transform) {
  constexpr auto scale = MatrixUtils::Scale(2, 2, 2);
  constexpr Sphere sphere{scale};
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr TestPattern testPatn;
  constexpr PatternWrapper patternWrapper = PatternWrapper(testPatn);
  constexpr Tuple p = MakePoint(2, 3, 4);
  EXPECT_EQ(patternWrapper.StrideAtObject(shapeWrapper, p),
            MakeColour(1.0, 1.5, 2.0));
}

TEST(Pattern, pattern_with_pattern_transform) {
  constexpr auto scale = MatrixUtils::Scale(2, 2, 2);
  constexpr Sphere sphere;
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr TestPattern testPatn{scale};
  constexpr PatternWrapper patternWrapper = PatternWrapper(testPatn);
  constexpr Tuple p = MakePoint(2, 3, 4);
  EXPECT_EQ(patternWrapper.StrideAtObject(shapeWrapper, p),
            MakeColour(1.0, 1.5, 2.0));
}

TEST(Pattern, pattern_with_both_object_and_pattern_transform) {
  constexpr auto scale = MatrixUtils::Scale(2, 2, 2);
  constexpr Sphere sphere{scale};
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr TestPattern testPatn{scale};
  constexpr PatternWrapper patternWrapper = PatternWrapper(testPatn);
  constexpr Tuple p = MakePoint(2, 3, 4);
  EXPECT_EQ(patternWrapper.StrideAtObject(shapeWrapper, p),
            MakeColour(0.5, 0.75, 1));
}

TEST(Pattern, gradient_pattern_linearly_interpolates) {

  constexpr GradientPattern gradientPatn = [&]() {
    GradientPattern ret;
    ret.SetColourA(PredefinedColours::WHITE);
    ret.SetColourB(PredefinedColours::BLACK);
    return ret;
  }();
  constexpr Tuple p1 = MakePoint(0, 0, 0);
  constexpr Tuple p2 = MakePoint(0.25, 0, 0);
  constexpr Tuple p3 = MakePoint(0.5, 2, 0);
  constexpr Tuple p4 = MakePoint(0.75, 2, 0);
  EXPECT_EQ(gradientPatn.StrideAt(p1), PredefinedColours::WHITE);
  EXPECT_EQ(gradientPatn.StrideAt(p2), MakeColour(0.75, 0.75, 0.75));
  EXPECT_EQ(gradientPatn.StrideAt(p3), MakeColour(0.5, 0.5, 0.5));
  EXPECT_EQ(gradientPatn.StrideAt(p4), MakeColour(0.25, 0.25, 0.25));
}
