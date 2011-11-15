#include <cfloat>
#include <cmath>
#include <algorithm>
#include "Renderer.h"

using namespace Renderer;
using std::min;
using std::max;

static const Pixel background = { float3(), float3(), float3(), float3(), float3(), float3(), float3(), FLT_MAX, 0.0f, 0.0f, 0.0f, 0.0f, 0 };

inline static float3 estimateNormal(DistanceField dist, const float3 &z);
static float rayMarchHit(const float3 &origin, const float3 &ray, DistanceField dist, int &_steps);
static float rayMarchShadow(const float3 &origin, const float3 &ray, DistanceField dist, float maxz, int &_steps);
static float ambientOcclusion(const float3 &point, const float3 &normal, DistanceField dist);
static void renderPixel(Renderer::Pixel &pix, const float3 &origin, DistanceField dist, const float3 &ray);

static inline float3 reflect(float3 ray, float3 normal)
{
  return ray - (float(2.0)*vdot(ray, normal))*normal;
}

static inline bool refract(float3 ray, float3 normal, float k, float3 &newRay)
{
  float3 cos_phi1 = vdot(-ray, normal);
  float3 sin2_phi2 = (k*k) * (float3(1.0f) - cos_phi1*cos_phi1);
  if (sin2_phi2.scalar() > 1.0f)
    return false; // Total internal reflection
  float3 cos_phi2 = vsqrt(float3(1.0f) - sin2_phi2);
  newRay = k*ray + (k*cos_phi1 - cos_phi2)*normal;
  return true;
}

static inline float3 diffuseBRDF(const float3 &normal, const float3 &eye, const float3 &light)
{
  // Blinn:
  return vdot(normal, vnormalize(eye+light));
}

float3 Renderer::Pixel::blend() const
{
  float3 color = ambientOcclusion * (ambient + diffuse);
  color = /* (1.0f-reflect) * */ color + reflect*reflected;
  color = /* (1.0f-refract) * */ color + refract*refracted;
  return color + specular;
}


// Render a single pixel
void Renderer::Scene::renderPixel(Renderer::Pixel &pix, float3 origin, float3 ray, int depth, bool inner) const
{
  DistanceField dist = inner? innerSceneDist : sceneDist;

  pix = background;
  pix.z = rayMarchHit(origin, ray, dist, pix.steps);
  if (pix.z < FLT_MAX)
  {
    pix.pos = origin + pix.z*ray;
    pix.normal = estimateNormal(dist, pix.pos);
    const Material &mat = materials[matId(pix.pos)];

    pix.alpha = 1.0f;
    pix.ambient = mat.ambient;
    pix.ambientOcclusion = ambientOcclusion(pix.pos, pix.normal, dist);
    pix.diffuse = float3(0.0f, 0.0f, 0.0f);
    pix.specular = float3(0.0f, 0.0f, 0.0f);
    for (int i=0; i<lights.size(); i++)
    {
      float3 lvec = lights[i].pos - pix.pos;
      float dl = vlength(lvec).scalar();
      lvec = vnormalize(lvec);

      int steps = 0;
      float att = 1.0f / (1e-4f + lights[i].attConst + lights[i].attLinear*dl + lights[i].attQuad*dl*dl);
      float power = rayMarchShadow(pix.pos, lvec, dist, dl, steps) * att;
      float diffuse = diffuseBRDF(pix.normal, -ray, lvec).scalar();
      float specular = powf(diffuse, mat.shininess);

      pix.steps += steps;
      pix.diffuse += power*diffuse*lights[i].color;
      pix.specular += power*specular*lights[i].color;
    }
    pix.diffuse *= mat.diffuse;
    pix.specular *= mat.specular;

    if (depth>0 && (mat.reflect>1e-4f || mat.refract > 1e-4f))
    {
      // Compute Fresnel coefficients
      float k = inner? 1.0f/mat.refractionIndex : mat.refractionIndex;
      float cos1 = vdot(-ray, pix.normal).scalar();
      float cos2s = 1.0f - k*k*(1.0f - cos1*cos1);
      float cos2 = 0.0f;
      pix.reflect = 1.0f;
      if (cos2s > 0.0f)
      {
        cos2 = sqrt(cos2s);
        pix.reflect = 0.5f * (sqr((k*cos1 - cos2)/(k*cos1 + cos2)) + sqr((k*cos2 - cos1)/(k*cos2 + cos1)));
      }
      pix.refract = 1.0f - pix.reflect;
      pix.reflect *= mat.reflect;
      pix.refract *= mat.refract;

      if (pix.reflect>1e-4f)
      {
        Pixel tmp;
        float3 newRay = reflect(ray, pix.normal);
        renderPixel(tmp, pix.pos, newRay, depth-1, inner);
        pix.reflected = tmp.blend();
        pix.steps += tmp.steps;
      }

      if (pix.refract>1e-4f)
      {
        Pixel tmp;
        float3 newRay = k*ray + (k*cos1 - cos2)*pix.normal;
        renderPixel(tmp, pix.pos+0.01*ray, newRay, depth-1, !inner);
        pix.refracted = tmp.blend();
        pix.steps += tmp.steps;
      }
    }
  }
  else
  {
    pix.ambientOcclusion = 1.0;
    pix.ambient = envColor(ray);
  }
}

void Renderer::Scene::renderPixel(Renderer::Pixel &pix, float x, float y) const
{
  renderPixel(pix, cam.pos(), cam.project(x, y));
}

// Numeric gradient approximation
inline static float3 estimateNormal(DistanceField dist, const float3 &p)
{
  static const float eps = 1e-4f;

  float dx = dist(p + float3(eps, 0, 0)) - dist(p - float3(eps, 0, 0));
  float dy = dist(p + float3(0, eps, 0)) - dist(p - float3(0, eps, 0));
  float dz = dist(p + float3(0, 0, eps)) - dist(p - float3(0, 0, eps));

  return vnormalize(float3(dx, dy, dz));
}

// Ray marching tracer
static float rayMarchHit(const float3 &origin, const float3 &ray, DistanceField dist, int &_steps)
{
  static const int maxSteps = 1000;
  static const float hitThreshold = 1e-4f;
  static const float missThreshold = 100.0f;

  float z = 3e-4f;
  int steps=0;
  for (steps=0; steps<maxSteps && z<missThreshold; steps++)
  {
    float d = dist(origin + z*ray);
    if (d < hitThreshold)
      break;
    z += d;
  }

  _steps += steps;
  if (z > missThreshold)
    return FLT_MAX;
  else
    return z;
}

static float rayMarchShadow(const float3 &origin, const float3 &ray, DistanceField dist, float maxz, int &_steps)
{
  static const float k = 5; // Shadow smoothness
  static const int maxSteps = 100;
  int steps = 0;
  float dens = 1.0f;
  for (float z = 0.01f; steps<maxSteps && z<maxz; steps++)
  {
    float d = dist(origin + z*ray);
    if (d < 1e-5f) // Obstacle found
    {
      _steps += steps;
      return 0.0f;
    }
    dens = min(dens, k*d/z);
    z += d;
  }
  _steps += steps;
  return dens;
}

static float ambientOcclusion(const float3 &point, const float3 &normal, DistanceField dist)
{
  static const float delta = 0.01f;
  static const float blend = 1.0f;
  static const int iter = 10;

  float ao = 0;
  for (int i=iter; i>0; i--)
    ao = ao/2 + max(0.0f, (i*delta - dist(point + (i*delta)*normal)));

  return 1 - blend*ao;
}
