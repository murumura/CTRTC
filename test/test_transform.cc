#include <gtest/gtest.h>
#include <transform.hh>
#include <vec.hh>
using namespace RayTracer;
using testing::Eq;
TEST(Transform, constructor)
{
    // default constructor
    constexpr auto defaultValue = Transform();
    constexpr auto I4 = PredefinedMatrices::I<double, 4>;
    EXPECT_EQ(defaultValue, I4);
}

TEST(Transform, translation)
{
    constexpr auto translation = MatrixUtils::Translation(5, -3, 2);
    constexpr TransformMatrix expected = {
        1, 0, 0, 5,
        0, 1, 0, -3,
        0, 0, 1, 2,
        0, 0, 0, 1};
    EXPECT_EQ(translation, expected);
    constexpr auto p = MakePoint(-3.0, 4.0, 5.0);
    EXPECT_EQ(translation * p, MakePoint(2.0, 1.0, 7.0));

    // Reverse translation to move point back to its original position
    constexpr auto invTranslation = MatrixUtils::Translation(-5, 3, -2);
    EXPECT_EQ(p, invTranslation * (translation * p));

    // Inverse of translation matrix should be the reverse translation matrix
    EXPECT_EQ(invTranslation, Inverse(translation));

    // Multiplying a tanslation matrix by a vector should not change the vector
    constexpr auto v = MakeVector(-3.0, 4.0, 5.0);
    EXPECT_EQ(v, translation * v);
}

TEST(Transform, scale)
{
    constexpr auto scale = MatrixUtils::Scale(2.0, 3.0, 4.0);
    constexpr auto p1 = MakePoint(-4.0, 6.0, 8.0);
    constexpr auto expectedP1 = MakePoint(-8.0, 18.0, 32.0);
    EXPECT_EQ(scale * p1, expectedP1);

    // scaling could affect vector too
    constexpr auto v1 = MakeVector(-4.0, 6.0, 8.0);
    constexpr auto expectedV1 = MakeVector(-8.0, 18.0, 32.0);
    EXPECT_EQ(scale * v1, expectedV1);

    // scale the tuple back by multiply with inverse scale matrix
    constexpr auto invScale = Inverse(scale);
    constexpr auto expectedV2 = MakeVector(-2.0, 2.0, 2.0);
    EXPECT_EQ(invScale * v1, expectedV2);

    // reflection test
    constexpr auto reflection = MatrixUtils::Scale(-1.0, 1.0, 1.0);
    constexpr auto p2 = MakePoint(2.0, 3.0, 4.0);
    constexpr auto expectedP2 = MakePoint(-2.0, 3.0, 4.0);
    EXPECT_EQ(reflection * p2, expectedP2);
}

TEST(Transform, rotationX)
{
    constexpr auto piRad = MathUtils::MathConstants::PI<double>;
    constexpr auto p1 = MakePoint(0.0, 1.0, 0.0);
    constexpr auto expectedP1 = MakePoint(0.0, MathUtils::ConstExprSqrtf(2.0) / 2, MathUtils::ConstExprSqrtf(2.0) / 2);
    constexpr auto halfQuarterX = MatrixUtils::RotateX(piRad / 4.0);
    EXPECT_EQ(halfQuarterX * p1, expectedP1);

    constexpr auto fullQuarterX = MatrixUtils::RotateX(piRad / 2.0);
    EXPECT_EQ(fullQuarterX * p1, MakePoint(0.0, 0.0, 1.0));

    // The inverse of an x-rotation rotates in the opposite direction
    constexpr auto invHalfQuarterX = Inverse(halfQuarterX);
    constexpr auto invExpectedP = MakePoint(0.0, MathUtils::ConstExprSqrtf(2.0) / 2, -MathUtils::ConstExprSqrtf(2.0) / 2);

    EXPECT_EQ(invHalfQuarterX * p1, invExpectedP);
}

TEST(Transform, rotationY)
{
    constexpr auto piRad = MathUtils::MathConstants::PI<double>;
    constexpr Tuple p1 = MakePoint(0, 0, 1);
    constexpr Tuple expectedP1 = MakePoint(MathUtils::ConstExprSqrtf(2.0) / 2, 0, MathUtils::ConstExprSqrtf(2.0) / 2);
    constexpr auto halfQuarterY = MatrixUtils::RotateY(piRad / 4);
    EXPECT_EQ(halfQuarterY * p1, expectedP1);

    constexpr Tuple expectedP2 = MakePoint(1, 0, 0);
    constexpr auto fullQuarterY = MatrixUtils::RotateY(piRad / 2);

    EXPECT_EQ(fullQuarterY * p1, expectedP2);
}

TEST(Tranform, rotationZ)
{
    constexpr auto piRad = MathUtils::MathConstants::PI<double>;
    constexpr Tuple p1 = MakePoint(0, 1, 0);
    constexpr Tuple expectedP1 = MakePoint(-MathUtils::ConstExprSqrtf(2.0) / 2, MathUtils::ConstExprSqrtf(2.0) / 2, 0);
    constexpr auto halfQuarterZ = MatrixUtils::RotateZ(piRad / 4);
    EXPECT_EQ(halfQuarterZ * p1, expectedP1);

    constexpr Tuple expectedP2 = MakePoint(-1, 0, 0);
    constexpr auto fullQuarterZ = MatrixUtils::RotateZ(piRad / 2);

    EXPECT_EQ(fullQuarterZ * p1, expectedP2);
}

TEST(Transform, shearing)
{
    // A MatrixUtils::Shearing transformation moves x in proportion to z
    constexpr auto shear1 = MatrixUtils::Shearing(0, 1, 0, 0, 0, 0);
    constexpr Tuple p1 = MakePoint(2, 3, 4);
    constexpr Tuple expectedP1 = MakePoint(6, 3, 4);
    EXPECT_EQ(shear1 * p1, expectedP1);

    // A MatrixUtils::Shearing transformation moves y in proportion to x
    constexpr auto shear2 = MatrixUtils::Shearing(0, 0, 1, 0, 0, 0);
    constexpr Tuple expectedP2 = MakePoint(2, 5, 4);
    EXPECT_EQ(shear2 * p1, expectedP2);

    // A MatrixUtils::Shearing transformation moves y in prportion to z
    constexpr auto shear3 = MatrixUtils::Shearing(0, 0, 0, 1, 0, 0);
    constexpr Tuple expectedP3 = MakePoint(2, 7, 4);
    EXPECT_EQ(shear3 * p1, expectedP3);

    // A MatrixUtils::Shearing transformation moves z in prportion to x
    constexpr auto shear4 = MatrixUtils::Shearing(0, 0, 0, 0, 1, 0);
    constexpr Tuple expectedP4 = MakePoint(2, 3, 6);
    EXPECT_EQ(shear4 * p1, expectedP4);

    // A MatrixUtils::Shearing transformation moves z in prportion to y
    constexpr auto shear5 = MatrixUtils::Shearing(0, 0, 0, 0, 0, 1);
    constexpr Tuple expectedP5 = MakePoint(2, 3, 7);
    EXPECT_EQ(shear5 * p1, expectedP5);
}

TEST(Transform, identity)
{
    constexpr auto from = MakePoint(0, 0, 0);
    constexpr auto to = MakePoint(0, 0, -1);
    constexpr auto up = MakePoint(0, 1, 0);

    constexpr auto t = MatrixUtils::ViewTransform(from, to, up);
    constexpr Matrix<double, 4, 4> I_4 = PredefinedMatrices::I<double, 4>;

    EXPECT_EQ(t, I_4);
}

TEST(Transform, lookat_positiveZ)
{
    constexpr auto from = MakePoint(0, 0, 0);
    constexpr auto to = MakePoint(0, 0, 1);
    constexpr auto up = MakePoint(0, 1, 0);

    constexpr auto t = MatrixUtils::ViewTransform(from, to, up);
    constexpr auto scale = MatrixUtils::Scale(-1, 1, -1);
    EXPECT_EQ(t, scale);
}

TEST(Transform, move_the_world)
{
    // the view transform moves the world instead of camera
    constexpr auto from = MakePoint(0, 0, 8);
    constexpr auto to = MakePoint(0, 0, 1);
    constexpr auto up = MakePoint(0, 1, 0);

    constexpr auto t = MatrixUtils::ViewTransform(from, to, up);
    EXPECT_EQ(t, MatrixUtils::Translation(0, 0, -8));
}

TEST(Transform, arbitary_transform)
{
    constexpr auto from = MakePoint(1, 3, 2);
    constexpr auto to = MakePoint(4, -2, 8);
    constexpr auto up = MakePoint(1, 1, 0);

    constexpr auto t = MatrixUtils::ViewTransform(from, to, up);
    constexpr auto expected = TransformMatrix{
        -0.50709, 0.50709, 0.67612, -2.36643,
        0.76772, 0.60609, 0.12122, -2.82843,
        -0.35857, 0.59761, -0.71714, 0.00000,
        0.00000, 0.00000, 0.00000, 1.00000};
    EXPECT_EQ(expected, t);
}

TEST(Transform, chaining)
{
    constexpr auto piRad = MathUtils::MathConstants::PI<double>;
    constexpr auto p = MakePoint(1, 0, 1);
    constexpr auto I4 = PredefinedMatrices::I<double, 4>;
    constexpr auto A = MatrixUtils::RotateX(piRad / 2.0);
    constexpr auto B = MatrixUtils::Scale(5, 5, 5);
    constexpr auto C = MatrixUtils::Translation(10, 5, 7);

    // Individual transformations are applied in sequence
    constexpr auto p1 = A * p;
    EXPECT_EQ(p1, MakePoint(1, -1, 0));
    constexpr auto p2 = B * p1;
    EXPECT_EQ(p2, MakePoint(5, -5, 0));
    constexpr auto p3 = C * p2;
    EXPECT_EQ(p3, MakePoint(15, 0, 7));

    // Chained transformations must be applied in reverse order
    constexpr auto T1 = C * B * A;
    EXPECT_EQ(T1 * p, MakePoint(15, 0, 7));

    // Using fluent API to perform chained transformations
    constexpr auto T2 = MatrixUtils::RotateX(piRad / 2.0).Scale(5, 5, 5).Translation(10, 5, 7);
    EXPECT_EQ(T1, T2);

    // Using chain method to perform chained transformations
    constexpr auto T3 = Transform().Chain(C, B, A);
    EXPECT_EQ(T2, T3);
}
