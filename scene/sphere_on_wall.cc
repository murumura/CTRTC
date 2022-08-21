#include <canvas.hh>
#include <shape.hh>
#include <vec.hh>
using namespace RayTracer;
#ifdef COMPILETIME
template <std::size_t canvasPixels>
constexpr auto RenderStaticChapter5() {
  constexpr auto image = []() {
    Canvas<canvasPixels, canvasPixels> canvas;
    auto plotColor = MakeColour(1, 0, 0);  // red
    auto rayOrigin = MakePoint(0, 0, -5);
    auto wallSize = 7.0;  // world-unit
    auto wallZ = 10.0;
    auto wallHalf = wallSize * 0.5;
    auto pixelSize = wallSize / static_cast<double>(canvasPixels);
    Sphere sphere;
    ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
    for (int y = 0; y < canvasPixels; y++) {
      double worldY = wallHalf - pixelSize * static_cast<double>(y);
      for (int x = 0; x < canvasPixels; x++) {
        auto worldX = -wallHalf + pixelSize * static_cast<double>(x);
        auto pointOnWall = MakePoint(worldX, worldY, wallZ);
        auto r = Ray(rayOrigin, ToNormalizedVector(pointOnWall - rayOrigin));
        auto xs = sphere.IntersectWith(r, &shapeWrapper);
        auto I = IntersectionUtils::VisibleHit(xs);
        if (I.has_value())
          canvas(x, y) = plotColor;
      }
    }
    return canvas;
  }();

  return image;
}
#endif
int main() {
  /*
        If you want to build a compile time tracer,
        it is best not to adjust the canvas size too much, 100-300 pixels would be a good choice,
        otherwise it will take a long time to compile
    */
  static constexpr std::size_t canvasPixels = 50;
#ifdef COMPILETIME
  auto canvas = RenderStaticChapter5<canvasPixels>();
#else
  Canvas<canvasPixels, canvasPixels> canvas;
  constexpr auto plotColor = MakeColour(1, 0, 0);  // red
  constexpr auto rayOrigin = MakePoint(0, 0, -5);
  constexpr double wallSize = 7.0;  // world-unit
  constexpr double wallZ = 10.0;
  constexpr double wallHalf = wallSize * 0.5;
  constexpr double pixelSize = wallSize / static_cast<double>(canvasPixels);
  constexpr Sphere sphere;
  static constexpr ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
  for (int y = 0; y < canvasPixels; y++) {
    //  compute the world y-coordinate (and don't forget the y-coordinate is upside-down)
    double worldY = wallHalf - pixelSize * static_cast<double>(y);
    for (int x = 0; x < canvasPixels; x++) {
      auto worldX = -wallHalf + pixelSize * static_cast<double>(x);
      auto pointOnWall = MakePoint(worldX, worldY, wallZ);
      auto r = Ray(rayOrigin, ToNormalizedVector(pointOnWall - rayOrigin));
      auto xs = sphere.IntersectWith(r, &shapeWrapper);
      auto I = IntersectionUtils::VisibleHit(xs);
      if (I.has_value())
        canvas(x, y) = plotColor;
    }
  }
#endif
  canvas.ToPPM("sphere_on_wall.ppm");
}
