#include <gtest/gtest.h>
#include <shape.hh>
using namespace RayTracer;

TEST(Sphere, normal_at_local_x_axis) {
  constexpr Sphere s;
  constexpr auto n = s.LocalNormalAt(MakePoint(1, 0, 0));
  EXPECT_EQ(n, MakeVector(1, 0, 0));
}

TEST(Sphere, normal_at_local_y_axis) {
  constexpr Sphere s;
  constexpr auto n = s.LocalNormalAt(MakePoint(0, 1, 0));
  EXPECT_EQ(n, MakeVector(0, 1, 0));
}

TEST(Sphere, normal_at_local_z_axis) {
  constexpr Sphere s;
  constexpr auto n = s.LocalNormalAt(MakePoint(0, 0, 1));
  EXPECT_EQ(n, MakeVector(0, 0, 1));
}

TEST(Sphere, normal_at_local_nonaxial_point) {
  constexpr Sphere s;
  constexpr double coord = MathUtils::ConstExprSqrtf(3.0) / 3;
  constexpr auto n = s.LocalNormalAt(MakePoint(coord, coord, coord));
  EXPECT_EQ(n, MakeVector(coord, coord, coord));
}

TEST(Sphere, normal_vector_should_be_normalized) {
  constexpr Sphere s;
  constexpr double coord = MathUtils::ConstExprSqrtf(3.0) / 3;
  const auto n = s.LocalNormalAt(MakePoint(coord, coord, coord));
  EXPECT_EQ(n, ToNormalizedVector(n));
}

TEST(sphere, normal_on_a_translated_sphere) {
  constexpr Transform M = MatrixUtils::Translation(0, 1, 0);
  constexpr Sphere s{M};
  constexpr auto worldPoint = MakePoint(0, 1.70711, -0.70711);
  constexpr auto n = s.WorldNormalAt(worldPoint);
  EXPECT_EQ(n, MakeVector(0, 0.70711, -0.70711));
}

TEST(sphere, normal_on_a_translated_sphere_2) {
  constexpr auto pi = MathUtils::MathConstants::PI<double>;
  constexpr double coord = MathUtils::ConstExprSqrtf(2.0) / 2;
  constexpr Transform M =
      MatrixUtils::Scale(1, 0.5, 1) * MatrixUtils::RotateZ(pi / 5);
  constexpr Sphere s{M};
  constexpr auto worldPoint = MakePoint(0, coord, -coord);
  constexpr auto n = s.WorldNormalAt(worldPoint);
  EXPECT_EQ(n, MakeVector(0, 0.97014, -0.24254));
}

TEST(plane, normal_of_plane_constant_everywhere) {
  constexpr Plane p;
  constexpr auto p1 = MakePoint(0, 0, 0);
  constexpr auto p2 = MakePoint(10, 0, -10);
  constexpr auto p3 = MakePoint(-5, 0, 150);
  constexpr auto n1 = p.LocalNormalAt(p1);
  constexpr auto n2 = p.LocalNormalAt(p2);
  constexpr auto n3 = p.LocalNormalAt(p3);
  EXPECT_EQ(n1, MakeVector(0, 1, 0));
  EXPECT_EQ(n2, MakeVector(0, 1, 0));
  EXPECT_EQ(n3, MakeVector(0, 1, 0));
}