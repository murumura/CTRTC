#include <gtest/gtest.h>

#include <primitives.hh>
using namespace RayTracer;

TEST(PointLight, default_constructor) {
  // A point light has a position and intensity
  constexpr auto intensity = MakeColour(1, 1, 1);
  constexpr auto position = MakePoint(0, 0, 0);
  constexpr auto light = PointLight(position, intensity);
  EXPECT_EQ(light.position, position);
  EXPECT_EQ(light.intensity, intensity);
}

TEST(Material, default_properties) {
  constexpr Material m;
  EXPECT_EQ(m.color, PredefinedColours::WHITE);
  EXPECT_EQ(m.ambient, 0.1);
  EXPECT_EQ(m.diffuse, 0.9);
  EXPECT_EQ(m.specular, 0.9);
  EXPECT_EQ(m.shininess, 200.0);
}

TEST(Material, sphere_material_assign) {
  constexpr Material m = []() {
    Material ret;
    ret.ambient = 1.0;
    return ret;
  }();
  constexpr Sphere s = Sphere{m};
  EXPECT_EQ(s.material, m);
}

TEST(Material, Lighting_with_the_eye_between_light_and_surface) {
  constexpr Material m;
  constexpr Sphere sphere;
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr auto position = PredefinedTuples::ZeroPoint;
  constexpr auto eyeV = MakeVector(0, 0, -1);
  constexpr auto normalV = MakeVector(0, 0, -1);
  constexpr PointLight light{MakePoint(0, 0, -10), PredefinedColours::WHITE};
  constexpr auto result =
      Lighting(m, shapeWrapper, light, position, eyeV, normalV);
  EXPECT_EQ(result, MakeColour(1.9, 1.9, 1.9));
}

TEST(Material, eye_45_deg_offset) {
  constexpr auto sqrt2By2 = MathUtils::ConstExprSqrtf(2.0) / 2;
  constexpr Material m;
  constexpr Sphere sphere;
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr auto position = PredefinedTuples::ZeroPoint;
  constexpr auto eyeV = MakeVector(0, sqrt2By2, -sqrt2By2);
  constexpr auto normalV = MakeVector(0, 0, -1);
  constexpr PointLight light{MakePoint(0, 0, -10), PredefinedColours::WHITE};
  constexpr auto result =
      Lighting(m, shapeWrapper, light, position, eyeV, normalV);
  EXPECT_EQ(result, MakeColour(1.0, 1.0, 1.0));
}

TEST(Material, light_45_deg_offset) {
  constexpr Material m;
  constexpr Sphere sphere;
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr auto position = PredefinedTuples::ZeroPoint;
  constexpr auto eyeV = MakeVector(0, 0, -1);
  constexpr auto normalV = MakeVector(0, 0, -1);
  constexpr PointLight light{MakePoint(0, 10, -10), PredefinedColours::WHITE};
  constexpr auto result =
      Lighting(m, shapeWrapper, light, position, eyeV, normalV);
  EXPECT_EQ(result, MakeColour(0.7364, 0.7364, 0.7364));
}

TEST(Material, eye_in_the_path_of_the_reflection_path) {
  constexpr auto sqrt2By2 = MathUtils::ConstExprSqrtf(2.0) / 2;
  constexpr Material m;
  constexpr Sphere sphere;
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr auto position = PredefinedTuples::ZeroPoint;
  constexpr auto eyeV = MakeVector(0, -sqrt2By2, -sqrt2By2);
  constexpr auto normalV = MakeVector(0, 0, -1);
  constexpr PointLight light{MakePoint(0, 10, -10), PredefinedColours::WHITE};
  constexpr auto result =
      Lighting(m, shapeWrapper, light, position, eyeV, normalV);
  EXPECT_EQ(result, MakeColour(1.6364, 1.6364, 1.6364));
}

TEST(Material, light_behind_surface) {
  constexpr Material m;
  constexpr Sphere sphere;
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr auto position = PredefinedTuples::ZeroPoint;
  constexpr auto eyeV = MakeVector(0, 0, -1);
  constexpr auto normalV = MakeVector(0, 0, -1);
  constexpr PointLight light{MakePoint(0, 0, 10), PredefinedColours::WHITE};
  constexpr auto result =
      Lighting(m, shapeWrapper, light, position, eyeV, normalV);
  EXPECT_EQ(result, MakeColour(0.1, 0.1, 0.1));
}

TEST(Material, light_with_surface_in_shadow) {
  constexpr Material m;
  constexpr Sphere sphere;
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  constexpr auto position = PredefinedTuples::ZeroPoint;
  constexpr auto eyeV = MakeVector(0, 0, -1);
  constexpr auto normalV = MakeVector(0, 0, -1);
  constexpr bool inShadow = true;
  constexpr PointLight light{MakePoint(0, 0, 10), PredefinedColours::WHITE};
  constexpr auto result =
      Lighting(m, shapeWrapper, light, position, eyeV, normalV);
  EXPECT_EQ(result, MakeColour(0.1, 0.1, 0.1));
}
