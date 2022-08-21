#ifndef WORLD_HH
#define WORLD_HH
#include <concepts>
#include <primitives/dynamic_vector.hh>
#include <shading.hh>
#include <shape.hh>
#include <tuple>
namespace RayTracer {

template <typename T>
using uncvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

// helper constant for the visitor #3
template <class>
inline constexpr bool always_false_v = false;

template <class ShapeContainer, class LightContainer>
class World {
 public:
  constexpr World(ShapeContainer&& shapeContainer,
                  LightContainer&& lightContainer)
      : shapes{std::forward<ShapeContainer>(shapeContainer)},
        lights{std::forward<LightContainer>(lightContainer)} {}

  template <std::size_t N>
  constexpr auto IntersectWithRay(const Ray& ray) const
      -> std::array<Intersection, N> {
    DynamicVector<Intersection> dynamicVector;

    for (int i = 0; i < shapes.size(); i++) {
      const auto variant = shapes[i].IntersectWith(ray);
      auto visitor = [&](auto&& v) {
        using T = uncvref_t<decltype(v)>;
        if constexpr (std::is_same_v<T, StaticVector<Intersection, 2>>) {
          const auto xs = std::get<StaticVector<Intersection, 2>>(variant);
          for (std::size_t i = 0; i < 2; i++)
            dynamicVector.push_back(xs[i]);
        } else if constexpr (std::is_same_v<T, StaticVector<Intersection, 1>>) {
          const auto xs = std::get<StaticVector<Intersection, 1>>(variant);
          dynamicVector.push_back(xs[0]);
        } else {
          static_assert(always_false_v<T>, "non-exhaustive visitor!");
        }
      };
      // visit the variant
      std::visit(visitor, variant);
    }
    std::array<Intersection, N> ret;
    std::ranges::copy(ret, dynamicVector.begin());
    return ret;
  }

  constexpr std::size_t NumIntersections() const {
    std::size_t numXs = 0;
    for (int i = 0; i < shapes.size(); i++) {
      const auto variant = shapes[i];
      if constexpr (std::is_constructible_v<decltype(variant),
                                            std::variant<Sphere>>) {
        numXs += 2;
      }
    }
    return numXs;
  }

  template <typename ShapeType>
  constexpr bool ContainShape(const ShapeType& shape) const {
    for (std::size_t i = 0; i < shapes.size(); i++)
      if (shape == shapes[i])
        return true;
    return false;
  }

  constexpr ShapeContainer const& GetShapes() const { return shapes; }

  constexpr LightContainer const& GetLights() const { return lights; }

  ShapeContainer shapes;
  LightContainer lights;
};

namespace WorldUtils {

constexpr auto DefaultWorld() {
  PointLight light = PointLight(MakePoint(0, 0, -5), MakeColour(1, 1, 1));
  std::array<PointLight, 1> lights = {light};
  Material m = []() {
    Material ret;
    ret.color = MakeColour(0.8, 1.0, 0.6);
    ret.diffuse = 0.7;
    ret.specular = 0.2;
    return ret;
  }();
  Sphere s1 = Sphere{m};
  Transform scale = MatrixUtils::Scale(0.5, 0.5, 0.5);
  Sphere s2 = Sphere{scale};
  std::array<ShapeWrapper, 2> shapes = {s1, s2};
  World<decltype(shapes), decltype(lights)> defaultWorld{std::move(shapes),
                                                         std::move(lights)};
  return defaultWorld;
}
}  // namespace WorldUtils
}  // namespace RayTracer

#endif
