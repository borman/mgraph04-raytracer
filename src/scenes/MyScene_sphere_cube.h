// Box, sphere, box, reflections, refractions

static const float3 box1Pos(0.0f, 0.0f, 0.8f);
static const float3 box1Size(0.7f, 0.7f, 0.9f);
static const float3 box2Pos(0.5f, -0.3f, 2.2f);
static const float3 box2Size(0.1f, 0.3f, 0.3f);

static const float3 sphere1Pos(-0.5f, -0.5f, 1.3f);
static const float3 sphere2Pos(0.3f, 0.6f, 2.3f);
static const float3 sphere3Pos(0.3f, 0.7f, 2.3f);
static const float3 sphere2Stretch(1.0f, 1.0f, 1.0f);

static float sceneDist(float3r p)
{
  float3 sphere1 = sphere(p-sphere1Pos, 1.0f);
  float3 sphere2 = sphere(p-sphere2Pos, 0.4f);
  //float3 sphere3 = sphere(p-sphere3Pos, 0.6f);
  float3 box1 = box(p-box1Pos, box1Size);
  float3 box2 = box(p-box2Pos, box2Size);

  float3 scene = sphere1;
  scene = unite(scene, box1);
  scene = unite(scene, sphere2);
  //scene = subtract(scene, sphere3);
  scene = unite(scene, box2);
  return scene.scalar();
}

static float innerSceneDist(float3r p)
{
  return -sceneDist(p);
}

static int sceneMat(float3r p)
{
  if (vlength(p-sphere2Pos).scalar() - 0.5f < 0.01)
    return 1;
  else if (box(p-box2Pos, box2Size).scalar() < 0.01)
    return 1;
  else
    return 0;
}

static float3 envColor(float3r p)
{
  float phi = float(M_PI) + acos(p.z()/vlength(p).scalar());
  float theta = float(M_PI) + atan2(p.y(), p.x());
  theta += 0.5*phi;
  float3 checkers = float3((int(phi * 8/M_PI)%2) ^ (int(theta * 8/M_PI)%2));
  float3 colors = vabs(vcross(p, float3(1.0f, 1.0f, 1.0f)));
  return checkers * colors;
}


MyScene::MyScene()
{
  sceneDist = ::sceneDist;
  innerSceneDist = ::innerSceneDist;
  matId = ::sceneMat;
  envColor = ::envColor;

  nMaterials = 2;
  materials = (Renderer::Material *) malloc(sizeof(Renderer::Material) * nMaterials);
  // Big object
  {
    Renderer::Material mat;
    mat.ambientColor = float3(0.3f, 0.3f, 0.3f);
    mat.diffuseColor = float3(0.8f, 0.9f, 0.8f);
    mat.specularColor = float3(1.0f, 1.0f, 1.0f);
    mat.shininess = 1000;
    mat.reflect = float3(0.7f);
    mat.refract = float3(0.0f);
    mat.refractionIndex = 1.5f;
    materials[0] = mat;
  }
  // Small object
  {
    Renderer::Material mat;
    mat.ambientColor = float3(0.01f, 0.01f, 0.05f);
    mat.diffuseColor = float3(0.0f, 0.0f, 0.0);
    mat.specularColor = float3(1.0f, 1.0f, 1.0f);
    mat.shininess = 1000;
    mat.reflect = float3(1.0, 1.0f, 1.0f);
    mat.refract = float3(0.7f, 0.7f, 0.7f);
    mat.refractionIndex = 1.1f;
    materials[1] = mat;
  }

  nLights = 3;
  lights = (Renderer::Light *) malloc(sizeof(Renderer::Light) * nLights);
  lights[0] = Renderer::Light(float3(10.0f, 10.0f, 10.0f),
                              float3(0.8f, 0.2f, 0.2f),
                              0.5f, 0.0f, 0.005f);
  lights[1] = Renderer::Light(float3(-3.0f, 1.0f, 4.0f),
                              float3(0.1f, 0.8f, 0.1f),
                              0.5f, 0.0f, 0.05f);
  lights[2] = Renderer::Light(float3(-1.0f, -5.0f, 5.0f),
                              float3(0.1f, 0.1f, 0.8f),
                              0.5f, 0.0f, 0.01f);
}
