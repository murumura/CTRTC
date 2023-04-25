#ifndef MATH_HH
#define MATH_HH
#include <cmath>
#include <concepts>
#include <numeric>
#include <optional>
#include <type_traits>
#include <utility>
#define EPSILON 1e-4
namespace RayTracer {
namespace MathUtils {
namespace MathConstants {

template <typename T = double>
constexpr T PI = T(3.1415926535897932385L);

template <typename T = double>
constexpr T INF = std::numeric_limits<T>::infinity();

template <typename T = double>
constexpr T NINF = -std::numeric_limits<T>::infinity();
};  // namespace MathConstants

/* Constexpr square root value for floating point value. */
template <std::floating_point T>
constexpr T ConstExprSqrtf(T x) {
  if (x >= 0 && x < std::numeric_limits<T>::infinity()) {
    T curr{x}, prev{0.0f};
    while (curr != prev) {
      prev = curr;
      curr = 0.5f * (curr + x / curr);
    }

    return curr;
  }
  return std::numeric_limits<T>::quiet_NaN();
}

/* constexpr absolute value of a floating point value. */
template <std::floating_point T>
constexpr T ConstExprAbsf(T val) {
  return val < static_cast<T>(0) ? -val : val;
}

template <std::floating_point S, std::floating_point T>
constexpr bool ApproxEqual(S x, T y) {
  // We need special treatment for infinities here.
  if (x == std::numeric_limits<S>::infinity() &&
      y == std::numeric_limits<S>::infinity())
    return true;
  if (x == -std::numeric_limits<T>::infinity() &&
      y == -std::numeric_limits<T>::infinity())
    return true;
  return (ConstExprAbsf(x - y) < EPSILON);
}

// test whether values are within machine epsilon, used for algorithm
// termination
template <std::floating_point T>
constexpr T Feq(T x, T y) {
  return ConstExprAbsf(x - y) <= std::numeric_limits<T>::epsilon();
}

/* Constexpr calculation of digit number from single number */
template <typename T>
constexpr int NumOfDigitsHelper(T number) {
  int digits = 0;
  if (number < 0)
    digits = 1;
  while (number) {
    number /= 10;
    digits++;
  }
  return digits;
}

/* Constexpr calculation of digit number from packs of numbers */
template <typename... Ts>
requires(std::conjunction_v<std::is_arithmetic<
             std::decay_t<Ts>>...>) constexpr int NumOfDigits(Ts... args) {
  return (NumOfDigitsHelper(args) + ...);
}

/** @brief compute taylor series for sine/cosine in recursive fasion
 * @param x: initial value
 * @param sum: summation term from previous funtion call
 * @param fact: factorial term, e.g 3!, 5!, ...
 * @param i: iteration index for this iteration
 * @param sign: sign for this iteration
 * @param xProduct: product term of x, e.g x^2, x^4, ...
 * @return Traiangle expannsion value
 */
template <std::floating_point T>
constexpr T TaylorSeries(T x, T sum, T fact, int i, int sign, T xProduct) {
  // clang-format off
            return Feq(sum, sum + xProduct * sign / fact) ? sum : \
                TaylorSeries(x, sum + xProduct * sign / fact, fact * i * (i + 1), i + 2, -sign, xProduct * x * x);
  // clang-format on
}

/* Constexpr calculation sin by Taylor series expansion */
template <std::floating_point T>
constexpr T Sine(T rad) {
  // sinx = x - \frac{x^3}{3!} + \frac{x^5}{5!} - ...
  int sign = -1;
  int idx = 4;
  T t = rad * rad * rad;  // x^3
  T fact = 6.0;           // 3!
  return TaylorSeries(rad, rad, fact, idx, sign, t);
}

/* Constexpr calculation cos by Taylor series expansion */
template <std::floating_point T>
constexpr T Cosine(T rad) {
  // cosx = 1.0 - \frac{x^2}{2!} + \frac{x^4}{4!} - ...
  int sign = -1;
  int idx = 3;
  T t = rad * rad;  // x^2
  T fact = 2.0;     // 2!
  return TaylorSeries(rad, 1.0, fact, idx, sign, t);
}

/* Constexpr calculation cos by Taylor series expansion */
template <std::floating_point T>
constexpr T Tangent(T rad) {
  return Sine(rad) / Cosine(rad);
}

//! computes base^exponent, base to the power of exponent (with an integer exponent)
template <std::floating_point T>
constexpr T ConstExprExp(const T base, const int32_t exponent) {
  T ret = (T)1;
  for (int32_t i = 0; i < exponent; ++i) {
    ret *= base;
  }
  return ret;
}

template <std::floating_point T>
constexpr std::optional<std::pair<T, T>> SolveQuadratic(T a, T b, T c) {
  T discriminant = (b * b) - (4.0 * a * c);
  if (ConstExprAbsf(discriminant) < 1e-4) {
    // Discriminant is 0, we have two equal roots
    T r = -b / (2.0 * a);
    return std::pair(r, r);
  }
  // Test for complex roots
  if (discriminant < 0.0)
    return std::nullopt;
  // Discriminant is positive solve for two real and different roots
  T sqrtD = ConstExprSqrtf(discriminant);
  T Denominator = 1.0 / (2.0 * a);
  T r1 = (-b - sqrtD) * Denominator;
  T r2 = (-b + sqrtD) * Denominator;
  if (r1 > r2)
    std::swap(r1, r2);
  return std::pair(r1, r2);
}

}  // namespace MathUtils
}  // namespace RayTracer
#endif
