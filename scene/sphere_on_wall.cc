#include <canvas.hh>
#include <shape.hh>
#include <vec.hh>
using namespace RayTracer;

template <int I, int... N>
struct RenderLooper {
    template <typename F, typename... X>
    constexpr void operator()(F& f, X... x)
    {
        for (int i = 0; i < I; ++i) {
            RenderLooper<N...>()(f, x..., i);
        }
    }
};

template <int I>
struct RenderLooper<I> {
    template <typename F, typename... X>
    constexpr void operator()(F& f, X... x)
    {
        for (int i = 0; i < I; ++i) {
            f(x..., i);
        }
    }
};

int main()
{
    constexpr std::size_t canvasPixels = 100;
    Canvas<canvasPixels, canvasPixels> canvas;
    constexpr auto plotColor = MakeColour(1, 0, 0); // red
    constexpr auto rayOrigin = MakePoint(0, 0, -5);
    constexpr double wallSize = 7.0; // world-unit
    constexpr double wallZ = 10.0;
    constexpr double wallHalf = wallSize * 0.5;
    constexpr double pixelSize = wallSize / static_cast<double>(canvasPixels);
    constexpr Sphere sphere;
    static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
    auto renderOnce = [&](int y, int x) {
        const double worldY = wallHalf - pixelSize * static_cast<double>(y);
        const double worldX = -wallHalf + pixelSize * static_cast<double>(x);
        const auto pointOnWall = MakePoint(worldX, worldY, wallZ);
        const auto r = Ray(rayOrigin, ToNormalizedVector(pointOnWall - rayOrigin));
        const auto xs = sphere.IntersectWith(r, &shapeWrapper);
        const auto I = IntersectionUtils::VisibleHit(xs);
        if (I.has_value())
            canvas(x, y) = plotColor;
    };
    // for each row of pixels in the canvas
    //  compute the world y-coordinate (and don't forget the y-coordinate is upside-down)
    RenderLooper<canvasPixels, canvasPixels>()(renderOnce);

    canvas.ToPPM("sphere_on_wall.ppm");
}
