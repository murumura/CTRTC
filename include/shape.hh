#ifndef SHAPE_HH
#define SHAPE_HH
#include <concepts>
#include <optional>
#include <primitives/static_base.hh>
#include <primitives/static_vector.hh>
#include <ray.hh>
#include <shading.hh>
#include <utils/math.hh>
#include <variant>
namespace RayTracer {

enum ShapeType { SphereTag, None };

class ShapeWrapper;

class Intersection {
 public:
  float t{0.f};
  ShapeType shapeType{SphereTag};
  const ShapeWrapper* shapePtr{nullptr};

  constexpr Intersection() noexcept = default;

  constexpr Intersection(float t_, const ShapeWrapper* shapePtr_)
      : t{t_}, shapePtr{shapePtr_} {}

  constexpr float GetIntersectDistance() const { return t; }

  constexpr ShapeType GetShapeType() const;
};

constexpr bool operator==(const Intersection& lhs, const Intersection& rhs) {
  return MathUtils::ApproxEqual(lhs.GetIntersectDistance(),
                                rhs.GetIntersectDistance()) &&
         lhs.shapePtr == rhs.shapePtr;
}

constexpr bool operator!=(const Intersection& lhs, const Intersection& rhs) {
  return !(lhs == rhs);
}

constexpr bool operator<(const Intersection& lhs, const Intersection& rhs) {
  // Comparison between negative intersections is undefined
  if (lhs.GetIntersectDistance() < 0)
    return false;
  return lhs.GetIntersectDistance() < rhs.GetIntersectDistance();
}

namespace IntersectionUtils {
template <typename... Args>
requires(
    std::is_same_v<
        std::decay_t<Args>,
        Intersection>&&...) constexpr auto AggregateIntersection(Args&&... args) {
  auto result = std::array{std::forward<Args>(args)...};
  std::sort(result.begin(), result.end(),
            [](const Intersection& lhs, const Intersection& rhs) -> bool {
              // ignore all intersection with negative t (hit will never be behind the ray's origin)
              if (lhs.GetIntersectDistance() < 0)
                return false;
              else if (rhs.GetIntersectDistance() < 0)
                return true;
              return lhs.GetIntersectDistance() < rhs.GetIntersectDistance();
            });
  return result;
}

template <typename IntersectionList>
requires(
    std::is_same_v<
        typename IntersectionList::value_type,
        Intersection>) constexpr void SortIntersections(IntersectionList&
                                                            intersections) noexcept {
  std::sort(intersections.begin(), intersections.end(),
            [](const Intersection& lhs, const Intersection& rhs) -> bool {
              // ignore all intersection with negative t (hit will never be behind the ray's origin)
              if (lhs.GetIntersectDistance() < 0)
                return false;
              else if (rhs.GetIntersectDistance() < 0)
                return true;
              return lhs.GetIntersectDistance() < rhs.GetIntersectDistance();
            });
}

template <typename IntersectionList>
requires(std::is_same_v<typename IntersectionList::value_type, Intersection>)
    [[nodiscard]] constexpr std::optional<Intersection> VisibleHit(
        IntersectionList intersections) noexcept {
  if (intersections.size() == 0)
    return std::nullopt;
  assert(std::is_sorted(
      intersections.begin(), intersections.end(),
      [](const Intersection& lhs, const Intersection& rhs) -> bool {
        if (lhs.GetIntersectDistance() < 0)
          return false;
        else if (rhs.GetIntersectDistance() < 0)
          return true;
        return lhs.GetIntersectDistance() < 0 &&
               lhs.GetIntersectDistance() < rhs.GetIntersectDistance();
      }));
  return intersections[0].GetIntersectDistance() < 0
             ? std::nullopt
             : std::optional{intersections[0]};
}

}  // namespace IntersectionUtils

template <typename T>
class Shape : public StaticBase<T, Shape> {
 public:
  constexpr Shape()
      : transform{PredefinedMatrices::I<double, 4>}, material{Material{}} {}

  constexpr Shape(const Transform& transform_) : transform{transform_} {}

  constexpr Shape(
      const Material& material_,
      const Transform& transform_ = PredefinedMatrices::I<double, 4>)
      : material{material_}, transform{transform_} {}

  constexpr std::optional<Intersection> LocalIntersection(
      const Ray& ray, const ShapeWrapper* ptrSelf) const {
    return this->derived().LocalIntersection(ray, ptrSelf);
  }

  constexpr std::optional<Intersection> IntersectWith(
      const Ray& ray, const ShapeWrapper* ptrSelf) const {
    return this->derived().IntersectWith(ray, ptrSelf);
  }

  constexpr ShapeType GetShapeType() const {
    return this->derived().GetShapeType();
  }

  constexpr Tuple LocalNormalAt(const Tuple& point) const {
    return ToNormalizedVector(point - PredefinedTuples::ZeroPoint);
  }

  constexpr Tuple WorldNormalAt(const Tuple& worldPoint) const {
    const auto objectPoint = Inverse(transform) * worldPoint;
    const auto objectNormal = objectPoint - PredefinedTuples::ZeroPoint;
    const auto worldNormal = Transpose(Inverse(transform)) * objectNormal;
    return ToNormalizedVector(worldNormal);
  }

  constexpr friend bool operator==(const Shape& lhs, const Shape& rhs) {
    if (lhs.GetShapeType() != rhs.GetShapeType())
      return false;
    return lhs.derived() == rhs.derived();
  }

  Transform transform{PredefinedMatrices::I<double, 4>};
  Material material{Material{}};
};

namespace ShapeTraits {

struct SphereTrait {
  using return_type = StaticVector<Intersection, 2>;
  static constexpr std::size_t NumIntersections = 2;
  static constexpr std::size_t ReturnIndex = 0;
};

}  // namespace ShapeTraits
class Sphere : public Shape<Sphere> {
 public:
  constexpr ShapeType GetShapeType() const { return SphereTag; }

  constexpr auto LocalIntersection(const Ray& ray,
                                   const ShapeWrapper* ptrSelf) const noexcept
      -> StaticVector<Intersection, 2> {
    // Caculate distance between sphere to ray origin points assuming sphere's center located at world origin.
    const auto sphereToRay = ray.GetOrigin() - PredefinedTuples::ZeroPoint;
    const auto rayDir = ray.GetDirection();
    const auto a = rayDir.DotProduct(rayDir);
    const auto b = 2 * rayDir.DotProduct(sphereToRay);
    const auto c = sphereToRay.DotProduct(sphereToRay) - 1;

    if (const auto& res = MathUtils::SolveQuadratic(a, b, c)) {
      const auto& [r1, r2] = res.value();
      return StaticVector<Intersection, 2>{Intersection(r1, ptrSelf),
                                           Intersection(r2, ptrSelf)};
    } else
      return StaticVector<Intersection, 2>();
  }

  constexpr auto IntersectWith(const Ray& ray,
                               const ShapeWrapper* ptrSelf) const noexcept
      -> StaticVector<Intersection, 2> {
    const auto invTransform = Inverse(transform);
    const auto tranformedRay = ray.Transform(invTransform);
    return LocalIntersection(tranformedRay, ptrSelf);
  }

  constexpr friend bool operator==(const Sphere& lhs, const Sphere& rhs) {
    return lhs.transform == rhs.transform && lhs.material == rhs.material;
  }
};

class ShapeWrapper {
 public:
  template <typename T>
  constexpr ShapeWrapper(T&& elem) : shapeObject{std::forward<T>(elem)} {}

  template <typename T>
  constexpr bool operator==(T&& other) const {
    return std::visit([&](auto const& elem) -> bool { return elem == other; },
                      shapeObject);
  }

  constexpr ShapeType GetShapeType() const {
    return std::visit(
        [&](auto const& elem) -> decltype(auto) { return elem.GetShapeType(); },
        shapeObject);
  }

  constexpr Tuple GetWorldNormalAt(const Tuple& worldPoint) const {
    return std::visit(
        [&](auto const& elem) -> decltype(auto) {
          return elem.WorldNormalAt(worldPoint);
        },
        shapeObject);
  }

  constexpr auto IntersectWith(const Ray& ray) const
      -> std::variant<StaticVector<Intersection, 2>,
                      StaticVector<Intersection, 1>> {
    return std::visit(
        [&](auto const& elem) { return elem.IntersectWith(ray, this); },
        shapeObject);
  }
  std::variant<Sphere> shapeObject;
};

constexpr ShapeType Intersection::GetShapeType() const {
  return shapePtr->GetShapeType();
}

}  // namespace RayTracer

#endif
