// Terrain rendering

static const float3 planeNormal(0.0f, 0.0f, 1.0f);

static float sceneDist(float3 p)
{
  float x = p.x();
  float y = p.y();

  float r1 = vlength(float3(x-3, y-3, 0.0f)).scalar();
  float r2 = vlength(float3(x+3, y-3, 0.0f)).scalar();
  float offset = 0.3*exp(sin(3.0f*r1)*sin(3.0f*r2));

  return vdot(p, planeNormal).scalar() + offset;
}

static float innerSceneDist(float3 p)
{
  return -sceneDist(p);
}

static int sceneMat(float3 p)
{
  return 0;
}

static float3 envColor(float3 p)
{
  return vabs(vdot(p, float3(0.0f, 0.0f, 1.0f)) * vcross(p, float3(1.0f, 1.0f, 1.0f)));
}


MyScene::MyScene()
{
  sceneDist = ::sceneDist;
  innerSceneDist = ::innerSceneDist;
  matId = ::sceneMat;
  envColor = ::envColor;

  // Plane
  {
    Renderer::Material mat;
    mat.ambientColor = float3(0.3f, 0.3f, 0.3f);
    mat.diffuseColor = float3(1.0f, 1.0f, 1.0f);
    mat.specularColor = float3(1.0f, 1.0f, 1.0f);
    mat.shininess = 1000;
    mat.reflect = 0.0f;
    mat.refract = 0.0f;
    materials.push_back(mat);
  }

  lights.push_back(
        Renderer::Light(float3(10.0f, 10.0f, 10.0f),
                        float3(0.8f, 0.8f, 0.8f),
                        0.1f, 0.0f, 0.05f));
  lights.push_back(
        Renderer::Light(float3(-10.0f, 0.0f, 3.0f),
                        float3(0.8f, 0.8f, 0.8f),
                        0.8f, 0.0f, 0.05f));
}
