#include <camera.hh>
#include <primitives.hh>
#include <vec.hh>
#include <world.hh>
using namespace RayTracer;

int main() {
  constexpr auto pi = MathUtils::MathConstants::PI<double>;
  static constexpr std::size_t hSize = 380;
  static constexpr std::size_t vSize = 350;

  // Floor
  constexpr CheckerPattern flrPatn = []() {
    CheckerPattern ret;
    auto colorA = PredefinedColours::GREEN;
    auto colorB = PredefinedColours::BLUE;
    ret.SetColourA(colorA);
    ret.SetColourB(colorB);
    return ret;
  }();

  static constexpr PatternWrapper flrPatnWrapr = PatternWrapper(flrPatn);

  constexpr Material flrMtrl = [&]() {
    Material ret;
    ret.patternPtr = &flrPatnWrapr;
    ret.specular = 0.0;
    return ret;
  }();

  constexpr Plane floor = [&]() {
    Transform flrTF = MatrixUtils::Scale(10.0, 0.01, 10.0);
    Plane ret = Plane{flrTF};
    ret.SetMaterial(flrMtrl);
    return ret;
  }();

  // Left Wall
  constexpr StridePattern lWStriPatn = []() {
    StridePattern ret;
    auto colorA = PredefinedColours::GREEN;
    auto colorB = PredefinedColours::BLUE;
    ret.SetColourA(colorA);
    ret.SetColourB(colorB);
    return ret;
  }();

  static constexpr PatternWrapper lWPatnWrapr = PatternWrapper(lWStriPatn);

  constexpr Material lWMtrl = [&]() {
    Material ret;
    ret.patternPtr = &lWPatnWrapr;
    ret.specular = 0.0;
    return ret;
  }();

  constexpr Sphere leftWall = [&]() {
    Transform lWTFs = MatrixUtils::Translation(0.0, 0.0, 5.0) *
                      MatrixUtils::RotateY(-pi / 4.0) *
                      MatrixUtils::RotateX(pi / 2.0) *
                      MatrixUtils::Scale(10.0, 0.01, 10.0);
    Sphere ret = Sphere{lWTFs};
    ret.SetMaterial(lWMtrl);
    return ret;
  }();

  // Right Wall
  constexpr StridePattern rWStriPatn = []() {
    StridePattern ret;
    auto colorA = PredefinedColours::GREEN;
    auto colorB = PredefinedColours::BLUE;
    ret.SetColourA(colorA);
    ret.SetColourB(colorB);
    return ret;
  }();

  static constexpr PatternWrapper rWPatnWrapr = PatternWrapper(rWStriPatn);

  constexpr Material rWMtrl = [&]() {
    Material ret;
    ret.patternPtr = &rWPatnWrapr;
    ret.specular = 0.2;
    return ret;
  }();

  constexpr Sphere rightWall = [&]() {
    Transform rWTFs = MatrixUtils::Translation(0.0, 0.0, 5.0) *
                      MatrixUtils::RotateY(pi / 4.0) *
                      MatrixUtils::RotateX(pi / 2.0) *
                      MatrixUtils::Scale(10.0, 0.01, 10.0);
    Sphere ret = Sphere{rWTFs};
    ret.SetMaterial(rWMtrl);
    return ret;
  }();

  // Left Sphere
  constexpr StridePattern lSphPatn = []() {
    StridePattern ret;
    auto colorA = PredefinedColours::GREEN;
    auto colorB = PredefinedColours::BLUE;
    ret.SetColourA(colorA);
    ret.SetColourB(colorB);
    return ret;
  }();

  static constexpr PatternWrapper lSphPatnWrapr = PatternWrapper(lSphPatn);

  constexpr Material lSphMtrl = [&]() {
    Material ret;
    ret.patternPtr = &lSphPatnWrapr;
    ret.diffuse = 0.7;
    ret.specular = 0.3;
    return ret;
  }();

  constexpr Sphere leftSphere = [&]() {
    Transform lSTFs = MatrixUtils::Translation(-2.0, 0.33, -0.75) *
                      MatrixUtils::Scale(0.33, 0.33, 0.33);
    Sphere ret = Sphere{lSTFs};
    ret.SetMaterial(lSphMtrl);
    return ret;
  }();

  // Right Sphere
  constexpr StridePattern rSphPatn = []() {
    StridePattern ret;
    auto colorA = PredefinedColours::GREEN;
    auto colorB = PredefinedColours::BLUE;
    ret.SetColourA(colorA);
    ret.SetColourB(colorB);
    return ret;
  }();

  static constexpr PatternWrapper rSphPatnWrapr = PatternWrapper(rSphPatn);

  constexpr Material rSphMtrl = [&]() {
    Material ret;
    ret.patternPtr = &rSphPatnWrapr;
    ret.diffuse = 0.7;
    ret.specular = 0.3;
    return ret;
  }();

  constexpr Sphere rightSphere = [&]() {
    Transform rSTFs = MatrixUtils::Translation(1.5, 0.5, -0.5) *
                      MatrixUtils::Scale(0.5, 0.5, 0.5);
    Sphere ret = Sphere{rSTFs};
    ret.SetMaterial(rSphMtrl);
    return ret;
  }();

  // Middle Sphere
  constexpr StridePattern mSphPatn = []() {
    Transform mSTFs = MatrixUtils::Translation(-0.5, 1.0, 0.5) *
                    MatrixUtils::Scale(1.3, 1.3, 1.3);
    StridePattern ret{mSTFs};
    auto colorA = PredefinedColours::GREEN;
    auto colorB = PredefinedColours::BLUE;
    ret.SetColourA(colorA);
    ret.SetColourB(colorB);
    return ret;
  }();

  static constexpr PatternWrapper mSphPatnWrapr = PatternWrapper(mSphPatn);

  constexpr Material mSphMtrl = [&]() {
    Material ret;
    ret.patternPtr = &mSphPatnWrapr;
    ret.ambient = 0.7;
    ret.diffuse = 0.3;
    return ret;
  }();

  constexpr Sphere middleSphere = [&]() {
    Transform mSTFs = MatrixUtils::Translation(-0.5, 1.0, 0.5) *
                      MatrixUtils::Scale(1.3, 1.3, 1.3);
    Sphere ret = Sphere{mSTFs};
    ret.SetMaterial(mSphMtrl);
    return ret;
  }();

  constexpr std::array<ShapeWrapper, 6> shapes = {
      floor, leftWall, rightWall, middleSphere, rightSphere, leftSphere};
  constexpr std::size_t numXS =
      NumXSOf<Plane, Sphere, Sphere, Sphere, Sphere, Sphere>::numXS;

  // Camera setup
  constexpr auto camFov = pi / 3.0;
  constexpr auto camViewTF = MatrixUtils::ViewTransform(
      MakePoint(0.0, 1.5, -5.0), MakePoint(0.0, 1.0, 0.0),
      MakeVector(0.0, 1.0, 0.0));
  constexpr auto camera = Camera(hSize, vSize, camFov, camViewTF);

  // The light source is white, shining from above and to the left.
  constexpr auto lightPoint = MakePoint(-10.0, 10.0, -10.0);
  constexpr auto lightColour = MakeColour(1.0, 1.0, 1.0);  // white
  constexpr auto light = PointLight(lightPoint, lightColour);
  constexpr std::array<PointLight, 1> lights = {light};

  // Made up world
  constexpr World<decltype(shapes), decltype(lights), numXS> world{
      std::move(shapes), std::move(lights)};
#ifdef COMPILETIME
  constexpr auto image = camera.render<hSize, vSize, decltype(world)>(world);
#else
  auto image = camera.render<hSize, vSize, decltype(world)>(world);
#endif
  image.ToPPM("pattern_scene.ppm");

  return 0;
}
