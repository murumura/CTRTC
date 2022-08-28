#include <gtest/gtest.h>
#include <camera.hh>
#include <world.hh>
using namespace RayTracer;
using testing::Eq;
TEST(Camera, constrctor) {
  constexpr std::size_t hsize = 160;
  constexpr std::size_t vsize = 120;
  constexpr auto fov = MathUtils::MathConstants::PI<double> * 0.5;
  constexpr Camera c{hsize, vsize, fov};

  EXPECT_EQ(c.HorizontalSize(), hsize);
  EXPECT_EQ(c.VerticalSize(), vsize);
  EXPECT_EQ(c.FieldOfView(), fov);
  constexpr auto I4 = PredefinedMatrices::I<double, 4>;
  EXPECT_EQ(c.Transform(), I4);
}

TEST(Camera, pixel_size_horizontal) {
  constexpr std::size_t hsize = 200;
  constexpr std::size_t vsize = 125;
  constexpr auto fov = MathUtils::MathConstants::PI<double> * 0.5;
  constexpr Camera c{hsize, vsize, fov};
  EXPECT_EQ(c.PixelSize(), 0.01);
}

TEST(Camera, pixel_size_vertical) {
  constexpr std::size_t hsize = 125;
  constexpr std::size_t vsize = 200;
  constexpr auto fov = MathUtils::MathConstants::PI<double> * 0.5;
  constexpr Camera c{hsize, vsize, fov};
  EXPECT_EQ(c.PixelSize(), 0.01);
}

TEST(Camera, ray_through_corner) {
  constexpr std::size_t hsize = 201;
  constexpr std::size_t vsize = 101;
  constexpr auto fov = MathUtils::MathConstants::PI<double> * 0.5;
  constexpr Camera c{hsize, vsize, fov};
  const auto ray = c.RayForPixel(0, 0);
  EXPECT_EQ(ray.GetOrigin(), PredefinedTuples::ZeroPoint);
  EXPECT_EQ(ray.GetDirection(), MakeVector(0.66519, 0.33259, -0.66851));
}

TEST(Camera, transform_camera) {
  constexpr std::size_t hsize = 201;
  constexpr std::size_t vsize = 101;
  constexpr auto fov = MathUtils::MathConstants::PI<double> * 0.5;
  constexpr auto pi = MathUtils::MathConstants::PI<double>;
  constexpr auto transform =
      MatrixUtils::RotateY(pi / 4.0) * MatrixUtils::Translation(0, -2, 5);
  constexpr Camera cam{hsize, vsize, fov, transform};
  constexpr auto ray = cam.RayForPixel(100, 50);
  constexpr auto coord = MathUtils::ConstExprSqrtf(2.0) / 2;
  EXPECT_EQ(ray.GetOrigin(), MakePoint(0, 2, -5));
  EXPECT_EQ(ray.GetDirection(), MakeVector(coord, 0.0, -coord));
}

TEST(Camera, render_a_world) {
  constexpr auto static defaultWorld = WorldUtils::DefaultWorld();
  constexpr auto pi = MathUtils::MathConstants::PI<double>;
  constexpr auto from = MakePoint(0, 0, -5);
  constexpr auto to = PredefinedTuples::ZeroPoint;
  constexpr auto up = MakeVector(0, 1, 0);
  constexpr auto c2w = MatrixUtils::ViewTransform(from, to, up);
  constexpr std::size_t hsize = 11;
  constexpr std::size_t vsize = 11;
  constexpr auto cam = Camera{hsize, vsize, pi / 2, c2w};
  constexpr std::size_t numXS = defaultWorld.PossibleXSNums();
  constexpr auto image =
      cam.render<hsize, vsize, numXS, decltype(defaultWorld)>(defaultWorld);

  EXPECT_EQ(image(5, 5), MakeColour(0.38066, 0.47583, 0.2855));
}