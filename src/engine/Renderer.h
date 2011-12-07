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
    float3 ray;
    float3 normal;

    // Color components
    float3 ambient;
    float3 diffuse;
    float3 specular;
    float3 reflected;
    float3 refracted;
    float3 reflect;
    float3 refract;

    float z; // Depth buffer
    float alpha;
    float ambientOcclusion;
    int steps;
    int matId;

    float3 blend() const;
  };

  struct Light
  {
    Light(float3r _pos=float3(), float3r _color=float3(), float _ac=1.0, float _al=0.0, float _aq=0.0)
      : pos(_pos), color(_color), attConst(_ac), attLinear(_al), attQuad(_aq) {}
    float3 pos;
    float3 color;
    float attConst, attLinear, attQuad;
  };

  typedef float3 (*ShaderFunc)(float3r, float3r, float3r, const struct Material &);

  struct Material
  {
    Material()
      : diffuseShader(diffuseLambert), specularShader(specularBlinn),
        shininess(0.0f), reflect(0.0f), refract(0.0f), refractionIndex(1.0f) {}

    // Basic shading props
    float3 ambientColor;
    float3 diffuseColor;
    float3 specularColor;
    ShaderFunc diffuseShader;
    ShaderFunc specularShader;

    // Shader-specific shading props
    float shininess;

    // Optical effects
    float refractionIndex;
    float3 reflect;
    float3 refract;

    // Predefined shaders
    static float3 diffuseLambert(float3r normal, float3r eye, float3r light, const Material &mat);
    //static float3 diffuseOrenNayar(float3r normal, float3r eye, float3r light, const Material &mat);
    static float3 specularPhong(float3r normal, float3r eye, float3r light, const Material &mat);
    static float3 specularBlinn(float3r normal, float3r eye, float3r light, const Material &mat);
    //static float3 specularCookTorrance(float3r normal, float3r eye, float3r light, const Material &mat);
  };

  typedef float (*DistanceField)(float3r);
  typedef int (*MaterialFunc)(float3r);
  typedef float3 (*EnvironmentFunc)(float3r);

  struct Scene
  {
    DistanceField sceneDist;
    DistanceField innerSceneDist;
    MaterialFunc matId;
    EnvironmentFunc envColor;

    // MSVS std::vector cannot into alignment
    Light *lights;
    size_t nLights;
    Material *materials;
    size_t nMaterials;
    FlatCamera cam;

    void renderPixel(Pixel &pix, float x, float y) const;
    void renderPixel(Pixel &pix, float3r origin, float3r ray, int depth = 4, bool inner = false) const;

    void shade(Pixel &pix, bool inner) const;
    void reflectRefract(Pixel &pix, int depth, bool inner) const;
  };
}

#endif // RENDERER_H
