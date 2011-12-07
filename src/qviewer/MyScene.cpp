#include <algorithm>
#include "MyScene.h"

using namespace std;

// Primitives

static inline float3 box(float3r p, float3r b)
{
  float3 di = vabs(p) - b;
  return vmin(vmaxcomp(di), vlength(vmax(di, float3(0.0))));
}

static inline float3 sphere(float3r p, float r)
{
  return vlength(p) - float3(r);
}

static inline float3 unite(float3r a, float3r b)
{
  return vmin(a, b);
}

static inline float3 intersect(float3r a, float3r b)
{
  return vmax(a, b);
}

static inline float3 subtract(float3r a, float3r b)
{
  return vmax(a, -b);
}

// Scene

#define malloc(x) _mm_malloc(x, 16)
#define free(x) _mm_free(x)

#include "MyScene_sphere_cube.h"
//#include "MyScene_terrain.h"
//#include "MyScene_shader_test.h"
