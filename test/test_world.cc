#include <gtest/gtest.h>
#include <ray.hh>
#include <world.hh>
using namespace RayTracer;

TEST(World, empty_world) {
  constexpr std::array<ShapeWrapper, 0> emptyShapes;
  constexpr std::array<PointLight, 0> emptyLights;
  constexpr static World<decltype(emptyShapes), decltype(emptyLights)>
      emptyWorld{std::move(emptyShapes), std::move(emptyLights)};
  EXPECT_TRUE(emptyWorld.shapes.empty());
  EXPECT_TRUE(emptyWorld.lights.empty());
}

TEST(World, default_world) {
  constexpr auto static defaultWorld = WorldUtils::DefaultWorld();
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
  constexpr auto static defaultWorld = WorldUtils::DefaultWorld();
  constexpr Tuple origin = MakePoint(0, 0, -5);
  constexpr Tuple direction = MakeVector(0, 0, 1);
  constexpr Ray ray{origin, direction};
  constexpr std::size_t numXS = defaultWorld.PossibleXSNums();
  constexpr auto xs = defaultWorld.IntersectWithRay<numXS>(ray);
  EXPECT_EQ(xs[0].GetIntersectDistance(), 4.0);
  EXPECT_EQ(xs[1].GetIntersectDistance(), 4.5);
  EXPECT_EQ(xs[2].GetIntersectDistance(), 5.5);
  EXPECT_EQ(xs[3].GetIntersectDistance(), 6.0);
  EXPECT_EQ(xs.size(), 4);
}

TEST(World, precompute_state_of_an_interdection) {
  constexpr auto static defaultWorld = WorldUtils::DefaultWorld();
  constexpr Tuple origin = MakePoint(0, 0, -5);
  constexpr Tuple direction = MakeVector(0, 0, 1);
  constexpr Ray ray{origin, direction};
  // first object in the world
  constexpr auto shape = defaultWorld.GetShapes().front();
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(shape);
  constexpr auto I = Intersection(4.0, &shapeWrapper);
  constexpr auto comp = I.PrepareComputation(ray);
  EXPECT_EQ(comp.t, I.t);
  EXPECT_EQ(comp.shapePtr, I.shapePtr);
  EXPECT_EQ(comp.point, MakePoint(0, 0, -1));
  EXPECT_EQ(comp.eyeV, MakeVector(0, 0, -1));
  EXPECT_EQ(comp.normalV, MakeVector(0, 0, -1));
}

TEST(World, color_when_ray_hits) {
  constexpr auto static defaultWorld = WorldUtils::DefaultWorld();
  constexpr Tuple origin = MakePoint(0, 0, -5);
  constexpr Tuple direction = MakeVector(0, 0, 1);
  constexpr Ray ray{origin, direction};
  constexpr std::size_t numXS = defaultWorld.PossibleXSNums();
  constexpr auto colour = defaultWorld.ColorAt<numXS>(ray);
  EXPECT_EQ(colour, MakeColour(0.38066, 0.47583, 0.2855));
}

TEST(World, color_when_ray_misses) {
  constexpr auto static defaultWorld = WorldUtils::DefaultWorld();
  constexpr Tuple origin = MakePoint(0, 0, -5);
  constexpr Tuple direction = MakeVector(0, 1, 0);
  constexpr Ray ray{origin, direction};
  constexpr std::size_t numXS = defaultWorld.PossibleXSNums();
  constexpr auto colour = defaultWorld.ColorAt<numXS>(ray);
  EXPECT_EQ(colour, PredefinedColours::BLACK);
}

TEST(World, color_with_ray_intersect_behind_scene) {
  constexpr PointLight light =
      PointLight(MakePoint(-10, 10, -10), MakeColour(1, 1, 1));
  constexpr std::array<PointLight, 1> lights = {light};
  constexpr Material m = []() {
    Material ret;
    ret.color = MakeColour(0.8, 1.0, 0.6);
    ret.diffuse = 0.7;
    ret.specular = 0.2;
    ret.ambient = 1.0;
    return ret;
  }();
  constexpr Sphere s1 = Sphere{m};
  constexpr Transform scale = MatrixUtils::Scale(0.5, 0.5, 0.5);
  constexpr Sphere s2 = [&]() {
    Sphere ret = Sphere{scale};
    ret.SetMaterial(m);
    return ret;
  }();
  constexpr std::array<ShapeWrapper, 2> shapes = {s1, s2};
  constexpr World<decltype(shapes), decltype(lights)> world{std::move(shapes),
                                                            std::move(lights)};
  constexpr Tuple origin = MakePoint(0, 0, 0.75);
  constexpr Tuple direction = MakeVector(0, 0, -1);
  constexpr Ray ray{origin, direction};
  constexpr auto outerShape = world.GetShapes()[0];
  constexpr auto innerShape = world.GetShapes()[1];
  constexpr std::size_t numXS = world.PossibleXSNums();
  constexpr auto colour = world.ColorAt<numXS>(ray);
  EXPECT_EQ(colour, innerShape.GetMaterial().color);
}