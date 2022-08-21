#include <gtest/gtest.h>

#include <matrix.hh>
using namespace RayTracer;

using testing::Eq;

TEST(Mat, constructor) {
  constexpr Matrix<double, 3, 3> m1 = {1., 2., 3., 4., 5., 6., 7., 8., 9.};
  EXPECT_EQ(m1[0][2], 3.);
  EXPECT_EQ(m1[1][2], 6.);
  EXPECT_EQ(m1[2][2], 9.);
  static_assert(m1.Row(2) == Vec{7., 8., 9.});
  static_assert(m1.Column(2) == Vec{3., 6., 9.});
}

TEST(Mat, negate) {
  constexpr Matrix<double, 3, 4> m1 = {1., 2., 3., 4.,  5.,  6.,
                                       7., 8., 9., 10., 11., 12.};
  constexpr auto m1Negative = -m1;
  ASSERT_NE(m1Negative, m1);
  EXPECT_EQ(m1Negative[0][0], -1.);
  EXPECT_EQ(m1Negative[0][1], -2.);

  constexpr auto m1NegativeRow2 = m1Negative.Row(2);
  constexpr auto m1NegativeRow2Expected = -Vec{9., 10., 11., 12.};
  EXPECT_EQ(m1NegativeRow2, m1NegativeRow2Expected);

  constexpr auto m1NegativeCol0 = m1Negative.Column(0);
  constexpr auto m1NegativeCol0Expected = -Vec{1., 5., 9.};
  EXPECT_EQ(m1NegativeCol0, m1NegativeCol0Expected);
}

TEST(Mat, opeartor_plus) {
  constexpr Matrix<double, 3, 4> m1 = {1., 2., 3., 4.,  5.,  6.,
                                       7., 8., 9., 10., 11., 12.};

  constexpr Matrix<double, 3, 4> m2 = {1., 2., 3., 4.,  5.,  6.,
                                       7., 8., 9., 10., 11., 12.};

  constexpr auto mSum12 = m1 + m2;
  for (std::size_t i = 0; i < 3; i++)
    for (std::size_t j = 0; j < 4; j++)
      EXPECT_EQ(mSum12[i][j], 2 * (4 * i + j + 1));

  constexpr auto m1Plus2 = m1 + 2.0;
  for (std::size_t i = 0; i < 3; i++)
    for (std::size_t j = 0; j < 4; j++)
      EXPECT_EQ(m1Plus2[i][j], m1[i][j] + 2.0);

  Matrix<double, 3, 4> m3 = {1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12.};

  m3 += 2.0;
  for (std::size_t i = 0; i < 3; i++)
    for (std::size_t j = 0; j < 4; j++)
      EXPECT_EQ(m3[i][j], m1[i][j] + 2.0);
  m3 += m1;

  for (std::size_t i = 0; i < 3; i++)
    for (std::size_t j = 0; j < 4; j++)
      EXPECT_EQ(m3[i][j], 2 * m1[i][j] + 2.0);
}

TEST(Mat, operator_minus) {

  constexpr Matrix<double, 3, 4> m1 = {1., 2., 3., 4.,  5.,  6.,
                                       7., 8., 9., 10., 11., 12.};

  constexpr Matrix<double, 3, 4> m2 = {1., 2., 3., 4.,  5.,  6.,
                                       7., 8., 9., 10., 11., 12.};
  constexpr auto mNeg12 = m1 - m2;

  for (std::size_t i = 0; i < 3; i++)
    for (std::size_t j = 0; j < 4; j++)
      EXPECT_EQ(mNeg12[i][j], 0);

  constexpr auto m1Negate2 = m1 - 2.0;

  for (std::size_t i = 0; i < 3; i++)
    for (std::size_t j = 0; j < 4; j++)
      EXPECT_EQ(m1Negate2[i][j], m1[i][j] - 2.0);

  Matrix<double, 3, 4> m3 = {1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12.};

  m3 -= 2.0;

  for (std::size_t i = 0; i < 3; i++)
    for (std::size_t j = 0; j < 4; j++)
      EXPECT_EQ(m3[i][j], m1[i][j] - 2.0);
  m3 -= m1;

  for (std::size_t i = 0; i < 3; i++)
    for (std::size_t j = 0; j < 4; j++)
      EXPECT_EQ(m3[i][j], -2.0);
}

TEST(Mat, operator_mul) {
  constexpr Matrix<double, 4, 4> m1 = {1, 2, 3, 4, 2, 3, 4, 5,
                                       3, 4, 5, 6, 4, 5, 6, 7};

  constexpr Matrix<double, 4, 4> m2 = {0, 1, 2, 4,  1, 2, 4,  8,
                                       2, 4, 8, 16, 4, 8, 16, 32};

  constexpr Matrix<double, 4, 4> mMul12 = {24, 49, 98,  196, 31, 64, 128, 256,
                                           38, 79, 158, 316, 45, 94, 188, 376};

  EXPECT_EQ(mMul12.Rows, 4);
  EXPECT_EQ(mMul12.Cols, 4);
  EXPECT_EQ(mMul12, m1 * m2);
  constexpr auto m1Mul2 = m1 * 2.0;
  EXPECT_EQ(m1Mul2, m1 * 2.0);
  EXPECT_EQ(m1Mul2, 2.0 * m1);
  Matrix<double, 4, 4> m3 = m1;
  m3 *= m1;
  constexpr Matrix<double, 4, 4> m3Expected = {
      30, 40, 50, 60, 40, 54, 68, 82, 50, 68, 86, 104, 60, 82, 104, 126};
  EXPECT_EQ(m3, m3Expected);

  constexpr Vec<double, 4> v1 = {1., 1., 1., 1.};
  constexpr auto m1Mulv1 = m1 * v1;
  EXPECT_EQ(m1Mulv1[0], 10.);
  EXPECT_EQ(m1Mulv1[1], 14.);
  EXPECT_EQ(m1Mulv1[2], 18.);
  EXPECT_EQ(m1Mulv1[3], 22.);
}

TEST(Mat, operator_div) {
  constexpr Matrix<double, 4, 4> m1 = {2, 4,  6,  8,  4,  8,  12, 16,
                                       8, 16, 24, 32, 16, 32, 48, 64};

  constexpr Matrix<double, 4, 4> m2 = {2, 2, 2, 2, 4,  4,  4,  4,
                                       8, 8, 8, 8, 16, 16, 16, 16};

  constexpr Matrix<double, 4, 4> mDiv12 = {1, 2, 3, 4, 1, 2, 3, 4,
                                           1, 2, 3, 4, 1, 2, 3, 4};

  constexpr Matrix<double, 4, 4> divide2Expected = {1, 1, 1, 1, 2, 2, 2, 2,
                                                    4, 4, 4, 4, 8, 8, 8, 8};

  EXPECT_EQ(mDiv12, m1 / m2);
  EXPECT_EQ(divide2Expected, m2 / 2.0);

  Matrix<double, 4, 4> m3 = m1;
  m3 /= m2;
  EXPECT_EQ(m3, m1 / m2);
  m3 = m2;
  m3 /= 2.0;
  EXPECT_EQ(m3, divide2Expected);
}

TEST(Mat, transpose) {
  constexpr Matrix<double, 3, 4> mat = {1., 2., 3., 4., 2., 4.,
                                        4., 2., 8., 6., 4., 1.};

  const auto tranMat = Transpose(mat);
  EXPECT_EQ(tranMat.Rows, 4);
  EXPECT_EQ(tranMat.Cols, 3);
  constexpr Matrix<double, 2, 3> m1 = {0., 1., 2., 3., 4., 5.};

  constexpr Matrix<double, 3, 2> m2 = {0., 3., 1., 4., 2., 5.};
  EXPECT_EQ(Transpose(m1), m2);
  constexpr Matrix<double, 4, 4> I4 = PredefinedMatrices::I<double, 4>;

  EXPECT_EQ(Transpose(I4), I4);
  // transpose a transpose matrix should return itself
  EXPECT_EQ(Transpose(Transpose(mat)), mat);
}

TEST(Mat, submatrix) {
  constexpr Matrix<double, 3, 3> A = {1., 5., 0., -3., 2., 7., 0., 6., -3.};
  constexpr Matrix<double, 2, 2> subA = SubMatrix(A, 0, 2);
  constexpr Matrix<double, 2, 2> ExpectedSubA = {-3., 2., 0., 6.};
  EXPECT_EQ(subA, ExpectedSubA);

  constexpr Matrix<double, 4, 4> B = {-6., 1., 1., 6., -8., 5., 8.,  6.,
                                      -1., 0., 8., 2., -7., 1., -1., 1.};
  constexpr Matrix<double, 3, 3> ExpectedSubB = {-6., 1.,  6.,  -8., 8.,
                                                 6.,  -7., -1., 1.};
  constexpr Matrix<double, 3, 3> subB = SubMatrix(B, 2, 1);
  EXPECT_EQ(subB, ExpectedSubB);
}

TEST(Mat, minor) {

  constexpr Matrix<double, 3, 3> A = {3., 5., 0., 2., -1., -7., 6., -1., 5.};
  constexpr double expected = 25.;
  constexpr auto minorA = Minor(A, 1, 0);
  EXPECT_EQ(expected, minorA);
}

TEST(Mat, cofactor) {
  constexpr Matrix<double, 3, 3> A = {3., 5., 0., 2., -1., -7., 6., -1., 5.};
  constexpr double expected = -25.;
  constexpr auto cofactorA = Cofactor(A, 1, 0);
  EXPECT_EQ(expected, cofactorA);
  constexpr auto cf = Cofactor(A, 0, 0);
  EXPECT_EQ(-12., cf);
}

TEST(Mat, determinant) {
  constexpr Matrix<double, 3, 3> m = {1., 2., 6., -5., 8., -4., 2., 6., 4.};

  // Check that determinant is constexpr.
  [[maybe_unused]] constexpr auto res1 = Determinant(m);
  [[maybe_unused]] constexpr auto res2 = Cofactor(m, 0, 0);

  EXPECT_EQ(res2, 56.);
  EXPECT_EQ(res1, -196.);

  constexpr Matrix<double, 4, 4> m2 = {-2., -8., 3.,  5., -3., 1., 7., 3.,
                                       1.,  2.,  -9., 6., -6., 7., 7., -9.};

  [[maybe_unused]] constexpr auto res3 = Determinant(m2);
  [[maybe_unused]] constexpr auto res4 = Cofactor(m2, 0, 0);
  [[maybe_unused]] constexpr auto res5 = Cofactor(m2, 0, 1);
  [[maybe_unused]] constexpr auto res6 = Cofactor(m2, 0, 2);
  [[maybe_unused]] constexpr auto res7 = Cofactor(m2, 0, 3);

  EXPECT_EQ(res3, -4071.);
  EXPECT_EQ(res4, 690.);
  EXPECT_EQ(res5, 447.);
  EXPECT_EQ(res6, 210.);
  EXPECT_EQ(res7, 51.);
}

TEST(Mat, invertible) {
  constexpr Matrix<double, 4, 4> m = {6., 4.,  4., 4.,  5., 5., 7., 6.,
                                      4., -9., 3., -7., 9., 1., 7., -6.};
  EXPECT_EQ(Determinant(m), -2120.);
  EXPECT_TRUE(Invertible(m));
  constexpr Matrix<double, 4, 4> m2 = {-4., 2.,  -2., -3., 9., 6., 2., 6.,
                                       0,   -5., 1.,  -5., 0,  0,  0,  0};
  EXPECT_FALSE(Invertible(m2));
}

TEST(Mat, inverse) {
  constexpr Matrix<double, 4, 4> M = {8.,  -5., 9., 2., 7.,  5., 6.,  1.,
                                      -6., -0,  9., 6., -3., 0,  -9., -4.};
  constexpr Matrix<double, 4, 4> mInv = Inverse(M);
  constexpr Matrix<double, 4, 4> mInvExpected = {
      -0.15385, -0.15385, -0.28205, -0.53846, -0.07692, 0.12308,
      0.02564,  0.03077,  0.35897,  0.35897,  0.43590,  0.92308,
      -0.69231, -0.69231, -0.76923, -1.92308};
  EXPECT_EQ(mInv, mInvExpected);

  constexpr Matrix<double, 4, 4> M2 = {9.,  3., 0,  9., -5., -2., -6., -3.,
                                       -4., 9., 6., 4., -7., 6.,  6.,  2.};
  constexpr Matrix<double, 4, 4> m2InvExpected = {
      -0.04074, -0.07778, 0.14444,  -0.22222, -0.07778, 0.03333,
      0.36667,  -0.33333, -0.02901, -0.14630, -0.10926, 0.12963,
      0.17778,  0.06667,  -0.26667, 0.33333};
  constexpr Matrix<double, 4, 4> invM2 = Inverse(M2);
  EXPECT_EQ(invM2, m2InvExpected);

  constexpr Matrix<double, 4, 4> I4 = PredefinedMatrices::I<double, 4>;
  constexpr Matrix<double, 4, 4> I4_ = M2 * invM2;
  EXPECT_EQ(I4, I4_);
}
