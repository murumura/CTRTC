#include <camera.hh>
#include <shading.hh>
#include <shape.hh>
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
  static constexpr std::size_t hsize = 80;
  static constexpr std::size_t vsize = 40;

  /* 
    (1) The floor is an extremely flatten sphere with a matte texture 
  */
  constexpr Material matte = []() {
    Material ret;
    ret.color = MakeColour(1.0, 0.9, 0.9);
    ret.specular = 0.0;
    return ret;
  }();

  constexpr Transform floorTF = MatrixUtils::Scale(10.0, 0.01, 10.0);

  constexpr Sphere floor = [&]() {
    Sphere ret = Sphere{floorTF};
    ret.SetMaterial(matte);
    return ret;
  }();

  /*
    (2) The wall on the left has the same scale and color as the floor,
    but is also rotated and translate into plane.
  */
  constexpr Transform leftWallTF = MatrixUtils::Translation(0.0, 0.0, 5.0) *
                                   MatrixUtils::RotateY(-pi / 4.0) *
                                   MatrixUtils::RotateX(pi / 2.0) *
                                   MatrixUtils::Scale(10, 0.01, 10);

  constexpr Sphere leftWall = [&]() {
    Sphere ret = Sphere{leftWallTF};
    ret.SetMaterial(matte);
    return ret;
  }();

  /*
    (3) The wall on the right is identical to the left wall, 
    but is rotated the opposite direction in y
  */
  constexpr Transform rightWallTF =
      MatrixUtils::Translation(0.0, 0.0, 5.0) * MatrixUtils::RotateY(pi / 4.0) *
      MatrixUtils::RotateX(pi / 2.0) * MatrixUtils::Scale(10, 0.01, 10);

  constexpr Sphere rightWall = [&]() {
    Sphere ret = Sphere{rightWallTF};
    ret.SetMaterial(matte);
    return ret;
  }();

  /*
    (4) The large sphere in the middle is a unit sphere, translated upward slightly and colored green.
  */
  constexpr Transform midSphereTF = MatrixUtils::Translation(-0.5, 1.0, 0.5);
  constexpr Material midSphereMaterial = []() {
    Material ret;
    ret.color = MakeColour(0.1, 1.0, 0.5);
    ret.diffuse = 0.7;
    ret.specular = 0.3;
    return ret;
  }();
  constexpr Sphere midSphere = [&]() {
    Sphere ret = Sphere{midSphereTF};
    ret.SetMaterial(midSphereMaterial);
    return ret;
  }();

  /*
    (5) The smaller green sphere on the right is scaled in half.
  */
  constexpr Transform rightSphereTF = MatrixUtils::Translation(1.5, 0.5, -0.5) *
                                      MatrixUtils::Scale(0.5, 0.5, 0.5);
  constexpr Material rightSphereMaterial = []() {
    Material ret;
    ret.color = MakeColour(0.5, 1.0, 0.1);
    ret.diffuse = 0.7;
    ret.specular = 0.3;
    return ret;
  }();
  constexpr Sphere rightSphere = [&]() {
    Sphere ret = Sphere{rightSphereTF};
    ret.SetMaterial(rightSphereMaterial);
    return ret;
  }();

  /* 
    (6) The smaller sphere is scaled by a third, before being translated 
  */
  constexpr Transform leftSphereTF =
      MatrixUtils::Translation(-1.5, 0.33, -0.75) *
      MatrixUtils::Scale(0.33, 0.33, 0.33);

  constexpr Material leftSphereMaterial = []() {
    Material ret;
    ret.color = MakeColour(1.0, 0.8, 0.1);
    ret.diffuse = 0.7;
    ret.specular = 0.3;
    return ret;
  }();
  constexpr Sphere leftSphere = [&]() {
    Sphere ret = Sphere{leftSphereTF};
    ret.SetMaterial(leftSphereMaterial);
    return ret;
  }();

  constexpr std::array<ShapeWrapper, 6> shapes = {
      floor, leftWall, rightWall, midSphere, rightSphere, leftSphere};

  // Camera setup
  constexpr auto cameraFov = pi / 3.0;
  constexpr auto camereView = MatrixUtils::ViewTransform(
      MakePoint(0.0, 1.5, -5.0), MakePoint(0.0, 1.0, 0.0),
      MakeVector(0.0, 1.0, 0.0));
  constexpr auto camera = Camera(hsize, vsize, cameraFov, camereView);

  // The light source is white, shining from above and to the left.
  constexpr auto lightPoint = MakePoint(-10, 10, -10);
  constexpr auto lightColour = MakeColour(1, 1, 1);  // white
  constexpr auto light = PointLight(lightPoint, lightColour);
  constexpr std::array<PointLight, 1> lights = {light};

  // Made up world
  constexpr World<decltype(shapes), decltype(lights)> world{std::move(shapes),
                                                            std::move(lights)};
  constexpr std::size_t numXS = world.PossibleXSNums();
#ifdef COMPILETIME
  constexpr auto image =
      camera.render<hsize, vsize, numXS, decltype(world)>(world);
#else
  auto image = camera.render<hsize, vsize, numXS, decltype(world)>(world);
#endif
  image.ToPPM("scene.ppm");
}
