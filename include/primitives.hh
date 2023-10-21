#ifndef SHAPE_HH
#define SHAPE_HH
#include <concepts>
#include <optional>
#include <primitives/matrix.hh>
#include <primitives/static_base.hh>
#include <primitives/static_vector.hh>
#include <ray.hh>

#include <utils/math.hh>
#include <variant>

namespace RayTracer {

enum ShapeType { SphereTag, PlaneTag, None };
enum PatternType {
  StripeTag,
  GradientTag,
  TestTag,
  RingTag,
  CheckerTag,
  SolidTag,
  RadialTag,
  NestedTag,
  BlendedTag
};

class PatternWrapper;
class TestPattern;
class StridePattern;
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

/*!
 * \brief An "midway" class responsible for wrapping different shapes's 
 * intersection result during rendering computation for unifying calling interface.
 * 
 * To account for different return type of intersection function by different shape objects.  
 * (check out `ShapesTraits` class for detail) And to account for the case where "no-hitting" occurs
 * the `IntxnRetVariant` could sometimes be empty, so any function that return a `IntxnRetVariant` 
 * should further calling `VisibleHitFromVariant` (which first determing its holding value, and
 * later calling `VisibleHit` and return most visible hits)
 * as a underlying return type dispatching function.
 */
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

/*!
 * \brief  Sort pack of `Intersection` from near to far and 
 * return them in form of array (if any).
 * 
 * \tparam Args Parameter pack of type `Intersection`
 * \return Array of sorting hits from near to far.
 * */
template <typename... Args>
requires(
    std::same_as<
        std::decay_t<Args>,
        Intersection>&&...) constexpr auto AggregateIntersections(Args&&... args) {
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

/*!
 * \brief  Sort collection of `Intersection` from near to far and 
 * return them in form of array (if any).
 * 
 * \tparam container with value_type defined as `Intersection`
 * \return Array of sorting hits from near to far.
 * */
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

/*!
 * \brief Filter most and return visible hits from a collection of `Intersection` object (if any).
 * \tparam IntersectionList container with value_type defined as `Intersection`
 * \return Either return the encapsulation information `Intersection` from most visible 
 * intersection point or nullopt if tracing ray totally missing from the shape object.
 * */
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

/*!
 * \brief Filter most visible hits from `IntxnRetVariant` object 
 * \return Either return the encapsulation information `Intersection` from most visible 
 * intersection point or nullopt if tracing ray totally missing from the shape object.
 * */
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

template <typename FT = double>
requires(std::is_floating_point_v<
         PrimitiveTraits::RemoveCVR<FT>>) class PerlinNoise {
 public:
  using T = PrimitiveTraits::RemoveCVR<FT>;
  static constexpr std::array<uint8_t, 256> Permutation = {
      151, 160, 137, 91,  90,  15,  131, 13,  201, 95,  96,  53,  194, 233, 7,
      225, 140, 36,  103, 30,  69,  142, 8,   99,  37,  240, 21,  10,  23,  190,
      6,   148, 247, 120, 234, 75,  0,   26,  197, 62,  94,  252, 219, 203, 117,
      35,  11,  32,  57,  177, 33,  88,  237, 149, 56,  87,  174, 20,  125, 136,
      171, 168, 68,  175, 74,  165, 71,  134, 139, 48,  27,  166, 77,  146, 158,
      231, 83,  111, 229, 122, 60,  211, 133, 230, 220, 105, 92,  41,  55,  46,
      245, 40,  244, 102, 143, 54,  65,  25,  63,  161, 1,   216, 80,  73,  209,
      76,  132, 187, 208, 89,  18,  169, 200, 196, 135, 130, 116, 188, 159, 86,
      164, 100, 109, 198, 173, 186, 3,   64,  52,  217, 226, 250, 124, 123, 5,
      202, 38,  147, 118, 126, 255, 82,  85,  212, 207, 206, 59,  227, 47,  16,
      58,  17,  182, 189, 28,  42,  223, 183, 170, 213, 119, 248, 152, 2,   44,
      154, 163, 70,  221, 153, 101, 155, 167, 43,  172, 9,   129, 22,  39,  253,
      19,  98,  108, 110, 79,  113, 224, 232, 178, 185, 112, 104, 218, 246, 97,
      228, 251, 34,  242, 193, 238, 210, 144, 12,  191, 179, 162, 241, 81,  51,
      145, 235, 249, 14,  239, 107, 49,  192, 214, 31,  181, 199, 106, 157, 184,
      84,  204, 176, 115, 121, 50,  45,  127, 4,   150, 254, 138, 236, 205, 93,
      222, 114, 67,  29,  24,  72,  243, 141, 128, 195, 78,  66,  215, 61,  156,
      180};

  /*!
  * \brief Calculate gradient using Perlin noise hash.
  */
  [[nodiscard]] inline constexpr T Grad(const std::uint8_t hash, const T x,
                                        const T y, const T z) const noexcept {
    const std::uint8_t h = hash & 15;
    const T u = h < 8 ? x : y;
    const T v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
  }

  inline constexpr int32_t FastFloor(const T fp) const {
    int32_t i = static_cast<int32_t>(fp);
    return (fp < i) ? (i - 1) : (i);
  }

  [[nodiscard]] inline constexpr uint8_t Hash(const int32_t i) const {
    return Permutation[static_cast<uint8_t>(i)];
  }

  constexpr Tuple operator()(const Tuple& point) const {
    const T pX = point[TupleConstants::x];
    const T pY = point[TupleConstants::y];
    const T pZ = point[TupleConstants::z];
    T n0, n1, n2, n3;

    constexpr T F3 = T(1.0 / 3.0);
    constexpr T G3 = T(1.0 / 6.0);

    // Very nice and simple skew factor for 3D
    T jitter = (pX + pY + pZ) * F3;
    int32_t i = FastFloor(pX + jitter);
    int32_t j = FastFloor(pY + jitter);
    int32_t k = FastFloor(pZ + jitter);
    T t = (i + j + k) * G3;
    // Unskew the cell origin back to (pX, pY, pZ) space
    T X0 = i - t;
    T Y0 = j - t;
    T Z0 = k - t;
    // The pX, pY, pZ distances from the cell origin
    T x0 = pX - X0;
    T y0 = pY - Y0;
    T z0 = pZ - Z0;
    // Offsets for second corner of simplex in (i,j,k) coords
    int i1, j1, k1;
    // Offsets for third corner of simplex in (i,j,k) coords
    int i2, j2, k2;

    // clang-format off
    if (x0 >= y0) {
      if (y0 >= z0) {
        // X Y Z order
        i1 = 1; j1 = 0; k1 = 0;
        i2 = 1; j2 = 1; k2 = 0;  
      } else if (x0 >= z0) {
        // X Z Y order
        i1 = 1; j1 = 0; k1 = 0;
        i2 = 1; j2 = 0; k2 = 1;  
      } else {
        // Z X Y order
        i1 = 0; j1 = 0; k1 = 1;
        i2 = 1; j2 = 0; k2 = 1;  
      }
    } else {  // x0<y0
      if (y0 < z0) {
        // Z Y X order
        i1 = 0; j1 = 0; k1 = 1;
        i2 = 0; j2 = 1; k2 = 1;  
      } else if (x0 < z0) {
        // Y Z X order
        i1 = 0; j1 = 1; k1 = 0;
        i2 = 0; j2 = 1; k2 = 1;  
      } else {
        // Y X Z order
        i1 = 0; j1 = 1; k1 = 0;
        i2 = 1; j2 = 1; k2 = 0;  
      }
    }
    // clang-format on

    // Offsets for second corner in (x,y,z) coords
    T x1 = x0 - T(i1) + G3;
    T y1 = y0 - T(j1) + G3;
    T z1 = z0 - T(k1) + G3;
    // Offsets for third corner in (x,y,z) coords
    T x2 = x0 - T(i2) + T(2.0) * G3;
    T y2 = y0 - T(j2) + T(2.0) * G3;
    T z2 = z0 - T(k2) + T(2.0) * G3;
    // Offsets for last corner in (x,y,z) coords
    T x3 = x0 - T(1.0) + T(3.0) * G3;
    T y3 = y0 - T(1.0) + T(3.0) * G3;
    T z3 = z0 - T(1.0) + T(3.0) * G3;

    uint8_t gi0 = Hash(i + Hash(j + Hash(k)));
    uint8_t gi1 = Hash(i + i1 + Hash(j + j1 + Hash(k + k1)));
    uint8_t gi2 = Hash(i + i2 + Hash(j + j2 + Hash(k + k2)));
    uint8_t gi3 = Hash(i + 1 + Hash(j + 1 + Hash(k + 1)));

    // Calculate the contribution from the four corners
    T t0 = 0.6f - x0 * x0 - y0 * y0 - z0 * z0;
    if (t0 < 0) {
      n0 = 0.0;
    } else {
      t0 *= t0;
      n0 = t0 * t0 * Grad(gi0, x0, y0, z0);
    }
    T t1 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1;
    if (t1 < 0) {
      n1 = 0.0;
    } else {
      t1 *= t1;
      n1 = t1 * t1 * Grad(gi1, x1, y1, z1);
    }
    T t2 = 0.6f - x2 * x2 - y2 * y2 - z2 * z2;
    if (t2 < 0) {
      n2 = 0.0;
    } else {
      t2 *= t2;
      n2 = t2 * t2 * Grad(gi2, x2, y2, z2);
    }
    T t3 = 0.6f - x3 * x3 - y3 * y3 - z3 * z3;
    if (t3 < 0) {
      n3 = 0.0;
    } else {
      t3 *= t3;
      n3 = t3 * t3 * Grad(gi3, x3, y3, z3);
    }

    const T distortion = T(32.0) * (n0 + n1 + n2 + n3);
    return MakePoint(pX + distortion, pY + distortion, pZ + distortion);
  }
};

/*!
 * \brief Perturb the given point using a jitter function if callable, 
 *        otherwise return the original point.
 * 
 * \tparam JitterFn Type of the jitter function.
 */
template <typename JitterFn>
[[nodiscard]] constexpr Tuple JitterPoint(JitterFn&& jitterFn,
                                          const Tuple& point) {
  if constexpr (PrimitiveTraits::InvocableWithRetType<Tuple, JitterFn,
                                                      const Tuple&>) {
    return jitterFn(point);
  } else {
    return point;
  }
}

template <typename T>
class Pattern : public StaticBase<T, Pattern> {
 public:
  constexpr Pattern()
      : transform{PredefinedMatrices::I<double, 4>},
        colourA{PredefinedColours::WHITE},
        colourB{PredefinedColours::BLACK} {}

  constexpr Pattern(const Transform& transform_)
      : transform{transform_},
        colourA{PredefinedColours::WHITE},
        colourB{PredefinedColours::BLACK} {}

  [[nodiscard]] constexpr Colour StrideAt(const Tuple& point) const {
    return this->derived().StrideAt(point);
  }

  /*!
  * \brief Return the color for the given pattern, on the given object at the
  * given world-space point, and it should respect the transformations on both the
  * pattern and the object while doing so.
  * 
  * \param object the shape object for evaluating stride colour with respect to its transformation.
  * \param world point the stride point in world-coordinate for evaluating stride colour 
  * \return colour of stride point
  */
  [[nodiscard]] constexpr Colour StrideAtObject(const ShapeWrapper& object,
                                                const Tuple& worldPoint) const;

  constexpr PatternType GetPatternType() const {
    return this->derived().GetPatternType();
  }

  constexpr void SetTransform(const Transform& transform_) {
    transform = transform_;
  }

  constexpr void SetColourA(const Colour& colourA_) { colourA = colourA_; }

  constexpr void SetColourB(const Colour& colourB_) { colourB = colourB_; }

  constexpr Transform GetTransform() const { return transform; }

  Transform transform{PredefinedMatrices::I<double, 4>};
  Colour colourA{PredefinedColours::WHITE}, colourB{PredefinedColours::BLACK};
};

class StridePattern : public Pattern<StridePattern> {
 public:
  constexpr PatternType GetPatternType() const { return StripeTag; }

  [[nodiscard]] constexpr Colour StrideAt(const Tuple& point) const {
    if (MathUtils::ApproxEqual(
            MathUtils::Modulo(MathUtils::Floor(point[TupleConstants::x]), 2.0),
            0.0))
      return colourA;
    else
      return colourB;
  }
};

class TestPattern : public Pattern<TestPattern> {
 public:
  constexpr PatternType GetPatternType() const { return TestTag; }

  [[nodiscard]] constexpr Colour StrideAt(const Tuple& point) const {
    return MakeColour(point[TupleConstants::x], point[TupleConstants::y],
                      point[TupleConstants::z]);
  }
};

class GradientPattern : public Pattern<GradientPattern> {
 public:
  constexpr PatternType GetPatternType() const { return GradientTag; }

  [[nodiscard]] constexpr Colour StrideAt(const Tuple& point) const {
    // compute distance between the two colors
    const Colour delta = colourB - colourA;
    // compute the fractional portion of the x coordinate
    const auto fraction =
        point[TupleConstants::x] - MathUtils::Floor(point[TupleConstants::x]);
    // linearly interpolating from one to the other as the x coordinate changes
    return colourA + (delta * fraction);
  }
};

class RingPattern : public Pattern<RingPattern> {
 public:
  constexpr PatternType GetPatternType() const { return RingTag; }

  [[nodiscard]] constexpr Colour StrideAt(const Tuple& point) const {
    const auto pXSquare = point[TupleConstants::x] * point[TupleConstants::x];
    const auto pZSquare = point[TupleConstants::z] * point[TupleConstants::z];
    const double magnitude = MathUtils::ConstExprSqrtf(pXSquare + pZSquare);
    if (MathUtils::ApproxEqual(
            MathUtils::Modulo(MathUtils::Floor(magnitude), 2.0), 0.0))
      return colourA;
    else
      return colourB;
  }
};

class CheckerPattern : public Pattern<CheckerPattern> {
 public:
  constexpr PatternType GetPatternType() const { return CheckerTag; }

  [[nodiscard]] constexpr Colour StrideAt(const Tuple& point) const {
    const auto pX = MathUtils::Floor(point[TupleConstants::x]);
    const auto pY = MathUtils::Floor(point[TupleConstants::y]);
    const auto pZ = MathUtils::Floor(point[TupleConstants::z]);
    if (MathUtils::ApproxEqual(
            MathUtils::Modulo(MathUtils::Floor(pX + pY + pZ), 2.0), 0.0))
      return colourA;
    else
      return colourB;
  }
};

class SolidPattern : public Pattern<SolidPattern> {
 public:
  constexpr PatternType GetPatternType() const { return SolidTag; }

  [[nodiscard]] constexpr Colour StrideAt(const Tuple& point) const {
    return colourA;
  }
};

class RadialGradientPattern : public Pattern<RadialGradientPattern> {
 public:
  constexpr PatternType GetPatternType() const { return RadialTag; }

  [[nodiscard]] constexpr Colour StrideAt(const Tuple& point) const {
    // compute distance between the two colors
    const Colour delta = colourB - colourA;
    const auto pXSquare = point[TupleConstants::x] * point[TupleConstants::x];
    const auto pZSquare = point[TupleConstants::z] * point[TupleConstants::z];
    const double magnitude = MathUtils::ConstExprSqrtf(pXSquare + pZSquare);
    const Colour radialColor = colourA + (delta * magnitude);
    // make sure color in valid range
    if (!IsValidColour(radialColor)) {
      return ToValidColour(radialColor);
    } else
      return radialColor;
  }
};

class BlendedPattern : public Pattern<BlendedPattern> {
 public:
  constexpr PatternType GetPatternType() const { return BlendedTag; }

  [[nodiscard]] constexpr Colour StrideAt(const Tuple& point) const {
    return (colourA + colourB) * 0.5;
  }
};

class PatternWrapper {
 public:
  template <typename T>
  constexpr PatternWrapper(T&& elem) : patternObject{std::forward<T>(elem)} {}

  constexpr PatternType GetPatternType() const {
    return std::visit(
        [&](auto const& elem) -> decltype(auto) {
          return elem.GetPatternType();
        },
        patternObject);
  }

  /*!
  * \brief Return the color for PatternWrapper's underlying pattern type, 
  * on the given object at the given point coordinate.
  * 
  * \note Assume underlying pattern type of PatternWrapper implements its own 
  * `StrideAt` function.
  * 
  * \param point the stride point coordinate for evaluating stride colour 
  * \return colour of stride point
  */
  [[nodiscard]] constexpr Colour StrideAt(const Tuple& point) const {
    return std::visit(
        [&](auto const& elem) -> decltype(auto) {
          return elem.StrideAt(point);
        },
        patternObject);
  }

  constexpr Transform GetTransform() const {
    return std::visit(
        [&](auto const& elem) -> decltype(auto) { return elem.GetTransform(); },
        patternObject);
  }

  /*!
  * \brief Return the color for PatternWrapper's underlying pattern type, 
  * on the given object at the given world-space point, and it should respect 
  * the transformations on both the pattern and the object while doing so.
  * 
  * \note Assume underlying pattern type of PatternWrapper implements its own 
  * `StrideAtObject` function.
  * 
  * \param object the shape object for evaluating stride colour with respect to its transformations 
  * \param world point the stride point in world-coordinate for evaluating stride colour 
  * \return colour of stride point
  */
  [[nodiscard]] constexpr Colour StrideAtObject(const ShapeWrapper& object,
                                                const Tuple& worldPoint) const;

  // Underlying pattern type of PatternWrapper
  std::variant<StridePattern, GradientPattern, TestPattern, RingPattern,
               CheckerPattern, SolidPattern, RadialGradientPattern,
               BlendedPattern>
      patternObject;
};

class PointLight {
 public:
  Tuple position;
  Colour intensity;
  constexpr PointLight(const Tuple& position_,
                       const Colour& intensity_) noexcept
      : position{position_}, intensity{intensity_} {}

  constexpr friend bool operator==(const PointLight& lhs,
                                   const PointLight& rhs) noexcept {
    // clang-format off
      return lhs.position == rhs.position && 
        lhs.intensity == rhs.intensity ;
    // clang-format on
  }
};

class Material {
 public:
  Colour color{PredefinedColours::WHITE};
  double ambient{0.1};
  double diffuse{0.9};
  double specular{0.9};
  double shininess{200.0};

  const PatternWrapper* patternPtr{nullptr};

  constexpr bool HasPattern() const {
    if ((patternPtr == nullptr) || (patternPtr->GetPatternType() == TestTag))
      return false;
    return true;
  }

  constexpr friend bool operator==(const Material& lhs,
                                   const Material& rhs) noexcept {
    // clang-format off
            return lhs.color == rhs.color && 
                lhs.ambient == rhs.ambient && 
                lhs.diffuse == rhs.diffuse && 
                lhs.specular == rhs.specular && 
                lhs.shininess == rhs.shininess;
    // clang-format on
  }
};

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
    const double yDirection = ray.GetDirection()[TupleConstants::y];
    if (MathUtils::ConstExprAbsf(yDirection) < EPSILON)
      return IntxnRetVariant(
          StaticVector<Intersection, 1>{Intersection(-1, nullptr)});
    else {
      const double t = (-ray.GetOrigin()[TupleConstants::y]) / yDirection;
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

template <typename T>
constexpr Colour Pattern<T>::StrideAtObject(const ShapeWrapper& object,
                                            const Tuple& worldPoint) const {
  const auto invObjectTrans = Inverse(object.GetTransform());
  const auto invPatternTrans = Inverse(this->GetTransform());
  const Tuple objectPoint = invObjectTrans * worldPoint;
  const Tuple patternPoint = invPatternTrans * objectPoint;
  return this->StrideAt(patternPoint);
}

constexpr Colour PatternWrapper::StrideAtObject(const ShapeWrapper& object,
                                                const Tuple& worldPoint) const {
  return std::visit(
      [&](auto const& elem) -> decltype(auto) {
        return elem.StrideAtObject(object, worldPoint);
      },
      patternObject);
}

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

[[nodiscard]] constexpr Colour Lighting(const Material& material,
                                        const ShapeWrapper& object,
                                        const PointLight& light,
                                        const Tuple& point, const Tuple& eye,
                                        const Tuple& normal,
                                        const bool inShadow = false) noexcept {
  // get the color from the pattern (via StrideAtObject())
  // if the material has associate pattern
  const Colour materialColour =
      material.HasPattern() ? material.patternPtr->StrideAtObject(object, point)
                            : material.color;

  // combine the surface color with the light's color/intensity
  const Colour effectiveColour = materialColour * light.intensity;

  // find the direction to the light source
  const Tuple lightDirection = ToNormalizedVector(light.position - point);

  // compute the ambient contribution
  const Colour ambient = effectiveColour * material.ambient;

  // when the point is in shadow use only the ambient component
  if (inShadow)
    return ambient;

  // light_dot_normal represent the cosine of the angle between the
  // light vector and the normal vector.
  // A negative number means the light is on the other side of the surface
  const double lightDotNormal = lightDirection.DotProduct(normal);

  // precompute for ambient contribution
  const auto reflectv = -lightDirection.Reflect(normal);
  const double reflectDotEye = reflectv.DotProduct(eye);

  // compute the diffuse contribution
  const auto diffuse =
      (lightDotNormal < 0)
          ? PredefinedColours::BLACK
          : effectiveColour * material.diffuse * lightDotNormal;

  const auto specular =
      (lightDotNormal < 0 || reflectDotEye <= 0)
          ? PredefinedColours::BLACK
          : light.intensity * material.specular *
                MathUtils::ConstExprExp(reflectDotEye, material.shininess);

  return ambient + diffuse + specular;
}

}  // namespace RayTracer

#endif
