#ifndef CAMERA_HH
#define CAMERA_HH
#include <canvas.hh>
#include <ray.hh>
#include <transform.hh>
#include <utils/math.hh>
namespace RayTracer {
class Camera final {
 private:
  /// horizontal size of canvas (in pixels)
  std::size_t hsize;

  /// vertical size of canvas (in pixels)
  std::size_t vsize;

  /// how much the camera can see.
  double fov;

  /// half of canvas width in world unit
  double halfWidth;

  /// half of canvas height in world unit
  double halfHeight;

  /// how much one pixel equal to world unit
  double pixelSize;

  /// a matrix describing how the world should be oriented relative
  /// to the camera (camera to world matrix)
  Transform transform;

 public:
  constexpr Camera(
      const std::size_t hsize_, const std::size_t vsize_, const double fov_,
      const Transform& transform_ = PredefinedMatrices::I<double, 4>)
      : hsize{hsize_}, vsize{vsize_}, fov{fov_}, transform{transform_} {
    const auto halfView = MathUtils::Tangent(fov / 2);
    const auto aspectRatio =
        (static_cast<double>(hsize) / static_cast<double>(vsize));

    // image width >= image height (in pixels)
    // meaning that there is more pixels along the y axis then there is along the x axis
    // this give us a wider-shape pixel instead of a square pixels
    // to fix we could divide the longer side (x-axis in this case) by aspect ratio
    //
    //                      200-pixels
    //                    _______________
    //                    |             |
    //      2-world-units |             | 125-pixels
    //                    |_____________|
    //                     2-world-units

    if (aspectRatio >= 1.0) {
      halfWidth = halfView;
      halfHeight = halfView / aspectRatio;
    } else {
      halfWidth = halfView * aspectRatio;
      halfHeight = halfView;
    }
    // computing the size of a single pixel on the canvas by dividing the full width of the canvas(half width * 2)
    // by the horizontal size(in pixels) of the canvas (hsize)
    pixelSize = (halfWidth * 2) / (hsize);
  }

  std::size_t HorizontalSize() const { return hsize; }
  std::size_t VerticalSize() const { return vsize; }
  double FieldOfView() const { return fov; }
  double PixelSize() const { return pixelSize; }
  Transform TransformMatrix() const { return transform; }

  /** Emit a new ray that start at the camera and pass through the indicaed pixel location on the canvas.
   * @param x  x-location on canvas(in pixels)
   * @param y  y-location on canvas(in pixels)
   */
  constexpr Ray RayForPixel(double px, double py) const noexcept {
    // the offset from the edge of the canvas to the pixel's center
    // the reason we add a small shift (0.5) to the pixel position because we want the final camera ray to pass through the middle of the pixel
    const double xOffset = (px + 0.5) * pixelSize;  //in world-units
    const double yOffset = (py + 0.5) * pixelSize;  //in world-units
    // the untransformed coordinate of the pixel in world-space
    // (since camera look toward -z, so +x axis is to the left)
    const double xWorld = halfWidth - xOffset;
    const double yWorld = halfHeight - yOffset;

    // using the camera matrix transform the canvas point and the origin,
    // then compute the ray's direction vector
    // (remember that the canvas is at z = -1)
    const auto w2c = Inverse(transform);

    const auto pixelCam =
        w2c * MakePoint(xWorld, yWorld, -1);          // in camera space
    const auto originCam = w2c * MakePoint(0, 0, 0);  // in camera space

    const auto directionCam = ToNormalizedVector(pixelCam - originCam);
    return Ray{originCam, directionCam};
  }

  template <std::size_t W, std::size_t H, class World>
  constexpr auto render(const World& world) const {
    auto image = [&]() {
      Canvas<W, H> canvas;
      for (std::size_t y = 0; y < H; ++y) {
        for (std::size_t x = 0; x < W; ++x) {
          const auto ray = RayForPixel(x, y);
          const auto colour = world.ColorAt(ray);
          canvas(y, x) = colour;
        }
      }
      return canvas;
    }();

    return image;
  }
};
}  // namespace RayTracer

#endif
