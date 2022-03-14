#include <canvas.hh>
#include <matrix.hh>
#include <transform.hh>
#include <utils/math.hh>
#include <vec.hh>
using namespace RayTracer;
int main()
{
    constexpr auto width = 400;
    constexpr auto height = 400;
    constexpr auto radius = 150;
    Canvas<width, height> canvas;
    constexpr auto plotColor = MakeColour(1, 1, 1);

    // rotation angle needed to advance one hour
    constexpr auto rotationAngle = MathUtils::MathConstants::PI<double> / 6;

    // translation to get to the middle of the canvas
    constexpr auto translationMat = MatrixUtils::Translation(width / 2, height / 2, 0.0);

    // scale matrix for moving x, y component of the computing point by multipling the radius
    constexpr auto scaleMat = MatrixUtils::Scale(radius, radius, 1);

    // for each computed point, first multiply x,y component by radius then move them to the center of your canvas
    constexpr auto tx = translationMat * scaleMat;

    // draw clock center
    canvas(width / 2, height / 2) = plotColor;
    constexpr auto tweleveClock = MakePoint(0, 1, 0);

    for (auto clk : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}) {
        // counter clock wise rotation
        Transform rotate = MatrixUtils::RotateZ(clk * -rotationAngle);
        Tuple computedClk = tx * rotate * tweleveClock;
        // note the y-coordinate is upside-down, subtract the point's y from the canvas's height

        canvas((int)computedClk[TupleConstants::x], height - (int)computedClk[TupleConstants::y]) = plotColor;
    }
    canvas.ToPPM("clock.ppm");
}
