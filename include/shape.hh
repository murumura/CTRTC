#ifndef SHAPE_HH
#define SHAPE_HH
#include <concepts>
#include <optional>
#include <primitives/matrix.hh>
#include <primitives/static_base.hh>
#include <primitives/static_vector.hh>
#include <ray.hh>
#include <shading.hh>
#include <utils/math.hh>
#include <variant>
namespace RayTracer {

enum ShapeType { SphereTag, PlaneTag, None };

class ShapeWrapper;

struct HitRecord {
  float t;
  const ShapeWrapper* shapePtr{nullptr};
  Tuple eyeV;
  Tuple point;
  Tuple pointOverSurface;
  Tuple normalV;
  bool inside;
};

class Intersection {
 public:
  float t{0.f};
  ShapeType shapeType{None};
  const ShapeWrapper* shapePtr{nullptr};

  constexpr Intersection() noexcept = default;

  constexpr Intersection(float t_, const ShapeWrapper* shapePtr_)
      : t{t_}, shapePtr{shapePtr_} {}

  constexpr Intersection(const Intersection& other)
      : t(other.t), shapeType(other.shapeType), shapePtr(other.shapePtr) {}

  constexpr Intersection(Intersection&& other)
      : t(std::exchange(other.t, 0)),
        shapeType(std::exchange(other.shapeType, ShapeType::None)),
        shapePtr(std::exchange(other.shapePtr, nullptr)) {}

  constexpr Intersection& operator=(const Intersection& other) noexcept {
    shapePtr = other.shapePtr;
    shapeType = other.shapeType;
    t = other.t;
    return *this;
  }

  constexpr Intersection& operator=(Intersection&& other) noexcept {
    shapePtr = std::exchange(other.shapePtr, nullptr);
    shapeType = std::exchange(other.shapeType, ShapeType::None);
    t = std::exchange(other.t, 0);
    return *this;
  }
  constexpr float GetIntersectDistance() const { return t; }
  constexpr HitRecord PrepareComputation(const Ray& ray) const;

  constexpr ShapeType GetShapeType() const;
};

using IntxnRetVariant =
    std::variant<StaticVector<Intersection, 2>, StaticVector<Intersection, 1>>;

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
    std::same_as<
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
    std::same_as<
        typename IntersectionList::value_type,
        Intersection>) constexpr auto SortIntersections(IntersectionList&
                                                            intersections) noexcept {
  auto result = std::array{intersections};
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
requires(std::same_as<typename IntersectionList::value_type, Intersection>)
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

[[nodiscard]] constexpr std::optional<Intersection> VisibleHitFromVariant(
    const IntxnRetVariant& variant) noexcept {
  auto visitor = [&](auto&& v) {
    using T = PrimitiveTraits::RemoveCVR<decltype(v)>;
    if constexpr (std::same_as<T, StaticVector<Intersection, 2>>) {
      const auto xs = std::get<StaticVector<Intersection, 2>>(variant);
      const auto ret = VisibleHit(xs);
      return ret;
    } else if constexpr (std::same_as<T, StaticVector<Intersection, 1>>) {
      const auto xs = std::get<StaticVector<Intersection, 1>>(variant);
      const auto ret = VisibleHit(xs);
      return ret;
    }
  };
  const auto ret = std::visit(visitor, variant);
  return ret;
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

  constexpr IntxnRetVariant LocalIntersection(
      const Ray& ray, const ShapeWrapper* ptrSelf) const {
    return this->derived().LocalIntersection(ray, ptrSelf);
  }

  constexpr IntxnRetVariant IntersectWith(
      const Ray& ray, const ShapeWrapper* ptrSelf) const noexcept {
    const auto invTransform = Inverse(transform);
    const auto tranformedRay = ray.Transform(invTransform);
    return LocalIntersection(tranformedRay, ptrSelf);
  }

  constexpr ShapeType GetShapeType() const {
    return this->derived().GetShapeType();
  }

  constexpr void SetMaterial(const Material& material_) {
    material = material_;
  }
  constexpr Material GetMaterial() const { return material; }

  constexpr Transform GetTransform() const { return transform; }

  constexpr Tuple LocalNormalAt(const Tuple& point) const {
    return this->derived().LocalNormalAt();
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

struct PlaneTrait {
  using return_type = StaticVector<Intersection, 1>;
  static constexpr std::size_t NumIntersections = 1;
  static constexpr std::size_t ReturnIndex = 0;
};

struct SphereTrait {
  using return_type = StaticVector<Intersection, 2>;
  static constexpr std::size_t NumIntersections = 2;
  static constexpr std::size_t ReturnIndex = 0;
};

}  // namespace ShapeTraits

class Plane : public Shape<Plane> {
 public:
  constexpr ShapeType GetShapeType() const { return PlaneTag; }

  constexpr Tuple LocalNormalAt(const Tuple& point) const {
    return MakeVector(0, 1, 0);
  }

  constexpr IntxnRetVariant LocalIntersection(
      const Ray& ray, const ShapeWrapper* ptrSelf) const noexcept {
    const float yDirection = ray.GetDirection()[TupleConstants::y];
    if (MathUtils::ConstExprAbsf(yDirection) < EPSILON)
      return IntxnRetVariant(
          StaticVector<Intersection, 1>{Intersection(-1, nullptr)});
    else {
      const float t = (-ray.GetOrigin()[TupleConstants::y]) / yDirection;
      return IntxnRetVariant(
          StaticVector<Intersection, 1>{Intersection(t, ptrSelf)});
    }
  }

  template <typename OtherType>
  constexpr friend bool operator==(const Plane& lhs, const OtherType& rhs) {
    return lhs.GetShapeType() == rhs.GetShapeType() &&
           lhs.GetTransform() == rhs.GetTransform() &&
           lhs.GetMaterial() == rhs.GetMaterial();
  }
};

class Sphere : public Shape<Sphere> {
 public:
  constexpr ShapeType GetShapeType() const { return SphereTag; }

  constexpr Tuple LocalNormalAt(const Tuple& point) const {
    return ToNormalizedVector(point - PredefinedTuples::ZeroPoint);
  }

  constexpr IntxnRetVariant LocalIntersection(
      const Ray& ray, const ShapeWrapper* ptrSelf) const noexcept {
    // Caculate distance between sphere to ray origin points assuming sphere's center located at world origin.
    const auto sphereToRay = ray.GetOrigin() - PredefinedTuples::ZeroPoint;
    const auto rayDir = ray.GetDirection();
    const auto a = rayDir.DotProduct(rayDir);
    const auto b = 2 * rayDir.DotProduct(sphereToRay);
    const auto c = sphereToRay.DotProduct(sphereToRay) - 1;

    if (const auto& res = MathUtils::SolveQuadratic(a, b, c)) {
      const auto& [r1, r2] = res.value();
      return IntxnRetVariant(StaticVector<Intersection, 2>{
          Intersection(r1, ptrSelf), Intersection(r2, ptrSelf)});
    } else  // Currently a workaround for non-hitting, TODO: account for a more elegent solution in the future
      return IntxnRetVariant(StaticVector<Intersection, 2>{
          Intersection(-1, nullptr), Intersection(-1, nullptr)});
  }

  template <typename OtherType>
  constexpr friend bool operator==(const Sphere& lhs, const OtherType& rhs) {
    return lhs.GetShapeType() == rhs.GetShapeType() &&
           lhs.GetTransform() == rhs.GetTransform() &&
           lhs.GetMaterial() == rhs.GetMaterial();
  }
};

class ShapeWrapper {
 public:
  template <typename T>
  constexpr ShapeWrapper(T&& elem) : shapeObject{std::forward<T>(elem)} {}

  constexpr bool operator==(ShapeWrapper&& other) const {
    return std::visit([&](auto const& elem) -> bool { return elem == other; },
                      shapeObject);
  }

  constexpr ShapeType GetShapeType() const {
    return std::visit(
        [&](auto const& elem) -> decltype(auto) { return elem.GetShapeType(); },
        shapeObject);
  }

  constexpr Material GetMaterial() const {
    return std::visit(
        [&](auto const& elem) -> decltype(auto) { return elem.GetMaterial(); },
        shapeObject);
  }

  constexpr Transform GetTransform() const {
    return std::visit(
        [&](auto const& elem) -> decltype(auto) { return elem.GetTransform(); },
        shapeObject);
  }

  constexpr Tuple GetWorldNormalAt(const Tuple& worldPoint) const {
    return std::visit(
        [&](auto const& elem) -> decltype(auto) {
          return elem.WorldNormalAt(worldPoint);
        },
        shapeObject);
  }

  constexpr IntxnRetVariant IntersectWith(const Ray& ray) const {
    return std::visit(
        [&](auto const& elem) { return elem.IntersectWith(ray, this); },
        shapeObject);
  }

  std::variant<Sphere, Plane> shapeObject;
};

constexpr ShapeType Intersection::GetShapeType() const {
  return shapePtr->GetShapeType();
}

constexpr HitRecord Intersection::PrepareComputation(const Ray& ray) const {
  HitRecord returnRec{};
  returnRec.t = t;
  returnRec.shapePtr = shapePtr;
  returnRec.point = ray.PositionAlong(GetIntersectDistance());
  returnRec.eyeV = -ray.GetDirection();
  const auto normalV = shapePtr->GetWorldNormalAt(returnRec.point);
  returnRec.inside = returnRec.normalV.DotProduct(returnRec.eyeV) < 0;
  // The normal is inverted when the hit occurs inside the object
  returnRec.normalV = returnRec.inside ? -normalV : normalV;
  returnRec.pointOverSurface = returnRec.point + (returnRec.normalV * EPSILON);
  return returnRec;
}

}  // namespace RayTracer

#endif
