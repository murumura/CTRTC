#ifndef WORLD_HH
#define WORLD_HH
#include <concepts>
#include <shading.hh>
#include <shape.hh>
#include <tuple>
namespace RayTracer {

template <typename... Ts>
struct NumXSOf {
  static constexpr int numXS = 0;
};

template <typename... Ts>
struct NumXSOf<Sphere, Ts...> {
  static constexpr int numXS =
      ShapeTraits::SphereTrait::NumIntersections + NumXSOf<Ts...>::numXS;
};

template <typename... Ts>
struct NumXSOf<Plane, Ts...> {
  static constexpr int numXS =
      ShapeTraits::PlaneTrait::NumIntersections + NumXSOf<Ts...>::numXS;
};

template <class ShapeContainer, class LightContainer, std::size_t NumXS_ = 10>
class World {
 public:
  constexpr World(ShapeContainer&& shapeContainer,
                  LightContainer&& lightContainer)
      : shapes{std::forward<ShapeContainer>(shapeContainer)},
        lights{std::forward<LightContainer>(lightContainer)} {}

  static constexpr std::size_t NumXS{NumXS_};

  constexpr auto IntersectWithRay(const Ray& ray) const
      -> std::array<Intersection, NumXS> {
    std::array<Intersection, NumXS> ret = {};
    std::size_t iter = 0;
    for (int i = 0; i < shapes.size(); i++) {
      const auto variant = shapes[i].IntersectWith(ray);
      auto visitor = [&](auto&& v) {
        using T = PrimitiveTraits::uncvref_t<decltype(v)>;
        if constexpr (std::is_same_v<T, StaticVector<Intersection, 2>>) {
          const auto xs = std::get<StaticVector<Intersection, 2>>(variant);
          for (std::size_t i = 0; i < 2; i++)
            ret[iter++] = std::move(xs[i]);
        } else if constexpr (std::is_same_v<T, StaticVector<Intersection, 1>>) {
          const auto xs = std::get<StaticVector<Intersection, 1>>(variant);
          ret[iter++] = std::move(xs[0]);
        }
      };
      // visit the variant
      std::visit(visitor, variant);
    }
    // Sort intersections before return
    return IntersectionUtils::SortIntersections(ret);
  }

  constexpr Colour ShadeHit(const HitRecord& hitRecord) const {
    Colour color = PredefinedColours::BLACK;
    // supporting multiple light sources
    for (int i = 0; i < lights.size(); i++) {
      bool isInShadow = IsShadowed(hitRecord.pointOverSurface, lights[i]);
      color += lighting(hitRecord.shapePtr->GetMaterial(), lights[i],
                        hitRecord.pointOverSurface, hitRecord.eyeV,
                        hitRecord.normalV, isInShadow);
    }
    return color;
  }

  constexpr bool IsShadowed(const Tuple& point, const PointLight& light) const {
    const Tuple v =
        (light.position - point);  // direction vector from point to light
    const double distance = v.Magnitude();
    const Tuple direction = ToNormalizedVector(v);
    const Ray shadowRay = Ray(point, direction);

    // Cast a shadow ray to see if it intersects anything.
    const auto xs = this->IntersectWithRay(shadowRay);

    // find the hit from the resulting intersections
    const auto I = IntersectionUtils::VisibleHit(xs);

    return I.has_value() && I.value().GetIntersectDistance() < distance;
  }

  constexpr Colour ColorAt(const Ray& ray) const {
    // find the intersections with given ray
    const auto xs = this->IntersectWithRay(ray);
    // find the hit from the resulting intersections
    const auto I = IntersectionUtils::VisibleHit(xs);
    // return the color black if there is no such intersection
    if (!I.has_value() || !I.value().shapePtr || I == std::nullopt)
      return PredefinedColours::BLACK;
    // otherwise, precompute the necessary values
    const auto hitRecord = I.value().PrepareComputation(ray);
    // find the color at the hit
    return ShadeHit(hitRecord);
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
  PointLight light = PointLight(MakePoint(-10, 10, -10), MakeColour(1, 1, 1));
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
  const std::size_t numXS = NumXSOf<Sphere, Sphere>::numXS;

  World<decltype(shapes), decltype(lights), numXS> defaultWorld{
      std::move(shapes), std::move(lights)};
  return defaultWorld;
}
}  // namespace WorldUtils
}  // namespace RayTracer

#endif
