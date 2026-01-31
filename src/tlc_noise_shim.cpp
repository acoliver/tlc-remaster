#include <noise/noise.h>

// This project vendors libnoise *headers* (under src/build/include/noise/...) but
// does not currently ship a libnoise implementation library on macOS. The original
// Windows build linked against libnoise.lib.
//
// To get a linked executable (even if procedural textures are low fidelity), we
// provide a minimal shim implementing only the libnoise symbols referenced by the
// game.

namespace noise {
namespace module {

Module::Module(int /*sourceModuleCount*/) {}

Module::~Module() = default;

Perlin::Perlin()
  : Module(0)
{
}

double Perlin::GetValue(double /*x*/, double /*y*/, double /*z*/) const
{
  return 0.0;
}

} // namespace module

namespace model {

Plane::Plane() = default;
Sphere::Sphere() = default;
Cylinder::Cylinder() = default;

// Provide deterministic placeholder values.
double Plane::GetValue(double /*x*/, double /*z*/) const
{
  return 0.0;
}

double Sphere::GetValue(double /*lat*/, double /*lon*/) const
{
  return 0.0;
}

double Cylinder::GetValue(double /*angle*/, double /*height*/) const
{
  return 0.0;
}

} // namespace model
} // namespace noise
