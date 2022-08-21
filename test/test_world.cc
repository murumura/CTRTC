#include <gtest/gtest.h>
#include <ray.hh>
#include <world.hh>
using namespace RayTracer;

TEST(World, empty_world) {
  constexpr std::array<ShapeWrapper, 0> emptyShapes;
  constexpr std::array<PointLight, 0> emptyLights;
  World<decltype(emptyShapes), decltype(emptyLights)> emptyWorld{
      std::move(emptyShapes), std::move(emptyLights)};
  EXPECT_TRUE(emptyWorld.shapes.empty());
  EXPECT_TRUE(emptyWorld.lights.empty());
}

TEST(World, default_world) {
  constexpr auto defaultWorld = WorldUtils::DefaultWorld();
  constexpr Material m = []() {
    Material ret;
    ret.color = MakeColour(0.8, 1.0, 0.6);
    ret.diffuse = 0.7;
    ret.specular = 0.2;
    return ret;
  }();
  constexpr Sphere s1 = Sphere{m};
  constexpr Transform scale = MatrixUtils::Scale(0.5, 0.5, 0.5);
  constexpr Sphere s2 = Sphere{scale};

  EXPECT_TRUE(defaultWorld.ContainShape(s1));
  EXPECT_TRUE(defaultWorld.ContainShape(s2));
}

TEST(World, intersect_world_with_a_ray) {
  constexpr auto defaultWorld = WorldUtils::DefaultWorld();
  constexpr Tuple origin = MakePoint(0, 0, -5);
  constexpr Tuple direction = MakeVector(0, 0, 1);
  constexpr Ray ray{origin, direction};
  constexpr std::size_t numXS = defaultWorld.NumIntersections();
  constexpr auto xs = defaultWorld.IntersectWithRay<numXS>(ray);
  EXPECT_EQ(xs.size(), 4);
}
