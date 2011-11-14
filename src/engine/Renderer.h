#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include "float3.h"
#include "matrix.h"
#include "FlatCamera.h"

namespace Renderer
{
  struct Pixel
  {
    float3 pos;
    float3 normal;

    // Color components
    float3 ambient;
    float3 diffuse;
    float3 specular;
    float3 reflected;
    float3 refracted;

    float z; // Depth buffer
    float alpha;
    float ambientOcclusion;
    float reflect;
    float refract;
    int steps;

    float3 blend() const;
  };

  struct Light
  {
    Light(const float3 &_pos=float3(), const float3 &_color=float3(), float _ac=1.0, float _al=0.0, float _aq=0.0)
      : pos(_pos), color(_color), attConst(_ac), attLinear(_al), attQuad(_aq) {}
    float3 pos;
    float3 color;
    float attConst, attLinear, attQuad;
  };

  struct Material
  {
    float3 ambient;
    float3 diffuse;
    float3 specular;
    float shininess;
    float reflect;
    float refract;
    float refractionIndex;
  };

  typedef float (*DistanceField)(float3);
  typedef int (*MaterialFunc)(float3);
  typedef float3 (*EnvironmentFunc)(float3);

  struct Scene
  {
    DistanceField sceneDist;
    DistanceField innerSceneDist;
    MaterialFunc matId;
    EnvironmentFunc envColor;

    std::vector<Light> lights;
    std::vector<Material> materials;
    FlatCamera cam;

    void renderPixel(Renderer::Pixel &pix, float x, float y) const;
    void renderPixel(Renderer::Pixel &pix, float3 origin, float3 ray, int depth = 4, bool inner = false) const;
  };
}

#endif // RENDERER_H
