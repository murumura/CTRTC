#include <canvas.hh>
#include <shape.hh>
#include <vec.hh>
using namespace RayTracer;
#ifdef COMPILETIME
template <std::size_t canvasPixels>
constexpr auto RenderStaticChapter6() {
  constexpr auto image = []() {
    Canvas<canvasPixels, canvasPixels> canvas;
    auto rayOrigin = MakePoint(0, 0, -5);
    auto wallSize = 7.0;  // world-unit
    auto wallZ = 10.0;
    auto wallHalf = wallSize * 0.5;
    auto pixelSize = wallSize / static_cast<double>(canvasPixels);
    // Give sphere a purple color
    Material m = []() {
      Material ret;
      ret.color = MakeColour(1, 0.2, 1);
      return ret;
    }();
    Sphere sphere = Sphere{m};
    ShapeWrapper shapeWrapper = ShapeWrapper(sphere);
    // Add light source
    auto lightPoint = MakePoint(-10, 10, -10);
    auto lightColour = MakeColour(1, 1, 1);  // white
    auto light = PointLight(lightPoint, lightColour);
    for (int y = 0; y < canvasPixels; y++) {
      double worldY = wallHalf - pixelSize * static_cast<double>(y);
      for (int x = 0; x < canvasPixels; x++) {
        auto worldX = -wallHalf + pixelSize * static_cast<double>(x);
        auto pointOnWall = MakePoint(worldX, worldY, wallZ);
        auto r = Ray(rayOrigin, ToNormalizedVector(pointOnWall - rayOrigin));
        auto xs = sphere.IntersectWith(r, &shapeWrapper);
        auto I = IntersectionUtils::VisibleHit(xs);
        if (I.has_value()) {
          auto& nearestHit = I.value();
          // nearest intersection point in world-space
          auto point = r.PositionAlong(nearestHit.GetIntersectDistance());
          auto normal = nearestHit.shapePtr->GetWorldNormalAt(point);
          auto eye = -r.GetDirection();
          auto plotColor = lighting(m, light, point, eye, normal);
          canvas(x, y) = plotColor;
        }
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
  static constexpr std::size_t canvasPixels = 260;
#ifdef COMPILETIME
  constexpr auto canvas = RenderStaticChapter6<canvasPixels>();
#else
  Canvas<canvasPixels, canvasPixels> canvas;

  constexpr auto rayOrigin = MakePoint(0, 0, -5);

  constexpr double wallSize = 7.0;  // world-unit
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
  constexpr auto lightColour = MakeColour(1, 1, 1);  // white
  constexpr auto light = PointLight(lightPoint, lightColour);
  for (int y = 0; y < canvasPixels; y++) {
    double worldY = wallHalf - pixelSize * static_cast<double>(y);
    for (int x = 0; x < canvasPixels; x++) {
      auto worldX = -wallHalf + pixelSize * static_cast<double>(x);
      auto pointOnWall = MakePoint(worldX, worldY, wallZ);
      auto r = Ray(rayOrigin, ToNormalizedVector(pointOnWall - rayOrigin));
      auto xs = sphere.IntersectWith(r, &shapeWrapper);
      auto I = IntersectionUtils::VisibleHit(xs);
      if (I.has_value()) {
        auto& nearestHit = I.value();
        // nearest intersection point in world-space
        auto point = r.PositionAlong(nearestHit.GetIntersectDistance());
        auto normal = nearestHit.shapePtr->GetWorldNormalAt(point);
        auto eye = -r.GetDirection();
        auto plotColor = lighting(m, light, point, eye, normal);
        canvas(x, y) = plotColor;
      }
    }
  }
#endif
  canvas.ToPPM("SphereSilhouette.ppm");
}
