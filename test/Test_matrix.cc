#include <gmock/gmock.h>

#include <matrix.hh>
using namespace RayTracer;

using testing::Eq;

TEST(Mat, constructor)
{
    constexpr Matrix<double, 3, 3> m1 = {
        1., 2., 3.,
        4., 5., 6.,
        7., 8., 9.};
    ASSERT_EQ(m1[0][2], 3.);
    ASSERT_EQ(m1[1][2], 6.);
    ASSERT_EQ(m1[2][2], 9.);
    static_assert(m1.Row(2) == Vec{7., 8., 9.});
    static_assert(m1.Column(2) == Vec{3., 6., 9.});
}

TEST(Mat, negate)
{
    constexpr Matrix<double, 3, 4> m1 = {
        1., 2., 3., 4.,
        5., 6., 7., 8.,
        9., 10., 11., 12.};
    constexpr auto m1Negative = -m1;
    ASSERT_NE(m1Negative, m1);
    ASSERT_EQ(m1Negative[0][0], -1.);
    ASSERT_EQ(m1Negative[0][1], -2.);

    constexpr auto m1NegativeRow2 = m1Negative.Row(2);
    constexpr auto m1NegativeRow2Expected = -Vec{9., 10., 11., 12.};
    ASSERT_EQ(m1NegativeRow2, m1NegativeRow2Expected);

    constexpr auto m1NegativeCol0 = m1Negative.Column(0);
    constexpr auto m1NegativeCol0Expected = -Vec{1., 5., 9.};
    ASSERT_EQ(m1NegativeCol0, m1NegativeCol0Expected);
}

TEST(Mat, opeartor_plus)
{
    constexpr Matrix<double, 3, 4> m1 = {
        1., 2., 3., 4.,
        5., 6., 7., 8.,
        9., 10., 11., 12.};

    constexpr Matrix<double, 3, 4> m2 = {
        1., 2., 3., 4.,
        5., 6., 7., 8.,
        9., 10., 11., 12.};

    constexpr auto mSum12 = m1 + m2;
    for (std::size_t i = 0; i < 3; i++)
        for (std::size_t j = 0; j < 4; j++)
            ASSERT_EQ(mSum12[i][j], 2 * (4 * i + j + 1));

    constexpr auto m1Plus2 = m1 + 2.0;
    for (std::size_t i = 0; i < 3; i++)
        for (std::size_t j = 0; j < 4; j++)
            ASSERT_EQ(m1Plus2[i][j], m1[i][j] + 2.0);

    Matrix<double, 3, 4> m3 = {
        1., 2., 3., 4.,
        5., 6., 7., 8.,
        9., 10., 11., 12.};

    m3 += 2.0;
    for (std::size_t i = 0; i < 3; i++)
        for (std::size_t j = 0; j < 4; j++)
            ASSERT_EQ(m3[i][j], m1[i][j] + 2.0);
    m3 += m1;

    for (std::size_t i = 0; i < 3; i++)
        for (std::size_t j = 0; j < 4; j++)
            ASSERT_EQ(m3[i][j], 2 * m1[i][j] + 2.0);
}

TEST(Mat, operator_minus)
{

    constexpr Matrix<double, 3, 4> m1 = {
        1., 2., 3., 4.,
        5., 6., 7., 8.,
        9., 10., 11., 12.};

    constexpr Matrix<double, 3, 4> m2 = {
        1., 2., 3., 4.,
        5., 6., 7., 8.,
        9., 10., 11., 12.};
    constexpr auto mNeg12 = m1 - m2;

    for (std::size_t i = 0; i < 3; i++)
        for (std::size_t j = 0; j < 4; j++)
            ASSERT_EQ(mNeg12[i][j], 0);

    constexpr auto m1Negate2 = m1 - 2.0;

    for (std::size_t i = 0; i < 3; i++)
        for (std::size_t j = 0; j < 4; j++)
            ASSERT_EQ(m1Negate2[i][j], m1[i][j] - 2.0);

    Matrix<double, 3, 4> m3 = {
        1., 2., 3., 4.,
        5., 6., 7., 8.,
        9., 10., 11., 12.};

    m3 -= 2.0;

    for (std::size_t i = 0; i < 3; i++)
        for (std::size_t j = 0; j < 4; j++)
            ASSERT_EQ(m3[i][j], m1[i][j] - 2.0);
    m3 -= m1;

    for (std::size_t i = 0; i < 3; i++)
        for (std::size_t j = 0; j < 4; j++)
            ASSERT_EQ(m3[i][j], -2.0);
}
