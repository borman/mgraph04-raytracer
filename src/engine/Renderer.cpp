#include <cfloat>
#include "Renderer.h"

using namespace Renderer;

static const Pixel background = { float3(), float3(), FLT_MAX, 0.0, 0.0, 0 };

inline static float3 estimateNormal(DistanceField dist, const float3 &z);
static float rayMarchHit(const float3 &origin, const float3 &ray, DistanceField dist, int &_steps);
static float ambientOcclusion(const float3 &point, const float3 &normal, DistanceField dist);


void Renderer::render(Matrix<Renderer::Pixel> &buf, DistanceField dist, const FlatCamera &cam)
{
  float w = buf.width()-1;
  float h = buf.height()-1;

#pragma omp parallel for
  for (int y=0; y<buf.height(); y++)
    for (int x=0; x<buf.width(); x++)
    {
      float3 ray = cam.project(x/w, 1-y/h);
      Pixel &pix = buf.at(x, y);
      int steps;
      float z = rayMarchHit(cam.pos(), ray, dist, steps);
      if (z < FLT_MAX)
      {
        float3 p = cam.pos() + z*ray;
        pix.z = z;
        pix.alpha = 1.0;
        pix.steps = steps;
        pix.normal = estimateNormal(dist, p);
        pix.color = float3(1.0, 1.0, 1.0);
        pix.ambientOcclusion = ambientOcclusion(p, pix.normal, dist);
      }
      else
        pix = background;
    }
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
  static const float hitThreshold = 0.002f;
  static const float missThreshold = 1000.0f;

  float z = 0;
  for (int steps=0; steps<maxSteps; steps++)
  {
    float3 p = origin + z*ray;
    float d = dist(p);
    if (d < hitThreshold) // assume a hit
    {
      _steps = steps;
      return z;
    }

    if (steps > 5 && d > missThreshold)
      break;

    z += d;
  }

  _steps = 0;
  return FLT_MAX;
}

static float ambientOcclusion(const float3 &point, const float3 &normal, DistanceField dist)
{
  static const float delta = 0.1f;
  static const float blend = 1.0f;
  static const int iter = 5;

  float ao = 0;
  for (int i=iter; i>0; i--)
    ao = ao/2 + (i*delta - dist(point + (i*delta)*normal));

  return 1 - blend*ao;
}
