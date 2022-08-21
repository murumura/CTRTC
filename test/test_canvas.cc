#include <gtest/gtest.h>
#include <canvas.hh>
using namespace RayTracer;
using testing::Eq;

constexpr std::size_t width = 10;
constexpr std::size_t height = 10;

TEST(Canvas, black_background) {
  Canvas<width, height> canvas;
  EXPECT_EQ(canvas.GetWidth(), width);
  EXPECT_EQ(canvas.GetHeight(), height);
  for (auto i = 0; i < width; i++)
    for (auto j = 0; j < height; j++)
      EXPECT_EQ(canvas(i, j), PredefinedColours::BLACK);
}

TEST(Canvas, write_to_canvas) {
  Canvas<width, height> canvas;
  EXPECT_EQ(canvas.GetWidth(), width);
  EXPECT_EQ(canvas.GetHeight(), height);

  for (auto i = 0; i < width; i++)
    for (auto j = 0; j < height; j++)
      canvas(i, j) = PredefinedColours::WHITE;

  for (auto i = 0; i < width; i++)
    for (auto j = 0; j < height; j++)
      EXPECT_EQ(canvas(i, j), PredefinedColours::WHITE);
}

TEST(Canvas, to_ppm) {
  Canvas<width, height> canvas;

  for (auto i = 0; i < width; i++)
    for (auto j = 0; j < height; j++)
      canvas(i, j) = PredefinedColours::WHITE;

  canvas.ToPPM("test.ppm");
  EXPECT_EQ(0, remove("test.ppm"));
}
