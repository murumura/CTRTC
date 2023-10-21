#ifndef WORLD_HH
#define WORLD_HH
#include <concepts>
#include <primitives.hh>
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

template <typename ShapeContType, typename LightContType, std::size_t NXs = 10>
requires requires(ShapeContType shapeArgs, LightContType lightArgs) {
  // number of possible intersections should > 0 for valid ray intersection result
  requires NXs > 0;
  typename LightContType::difference_type;
  typename ShapeContType::difference_type;
  typename LightContType::size_type;
  typename ShapeContType::size_type;
  { shapeArgs.size() } -> std::same_as<typename ShapeContType::size_type>;
  { lightArgs.size() } -> std::same_as<typename LightContType::size_type>;
}
class World {
 public:
  constexpr World(ShapeContType&& shapeArgs, LightContType&& lightArgs)
      : shapes{std::forward<ShapeContType>(shapeArgs)},
        lights{std::forward<LightContType>(lightArgs)} {}

  static constexpr std::size_t NumXS{NXs};

  constexpr auto IntersectWithRay(const Ray& ray) const
      -> std::array<Intersection, NumXS> {
    std::array<Intersection, NumXS> ret = {};
    std::size_t iter = 0;
    for (int i = 0; i < shapes.size(); i++) {
      const auto variant = shapes[i].IntersectWith(ray);
      auto visitor = [&](auto&& v) {
        using T = PrimitiveTraits::RemoveCVR<decltype(v)>;
        if constexpr (std::same_as<T, StaticVector<Intersection, 2>>) {
          const auto xs = std::get<StaticVector<Intersection, 2>>(variant);
          for (std::size_t i = 0; i < 2; i++)
            ret[iter++] = std::move(xs[i]);
        } else if constexpr (std::same_as<T, StaticVector<Intersection, 1>>) {
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
      color += Lighting(hitRecord.shapePtr->GetMaterial(), *hitRecord.shapePtr,
                        lights[i], hitRecord.pointOverSurface, hitRecord.eyeV,
                        hitRecord.normalV, isInShadow);
    }
    return color;
  }

  constexpr bool IsShadowed(const Tuple& point, const PointLight& light) const {
    // direction vector from point to light
    const Tuple v = (light.position - point);
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

  constexpr ShapeContType const& GetShapes() const { return shapes; }

  constexpr LightContType const& GetLights() const { return lights; }

  ShapeContType shapes;
  LightContType lights;
};

namespace WorldUtils {

constexpr auto DefaultWorld() {
  PointLight light = PointLight(MakePoint(-10, 10, -10), MakeColour(1, 1, 1));
  const std::size_t numLights = 1;
  std::array<PointLight, numLights> lights = {light};
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
  const std::size_t numShapes = 2;
  std::array<ShapeWrapper, 2> shapes = {s1, s2};
  const std::size_t numXS = NumXSOf<Sphere, Sphere>::numXS;

  World<decltype(shapes), decltype(lights), numXS> defaultWorld{
      std::move(shapes), std::move(lights)};
  return defaultWorld;
}
}  // namespace WorldUtils
}  // namespace RayTracer

#endif
