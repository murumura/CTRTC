#include <gmock/gmock.h>
#include <limits>
#include <math.hh>
using namespace RayTracer::MathUtils;
using testing::Eq;

TEST(Math, number_of_digit)
{
    constexpr int x1 = 164;
    constexpr int x2 = 95;
    constexpr int y1 = NumOfDigitsHelper(x1);
    ASSERT_TRUE(y1 == 3);
    constexpr int y2 = NumOfDigitsHelper(x2);
    ASSERT_TRUE(y2 == 2);
    constexpr int y3 = NumOfDigits(x1, x2);
    ASSERT_TRUE(y3 == y1 + y2);
}

TEST(Math, sine)
{
    constexpr auto piRad = MathConstants::PI<double>;
    constexpr auto sinVal = Sine(piRad / 2.);
    ASSERT_TRUE((ApproxEqual(sinVal, 1.0)));
    ASSERT_TRUE((ApproxEqual(Sine(piRad), 0.0)));
    ASSERT_TRUE((ApproxEqual(Sine(2 * piRad), 0.0)));
    ASSERT_TRUE((ApproxEqual(Sine(piRad / 4.), ConstExprSqrtf(2.0) / 2)));
}

TEST(Math, cosine)
{
    constexpr auto piRad = MathConstants::PI<double>;
    constexpr auto cosVal = Cosine(piRad / 2.);
    ASSERT_TRUE((ApproxEqual(cosVal, 0.0)));
    ASSERT_TRUE((ApproxEqual(Cosine(piRad), -1.0)));
    ASSERT_TRUE((ApproxEqual(Cosine(2 * piRad), 1.0)));
    ASSERT_TRUE((ApproxEqual(Cosine(piRad / 4.), ConstExprSqrtf(2.0) / 2)));
}

TEST(Math, tangent)
{
    constexpr auto piRad = MathConstants::PI<double>;
    constexpr auto tanVal = Tangent(piRad / 4.);
    ASSERT_TRUE((ApproxEqual(tanVal, 1.0)));
    ASSERT_TRUE((ApproxEqual(Tangent(piRad), 0.0)));
    ASSERT_TRUE((ApproxEqual(Tangent(2 * piRad), 0.0)));
    double tan60d = 1.73205080757;
    ASSERT_TRUE((ApproxEqual(Tangent(piRad / 3.), tan60d)));
}

TEST(Math, solve_quadratic)
{
    constexpr auto a1 = 1.0;
    constexpr auto b1 = -4.0;
    constexpr auto c1 = 4.0;
    auto res1 = SolveQuadratic(a1, b1, c1);
    const auto& [r_11, r_12] = res1.value();
    ASSERT_TRUE(r_11 == r_12);
    ASSERT_TRUE((ApproxEqual(r_11, 2.0)));

    constexpr auto a2 = 1.0;
    constexpr auto b2 = -3.0;
    constexpr auto c2 = 2.0;
    auto res2 = SolveQuadratic(a2, b2, c2);
    const auto& [r_21, r_22] = res2.value();
    ASSERT_TRUE((ApproxEqual(r_21, 1.0)));
    ASSERT_TRUE((ApproxEqual(r_22, 2.0)));

    constexpr auto a3 = 1.0;
    constexpr auto b3 = 1.0;
    constexpr auto c3 = 1.0;
    auto res3 = SolveQuadratic(a3, b3, c3);
    ASSERT_EQ(res3, std::nullopt);
}
