#include <gmock/gmock.h>

#include <vec.hh>
using namespace RayTracer;

using testing::Eq;

TEST(Vec, utils)
{
    constexpr Vec<double, 4> seqVec = VecUtils::SeqVec<4>();
    constexpr auto seqVecExpected = Vec{0.0, 1.0, 2.0, 3.0};
    ASSERT_EQ(seqVec, seqVecExpected);
    constexpr auto genVecExpected = Vec{0.0, 1.0, 4.0, 9.0};
    constexpr auto genVec = VecUtils::Generate<4>([](auto i) { return double(i * i); });
    ASSERT_EQ(genVec, genVecExpected);
}

TEST(Vec, constructor)
{
    /* Regular vector initialization from list constructor */
    constexpr Vec<double, 4> v1{0.0, 1.0, 2.0, 3.0};
    constexpr Vec<double, 4> v2{0.0, 1.0, 2.0, 3.0};
    ASSERT_THAT(v1.size(), Eq(4));
    ASSERT_EQ(v1.size(), v2.size());
    ASSERT_TRUE(v1 == v2);
    ASSERT_EQ(v1, v2);
    // Make Vector and aggregate initialization deduction guide
    static_assert(CreateVector(1.0, 2.0, 3.0) == Vec{1.0, 2.0, 3.0}, "MakeVector and uniform initialization deduction guide");
    static_assert(CreateVector(1.0, 2.0, 3.0) == Vec{{1.0, 2.0, 3.0}}, "MakeVector and uniform initialization deduction guide");
}

TEST(Vec, opeartor_negate)
{
    constexpr auto v1 = Vec{1.0, 2.0, 3.0};
    constexpr auto v2 = Vec{-1.0, -2.0, -3.0};
    ASSERT_EQ(-v1, v2);
    ASSERT_EQ(-1.0 * v1, v2);
    ASSERT_EQ(v1 * -1.0, v2);
}

TEST(Vec, opeartor_plus)
{
    static_assert(2.0 + Vec{1.0, 2.0, 3.0} == Vec{3.0, 4.0, 5.0}, "operator+");
    static_assert(Vec{1.0, 2.0, 3.0} + 2.0 == Vec{3.0, 4.0, 5.0}, "operator+");
    static_assert(Vec{1.0, 2.0, 3.0} + Vec{2.0, 2.0, 2.0} == Vec{3.0, 4.0, 5.0}, "operator+");
    constexpr auto v1 = Vec{1.0, 2.0, 3.0} + 2.0;
    constexpr auto v2 = Vec{3.0, 4.0, 5.0};
    ASSERT_EQ(v1, v2);
    Vec<double, 3> v3 = Vec{1.0, 2.0, 3.0};
    v3 += 2.0;
    ASSERT_EQ(v3, v2);
    constexpr auto rhs = Vec{2.0, 2.0, 2.0};
    Vec<double, 3> v4 = Vec{1.0, 2.0, 3.0};
    v4 += rhs;
    ASSERT_EQ(v4, v2);
    v4 = Vec{1.0, 2.0, 3.0} + rhs;
    ASSERT_EQ(v4, v2);
}

TEST(Vec, opeartor_minus)
{
    static_assert((Vec{1.0, 2.0, 3.0} - 2.0) == Vec{-1.0, 0.0, 1.0}, "operator-");
    static_assert((Vec{1.0, 2.0, 3.0} - Vec{2.0, 2.0, 2.0}) == Vec{-1.0, 0.0, 1.0}, "operator-");
    constexpr auto v1 = Vec{1.0, 2.0, 3.0} - 2.0;
    constexpr auto v2 = Vec{-1.0, 0.0, 1.0};
    ASSERT_EQ(v1, v2);
    Vec<double, 3> v3 = Vec{1.0, 2.0, 3.0};
    v3 -= 2.0;
    ASSERT_EQ(v3, v2);
    constexpr auto rhs = Vec{2.0, 2.0, 2.0};
    Vec<double, 3> v4 = Vec{1.0, 2.0, 3.0};
    v4 -= rhs;
    ASSERT_EQ(v4, v2);
    v4 = Vec{1.0, 2.0, 3.0} - rhs;
    ASSERT_EQ(v4, v2);
}

TEST(Vec, opeartor_mul)
{
    static_assert((Vec{1.0, 2.0, 3.0} * 2.0) == Vec{2.0, 4.0, 6.0}, "operator*");
    static_assert((Vec{1.0, 2.0, 3.0} * Vec{2.0, 2.0, 2.0}) == Vec{2.0, 4.0, 6.0}, "operator*");
    constexpr auto v1 = Vec{1.0, 2.0, 3.0} * 2.0;
    constexpr auto v2 = Vec{2.0, 4.0, 6.0};
    ASSERT_EQ(v1, v2);
    Vec<double, 3> v3 = Vec{1.0, 2.0, 3.0};
    v3 *= 2.0;
    ASSERT_EQ(v3, v2);
    constexpr auto rhs = Vec{2.0, 2.0, 2.0};
    Vec<double, 3> v4 = Vec{1.0, 2.0, 3.0};
    v4 *= rhs;
    ASSERT_EQ(v4, v2);
    v4 = Vec{1.0, 2.0, 3.0} * rhs;
    ASSERT_EQ(v4, v2);
}

TEST(Vec, opeartor_div)
{
    static_assert((Vec{2.0, 4.0, 6.0} / 2.0) == Vec{1.0, 2.0, 3.0}, "operator/");
    static_assert((Vec{2.0, 4.0, 6.0} / Vec{2.0, 2.0, 2.0}) == Vec{1.0, 2.0, 3.0}, "operator/");
    constexpr auto v1 = Vec{2.0, 4.0, 6.0} / 2.0;
    constexpr auto v2 = Vec{1.0, 2.0, 3.0};
    ASSERT_EQ(v1, v2);
    Vec<double, 3> v3 = Vec{2.0, 4.0, 6.0};
    v3 /= 2.0;
    ASSERT_EQ(v3, v2);
    constexpr auto rhs = Vec{2.0, 2.0, 2.0};
    Vec<double, 3> v4 = Vec{2.0, 4.0, 6.0};
    v4 /= rhs;
    ASSERT_EQ(v4, v2);
    v4 = Vec{2.0, 4.0, 6.0} / rhs;
    ASSERT_EQ(v4, v2);
}

TEST(Vec, dot_product)
{
    constexpr Vec v1{1.0, 2.0, 3.0};
    constexpr Vec v2{2.0, 4.0, 6.0};
    ASSERT_TRUE(v1.DotProduct(v2) == 28.0);
}

TEST(Vec, unormalized_magnitude)
{
    constexpr Vec<double, 5> v1({5.1, 6.1, 8.1, 7.1, 9.1});
    constexpr Vec<double, 5> v2({6.1, 5.1, 7.1, 8.1, 9.1});
    ASSERT_TRUE(v1.Magnitude() == v2.Magnitude());
    constexpr Vec<double, 3> v3({1, 2, 2});
    ASSERT_TRUE(v3.Magnitude() == 3);
}

TEST(Vec, normalized)
{
    constexpr Vec<double, 5> v1({5.1, 6.1, 8.1, 7.1, 9.1});
    constexpr Vec<double, 5> v2({5.1, 6.1, 8.1, 7.1, 9.1});
    ASSERT_TRUE(v1.Normalize() == v2.Normalize());
}

TEST(Vec, normalized_magnitude)
{
    constexpr Vec<double, 5> v1({5.1, 6.1, 8.1, 7.1, 9.1});
    constexpr Vec<double, 5> v2({30.1, 40.1, 70.1, 80.1, 219.1});
    ASSERT_TRUE(v1.Normalize().Magnitude() == v2.Normalize().Magnitude());
    ASSERT_TRUE(v1.Normalize().Magnitude() == 1.0);
}

TEST(Vec, unnormalized_dot_product)
{
    constexpr Vec<double, 3> v1({5, 3, 4});
    constexpr Vec<double, 3> v2({5, 3, 4});
    ASSERT_EQ(v1.DotProduct(v2), 50);
    constexpr Vec<double, 3> v3({-5, 3, 4});
    ASSERT_EQ(v1.DotProduct(v3), 0);
}

TEST(Tuple, constructor)
{
    /* vector initialization from list constructor*/
    constexpr Tuple vec1({0, 1, 2, TupleConstants::VectorFlag});
    constexpr Tuple vec2({0, 1, 2, TupleConstants::VectorFlag});
    ASSERT_THAT(vec1.size(), Eq(4));
    ASSERT_EQ(vec1.size(), vec2.size());
    ASSERT_TRUE(vec1 == MakeVector(0, 1, 2));

    /* Point initialization from list constructor */
    constexpr Tuple pt1{1, 2, 3, TupleConstants::PointFlag};
    constexpr Tuple pt2 = {1, 2, 3, TupleConstants::PointFlag};
    ASSERT_THAT(pt1.size(), Eq(4));
    ASSERT_TRUE(pt1 == pt2);
    ASSERT_TRUE(pt1 == MakePoint(1, 2, 3));

    ASSERT_FALSE(vec1 == MakePoint(1, 2, 3));
}

TEST(Tuple, opeartor_negate)
{
    constexpr auto vec1 = MakeVector(1.0, 2.0, 3.0);
    constexpr auto vec2 = MakeVector(-1.0, -2.0, -3.0);
    ASSERT_EQ(-vec1, vec2);
    ASSERT_EQ(-1.0 * vec1, vec2);
    ASSERT_EQ(vec1 * -1.0, vec2);

    constexpr auto pt1 = MakePoint(1.0, 2.0, 3.0);
    constexpr auto pt2 = MakePoint(-1.0, -2.0, -3.0);
    ASSERT_EQ(-pt1, pt2);
    ASSERT_EQ(-1.0 * pt1, pt2);
    ASSERT_EQ(pt1 * -1.0, pt2);
}

TEST(Tuple, opeartor_plus)
{
    constexpr auto vec1 = MakeVector(1.0, 2.0, 3.0) + 2.0;
    constexpr auto vec2 = MakeVector(3.0, 4.0, 5.0);
    ASSERT_EQ(vec1, vec2);
    auto vec3 = MakeVector(1.0, 2.0, 3.0);
    vec3 += 2.0;
    ASSERT_EQ(vec3, vec2);
    constexpr auto rhs = MakeVector(2.0, 2.0, 2.0);
    auto vec4 = MakeVector(1.0, 2.0, 3.0);
    vec4 += rhs;
    ASSERT_EQ(vec4, vec2);
    vec4 = MakeVector(1.0, 2.0, 3.0) + rhs;
    ASSERT_EQ(vec4, vec2);

    constexpr auto pt1 = MakePoint(1.0, 2.0, 3.0) + 2.0;
    constexpr auto pt2 = MakePoint(3.0, 4.0, 5.0);
    ASSERT_EQ(pt1, pt2);
    auto pt3 = MakePoint(1.0, 2.0, 3.0);
    pt3 += 2.0;
    ASSERT_EQ(pt3, pt2);
    constexpr auto rhs_pt = MakePoint(2.0, 2.0, 2.0);
    auto pt4 = MakePoint(1.0, 2.0, 3.0);
    pt4 += rhs_pt;
    ASSERT_EQ(pt4, pt2);
    pt4 = MakePoint(1.0, 2.0, 3.0) + rhs_pt;
    ASSERT_EQ(pt4, pt2);
}

TEST(Tuple, opeartor_minus)
{
    constexpr auto vec1 = MakeVector(3.0, 4.0, 5.0) - 2.0;
    constexpr auto vec2 = MakeVector(1.0, 2.0, 3.0);
    ASSERT_EQ(vec1, vec2);
    auto vec3 = MakeVector(3.0, 4.0, 5.0);
    vec3 -= 2.0;
    ASSERT_EQ(vec3, vec2);
    constexpr auto rhs = MakeVector(2.0, 2.0, 2.0);
    auto vec4 = MakeVector(3.0, 4.0, 5.0);
    vec4 -= rhs;
    ASSERT_EQ(vec4, vec2);
    vec4 = MakeVector(3.0, 4.0, 5.0) - rhs;
    ASSERT_EQ(vec4, vec2);

    constexpr auto pt1 = MakePoint(3.0, 4.0, 5.0) - 2.0;
    constexpr auto pt2 = MakePoint(1.0, 2.0, 3.0);
    ASSERT_EQ(pt1, pt2);
    auto pt3 = MakePoint(3.0, 4.0, 5.0);
    pt3 -= 2.0;
    ASSERT_EQ(pt3, pt2);
    constexpr auto rhs_pt = MakePoint(2.0, 2.0, 2.0);
    auto pt4 = MakePoint(3.0, 4.0, 5.0);
    pt4 -= rhs_pt;
    ASSERT_EQ(pt4, pt2);
    pt4 = MakePoint(3.0, 4.0, 5.0) - rhs_pt;
    ASSERT_EQ(pt4, pt2);
}

TEST(Tuple, opeartor_mul)
{
    constexpr auto vec1 = MakeVector(1.0, 2.0, 3.0) * 2.0;
    constexpr auto vec2 = MakeVector(2.0, 4.0, 6.0);
    ASSERT_EQ(vec1, vec2);
    auto vec3 = MakeVector(1.0, 2.0, 3.0);
    vec3 *= 2.0;
    ASSERT_EQ(vec3, vec2);
    constexpr auto rhs = MakeVector(2.0, 2.0, 2.0);
    auto vec4 = MakeVector(1.0, 2.0, 3.0);
    vec4 *= rhs;
    ASSERT_EQ(vec4, vec2);

    vec4 = MakeVector(1.0, 2.0, 3.0) * rhs;
    ASSERT_EQ(vec4, vec2);

    constexpr auto vec5 = 2.0 * MakeVector(1.0, 2.0, 3.0);
    ASSERT_EQ(vec4, vec5);

    constexpr auto pt1 = MakePoint(1.0, 2.0, 3.0) * 2.0;
    constexpr auto pt2 = MakePoint(2.0, 4.0, 6.0);
    ASSERT_EQ(pt1, pt2);
    auto pt3 = MakePoint(1.0, 2.0, 3.0);
    pt3 *= 2.0;
    ASSERT_EQ(pt3, pt2);
    constexpr auto rhs_pt = MakePoint(2.0, 2.0, 2.0);
    auto pt4 = MakePoint(1.0, 2.0, 3.0);
    pt4 *= rhs_pt;
    ASSERT_EQ(pt4, pt2);
    pt4 = MakePoint(1.0, 2.0, 3.0) * rhs_pt;
    ASSERT_EQ(pt4, pt2);
}

TEST(Tuple, opeartor_div)
{
    constexpr auto vec1 = MakeVector(2.0, 4.0, 6.0) / 2.0;
    constexpr auto vec2 = MakeVector(1.0, 2.0, 3.0);
    ASSERT_EQ(vec1, vec2);
    auto vec3 = MakeVector(2.0, 4.0, 6.0);
    vec3 /= 2.0;
    ASSERT_EQ(vec3, vec2);
    constexpr auto rhs = MakeVector(2.0, 2.0, 2.0);
    auto vec4 = MakeVector(2.0, 4.0, 6.0);
    vec4 /= rhs;
    ASSERT_EQ(vec4, vec2);

    vec4 = MakeVector(2.0, 4.0, 6.0) / rhs;
    ASSERT_EQ(vec4, vec2);

    constexpr auto pt1 = MakePoint(2.0, 4.0, 6.0) / 2.0;
    constexpr auto pt2 = MakePoint(1.0, 2.0, 3.0);
    ASSERT_EQ(pt1, pt2);
    auto pt3 = MakePoint(2.0, 4.0, 6.0);
    pt3 /= 2.0;
    ASSERT_EQ(pt3, pt2);
    constexpr auto rhs_pt = MakePoint(2.0, 2.0, 2.0);
    auto pt4 = MakePoint(2.0, 4.0, 6.0);
    pt4 /= rhs_pt;
    ASSERT_EQ(pt4, pt2);
    pt4 = MakePoint(2.0, 4.0, 6.0) / rhs_pt;
    ASSERT_EQ(pt4, pt2);
}

TEST(Tuple, cross_product)
{
    Tuple x{1, 2, 3, TupleConstants::VectorFlag};
    Tuple y{2, 4, 6, TupleConstants::VectorFlag};
    Tuple z = x.CrossProduct(y);
    ASSERT_TRUE(z == PredefinedTuples::ZeroVector);
    Tuple v1{1, 2, 3, TupleConstants::VectorFlag};
    Tuple v2{1, 5, 7, TupleConstants::VectorFlag};
    Tuple actual = v1.CrossProduct(v2);
    constexpr Tuple expected{-1, -4, 3, TupleConstants::VectorFlag};
    ASSERT_FALSE(v2.CrossProduct(v1) == v1.CrossProduct(v2));
    ASSERT_TRUE(actual == expected);
}

TEST(Tuple, reflect)
{
    // Reflecting a vector approaching at 45 degrees
    constexpr auto v = MakeVector(1.0, -1.0, 0.0);
    constexpr auto n = MakeVector(0.0, 1.0, 0.0);
    constexpr auto r = v.Reflect(n);
    ASSERT_EQ(r, MakeVector(1.0, 1.0, 0.0));

    //  Reflecting a vector off a slanted surface
    constexpr auto sqrt_2_by_2 = MathUtils::ConstExprSqrtf(2.0) / 2;
    const auto v2 = MakeVector(0.0, -1.0, 0.0);
    const auto n2 = MakeVector(sqrt_2_by_2, sqrt_2_by_2, 0.0);
    const auto r2 = v2.Reflect(n2);
    ASSERT_EQ(r2, MakeVector(1.0, 0.0, 0.0));
}

TEST(Tuple, dot_product)
{
    constexpr Tuple vec1{1, 2, 3, TupleConstants::VectorFlag};
    constexpr Tuple vec2{2, 4, 6, TupleConstants::VectorFlag};
    ASSERT_TRUE(vec1.DotProduct(vec2) == 28);

    constexpr Tuple pt1{1, 2, 3, TupleConstants::PointFlag};
    constexpr Tuple pt2{2, 4, 6, TupleConstants::PointFlag};
    ASSERT_TRUE(pt1.DotProduct(pt2) == 28);
}

TEST(Tuple, point_to_vector)
{
    Tuple px{1.0, 2.0, 3.0, TupleConstants::PointFlag};
    Tuple py{-1.0, -2.0, -3.0, TupleConstants::PointFlag};
    auto vx = ToVector(px);
    auto vxExpected = MakeVector(1.0, 2.0, 3.0);
    ASSERT_EQ(vxExpected, vx);

    auto vxNorm = ToNormalizedVector(px);
    auto vxExpectedNorm = MakeNormalizedVector(1.0, 2.0, 3.0);
    ASSERT_EQ(vxExpectedNorm, vxNorm);

    auto vxy = ToVector(py - px);
    auto vxyExpected = MakeVector(-2.0, -4.0, -6.0);
    ASSERT_EQ(vxyExpected, vxy);
}

TEST(Colour, colour_are_rgb_tuple)
{
    constexpr Colour rgb{-0.5, 0.4, 1.7};
    ASSERT_TRUE(MathUtils::ApproxEqual(rgb[ColourConstants::r], -0.5));
    ASSERT_TRUE(MathUtils::ApproxEqual(rgb[ColourConstants::g], 0.4));
    ASSERT_TRUE(MathUtils::ApproxEqual(rgb[ColourConstants::b], 1.7));
}

TEST(Colour, colour_addition)
{
    constexpr Colour c1{0.9, 0.6, 0.75};
    constexpr Colour c2{0.7, 0.1, 0.25};
    constexpr Colour actual = c1 + c2;
    constexpr Colour expected{1.6, 0.7, 1.0};
    ASSERT_TRUE(actual == expected);
}

TEST(Colour, colour_subtraction)
{
    constexpr Colour c1{0.9, 0.6, 0.75};
    constexpr Colour c2{0.7, 0.1, 0.25};
    constexpr auto actual = c1 - c2;
    constexpr Colour expected{0.2, 0.5, 0.5};
    ASSERT_TRUE(actual == expected);
}

TEST(Colour, Colour_Multiplication_By_Scalar)
{
    constexpr Colour c{0.2, 0.3, 0.4};
    constexpr auto actual = 2.0 * c;
    constexpr Colour expected{0.4, 0.6, 0.8};
    ASSERT_TRUE(actual == expected);
}

TEST(Colour, colour_hadamard_product)
{
    constexpr Colour c1{1, 0.2, 0.4};
    constexpr Colour c2{0.9, 1, 0.1};
    constexpr auto actual = c1 * c2;
    constexpr Colour expected{0.9, 0.2, 0.04};
    ASSERT_TRUE(actual == expected);
}

TEST(Colour, valid)
{
    constexpr Colour validC{0.2, 0.2, 0.4};
    ASSERT_TRUE(IsValidColour(validC));
    constexpr Colour invalidC{1.2, -0.2, 0.4};
    ASSERT_FALSE(IsValidColour(invalidC));
}