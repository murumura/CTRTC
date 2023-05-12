#ifndef VEC_HH
#define VEC_HH
#include <array>
#include <primitive_traits.hh>
#include <utils/math.hh>
namespace RayTracer {

struct TupleConstants {
  constexpr static int x = 0;
  constexpr static int y = 1;
  constexpr static int z = 2;
  constexpr static int w = 3;
  /** Here we follow the instructions in the book to regard flag=1 as points and flag=0 as vector */
  constexpr static double PointFlag = 1;
  constexpr static double VectorFlag = 0;
};

struct ColourConstants {
  constexpr static int r = 0;
  constexpr static int g = 1;
  constexpr static int b = 2;
  constexpr static int MaxValue = 255;
};

template <PrimitiveTraits::Arithmetic T, std::size_t N>
class Vec final {
 public:
  static_assert(N > 0, "vector must contain at least one element");
  using ValueType = T;
  static constexpr std::size_t Length = N;
  static constexpr std::size_t size() { return Length; }

  constexpr T& operator[](std::size_t index) {
    if (index >= Length) {
      throw std::out_of_range("out of range vector access");
    }
    return contents[index];
  }

  constexpr const T& operator[](std::size_t index) const {
    if (index >= Length) {
      throw std::out_of_range("out of range vector access");
    }
    return contents[index];
  }

  constexpr T DotProduct(const Vec& rhs) const noexcept {
    T result{};
    for (std::size_t i = 0; i < N; ++i) {
      result += contents[i] * rhs[i];
    }
    return result;
  }

  constexpr T Magnitude() const noexcept {
    return MathUtils::ConstExprSqrtf(DotProduct(*this));
  }

  constexpr Vec Normalize() const noexcept { return Vec{*this / Magnitude()}; }

  constexpr Vec CrossProduct(const Vec& rhs) const noexcept {
    static_assert(Length == 4,
                  "Cross product can only be performed on vector type");
    // clang-format off
    return Vec{contents[TupleConstants::y] * rhs[TupleConstants::z] - contents[TupleConstants::z] * rhs[TupleConstants::y],
               contents[TupleConstants::z] * rhs[TupleConstants::x] - contents[TupleConstants::x] * rhs[TupleConstants::z],
               contents[TupleConstants::x] * rhs[TupleConstants::y] - contents[TupleConstants::y] * rhs[TupleConstants::x],
               TupleConstants::VectorFlag};
    // clang-format on
  }

  constexpr Vec Reflect(const Vec& normal) const noexcept {
    return (*this) - normal * 2.0 * DotProduct(normal);
  }

  friend std::ostream& operator<<(std::ostream& stream,
                                  const Vec& vec) noexcept {
    stream << "Vector(";
    for (std::size_t i = 0; i < vec.Length; ++i)
      stream << vec[i] << (i == vec.Length - 1 ? "" : ", ");
    return stream << ')';
  }

  std::array<ValueType, Length> contents;
  // ValueType  contents[Length]; ///< @private
};

/*  Constructs scalar vector from its arguments, checking that all arguments are of
 *  the same type.
 */
template <typename... Args>
constexpr decltype(auto) CreateVector(Args... args) {
  return Vec{args...};
}

/* Deduction guide for uniform initialization */
template <typename T, typename... U>
requires(std::same_as<T, U>&&...) Vec(T, U...)->Vec<T, 1 + sizeof...(U)>;

/* Deduction guide for uniform initialization */
template <typename T, std::size_t N>
Vec(const T (&)[N]) -> Vec<T, N>;

/** Checks equality of two vectors.*/
template <typename T, std::size_t N>
constexpr bool operator==(const Vec<T, N>& vecA,
                          const Vec<T, N>& vecB) noexcept {
  for (std::size_t i = 0; i < Vec<T, N>::Length; ++i) {
    if (!MathUtils::ApproxEqual(vecA[i], vecB[i]))
      return false;
  }
  return true;
}

/** Checks inequality of two vectors.*/
template <typename T, std::size_t N>
constexpr bool operator!=(const Vec<T, N>& vecA,
                          const Vec<T, N>& vecB) noexcept {
  return !(vecA == vecB);
}

namespace VecUtils {
/* Applies a function elementwise between many vectors. */
template <typename Func, typename T, typename... Vectors,
          typename ResultType =
              std::invoke_result_t<Func, T, typename Vectors::ValueType...>,
          std::size_t N =
              PrimitiveTraits::AllSameValue_v<std::size_t, Vectors::Length...>>
constexpr Vec<ResultType, N> ElementWise(Func func, const Vec<T, N>& v,
                                         const Vectors&... vectors) noexcept {
  Vec<ResultType, N> opApplied = {};
  for (std::size_t i = 0; i < N; ++i)
    opApplied[i] = std::apply(func, std::forward_as_tuple(v[i], vectors[i]...));
  return opApplied;
}

/*
*  Generates a vector containing consecutive elements spaced by 1.
*/
template <std::size_t N, typename T = double>
constexpr Vec<T, N> SeqVec(T value = T()) {
  Vec<T, N> seq = {};
  for (std::size_t i = 0; i < N; ++i) {
    seq[i] = value;
    value += 1;
  }
  return seq;
}

/*
*  Generates a vector as a function of its index.
*/
template <std::size_t N, typename Func>
constexpr decltype(auto) Generate(Func&& func) {
  return ElementWise(func, SeqVec<N, std::size_t>());
}

}  // namespace VecUtils

/* Negate vector */

/** Numerical negative, element-wise. */
template <typename T, std::size_t N>
constexpr Vec<T, N> operator-(const Vec<T, N>& vec) noexcept {
  return VecUtils::ElementWise([](T x) { return -1.0 * x; }, vec);
}

/* Operator overloading for `+`/`+=`*/
template <typename T, std::size_t N>
constexpr Vec<T, N> operator+(const Vec<T, N>& vec, T scalar) noexcept {
  return VecUtils::ElementWise([scalar](T x) { return x + scalar; }, vec);
}

template <typename T, std::size_t N>
constexpr Vec<T, N> operator+(T scalar, const Vec<T, N>& vec) noexcept {
  return VecUtils::ElementWise([scalar](T x) { return scalar + x; }, vec);
}

template <typename T, std::size_t N>
constexpr Vec<T, N>& operator+=(Vec<T, N>& vec, T scalar) noexcept {
  for (std::size_t i = 0; i < N; ++i)
    vec[i] += scalar;
  return vec;
}

template <typename T, std::size_t N>
constexpr Vec<T, N> operator+(const Vec<T, N>& vecA,
                              const Vec<T, N>& vecB) noexcept {
  return VecUtils::ElementWise(std::plus<T>(), vecA, vecB);
}

template <typename T, std::size_t N>
constexpr Vec<T, N>& operator+=(Vec<T, N>& vecA,
                                const Vec<T, N>& vecB) noexcept {
  for (std::size_t i = 0; i < N; ++i)
    vecA[i] += vecB[i];
  return vecA;
}

/* Operator overloading for - / -=*/

template <typename T, std::size_t N>
constexpr Vec<T, N> operator-(const Vec<T, N>& vec, T scalar) noexcept {
  return VecUtils::ElementWise([scalar](T x) { return x - scalar; }, vec);
}

template <typename T, std::size_t N>
constexpr Vec<T, N>& operator-=(Vec<T, N>& vec, T scalar) noexcept {
  for (std::size_t i = 0; i < N; ++i)
    vec[i] -= scalar;
  return vec;
}

template <typename T, std::size_t N>
constexpr Vec<T, N> operator-(const Vec<T, N>& vecA,
                              const Vec<T, N>& vecB) noexcept {
  return VecUtils::ElementWise(std::minus<T>(), vecA, vecB);
}

template <typename T, std::size_t N>
constexpr Vec<T, N>& operator-=(Vec<T, N>& vecA,
                                const Vec<T, N>& vecB) noexcept {
  for (std::size_t i = 0; i < N; ++i)
    vecA[i] -= vecB[i];
  return vecA;
}

/* Operator overloading for `*` / `*=` */

template <typename T, std::size_t N>
constexpr Vec<T, N> operator*(const Vec<T, N>& vec, T scalar) noexcept {
  return VecUtils::ElementWise([scalar](T x) { return x * scalar; }, vec);
}

template <typename T, std::size_t N>
constexpr Vec<T, N> operator*(T scalar, const Vec<T, N>& vec) noexcept {
  return VecUtils::ElementWise([scalar](T x) { return scalar * x; }, vec);
}

template <typename T, std::size_t N>
constexpr Vec<T, N>& operator*=(Vec<T, N>& vec, T scalar) noexcept {
  for (std::size_t i = 0; i < N; ++i)
    vec[i] *= scalar;
  return vec;
}

template <typename T, std::size_t N>
constexpr Vec<T, N> operator*(const Vec<T, N>& vecA,
                              const Vec<T, N>& vecB) noexcept {
  return VecUtils::ElementWise(std::multiplies<T>(), vecA, vecB);
}

template <typename T, std::size_t N>
constexpr Vec<T, N>& operator*=(Vec<T, N>& vecA,
                                const Vec<T, N>& vecB) noexcept {
  for (std::size_t i = 0; i < N; ++i)
    vecA[i] *= vecB[i];
  return vecA;
}

/* Operator overloading for / and /= */

template <typename T, std::size_t N>
constexpr Vec<T, N> operator/(const Vec<T, N>& vec, T scalar) noexcept {
  return VecUtils::ElementWise([scalar](T x) { return x / scalar; }, vec);
}

template <typename T, std::size_t N>
constexpr Vec<T, N>& operator/=(Vec<T, N>& vec, T scalar) noexcept {
  for (std::size_t i = 0; i < N; ++i)
    vec[i] /= scalar;
  return vec;
}

template <typename T, std::size_t N>
constexpr Vec<T, N> operator/(const Vec<T, N>& vecA,
                              const Vec<T, N>& vecB) noexcept {
  return VecUtils::ElementWise(std::divides<T>(), vecA, vecB);
}

template <typename T, std::size_t N>
constexpr Vec<T, N>& operator/=(Vec<T, N>& vecA,
                                const Vec<T, N>& vecB) noexcept {
  for (std::size_t i = 0; i < N; ++i)
    vecA[i] /= vecB[i];
  return vecA;
}

/*
---------------------
Tuple specialization
---------------------
*/
using Tuple = Vec<double, 4>;

template <>
constexpr Tuple& operator*=(Tuple& tuple, const Tuple& rhs) noexcept;

template <>
constexpr Tuple operator/(const Tuple& tuple, double scalar) noexcept;

template <>
constexpr Tuple::ValueType Tuple::DotProduct(const Tuple& rhs) const noexcept;

constexpr bool IsPoint(const Tuple& tuple) {
  return tuple[TupleConstants::w] == TupleConstants::PointFlag;
}

constexpr bool IsVector(const Tuple& tuple) {
  return tuple[TupleConstants::w] == TupleConstants::VectorFlag;
}

constexpr Tuple MakePoint(double dx, double dy, double dz) {
  return Tuple{dx, dy, dz, TupleConstants::PointFlag};
}

constexpr Tuple MakeNormalizedPoint(double dx, double dy, double dz) {
  return Tuple{dx, dy, dz, TupleConstants::PointFlag}.Normalize();
}

constexpr Tuple MakeVector(double dx, double dy, double dz) {
  return Tuple{dx, dy, dz, TupleConstants::VectorFlag};
}

constexpr Tuple MakeNormalizedVector(double dx, double dy, double dz) {
  return Tuple{dx, dy, dz, TupleConstants::VectorFlag}.Normalize();
}

/*Need some speclization to make the flag unchanged*/

/*
  Operator overloading Tuple '+' / '+='
  Note:To follow the book's convention on point and vector flag,
  we omit the equality comparison on 4th dimension of tuple instance if and only if the value of
  4th dimension not belongs to one of TupleConstants::PointFlag, TupleConstants::VectorFlag
*/
template <>
constexpr bool operator==(const Tuple& tupleA, const Tuple& tupleB) noexcept {
  // clang-format off
        return 
            MathUtils::ApproxEqual(tupleA[TupleConstants::x], tupleB[TupleConstants::x]) &&
            MathUtils::ApproxEqual(tupleA[TupleConstants::y], tupleB[TupleConstants::y]) &&
            MathUtils::ApproxEqual(tupleA[TupleConstants::z], tupleB[TupleConstants::z]) &&
            (
                    tupleA[TupleConstants::w] == TupleConstants::VectorFlag || 
                    tupleA[TupleConstants::w] == TupleConstants::PointFlag ||
                    tupleB[TupleConstants::w] == TupleConstants::VectorFlag ||
                    tupleB[TupleConstants::w] == TupleConstants::PointFlag 
                    ?  MathUtils::ApproxEqual(tupleA[TupleConstants::w], tupleB[TupleConstants::w])
                    :  true
            )    
        ;
  // clang-format on
}

/* Negate Tuple */

/** Numerical negative, element-wise. */
template <>
constexpr Tuple operator-(const Tuple& tuple) noexcept {
  return Tuple{tuple[TupleConstants::x] * -1.0, tuple[TupleConstants::y] * -1.0,
               tuple[TupleConstants::z] * -1.0, tuple[TupleConstants::w]};
}

/* Operator overloading Tuple '+' / '+=' */

template <>
constexpr Tuple operator+(const Tuple& tuple, double scalar) noexcept {
  return Tuple{tuple[TupleConstants::x] + scalar,
               tuple[TupleConstants::y] + scalar,
               tuple[TupleConstants::z] + scalar, tuple[TupleConstants::w]};
}

template <>
constexpr Tuple operator+(double scalar, const Tuple& tuple) noexcept {
  return Tuple{tuple[TupleConstants::x] + scalar,
               tuple[TupleConstants::y] + scalar,
               tuple[TupleConstants::z] + scalar, tuple[TupleConstants::w]};
}

template <>
constexpr Tuple& operator+=(Tuple& tuple, double scalar) noexcept {
  for (std::size_t i = 0; i < 3; ++i)
    tuple[i] += scalar;
  return tuple;
}

template <>
constexpr Tuple operator+(const Tuple& tuple, const Tuple& rhs) noexcept {
  return Tuple{tuple[TupleConstants::x] + rhs[TupleConstants::x],
               tuple[TupleConstants::y] + rhs[TupleConstants::y],
               tuple[TupleConstants::z] + rhs[TupleConstants::z],
               tuple[TupleConstants::w]};
}

template <>
constexpr Tuple& operator+=(Tuple& tuple, const Tuple& rhs) noexcept {
  for (std::size_t i = 0; i < 3; ++i)
    tuple[i] += rhs[i];
  return tuple;
}

/* Operator overloading Tuple '-' / '-=' */

template <>
constexpr Tuple operator-(const Tuple& tuple, double scalar) noexcept {
  return Tuple{tuple[TupleConstants::x] - scalar,
               tuple[TupleConstants::y] - scalar,
               tuple[TupleConstants::z] - scalar, tuple[TupleConstants::w]};
}

template <>
constexpr Tuple& operator-=(Tuple& tuple, double scalar) noexcept {
  for (std::size_t i = 0; i < 3; ++i)
    tuple[i] -= scalar;
  return tuple;
}

template <>
constexpr Tuple operator-(const Tuple& tuple, const Tuple& rhs) noexcept {
  return Tuple{tuple[TupleConstants::x] - rhs[TupleConstants::x],
               tuple[TupleConstants::y] - rhs[TupleConstants::y],
               tuple[TupleConstants::z] - rhs[TupleConstants::z],
               tuple[TupleConstants::w]};
}

template <>
constexpr Tuple& operator-=(Tuple& tuple, const Tuple& rhs) noexcept {
  for (std::size_t i = 0; i < 3; ++i)
    tuple[i] -= rhs[i];
  return tuple;
}

/* Operator overloading Tuple '*' / '*=' */

template <>
constexpr Tuple operator*(const Tuple& tuple, double scalar) noexcept {
  return Tuple{tuple[TupleConstants::x] * scalar,
               tuple[TupleConstants::y] * scalar,
               tuple[TupleConstants::z] * scalar, tuple[TupleConstants::w]};
}

template <>
constexpr Tuple operator*(double scalar, const Tuple& tuple) noexcept {
  return Tuple{tuple[TupleConstants::x] * scalar,
               tuple[TupleConstants::y] * scalar,
               tuple[TupleConstants::z] * scalar, tuple[TupleConstants::w]};
}

template <>
constexpr Tuple& operator*=(Tuple& tuple, double scalar) noexcept {
  for (std::size_t i = 0; i < 3; ++i)
    tuple[i] *= scalar;
  return tuple;
}

template <>
constexpr Tuple operator*(const Tuple& tuple, const Tuple& rhs) noexcept {
  return Tuple{tuple[TupleConstants::x] * rhs[TupleConstants::x],
               tuple[TupleConstants::y] * rhs[TupleConstants::y],
               tuple[TupleConstants::z] * rhs[TupleConstants::z],
               tuple[TupleConstants::w]};
}

template <>
constexpr Tuple& operator*=(Tuple& tuple, const Tuple& rhs) noexcept {
  for (std::size_t i = 0; i < 3; ++i)
    tuple[i] *= rhs[i];
  return tuple;
}

/* Operator overloading Tuple '/' / '/=' */
template <>
constexpr Tuple operator/(const Tuple& tuple, double scalar) noexcept {
  return Tuple{tuple[TupleConstants::x] / scalar,
               tuple[TupleConstants::y] / scalar,
               tuple[TupleConstants::z] / scalar, tuple[TupleConstants::w]};
}

template <>
constexpr Tuple& operator/=(Tuple& tuple, double scalar) noexcept {
  for (std::size_t i = 0; i < 3; ++i)
    tuple[i] /= scalar;
  return tuple;
}

template <>
constexpr Tuple operator/(const Tuple& tuple, const Tuple& rhs) noexcept {
  return Tuple{tuple[TupleConstants::x] / rhs[TupleConstants::x],
               tuple[TupleConstants::y] / rhs[TupleConstants::y],
               tuple[TupleConstants::z] / rhs[TupleConstants::z],
               tuple[TupleConstants::w]};
}

template <>
constexpr Tuple& operator/=(Tuple& tuple, const Tuple& rhs) noexcept {
  for (std::size_t i = 0; i < 3; ++i)
    tuple[i] /= rhs[i];
  return tuple;
}

template <>
constexpr Tuple::ValueType Tuple::DotProduct(const Tuple& rhs) const noexcept {
  ValueType result{};
  result += ((contents[TupleConstants::x] * rhs[TupleConstants::x]) +
             (contents[TupleConstants::y] * rhs[TupleConstants::y]) +
             (contents[TupleConstants::z] * rhs[TupleConstants::z]));
  return result;
}

constexpr Tuple ToVector(const Tuple& tuple) {
  return MakeVector(tuple[TupleConstants::x], tuple[TupleConstants::y],
                    tuple[TupleConstants::z]);
}

constexpr Tuple ToNormalizedVector(const Tuple& tuple) {
  return MakeVector(tuple[TupleConstants::x], tuple[TupleConstants::y],
                    tuple[TupleConstants::z])
      .Normalize();
}

struct PredefinedTuples {
  static constexpr Tuple ZeroPoint{0, 0, 0, TupleConstants::PointFlag};
  static constexpr Tuple ZeroVector{0, 0, 0, TupleConstants::VectorFlag};
  static constexpr Tuple UnitVecX{1, 0, 0, TupleConstants::VectorFlag};
  static constexpr Tuple UnitVecY{0, 1, 0, TupleConstants::VectorFlag};
  static constexpr Tuple UnitVecZ{0, 0, 1, TupleConstants::VectorFlag};
  static constexpr Tuple MinPoint{
      MathUtils::MathConstants::INF<>, MathUtils::MathConstants::INF<>,
      MathUtils::MathConstants::INF<>, TupleConstants::PointFlag};
  static constexpr Tuple MaxPoint{
      MathUtils::MathConstants::NINF<>, MathUtils::MathConstants::NINF<>,
      MathUtils::MathConstants::NINF<>, TupleConstants::PointFlag};
};

/*
  ---------------------
  Colour specialization
  ---------------------
*/
using Colour = Vec<double, 3>;

constexpr bool IsValidColour(const Colour& colour) {
  return 0 <= colour[ColourConstants::r] && colour[ColourConstants::r] <= 1 &&
         0 <= colour[ColourConstants::g] && colour[ColourConstants::g] <= 1 &&
         0 <= colour[ColourConstants::b] && colour[ColourConstants::b] <= 1;
}

constexpr Colour MakeColour(double r, double g, double b) {
  return Colour{r, g, b};
}

struct PredefinedColours {
  static constexpr Colour BLACK{0, 0, 0};
  static constexpr Colour WHITE{1, 1, 1};
  static constexpr Colour RED{1, 0, 0};
  static constexpr Colour GREEN{0, 1, 0};
  static constexpr Colour BLUE{0, 0, 1};
};

}  // namespace RayTracer

#endif
