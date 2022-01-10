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

TEST(Mat, operator_mul)
{
    constexpr Matrix<double, 4, 4> m1 = {1, 2, 3, 4,
        2, 3, 4, 5,
        3, 4, 5, 6,
        4, 5, 6, 7};

    constexpr Matrix<double, 4, 4> m2 = {0, 1, 2, 4,
        1, 2, 4, 8,
        2, 4, 8, 16,
        4, 8, 16, 32};

    constexpr Matrix<double, 4, 4> mMul12 = {24, 49, 98, 196,
        31, 64, 128, 256,
        38, 79, 158, 316,
        45, 94, 188, 376};

    ASSERT_EQ(mMul12.Rows, 4);
    ASSERT_EQ(mMul12.Cols, 4);
    ASSERT_EQ(mMul12, m1 * m2);
    constexpr auto m1Mul2 = m1 * 2.0;
    ASSERT_EQ(m1Mul2, m1 * 2.0);
    ASSERT_EQ(m1Mul2, 2.0 * m1);
    Matrix<double, 4, 4> m3 = m1;
    m3 *= m1;
    constexpr Matrix<double, 4, 4> m3Expected = {
        30, 40, 50, 60,
        40, 54, 68, 82,
        50, 68, 86, 104,
        60, 82, 104, 126};
    ASSERT_EQ(m3, m3Expected);

    constexpr Vec<double, 4> v1 = {1., 1., 1., 1.};
    constexpr auto m1Mulv1 = m1 * v1;
    ASSERT_EQ(m1Mulv1[0], 10.);
    ASSERT_EQ(m1Mulv1[1], 14.);
    ASSERT_EQ(m1Mulv1[2], 18.);
    ASSERT_EQ(m1Mulv1[3], 22.);
}

TEST(Mat, operator_div)
{
    constexpr Matrix<double, 4, 4> m1 = {2, 4, 6, 8,
        4, 8, 12, 16,
        8, 16, 24, 32,
        16, 32, 48, 64};

    constexpr Matrix<double, 4, 4> m2 = {2, 2, 2, 2,
        4, 4, 4, 4,
        8, 8, 8, 8,
        16, 16, 16, 16};

    constexpr Matrix<double, 4, 4> mDiv12 = {1, 2, 3, 4,
        1, 2, 3, 4,
        1, 2, 3, 4,
        1, 2, 3, 4};

    constexpr Matrix<double, 4, 4> divide2Expected = {
        1, 1, 1, 1,
        2, 2, 2, 2,
        4, 4, 4, 4,
        8, 8, 8, 8};

    ASSERT_EQ(mDiv12, m1 / m2);
    ASSERT_EQ(divide2Expected, m2 / 2.0);

    Matrix<double, 4, 4> m3 = m1;
    m3 /= m2;
    ASSERT_EQ(m3, m1 / m2);
    m3 = m2;
    m3 /= 2.0;
    ASSERT_EQ(m3, divide2Expected);
}

TEST(Mat, transpose)
{
    constexpr Matrix<double, 3, 4> mat = {
        1, 2, 3, 4,
        2, 4, 4, 2,
        8, 6, 4, 1};

    const auto tranMat = Transpose(mat);
    ASSERT_EQ(tranMat.Rows, 4);
    ASSERT_EQ(tranMat.Cols, 3);
    constexpr Matrix<double, 2, 3> m1 = {
        0, 1, 2,
        3, 4, 5};

    constexpr Matrix<double, 3, 2> m2 = {
        0, 3,
        1, 4,
        2, 5};
    ASSERT_EQ(Transpose(m1), m2);
    constexpr Matrix<double, 4, 4> I4 = PredefinedMatrices::I<double, 4>;

    ASSERT_EQ(Transpose(I4), I4);
    // transpose a transpose matrix should return itself
    ASSERT_EQ(Transpose(Transpose(mat)), mat);
}
