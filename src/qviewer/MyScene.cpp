#include "MyScene.h"

// Primitives

static inline float3 box(float3 p, float3 b)
{
  float3 di = vabs(p) - b;
  return vmin(vmaxcomp(di), vlength(vmax(di, float3(0.0))));
}

static inline float3 sphere(float3 p, float r)
{
  return vlength(p) - float3(r);
}

static inline float3 unite(float3 a, float3 b)
{
  return vmin(a, b);
}

static inline float3 subtract(float3 a, float3 b)
{
  return vmax(a, -b);
}

// Scene

static const float3 box1Pos(0.0f, 0.0f, 0.8f);
static const float3 box1Size(0.7f, 0.7f, 0.9f);
static const float3 box2Pos(0.3f, 0.3f, 2.5f);
static const float3 box2Size(0.4f, 0.4f, 0.5f);
static const float3 sphere1Pos(-0.5f, -0.5f, 1.3f);
static const float3 sphere2Pos(0.3f, 0.3f, 2.5f);

static float sceneDist(float3 p)
{
  float3 sphere1 = sphere(p-sphere1Pos, 1.0f);
  float3 sphere2 = sphere(p-sphere2Pos, 0.4f);
  float3 box1 = box(p-box1Pos, box1Size);
  float3 box2 = box(p-box2Pos, box2Size);

  float3 scene = sphere1;
  scene = unite(scene, box1);
  //scene = unite(scene, sphere2);
  scene = unite(scene, box2);
  return scene.scalar();
}

static float innerSceneDist(float3 p)
{
  return -sceneDist(p);
}

static int sceneMat(float3 p)
{
  if (vlength(p-sphere2Pos).scalar() - 0.6f < 0.01)
    return 1;
  else
    return 0;
}

static float3 envColor(float3 p)
{
  return vabs(vcross(p, float3(1.0f, 1.0f, 1.0f)));
}


MyScene::MyScene()
{
  sceneDist = ::sceneDist;
  innerSceneDist = ::innerSceneDist;
  matId = ::sceneMat;
  envColor = ::envColor;

  // Big object
  {
    Renderer::Material mat;
    mat.ambient = float3(0.1f, 0.1f, 0.1f);
    mat.diffuse = float3(0.8f, 0.9f, 0.8f);
    mat.specular = float3(1.0f, 1.0f, 1.0f);
    mat.shininess = 1000;
    mat.reflect = 0.1f;
    mat.refract = 0.0f;
    materials.push_back(mat);
  }
  // Small object
  {
    Renderer::Material mat;
    mat.ambient = float3(0.01f, 0.01f, 0.01f);
    mat.diffuse = float3(0.0f, 0.0f, 0.5);
    mat.specular = float3(1.0f, 1.0f, 1.0f);
    mat.shininess = 1000;
    mat.reflect = 0.3f;
    mat.refract = 0.7f;
    mat.refractionIndex = 0.999f;
    materials.push_back(mat);
  }

  lights.push_back(
        Renderer::Light(float3(10.0f, 10.0f, 10.0f),
                        float3(0.8f, 0.2f, 0.2f),
                        0.03f, 0.0f, 0.005f));
  lights.push_back(
        Renderer::Light(float3(-3.0f, 1.0f, 4.0f),
                        float3(0.1f, 0.8f, 0.1f),
                        0.1f, 0.0f, 0.05f));
  lights.push_back(
        Renderer::Light(float3(-1.0f, -5.0f, 2.0f),
                        float3(0.1f, 0.1f, 0.8f),
                        0.3f, 0.0f, 0.01f));
}