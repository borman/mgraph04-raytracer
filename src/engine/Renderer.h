#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include "float3.h"
#include "matrix.h"
#include "FlatCamera.h"

namespace Renderer
{
  typedef float (*DistanceField)(float3);

  struct Pixel
  {
    float3 normal;

    // Color components
    float3 ambient;
    float3 diffuse;
    float3 specular;

    float z; // Depth buffer
    float alpha;
    float ambientOcclusion;
    int steps;
  };

  struct Light
  {
    Light(const float3 &_pos=float3(), const float3 &_color=float3())
      : pos(_pos), color(_color) {}
    float3 pos;
    float3 color;
  };

  struct Scene
  {
    DistanceField dist;
    std::vector<Light> lights;
    FlatCamera cam;
    float3 ambient;
    float3 diffuse;
    float3 specular;

    void renderPixel(Renderer::Pixel &pix, float x, float y);
  };
}

#endif // RENDERER_H
