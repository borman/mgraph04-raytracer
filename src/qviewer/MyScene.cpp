#include <algorithm>
#include "MyScene.h"

using namespace std;

// Primitives

static inline float3 box(float3 p, float3 b)
{
  float3 di = vabs(p) - b;
  return vmin(vmaxcomp(di), vlength(vmax(di, float3(0.0))));
}

static inline float3 sphere(float3 p, float r)
{
  return vlength(p) - float3(r);
}

static inline float3 unite(float3 a, float3 b)
{
  return vmin(a, b);
}

static inline float3 subtract(float3 a, float3 b)
{
  return vmax(a, -b);
}

// Scene

#include "MyScene_sphere_cube.h"
//#include "MyScene_terrain.h"
//#include "MyScene_shader_test.h"
