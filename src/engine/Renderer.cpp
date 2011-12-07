#include <cfloat>
#include <cmath>
#include <algorithm>
#include "Renderer.h"

using namespace Renderer;
using std::min;
using std::max;

static const Pixel background = { float3(), float3(), float3(), float3(), float3(), float3(), float3(), float3(), float3(), float3(), FLT_MAX, 0.0f, 0.0f, 0, 0 };

inline static float3 estimateNormal(DistanceField dist, float3r z);
static bool rayCast(float3r origin, float3r ray, DistanceField dist,
                    float minz, float maxz,
                    float &_z, float3 &_hit, float &_prox, int &_steps);
static inline float rayCastShadow(float3r origin, float3r ray, DistanceField dist,
                                  float maxz, int &_steps);
static float ambientOcclusion(float3r point, float3r normal, DistanceField dist);


float3 Renderer::Pixel::blend() const
{
  float3 color = ambientOcclusion * (ambient + diffuse);
  color = /* (1.0f-reflect) * */ color + reflect*reflected;
  color = /* (1.0f-refract) * */ color + refract*refracted;
  return color + specular;
}


// Render a single pixel
void Renderer::Scene::renderPixel(Renderer::Pixel &pix, float3r origin, float3r ray, int depth, bool inner) const
{
  DistanceField dist = inner? innerSceneDist : sceneDist;

  pix = background;
  pix.ray = ray;
  float _prox; // unused
  if (rayCast(origin, ray, dist, 1e-2f, 100.f, pix.z, pix.pos, _prox, pix.steps))
  {
    pix.normal = estimateNormal(dist, pix.pos);
    pix.alpha = 1.0f;
    pix.matId = matId(pix.pos);
    const Material &mat = materials[pix.matId];

    shade(pix, inner);
    if (depth>0 && (vsqlength(mat.reflect).scalar()>1e-4f || vsqlength(mat.refract).scalar() > 1e-4f))
      reflectRefract(pix, depth, inner);
  }
  else
  {
    pix.ambientOcclusion = 1.0;
    pix.ambient = envColor(ray);
  }
}

void Renderer::Scene::shade(Renderer::Pixel &pix, bool inner) const
{
  const Material &mat = materials[pix.matId];
  DistanceField dist = inner? innerSceneDist : sceneDist;

  pix.ambient = mat.ambientColor;
  pix.ambientOcclusion = ambientOcclusion(pix.pos, pix.normal, dist);
  pix.diffuse = float3(0.0f, 0.0f, 0.0f);
  pix.specular = float3(0.0f, 0.0f, 0.0f);
  for (size_t i=0; i<nLights; i++)
  {
    float3 lvec = lights[i].pos - pix.pos;
    float dl = vlength(lvec).scalar();
    lvec = vnormalize(lvec);

    int steps = 0;
    float att = 1.0f / (1e-4f + lights[i].attConst + lights[i].attLinear*dl + lights[i].attQuad*dl*dl);
    float power = rayCastShadow(pix.pos, lvec, dist, dl, steps) * att;
    float3 diffuse = mat.diffuseShader(pix.normal, -pix.ray, lvec, mat);
    float3 specular = mat.specularShader(pix.normal, -pix.ray, lvec, mat);

    pix.steps += steps;
    pix.diffuse += power*diffuse*lights[i].color;
    pix.specular += power*specular*lights[i].color;
  }
  pix.diffuse *= mat.diffuseColor;
  pix.specular *= mat.specularColor;
}

void Renderer::Scene::reflectRefract(Renderer::Pixel &pix, int depth, bool inner) const
{
  const Material &mat = materials[pix.matId];

  // Compute Fresnel coefficients
  float k = inner? mat.refractionIndex : 1.0f/mat.refractionIndex; // FIXME: handle actual index relation

  // cos1: falling light angle; cos2: refracted light angle
  float cos1 = vdot(-pix.ray, pix.normal).scalar();
  float cos2s = 1.0f - k*k*(1.0f - cos1*cos1);
  float cos2 = 0.0f;

  float kReflect = 1.0f;
  if (cos2s > 0.0f)
  {
    cos2 = sqrt(cos2s);
    kReflect = 0.5f * (sqr((k*cos1 - cos2)/(k*cos1 + cos2)) + sqr((k*cos2 - cos1)/(k*cos2 + cos1)));
  }
  pix.refract = (1.0f - kReflect) * mat.refract;
  pix.reflect = kReflect * mat.reflect;

  // Reflected ray
  if (vsqlength(pix.reflect).scalar()>1e-4f)
  {
    Pixel tmp;
    float3 newRay = vreflect(pix.ray, pix.normal);
    renderPixel(tmp, pix.pos, newRay, depth-1, inner);
    pix.reflected = tmp.blend();
    pix.steps += tmp.steps;
  }

  // Refracted ray
  if (vsqlength(pix.refract).scalar()>1e-4f)
  {
    Pixel tmp;
    float3 newRay = k*pix.ray + (k*cos1 - cos2)*pix.normal;
    renderPixel(tmp, pix.pos + 0.001f*pix.ray, newRay, depth-1, !inner);
    pix.refracted = tmp.blend();
    pix.steps += tmp.steps;
  }
}

void Renderer::Scene::renderPixel(Renderer::Pixel &pix, float x, float y) const
{
  renderPixel(pix, cam.pos(), cam.project(x, y));
}

// Numeric gradient approximation
inline static float3 estimateNormal(DistanceField dist, float3r p)
{
  static const float eps = 1e-4f;

  float dx = dist(p + float3(eps, 0, 0)) - dist(p - float3(eps, 0, 0));
  float dy = dist(p + float3(0, eps, 0)) - dist(p - float3(0, eps, 0));
  float dz = dist(p + float3(0, 0, eps)) - dist(p - float3(0, 0, eps));

  return vnormalize(float3(dx, dy, dz));
}

// Ray marching tracer
// TODO: detect material boundary as a hit?
static bool rayCast(float3r origin, float3r ray, DistanceField dist,
                    float minz, float maxz,
                    float &_z, float3 &_hit, float &_prox, int &_steps)
{
  static const float minStepK = 1e-5;
  static const float maxStepBase = 1e-1;
  static const float maxStepK = 1e-1;
  static const float epsK = 1e-5;
  static const int maxSteps = 1000;

  float z = minz;
  int steps;
  float3 p;
  float prox = FLT_MAX;
  bool isHit = false;

  for (steps=0; steps<maxSteps && z<maxz; steps++)
  {
    p = origin + z*ray;
    float d = dist(p);
    if (d < epsK * z)
    {
      isHit = true;
      prox = 0;
      break;
    }
    prox = min(prox, d/z);
    z += max(minStepK * z, min(d, maxStepBase + maxStepK * z));
  }

  _z = z;
  _hit = p;
  _prox = prox;
  _steps += steps;
  return isHit;
}

static inline float rayCastShadow(float3r origin, float3r ray, DistanceField dist,
                                  float maxz, int &_steps)
{
  float z=0, prox=0;
  float3 p;
  rayCast(origin, ray, dist, 1e-2, maxz, z, p, prox, _steps);
  return prox;
}

static float ambientOcclusion(float3r point, float3r normal, DistanceField dist)
{
  static const float delta = 0.1f;
  static const float blend = 1.0f;
  static const int iter = 10;

  float ao = 0;
  for (int i=iter; i>0; i--)
    ao = ao/2 + max(0.0f, (i*delta - dist(point + (i*delta)*normal)));

  return 1 - blend*ao;
}

float3 Renderer::Material::diffuseLambert(float3r normal, float3r /*eye*/, float3r light,
                                          const Renderer::Material &/*mat*/)
{
  return vdot(light, normal);
}

/*
float3 Renderer::Material::diffuseOrenNayar(float3 normal, float3 eye, float3 light,
                                            const Renderer::Material &mat)
{
  float a = 1.0f - 0.5f*sqr(mat.roughness)/(sqr(mat.roughness) + 0.33);
  float b = 0.45f*sqr(mat.roughness)/(sqr(mat.roughness) + 0.09);
  float thetai = acosf(vdot(light, normal).scalar());
  float thetar = acosf(vdot(eye, normal).scalar());
  float alpha = max(thetai, thetar);
  float beta = min(thetai, thetar);
  float dphi = atan2f(

  return float3(0.0f);
}
*/

float3 Renderer::Material::specularPhong(float3r normal, float3r eye, float3r light,
                                         const Renderer::Material &mat)
{
  float3 k = vdot(vreflect(light, normal), eye);
  return float3(powf(k.scalar(), mat.shininess));
}

float3 Renderer::Material::specularBlinn(float3r normal, float3r eye, float3r light,
                                         const Renderer::Material &mat)
{
  float3 k = vdot(vnormalize(eye+light), normal);
  return float3(powf(k.scalar(), mat.shininess));
}

/*
float3 Renderer::Material::specularCookTorrance(float3 normal, float3 eye, float3 light,
                                                const Renderer::Material &mat)
{
  return float3(0.0f);
}
*/
