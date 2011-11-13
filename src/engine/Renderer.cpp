#include <cfloat>
#include <algorithm>
#include "Renderer.h"

using namespace Renderer;
using std::min;
using std::max;

static const Pixel background = { float3(), float3(), float3(), float3(), FLT_MAX, 0.0, 0.0, 0 };

inline static float3 estimateNormal(DistanceField dist, const float3 &z);
static float rayMarchHit(const float3 &origin, const float3 &ray, DistanceField dist, int &_steps);
static float rayMarchShadow(const float3 &origin, const float3 &ray, DistanceField dist, float maxz);
static float ambientOcclusion(const float3 &point, const float3 &normal, DistanceField dist);
static void renderPixel(Renderer::Pixel &pix, const float3 &origin, DistanceField dist, const float3 &ray);

static inline float3 diffuseBRDF(const float3 &normal, const float3 &eye, const float3 &light)
{
  return dot(normal, normalize(eye+light));
}

// Render a single pixel
void Renderer::Scene::renderPixel(Renderer::Pixel &pix, float x, float y)
{
  float3 ray = cam.project(x, y);
  pix.z = rayMarchHit(cam.pos(), ray, dist, pix.steps);
  if (pix.z < FLT_MAX)
  {
    float3 p = cam.pos() + pix.z*ray;
    pix.alpha = 1.0f;
    pix.normal = estimateNormal(dist, p);

    pix.ambient = ambient;

    pix.diffuse = float3(0.0f, 0.0f, 0.0f);
    pix.specular = float3(0.0f, 0.0f, 0.0f);
    for (int i=0; i<lights.size(); i++)
    {
      float3 lvec = lights[i].pos - p;
      float dl = length(lvec).scalar();
      lvec = normalize(lvec);

      float att = 1.0f / (1e-4f + lights[i].attConst + lights[i].attLinear*dl + lights[i].attQuad*dl*dl);
      float power = rayMarchShadow(p, lvec, dist, dl) * att;
      float diffuse = diffuseBRDF(pix.normal, -ray, lvec).scalar();
      float specular = pow(diffuse, shininess);

      pix.diffuse += power*diffuse*lights[i].color;
      pix.specular += power*specular*lights[i].color;
    }
    pix.diffuse *= diffuse;
    pix.specular *= specular;

    pix.ambientOcclusion = ambientOcclusion(p, pix.normal, dist);
  }
  else
    pix = background;
}


// Numeric gradient approximation
inline static float3 estimateNormal(DistanceField dist, const float3 &p)
{
  static const float eps = 0.0001f;

  float dx = dist(p + float3(eps, 0, 0)) - dist(p - float3(eps, 0, 0));
  float dy = dist(p + float3(0, eps, 0)) - dist(p - float3(0, eps, 0));
  float dz = dist(p + float3(0, 0, eps)) - dist(p - float3(0, 0, eps));

  return normalize(float3(dx, dy, dz));
}

// Ray marching tracer
static float rayMarchHit(const float3 &origin, const float3 &ray, DistanceField dist, int &_steps)
{
  static const int maxSteps = 1000;
  static const float hitThreshold = 1e-4f;
  static const float missThreshold = 100.0f;

  float z = 0.01f;
  for (int steps=0; steps<maxSteps; steps++)
  {
    float d = dist(origin + z*ray);
    if (d < hitThreshold)
    {
      _steps = steps;
      return z;
    }
    if (z > missThreshold)
      break;
    z += d;
  }

  _steps = maxSteps;
  if (z > missThreshold)
    return FLT_MAX;
  else
    return z;
}

static float rayMarchShadow(const float3 &origin, const float3 &ray, DistanceField dist, float maxz)
{
  static const float k = 5;
  int steps = 100;
  float dens = 1.0f;
  for (float z = 0.01f; steps && z<maxz; --steps)
  {
    float d = dist(origin + z*ray);
    if (d < 1e-4)
      return 0.0f;
    dens = min(dens, k*d/z);
    z += d;
  }
  return dens;
}

static float ambientOcclusion(const float3 &point, const float3 &normal, DistanceField dist)
{
  static const float delta = 0.07f;
  static const float blend = 1.0f;
  static const int iter = 10;

  float ao = 0;
  for (int i=iter; i>0; i--)
    ao = ao/2 + (i*delta - dist(point + (i*delta)*normal));

  return 1 - blend*ao;
}
