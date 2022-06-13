#include <canvas.hh>
#include <shape.hh>
#include <vec.hh>
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

using namespace RayTracer;
int main()
{
    constexpr std::size_t canvasPixels = 500;
    Canvas<canvasPixels, canvasPixels> canvas;

    constexpr auto rayOrigin = MakePoint(0, 0, -5);

    constexpr double wallSize = 7.0; // world-unit
    constexpr double wallZ = 10.0;
    constexpr double wallHalf = wallSize * 0.5;
    constexpr double pixelSize = wallSize / static_cast<double>(canvasPixels);

    // Give sphere a purple color
    constexpr Material m = []() {
        Material ret;
        ret.color = MakeColour(1, 0.2, 1);
        return ret;
    }();
    constexpr Sphere sphere = Sphere{m};
    static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
    // Add light source
    constexpr auto lightPoint = MakePoint(-10, 10, -10);
    constexpr auto lightColour = MakeColour(1, 1, 1); // white
    constexpr auto light = PointLight(lightPoint, lightColour);

    auto renderOnce = [&](int y, int x) {
        const double worldY = wallHalf - pixelSize * static_cast<double>(y);
        const double worldX = -wallHalf + pixelSize * static_cast<double>(x);
        const auto pointOnWall = MakePoint(worldX, worldY, wallZ);
        const auto r = Ray(rayOrigin, ToNormalizedVector(pointOnWall - rayOrigin));
        const auto xs = sphere.IntersectWith(r, &shapeWrapper);
        const auto I = IntersectionUtils::VisibleHit(xs);
        if (I.has_value()) {
            const auto& nearestHit = I.value();
            // nearest intersection point in world-space
            const auto point = r.PositionAlong(nearestHit.GetIntersectDistance());
            const auto normal = nearestHit.shapePtr->GetWorldNormalAt(point);
            const auto eye = -r.GetDirection();
            const auto plotColor = lighting(m, light, point, eye, normal);
            canvas(x, y) = plotColor;
        }
    };
    RenderLooper<canvasPixels, canvasPixels>()(renderOnce);
    canvas.ToPPM("SphereSilhouette.ppm");
}