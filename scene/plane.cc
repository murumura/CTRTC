#include <camera.hh>

#include <primitives.hh>
#include <vec.hh>
#include <world.hh>
using namespace RayTracer;

int main() {
  /*
    If you want to build a compile time tracer,
    it is best not to adjust the canvas size too much, 100-300 pixels would be a good choice,
    otherwise it will take a long time to compile
  */

  constexpr auto pi = MathUtils::MathConstants::PI<double>;
  static constexpr std::size_t hsize = 180;
  static constexpr std::size_t vsize = 150;

  constexpr Material floorMaterial = []() {
    Material ret;
    ret.color = MakeColour(1.0, 0.9, 0.9);
    ret.specular = 0.0;
    return ret;
  }();
  constexpr Plane floor = [&]() {
    Plane ret;
    ret.SetMaterial(floorMaterial);
    return ret;
  }();

  constexpr Transform roofTransform = MatrixUtils::Translation(0.0, 10.0, 0.0);
  constexpr Material roofMaterial = []() {
    Material ret;
    ret.color = MakeColour(0.2, 0.9, 0.1);
    ret.specular = 0.1;
    return ret;
  }();
  constexpr Plane roof = [&]() {
    Plane ret = Plane{roofTransform};
    ret.SetMaterial(roofMaterial);
    return ret;
  }();

  constexpr Transform middleSphereTransform =
      MatrixUtils::Translation(-0.5, 1.0, 0.5);
  constexpr Material middleSphereMaterial = []() {
    Material ret;
    ret.color = MakeColour(0.1, 1.0, 0.5);
    ret.specular = 0.3;
    ret.diffuse = 0.7;
    return ret;
  }();
  constexpr Sphere middleSphere = [&]() {
    Sphere ret = Sphere{middleSphereTransform};
    ret.SetMaterial(middleSphereMaterial);
    return ret;
  }();

  constexpr Transform rightSphereTransform =
      MatrixUtils::Translation(1.5, 0.5, -0.5) *
      MatrixUtils::Scale(0.5, 0.5, 0.5);
  constexpr Material rightSphereMaterial = []() {
    Material ret;
    ret.color = MakeColour(0.5, 1.0, 0.1);
    ret.specular = 0.3;
    ret.diffuse = 0.7;
    return ret;
  }();
  constexpr Sphere rightSphere = [&]() {
    Sphere ret = Sphere{rightSphereTransform};
    ret.SetMaterial(rightSphereMaterial);
    return ret;
  }();

  constexpr Transform leftSphereTransform =
      MatrixUtils::Translation(-1.5, 0.33, -0.75) *
      MatrixUtils::Scale(0.33, 0.33, 0.33);
  constexpr Material leftSphereMaterial = []() {
    Material ret;
    ret.color = MakeColour(0.1, 0.8, 0.1);
    ret.specular = 0.3;
    ret.diffuse = 0.7;
    return ret;
  }();
  constexpr Sphere leftSphere = [&]() {
    Sphere ret = Sphere{leftSphereTransform};
    ret.SetMaterial(leftSphereMaterial);
    return ret;
  }();

  constexpr std::array<ShapeWrapper, 5> shapes = {floor, roof, middleSphere,
                                                  rightSphere, leftSphere};

  // Camera setup
  constexpr auto cameraFov = pi / 3.0;
  constexpr auto camereView = MatrixUtils::ViewTransform(
      MakePoint(5.0, 2.5, -5.0), MakePoint(-3.0, 2.2, 0.0),
      MakeVector(0.0, 1.0, 0.0));
  constexpr auto camera = Camera(hsize, vsize, cameraFov, camereView);

  // The light source is white, shining from above and to the left.
  constexpr auto lightPoint = MakePoint(-5, 5, -2);
  constexpr auto lightColour = MakeColour(1, 1, 1);  // white
  constexpr auto light = PointLight(lightPoint, lightColour);
  constexpr std::array<PointLight, 1> lights = {light};
  constexpr std::size_t numXS =
      NumXSOf<Plane, Plane, Sphere, Sphere, Sphere>::numXS;
  // Made up world
  constexpr World<decltype(shapes), decltype(lights), numXS> world{
      std::move(shapes), std::move(lights)};
#ifdef COMPILETIME
  constexpr auto image = camera.render<hsize, vsize, decltype(world)>(world);
#else
  auto image = camera.render<hsize, vsize, decltype(world)>(world);
#endif
  image.ToPPM("plane.ppm");
}
