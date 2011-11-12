#ifndef RAYMARCHER_H
#define RAYMARCHER_H

#include <QDebug>
#include "float3.h"
#include "HitAccum.h"

template<typename F>
float3 estimateNormal(F dist, float3 z)
{
  const float eps = 0.0001f;

  float3 z1 = z + float3(eps, 0, 0);
  float3 z2 = z - float3(eps, 0, 0);
  float3 z3 = z + float3(0, eps, 0);
  float3 z4 = z - float3(0, eps, 0);
  float3 z5 = z + float3(0, 0, eps);
  float3 z6 = z - float3(0, 0, eps);

  float dx = dist(z1) - dist(z2);
  float dy = dist(z3) - dist(z4);
  float dz = dist(z5) - dist(z6);

  return normalize(float3(dx, dy, dz));
}

template<class F>
bool rayMarchHit(const float3 &origin, const float3 &ray, F dist, HitAccum &result)
{
  static const int maxSteps = 100;
  static const float threshold = 0.002f;

  float z = 0;
  for (int steps=0; steps<maxSteps; steps++)
  {
    float3 p = origin + z*ray;
    float d = dist(p);
    if (d < threshold)
    {
      // assume a hit
      result.add(z, estimateNormal(dist, p), steps);
      return true;
    }

    z += d;
  }

  return false;
}

template<class F>
float ambientOcclusion(const float3 &point, const float3 &normal, F dist)
{
  static const float delta = 0.1f;
  static const float blend = 1.0f;
  static const int iter = 5;

  float ao = 0;
  for (int i=iter; i>0; i--)
    ao = ao/2 + (i*delta - dist(point + (i*delta)*normal));

  return 1 - blend*ao;
}

#endif // RAYMARCHER_H
