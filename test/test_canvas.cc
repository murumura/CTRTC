#include <canvas.hh>
#include <gmock/gmock.h>
using namespace RayTracer;
using testing::Eq;

constexpr std::size_t width = 10;
constexpr std::size_t height = 10;

TEST(Canvas, black_background)
{
    Canvas<width, height> canvas;
    ASSERT_EQ(canvas.GetWidth(), width);
    ASSERT_EQ(canvas.GetHeight(), height);
    for (auto i = 0; i < width; i++)
        for (auto j = 0; j < height; j++)
            ASSERT_EQ(canvas(i, j), PredefinedColours::BLACK);
}

TEST(Canvas, write_to_canvas)
{
    Canvas<width, height> canvas;
    ASSERT_EQ(canvas.GetWidth(), width);
    ASSERT_EQ(canvas.GetHeight(), height);

    for (auto i = 0; i < width; i++)
        for (auto j = 0; j < height; j++)
            canvas(i, j) = PredefinedColours::WHITE;

    for (auto i = 0; i < width; i++)
        for (auto j = 0; j < height; j++)
            ASSERT_EQ(canvas(i, j), PredefinedColours::WHITE);
}

TEST(Canvas, to_ppm)
{
    Canvas<width, height> canvas;

    for (auto i = 0; i < width; i++)
        for (auto j = 0; j < height; j++)
            canvas(i, j) = PredefinedColours::WHITE;

    canvas.ToPPM("test.ppm");
    ASSERT_EQ(0, remove("test.ppm"));
}
