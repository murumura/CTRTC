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

TEST(Pattern, ring_pattern_extend_along_X_and_Z) {
  constexpr RingPattern ringPatn = [&]() {
    RingPattern ret;
    ret.SetColourA(PredefinedColours::WHITE);
    ret.SetColourB(PredefinedColours::BLACK);
    return ret;
  }();
  constexpr Tuple p1 = MakePoint(0, 0, 0);
  constexpr Tuple p2 = MakePoint(1, 0, 0);
  constexpr Tuple p3 = MakePoint(0, 0, 1);
  constexpr Tuple p4 = MakePoint(0.708, 0, 0.708);
  EXPECT_EQ(ringPatn.StrideAt(p1), PredefinedColours::WHITE);
  EXPECT_EQ(ringPatn.StrideAt(p2), PredefinedColours::BLACK);
  EXPECT_EQ(ringPatn.StrideAt(p3), PredefinedColours::BLACK);
  EXPECT_EQ(ringPatn.StrideAt(p4), PredefinedColours::BLACK);
}

TEST(Pattern, Checkers_repeat_along_X) {
  constexpr CheckerPattern checkerPatn = [&]() {
    CheckerPattern ret;
    ret.SetColourA(PredefinedColours::WHITE);
    ret.SetColourB(PredefinedColours::BLACK);
    return ret;
  }();
  constexpr Tuple p1 = MakePoint(0, 0, 0);
  constexpr Tuple p2 = MakePoint(0.99, 0, 0);
  constexpr Tuple p3 = MakePoint(1.01, 0, 0);

  EXPECT_EQ(checkerPatn.StrideAt(p1), PredefinedColours::WHITE);
  EXPECT_EQ(checkerPatn.StrideAt(p2), PredefinedColours::WHITE);
  EXPECT_EQ(checkerPatn.StrideAt(p3), PredefinedColours::BLACK);
}

TEST(Pattern, Checkers_repeat_along_Y) {
  constexpr CheckerPattern checkerPatn = [&]() {
    CheckerPattern ret;
    ret.SetColourA(PredefinedColours::WHITE);
    ret.SetColourB(PredefinedColours::BLACK);
    return ret;
  }();
  constexpr Tuple p1 = MakePoint(0, 0, 0);
  constexpr Tuple p2 = MakePoint(0, 0.99, 0);
  constexpr Tuple p3 = MakePoint(0, 1.01, 0);

  EXPECT_EQ(checkerPatn.StrideAt(p1), PredefinedColours::WHITE);
  EXPECT_EQ(checkerPatn.StrideAt(p2), PredefinedColours::WHITE);
  EXPECT_EQ(checkerPatn.StrideAt(p3), PredefinedColours::BLACK);
}

TEST(Pattern, Checkers_repeat_along_Z) {
  constexpr CheckerPattern checkerPatn = [&]() {
    CheckerPattern ret;
    ret.SetColourA(PredefinedColours::WHITE);
    ret.SetColourB(PredefinedColours::BLACK);
    return ret;
  }();
  constexpr Tuple p1 = MakePoint(0, 0, 0);
  constexpr Tuple p2 = MakePoint(0, 0, 0.99);
  constexpr Tuple p3 = MakePoint(0, 0, 1.01);

  EXPECT_EQ(checkerPatn.StrideAt(p1), PredefinedColours::WHITE);
  EXPECT_EQ(checkerPatn.StrideAt(p2), PredefinedColours::WHITE);
  EXPECT_EQ(checkerPatn.StrideAt(p3), PredefinedColours::BLACK);
}

TEST(Pattern, solid_pattern_return_same_colors) {
  constexpr auto solidColor = MakeColour(1.0, 0.0, 0.0);
  constexpr SolidPattern solidPatn = [&]() {
    SolidPattern ret;
    ret.SetColourA(solidColor);
    return ret;
  }();
  constexpr Tuple p1 = MakePoint(0, 0, 0);
  constexpr Tuple p2 = MakePoint(0, 0, 0.99);
  constexpr Tuple p3 = MakePoint(0, 0, 1.01);

  EXPECT_EQ(solidPatn.StrideAt(p1), solidColor);
  EXPECT_EQ(solidPatn.StrideAt(p2), solidColor);
  EXPECT_EQ(solidPatn.StrideAt(p3), solidColor);
}

TEST(Pattern, blended_pattern_return_blended_colors) {
  constexpr BlendedPattern blendedPatn = [&]() {
    BlendedPattern ret;
    ret.SetColourA(PredefinedColours::WHITE);
    ret.SetColourB(PredefinedColours::BLACK);
    return ret;
  }();
  constexpr Tuple p1 = MakePoint(0, 0, 0);
  constexpr Tuple p2 = MakePoint(0, 0, 0.99);
  constexpr Tuple p3 = MakePoint(0, 0, 1.01);
  constexpr Colour gray{0.5, 0.5, 0.5};
  EXPECT_EQ(blendedPatn.StrideAt(p1), gray);
  EXPECT_EQ(blendedPatn.StrideAt(p2), gray);
  EXPECT_EQ(blendedPatn.StrideAt(p3), gray);
}

TEST(Pattern, radial_gradient_pattern) {
  constexpr Colour red{1.0, 0.0, 0.0};   // Red color for the gradient start
  constexpr Colour blue{0.0, 0.0, 1.0};  // Blue color for the gradient end
  constexpr Colour gray{0.5, 0.5, 0.5};

  constexpr RadialGradientPattern radialPatn = [&]() {
    RadialGradientPattern ret;
    ret.SetColourA(red);
    ret.SetColourB(blue);
    return ret;
  }();

  constexpr Tuple p1 = MakePoint(0, 0, 0);
  constexpr Tuple p2 = MakePoint(0, 0, 0.99);
  constexpr Tuple p3 = MakePoint(0, 0, 1.01);

  EXPECT_EQ(radialPatn.StrideAt(p1), red);
  EXPECT_EQ(radialPatn.StrideAt(p2), MakeColour(0.01, 0, 0.99));
  EXPECT_EQ(radialPatn.StrideAt(p3), blue);
}

TEST(Pattern, jitter_point) {
  auto NoJitter = [](const Tuple& pts) constexpr {
    return pts;
  };
  constexpr Tuple p1 = MakePoint(0, 0, 0);
  constexpr Tuple p2 = MakePoint(0, 0, 0.99);
  constexpr Tuple p3 = MakePoint(0, 0, 1.01);
  constexpr Tuple NojitterPt1 = JitterPoint(NoJitter, p1);
  constexpr Tuple NojitterPt2 = JitterPoint(NoJitter, p2);
  constexpr Tuple NojitterPt3 = JitterPoint(NoJitter, p3);
  EXPECT_EQ(NojitterPt1, p1);
  EXPECT_EQ(NojitterPt2, p2);
  EXPECT_EQ(NojitterPt3, p3);
}