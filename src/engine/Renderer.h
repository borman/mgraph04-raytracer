#ifndef RENDERER_H
#define RENDERER_H

#include "float3.h"
#include "matrix.h"
#include "FlatCamera.h"

namespace Renderer
{
  struct Pixel
  {
    float3 normal;
    float3 color;
    float z; // Depth buffer
    float alpha;
    float ambientOcclusion;
    int steps;
  };

  typedef float (*DistanceField)(float3);

  void render(Matrix<Pixel> &buf, DistanceField dist, const FlatCamera &cam);
}

#endif // RENDERER_H
