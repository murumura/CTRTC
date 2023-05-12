#include <gtest/gtest.h>
#include <limits>
#include <math.hh>
using namespace RayTracer::MathUtils;
using testing::Eq;

TEST(Math, number_of_digit) {
  constexpr int x1 = 164;
  constexpr int x2 = 95;
  constexpr int y1 = NumOfDigitsHelper(x1);
  EXPECT_TRUE(y1 == 3);
  constexpr int y2 = NumOfDigitsHelper(x2);
  EXPECT_TRUE(y2 == 2);
  constexpr int y3 = NumOfDigits(x1, x2);
  EXPECT_TRUE(y3 == y1 + y2);
}

TEST(Math, sine) {
  constexpr auto piRad = MathConstants::PI<double>;
  constexpr auto sinVal = Sine(piRad / 2.);
  EXPECT_TRUE((ApproxEqual(sinVal, 1.0)));
  EXPECT_TRUE((ApproxEqual(Sine(piRad), 0.0)));
  EXPECT_TRUE((ApproxEqual(Sine(2 * piRad), 0.0)));
  EXPECT_TRUE((ApproxEqual(Sine(piRad / 4.), ConstExprSqrtf(2.0) / 2)));
}

TEST(Math, cosine) {
  constexpr auto piRad = MathConstants::PI<double>;
  constexpr auto cosVal = Cosine(piRad / 2.);
  EXPECT_TRUE((ApproxEqual(cosVal, 0.0)));
  EXPECT_TRUE((ApproxEqual(Cosine(piRad), -1.0)));
  EXPECT_TRUE((ApproxEqual(Cosine(2 * piRad), 1.0)));
  EXPECT_TRUE((ApproxEqual(Cosine(piRad / 4.), ConstExprSqrtf(2.0) / 2)));
}

TEST(Math, tangent) {
  constexpr auto piRad = MathConstants::PI<double>;
  constexpr auto tanVal = Tangent(piRad / 4.);
  EXPECT_TRUE((ApproxEqual(tanVal, 1.0)));
  EXPECT_TRUE((ApproxEqual(Tangent(piRad), 0.0)));
  EXPECT_TRUE((ApproxEqual(Tangent(2 * piRad), 0.0)));
  constexpr double tan60d = 1.73205080757;
  EXPECT_TRUE((ApproxEqual(Tangent(piRad / 3.), tan60d)));
}

TEST(Math, solve_quadratic) {
  constexpr auto a1 = 1.0;
  constexpr auto b1 = -4.0;
  constexpr auto c1 = 4.0;
  constexpr auto res1 = SolveQuadratic(a1, b1, c1);
  const auto& [r_11, r_12] = res1.value();
  EXPECT_TRUE(r_11 == r_12);
  EXPECT_TRUE((ApproxEqual(r_11, 2.0)));

  constexpr auto a2 = 1.0;
  constexpr auto b2 = -3.0;
  constexpr auto c2 = 2.0;
  constexpr auto res2 = SolveQuadratic(a2, b2, c2);
  const auto& [r_21, r_22] = res2.value();
  EXPECT_TRUE((ApproxEqual(r_21, 1.0)));
  EXPECT_TRUE((ApproxEqual(r_22, 2.0)));

  constexpr auto a3 = 1.0;
  constexpr auto b3 = 1.0;
  constexpr auto c3 = 1.0;
  constexpr auto res3 = SolveQuadratic(a3, b3, c3);
  EXPECT_EQ(res3, std::nullopt);
}

TEST(Math, modulo) {
  constexpr auto x1 = 4.0;
  constexpr auto x2 = 2.0;
  constexpr auto x3 = 3.0;
  constexpr auto x4 = 7.0;
  constexpr auto y1 = 3.0;
  constexpr auto y2 = 1.0;
  constexpr auto y3 = 4.0;
  constexpr auto y4 = 5.0;
  constexpr auto m1 = Modulo(x1, y1);
  EXPECT_EQ(m1, 1.0);

  constexpr auto m2 = Modulo(x2, y2);
  EXPECT_EQ(m2, 0.0);

  constexpr auto m3 = Modulo(x3, y3);
  EXPECT_EQ(m3, 3.0);

  constexpr auto m4 = Modulo(x4, y4);
  EXPECT_EQ(m4, 2.0);
}
