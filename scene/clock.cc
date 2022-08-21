#include <canvas.hh>
#include <matrix.hh>
#include <transform.hh>
#include <utils/math.hh>
#include <vec.hh>
using namespace RayTracer;
template <std::size_t canvasPixels>
constexpr auto renderStaticChapter4() {
  constexpr auto image = []() {
    Canvas<canvasPixels, canvasPixels> canvas;
    auto radius = 150;
    auto plotColor = MakeColour(1, 1, 1);
    // rotation angle needed to advance one hour
    auto rotationAngle = MathUtils::MathConstants::PI<double> / 6;

    // translation to get to the middle of the canvas
    auto translationMat =
        MatrixUtils::Translation(canvasPixels / 2, canvasPixels / 2, 0.0);

    // scale matrix for moving x, y component of the computing point by multipling the radius
    auto scaleMat = MatrixUtils::Scale(radius, radius, 1);

    // for each computed point, first multiply x,y component by radius then move them to the center of your canvas
    auto tx = translationMat * scaleMat;

    // draw clock center
    canvas(canvasPixels / 2, canvasPixels / 2) = plotColor;

    auto tweleveClock = MakePoint(0, 1, 0);
    for (auto clk : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}) {
      // counter clock wise rotation
      auto rotate = MatrixUtils::RotateZ(clk * -rotationAngle);
      auto computedClk = tx * rotate * tweleveClock;
      // note the y-coordinate is upside-down, subtract the point's y from the canvas's height

      canvas((int)computedClk[TupleConstants::x],
             canvasPixels - (int)computedClk[TupleConstants::y]) = plotColor;
    }
    return canvas;
  }();

  return image;
}
int main() {
  static constexpr std::size_t canvasPixels = 400;
  auto canvas = renderStaticChapter4<canvasPixels>();
  canvas.ToPPM("clock.ppm");
}
